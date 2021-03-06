/*
 * Copyright (c) 2000-2004 Apple Computer, Inc. All rights reserved.
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

/*
 * Modification History
 *
 * October 30, 2003		Allan Nathanson <ajn@apple.com>
 * - add plugin "stop()" function support
 *
 * June 1, 2001			Allan Nathanson <ajn@apple.com>
 * - public API conversion
 *
 * 24 March 2000		Allan Nathanson (ajn@apple.com)
 * - created
 */

#include <getopt.h>
#include <stdio.h>
#include <sysexits.h>
#include <syslog.h>
#include <unistd.h>
#include <paths.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <objc/objc-runtime.h>

#include "configd.h"
#include "configd_server.h"
#include "plugin_support.h"

__private_extern__
Boolean	_configd_verbose		= FALSE;	/* TRUE if verbose logging enabled */

__private_extern__
FILE	*_configd_trace			= NULL;		/* non-NULL if tracing enabled */

__private_extern__
CFMutableSetRef	_plugins_exclude	= NULL;		/* bundle identifiers to exclude from loading */

__private_extern__
CFMutableSetRef	_plugins_verbose	= NULL;		/* bundle identifiers to enable verbose logging */

static CFMachPortRef termRequested	= NULL;		/* Mach port used to notify runloop of a shutdown request */


static const struct option longopts[] = {
//	{ "no-bundles",		no_argument,		0,	'b' },
//	{ "exclude-plugin",	required_argument,	0,	'B' },
//	{ "no-fork",		no_argument,		0,	'd' },
//	{ "test-bundle",	required_argument,      0,	't' },
//	{ "verbose",		no_argument,		0,	'v' },
//	{ "verbose-bundle",	required_argument,	0,	'V' },
	{ "help",		no_argument,		0,	'?' },
	{ 0,			0,                      0,	0 }
};


static void
usage(const char *prog)
{
	SCPrint(TRUE, stderr, CFSTR("%s: [-d] [-v] [-V bundleID] [-b] [-B bundleID] [-t bundle-path]\n"), prog);
	SCPrint(TRUE, stderr, CFSTR("options:\n"));
	SCPrint(TRUE, stderr, CFSTR("\t-d\tdisable daemon/run in foreground\n"));
	SCPrint(TRUE, stderr, CFSTR("\t-v\tenable verbose logging\n"));
	SCPrint(TRUE, stderr, CFSTR("\t-V\tenable verbose logging for the specified plug-in\n"));
	SCPrint(TRUE, stderr, CFSTR("\t-b\tdisable loading of ALL plug-ins\n"));
	SCPrint(TRUE, stderr, CFSTR("\t-B\tdisable loading of the specified plug-in\n"));
	SCPrint(TRUE, stderr, CFSTR("\t-t\tload/test the specified plug-in\n"));
	SCPrint(TRUE, stderr, CFSTR("\t\t  (Note: only the plug-in will be started)\n"));
	exit (EX_USAGE);
}


static void
catcher(int signum)
{
	switch (signum) {
		case SIGTERM :
			if (termRequested != NULL) {
				mach_msg_empty_send_t	msg;
				mach_msg_option_t	options;
				kern_return_t		status;

				/*
				 * send message to indicate that a request has been made
				 * for the daemon to be shutdown.
				 */
				msg.header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
				msg.header.msgh_size = sizeof(msg);
				msg.header.msgh_remote_port = CFMachPortGetPort(termRequested);
				msg.header.msgh_local_port = MACH_PORT_NULL;
				msg.header.msgh_id = 0;
				options = MACH_SEND_TIMEOUT;
				status = mach_msg(&msg.header,				/* msg */
							    MACH_SEND_MSG|options,	/* options */
							    msg.header.msgh_size,	/* send_size */
							    0,				/* rcv_size */
							    MACH_PORT_NULL,		/* rcv_name */
							    0,				/* timeout */
							    MACH_PORT_NULL);		/* notify */
				if (status == MACH_SEND_TIMED_OUT) {
					mach_msg_destroy(&msg.header);
				}
			} else {
				_exit(EX_OK);
			}
			break;
	}

	return;
}


