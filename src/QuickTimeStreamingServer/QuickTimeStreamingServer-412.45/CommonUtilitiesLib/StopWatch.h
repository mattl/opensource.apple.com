/*
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * Copyright (c) 1999-2001 Apple Computer, Inc.  All Rights Reserved. The
 * contents of this file constitute Original Code as defined in and are
 * subject to the Apple Public Source License Version 1.2 (the 'License').
 * You may not use this file except in compliance with the License.  Please
 * obtain a copy of the License at http://www.apple.com/publicsource and
 * read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.  Please
 * see the License for the specific language governing rights and
 * limitations under the License.
 *
 *
 * @APPLE_LICENSE_HEADER_END@
 *
 */
 

/*
	File:		StopWatch.h

	Contains:  	classes for stopwatch timers		
	
*/ 


#include "OS.h"

class MilliSecondStopWatch {

	public:
			MilliSecondStopWatch () { fStartedAt = -1; fStoppedAt = -1; }
			
			void Start() { fStartedAt = OS::Milliseconds(); }
			void Stop() { fStoppedAt = OS::Milliseconds(); }
			SInt64	Duration() { return fStoppedAt - fStartedAt; }
	private:
		SInt64	fStartedAt;
		SInt64	fStoppedAt;
		
};



class MicroSecondStopWatch {

	public:
			MicroSecondStopWatch () { fStartedAt = -1; fStoppedAt = -1; }
			
			void Start() { fStartedAt = OS::Microseconds(); }
			void Stop() { fStoppedAt = OS::Microseconds(); }
			SInt64	Duration() { return fStoppedAt - fStartedAt; }
	private:
		SInt64	fStartedAt;
		SInt64	fStoppedAt;
		
};

