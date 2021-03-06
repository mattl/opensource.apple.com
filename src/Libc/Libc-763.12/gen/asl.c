/*
 * Copyright (c) 2004-2010 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <crt_externs.h>
#include <asl.h>
#include <regex.h>
#include <notify.h>
#include <mach/mach.h>
#include <mach/std_types.h>
#include <mach/mig.h>
#include <mach/mach_types.h>
#include <sys/types.h>
#include <servers/bootstrap.h>
#include <pthread.h>
#include <dispatch/dispatch.h>
#include <asl_ipc.h>
#include "asl_core.h"
#include "asl_msg.h"
#include "asl_store.h"
#include "asl_private.h"

#define streq(A, B) (strcmp(A, B) == 0)
#define strcaseeq(A, B) (strcasecmp(A, B) == 0)

#define forever for(;;)

#define FETCH_BATCH	256

/* forward */
time_t asl_parse_time(const char *);
const char *asl_syslog_faciliy_num_to_name(int n);
__private_extern__ int _asl_send_message(aslclient ac, asl_msg_t *msg, int slevel, const char *aux_message);
__private_extern__ asl_client_t *_asl_open_default();

/* private asl_msg SPI */
__private_extern__ uint32_t _asl_msg_string_length_aux(asl_msg_t *msg, asl_msg_aux_t *aux);
__private_extern__ uint32_t _asl_msg_to_string_buffer_aux(asl_msg_t *msg, asl_msg_aux_t *aux, char *buf, uint32_t bufsize);

/* private asl_file SPI */
__private_extern__ uint32_t asl_file_open_write_fd(int fd, asl_file_t **s);

/* notify SPI */
uint32_t notify_register_plain(const char *name, int *out_token);

/* fork handling in syslog.c */
extern void _syslog_fork_child();

typedef struct
{
	int fd;
	asl_msg_t *msg;
	dispatch_semaphore_t sem;
} asl_aux_context_t;

typedef struct
{
	int notify_count;
	int rc_change_token;
	int notify_token;
	int master_token;
	uint64_t proc_filter;
	uint64_t master_filter;
	dispatch_once_t port_lookup_once;
	mach_port_t server_port;
	char *sender;
	pthread_mutex_t lock;
	int aux_count;
	asl_aux_context_t **aux_ctx;
	asl_client_t *asl;
} _asl_global_t;


#ifndef BUILDING_VARIANT
__private_extern__ _asl_global_t _asl_global = {0, -1, -1, -1, 0LL, 0LL, 0, MACH_PORT_NULL, NULL, PTHREAD_MUTEX_INITIALIZER, 0, NULL, NULL};

#define ASL_SERVICE_NAME "com.apple.system.logger"

/*
 * Called from the child process inside fork() to clean up
 * inherited state from the parent process.
 *
 * NB. A lock isn't required, since we're single threaded in this call.
 */
__private_extern__ void
_asl_fork_child()
{
	_asl_global.notify_count = 0;
	_asl_global.rc_change_token = -1;
	_asl_global.master_token = -1;
	_asl_global.notify_token = -1;

	_asl_global.port_lookup_once = 0;
	_asl_global.server_port = MACH_PORT_NULL;
}

/*
 * asl_remote_notify_name: returns the notification key for remote-control filter
 * changes for this process.
 */
char *
asl_remote_notify_name()
{
	pid_t pid = getpid();
	uid_t euid = geteuid();
	char *str = NULL;

	if (euid == 0) asprintf(&str, "%s.%d", NOTIFY_PREFIX_SYSTEM, pid);
	else asprintf(&str, "user.uid.%d.syslog.%d", euid, pid);

	return str;
}

