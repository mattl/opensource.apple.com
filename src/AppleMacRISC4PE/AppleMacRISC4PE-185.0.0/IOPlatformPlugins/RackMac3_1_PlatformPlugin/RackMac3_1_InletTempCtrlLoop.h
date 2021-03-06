/*
 * Copyright (c) 2003-2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 2003-2004 Apple Computer, Inc.  All rights reserved.
 *
 *  File: $Id: RackMac3_1_InletTempCtrlLoop.h,v 1.4 2004/03/18 02:18:52 eem Exp $
 *
 */


#ifndef _RACKMAC3_1_INLETTEMPCTRLLOOP_H
#define _RACKMAC3_1_INLETTEMPCTRLLOOP_H

#include "IOPlatformPIDCtrlLoop.h"

class RackMac3_1_InletTempCtrlLoop : public IOPlatformPIDCtrlLoop
{
    OSDeclareDefaultStructors(RackMac3_1_InletTempCtrlLoop)

protected:

    IOPlatformSensor * inputSensor2;
	SensorValue inletATemperature, inletBTemperature;
	
public:

    virtual IOReturn initPlatformCtrlLoop(const OSDictionary *dict);
    virtual void deadlinePassed( void );
    bool acquireSample( void );
};

#endif	// _RACKMAC3_1_INLETTEMPCTRLLOOP_H