static void
term(CFMachPortRef port, void *msg, CFIndex size, void *info)
{
	int	status	= EX_OK;
	Boolean	wait;

	wait = plugin_term(&status);
	if (!wait) {
		// if we are not waiting on a plugin
		status = server_shutdown();
		exit (status);
	}

	return;
}


static void
parent_exit(int i)
{
	_exit (0);
}


static void
init_fds()
{
	int	fd;
	int	i;

	/* close any open FDs */
	for (i = getdtablesize()-1; i>=0; i--) close(i);

	/* set stdin, stdout, stderr */
	fd = open(_PATH_DEVNULL, O_RDWR, 0);
	if (fd != -1) {
		int	ofd;

		// stdin
		(void) dup2(fd, STDIN_FILENO);

		// stdout, stderr
		ofd = open("/var/log/configd.log", O_WRONLY|O_APPEND, 0);
		if (ofd != -1) {
			if (fd > STDIN_FILENO) {
				(void) close(fd);
			}
			fd = ofd;
		}
		(void) dup2(fd, STDOUT_FILENO);
		(void) dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO) {
			(void) close(fd);
		}
	}

	return;
}


static void
set_trace()
{
	int	fd;

	/* set _configd_trace */
	fd = open("/var/log/configd.trace", O_WRONLY|O_APPEND, 0);
	if (fd != -1) {
		_configd_trace = fdopen(fd, "a");
	}

	return;
}


static int
fork_child()
{
	int	child_pid;

	signal(SIGTERM, parent_exit);
	child_pid = fork();
	switch (child_pid) {
		case -1: {
			return -1;
		}
		case 0: {
			/* child: becomes the daemon (see below) */
			signal(SIGTERM, SIG_DFL);
			break;
		}
		default: {
			/* parent: wait for signal, then exit */
			int	status;

			(void) wait4(child_pid, (int *)&status, 0, 0);
			if (WIFEXITED(status)) {
				fprintf(stderr,
					"*** configd (daemon) failed to start, exit status=%d",
					WEXITSTATUS(status));
			} else {
				fprintf(stderr,
					"*** configd (daemon) failed to start, received signal=%d",
					WTERMSIG(status));
			}
			fflush (stderr);
			exit (EX_SOFTWARE);
		}
	}

	if (setsid() == -1)
		return -1;

	(void) chdir("/");

	return 0;
}


static void
writepid(void)
{
	FILE *fp;

	fp = fopen("/var/run/configd.pid", "w");
	if (fp != NULL) {
		fprintf(fp, "%d\n", getpid());
		fclose(fp);
	}
}