static int
_asl_notify_open(int do_lock)
{
	char *notify_name;
	uint32_t status;

	if (do_lock != 0) pthread_mutex_lock(&_asl_global.lock);

	_asl_global.notify_count++;

	if (_asl_global.notify_token != -1)
	{
		if (do_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
		return 0;
	}

	if (_asl_global.rc_change_token == -1)
	{
		status = notify_register_check(NOTIFY_RC, &_asl_global.rc_change_token);
		if (status != NOTIFY_STATUS_OK) _asl_global.rc_change_token = -1;
	}

	if (_asl_global.master_token == -1)
	{
		status = notify_register_plain(NOTIFY_SYSTEM_MASTER, &_asl_global.master_token);
		if (status != NOTIFY_STATUS_OK) _asl_global.master_token = -1;
	}

	notify_name = asl_remote_notify_name();
	if (notify_name != NULL)
	{
		status = notify_register_plain(notify_name, &_asl_global.notify_token);
		free(notify_name);
		if (status != NOTIFY_STATUS_OK) _asl_global.notify_token = -1;
	}

	if (do_lock != 0) pthread_mutex_unlock(&_asl_global.lock);

	if (_asl_global.notify_token == -1) return -1;
	return 0;
}

static void
_asl_notify_close()
{
	pthread_mutex_lock(&_asl_global.lock);

	if (_asl_global.notify_count > 0) _asl_global.notify_count--;

	if (_asl_global.notify_count > 0)
	{
		pthread_mutex_unlock(&_asl_global.lock);
		return;
	}

	if (_asl_global.rc_change_token >= 0) notify_cancel(_asl_global.rc_change_token);
	_asl_global.rc_change_token = -1;

	if (_asl_global.master_token >= 0) notify_cancel(_asl_global.master_token);
	_asl_global.master_token = -1;

	if (_asl_global.notify_token >= 0) notify_cancel(_asl_global.notify_token);
	_asl_global.notify_token = -1;

	pthread_mutex_unlock(&_asl_global.lock);
}

static void
_asl_global_init()
{
	dispatch_once(&_asl_global.port_lookup_once, ^{
		char *str = getenv("ASL_DISABLE");
		if ((str == NULL) || strcmp(str, "1"))
		{
			bootstrap_look_up(bootstrap_port, ASL_SERVICE_NAME, &_asl_global.server_port);
		}
	});
}

aslclient
asl_open(const char *ident, const char *facility, uint32_t opts)
{
	char *name, *x;
	asl_client_t *asl;

	asl = (asl_client_t *)calloc(1, sizeof(asl_client_t));
	if (asl == NULL)
	{
		errno = ENOMEM;
		return NULL;
	}

	asl->options = opts;

	asl->sock = -1;

	_asl_global_init();

	asl->pid = getpid();
	asl->uid = getuid();
	asl->gid = getgid();

	asl->filter = ASL_FILTER_MASK_UPTO(ASL_LEVEL_NOTICE);

	if (ident != NULL)
	{
		asl->name = strdup(ident);
		if (asl->name == NULL)
		{
			if (asl->sock >= 0) close(asl->sock);
			free(asl);
			return NULL;
		}
	}
	else
	{
		name = *(*_NSGetArgv());
		if (name != NULL)
		{
			x = strrchr(name, '/');
			if (x != NULL) x++;
			else x = name;
			asl->name = strdup(x);
			if (asl->name == NULL)
			{
				if (asl->sock >= 0) close(asl->sock);
				free(asl);
				return NULL;
			}
		}
	}

	asl->facility = NULL;
	if (facility != NULL) asl->facility = strdup(facility);
	else asl->facility = strdup(asl_syslog_faciliy_num_to_name(LOG_USER));
	if (asl->facility == NULL)
	{
		if (asl->sock >= 0) close(asl->sock);
		if (asl->name != NULL) free(asl->name);
		free(asl);
		return NULL;
	}

	if (!(asl->options & ASL_OPT_NO_REMOTE)) _asl_notify_open(1);

	if (asl->options & ASL_OPT_STDERR) asl_add_output((aslclient)asl, fileno(stderr), ASL_MSG_FMT_STD, ASL_TIME_FMT_LCL, ASL_ENCODE_SAFE);

	return (aslclient)asl;
}

aslclient
asl_open_from_file(int fd, const char *ident, const char *facility)
{
	char *name, *x;
	asl_client_t *asl;
	uint32_t status;

	asl = (asl_client_t *)calloc(1, sizeof(asl_client_t));
	if (asl == NULL)
	{
		errno = ENOMEM;
		return NULL;
	}

	asl->options = ASL_OPT_NO_REMOTE;
	asl->sock = -1;

	asl->pid = getpid();
	asl->uid = getuid();
	asl->gid = getgid();

	asl->filter = ASL_FILTER_MASK_UPTO(ASL_LEVEL_DEBUG);

	if (ident != NULL)
	{
		asl->name = strdup(ident);
		if (asl->name == NULL)
		{
			free(asl);
			return NULL;
		}
	}
	else
	{
		name = *(*_NSGetArgv());
		if (name != NULL)
		{
			x = strrchr(name, '/');
			if (x != NULL) x++;
			else x = name;
			asl->name = strdup(x);
			if (asl->name == NULL)
			{
				free(asl);
				return NULL;
			}
		}
	}

	asl->facility = NULL;
	if (facility != NULL) asl->facility = strdup(facility);
	else asl->facility = strdup(asl_syslog_faciliy_num_to_name(LOG_USER));
	if (asl->facility == NULL)
	{
		if (asl->name != NULL) free(asl->name);
		free(asl);
		return NULL;
	}

	status = asl_file_open_write_fd(fd, &(asl->aslfile));
	if (status != ASL_STATUS_OK)
	{
		if (asl->name != NULL) free(asl->name);
		if (asl->facility != NULL) free(asl->facility);
		free(asl);
		return NULL;
	}

	asl->aslfileid = 1;

	return (aslclient)asl;
}

void
asl_close(aslclient ac)
{
	asl_client_t *asl;
	uint32_t i;

	asl = (asl_client_t *)ac;
	if (asl == NULL) return;

	free(asl->name);
	free(asl->facility);

	if (asl->sock >= 0) close(asl->sock);
	if (!(asl->options & ASL_OPT_NO_REMOTE)) _asl_notify_close();
	if (asl->fd_list != NULL) free(asl->fd_list);

	if (asl->fd_mfmt != NULL)
	{
		for (i = 0; i < asl->fd_count; i++) if (asl->fd_mfmt[i] != NULL) free(asl->fd_mfmt[i]);
		free(asl->fd_mfmt);
	}

	if (asl->fd_tfmt != NULL)
	{
		for (i = 0; i < asl->fd_count; i++) if (asl->fd_tfmt[i] != NULL) free(asl->fd_tfmt[i]);
		free(asl->fd_tfmt);
	}

	if (asl->fd_encoding != NULL) free(asl->fd_encoding);

	memset(asl, 0, sizeof(asl_client_t));
	free(asl);
}

__private_extern__ asl_client_t *
_asl_open_default()
{
	static dispatch_once_t once;

	dispatch_once(&once, ^{
		/*
		 * Do a sleight-of-hand with ASL_OPT_NO_REMOTE to avoid a deadlock
		 * since asl_open(xxx, yyy, 0) calls _asl_notify_open(1)
		 * which locks _asl_global.lock.
		 */
		_asl_global.asl = asl_open(NULL, NULL, ASL_OPT_NO_REMOTE);

		/* Reset options to clear ASL_OPT_NO_REMOTE bit */
		if (_asl_global.asl != NULL) _asl_global.asl->options = 0;

		/* Now call _asl_notify_open(0) to finish the work */
		_asl_notify_open(0);
	});

	return _asl_global.asl;
}

/*
 * asl_add_file: write log messages to the given file descriptor
 * Log messages will be written to this file as well as to the server.
 */
int
asl_add_output(aslclient ac, int fd, const char *mfmt, const char *tfmt, uint32_t text_encoding)
{
	uint32_t i;
	int use_global_lock;
	asl_client_t *asl;

	use_global_lock = 0;
	asl = (asl_client_t *)ac;
	if (asl == NULL)
	{
		asl = _asl_open_default();
		if (asl == NULL) return -1;
		pthread_mutex_lock(&_asl_global.lock);
		use_global_lock = 1;
	}

	for (i = 0; i < asl->fd_count; i++) 
	{
		if (asl->fd_list[i] == fd)
		{
			/* update message format, time format, and text encoding */
			if (asl->fd_mfmt[i] != NULL) free(asl->fd_mfmt[i]);
			asl->fd_mfmt[i] = NULL;
			if (mfmt != NULL) asl->fd_mfmt[i] = strdup(mfmt);

			if (asl->fd_tfmt[i] != NULL) free(asl->fd_tfmt[i]);
			asl->fd_tfmt[i] = NULL;
			if (tfmt != NULL) asl->fd_tfmt[i] = strdup(tfmt);

			asl->fd_encoding[i] = text_encoding;

			if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
			return 0;
		}
	}

	if (asl->fd_count == 0)
	{
		asl->fd_list = (int *)calloc(1, sizeof(int));
		asl->fd_mfmt = (char **)calloc(1, sizeof(char *));
		asl->fd_tfmt = (char **)calloc(1, sizeof(char *));
		asl->fd_encoding = (uint32_t *)calloc(1, sizeof(int));
	}
	else
	{
		asl->fd_list = (int *)reallocf(asl->fd_list, (1 + asl->fd_count) * sizeof(int));
		asl->fd_mfmt = (char **)reallocf(asl->fd_mfmt, (1 + asl->fd_count) * sizeof(char *));
		asl->fd_tfmt = (char **)reallocf(asl->fd_tfmt, (1 + asl->fd_count) * sizeof(char *));
		asl->fd_encoding = (uint32_t *)reallocf(asl->fd_encoding, (1 + asl->fd_count) * sizeof(uint32_t));
	}

	if ((asl->fd_list == NULL) || (asl->fd_mfmt == NULL) || (asl->fd_tfmt == NULL) || (asl->fd_encoding == NULL))
	{
		if (asl->fd_list != NULL) free(asl->fd_list);
		if (asl->fd_mfmt != NULL) free(asl->fd_mfmt);
		if (asl->fd_tfmt != NULL) free(asl->fd_tfmt);
		if (asl->fd_encoding != NULL) free(asl->fd_encoding);

		if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
		return -1;
	}

	asl->fd_list[asl->fd_count] = fd;
	if (mfmt != NULL) asl->fd_mfmt[asl->fd_count] = strdup(mfmt);
	if (tfmt != NULL) asl->fd_tfmt[asl->fd_count] = strdup(tfmt);
	asl->fd_encoding[asl->fd_count] = text_encoding;

	asl->fd_count++;

	if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
	return 0;
}

int
asl_add_log_file(aslclient ac, int fd)
{
	return asl_add_output(ac, fd, ASL_MSG_FMT_STD, ASL_TIME_FMT_LCL, ASL_ENCODE_SAFE);
}

/*
 * asl_remove_output: stop writing log messages to the given file descriptor
 */
int
asl_remove_output(aslclient ac, int fd)
{
	uint32_t i;
	int x, use_global_lock;
	asl_client_t *asl;

	use_global_lock = 0;
	asl = (asl_client_t *)ac;
	if (asl == NULL)
	{
		asl = _asl_open_default();
		if (asl == NULL) return -1;
		pthread_mutex_lock(&_asl_global.lock);
		use_global_lock = 1;
	}

	if (asl->fd_count == 0)
	{
		if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
		return 0;
	}

	x = -1;
	for (i = 0; i < asl->fd_count; i++) 
	{
		if (asl->fd_list[i] == fd)
		{
			x = i;
			break;
		}
	}

	if (x == -1)
	{
		if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
		return 0;
	}

	if (asl->fd_mfmt[x] != NULL) free(asl->fd_mfmt[x]);
	if (asl->fd_tfmt[x] != NULL) free(asl->fd_tfmt[x]);

	for (i = x + 1; i < asl->fd_count; i++, x++)
	{
		asl->fd_list[x] = asl->fd_list[i];
		asl->fd_mfmt[x] = asl->fd_mfmt[i];
		asl->fd_tfmt[x] = asl->fd_tfmt[i];
		asl->fd_encoding[x] = asl->fd_encoding[i];
	}

	asl->fd_count--;

	if (asl->fd_count == 0)
	{
		free(asl->fd_list);
		asl->fd_list = NULL;

		free(asl->fd_mfmt);
		asl->fd_mfmt = NULL;

		free(asl->fd_tfmt);
		asl->fd_tfmt = NULL;

		free(asl->fd_encoding);
		asl->fd_encoding = NULL;
	}
	else
	{
		asl->fd_list = (int *)reallocf(asl->fd_list, asl->fd_count * sizeof(int));
		asl->fd_mfmt = (char **)reallocf(asl->fd_mfmt, asl->fd_count * sizeof(char *));
		asl->fd_tfmt = (char **)reallocf(asl->fd_tfmt, asl->fd_count * sizeof(char *));
		asl->fd_encoding = (uint32_t *)reallocf(asl->fd_encoding, asl->fd_count * sizeof(uint32_t));

		if ((asl->fd_list == NULL) || (asl->fd_mfmt == NULL) || (asl->fd_tfmt == NULL) || (asl->fd_encoding == NULL))
		{
			if (asl->fd_list != NULL)
			{
				free(asl->fd_list);
				asl->fd_list = NULL;
			}

			if (asl->fd_mfmt != NULL)
			{
				for (i = 0; i < asl->fd_count; i++) if (asl->fd_mfmt[i] != NULL) free(asl->fd_mfmt[i]);
				free(asl->fd_mfmt);
				asl->fd_mfmt = NULL;
			}

			if (asl->fd_tfmt != NULL)
			{
				for (i = 0; i < asl->fd_count; i++) if (asl->fd_tfmt[i] != NULL) free(asl->fd_tfmt[i]);
				free(asl->fd_tfmt);
				asl->fd_tfmt = NULL;
			}

			if (asl->fd_encoding != NULL)
			{
				free(asl->fd_encoding);
				asl->fd_encoding = NULL;
			}

			asl->fd_count = 0;
			if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
			return -1;
		}
	}

	if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
	return 0;
}

int
asl_remove_log_file(aslclient ac, int fd)
{
	return asl_remove_output(ac, fd);
}

int
asl_set_filter(aslclient ac, int f)
{
	int last, use_global_lock;
	asl_client_t *asl;

	use_global_lock = 0;
	asl = (asl_client_t *)ac;
	if (asl == NULL)
	{
		asl = _asl_open_default();
		if (asl == NULL) return -1;
		pthread_mutex_lock(&_asl_global.lock);
		use_global_lock = 1;
	}

	last = asl->filter;
	asl->filter = f;

	if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
	return last;
}

#endif /* BUILDING_VARIANT */

/*
 * asl_vlog: Similar to asl_log, but taking a va_list instead of a list of
 * arguments.
 * msg:  an aslmsg
 * level: the log level of the associated message
 * format: A formating string followed by a list of arguments, like vprintf()
 * returns 0 for success, non-zero for failure
 */
int
asl_vlog(aslclient ac, aslmsg msg, int level, const char *format, va_list ap)
{
	int saved_errno = errno;
	int status;
	char *str, *fmt, estr[NL_TEXTMAX];
	uint32_t i, len, elen, expand;
	asl_client_t *asl;

	asl = (asl_client_t *)ac;
	if (asl == NULL)
	{
		/*
		 * Initialize _asl_global so that asl_new will have global data.
		 * Not strictly necessary, but helps performance.
		 */
		asl = _asl_open_default();
		if (asl == NULL) return -1;
	}

	if (format == NULL) return -1;

	/* insert strerror for %m */
	len = 0;
	elen = 0;

	expand = 0;
	for (i = 0; format[i] != '\0'; i++)
	{
		if (format[i] == '%')
		{
			if (format[i+1] == '\0') len++;
			else if (format[i+1] == 'm')
			{
				expand = 1;
				strerror_r(saved_errno, estr, sizeof(estr));
				elen = strlen(estr);
				len += elen;
				i++;
			}
			else
			{
				len += 2;
				i++;
			}
		}
		else len++;
	}

	fmt = (char *)format;

	if (expand != 0)
	{
		fmt = malloc(len + 1);
		if (fmt == NULL)
		{
			if (estr != NULL) free(estr);
			return -1;
		}

		len = 0;

		for (i = 0; format[i] != '\0'; i++)
		{
			if (format[i] == '%')
			{
				if (format[i+1] == '\0')
				{
				}
				else if ((format[i+1] == 'm') && (elen != 0))
				{
					memcpy(fmt+len, estr, elen);
					len += elen;
					i++;
				}
				else
				{
					fmt[len++] = format[i++];
					fmt[len++] = format[i];
				}
			}
			else fmt[len++] = format[i];
		}

		fmt[len] = '\0';
	}

	vasprintf(&str, fmt, ap);
	if (expand != 0) free(fmt);

	if (str == NULL) return -1;

	status = _asl_send_message(ac, (asl_msg_t *)msg, level, str);
	free(str);

	return status;
}

/*
 * asl_log: log a message with a particular log level 
 * msg:  an aslmsg
 * level: the log level
 * format: A formating string followed by a list of arguments, like printf()
 * returns 0 for success, non-zero for failure
 */
int
asl_log(aslclient ac, aslmsg a, int level, const char *format, ...)
{
	va_list ap;
	int status;

	if (format == NULL) return -1;

	va_start(ap, format);
	status = asl_vlog(ac, a, level, format, ap);
	va_end(ap);

	return status;
}

#ifndef BUILDING_VARIANT

static asl_msg_t *
_asl_merge_msg_aux0(asl_msg_t *msg, asl_msg_aux_0_t aux0)
{
	const char *key, *val;
	asl_msg_t *out;
	uint32_t x;
	int klevel, ktime, knano, khost, ksender, kfac, kpid, kuid, kgid, kmsg, kaux, kuti, kurl;

	out = asl_msg_new(ASL_TYPE_MSG);
	if (out == NULL) return NULL;

	klevel = 0;
	ktime = 0;
	knano = 0;
	khost = 0;
	ksender= 0;
	kfac = 0;
	kpid = 0;
	kuid = 0;
	kgid = 0;
	kmsg = 0;
	kaux = 0;
	kuti = 0;
	kurl = 0;

	key = NULL;
	val = NULL;

	for (x = asl_msg_fetch(msg, 0, &key, &val, NULL); x != IndexNull; x = asl_msg_fetch(msg, x, &key, &val, NULL))
	{
		if (streq(key, ASL_KEY_LEVEL))
		{
			klevel = 1;
			if (aux0.level != NULL) asl_msg_set_key_val(out, key, aux0.level);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_TIME))
		{
			ktime = 1;
			if (aux0.time != NULL) asl_msg_set_key_val(out, key, aux0.time);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_TIME_NSEC))
		{
			knano = 1;
			if (aux0.nano != NULL) asl_msg_set_key_val(out, key, aux0.nano);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_HOST))
		{
			khost = 1;
			if (aux0.host != NULL) asl_msg_set_key_val(out, key, aux0.host);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_SENDER))
		{
			ksender = 1;
			if (aux0.sender != NULL) asl_msg_set_key_val(out, key, aux0.sender);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_FACILITY))
		{
			kfac = 1;
			if (aux0.facility != NULL) asl_msg_set_key_val(out, key, aux0.facility);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_PID))
		{
			kpid = 1;
			if (aux0.pid != NULL) asl_msg_set_key_val(out, key, aux0.pid);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_UID))
		{
			kuid = 1;
			if (aux0.uid != NULL) asl_msg_set_key_val(out, key, aux0.uid);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_GID))
		{
			kgid = 1;
			if (aux0.gid != NULL) asl_msg_set_key_val(out, key, aux0.gid);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_MSG))
		{
			kmsg = 1;
			if (aux0.message != NULL) asl_msg_set_key_val(out, key, aux0.message);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_AUX_TITLE))
		{
			kaux = 1;
			if (aux0.auxtitle != NULL) asl_msg_set_key_val(out, key, aux0.auxtitle);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_AUX_UTI))
		{
			kuti = 1;
			if (aux0.auxuti != NULL) asl_msg_set_key_val(out, key, aux0.auxuti);
			else asl_msg_set_key_val(out, key, val);
		}
		else if (streq(key, ASL_KEY_AUX_URL))
		{
			kurl = 1;
			if (aux0.auxurl != NULL) asl_msg_set_key_val(out, key, aux0.auxurl);
			else asl_msg_set_key_val(out, key, val);
		}
		else
		{
			asl_msg_set_key_val(out, key, val);
		}
	}

	if ((klevel == 0) && (aux0.level != NULL)) asl_msg_set_key_val(out, ASL_KEY_LEVEL, aux0.level);
	if ((ktime == 0) && (aux0.time != NULL)) asl_msg_set_key_val(out, ASL_KEY_TIME, aux0.time);
	if ((knano == 0) && (aux0.nano != NULL)) asl_msg_set_key_val(out, ASL_KEY_TIME_NSEC, aux0.nano);
	if ((khost == 0) && (aux0.host != NULL)) asl_msg_set_key_val(out, ASL_KEY_HOST, aux0.host);
	if ((ksender == 0) && (aux0.sender != NULL)) asl_msg_set_key_val(out, ASL_KEY_SENDER, aux0.sender);
	if ((kfac == 0) && (aux0.facility != NULL)) asl_msg_set_key_val(out, ASL_KEY_FACILITY, aux0.facility);
	if ((kpid == 0) && (aux0.pid != NULL)) asl_msg_set_key_val(out, ASL_KEY_PID, aux0.pid);
	if ((kuid == 0) && (aux0.uid != NULL)) asl_msg_set_key_val(out, ASL_KEY_UID, aux0.uid);
	if ((kgid == 0) && (aux0.gid != NULL)) asl_msg_set_key_val(out, ASL_KEY_GID, aux0.gid);
	if ((kmsg == 0) && (aux0.message != NULL)) asl_msg_set_key_val(out, ASL_KEY_MSG, aux0.message);
	if ((kaux == 0) && (aux0.auxtitle != NULL)) asl_msg_set_key_val(out, ASL_KEY_AUX_TITLE, aux0.auxtitle);
	if ((kuti == 0) && (aux0.auxuti != NULL)) asl_msg_set_key_val(out, ASL_KEY_AUX_UTI, aux0.auxuti);
	if ((kurl == 0) && (aux0.auxurl != NULL)) asl_msg_set_key_val(out, ASL_KEY_AUX_URL, aux0.auxurl);

	return out;
}

