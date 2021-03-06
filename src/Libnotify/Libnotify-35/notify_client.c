/*
 * Copyright (c) 2003-2007 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * "Portions Copyright (c) 2003 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.0 (the 'License').  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License."
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/signal.h>
#include <sys/syslimits.h>
#include <mach/mach.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <servers/bootstrap.h>
#include <errno.h>
#include <pthread.h>
#include "notify.h"
#include "notify_ipc.h"
#include "common.h"

#define SELF_PREFIX "self."
#define SELF_PREFIX_LEN 5

extern uint32_t _notify_lib_peek(notify_state_t *ns, uint32_t cid, int *val);
extern int *_notify_lib_check_addr(notify_state_t *ns, uint32_t cid);

static notify_state_t *self_state = NULL;
static mach_port_t notify_server_port = MACH_PORT_NULL;

#define CLIENT_TOKEN_TABLE_SIZE 256

#define TOKEN_TYPE_SELF 0x00000001
#define TOKEN_TYPE_MEMORY 0x00000002

typedef struct
{
	uint32_t client_id;
	uint32_t slot;
	uint32_t val;
	uint32_t flags;
	int fd;
	mach_port_t mp;
} token_table_node_t;

static table_t *token_table = NULL;
static uint32_t token_id = 0;
static pthread_mutex_t token_lock = PTHREAD_MUTEX_INITIALIZER;

static uint32_t fd_count = 0;
static int *fd_list = NULL;
static int *fd_refcount = NULL;
static char **fd_path = NULL;

static uint32_t mp_count = 0;
static mach_port_t *mp_list = NULL;
static int *mp_refcount = NULL;
static int *mp_mine = NULL;

static uint32_t *shm_base = NULL;

static int
shm_attach(uint32_t size)
{
	int32_t shmfd;

	shmfd = shm_open(SHM_ID, O_RDONLY, 0);
	if (shmfd == -1) return -1;

	shm_base = mmap(NULL, size, PROT_READ, MAP_SHARED, shmfd, 0);
	close(shmfd);

	if (shm_base == (uint32_t *)-1) shm_base = NULL;
	if (shm_base == NULL) return -1;

	return 0;
}

#ifdef NOTDEF
static void
shm_detach(void)
{
	if (shm_base != NULL)
	{
		shmdt(shm_base);
		shm_base = NULL;
	}
}
#endif

uint32_t
_notify_lib_init(char *sname)
{
	kern_return_t status;

	if (notify_server_port != MACH_PORT_NULL) return NOTIFY_STATUS_OK;
	status = bootstrap_look_up(bootstrap_port, sname, &notify_server_port);
	if (status != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return NOTIFY_STATUS_OK;
}

void _notify_fork_child()
{
	notify_server_port = MACH_PORT_NULL;
}

static uint32_t
token_table_add(uint32_t cid, uint32_t slot, uint32_t flags, uint32_t lock, int fd, mach_port_t mp)
{
	uint32_t tid;
	token_table_node_t *t;

	if (lock != 0) pthread_mutex_lock(&token_lock);
	if (token_table == NULL) 
	{
		token_table = _nc_table_new(CLIENT_TOKEN_TABLE_SIZE);
		if (token_table == NULL)
		{
			if (lock != 0) pthread_mutex_unlock(&token_lock);
			return -1;
		}
	}

	t = (token_table_node_t *)calloc(1, sizeof(token_table_node_t));
	if (t == NULL)
	{
		if (lock != 0) pthread_mutex_unlock(&token_lock);
		return -1;
	}

	tid = token_id++;

	t->client_id = cid;
	t->slot = slot;
	t->val = 0;
	t->flags = flags;
	t->fd = fd;
	t->mp = mp;

	_nc_table_insert_n(token_table, tid, t);

	if (lock != 0) pthread_mutex_unlock(&token_lock);

	return tid;
}

static token_table_node_t *
token_table_find(uint32_t tid)
{
	token_table_node_t *t;

	pthread_mutex_lock(&token_lock);

	t = (token_table_node_t *)_nc_table_find_n(token_table, tid);

	pthread_mutex_unlock(&token_lock);

	return t;
}

static void
notify_retain_file_descriptor(int fd, const char *path)
{
	int x, i;
	char *p;

	if (fd < 0) return;
	if (path == NULL) return;

	x = -1;
	for (i = 0; (i < fd_count) && (x < 0); i++)
	{
		if (fd_list[i] == fd) x = i;
	}

	if (x >= 0)
	{
		fd_refcount[x]++;
		return;
	}

	x = fd_count;
	fd_count++;
	p = strdup(path);
	if (p == NULL)
	{
		if (fd_list != NULL) free(fd_list);
		if (fd_refcount != NULL) free(fd_refcount);
		fd_count = 0;
		return;
	}

	if (x == 0)
	{
		fd_list = (int *)calloc(1, sizeof(int));
		fd_refcount = (int *)calloc(1, sizeof(int));
		fd_path = (char **)calloc(1, sizeof(char *));
	}
	else
	{
		fd_list = (int *)reallocf(fd_list, fd_count * sizeof(int));
		fd_refcount = (int *)reallocf(fd_refcount, fd_count * sizeof(int));
		fd_path = (char **)reallocf(fd_path, fd_count * sizeof(char *));
	}

	if ((fd_list == NULL) || (fd_refcount == NULL) || (fd_path == NULL))
	{
		if (fd_list != NULL) free(fd_list);
		if (fd_refcount != NULL) free(fd_refcount);
		fd_count = 0;
		return;
	}

	fd_list[x] = fd;
	fd_refcount[x] = 1;
	fd_path[x] = p;
}

static void
notify_release_file_descriptor(int fd)
{
	int x, i;

	if (fd < 0) return;

	x = -1;
	for (i = 0; (i < fd_count) && (x < 0); i++)
	{
		if (fd_list[i] == fd) x = i;
	}

	if (x < 0) return;

	if (fd_refcount[x] > 0) fd_refcount[x]--;
	if (fd_refcount[x] > 0) return;

	close(fd);
	unlink(fd_path[x]);
	free(fd_path[x]);

	if (fd_count == 1)
	{
		if (fd_list != NULL) free(fd_list);
		if (fd_refcount != NULL) free(fd_refcount);
		if (fd_path != NULL) free(fd_path);
		fd_count = 0;
		return;
	}

	for (i = x + 1; i < fd_count; i++)
	{
		fd_list[i - 1] = fd_list[i];
		fd_refcount[i - 1] = fd_refcount[i];
		fd_path[i - 1] = fd_path[i];
	}

	fd_count--;

	fd_list = (int *)reallocf(fd_list, fd_count * sizeof(int));
	fd_refcount = (int *)reallocf(fd_refcount, fd_count * sizeof(int));
	fd_path = (char **)reallocf(fd_path, fd_count * sizeof(char *));

	if ((fd_list == NULL) || (fd_refcount == NULL) || (fd_path == NULL))
	{
		if (fd_list != NULL) free(fd_list);
		if (fd_refcount != NULL) free(fd_refcount);
		if (fd_path != NULL) free(fd_path);
		fd_count = 0;
	}
}

static void
notify_retain_mach_port(mach_port_t mp, int mine)
{
	int x, i;

	if (mp == MACH_PORT_NULL) return;

	x = -1;
	for (i = 0; (i < mp_count) && (x < 0); i++)
	{
		if (mp_list[i] == mp) x = i;
	}

	if (x >= 0)
	{
		mp_refcount[x]++;
		return;
	}

	x = mp_count;
	mp_count++;

	if (x == 0)
	{
		mp_list = (mach_port_t *)calloc(1, sizeof(mach_port_t));
		mp_refcount = (int *)calloc(1, sizeof(int));
		mp_mine = (int *)calloc(1, sizeof(int));
	}
	else
	{
		mp_list = (mach_port_t *)reallocf(mp_list, mp_count * sizeof(mach_port_t));
		mp_refcount = (int *)reallocf(mp_refcount, mp_count * sizeof(int));
		mp_mine = (int *)reallocf(mp_mine, mp_count * sizeof(int));
	}

	if ((mp_list == NULL) || (mp_refcount == NULL) || (mp_mine == NULL))
	{
		if (mp_list != NULL) free(mp_list);
		if (mp_refcount != NULL) free(mp_refcount);
		if (mp_mine != NULL) free(mp_mine);
		mp_count = 0;
		return;
	}

	mp_list[x] = mp;
	mp_refcount[x] = 1;
	mp_mine[x] = mine;
}

static void
notify_release_mach_port(mach_port_t mp)
{
	int x, i;

	if (mp == MACH_PORT_NULL) return;

	x = -1;
	for (i = 0; (i < mp_count) && (x < 0); i++)
	{
		if (mp_list[i] == mp) x = i;
	}

	if (x < 0) return;

	if (mp_refcount[x] > 0) mp_refcount[x]--;
	if (mp_refcount[x] > 0) return;

	if (mp_mine[x] == 1) mach_port_destroy(mach_task_self(), mp);

	if (mp_count == 1)
	{
		if (mp_list != NULL) free(mp_list);
		if (mp_refcount != NULL) free(mp_refcount);
		if (mp_mine != NULL) free(mp_mine);
		mp_count = 0;
		return;
	}

	for (i = x + 1; i < mp_count; i++)
	{
		mp_list[i - 1] = mp_list[i];
		mp_refcount[i - 1] = mp_refcount[i];
		mp_mine[i - 1] = mp_mine[i];
	}

	mp_count--;

	mp_list = (mach_port_t *)reallocf(mp_list, mp_count * sizeof(mach_port_t));
	mp_refcount = (int *)reallocf(mp_refcount, mp_count * sizeof(int));
	mp_mine = (int *)reallocf(mp_mine, mp_count * sizeof(int));

	if ((mp_list == NULL) || (mp_refcount == NULL) || (mp_mine == NULL))
	{
		if (mp_list != NULL) free(mp_list);
		if (mp_refcount != NULL) free(mp_refcount);
		if (mp_mine != NULL) free(mp_mine);
		mp_count = 0;
	}
}

static void
token_table_delete(uint32_t tid, token_table_node_t *t)
{
	if (t == NULL) return;

	pthread_mutex_lock(&token_lock);

	notify_release_file_descriptor(t->fd);
	notify_release_mach_port(t->mp);

	_nc_table_delete_n(token_table, tid);

	pthread_mutex_unlock(&token_lock);

	free(t);
}

/*
 * PUBLIC API
 */
 
