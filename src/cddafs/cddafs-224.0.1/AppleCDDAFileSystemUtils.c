/*
 * Copyright (c) 2000-2004 Apple Computer, Inc. All rights reserved.
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
 
// AppleCDDAFileSystemUtils.c created by CJS on Sun 14-May-2000

#ifndef __APPLE_CDDA_FS_UTILS_H__
#include "AppleCDDAFileSystemUtils.h"
#endif

#ifndef __APPLE_CDDA_FS_DEBUG_H__
#include "AppleCDDAFileSystemDebug.h"
#endif

#ifndef __APPLE_CDDA_FS_DEFINES_H__
#include "AppleCDDAFileSystemDefines.h"
#endif

#ifndef __AIFF_SUPPORT_H__
#include "AIFFSupport.h"
#endif

#ifndef __APPLE_CDDA_FS_VFS_OPS_H__
#include "AppleCDDAFileSystemVFSOps.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/stat.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <sys/malloc.h>
#include <sys/attr.h>
#include <sys/time.h>
#include <sys/ubc.h>
#include <sys/unistd.h>


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	Static Function Prototypes
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ


static int 		BuildTrackName 					( mount_t mountPtr, AppleCDDANodeInfoPtr nodeInfoPtr );
static UInt32	CalculateNumberOfDescriptors 	( const QTOCDataFormat10Ptr TOCDataPtr );
static UInt32	CalculateLBA 					( SubQTOCInfoPtr trackDescriptorPtr );
static int		FindName						( mount_t mountPtr, UInt8 trackNumber, char ** name, UInt8 * nameSize );


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	CreateNewCDDANode -	This routine is responsible for creating new nodes
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

int
CreateNewCDDANode ( mount_t 				mountPtr,
					UInt32 					nodeID,
					enum vtype 				vNodeType,
					vnode_t					parentVNodePtr,
					struct componentname *	compNamePtr,
					vnode_t * 				vNodeHandle )
{
	
	int 					result			= 0;
	AppleCDDANodePtr		cddaNodePtr		= NULL;
	vnode_t 				vNodePtr		= NULLVP;
	struct vnode_fsparam	vfsp;
	
	DebugAssert ( ( mountPtr != NULL ) );
	DebugAssert ( ( vNodeHandle != NULL ) );
	
	// Allocate the cddaNode
	MALLOC ( cddaNodePtr, AppleCDDANodePtr, sizeof ( AppleCDDANode ), M_TEMP, M_WAITOK );
	
	// Zero the structure
	bzero ( cddaNodePtr, sizeof ( AppleCDDANode ) );
	
	// Set the nodeID
	cddaNodePtr->nodeID = nodeID;
	
	// Zero the FS param structure
	bzero ( &vfsp, sizeof ( vfsp ) );
	
	vfsp.vnfs_mp		= mountPtr;
	vfsp.vnfs_vtype 	= vNodeType;
	vfsp.vnfs_str 		= "cddafs";
	vfsp.vnfs_dvp 		= parentVNodePtr;
	vfsp.vnfs_fsnode 	= cddaNodePtr;
	vfsp.vnfs_cnp 		= compNamePtr;
	vfsp.vnfs_vops 		= gCDDA_VNodeOp_p;
	vfsp.vnfs_rdev 		= 0;
	
	if ( ( parentVNodePtr != NULL ) && ( compNamePtr != NULL ) && ( compNamePtr->cn_flags & MAKEENTRY ) )
		vfsp.vnfs_flags = 0;
	else
		vfsp.vnfs_flags = VNFS_NOCACHE;
	
	vfsp.vnfs_markroot 	 = ( nodeID == kAppleCDDARootFileID );
	vfsp.vnfs_marksystem = 0;
	
	// Note that vnode_create ( ) returns the vnode with an iocount of +1;
	// this routine returns the newly created vnode with this positive iocount.
	result = vnode_create ( VNCREATE_FLAVOR, VCREATESIZE, &vfsp, &vNodePtr );
	if ( result != 0 )
	{
		
		DebugLog ( ( "getnewvnode failed with error code %d\n", result ) );
		goto FREE_CDDA_NODE_ERROR;
		
	}
	
	// Link the cddaNode to the vnode
	cddaNodePtr->vNodePtr = vNodePtr;
	
	vnode_addfsref ( vNodePtr );
	
	// Return the vnode to the caller
	*vNodeHandle = vNodePtr;
	
	return result;
	
	
FREE_CDDA_NODE_ERROR:
	
	
	// Free the allocated memory
	FREE ( ( caddr_t ) cddaNodePtr, M_TEMP );
	cddaNodePtr = NULL;
	
	return result;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	DisposeCDDANode -	This routine is responsible for cleaning up cdda nodes
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

int
DisposeCDDANode ( vnode_t vNodePtr )
{
	
	AppleCDDANodePtr	cddaNodePtr = NULL;
	
	DebugAssert ( ( vNodePtr != NULL ) );
	
	cddaNodePtr = VTOCDDA ( vNodePtr );
	
	DebugAssert ( ( cddaNodePtr != NULL ) );
	
	if ( cddaNodePtr != NULL )
	{
		
		// Free memory associated with our filesystem's internal data
		FREE ( vnode_fsnode ( vNodePtr ), M_TEMP );
		vnode_clearfsnode ( vNodePtr );
		
	}
	
	return ( 0 );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	CreateNewCDDAFile -	This routine is responsible for creating new
//						files
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

int
CreateNewCDDAFile ( mount_t 				mountPtr,
					UInt32 					nodeID,
					AppleCDDANodeInfoPtr 	nodeInfoPtr,
					vnode_t					parentVNodePtr,
					struct componentname *	compNamePtr,
					vnode_t * 				vNodeHandle )
{
	
	int						result				= 0;
	vnode_t 				vNodePtr			= NULLVP;
	AppleCDDANodePtr		cddaNodePtr			= NULL;
	AppleCDDANodePtr		parentCDDANodePtr	= NULL;
	AppleCDDAMountPtr		cddaMountPtr		= NULL;
	
	DebugAssert ( ( mountPtr != NULL ) );
	DebugAssert ( ( nodeInfoPtr != NULL ) );
	DebugAssert ( ( vNodeHandle != NULL ) );
	
	cddaMountPtr		= VFSTOCDDA ( mountPtr );
	parentCDDANodePtr	= VTOCDDA ( cddaMountPtr->root );
	
	DebugAssert ( ( cddaMountPtr != NULL ) );
	DebugAssert ( ( parentCDDANodePtr != NULL ) );
	
	result = CreateNewCDDANode ( mountPtr, nodeID, VREG, parentVNodePtr, compNamePtr, &vNodePtr );
	if ( result != 0 )
	{
		
		DebugLog ( ( "Error = %d returned from CreatNewCDDANode\n", result ) );
		return result;
		
	}
	
	cddaNodePtr = VTOCDDA ( vNodePtr );
	
	DebugAssert ( ( cddaNodePtr != NULL ) );
	
	// Build the header.
	BuildCDAIFFHeader ( &cddaNodePtr->u.file.aiffHeader, nodeInfoPtr->numBytes );
	
	// Fill in the miscellaneous fields for the cddaNode
	cddaNodePtr->nodeType 				= kAppleCDDATrackType;
	cddaNodePtr->blockDeviceVNodePtr	= parentCDDANodePtr->blockDeviceVNodePtr;
	
	// Set the back pointer
	cddaNodePtr->u.file.nodeInfoPtr	= nodeInfoPtr;
	
	DebugLog ( ( "LBA of %d = %ld.\n", cddaNodePtr->nodeID, nodeInfoPtr->LBA ) );
	
	// stuff the vNode in
	*vNodeHandle = vNodePtr;
	
	return 0;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	CreateNewXMLFile -	This routine is responsible for creating the ".TOC.plist"
//						file which has XML data describing the CD layout.
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

int
CreateNewXMLFile ( 	mount_t 				mountPtr,
					UInt32 					xmlFileSize,
					UInt8 * 				xmlData,
					vnode_t					parentVNodePtr,
					struct componentname *	compNamePtr,
					vnode_t * 				vNodeHandle )
{
	
	int						result				= 0;
	vnode_t 				vNodePtr			= NULLVP;
	AppleCDDANodePtr		cddaNodePtr			= NULL;
	AppleCDDANodePtr		parentCDDANodePtr	= NULL;
	AppleCDDAMountPtr		cddaMountPtr		= NULL;
	
	DebugAssert ( ( mountPtr != NULL ) );
	DebugAssert ( ( vNodeHandle != NULL ) );

	cddaMountPtr		= VFSTOCDDA ( mountPtr );
	parentCDDANodePtr	= VTOCDDA ( cddaMountPtr->root );

	DebugAssert ( ( cddaMountPtr != NULL ) );
	DebugAssert ( ( parentCDDANodePtr != NULL ) );
	
	result = CreateNewCDDANode ( mountPtr, kAppleCDDAXMLFileID, VREG, parentVNodePtr, compNamePtr, &vNodePtr );
	if ( result != 0 )
	{
		
		DebugLog ( ( "Error = %d returned from CreatNewCDDANode\n", result ) );
		return result;
		
	}
	
	cddaNodePtr = VTOCDDA ( vNodePtr );
	
	DebugAssert ( ( cddaNodePtr != NULL ) );
	
	// Fill in the miscellaneous fields for the cddaNode
	cddaNodePtr->nodeType 				= kAppleCDDAXMLFileType;
	cddaNodePtr->blockDeviceVNodePtr	= parentCDDANodePtr->blockDeviceVNodePtr;
	
	// Point the xmlData to the correct place
	cddaNodePtr->u.xmlFile.fileDataPtr 	= xmlData;
	cddaNodePtr->u.xmlFile.fileSize 	= xmlFileSize;
	
	#if 0
	{
		UInt32	count;
		// Let's see if we got the right data mapped in
		for ( count = 0; count < xmlFileSize; count = count + 8 )
		{
			
			DebugLog ( ( "%x:%x:%x:%x %x:%x:%x:%x\n",
						xmlData[count],
						xmlData[count+1],
						xmlData[count+2],
						xmlData[count+3],
						xmlData[count+4],
						xmlData[count+5],
						xmlData[count+6],
						xmlData[count+7] ) );
			
		}
		
		DebugLog ( ( "\n" ) );
		
	}
	#endif
	
	// stuff the vNode in
	*vNodeHandle = vNodePtr;
	
	return 0;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	CreateNewCDDADirectory -	This routine is responsible for creating new
//								directories (i.e. the root directory)
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

int
CreateNewCDDADirectory ( mount_t 		mountPtr,
						 UInt32 		nodeID,
						 vnode_t * 		vNodeHandle )
{
	
	int						result			= 0;
	vnode_t					vNodePtr		= NULLVP;
	AppleCDDANodePtr		cddaNodePtr		= NULL;
	
	DebugAssert ( ( mountPtr != NULL ) );
	DebugAssert ( ( vNodeHandle != NULL ) );

	result = CreateNewCDDANode ( mountPtr, nodeID, VDIR, NULL, NULL, &vNodePtr );
	if ( result != 0 )
	{
		
		DebugLog ( ( "Error = %d returned from CreatNewCDDANode\n", result ) );
		return result;
		
	}
		
	cddaNodePtr = VTOCDDA ( vNodePtr );
	DebugAssert ( ( cddaNodePtr != NULL ) );
	
	// Set up the directory-specific fields
	cddaNodePtr->nodeType 					= kAppleCDDADirectoryType;
	cddaNodePtr->u.directory.directorySize	= 0;			
	cddaNodePtr->u.directory.entryCount 	= kNumberOfFakeDirEntries; 		// ".", "..", and ".TOC.plist"
	
	// stuff the vNode in
	*vNodeHandle = vNodePtr;
	
	return 0;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	IsAudioTrack -	Checks the arguments passed in to find out if specified
//					track is audio or not
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

boolean_t
IsAudioTrack ( const SubQTOCInfoPtr trackDescriptorPtr )
{
	
	DebugAssert ( ( trackDescriptorPtr != NULL ) );

	// Check to make sure the point is between 1 and 99 (inclusive)
	if ( trackDescriptorPtr->point < 100 && trackDescriptorPtr->point > 0 )
	{
		
		// Do we have digital data?
		if ( ( trackDescriptorPtr->control & kDigitalDataMask ) == 0 )
		{
			
			// Found an audio track
			return TRUE;
			
		}
		
	}
	
	return FALSE;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	CalculateSize -	Calculate the file size based on number of frames
//					(i.e. blocks) in the track
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt32
CalculateSize ( const QTOCDataFormat10Ptr 	TOCDataPtr,
				UInt32 						trackDescriptorOffset,
				UInt32 						currentA2Offset )
{

	UInt32				size					= 0;
	UInt32				offset					= 0;
	UInt32				numberOfDescriptors		= 0;
	UInt32				nextOffset				= 0;
	SubQTOCInfoPtr		trackDescriptorPtr		= NULL;
	SubQTOCInfoPtr		nextTrackDescriptorPtr	= NULL;
	
	DebugLog ( ( "CalculateSize: Entering...\n" ) );
	
	DebugAssert ( ( TOCDataPtr != NULL ) );
	
	// Find the number of descriptors
	numberOfDescriptors = CalculateNumberOfDescriptors ( TOCDataPtr );
	
	// Get the correct track descriptor
	trackDescriptorPtr = &TOCDataPtr->trackDescriptors[trackDescriptorOffset];
	
	// Are we past the total number of descriptors in TOC?
	if ( trackDescriptorOffset + 1 >= numberOfDescriptors )
	{
	
		// yes, so set the descriptor to the last leadout descriptor we hit 
		nextTrackDescriptorPtr = &TOCDataPtr->trackDescriptors[currentA2Offset];
	
	}
	
	else
	{

		// no, so set the descriptor to the next entry in the TOC
		nextTrackDescriptorPtr = &TOCDataPtr->trackDescriptors[trackDescriptorOffset + 1];
		
		// Are we past the end of the session?
		if ( trackDescriptorPtr->sessionNumber != nextTrackDescriptorPtr->sessionNumber ||
			 !IsAudioTrack ( nextTrackDescriptorPtr ) )
		{
			
			// yes, so set the descriptor to the last leadout descriptor we hit 
			nextTrackDescriptorPtr = &TOCDataPtr->trackDescriptors[currentA2Offset];
		
		}
		
	}
	
	// Calculate the LBAs for both tracks
	offset 		= CalculateLBA ( trackDescriptorPtr );
	nextOffset 	= CalculateLBA ( nextTrackDescriptorPtr );
	
	// Multiply number of blocks by block size and add the header + pad (1 block)
	size = ( ( nextOffset - offset ) * kPhysicalMediaBlockSize ) + kPhysicalMediaBlockSize;
	
	DebugLog ( ( "CalculateSize: size = %ld.\n", size ) );
	
	DebugLog ( ( "CalculateSize: exiting...\n" ) );
	
	return size;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	FindName - 	Parses the names data that gets passed in to the
//				filesystem, looking for the specified track's name.
//				All names look like the following packed structure:
//
//	| 	1 byte 		| 		1 byte 		| 	number of bytes in 2nd byte		|
//		Track #	 		size of String			String for Name
//
//	Track # of zero corresponds to the album name (the mount point name).
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ


int
FindName ( mount_t mountPtr, UInt8 trackNumber, char ** name, UInt8 * nameSize )
{
	
	AppleCDDAMountPtr		cddaMountPtr		= NULL;
	UInt8 *					ptr					= NULL;
	UInt8					length				= 0;
	
	DebugLog ( ( "FindName: entering\n" ) );
	DebugLog ( ( "trackNumber = %d\n" ) );
	
	DebugAssert ( ( mountPtr != NULL ) );
	DebugAssert ( ( name != NULL ) );
	DebugAssert ( ( nameSize != NULL ) );
	
	cddaMountPtr = VFSTOCDDA ( mountPtr );
	DebugLog ( ( "cddaMountPtr->nameDataSize = %ld\n", cddaMountPtr->nameDataSize ) );
	
	ptr = cddaMountPtr->nameData;
	
	if ( ptr == NULL )
	{
		
		DebugLog ( ( "cddaMountPtr->nameData is NULL" ) );
		return ENOENT;
		
	}
	
	do
	{
				
		DebugLog ( ( "*ptr = %d\n", *ptr ) );
		
		if ( *ptr == trackNumber )
		{
			
			char	mylocalname[512];
			
			DebugLog ( ( "Found track = %d\n", trackNumber ) );
			
			*nameSize 	= ptr[1];
			*name 		= ( char * ) &ptr[2];
			
			bcopy ( &ptr[2], mylocalname, *nameSize );
			mylocalname[*nameSize] = 0;
			
			DebugLog ( ( "NameSize = %d\n", *nameSize ) );
			DebugLog ( ( "Name = %s\n", mylocalname ) );
			
			break;
			
		}
		
		else
		{
			
			length = ptr[1];
			
			DebugLog ( ( "Didn't find it, keep looking\n" ) );
			ptr = &ptr[length + 2];
			
		}
		
	} while ( ptr < ( cddaMountPtr->nameData + cddaMountPtr->nameDataSize ) );
	
	DebugLog ( ( "FindName: exiting\n" ) );
	
	return 0;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	ParseTOC - 	Parses the TOC to find audio tracks. It figures out which
//				tracks are audio and what their offsets are and fills in the
//				cddaNode structures associated with each vnode
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SInt32
ParseTOC ( mount_t 	mountPtr,
		   UInt32 	numTracks )
{

	QTOCDataFormat10Ptr			TOCDataPtr			= NULL;
	SubQTOCInfoPtr				trackDescriptorPtr	= NULL;
	AppleCDDAMountPtr			cddaMountPtr		= NULL;
	AppleCDDANodeInfoPtr		nodeInfoPtr			= NULL;
	AppleCDDADirectoryNodePtr	rootDirNodePtr		= NULL;
	OSStatus					error				= 0;
	UInt16						numberOfDescriptors = 0;
	UInt32						currentA2Offset		= 0;
	UInt32						currentOffset		= 0;
	
	DebugLog ( ( "ParseTOC: Entering...\n" ) );
	
	DebugAssert ( ( mountPtr != NULL ) );
	
	cddaMountPtr 	= VFSTOCDDA ( mountPtr );
	rootDirNodePtr	= &( ( VTOCDDA ( cddaMountPtr->root ) )->u.directory );
	
	DebugAssert ( ( cddaMountPtr != NULL ) );
	DebugAssert ( ( rootDirNodePtr != NULL ) );

	// Get the data from the registry entry
	TOCDataPtr = CreateBufferFromIORegistry ( mountPtr );
	
	if ( TOCDataPtr != NULL )
	{
		
		// calculate number of track descriptors
		numberOfDescriptors = CalculateNumberOfDescriptors ( TOCDataPtr );
		DebugLog ( ( "Number of descriptors = %d\n", numberOfDescriptors ) );

		if ( numberOfDescriptors <= 0 )
		{
			
			// This is bad...no track descriptors, time to bail
			error = EINVAL;
			goto Exit;
			
		}
		
		trackDescriptorPtr = TOCDataPtr->trackDescriptors;
		
		while ( numberOfDescriptors > 0 && rootDirNodePtr->entryCount < ( numTracks + kNumberOfFakeDirEntries ) )
		{
			
			if ( trackDescriptorPtr->point == 0xA2 )
			{
				
				// Set the a2 offset when we find an a2 point
				currentA2Offset = currentOffset;
				
			}
			
			// Is this an audio track?
			if ( IsAudioTrack ( trackDescriptorPtr ) )
			{
				
				// Make this easier to read by getting a pointer to the nodeInfo
				nodeInfoPtr = &cddaMountPtr->nodeInfoArrayPtr[rootDirNodePtr->entryCount - kNumberOfFakeDirEntries];
				
				// Copy this trackDescriptor into the AppleCDDANodeInfo array
				nodeInfoPtr->trackDescriptor = *trackDescriptorPtr;				
				
				// Get the LogicalBlockAddress and number of bytes in the track
				nodeInfoPtr->LBA 		= CalculateLBA ( trackDescriptorPtr );
				nodeInfoPtr->numBytes 	= CalculateSize ( TOCDataPtr, currentOffset, currentA2Offset );
				
				// Add this node's size to the root directory's directorySize field
				rootDirNodePtr->directorySize += nodeInfoPtr->numBytes;
				
				// Increment the number of audio tracks
				rootDirNodePtr->entryCount++;
				
				( void ) BuildTrackName ( mountPtr, nodeInfoPtr );
				
				DebugLog ( ( "LBA of %d = %ld.\n", trackDescriptorPtr->point, nodeInfoPtr->LBA ) );
				
			}
			
			// Advance the pointers and decrement the count
			trackDescriptorPtr++;
			numberOfDescriptors--;
			currentOffset++;
			
		}
		
		if ( ( numberOfDescriptors != 0 ) && ( rootDirNodePtr->entryCount == ( numTracks + kNumberOfFakeDirEntries )  ) )
		{
			
			// Oops...the parsing routine in userland must've screwed up
			DebugLog ( ( "ParseTOC: userland utility sent wrong number of audio tracks in at mount time.\n" ) );
			
		}
		
	}
	
	else
	{
		
		// Couldn't parse the TOC, so return an error
		return ENOMEM;
		
	}
	
	
Exit:
	
	
	if ( TOCDataPtr != NULL )
	{
		
		// Free the buffer allocated earlier
		DisposeBufferFromIORegistry ( TOCDataPtr );
		
	}
	
	DebugLog ( ( "ParseTOC: exiting...\n" ) );
	
	return error;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	BuildTrackName -	This routine is responsible for building a track
//						name based on its number
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

int
BuildTrackName ( mount_t mountPtr, AppleCDDANodeInfoPtr nodeInfoPtr )
{
	
	UInt8		trackNumber	= 0;
	char *		name		= NULL;
	UInt8		nameSize	= 0;
	int			error		= 0;
	
	DebugLog ( ( "BuildTrackName: entering.\n" ) );
	
	DebugAssert ( ( nodeInfoPtr != NULL ) );
	
	// Get the track number for which to find the name
	trackNumber = nodeInfoPtr->trackDescriptor.point;
	
	// Find the name
	error = FindName ( mountPtr, trackNumber, &name, &nameSize );
	if ( error != 0 )
	{
		
		DebugLog ( ( "cddafs : FindName returned error = %d\n", error ) );
		// Buffer copied in was formatted incorrectly. We'll just use
		// track numbers on this CD.
		
	}
	
	// Set the size of the name
	nodeInfoPtr->nameSize = nameSize;
	
	// If we got here, then we have a valid track and the nodeInfoArrayPtr points to our
	// offset into the array. So, MALLOC the name here
	MALLOC ( nodeInfoPtr->name, char *, nodeInfoPtr->nameSize + 1, M_TEMP, M_WAITOK ); 
	
	// Copy the name
	bcopy ( name, &nodeInfoPtr->name[0], nameSize );
	
	// Don't forget NULL byte
	nodeInfoPtr->name[nameSize] = 0;
	
	DebugLog ( ( "BuildTrackName: fileName = %s\n", nodeInfoPtr->name ) );
	
	return 0;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	CalculateNumberOfDescriptors -	Calculate the number of SubQTOCInfo entries
//									in the given TOC
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt32
CalculateNumberOfDescriptors ( const QTOCDataFormat10Ptr TOCDataPtr )
{
	
	UInt32		numberOfDescriptors = 0;
	UInt32		length				= 0;
	
	DebugLog ( ( "CalculateNumberOfDescriptors: Entering...\n" ) );
	
	DebugAssert ( ( TOCDataPtr != NULL ) );
	
	// Get the length of the TOC
	length = OSSwapBigToHostInt16 ( TOCDataPtr->TOCDataLength );
	
	// Remove the first and last session numbers so all we are left with are track descriptors
	length -= ( sizeof ( TOCDataPtr->firstSessionNumber ) +
				sizeof ( TOCDataPtr->lastSessionNumber ) );
	
	// Divide the length by the size of a single track descriptor to get total number
	numberOfDescriptors = length / ( sizeof ( SubQTOCInfo ) );
	
	DebugLog ( ( "CalculateNumberOfDescriptors: exiting...\n" ) );
	
	return numberOfDescriptors;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	CalculateLBA -	Convert the frames offset of the file (from the TOC) to
//					the LBA of the device
//
// NB: this is a workaround because the first 2 seconds of a CD are unreadable
// and defined as off-limits. So we convert our absolute MSF to an actual
// logical block which can be addressed through the BSD layer.
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt32
CalculateLBA ( SubQTOCInfoPtr trackDescriptorPtr )
{
	
	DebugAssert ( ( trackDescriptorPtr != NULL ) );
	
	// Simply convert MSF to LBA
	return ( ( trackDescriptorPtr->PMSF.startPosition.minutes * kSecondsPerMinute + 
	  		   trackDescriptorPtr->PMSF.startPosition.seconds ) * kFramesPerSecond +
	  		   trackDescriptorPtr->PMSF.startPosition.frames - kMSFToLBA );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//				End				Of			File
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