/*
 * asl_get_filter: gets the values for the local, master, and remote filters, 
 * and indicates which one is active.
 */
int
asl_get_filter(aslclient ac, int *local, int *master, int *remote, int *active)
{
	asl_client_t *asl, *asl_default;
	int l, m, r, x;
	int status, check;
	uint64_t v64;

	l = 0;
	m = 0;
	r = 0;
	x = 0;

	asl_default = _asl_open_default();

	asl = (asl_client_t *)ac;
	if (asl == NULL) asl = asl_default;
	if (asl != NULL) l = asl->filter & 0xff;

	if ((asl_default != NULL) && (!(asl_default->options & ASL_OPT_NO_REMOTE)))
	{
		pthread_mutex_lock(&_asl_global.lock);

		if (_asl_global.rc_change_token >= 0)
		{
			/* initialize or re-check process-specific and master filters  */
			check = 0;
			status = notify_check(_asl_global.rc_change_token, &check);
			if ((status == NOTIFY_STATUS_OK) && (check != 0))
			{
				if (_asl_global.master_token >= 0)
				{
					v64 = 0;
					status = notify_get_state(_asl_global.master_token, &v64);
					if (status == NOTIFY_STATUS_OK) _asl_global.master_filter = v64;
				}

				if (_asl_global.notify_token >= 0)
				{
					v64 = 0;
					status = notify_get_state(_asl_global.notify_token, &v64);
					if (status == NOTIFY_STATUS_OK) _asl_global.proc_filter = v64;
				}
			}
		}

		m = _asl_global.master_filter;
		if (m != 0) x = 1;

		r = _asl_global.proc_filter;
		if (r != 0) x = 2;

		pthread_mutex_unlock(&_asl_global.lock);
	}

	if (local != NULL) *local = l;
	if (master != NULL) *master = m;
	if (remote != NULL) *remote = r;
	if (active != NULL) *active = x;

	return 0;
}

