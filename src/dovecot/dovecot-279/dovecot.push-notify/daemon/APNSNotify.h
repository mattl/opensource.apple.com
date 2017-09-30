/*
 * Contains:   Routines Mail Services support for Apple Push Notification Service.
 * Written by: Michael (for addtl writers check SVN comments).
 *
 * Copyright (c) 2011-2012 Apple Inc. All Rights Reserved.
 * 
 * IMPORTANT NOTE: This file is licensed only for use on Apple-branded
 * computers and is subject to the terms and conditions of the Apple Software
 * License Agreement accompanying the package this file is a part of.
 * You may not port this file to another platform without Apple's written consent.
 * 
 *
 * Note:       Based heavily on APNBridge
 *
 * Note:       When editing this file set Xcode to "Editor uses tabs/width=4".
 *             Any other editor or tab setting may not show this file nicely!
 */

#import "APNSStream.h"

#define DEVICE_BINARY_SIZE 32
#define MAXPAYLOAD_SIZE 256

@interface APNSNotify : APNSStream {
	BOOL			_is_production;
    NSString	   *_topic_name;
	NSDictionary   *_ssl_settings;
}

-(id) init_with_cert_pref: (NSString *)cert_pref;
-(void)	generate_SSL_settings: (NSString *)certPref;
-(void)	connect;

-(BOOL) is_production;
-(NSString *) topic_name;
-(NSDictionary *)ssl_config;

-(void) send_notification: (NSString *)in_payload to_device:(NSString *)in_device;

@end
