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

#import <syslog.h>

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
		syslog(VLOG_INFO, "start connecting failed");
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
		syslog(VLOG_INFO, "saving %d already sent bytes for resend and dropping %d incoming bytes on stream %s",
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
	syslog(VLOG_NOTICE, "will attempt to reconnect stream %s to host %s:%d in %d seconds", [streamName UTF8String], [host UTF8String], (int)port, (int)secsToReconnect);
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
	return nil;
} // ssl_config

// -----------------------------------------------------------------
//	stream ()

- (void) stream: (NSStream *)in_stream handleEvent: (NSStreamEvent) in_event
{
	const char *direction;
	if (in_stream == oStream) {
		direction = "outgoing";
	} else {
		assert(in_stream == iStream);
		direction = "incoming";
	}

	if (in_event == NSStreamEventErrorOccurred) {
		syslog(VLOG_DEBUG, "stream: event: NSStreamEventErrorOccurred (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		syslog(VLOG_NOTICE, "stream: received error: %s on: %s stream: %s to host: %s:%d",
			[[[in_stream streamError]localizedDescription] UTF8String], direction, [streamName UTF8String], [host UTF8String], (int)port);
		[self closeConnection];
	} else if (in_event == NSStreamEventEndEncountered) {
		syslog(VLOG_DEBUG, "stream: event: NSStreamEventEndEncountered (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		syslog(VLOG_INFO, "stream: received stream end on: %s stream: %s to host: %s:%d", direction, [streamName UTF8String], [host UTF8String], (int)port);
		[self closeConnection];
	} else if (in_event == NSStreamEventOpenCompleted) {
		syslog(VLOG_DEBUG, "stream: event: NSStreamEventOpenCompleted (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		if (in_stream == oStream)
			[self newConnectionCreated];
	} else if (in_event == NSStreamEventHasSpaceAvailable) {
		syslog(VLOG_DEBUG, "stream: event: NSStreamEventHasSpaceAvailable (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		numReconnectAttempts = 0;
		while ([dataToWrite count] > 0) {
			NSData* currentData = [dataToWrite objectAtIndex:0];
			NSUInteger length = [currentData length] - currentOffset;
			syslog(VLOG_DEBUG, "stream: # blobs: %d, current blob length: %d, offset: %d", (int)[dataToWrite count], (int)[currentData length], (int)currentOffset);
			const uint8_t* bytes = [currentData bytes];
			NSInteger written = [oStream write:(bytes+currentOffset) maxLength:length];
			syslog(VLOG_DEBUG, "stream: wrote %d bytes", (int)written);
			if (written == length) {
				[dataToWrite removeObjectAtIndex:0];
				currentOffset = 0;
			} else if (written > 0) {
				currentOffset += written;
			} else {
				if (written < 0) {
					  syslog(VLOG_DEBUG, "stream: write returned: %d with stream error: %s on: %s stream: %s to host: %s:%u",
								(int)written, [[[oStream streamError]localizedDescription] UTF8String], direction,
								[streamName UTF8String], [host UTF8String], (int)port);
					[self closeConnection];
				}
				break;
			}
		}
	} else if (in_event == NSStreamEventHasBytesAvailable) {
		syslog(VLOG_DEBUG, "stream: event: NSStreamEventHasBytesAvailable (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		uint8 tempBuffer[2048];
		NSInteger amountRead = [iStream read: tempBuffer maxLength: 2048];
		if (amountRead < 0) {
			syslog(VLOG_DEBUG, "stream: read returned: %d with stream error: %s on: stream: %s to host: %s:%d",
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
		syslog(VLOG_DEBUG, "stream: event: unknown (%d) on: %s stream: %s to host: %s:%d",
				(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
		syslog(VLOG_DEBUG, "stream: received unknown stream event: %d on: %s stream: %s to host: %s:%d",
			(int)in_event, direction, [streamName UTF8String], [host UTF8String], (int)port);
	}
} // stream

// -----------------------------------------------------------------
//	writeData ()

- (void) writeData: (NSData *)in_data
{
	if ([dataToWrite count] == 0 && [oStream hasSpaceAvailable]) {
		syslog(VLOG_DEBUG, "writeData: data length: %d, data to write: %d, space available: %d", (int)[in_data length], (int)[dataToWrite count], [oStream hasSpaceAvailable]);
		// we can go ahead and just try writing the data
		NSUInteger dataSize = [in_data length];
		NSInteger written = [oStream write:[in_data bytes] maxLength:[in_data length]];
		syslog(VLOG_DEBUG, "writeData: wrote %d bytes, %d left", (int)written, (int)(dataSize-written));
		if (written < dataSize) {
			// it didn't all get written
			[dataToWrite addObject: in_data];
			if (written > 0) {
				currentOffset = written;
			} else {
				syslog(VLOG_INFO, "writeData: write returned %d with stream error (%s) on stream %s to host %s:%d",
					(int)written, [[[oStream streamError] localizedDescription] UTF8String], [streamName UTF8String], [host UTF8String], (int)port);
				if (written < 0) {
					[self closeConnection];
				}
			}
		}
	} else {
		// just add it to the queue of stuff to get written
		[dataToWrite addObject: in_data];
		syslog(VLOG_DEBUG, "writeData: adding object to queue: data to write: %d, space available: %d", (int)[dataToWrite count], [oStream hasSpaceAvailable]);
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
