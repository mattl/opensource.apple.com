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

#import <syslog.h>

#import "push_notify.h"
#import "APNSNotify.h"

NSString* sandboxOID = @"1.2.840.113635.100.6.3.1";
NSString* productionOID = @"1.2.840.113635.100.6.3.2";
NSString* userIDOID = @"0.9.2342.19200300.100.1.1";

uint32_t	g_APN_resp_ID	= 101;


@implementation APNSNotify

NSString* prodHost = @"gateway.push.apple.com";
NSString* sandboxHost = @"gateway.sandbox.push.apple.com";

// -----------------------------------------------------------------
// init_with_cert_pref

-(id) init_with_cert_pref: (NSString *)in_cert_pref
{
	self = [super initWithName:@"APN"];
	if (self) {
		[self generate_SSL_settings: in_cert_pref];
		if (_ssl_settings == nil) {
			[self release];
			self = nil;
		}
	}
	return( self );
}

// -----------------------------------------------------------------
// init

-(id) init
{
	return( [self init_with_cert_pref: @"apns:com.apple.mail"] );
}

// -----------------------------------------------------------------
// connect

-(void) connect
{
	NSString *name;
	if (_is_production)
		name = prodHost;
	else
		name = sandboxHost;

	syslog(VLOG_NOTICE, "Opening connection to apn server %s for topic %s (port 2195)", [name UTF8String], [[self topic_name] UTF8String]);
	[self connectToHost:name port:2195];
}

// -----------------------------------------------------------------
// newConnectionCreated

-(void) newConnectionCreated
{
	syslog(VLOG_INFO, "Connected to apn server %s for topic %s", [host UTF8String], [[self topic_name] UTF8String] );
}

// -----------------------------------------------------------------
// connectionClosed

-(void) connectionClosed: (NSError *)streamError
{
	if ( !streamError )
		syslog( streamError ? VLOG_WARNING : VLOG_NOTICE, 
			"Disconnected from apn server %s for topic %s", 
			[host UTF8String], [[self topic_name] UTF8String] );
	else {
		syslog(VLOG_NOTICE,  "Disconnected from apn server %s for topic %s: error %s", 
			[host UTF8String], [[self topic_name] UTF8String], [[streamError localizedFailureReason]UTF8String] );

		[self startReconnect];
	}

//	if (streamError)
//		[self startReconnect];
}

// -----------------------------------------------------------------
// ssl_config

-(NSDictionary*) ssl_config
{
	return( _ssl_settings );
}

// -----------------------------------------------------------------
// generate_SSL_settings

-(void) generate_SSL_settings: (NSString *)in_cert_pref;
{
    SecKeychainSetPreferenceDomain(kSecPreferencesDomainSystem);

	CFStringRef cp = CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)in_cert_pref);
	SecIdentityRef identity = SecIdentityCopyPreferred(cp, NULL, NULL);
	CFRelease(cp);

	if ( identity == NULL ) {
		syslog(VLOG_ERR, "Couldn't get identity for preference %s", [in_cert_pref UTF8String]);
		_ssl_settings = nil;
		return;
	}

	CFArrayRef ca = CFArrayCreate(NULL, (const void **)&identity, 1, &kCFTypeArrayCallBacks);
	CFRelease(identity);

	NSMutableDictionary* sslSettings = [NSMutableDictionary dictionary];
	
	[sslSettings setObject:(id)ca forKey:(NSString*)kCFStreamSSLCertificates];
	CFRelease(ca);

	[sslSettings setObject:(NSString*)kCFStreamSocketSecurityLevelTLSv1 forKey:(NSString*)kCFStreamSSLLevel];
	SecCertificateRef cert;
	SecIdentityCopyCertificate(identity, &cert);
	NSDictionary* dict = (NSDictionary*)SecCertificateCopyValues(cert, NULL, nil);
	CFRelease(cert);

	if (dict == nil)
		syslog(VLOG_WARNING, "SSL certificate null certificate dictionary!");

#if 0 // debug
	NSString *description = [dict description];
	NSArray *lines = [description componentsSeparatedByString: @"\n"];
	for (NSString *line in lines)
		syslog(VLOG_DEBUG, "%s", [line UTF8String]);
#endif

	if ([dict objectForKey:productionOID] != nil)
		_is_production = YES;
	else if ([dict objectForKey:sandboxOID] == nil)
		syslog(VLOG_WARNING, "Doesn't appear to be valid push cert");
			  
	NSArray* nameArray = [[dict objectForKey:(NSString*)kSecOIDX509V1SubjectName] objectForKey:(NSString*)kSecPropertyKeyValue];
	for (NSDictionary* nameDict in nameArray) {
		if ([[nameDict objectForKey:(NSString*)kSecPropertyKeyLabel] isEqualToString:userIDOID])
			_topic_name = [[nameDict objectForKey:(NSString*)kSecPropertyKeyValue] retain];
	}
	CFRelease((CFDictionaryRef)dict);
	
	syslog(VLOG_INFO, "is production = %d, topic = %s", _is_production, [_topic_name UTF8String]);
	
	if (_is_production)
		[sslSettings setObject:prodHost forKey:(NSString*)kCFStreamSSLPeerName];
	else
		[sslSettings setObject:sandboxHost forKey:(NSString*)kCFStreamSSLPeerName];
	
	_ssl_settings = [sslSettings retain];
} // generate_SSL_settings

// -----------------------------------------------------------------
// is_production

-(BOOL) is_production
{
    return( _is_production );
}

// -----------------------------------------------------------------
// topic_name

-(NSString*) topic_name
{
    return( _topic_name );
}

// -----------------------------------------------------------------
// hex_log_msg

