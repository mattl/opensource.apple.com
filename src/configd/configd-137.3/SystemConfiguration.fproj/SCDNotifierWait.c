/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
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
 * June 1, 2001			Allan Nathanson <ajn@apple.com>
 * - public API conversion
 *
 * March 31, 2000		Allan Nathanson <ajn@apple.com>
 * - initial revision
 */

#include <mach/mach.h>
#include <mach/mach_error.h>

#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/SCPrivate.h>
#include "SCDynamicStoreInternal.h"
#include "config.h"		/* MiG generated file */

static mach_msg_id_t
waitForMachMessage(mach_port_t port)
{
	kern_return_t 		status;
	mach_msg_empty_rcv_t	*buf;

	mach_msg_size_t		size = sizeof(mach_msg_empty_t) + MAX_TRAILER_SIZE;

	status = vm_allocate(mach_task_self(), (vm_address_t *)&buf, size, TRUE);
	if (status != KERN_SUCCESS) {
		SCLog(TRUE, LOG_DEBUG, CFSTR("waitForMachMessage vm_allocate(): %s"), mach_error_string(status));
		return -1;
	}

	status = mach_msg(&buf->header,			/* msg */
			  MACH_RCV_MSG,			/* options */
			  0,				/* send_size */
			  size,				/* rcv_size */
			  port,				/* rcv_name */
			  MACH_MSG_TIMEOUT_NONE,	/* timeout */
			  MACH_PORT_NULL);		/* notify */
	if (status != KERN_SUCCESS) {
		SCLog(TRUE, LOG_DEBUG, CFSTR("waitForMachMessage mach_msg(): %s"), mach_error_string(status));
		return -1;
	}

	return buf->header.msgh_id;
}


Boolean
SCDynamicStoreNotifyWait(SCDynamicStoreRef store)
{
	SCDynamicStorePrivateRef	storePrivate = (SCDynamicStorePrivateRef)store;
	kern_return_t			status;
	mach_port_t			port;
	mach_port_t			oldNotify;
	int				sc_status;
	mach_msg_id_t			msgid;

	if (store == NULL) {
		/* sorry, you must provide a session */
		_SCErrorSet(kSCStatusNoStoreSession);
		return FALSE;
	}

	if (storePrivate->server == MACH_PORT_NULL) {
		/* sorry, you must have an open session to play */
		_SCErrorSet(kSCStatusNoStoreServer);
		return FALSE;
	}

	if (storePrivate->notifyStatus != NotifierNotRegistered) {
		/* sorry, you can only have one notification registered at once */
		_SCErrorSet(kSCStatusNotifierActive);
		return FALSE;
	}

	/* Allocating port (for server response) */
	status = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
	if (status != KERN_SUCCESS) {
		SCLog(TRUE, LOG_DEBUG, CFSTR("SCDynamicStoreNotifyWait mach_port_allocate(): %s"), mach_error_string(status));
		_SCErrorSet(status);
		return FALSE;
	}

	status = mach_port_insert_right(mach_task_self(),
					port,
					port,
					MACH_MSG_TYPE_MAKE_SEND);
	if (status != KERN_SUCCESS) {
		SCLog(TRUE, LOG_DEBUG, CFSTR("SCDynamicStoreNotifyWait mach_port_insert_right(): %s"), mach_error_string(status));
		(void) mach_port_destroy(mach_task_self(), port);
		_SCErrorSet(status);
		return FALSE;
	}

	/* Request a notification when/if the server dies */
	status = mach_port_request_notification(mach_task_self(),
						port,
						MACH_NOTIFY_NO_SENDERS,
						1,
						port,
						MACH_MSG_TYPE_MAKE_SEND_ONCE,
						&oldNotify);
	if (status != KERN_SUCCESS) {
		SCLog(TRUE, LOG_DEBUG, CFSTR("SCDynamicStoreNotifyWait mach_port_request_notification(): %s"), mach_error_string(status));
		(void) mach_port_destroy(mach_task_self(), port);
		_SCErrorSet(status);
		return FALSE;
	}

#ifdef	DEBUG
	if (oldNotify != MACH_PORT_NULL) {
		SCLog(TRUE, LOG_ERR, CFSTR("SCDynamicStoreNotifyWait(): why is oldNotify != MACH_PORT_NULL?"));
	}
#endif	/* DEBUG */

	status = notifyviaport(storePrivate->server,
			       port,
			       0,
			       (int *)&sc_status);

	if (status != KERN_SUCCESS) {
#ifdef	DEBUG
		if (status != MACH_SEND_INVALID_DEST)
			SCLog(TRUE, LOG_DEBUG, CFSTR("SCDynamicStoreNotifyWait notifyviaport(): %s"), mach_error_string(status));
#endif	/* DEBUG */
		(void) mach_port_destroy(mach_task_self(), storePrivate->server);
		storePrivate->server = MACH_PORT_NULL;
		_SCErrorSet(status);
		return FALSE;
	}

	if (sc_status != kSCStatusOK) {
		_SCErrorSet(sc_status);
		return FALSE;
	}

	/* set notifier active */
	storePrivate->notifyStatus = Using_NotifierWait;

	msgid = waitForMachMessage(port);

	/* set notifier inactive */
	storePrivate->notifyStatus = NotifierNotRegistered;

	if (msgid == MACH_NOTIFY_NO_SENDERS) {
		/* the server closed the notifier port */
#ifdef	DEBUG
		SCLog(_sc_verbose, LOG_DEBUG, CFSTR("SCDynamicStoreNotifyWait notifier port closed, destroying port %d"), port);
#endif	/* DEBUG */
		_SCErrorSet(kSCStatusNoStoreServer);
		return FALSE;
	}

	if (msgid == -1) {
		/* one of the mach routines returned an error */
#ifdef	DEBUG
		SCLog(_sc_verbose, LOG_DEBUG, CFSTR("SCDynamicStoreNotifyWait communication with server failed, destroying port %d"), port);
#endif	/* DEBUG */
		(void) mach_port_destroy(mach_task_self(), port);
		_SCErrorSet(kSCStatusNoStoreServer);
		return FALSE;
	}

	// something changed, cancelling notification request
	status = notifycancel(storePrivate->server,
			      (int *)&sc_status);

	if (status != KERN_SUCCESS) {
#ifdef	DEBUG
		if (status != MACH_SEND_INVALID_DEST)
			SCLog(_sc_verbose, LOG_DEBUG, CFSTR("SCDynamicStoreNotifyWait notifycancel(): %s"), mach_error_string(status));
#endif	/* DEBUG */
		(void) mach_port_destroy(mach_task_self(), storePrivate->server);
		storePrivate->server = MACH_PORT_NULL;
		_SCErrorSet(status);
		return FALSE;
	}

	(void) mach_port_destroy(mach_task_self(), port);

	return TRUE;
}
