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

/*!
 * @header DSTCPConnection
 * Connection maintaining object implementation using DSTCPEndpoint.
 */

#include "DSTCPConnection.h"
#include "DSTCPEndpoint.h"
#include "DSEncryptedEndpoint.h"
#include "CLog.h"
#include "ServerControl.h"
#include "DSTCPListener.h"
#include "CHandlers.h"
#include "CMsgQueue.h"
#include "CRefTable.h"

#include <servers/bootstrap.h>
#include <sched.h>
#include <syslog.h>		// for syslog()


// --------------------------------------------------------------------------------
//	* Globals:
// --------------------------------------------------------------------------------

//API logging
extern UInt32				gAPICallCount;


//--------------------------------------------------------------------------------------------------
//	* DSTCPConnection()
//
//--------------------------------------------------------------------------------------------------

DSTCPConnection::DSTCPConnection ( DSTCPListener  *inParent )
	: CInternalDispatchThread( kTSTCPConnectionThread ),
	fTCPEndPt(nil),
	fMsgBlock(nil),
	mCurrentState(0),
	mConnectionType(kDSPXType),
	fParent(inParent),
	bFirstMsg(true)
{
} // DSTCPConnection



//--------------------------------------------------------------------------------------------------
//	* ~DSTCPConnection()
//
//--------------------------------------------------------------------------------------------------

DSTCPConnection::~DSTCPConnection()
{
	if ( fTCPEndPt != nil )
	{
		CRefTable::CleanClientRefs(fTCPEndPt->GetRemoteHostIPAddress(), (UInt32)fTCPEndPt );
		delete( fTCPEndPt );
		fTCPEndPt = nil;
	}
} // ~DSTCPConnection



//--------------------------------------------------------------------------------------------------
//	* StartThread()
//
//--------------------------------------------------------------------------------------------------

void DSTCPConnection::StartThread ( void )
{
	if ( this == nil ) throw((SInt32)eMemoryError);

	this->Resume();
} // StartThread



//--------------------------------------------------------------------------------------------------
//	* StopThread()
//
//--------------------------------------------------------------------------------------------------

void DSTCPConnection::StopThread ( void )
{
	if ( this == nil ) throw((SInt32)eMemoryError);

	SetThreadRunState( kThreadStop );		// Tell our thread to stop

} // StopThread



//--------------------------------------------------------------------------------------------------
//	* ThreadMain()
//
//--------------------------------------------------------------------------------------------------

SInt32 DSTCPConnection::ThreadMain ( void )
{
	bool		done		= false;
	SInt32		result		= eDSNoErr;

	while ( GetThreadRunState() != kThreadStop )
	{
        //this while will only go once since a throw below sets the kThreadStop
		try
		{
			if ( fTCPEndPt == nil )
			{
				DbgLog( kLogThreads, "File: %s. Line: %d", __FILE__, __LINE__ );
				ErrLog( kLogThreads, "  ***No TCP Endpoint provided." );
				StopThread();
				done = true;
			}

			result = ((DSEncryptedEndpoint*)fTCPEndPt)->ServerNegotiateKey();
			if ( result != eDSNoErr ) throw(result);

			while ( !done )
			{
				// Listen for incomming messages
				if ( ListenForMessage() == true )
				{
					if (fMsgBlock != nil)
					{
						CRequestHandler handler;
						handler.HandleRequest(&fMsgBlock);
						//send back reply using the TCP endpoint
						if (fMsgBlock != nil)
						{
							if (fTCPEndPt != nil )
							{
								(void)fTCPEndPt->SendClientReply( fMsgBlock );
								//only track external client API calls
								gAPICallCount++;
							}
							free(fMsgBlock);
							fMsgBlock = nil;
						}
					}
					//QueueMessage();//don't use the queue anymore since using direct dispatch
				}

				if ( GetThreadRunState() == kThreadStop )
				{
					done = true;
				}
			}
		}

		catch( SInt32 err )
		{
			DbgLog( kLogThreads, "File: %s. Line: %d", __FILE__, __LINE__ );
			DbgLog( kLogThreads, "  ***DSTCPConnection::ThreadMain error = %d", err );
			//notify the parent DSTCPListener that this thread is going away
			if (fParent != nil)
			{
				fParent->ConnectionClosed(this);
			}
			this->SetThreadRunState( kThreadStop );
		}
	}

	return( 0 );

} // ThreadMain


//--------------------------------------------------------------------------------------------------
//	* SetEndpoint()
//
//--------------------------------------------------------------------------------------------------

void DSTCPConnection::SetEndpoint ( DSTCPEndpoint *inTCPEndpoint )
{

	fTCPEndPt = inTCPEndpoint;

} // SetEndpoint


//--------------------------------------------------------------------------------------------------
//	* ListenForMessage()
//
//--------------------------------------------------------------------------------------------------

bool DSTCPConnection::ListenForMessage ( void )
{
	bool	gotOne	= false;
		
	fMsgBlock = (sComData *)fTCPEndPt->GetClientMessage();
	if ( fMsgBlock != nil )
	{
		gotOne = true;
		
		//from the endpoint let's find out the remote IP address and
		//then let's add it to the fMsgBlock for the handler threads and ref table to use
		fMsgBlock->fIPAddress = fTCPEndPt->GetRemoteHostIPAddress();
		
		//let's use the remote port for the remote PID value
		//since the actual remote PID value is of no value to us here when a connection goes away
		//so now we can tell the ref table to cleanup since we know when the connection drops
		//and we can call in with this value and the proper IP address ie. see destructor
		fMsgBlock->fPID = (UInt32)fTCPEndPt->GetRemoteHostPort();
		
		// note the first message received with a member Boolean
		//and then extract the remote IP process PID to give to the parent Listener
		//for stats
		if (bFirstMsg)
		{
			bFirstMsg = false;
			if (fParent != nil)
			{
				fParent->AddPIDForConnectionStat(this,fMsgBlock->fPID);
			}
		}
	}
	//if we get nil here then there is something wrong with TCP that we need to handle
	else
	{
		//notify the parent DSTCPListener that this thread is going away
		if (fParent != nil)
		{
			fParent->ConnectionClosed(this);
		}
		//stop this thread
		this->SetThreadRunState( kThreadStop );
	}

	return( gotOne );

} // ListenForMessage

//--------------------------------------------------------------------------------------------------
//	* QueueMessage()
//
//--------------------------------------------------------------------------------------------------

SInt32 DSTCPConnection::QueueMessage ( void )
{
    // no longer used
	return( eDSNoErr );

} // QueueMessage


