/*
 * Copyright (c) 1998-2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

// AppleCDDAFileSystemDebug.h created by CJS on Tue 20-Jun-2000

#ifndef __APPLE_CDDA_FS_DEBUG_H__
#define __APPLE_CDDA_FS_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

// Debug Options
#define DEBUG			0
#define DEBUGLEVEL		4		// 1-5  1=fatal errors only, 5=full debugging

#if DEBUG

	#if DEBUGLEVEL > 2
		#define DebugLog(x)		printf	x		// Turn DebugLog() on
	#else
		#define DebugLog(x)						// Turn DebugLog() off
	#endif
	
	#if DEBUGLEVEL > 3
		#define MACH_ASSERT		1				// To turn assert() on		
	#endif

#else

	#define DebugLog(x)							// Turn DebugLog() off
	
#endif


#include <kern/assert.h>		// for assert()


#if DEBUG

	#if DEBUGLEVEL > 5
	#define DebugAssert(x)			( void ) assert	x	// Turn DebugAssert() on
	#else
	#define DebugAssert(x)								// Turn DebugAssert() off
	#endif

#else

	#define DebugAssert(x)						// Turn DebugAssert() off
	
#endif

#ifdef __cplusplus
}
#endif


#endif // __APPLE_CDDA_FS_DEBUG_H__
