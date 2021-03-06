/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
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

#ifndef _COBJECT_H
#define _COBJECT_H

#include <libkern/OSAtomic.h>
#include <stdlib.h>

template <class ClassType> class CObject
{
	public:
		int32_t	RetainCount( void ) const
		{
			return mRefCount;
		}
	
		ClassType *Retain( void )
		{
			if ( 0 < mRefCount )
			{
				OSAtomicAdd32Barrier( 1, &mRefCount );
				return static_cast<ClassType *>( this );
			}
			else
			{
				abort();	// we abort so we can get crashes
			}
			
			return NULL;
		}
	
		void Release( void )
		{
			int32_t newCount = OSAtomicAdd32Barrier( -1, &mRefCount );
			if ( newCount == 0 ) delete this;
			else if ( newCount < 0 ) abort(); // we abort so we can get crashes
		}
	
	protected:
		CObject( void )
		{
			mRefCount = 1;	// we default to refcount of 1 when something is created
		}

		virtual	~CObject( void )
		{
			// not allowed to delete the object from outside
		}
	
	private:
		int32_t	mRefCount;	// our Refcount
};

#endif
