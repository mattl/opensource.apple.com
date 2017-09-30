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