/*
 * asl_send: send a message 
 * This routine may be used instead of asl_log() or asl_vlog() if asl_set() 
 * has been used to set all of a message's attributes.
 * msg:  an aslmsg
 * returns 0 for success, non-zero for failure
 */
__private_extern__ int
_asl_send_message(aslclient ac, asl_msg_t *msg, int slevel, const char *aux_message)
{
	vm_address_t out;
	uint32_t i, len, outlen, level, lmask, outstatus, filter, fd_write;
	uint64_t v64;
	const char *val;
	char *name, *x, *str;
	time_t tick;
	struct timeval tval;
	int status, check, tunnel;
	asl_client_t *asl;
	int use_global_lock;
	kern_return_t kstatus;
	char aux_level[64], aux_time[64], aux_nano[64], aux_pid[64], aux_uid[64], aux_gid[64];
	char *aux_option, aux_host[_POSIX_HOST_NAME_MAX];
	asl_msg_aux_t aux;
	asl_msg_aux_0_t aux0;

	use_global_lock = 0;
	asl = (asl_client_t *)ac;
	if (asl == NULL)
	{
		asl = _asl_open_default();
		if (asl == NULL) return -1;
		use_global_lock = 1;
	}

	if (asl->aslfile != NULL)
	{
		use_global_lock = 1;
	}

	level = ASL_LEVEL_DEBUG;
	if (slevel >= 0) level = slevel;

	val = asl_get((aslmsg)msg, ASL_KEY_LEVEL);
	if (val != NULL)
	{
		check = atoi(val);
		if (check < ASL_LEVEL_EMERG) check = ASL_LEVEL_EMERG;
		else if (check > ASL_LEVEL_DEBUG) check = ASL_LEVEL_DEBUG;
		level = check;
	}

	lmask = ASL_FILTER_MASK(level);

	if (!(asl->options & ASL_OPT_NO_REMOTE))
	{
		pthread_mutex_lock(&_asl_global.lock);

		if (_asl_global.rc_change_token >= 0)
		{
			/* initialize or re-check process-specific and master filters  */
			check = 0;
			status = notify_check(_asl_global.rc_change_token, &check);
			if ((status == NOTIFY_STATUS_OK) && (check != 0))
			{
				if (_asl_global.master_token >= 0)
				{
					v64 = 0;
					status = notify_get_state(_asl_global.master_token, &v64);
					if (status == NOTIFY_STATUS_OK) _asl_global.master_filter = v64;
				}

				if (_asl_global.notify_token >= 0)
				{
					v64 = 0;
					status = notify_get_state(_asl_global.notify_token, &v64);
					if (status == NOTIFY_STATUS_OK) _asl_global.proc_filter = v64;
				}
			}
		}

		pthread_mutex_unlock(&_asl_global.lock);
	}

	filter = asl->filter & 0xff;
	tunnel = (asl->filter & ASL_FILTER_MASK_TUNNEL) >> 8;

	/* master filter overrides local filter */
	if (_asl_global.master_filter != 0)
	{
		filter = _asl_global.master_filter;
		tunnel = 1;
	}

	/* process-specific filter overrides local and master */
	if (_asl_global.proc_filter != 0)
	{
		filter = _asl_global.proc_filter;
		tunnel = 1;
	}

	/* 
	 * Time, TimeNanoSec, Host, PID, UID, and GID values get set here.
	 * Also sets Sender & Facility (if unset) and "ASLOption store" if remote control is active.
	 */

	aux.type = ASL_MSG_TYPE_AUX_0;

	memset(&aux0, 0, sizeof(asl_msg_aux_0_t));
	aux.data.aux0 = &aux0;

	aux0.message = aux_message;

	snprintf(aux_level, sizeof(aux_level), "%u", level);
	aux0.level = aux_level;

	memset(&tval, 0, sizeof(struct timeval));

	status = gettimeofday(&tval, NULL);
	if (status == 0)
	{
		snprintf(aux_time, sizeof(aux_time), "%lu", tval.tv_sec);
		snprintf(aux_nano, sizeof(aux_nano), "%d", tval.tv_usec * 1000);
		aux0.time = aux_time;
		aux0.nano = aux_nano;
	}
	else
	{
		tick = time(NULL);
		snprintf(aux_time, sizeof(aux_time), "%lu", tick);
		aux0.time = aux_time;
	}

	memset(&aux_host, 0, _POSIX_HOST_NAME_MAX);
	if (gethostname(aux_host, _POSIX_HOST_NAME_MAX) == 0)
	{
		aux0.host = aux_host;
	}

	snprintf(aux_pid, sizeof(aux_pid), "%u", getpid());
	aux0.pid = aux_pid;

	snprintf(aux_uid, sizeof(aux_uid), "%d", getuid());
	aux0.uid = aux_uid;

	snprintf(aux_gid, sizeof(aux_uid), "%d", getgid());
	aux0.gid = aux_gid;

	/*
	 * Set Sender if needed
	 */
	status = asl_msg_lookup((asl_msg_t *)msg, ASL_KEY_SENDER, &val, NULL);
	if ((status != 0) || (val == NULL))
	{
		if ((ac != NULL) && (ac->name != NULL))
		{
			/* Use the Sender name from the client handle */
			aux0.sender = ac->name;
		}
		else
		{
			/* Get the value for ASL_KEY_SENDER from cache */
			if (_asl_global.sender == NULL)
			{
				name = *(*_NSGetArgv());
				if (name != NULL)
				{
					x = strrchr(name, '/');
					if (x != NULL) x++;
					else x = name;

					pthread_mutex_lock(&_asl_global.lock);
					if (_asl_global.sender == NULL) _asl_global.sender = strdup(x);
					pthread_mutex_unlock(&_asl_global.lock);
				}
			}

			if (_asl_global.sender != NULL) aux0.sender = _asl_global.sender;
			else aux0.sender = "Unknown";
		}
	}

	/*
	 * Set Facility
	 */
	status = asl_msg_lookup((asl_msg_t *)msg, ASL_KEY_FACILITY, &val, NULL);
	if ((status != 0) || (val == NULL))
	{
		if ((ac != NULL) && (ac->facility != NULL))
		{
			/* Use the Facility name from the client handle */
			aux0.facility = ac->facility;
		}
	}

	/* Set "ASLOption store" if remote control is active */

	aux_option = NULL;

	if (tunnel != 0)
	{
		val = asl_get((aslmsg)msg, ASL_KEY_OPTION);
		if (val == NULL)
		{
			aux0.option = ASL_OPT_STORE;
		}
		else
		{
			asprintf(&aux_option, "%s %s", ASL_OPT_STORE, val);
			aux0.option = aux_option;
		}
	}

	outstatus = -1;

	if (use_global_lock != 0) pthread_mutex_lock(&_asl_global.lock);

	/*
	 * If there is an aslfile this is a stand-alone file client.
	 * Just save to the file.
	 */
	if (asl->aslfile != NULL)
	{
		asl_msg_t *merged_msg;

		outstatus = ASL_STATUS_FAILED;

		merged_msg = _asl_merge_msg_aux0(msg, aux0);
		if (merged_msg != NULL)
		{
			outstatus = asl_file_save(asl->aslfile, (aslmsg)merged_msg, &(asl->aslfileid));
			asl->aslfileid++;
			asl_msg_release(merged_msg);
		}

		if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);
		return outstatus;
	}

	_asl_global_init();
	if ((_asl_global.server_port != MACH_PORT_NULL) && (filter != 0) && ((filter & lmask) != 0))
	{
		len = _asl_msg_string_length_aux(msg, &aux);

		if (len != 0)
		{
			/* send a mach message to syslogd */
			out = 0;
			outlen = len + 11;
			kstatus = vm_allocate(mach_task_self(), &out, outlen, TRUE);
			if (kstatus == KERN_SUCCESS)
			{
				memset((void *)out, 0, outlen);
				snprintf((char *)out, 12, "%10u ", len);

				status = _asl_msg_to_string_buffer_aux(msg, &aux, (char *)(out + 11), len);
				if (status == 0)
				{
					if (kstatus == KERN_SUCCESS) kstatus = _asl_server_message(_asl_global.server_port, (caddr_t)out, outlen);
					else vm_deallocate(mach_task_self(), out, outlen);

					if (kstatus == KERN_SUCCESS) outstatus = 0;
				}
				else
				{
					vm_deallocate(mach_task_self(), (vm_address_t)out, outlen);
				}
			}
		}
	}

	if (aux_option != NULL) free(aux_option);
	aux0.option = NULL;

	outstatus = 0;

	/* messages from syslog() get filtered on the way out to stderr */
	fd_write = 1;
	if ((asl->options & ASL_OPT_SYSLOG_LEGACY) && (filter != 0) && ((filter & lmask) == 0)) fd_write = 0;

	if ((fd_write != 0) && (asl->fd_count > 0))
	{
		asl_msg_t *merged_msg;

		/*
		 * It's easier to merge the aux fields with the msg into a new message
		 * than to have asl_format_messge deal with two inputs.  Since this is
		 * an infrequently used code path, we pay a small price to merge them
		 * here and save a lot of code complexity.
		 */

		merged_msg = _asl_merge_msg_aux0(msg, aux0);
		if (merged_msg != NULL)
		{
			/* write to file descriptors */

			for (i = 0; i < asl->fd_count; i++)
			{
				if (asl->fd_list[i] < 0) continue;

				len = 0;
				str = asl_format_message(merged_msg, asl->fd_mfmt[i], asl->fd_tfmt[i], asl->fd_encoding[i], &len);
				if (str == NULL) continue;

				status = write(asl->fd_list[i], str, len - 1);
				if (status < 0)
				{
					asl->fd_list[i] = -1;
					outstatus = -1;
				}

				free(str);
			}

			asl_msg_release(merged_msg);
		}
	}

	if (use_global_lock != 0) pthread_mutex_unlock(&_asl_global.lock);

	return outstatus;
}

