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

#import <Foundation/Foundation.h>

@interface APNSStream : NSObject <NSStreamDelegate> {
	NSInputStream	*iStream;
	NSOutputStream	*oStream;
	NSMutableArray	*dataToWrite;
	NSInteger		 currentOffset;
	NSMutableData	*inputData;
	NSString		*host;
	NSInteger		 port;
	NSInteger		 numReconnectAttempts;
	NSString		*streamName;
}

-(id)initWithName: (NSString *)name;

-(void)setName: (NSString *)name;

-(void)connectToHost: (NSString *)inHost port: (NSInteger)inPort;
-(BOOL)isConnectedOrConnecting;
-(void)startConnecting;
-(void)newConnectionCreated;
-(NSData *)haveData: (NSData *)data;
-(void)closeConnection;
-(void)connectionClosed: (NSError *)streamError;
-(void)startReconnect;

-(NSDictionary *)ssl_config;
- (void)stream: (NSStream  *)in_stream handleEvent: (NSStreamEvent)in_event;

- (void)writeData: (NSData *)data;
- (void)writeString:(NSString *)string;

@end
