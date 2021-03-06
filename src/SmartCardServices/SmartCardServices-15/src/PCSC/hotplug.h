/*
 * Copyright (c) 2000-2002 Apple Computer, Inc. All Rights Reserved.
 * The contents of this file constitute Original Code as defined in and are
 * subject to the Apple Public Source License Version 1.2 (the 'License').
 * You may not use this file except in compliance with the License. Please
 * obtain a copy of the License at http://www.apple.com/publicsource and
 * read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. Please
 * see the License for the specific language governing rights and
 * limitations under the License.
 */

/******************************************************************

	MUSCLE SmartCard Development ( http://www.linuxnet.com )
	    Title  : hotplug.h
	    Package: pcsc lite
            Author : David Corcoran
            Date   : 10/25/00
	    License: Copyright (C) 2000 David Corcoran
	             <corcoran@linuxnet.com>
            Purpose: This provides a search API for hot pluggble
	             devices.
	            
********************************************************************/

#ifndef __hotplug_h__
#define __hotplug_h__

#ifdef __cplusplus
extern "C"
{
#endif

        /* Function for registering for asynchronous device 
           additions or removals.
        */
        
	LONG HPRegisterForHotplugEvents();
        
        
        /* Function which forces looking for devices and
           registering / deregistering them with pcscd
        */
        
	LONG HPSearchHotPluggables();

#ifdef __cplusplus
}
#endif

#endif