int
asl_send(aslclient ac, aslmsg msg)
{
	return _asl_send_message(ac, (asl_msg_t *)msg, -1, NULL);
}

static int
_asl_aux_save_context(asl_aux_context_t *ctx)
{
	if (ctx == NULL) return -1;

	pthread_mutex_lock(&_asl_global.lock);

	_asl_global.aux_ctx = (asl_aux_context_t **)reallocf(_asl_global.aux_ctx, (_asl_global.aux_count + 1) * sizeof(asl_aux_context_t *));
	if (_asl_global.aux_ctx == NULL)
	{
		_asl_global.aux_count = 0;
		return -1;
	}

	_asl_global.aux_ctx[_asl_global.aux_count++] = ctx;

	pthread_mutex_unlock(&_asl_global.lock);

	return 0;
}

/*
 * Creates an auxiliary file that may be used to save arbitrary data.  The ASL message msg
 * will be saved at the time that the auxiliary file is created.  The message will include
 * any keys and values found in msg, and it will include the title and Uniform Type
 * Identifier specified.  Output parameter out_fd will contain the file descriptor of the
 * new auxiliary file.
 */
static int
_asl_auxiliary(aslmsg msg, const char *title, const char *uti, const char *url, int *out_fd)
{
	asl_msg_t *merged_msg;
	asl_msg_aux_t aux;
	asl_msg_aux_0_t aux0;
	fileport_t fileport;
	kern_return_t kstatus;
	uint32_t outlen, newurllen, len, where;
	int status, fd, fdpair[2];
	caddr_t out, newurl;
	dispatch_queue_t pipe_q;
	dispatch_io_t pipe_channel;
	dispatch_semaphore_t sem;

	aux.type = ASL_MSG_TYPE_AUX_0;

	memset(&aux0, 0, sizeof(asl_msg_aux_0_t));
	aux.data.aux0 = &aux0;

	aux0.auxtitle = title;
	if (uti == NULL) aux0.auxuti = "public.data";
	else aux0.auxuti = uti;
	aux0.auxurl = url;

	merged_msg = _asl_merge_msg_aux0((asl_msg_t *)msg, aux0);
	if (merged_msg == NULL) return -1;

	/* if (out_fd == NULL), this is from asl_log_auxiliary_location */
	if (out_fd == NULL)
	{
		status = _asl_send_message(NULL, merged_msg, -1, NULL);
		asl_msg_release(merged_msg);
		return status;
	}

	where = asl_store_location();

	if (where == ASL_STORE_LOCATION_MEMORY)
	{
		/* create a pipe */

		asl_aux_context_t *ctx = (asl_aux_context_t *)calloc(1, sizeof(asl_aux_context_t));
		if (ctx == NULL) return -1;

		status = pipe(fdpair);
		if (status < 0)
		{
			free(ctx);
			return -1;
		}

		/* give read end to dispatch_io_read */
		fd = fdpair[0];
		sem = dispatch_semaphore_create(0);
		ctx->sem = sem;
		ctx->fd = fdpair[1];

		status = _asl_aux_save_context(ctx);
		if (status != 0)
		{
			close(fdpair[0]);
			close(fdpair[1]);
			dispatch_release(sem);
			free(ctx);
			return -1;
		}

		pipe_q = dispatch_queue_create("PipeQ", NULL);
		pipe_channel = dispatch_io_create(DISPATCH_IO_STREAM, fd, pipe_q, ^(int err){
			close(fd);
		});

		*out_fd = fdpair[1];

		dispatch_io_set_low_water(pipe_channel, SIZE_MAX);

		dispatch_io_read(pipe_channel, 0, SIZE_MAX, pipe_q, ^(bool done, dispatch_data_t pipedata, int err){
			if (err == 0)
			{
				size_t len = dispatch_data_get_size(pipedata);
				if (len > 0)
				{
					const char *bytes = NULL;
					char *encoded;

					dispatch_data_t md = dispatch_data_create_map(pipedata, (const void **)&bytes, &len);
					encoded = asl_core_encode_buffer(bytes, len);
					asl_set((aslmsg)merged_msg, ASL_KEY_AUX_DATA, encoded);
					free(encoded);
					_asl_send_message(NULL, merged_msg, -1, NULL);
					asl_msg_release(merged_msg);
					dispatch_release(md);
				}
			}

			if (done)
			{
				dispatch_semaphore_signal(sem);
				dispatch_release(pipe_channel);
				dispatch_release(pipe_q);
			}
		});

		return 0;
	}

	_asl_global_init();
	if (_asl_global.server_port == MACH_PORT_NULL) return -1;

	len = _asl_msg_string_length_aux(merged_msg, NULL);
	if (len == 0) 
	{
		asl_msg_release(merged_msg);
		return -1;
	}

	outlen = len + 11;
	kstatus = vm_allocate(mach_task_self(), (vm_address_t *)&out, outlen, TRUE);
	if (kstatus != KERN_SUCCESS)
	{
		asl_msg_release(merged_msg);
		return -1;
	}

	memset(out, 0, outlen);
	snprintf((char *)out, 12, "%10u ", len);

	status = _asl_msg_to_string_buffer_aux(merged_msg, NULL, (char *)(out + 11), len);
	if (status != 0)
	{
		asl_msg_release(merged_msg);
		vm_deallocate(mach_task_self(), (vm_address_t)out, outlen);
		return -1;
	}

	status = 0;
	fileport = MACH_PORT_NULL;
	status = KERN_SUCCESS;

	kstatus = _asl_server_create_aux_link(_asl_global.server_port, out, outlen, &fileport, &newurl, &newurllen, &status);
	if (kstatus != KERN_SUCCESS)
	{
		asl_msg_release(merged_msg);
		return -1;
	}

	if (status != 0)
	{
		asl_msg_release(merged_msg);
		return status;
	}

	if (newurl != NULL)
	{
		asl_msg_set_key_val(merged_msg, ASL_KEY_AUX_URL, newurl);
		vm_deallocate(mach_task_self(), (vm_address_t)newurl, newurllen);
	}

	if (fileport == MACH_PORT_NULL)
	{
		asl_msg_release(merged_msg);
		return -1;
	}

	fd = fileport_makefd(fileport);
	mach_port_deallocate(mach_task_self(), fileport);
	if (fd < 0)
	{
		asl_msg_release(merged_msg);
		status = -1;
	}
	else
	{
		asl_aux_context_t *ctx = (asl_aux_context_t *)calloc(1, sizeof(asl_aux_context_t));
		if (ctx == NULL)
		{
			status = -1;
		}
		else
		{
			*out_fd = fd;

			ctx->fd = fd;
			ctx->msg = merged_msg;

			status = _asl_aux_save_context(ctx);
		}
	}

	return status;
}