-(void) hex_log_msg: (const uint8_t *) in_message size: (int)size tag: (NSString *)in_tag
{
	int i,j;

	NSMutableString * msg = [NSMutableString string];
	for (i = 0; i < size; i += 16) {
		NSMutableString *hex = [NSMutableString string];
		NSMutableString *chars = [NSMutableString string];
		for (j = 0; j < 16; j++) {
			NSString *hexVal;
			NSString *charVal;
			if (i + j >= size) {
				hexVal = @"  ";
				charVal = @" ";
			} else {
				if (isprint(in_message[i+j]))
					charVal = [NSString stringWithFormat:@"%c", in_message[i+j]];
				else
					charVal = @".";
					
				hexVal = [NSString stringWithFormat:@"%02X ", in_message[i+j]];
			}
			
			[hex appendString:hexVal];
			[chars appendString:charVal];
		}
		[msg appendString:@"push_notify: "];
		[msg appendString:hex];
		[msg appendString:@" "];
		[msg appendString:chars];
		[msg appendString:@"\n"];
	}
	syslog(VLOG_DEBUG, "%s message:\n%s", [in_tag UTF8String], [msg UTF8String]);
} // hex_log_msg

// -----------------------------------------------------------------
// haveData

-(NSData *) haveData: (NSData *)in_data
{
	if (!in_data || ![in_data length])
		return( nil );

	uint32_t resp_ID = 0;
	char *err_txt = NULL;
	size_t data_len = [in_data length];
	const uint8_t *data_buff = [in_data bytes];

	syslog(VLOG_DEBUG, "notify: have data: length: %d", (int)data_len);

	if (data_buff[0] == 8 && data_len >= 6) {
		int status = data_buff[1];
		memcpy(&resp_ID, &data_buff[2], sizeof(uint32_t));
		switch ( status ) {
			case 0: err_txt = " No error"; break;
			case 1: err_txt = " Processing error"; break;
			case 2: err_txt = " Missing device token"; break;
			case 3: err_txt = " Missing topic"; break;
			case 4: err_txt = " Missing payload"; break;
			case 5: err_txt = " Invalid token size"; break;
			case 6: err_txt = " Invalid topic size"; break;
			case 7: err_txt = " Invalid payload size"; break;
			case 8: err_txt = " Invalid token"; break;
		   default: err_txt = " Unknown error"; break;
		}
		syslog(VLOG_ERR, "Error: Received notification response: %s (%d) with response identifier: %d", err_txt, status, resp_ID);
	} else if ( data_len ){
		syslog(VLOG_ERR, "Received unknown notification response with command: %d length: %d", (int)data_buff[1], (int)data_len);
		[self hex_log_msg: data_buff size: data_len tag: @"receive"];
	}

	return( nil );
} // haveData

// -----------------------------------------------------------------
// send_notification

-(void) send_notification: (NSString *)in_payload to_device: (NSString *)in_device
{
	// verify connection
	if ( ![self isConnectedOrConnecting] )
		[self connect];

	// convert device to binary blob
	uint8_t binary_device_token[32];
	const char* device_str = [in_device UTF8String];
	if (device_str == NULL || strspn(device_str, "0123456789ABCDEFabcdef") != 64 || device_str[64] != '\0') {
		syslog(VLOG_WARNING, "Got bad device ID: %s", [in_device UTF8String]);
		return;
	}

	int i;
	for (i = 0; i < 64; i += 2) {
		int hi = device_str[i] >= 'a' ? device_str[i]-'a'+10 : device_str[i] >= 'A' ? device_str[i]-'A'+10 : device_str[i]-'0';
		int lo = device_str[i+1] >= 'a' ? device_str[i+1]-'a'+10 : device_str[i+1] >= 'A' ? device_str[i+1]-'A'+10 : device_str[i+1]-'0';
		binary_device_token[i/2] = (hi*16)+lo;
	}

	const char *payload_str = [in_payload UTF8String];
	size_t payload_len = strlen(payload_str);

	NSUInteger msg_size = 11 + DEVICE_BINARY_SIZE + 2 + payload_len;
	uint8_t *msg_buff = calloc(msg_size, 1);

	/* message format is, |COMMAND|ID|EXPIRY|TOKENLEN|TOKEN|PAYLOADLEN|PAYLOAD| */
	int offset = 0;	// buffer offset

	/* command */
	msg_buff[offset++] = 1;

	/* provider preference ordered ID */
	uint32_t order_err_resp = g_APN_resp_ID++;
	memcpy(msg_buff+offset, &order_err_resp, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	/* expiry date network order */
	uint32_t msg_expire = htonl(time(NULL) + 86400); // expire message if not delivered in 1 day
	memcpy(msg_buff+offset, &msg_expire, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	/* token length network order */
	uint16_t token_len = htons( DEVICE_BINARY_SIZE );
	memcpy(msg_buff+offset, &token_len, sizeof(uint16_t));
	offset += sizeof(uint16_t);

	/* device token */
	memcpy(msg_buff+offset, binary_device_token, DEVICE_BINARY_SIZE);
	offset += DEVICE_BINARY_SIZE;

	/* payload length network order */
	uint16_t net_order_payload_len = htons(payload_len);
	memcpy(msg_buff+offset, &net_order_payload_len, sizeof(uint16_t));
	offset += sizeof(uint16_t);

	/* payload */
	memcpy(msg_buff+offset, payload_str, payload_len);
	offset += payload_len;
		
	[self hex_log_msg: msg_buff size: msg_size tag: @"send"];
	NSData* data = [NSData dataWithBytesNoCopy: msg_buff length: msg_size];
	[self writeData: data];
} // send_notification


@end