uint32_t
notify_post(const char *name)
{
	kern_return_t kstatus;
	uint32_t status;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		_notify_lib_post(self_state, name, 0, 0);
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_post(notify_server_port, (caddr_t)name, strlen(name), (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;

	return status;
}

uint32_t
notify_set_owner(const char *name, uint32_t uid, uint32_t gid)
{
	kern_return_t kstatus;
	uint32_t status;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		status = _notify_lib_set_owner(self_state, name, uid, gid);
		return status;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_set_owner(notify_server_port, (caddr_t)name, strlen(name), uid, gid, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_get_owner(const char *name, uint32_t *uid, uint32_t *gid)
{
	kern_return_t kstatus;
	uint32_t status;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		status = _notify_lib_get_owner(self_state, name, uid, gid);
		return status;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_get_owner(notify_server_port, (caddr_t)name, strlen(name), (int32_t *)uid, (int32_t *)gid, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_set_access(const char *name, uint32_t access)
{
	kern_return_t kstatus;
	uint32_t status;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		status = _notify_lib_set_access(self_state, name, access);
		return status;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_set_access(notify_server_port, (caddr_t)name, strlen(name), access, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_get_access(const char *name, uint32_t *access)
{
	kern_return_t kstatus;
	uint32_t status;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		status = _notify_lib_get_access(self_state, name, access);
		return status;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_get_access(notify_server_port, (caddr_t)name, strlen(name), (int32_t *)access, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_release_name(const char *name)
{
	kern_return_t kstatus;
	uint32_t status;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		status = _notify_lib_release_name(self_state, name, 0, 0);
		return status;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_release_name(notify_server_port, (caddr_t)name, strlen(name), (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_register_check(const char *name, int *out_token)
{
	kern_return_t kstatus;
	uint32_t status, cid;
	int32_t slot, shmsize;
	task_t task;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	task = mach_task_self();

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		status = _notify_lib_register_plain(self_state, name, task, -1, 0, 0, &cid);
		if (status != NOTIFY_STATUS_OK) return status;
		*out_token = token_table_add(cid, -1, TOKEN_TYPE_SELF, 1, -1, MACH_PORT_NULL);
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_register_check(notify_server_port, (caddr_t)name, strlen(name), task, &shmsize, &slot, (int32_t *)&cid, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	if (status == NOTIFY_STATUS_OK)
	{
		if (shmsize == -1)
		{
			*out_token = token_table_add(cid, -1, 0, 1, -1, MACH_PORT_NULL);
		}
		else
		{
			if (shm_base == NULL)
			{
				if (shm_attach(shmsize) != 0) return NOTIFY_STATUS_FAILED;
			}

			if (shm_base == NULL) return NOTIFY_STATUS_FAILED;

			*out_token = token_table_add(cid, slot, TOKEN_TYPE_MEMORY, 1, -1, MACH_PORT_NULL);
		}
	}
	return status;
}

uint32_t
notify_register_plain(const char *name, int *out_token)
{
	kern_return_t kstatus;
	uint32_t status, cid;
	task_t task;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	task = mach_task_self();

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		status = _notify_lib_register_plain(self_state, name, task, -1, 0, 0, &cid);
		if (status != NOTIFY_STATUS_OK) return status;
		*out_token = token_table_add(cid, -1, TOKEN_TYPE_SELF, 1, -1, MACH_PORT_NULL);
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_register_plain(notify_server_port, (caddr_t)name, strlen(name), task, (int32_t *)&cid, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	if (status == NOTIFY_STATUS_OK) *out_token = token_table_add(cid, -1, 0, 1, -1, MACH_PORT_NULL);
	return status;
}

uint32_t
notify_register_signal(const char *name, int sig, int *out_token)
{
	kern_return_t kstatus;
	uint32_t status, cid;
	task_t task;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;

	task = mach_task_self();

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL) return NOTIFY_STATUS_FAILED;
		status = _notify_lib_register_signal(self_state, name, task, sig, 0, 0, &cid);
		if (status != NOTIFY_STATUS_OK) return status;
		*out_token = token_table_add(cid, -1, TOKEN_TYPE_SELF, 1, -1, MACH_PORT_NULL);
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_register_signal(notify_server_port, (caddr_t)name, strlen(name), task, sig, (int32_t *)&cid, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	if (status == NOTIFY_STATUS_OK) *out_token = token_table_add(cid, -1, 0, 1, -1, MACH_PORT_NULL);
	return status;
}

uint32_t
notify_register_mach_port(const char *name, mach_port_name_t *notify_port, int flags, int *out_token)
{
	kern_return_t kstatus;
	uint32_t status, cid;
	task_t task;
	security_token_t sec;
	int mine;

	mine = 0;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;
	if (notify_port == NULL) return NOTIFY_STATUS_INVALID_PORT;

	task = mach_task_self();

	if ((flags & NOTIFY_REUSE) == 0)
	{
		mine = 1;
		kstatus = mach_port_allocate(task, MACH_PORT_RIGHT_RECEIVE, notify_port);
		if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	}

	kstatus = mach_port_insert_right(task, *notify_port, *notify_port, MACH_MSG_TYPE_MAKE_SEND);
	if (kstatus != KERN_SUCCESS)
	{
		if (mine == 1) mach_port_destroy(task, *notify_port);
		return NOTIFY_STATUS_FAILED;
	}

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL)
		{
			if (mine == 1) mach_port_destroy(task, *notify_port);
			return NOTIFY_STATUS_FAILED;
		}

		/* lock so that we can reserve current token_id */
		pthread_mutex_lock(&token_lock);
		status = _notify_lib_register_mach_port(self_state, name, task, *notify_port, token_id, 0, 0, &cid);
		if (status != NOTIFY_STATUS_OK)
		{
			if (mine == 1) mach_port_destroy(task, *notify_port);
			pthread_mutex_unlock(&token_lock);
			return status;
		}

		*out_token = token_table_add(cid, -1, TOKEN_TYPE_SELF, 0, -1, *notify_port);
		notify_retain_mach_port(*notify_port, mine);

		pthread_mutex_unlock(&token_lock);
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0)
		{
			if (mine == 1) mach_port_destroy(task, *notify_port);
			return NOTIFY_STATUS_FAILED;
		}
	}

	/* lock so that we can reserve current token_id */
	pthread_mutex_lock(&token_lock);
	kstatus = _notify_server_register_mach_port(notify_server_port, (caddr_t)name, strlen(name), task, *notify_port, token_id, (int32_t *)&cid, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS)
	{
		if (mine == 1) mach_port_destroy(task, *notify_port);
		pthread_mutex_unlock(&token_lock);
		return NOTIFY_STATUS_FAILED;
	}

	if (status == NOTIFY_STATUS_OK)
	{
		*out_token = token_table_add(cid, -1, 0, 0, -1, *notify_port);
		notify_retain_mach_port(*notify_port, mine);
	}
	else if (mine == 1) mach_port_destroy(task, *notify_port);

	pthread_mutex_unlock(&token_lock);
	return status;
}

static char *
_notify_mk_tmp_path(int tid)
{
    char tmp[PATH_MAX], *path;

	if (confstr(_CS_DARWIN_USER_TEMP_DIR, tmp, sizeof(tmp)) <= 0) return NULL;

	path = NULL;
	asprintf(&path, "%s/com.apple.notify.%d.%d", tmp, getpid(), tid);
	return path;
}
			 
uint32_t
notify_register_file_descriptor(const char *name, int *notify_fd, int flags, int *out_token)
{
	kern_return_t kstatus;
	uint32_t i, status, cid;
	task_t task;
	security_token_t sec;
	int mine, fd;
	char *path;

	mine = 0;
	path = NULL;

	sec.val[0] = -1;
	sec.val[1] = -1;

	if (name == NULL) return NOTIFY_STATUS_INVALID_NAME;
	if (notify_fd == NULL) return NOTIFY_STATUS_INVALID_FILE;

	task = mach_task_self();

	/* lock so that we can reserve current token_id */
	pthread_mutex_lock(&token_lock);

	if ((flags & NOTIFY_REUSE) == 0)
	{
		mine = 1;

		path = _notify_mk_tmp_path(token_id);
		if (path == NULL)
		{
			pthread_mutex_unlock(&token_lock);
			return NOTIFY_STATUS_FAILED;
		}

		/* unlink in case pids wrapped around */
		unlink(path);

		if (mkfifo(path, 0600) < 0)
		{
			free(path);
			pthread_mutex_unlock(&token_lock);
			return NOTIFY_STATUS_FAILED;
		}

		fd = open(path, O_RDWR, 0);
		if (fd < 0)
		{
			free(path);
			pthread_mutex_unlock(&token_lock);
			return NOTIFY_STATUS_FAILED;
		}

		*notify_fd = fd;
	}
	else
	{
		/* check the file descriptor - it must be one of "ours" */
		for (i = 0; (i < fd_count) && (path == NULL); i++)
		{
			if (fd_list[i] == *notify_fd) path = fd_path[i];
		}

		if (status == 0)
		{
			pthread_mutex_unlock(&token_lock);
			return NOTIFY_STATUS_INVALID_FILE;
		}
	}

	if (!strncmp(name, SELF_PREFIX, SELF_PREFIX_LEN))
	{
		if (self_state == NULL) self_state = _notify_lib_notify_state_new(NOTIFY_STATE_USE_LOCKS);
		if (self_state == NULL)
		{
			if (mine == 1)
			{
				free(path);
				close(*notify_fd);
			}

			pthread_mutex_unlock(&token_lock);
			return NOTIFY_STATUS_FAILED;
		}

		status = _notify_lib_register_file_descriptor(self_state, name, task, path, token_id, 0, 0, (uint32_t *)&cid);
		if (mine == 1) free(path);
		if (status != NOTIFY_STATUS_OK)
		{
			if (mine == 1) close(*notify_fd);
			pthread_mutex_unlock(&token_lock);
			return status;
		}

		*out_token = token_table_add(cid, -1, TOKEN_TYPE_SELF, 0, *notify_fd, MACH_PORT_NULL);

		notify_retain_file_descriptor(*notify_fd, path);

		pthread_mutex_unlock(&token_lock);
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0)
		{
			if (mine == 1)
			{
				free(path);
				close(*notify_fd);
			}

			pthread_mutex_unlock(&token_lock);
			return NOTIFY_STATUS_FAILED;
		}
	}

	kstatus = _notify_server_register_file_descriptor(notify_server_port, (caddr_t)name, strlen(name), task, path, strlen(path), token_id, (int32_t *)&cid, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS)
	{
		if (mine == 1) free(path);
		if (mine == 1) close(*notify_fd);
		pthread_mutex_unlock(&token_lock);
		return NOTIFY_STATUS_FAILED;
	}

	if (status == NOTIFY_STATUS_OK)
	{
		*out_token = token_table_add(cid, -1, 0, 0, *notify_fd, MACH_PORT_NULL);
		notify_retain_file_descriptor(*notify_fd, path);
	}
	else if (mine == 1) close(*notify_fd);

	if (mine == 1) free(path);
	pthread_mutex_unlock(&token_lock);
	return status;
}

uint32_t
notify_check(int token, int *check)
{
	kern_return_t kstatus;
	uint32_t status, val;
	token_table_node_t *t;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return _notify_lib_check(self_state, t->client_id, check);
	}

	if (t->flags & TOKEN_TYPE_MEMORY)
	{
		if (shm_base == NULL) return NOTIFY_STATUS_FAILED;

		*check = 0;
		val = ntohl(shm_base[t->slot]);
		if (t->val != val)
		{
			*check = 1;
			t->val = val;
		}
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_check(notify_server_port, t->client_id, check, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_peek(int token, uint32_t *val)
{
	token_table_node_t *t;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return _notify_lib_peek(self_state, t->client_id, (int *)val);
	}

	if (t->flags & TOKEN_TYPE_MEMORY)
	{
		if (shm_base == NULL) return NOTIFY_STATUS_FAILED;

		*val = ntohl(shm_base[t->slot]);
		return NOTIFY_STATUS_OK;
	}

	return NOTIFY_STATUS_INVALID_REQUEST;
}

int *
notify_check_addr(int token)
{
	token_table_node_t *t;

	t = token_table_find(token);
	if (t == NULL) return NULL;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return _notify_lib_check_addr(self_state, t->client_id);
	}

	if (t->flags & TOKEN_TYPE_MEMORY)
	{
		if (shm_base == NULL) return NULL;
		return (int *)&(shm_base[t->slot]);
	}

	return NULL;
}

uint32_t
notify_monitor_file(int token, char *path, int flags)
{
	kern_return_t kstatus;
	uint32_t status, len;
	token_table_node_t *t;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return NOTIFY_STATUS_INVALID_REQUEST;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	len = 0;
	if (path != NULL) len = strlen(path);

	kstatus = _notify_server_monitor_file(notify_server_port, t->client_id, path, len, flags, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_get_event(int token, int *ev, char *buf, int *len)
{
	kern_return_t kstatus;
	uint32_t status;
	token_table_node_t *t;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return NOTIFY_STATUS_INVALID_REQUEST;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_get_event(notify_server_port, t->client_id, ev, buf, (uint32_t *)len, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_get_state(int token, uint64_t *state)
{
	kern_return_t kstatus;
	uint32_t status;
	token_table_node_t *t;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return _notify_lib_get_state(self_state, t->client_id, state);
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_get_state(notify_server_port, t->client_id, state, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_set_state(int token, uint64_t state)
{
	kern_return_t kstatus;
	uint32_t status;
	token_table_node_t *t;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return _notify_lib_set_state(self_state, t->client_id, state, 0, 0);
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_set_state(notify_server_port, t->client_id, state, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_get_val(int token, int *val)
{
	kern_return_t kstatus;
	uint32_t status;
	token_table_node_t *t;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return _notify_lib_get_val(self_state, t->client_id, val);
	}

	if (t->flags & TOKEN_TYPE_MEMORY)
	{
		if (shm_base == NULL) return NOTIFY_STATUS_FAILED;

		*val = ntohl(shm_base[t->slot]);
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_get_val(notify_server_port, t->client_id, val, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_set_val(int token, int val)
{
	kern_return_t kstatus;
	uint32_t status;
	token_table_node_t *t;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		return _notify_lib_set_val(self_state, t->client_id, val, 0, 0);
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0) return NOTIFY_STATUS_FAILED;
	}

	kstatus = _notify_server_set_val(notify_server_port, t->client_id, val, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS) return NOTIFY_STATUS_FAILED;
	return status;
}

uint32_t
notify_cancel(int token)
{
	token_table_node_t *t;
	uint32_t status;
	kern_return_t kstatus;
	security_token_t sec;

	sec.val[0] = -1;
	sec.val[1] = -1;

	t = token_table_find(token);
	if (t == NULL) return NOTIFY_STATUS_INVALID_TOKEN;

	if (t->flags & TOKEN_TYPE_SELF)
	{
		_notify_lib_cancel(self_state, t->client_id);
		token_table_delete(token, t);
		return NOTIFY_STATUS_OK;
	}

	if (notify_server_port == MACH_PORT_NULL)
	{
		status = _notify_lib_init(NOTIFY_SERVICE_NAME);
		if (status != 0)
		{
			token_table_delete(token, t);
			return NOTIFY_STATUS_FAILED;
		}
	}

	kstatus = _notify_server_cancel(notify_server_port, t->client_id, (int32_t *)&status, &sec);
	if (kstatus != KERN_SUCCESS)
	{
		token_table_delete(token, t);
		return NOTIFY_STATUS_FAILED;
	}

	token_table_delete(token, t);
	return status;
}

