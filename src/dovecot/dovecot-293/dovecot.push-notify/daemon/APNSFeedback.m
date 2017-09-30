/*
 * Contains:   Routines Mail Services support for Apple Push Notification Service.
 * Written by: Michael (for addtl writers check SVN comments).
 *
 * Copyright (c) 2010-2013 Apple Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without  
 * modification, are permitted provided that the following conditions  
 * are met:
 * 
 * 1.  Redistributions of source code must retain the above copyright  
 * notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above  
 * copyright notice, this list of conditions and the following  
 * disclaimer in the documentation and/or other materials provided  
 * with the distribution.
 * 3.  Neither the name of Apple Inc. ("Apple") nor the names of its  
 * contributors may be used to endorse or promote products derived  
 * from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS  
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND  
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF  
 * SUCH DAMAGE.
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


#import "push_notify.h"
#import "APNSFeedback.h"


@implementation APNSFeedback

NSString* feedbackProdHost = @"feedback.push.apple.com";
NSString* feedbackSandboxHost = @"feedback.sandbox.push.apple.com";


// -----------------------------------------------------------------
// init_with_connection

-(id) init_with_connection: (APNSNotify *)in_connection
{
	NSString* name;
	if ([in_connection is_production])
		name = @"APN-feedback";
	else
		name = @"APN-Sandbox-feedback";

	self = [super initWithName:name];
	if ( self ) {
		_is_production = [in_connection is_production];
		_pn_connection = in_connection;
	}
	
	return( self );
} // init_with_connection

// -----------------------------------------------------------------
// connect

-(void) connect
{
	NSString *name;
	if (_is_production) {
		name = feedbackProdHost;
	} else {
		name = feedbackSandboxHost;
	}
	log_info("Opening connection to apn feedback server %s for topic %s",
		[name UTF8String], [[_pn_connection topic_name] UTF8String]);
	[self connectToHost:name port:2196];
} // connect

// -----------------------------------------------------------------
// newConnectionCreated

-(void) newConnectionCreated
{
	log_info("Connected to apn feedback server %s for topic %s",
		[host UTF8String], [[_pn_connection topic_name]UTF8String]);
} // newConnectionCreated

// -----------------------------------------------------------------
// connectionClosed

-(void) connectionClosed: (NSError *) streamError
{
	if ( !streamError )
		log_info("Disconnected from apn feedback server %s for topic %s",
			[host UTF8String], [[_pn_connection topic_name]UTF8String]);
	else {
		log_warning("Disconnected from apn feedback server %s for topic %s: error: %s",
			[host UTF8String], [[_pn_connection topic_name]UTF8String], [[streamError localizedFailureReason]UTF8String]);

		[self startReconnect];
	}
} // connectionClosed

// -----------------------------------------------------------------
// ssl_config

-(NSDictionary *) ssl_config
{
	NSMutableDictionary* sslSettings = [NSMutableDictionary dictionaryWithDictionary:[_pn_connection ssl_config]];
	
	if (_is_production)
		[sslSettings setObject:feedbackProdHost forKey:(NSString*)kCFStreamSSLPeerName];
	else
		[sslSettings setObject:feedbackSandboxHost forKey:(NSString*)kCFStreamSSLPeerName];
	
	return sslSettings;
} // ssl_config

// -----------------------------------------------------------------
// convertBytesToToken

-(NSString *)convertBytesToToken: (const char *)bytes length: (UInt32)length
{
	NSMutableString* result = [NSMutableString string];
	int i;
	for (i = 0; i < length; i++) {
		char hi = (bytes[i]>>4)&0xf;
		hi = (hi >= 10) ? hi - 10 + 'A' : hi + '0';
		char lo = bytes[i]&0xf;
		lo = (lo >= 10) ? lo - 10 + 'A' : lo + '0';
		[result appendFormat:@"%c%c", hi, lo];
	}
	
	return result;
} // convertBytesToToken

// -----------------------------------------------------------------
// haveData

-(NSData *) haveData: (NSData *)data
{
	const char *start = [data bytes];
	size_t length = [data length];
	const char *bytes = start;

	log_debug("feedback: have data: length: %d", (int)length);

	while ( (start + length - bytes) >= 38 ) {
		// TODO: use date for re-rigestration of device
		UInt32 date = ntohl(*(UInt32*)bytes);
		log_debug("feedback: have data: date: %d", (int)date);

		bytes += 4;
		UInt16 size = ntohs(*(UInt16*)bytes);
		log_debug("feedback: have data: token length: %d", (int)size);

		bytes += 2;
		if (size != 32) {
			log_warning("feedback: have data: unexpected token size %d", (int)size);
			if (size > start+length-bytes) {
				bytes -= 6;
				break;
			}
		}

		// TO-DO:
		//	Check date & device token for re-registration
		NSDate* rejectDate = [NSDate dateWithTimeIntervalSince1970: date];
		NSDateFormatter *format = [[NSDateFormatter alloc] init];
		[format setDateFormat:@"MMM dd, yyyy HH:mm"];
		NSString *dateString = [format stringFromDate: rejectDate];
		log_debug("feedback: have data: date string: %s", [dateString UTF8String]);

		// Flag device as not responding & stop sending notifications
		NSString *token = [self convertBytesToToken: bytes length: size];
		log_warning("feedback: have data: token: %s", [token UTF8String]);
		bytes += size;

		NSMutableDictionary *devices = [NSMutableDictionary dictionaryWithContentsOfFile: DEVICE_MAPS_PATH];
		if ( devices ) {
			NSMutableArray *acct_maps = nil;
			NSEnumerator *acct_enum = [devices objectEnumerator];
			while ( (acct_maps = [acct_enum nextObject]) ) {
				NSMutableDictionary *dev_map = nil;
				NSEnumerator *dev_enum = [acct_maps objectEnumerator];
				while ( (dev_map = [dev_enum nextObject]) ) {
					if ( [[dev_map objectForKey: @"aps-device-token"] isEqualToString: token] ) {
						[dev_map setObject: rejectDate forKey: @"time-stamp"];
					}
				}
			}
			[devices writeToFile: DEVICE_MAPS_PATH atomically: YES];
		}

	}

	NSRange range = {
		bytes-start,
		start+length-bytes
	};

	return [data subdataWithRange:range];
} // haveData

@end
