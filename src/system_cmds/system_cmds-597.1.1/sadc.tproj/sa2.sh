#!/bin/sh
# /usr/lib/sa/sa2.sh
# Portions Copyright (c) 1999-2003 Apple Computer, Inc. All Rights
# Reserved.
#
# This file contains Original Code and/or Modifications of Original Code
# as defined in and that are subject to the Apple Public Source License
# Version 2.0 (the 'License'). You may not use this file except in
# compliance with the License. Please obtain a copy of the License at
# http://www.opensource.apple.com/apsl/ and read it before using this
# file.
# 
# The Original Code and all software distributed under the License are
# distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
# INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
# Please see the License for the specific language governing rights and
# limitations under the License.
#
umask 0022
DATE=`/bin/date  +%d`
RPT=/var/log/sa/sar${DATE}
ENDIR=/usr/bin
DFILE=/var/log/sa/sa${DATE}
[ -f "$DFILE" ] || exit 0
cd ${ENDIR}
${ENDIR}/sar $* -f ${DFILE} > ${RPT}
/usr/bin/find /var/log/sa \( -name 'sar??' -o -name 'sa??' \) -mtime +7 -exec /bin/rm -f {} \;