int
asl_create_auxiliary_file(aslmsg msg, const char *title, const char *uti, int *out_fd)
{
	if (out_fd == NULL) return -1;

	return _asl_auxiliary(msg, title, uti, NULL, out_fd);
}

int
asl_log_auxiliary_location(aslmsg msg, const char *title, const char *uti, const char *url)
{
	return _asl_auxiliary(msg, title, uti, url, NULL);
}

/*
 * Close an auxiliary file.
 * Sends the cached auxiliary message to syslogd.
 */
int
asl_close_auxiliary_file(int fd)
{
	int i, j, status;
	asl_msg_t *aux_msg;
	dispatch_semaphore_t aux_sem;

	pthread_mutex_lock(&(_asl_global.lock));

	aux_msg = NULL;
	status = -1;

	for (i = 0; i < _asl_global.aux_count; i++)
	{
		if (_asl_global.aux_ctx[i]->fd == fd)
		{
			status = 0;

			aux_msg = _asl_global.aux_ctx[i]->msg;
			aux_sem = _asl_global.aux_ctx[i]->sem;

			free(_asl_global.aux_ctx[i]);

			for (j = i + 1; j < _asl_global.aux_count; i++, j++)
			{
				_asl_global.aux_ctx[i] = _asl_global.aux_ctx[j];
			}

			_asl_global.aux_count--;

			if (_asl_global.aux_count == 0)
			{
				free(_asl_global.aux_ctx);
				_asl_global.aux_ctx = NULL;
			}
			else
			{
				_asl_global.aux_ctx = (asl_aux_context_t **)reallocf(_asl_global.aux_ctx, _asl_global.aux_count * sizeof(asl_aux_context_t *));
				if (_asl_global.aux_ctx == NULL)
				{
					_asl_global.aux_count = 0;
					status = -1;
				}
			}

			break;
		}
	}

	pthread_mutex_unlock(&(_asl_global.lock));

	close(fd);

	if (aux_msg != NULL)
	{
		if (_asl_send_message(NULL, aux_msg, -1, NULL) != ASL_STATUS_OK) status = -1;
		asl_msg_release(aux_msg);
	}

	if (aux_sem != NULL)
	{
		dispatch_semaphore_wait(aux_sem, DISPATCH_TIME_FOREVER);
		dispatch_release(aux_sem);
	}

	return status;
}

/*
 * asl_search: Search for messages matching the criteria described
 * by the aslmsg.  The caller should set the attributes to match using
 * asl_set_query() or asl_set().  The operatoin ASL_QUERY_OP_EQUAL is
 * used for attributes set with asl_set().
 * a:  an aslmsg
 * returns: a set of messages that can be iterated over using aslresp_next(),
 * and the values can be retrieved using aslresp_get.
 */

/*
 * This routine searches the ASL datastore on disk (/var/log/asl).
 * It is called my asl_search if syslogd is not running or if syslogd
 * indicates that an in-memory store is not being used.
 */
static aslresponse
_asl_search_store(aslclient ac, aslmsg a)
{
	asl_search_result_t query, *out;
	asl_msg_t *q, *qlist[1];
	uint32_t status, op;
	uint64_t last_id, start_id;
	asl_store_t *store;
	const char *val;

	if (a == NULL) return NULL;

	q = (asl_msg_t *)a;

	/* check for "ASLMessageId >[=] n" and set start_id */
	start_id = 0;
	val = NULL;

	status = asl_msg_lookup(q, ASL_KEY_MSG_ID, &val, &op);
	if ((status == 0) && (val != NULL) && (op & ASL_QUERY_OP_GREATER))
	{
		if (op & ASL_QUERY_OP_EQUAL) start_id = atoll(val);
		else start_id = atoll(val) + 1;
	}

	store = NULL;
	status = asl_store_open_read(NULL, &store);
	if (status != 0) return NULL;
	if (store == NULL) return NULL;

	out = NULL;
	last_id = 0;

	qlist[0] = (asl_msg_t *)a;
	memset(&query, 0, sizeof(asl_search_result_t));
	query.count = 1;
	query.msg = qlist;

	status = asl_store_match(store, &query, &out, &last_id, start_id, 0, 1);
	asl_store_close(store);

	return out;
}

