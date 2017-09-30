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
 * Note:       Based heavily on APNBridge
 *
 * Note:       When editing this file set Xcode to "Editor uses tabs/width=4".
 *             Any other editor or tab setting may not show this file nicely!
 */


#import "APNSStream.h"
#import "APNSNotify.h"

@interface APNSFeedback : APNSStream {
	BOOL		_is_production;
	APNSNotify *_pn_connection;
}

-(id)init_with_connection: (APNSNotify *)in_conn;
-(void)connect;

-(NSDictionary *)ssl_config;

@end
