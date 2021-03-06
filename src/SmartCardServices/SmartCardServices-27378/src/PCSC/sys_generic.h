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
	Title  : sys_generic.h
	Package: pcsc lite
	Author : David Corcoran
	Date   : 11/8/99
	License: Copyright (C) 1999 David Corcoran
		<corcoran@linuxnet.com>
	Purpose: This handles abstract system level calls. 

$Id: sys_generic.h,v 1.2.80.1 2005/06/17 22:40:12 mb Exp $

********************************************************************/

#ifndef __sys_generic_h__
#define __sys_generic_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/stat.h>

	int SYS_Initialize();

	int SYS_Mkdir(char *, int);

	int SYS_GetPID();

	int SYS_Sleep(int);

	int SYS_USleep(int);

	int SYS_OpenFile(char *, int, int);

	int SYS_CloseFile(int);

	int SYS_RemoveFile(char *);

	int SYS_Chmod(const char *, int);

	int SYS_Chdir(const char *);

	int SYS_Mkfifo(const char *, int);

	int SYS_Mknod(const char *, int, int);

	int SYS_GetUID();

	int SYS_GetGID();

	int SYS_Chown(const char *, int, int);

	int SYS_ChangePermissions(char *, int);

	int SYS_LockFile(int);

	int SYS_LockAndBlock(int);

	int SYS_UnlockFile(int);

	int SYS_SeekFile(int, int);

	int SYS_ReadFile(int, char *, int);

	int SYS_WriteFile(int, char *, int);

	int SYS_GetPageSize(void);

	void *SYS_MemoryMap(int, int, int);

	void *SYS_PublicMemoryMap(int, int, int);

	int SYS_MMapSynchronize(void *, int);

	int SYS_MUnmap(void *begin, int length);

	int SYS_Fork();

#ifdef HAVE_DAEMON
	int SYS_Daemon(int, int);
#endif

	int SYS_Wait(int, int);

	int SYS_Stat(char *pcFile, struct stat *psStatus);

	int SYS_Fstat(int);

	int SYS_Random(int, float, float);

	int SYS_GetSeed();

	void SYS_Exit(int);

	int SYS_Rmdir(char *pcFile);

	int SYS_Unlink(char *pcFile);

#ifdef __cplusplus
}
#endif

#endif							/* __sys_generic_h__ */