static uint32_t
_asl_search_concat_results(asl_search_result_t *batch, asl_search_result_t **out)
{
	uint32_t i, j;

	if (out == NULL) return ASL_STATUS_FAILED;

	/* nothing to do if batch is NULL or contains no messages */
	if (batch == NULL) return 0;
	if (batch->count == 0)
	{
		aslresponse_free(batch);
		return 0;
	}

	if (*out == NULL) *out = (asl_search_result_t *)calloc(1, sizeof(asl_search_result_t));
	if (*out == NULL)
	{
		aslresponse_free(batch);
		return ASL_STATUS_FAILED;
	}

	if ((*out)->count == 0)
	{
		(*out)->msg = (asl_msg_t **)calloc(batch->count, sizeof(asl_msg_t *));
	}
	else
	{
		(*out)->msg = (asl_msg_t **)reallocf((*out)->msg, ((*out)->count + batch->count) * sizeof(asl_msg_t *));
	}

	if ((*out)->msg == NULL)
	{
		aslresponse_free(batch);
		free(*out);
		*out = NULL;
		return ASL_STATUS_FAILED;
	}

	for (i = 0, j = (*out)->count; i < batch->count; i++, j++) (*out)->msg[j] = batch->msg[i];

	(*out)->count += batch->count;
	free(batch->msg);
	free(batch);
	return ASL_STATUS_OK;
}

static aslresponse
_asl_search_memory(aslclient ac, aslmsg a)
{
	asl_search_result_t *batch, *out;
	char *qstr, *str, *res;
	uint32_t len, reslen, status;
	uint64_t cmax, qmin;
	kern_return_t kstatus;
	security_token_t sec;
	caddr_t vmstr;

	if (a == NULL) return 0;

	_asl_global_init();
	if (_asl_global.server_port == MACH_PORT_NULL) return NULL;

	len = 0;
	qstr = asl_msg_to_string((asl_msg_t *)a, &len);

	str = NULL;
	if (qstr == NULL)
	{
		asprintf(&str, "0\n");
		len = 3;
	}
	else
	{
		asprintf(&str, "1\n%s\n", qstr);
		len += 3;
		free(qstr);
	}

	if (str == NULL) return NULL;

	/*
	 * Fetch a batch of results each time through the loop.
	 * Fetching small batches rebuces the load on syslogd.
	 */
	out = NULL;
	qmin = 0;
	cmax = 0;

	forever
	{
		res = NULL;
		reslen = 0;
		sec.val[0] = -1;
		sec.val[1] = -1;
		status = ASL_STATUS_OK;

		kstatus = vm_allocate(mach_task_self(), (vm_address_t *)&vmstr, len, TRUE);
		if (kstatus != KERN_SUCCESS) return NULL;

		memmove(vmstr, str, len);

		status = 0;
		kstatus = _asl_server_query(_asl_global.server_port, vmstr, len, qmin, FETCH_BATCH, 0, (caddr_t *)&res, &reslen, &cmax, (int *)&status, &sec);
		if (kstatus != KERN_SUCCESS) break;
		if (res == NULL) break;

		batch = asl_list_from_string(res);
		vm_deallocate(mach_task_self(), (vm_address_t)res, reslen);

		status = _asl_search_concat_results(batch, &out);
		if (status != ASL_STATUS_OK) break;
		if (out->count < FETCH_BATCH) break;

		if (cmax >= qmin) qmin = cmax + 1;
	}

	free(str);

	return out;
}

int
asl_store_location()
{
	kern_return_t kstatus;
	char *res;
	uint32_t reslen, status;
	uint64_t cmax;
	security_token_t sec;

	_asl_global_init();
	if (_asl_global.server_port == MACH_PORT_NULL) return ASL_STORE_LOCATION_FILE;

	res = NULL;
	reslen = 0;
	cmax = 0;
	sec.val[0] = -1;
	sec.val[1] = -1;
	status = ASL_STATUS_OK;

	kstatus = _asl_server_query(_asl_global.server_port, NULL, 0, 0, -1, 0, (caddr_t *)&res, &reslen, &cmax, (int *)&status, &sec);

	/* res should never be returned, but just to be certain we don't leak VM ... */
	if (res != NULL) vm_deallocate(mach_task_self(), (vm_address_t)res, reslen);

	if (kstatus != KERN_SUCCESS) return ASL_STORE_LOCATION_FILE;

	if (status == ASL_STATUS_OK) return ASL_STORE_LOCATION_MEMORY;
	return ASL_STORE_LOCATION_FILE;
}

aslresponse
asl_search(aslclient ac, aslmsg a)
{
	int where;
	asl_search_result_t *out;

	_asl_global_init();

	where = asl_store_location();
	if (where == ASL_STORE_LOCATION_FILE) out = _asl_search_store(ac, a);
	else out = _asl_search_memory(ac, a);

	return out;
}

int
asl_syslog_faciliy_name_to_num(const char *name)
{
	if (name == NULL) return -1;

	if (strcaseeq(name, "auth")) return LOG_AUTH;
	if (strcaseeq(name, "authpriv")) return LOG_AUTHPRIV;
	if (strcaseeq(name, "cron")) return LOG_CRON;
	if (strcaseeq(name, "daemon")) return LOG_DAEMON;
	if (strcaseeq(name, "ftp")) return LOG_FTP;
	if (strcaseeq(name, "install")) return LOG_INSTALL;
	if (strcaseeq(name, "kern")) return LOG_KERN;
	if (strcaseeq(name, "lpr")) return LOG_LPR;
	if (strcaseeq(name, "mail")) return LOG_MAIL;
	if (strcaseeq(name, "netinfo")) return LOG_NETINFO;
	if (strcaseeq(name, "remoteauth")) return LOG_REMOTEAUTH;
	if (strcaseeq(name, "news")) return LOG_NEWS;
	if (strcaseeq(name, "security")) return LOG_AUTH;
	if (strcaseeq(name, "syslog")) return LOG_SYSLOG;
	if (strcaseeq(name, "user")) return LOG_USER;
	if (strcaseeq(name, "uucp")) return LOG_UUCP;
	if (strcaseeq(name, "local0")) return LOG_LOCAL0;
	if (strcaseeq(name, "local1")) return LOG_LOCAL1;
	if (strcaseeq(name, "local2")) return LOG_LOCAL2;
	if (strcaseeq(name, "local3")) return LOG_LOCAL3;
	if (strcaseeq(name, "local4")) return LOG_LOCAL4;
	if (strcaseeq(name, "local5")) return LOG_LOCAL5;
	if (strcaseeq(name, "local6")) return LOG_LOCAL6;
	if (strcaseeq(name, "local7")) return LOG_LOCAL7;
	if (strcaseeq(name, "launchd")) return LOG_LAUNCHD;

	return -1;
}

const char *
asl_syslog_faciliy_num_to_name(int n)
{
	if (n < 0) return NULL;

	if (n == LOG_AUTH) return "auth";
	if (n == LOG_AUTHPRIV) return "authpriv";
	if (n == LOG_CRON) return "cron";
	if (n == LOG_DAEMON) return "daemon";
	if (n == LOG_FTP) return "ftp";
	if (n == LOG_INSTALL) return "install";
	if (n == LOG_KERN) return "kern";
	if (n == LOG_LPR) return "lpr";
	if (n == LOG_MAIL) return "mail";
	if (n == LOG_NETINFO) return "netinfo";
	if (n == LOG_REMOTEAUTH) return "remoteauth";
	if (n == LOG_NEWS) return "news";
	if (n == LOG_AUTH) return "security";
	if (n == LOG_SYSLOG) return "syslog";
	if (n == LOG_USER) return "user";
	if (n == LOG_UUCP) return "uucp";
	if (n == LOG_LOCAL0) return "local0";
	if (n == LOG_LOCAL1) return "local1";
	if (n == LOG_LOCAL2) return "local2";
	if (n == LOG_LOCAL3) return "local3";
	if (n == LOG_LOCAL4) return "local4";
	if (n == LOG_LOCAL5) return "local5";
	if (n == LOG_LOCAL6) return "local6";
	if (n == LOG_LOCAL7) return "local7";
	if (n == LOG_LAUNCHD) return "launchd";

	return NULL;
}

