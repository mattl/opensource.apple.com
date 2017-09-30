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
#import "APNSStream.h"

@implementation APNSStream

// -----------------------------------------------------------------
// initWithName ()

-(id)initWithName: (NSString *)name
{
    self = [super init];
    if (self) {
		dataToWrite = [[NSMutableArray array] retain];
		inputData = [[NSMutableData data] retain];
		currentOffset = 0;
		streamName = [name retain];
	}
	return( self );
} // initWithName

// -----------------------------------------------------------------
// dealloc ()

-(void) dealloc
{
	[iStream close];
	[oStream close];
	[dataToWrite release];
	[inputData release];
	[iStream release];
	[oStream release];
	[host release];
	[streamName release];
	[super dealloc];
} // dealloc

// -----------------------------------------------------------------
// setName ()

-(void) setName: (NSString *)name
{
	[streamName release];
	streamName = [name retain];
} // setName

// -----------------------------------------------------------------
//  ()

-(void) connectToHost: (NSString *)inHost port: (NSInteger)inPort
{
	host = [inHost retain];
	port = inPort;
	numReconnectAttempts = 0;
	[self startConnecting];
} // connectToHost

// -----------------------------------------------------------------
// isConnectedOrConnecting ()

-(BOOL) isConnectedOrConnecting
{
	return (iStream != nil && oStream != nil) || numReconnectAttempts > 0;
} // isConnectedOrConnecting

// -----------------------------------------------------------------
// startConnecting ()

