/*
 * Contains:   Routines Mail Services support for Apple Push Notification Service.
 * Written by: Michael (for addtl writers check SVN comments).
 *
 * Copyright:  (c) 2008-2012 Apple Inc. All Rights Reserved.
 * 
 * IMPORTANT NOTE: This file is licensed only for use on Apple-branded
 * computers and is subject to the terms and conditions of the Apple Software
 * License Agreement accompanying the package this file is a part of.
 * You may not port this file to another platform without Apple's written consent.
 * 
 *
 * Note:       When editing this file set Xcode to "Editor uses tabs/width=4".
 *             Any other editor or tab setting may not show this file nicely!
 */

#ifndef __push_notify_h__
#define __push_notify_h__
#endif

#define	BUFF_SIZE			8192
#define MAX_BUF_SIZE		5120000
#define	GUID_BUFF_SIZE		64
#define DEVICE_MAPS_PATH	@"/Library/Server/Mail/Data/mta/guid_device_maps.plist"
#define SERVER_ADMIN		@"/Applications/Server.app/Contents/ServerRoot/usr/sbin/serveradmin"

// verbose logging
extern	int sig_usr1;

#define	VLOG_ERR		sig_usr1 ? LOG_CRIT : LOG_ERR
#define	VLOG_WARNING	sig_usr1 ? LOG_CRIT : LOG_WARNING
#define	VLOG_NOTICE		sig_usr1 ? LOG_CRIT : LOG_NOTICE
#define	VLOG_INFO		sig_usr1 ? LOG_CRIT : LOG_INFO
#define	VLOG_DEBUG		sig_usr1 ? LOG_CRIT : LOG_DEBUG

// user, id & device info from mail service
typedef struct msg_data_s {
	char msg[16];
	char d1[128];
	char d2[512];
	char d3[512];
	char d4[512];
} msg_data_t;