int
main(int argc, char * const argv[])
{
	Boolean			enableRestart	= (argc <= 1);	/* only if there are no arguments */
	Boolean			forceForeground	= FALSE;
	mach_port_limits_t	limits;
	Boolean			loadBundles	= TRUE;
	struct sigaction	nact;
	int			opt;
	extern int		optind;
	const char		*prog		= argv[0];
	CFRunLoopSourceRef	rls;
	mach_port_t		service_port	= MACH_PORT_NULL;
	kern_return_t		status;
	CFStringRef		str;
	const char		*testBundle	= NULL;

	_plugins_exclude = CFSetCreateMutable(NULL, 0, &kCFTypeSetCallBacks);
	_plugins_verbose = CFSetCreateMutable(NULL, 0, &kCFTypeSetCallBacks);

	/* process any arguments */

	while ((opt = getopt_long(argc, argv, "bB:dt:vV:", longopts, NULL)) != -1) {
		switch(opt) {
			case 'b':
				loadBundles = FALSE;
				break;
			case 'B':
				str = CFStringCreateWithCString(NULL, optarg, kCFStringEncodingMacRoman);
				CFSetSetValue(_plugins_exclude, str);
				CFRelease(str);
				break;
			case 'd':
				forceForeground = TRUE;
				break;
			case 't':
				testBundle = optarg;
				break;
			case 'v':
				_configd_verbose = TRUE;
				break;
			case 'V':
				if (strcmp(optarg, "com.apple.SystemConfiguration") == 0) {
					_sc_verbose = TRUE;
				} else {
					str = CFStringCreateWithCString(NULL, optarg, kCFStringEncodingMacRoman);
					CFSetSetValue(_plugins_verbose, str);
					CFRelease(str);
				}
				break;
			case '?':
			default :
				usage(prog);
		}
	}
	argc -= optind;
	argv += optind;

	/*
	 * display an error if configd is already running and we are
	 * not solely executing/testing a bundle.
	 */
	if (testBundle == NULL) {
		if (server_active(&service_port)) {
			exit (EX_UNAVAILABLE);
		}
	}

	/* check credentials */
	if (getuid() != 0) {
		fprintf(stderr, "%s: permission denied.\n", prog);
		exit (EX_NOPERM);
	}

	if (!forceForeground && (service_port == MACH_PORT_NULL)) {
		/*
		 * if we haven't been asked to run in the foreground
		 * and have not been started by mach_init (i.e. we're
		 * not already running as a Foreground process) then
		 * daemonize ourself.
		 */
		if (fork_child() == -1) {
			fprintf(stderr, "configd: fork() failed, %s\n", strerror(errno));
			exit (1);
		}

		/*
		 * Note: we are now the child process. The parent
		 *       waits/exits in fork_child.
		 */
	}

	/*
	 * close file descriptors, establish stdin/stdout/stderr,
	 * setup logging.
	 */
	if (!forceForeground) {
		int	logopt	= LOG_NDELAY|LOG_PID;

		init_fds();

		if (_configd_verbose)
			logopt |= LOG_CONS;
		openlog("configd", logopt, LOG_DAEMON);
	} else {
		_sc_log = FALSE;	/* redirect SCLog() to stdout/stderr */
	}

	/* check/enable trace logging */
	set_trace();

	/* record process id */
	if (testBundle == NULL) {
		writepid();
	}

	/* add signal handler to catch a SIGHUP */
	nact.sa_handler = catcher;
	sigemptyset(&nact.sa_mask);
	nact.sa_flags = SA_RESTART;
	if (sigaction(SIGHUP, &nact, NULL) == -1) {
		SCLog(_configd_verbose, LOG_ERR,
		       CFSTR("sigaction(SIGHUP, ...) failed: %s"),
		       strerror(errno));
	}

	/* add signal handler to catch a SIGPIPE */
	if (sigaction(SIGPIPE, &nact, NULL) == -1) {
		SCLog(_configd_verbose, LOG_ERR,
		       CFSTR("sigaction(SIGPIPE, ...) failed: %s"),
		       strerror(errno));
	}

	/* add signal handler to catch a SIGTERM */
	if (sigaction(SIGTERM, &nact, NULL) == -1) {
		SCLog(_configd_verbose, LOG_ERR,
		      CFSTR("sigaction(SIGTERM, ...) failed: %s"),
		      strerror(errno));
	}

	/* create the "shutdown requested" notification port */
	termRequested = CFMachPortCreate(NULL, term, NULL, NULL);

	// set queue limit
	limits.mpl_qlimit = 1;
	status = mach_port_set_attributes(mach_task_self(),
					  CFMachPortGetPort(termRequested),
					  MACH_PORT_LIMITS_INFO,
					  (mach_port_info_t)&limits,
					  MACH_PORT_LIMITS_INFO_COUNT);
	if (status != KERN_SUCCESS) {
		perror("mach_port_set_attributes");
	}

	// add to our runloop
	rls = CFMachPortCreateRunLoopSource(NULL, termRequested, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
	CFRelease(rls);

	if (testBundle == NULL) {
		/* initialize primary (store management) thread */
		server_init(service_port, enableRestart);

		if (!forceForeground && (service_port == MACH_PORT_NULL)) {
		    /* synchronize with parent process */
		    kill(getppid(), SIGTERM);
		}

		/* load/initialize/start bundles into the secondary thread */
		if (loadBundles) {
			objc_setMultithreaded(YES);
			plugin_init();
		}

		/* start primary (store management) thread */
		server_loop();
	} else {
		/* load/initialize/start specified plug-in */
		plugin_exec((void *)testBundle);
	}

	exit (EX_OK);	// insure the process exit status is 0
	return 0;	// ...and make main fit the ANSI spec.
}