-(void) startConnecting
{
	// iStream and oStream are instance variables
	[NSStream getStreamsToHost:[NSHost hostWithName:host] port:port inputStream:&iStream outputStream:&oStream];
	if (!iStream || !oStream) {
		log_info("start connecting failed");
		if (iStream) {
			[iStream close];
			iStream = nil;
		}
		if (oStream) {
			[oStream close];
			oStream = nil;
		}
		[self startReconnect];
		return;
	}
	[iStream retain];
	[oStream retain];
	[iStream setDelegate:self];
	[oStream setDelegate:self];

	NSDictionary *sslCertSetting = [self ssl_config];

	if ( sslCertSetting != nil ) {
		CFReadStreamSetProperty((CFReadStreamRef)iStream, kCFStreamPropertySSLSettings, (CFTypeRef)sslCertSetting);
		CFWriteStreamSetProperty((CFWriteStreamRef)oStream, kCFStreamPropertySSLSettings, (CFTypeRef)sslCertSetting);
	}
	
	[iStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	[oStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	[iStream open];
	[oStream open];
} // startConnecting

// -----------------------------------------------------------------
// newConnectionCreated ()

-(void) newConnectionCreated
{
} // newConnectionCreated

// -----------------------------------------------------------------
// connectionClosed ()

-(void) connectionClosed: (NSError *)streamError
{
} // connectionClosed

// -----------------------------------------------------------------
// closeConnection ()

-(void) closeConnection
{
	NSError *streamError = [iStream streamError];
	if (!streamError) {
		streamError = [oStream streamError];
	}

	// close the streams and try again
	[iStream close];
	[iStream release];
	iStream = nil;
	[oStream close];
	[oStream release];
	oStream = nil;

	/* Clear out any partially transferred data
	 * We keep any partially transferred outgoing objects but we drop all incoming bytes
	 */
	if (currentOffset > 0 || [inputData length] > 0) {
		log_info("saving %d already sent bytes for resend and dropping %d incoming bytes on stream %s",
			(int)currentOffset, (int)[inputData length], [streamName UTF8String]);
	}
	[inputData release];
	inputData = [[NSMutableData data] retain];
	currentOffset = 0;

	[self connectionClosed:streamError];
} // closeConnection

// -----------------------------------------------------------------
// startReconnect ()

-(void) startReconnect
{
	assert(iStream == nil);
	assert(oStream == nil);
	NSInteger reconnectTimes[] = { 15, 15, 30, 60, 120, 300 };
	NSInteger secsToReconnect = reconnectTimes[numReconnectAttempts];
	numReconnectAttempts++;
	if (numReconnectAttempts > 5) numReconnectAttempts = 5;
	[NSTimer scheduledTimerWithTimeInterval:secsToReconnect target:self selector:@selector(startConnecting) userInfo:nil repeats:NO];
	log_info("will attempt to reconnect stream %s to host %s:%d in %d seconds",
		[streamName UTF8String], [host UTF8String], (int)port, (int)secsToReconnect);
} // startReconnect

// -----------------------------------------------------------------
//	haveData ()

-(NSData *) haveData: (NSData *)in_data
{
	return nil;
} // haveData

// -----------------------------------------------------------------
//	ssl_config ()

-(NSDictionary *) ssl_config
{
	// subclass needs to override if they want to use SSL
	return( nil );
} // ssl_config

// -----------------------------------------------------------------
//	stream ()

- (void) stream: (NSStream *)in_stream handleEvent: (NSStreamEvent) in_event
{
	const char *direction;
	if (in_stream == oStream)
		direction = "outgoing";
	else if (in_stream == iStream)
		direction = "incoming";
	else {
		log_warning("invalid stream");
		return;
	}

	if (in_event == NSStreamEventErrorOccurred) {
		log_debug("stream: event: NSStreamEventErrorOccurred (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		log_err("stream: received error: %s on: %s stream: %s to host: %s:%d",
			[[[in_stream streamError]localizedDescription] UTF8String], direction, [streamName UTF8String], [host UTF8String], (int)port);
		[self closeConnection];
	} else if (in_event == NSStreamEventEndEncountered) {
		log_debug("stream: event: NSStreamEventEndEncountered (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		log_info("stream: received stream end on: %s stream: %s to host: %s:%d", direction, [streamName UTF8String], [host UTF8String], (int)port);
		[self closeConnection];
	} else if (in_event == NSStreamEventOpenCompleted) {
		log_debug("stream: event: NSStreamEventOpenCompleted (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		if (in_stream == oStream)
			[self newConnectionCreated];
	} else if (in_event == NSStreamEventHasSpaceAvailable) {
		log_debug("stream: event: NSStreamEventHasSpaceAvailable (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		numReconnectAttempts = 0;
		while ([dataToWrite count] > 0) {
			NSData* currentData = [dataToWrite objectAtIndex:0];
			NSUInteger length = [currentData length] - currentOffset;
			log_debug("stream: # blobs: %d, current blob length: %d, offset: %d", (int)[dataToWrite count], (int)[currentData length], (int)currentOffset);
			const uint8_t* bytes = [currentData bytes];
			NSInteger written = [oStream write:(bytes+currentOffset) maxLength:length];
			log_debug("stream: wrote %d bytes", (int)written);
			if (written == length) {
				[dataToWrite removeObjectAtIndex:0];
				currentOffset = 0;
			} else if (written > 0) {
				currentOffset += written;
			} else {
				if (written < 0) {
					  log_debug("stream: write returned: %d with stream error: %s on: %s stream: %s to host: %s:%u",
								(int)written, [[[oStream streamError]localizedDescription] UTF8String], direction,
								[streamName UTF8String], [host UTF8String], (int)port);
					[self closeConnection];
				}
				break;
			}
		}
	} else if (in_event == NSStreamEventHasBytesAvailable) {
		log_debug("stream: event: NSStreamEventHasBytesAvailable (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		uint8 tempBuffer[2048];
		NSInteger amountRead = [iStream read: tempBuffer maxLength: 2048];
		if (amountRead < 0) {
			log_debug("stream: read returned: %d with stream error: %s on: stream: %s to host: %s:%d",
					(int)amountRead, [[[iStream streamError]localizedDescription] UTF8String], [streamName UTF8String], [host UTF8String], (int)port);
			[self closeConnection];
		} else {
			[inputData appendBytes:(const void *)tempBuffer length:amountRead];
		}
		NSData *extraData = [self haveData: inputData];
		if (extraData)
			[inputData setData: extraData];
		else
			[inputData setLength: 0];
	} else {
		log_debug("stream: event: unknown (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		log_debug("stream: received unknown stream event: %d on: %s stream: %s to host: %s:%d",
			(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
	}
} // stream

// -----------------------------------------------------------------
//	writeData ()

- (void) writeData: (NSData *)in_data
{
	if ([dataToWrite count] == 0 && [oStream hasSpaceAvailable]) {
		log_debug("writeData: data length: %d, data to write: %d, space available: %d", (int)[in_data length], (int)[dataToWrite count], [oStream hasSpaceAvailable]);
		// we can go ahead and just try writing the data
		NSUInteger dataSize = [in_data length];
		NSInteger written = [oStream write:[in_data bytes] maxLength:[in_data length]];
		log_debug("writeData: wrote %d bytes, %d left", (int)written, (int)(dataSize-written));
		if (written < dataSize) {
			// it didn't all get written
			[dataToWrite addObject: in_data];
			if (written > 0) {
				currentOffset = written;
			} else {
				log_info("writeData: write returned %d with stream error (%s) on stream %s to host %s:%d",
					(int)written, [[[oStream streamError] localizedDescription] UTF8String], [streamName UTF8String], [host UTF8String], (int)port);
				if (written < 0) {
					[self closeConnection];
				}
			}
		}
	} else {
		// just add it to the queue of stuff to get written
		[dataToWrite addObject: in_data];
		log_debug("writeData: adding object to queue: data to write: %d, space available: %d", (int)[dataToWrite count], [oStream hasSpaceAvailable]);
	}
} // writeData

// -----------------------------------------------------------------
//	writeString ()

- (void) writeString: (NSString *)string
{
	NSData* data = [string dataUsingEncoding:NSUTF8StringEncoding];
	[self writeData: data];
} // writeString

@end