/*
 * utility for converting a time string into a time_t
 * we only deal with the following formats:
 * Canonical form YYYY.MM.DD hh:mm:ss UTC
 * ctime() form Mth dd hh:mm:ss (e.g. Aug 25 09:54:37)
 * absolute form - # seconds since the epoch (e.g. 1095789191)
 * relative time - seconds before or after now (e.g. -300, +43200)
 * relative time - days/hours/minutes/seconds before or after now (e.g. -1d, +6h, +30m, -10s)
 */

#define CANONICAL_TIME_REX "^[0-9][0-9][0-9][0-9].[01]?[0-9].[0-3]?[0-9][ ]+[0-2]?[0-9]:[0-5][0-9]:[0-5][0-9][ ]+UTC$"
#define CTIME_REX "^[adfjmnos][aceopu][bcglnprtvy][ ]+[0-3]?[0-9][ ]+[0-2]?[0-9]:[0-5][0-9]:[0-5][0-9]$"
#define ABSOLUTE_TIME_REX "^[0-9]+[s]?$"
#define RELATIVE_TIME_REX "^[\\+-\\][0-9]+[smhdw]?$"

#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400
#define SECONDS_PER_WEEK 604800

static regex_t rex_canon, rex_ctime, rex_abs, rex_rel;
static int reg_status = 0;

/*
 * We use the last letter in the month name to determine
 * the month number (0-11).  There are two collisions:
 * Jan and Jun both end in n
 * Mar and Apr both end in r
 * In these cases we check the second letter.
 *
 * The MTH_LAST array maps the last letter to a number.
 */
static const int8_t MTH_LAST[] = {-1, 1, 11, -1, -1, -1, 7, -1, -1, -1, -1, 6, -1, 5, -1, 8, -1, 3, -1, 9, -1, 10, -1, -1, 4, -1};

static int
_month_num(char *s)
{
	int i;
	int8_t v8;

	v8 = -1;
	if (s[2] > 90) v8 = s[2] - 'a';
	else v8 = s[2] - 'A';

	if ((v8 < 0) || (v8 > 25)) return -1;

	v8 = MTH_LAST[v8];
	if (v8 < 0) return -1;

	i = v8;
	if ((i == 5) && ((s[1] == 'a') || (s[1] == 'A'))) return 0;
	if ((i == 3) && ((s[1] == 'a') || (s[1] == 'A'))) return 2;
	return i;
}

time_t
asl_parse_time(const char *in)
{
	int len, y;
	struct tm t;
	time_t tick, delta, factor;
	char *str, *p, *x;
	static dispatch_once_t once;

	if (in == NULL) return -1;

	dispatch_once(&once, ^{
		int status;
		int rflags = REG_EXTENDED | REG_NOSUB | REG_ICASE;

		memset(&rex_canon, 0, sizeof(regex_t));
		status = regcomp(&rex_canon, CANONICAL_TIME_REX, rflags);
		if (status != 0) reg_status = -1;

		memset(&rex_ctime, 0, sizeof(regex_t));
		status = regcomp(&rex_ctime, CTIME_REX, rflags);
		if (status != 0) reg_status = -1;

		memset(&rex_abs, 0, sizeof(regex_t));
		status = regcomp(&rex_abs, ABSOLUTE_TIME_REX, rflags);
		if (status != 0) reg_status = -1;

		memset(&rex_rel, 0, sizeof(regex_t));
		status = regcomp(&rex_rel, RELATIVE_TIME_REX, rflags);
		if (status != 0) reg_status = -1;
	});

	if (reg_status < 0) return -1;

	len = strlen(in) + 1;

	if (regexec(&rex_abs, in, 0, NULL, 0) == 0)
	{
		/*
		 * Absolute time (number of seconds since the epoch)
		 */
		str = strdup(in);
		if (str == NULL) return -1;

		if ((str[len-2] == 's') || (str[len-2] == 'S')) str[len-2] = '\0';

		tick = atol(str);
		free(str);

		return tick;
	}
	else if (regexec(&rex_rel, in, 0, NULL, 0) == 0)
	{
		/*
		 * Reletive time (number of seconds before or after right now)
		 */
		str = strdup(in);
		if (str == NULL) return -1;

		factor = 1;

		if ((str[len-2] == 's') || (str[len-2] == 'S'))
		{
			str[len-2] = '\0';
		}
		else if ((str[len-2] == 'm') || (str[len-2] == 'M'))
		{
			str[len-2] = '\0';
			factor = SECONDS_PER_MINUTE;
		}
		else if ((str[len-2] == 'h') || (str[len-2] == 'H'))
		{
			str[len-2] = '\0';
			factor = SECONDS_PER_HOUR;
		}
		else if ((str[len-2] == 'd') || (str[len-2] == 'D'))
		{
			str[len-2] = '\0';
			factor = SECONDS_PER_DAY;
		}
		else if ((str[len-2] == 'w') || (str[len-2] == 'W'))
		{
			str[len-2] = '\0';
			factor = SECONDS_PER_WEEK;
		}

		tick = time(NULL);
		delta = factor * atol(str);
		tick += delta;

		free(str);

		return tick;
	}
	else if (regexec(&rex_canon, in, 0, NULL, 0) == 0)
	{
		memset(&t, 0, sizeof(struct tm));
		str = strdup(in);
		if (str == NULL) return -1;

		/* Get year */
		x = str;
		p = strchr(x, '.');
		*p = '\0';
		t.tm_year = atoi(x) - 1900;

		/* Get month */
		x = p + 1;
		p = strchr(x, '.');
		*p = '\0';
		t.tm_mon = atoi(x) - 1;

		/* Get day */
		x = p + 1;
		p = strchr(x, ' ');
		*p = '\0';
		t.tm_mday = atoi(x);

		/* Get hour */
		for (x = p + 1; *x == ' '; x++);
		p = strchr(x, ':');
		*p = '\0';
		t.tm_hour = atoi(x);

		/* Get minutes */
		x = p + 1;
		p = strchr(x, ':');
		*p = '\0';
		t.tm_min = atoi(x);

		/* Get seconds */
		x = p + 1;
		p = strchr(x, ' ');
		*p = '\0';
		t.tm_sec = atoi(x);

		free(str);
		return timegm(&t);
	}
	else if (regexec(&rex_ctime, in, 0, NULL, 0) == 0)
	{
		/* We assume it's in the current year */
		memset(&t, 0, sizeof(struct tm));
		tick = time(NULL);
		gmtime_r(&tick, &t);
		y = t.tm_year;

		memset(&t, 0, sizeof(struct tm));
		str = strdup(in);
		if (str == NULL) return -1;

		t.tm_year = y;
		t.tm_mon = _month_num(str);
		if (t.tm_mon < 0) return -1;

		for (x = strchr(str, ' '); *x == ' '; x++);
		p = strchr(x, ' ');
		*p = '\0';
		t.tm_mday = atoi(x);

		/* Get hour */
		for (x = p + 1; *x == ' '; x++);
		p = strchr(x, ':');
		*p = '\0';
		t.tm_hour = atoi(x);

		/* Get minutes */
		x = p + 1;
		p = strchr(x, ':');
		*p = '\0';
		t.tm_min = atoi(x);

		/* Get seconds */
		x = p + 1;
		t.tm_sec = atoi(x);

		t.tm_isdst = -1;

		free(str);
		return mktime(&t);
	}

	return -1;
}

#endif /* BUILDING_VARIANT */
