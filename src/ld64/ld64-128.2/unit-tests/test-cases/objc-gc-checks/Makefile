##
# Copyright (c) 2007-2008 Apple Inc. All rights reserved.
#
# @APPLE_LICENSE_HEADER_START@
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
# @APPLE_LICENSE_HEADER_END@
##
TESTROOT = ../..
include ${TESTROOT}/include/common.makefile

SHELL = bash # use bash shell so we can redirect just stderr


IMAGE_INFO = "__image_info"

ifeq ($(ARCH),x86_64)
	IMAGE_INFO = "__objc_imageinfo"
endif

test: test-${FILEARCH}

test-i386: test-macosx
test-x86_64: test-macosx
test-arm:   test-good

#
# Validate that the linker catches illegal combinations of .o files 
# compiled with different GC settings. 
#

test-macosx:
	${CC} ${CCFLAGS} foo.m -c -o foo.o
	${FAIL_IF_BAD_OBJ} foo.o

	${CC} ${CCFLAGS} foo.m -c -o foo-gc.o -fobjc-gc
	${FAIL_IF_BAD_OBJ} foo-gc.o

	${CC} ${CCFLAGS} foo.m -c -o foo-gc-only.o -fobjc-gc-only
	${FAIL_IF_BAD_OBJ} foo-gc-only.o

	${CC} ${CCFLAGS} bar.m -c -o bar.o
	${FAIL_IF_BAD_OBJ} bar.o

	${CC} ${CCFLAGS} bar.m -c -o bar-gc.o -fobjc-gc
	${FAIL_IF_BAD_OBJ} bar-gc.o

	${CC} ${CCFLAGS} bar.m -c -o bar-gc-only.o -fobjc-gc-only
	${FAIL_IF_BAD_OBJ} bar-gc-only.o

	# check RR + RR -> RR
	${CC} ${CCFLAGS} foo.o bar.o runtime.c -dynamiclib -o libfoobar.dylib 
	${FAIL_IF_BAD_MACHO} libfoobar.dylib

	# check GC/RR + GC/RR -> GC/RR
	${CC} ${CCFLAGS} foo-gc.o bar-gc.o runtime.c -dynamiclib -o libfoobar.dylib 
	${FAIL_IF_BAD_MACHO} libfoobar.dylib
	otool -o libfoobar.dylib | grep -A4 _image | grep flags | grep 0x2  | ${FAIL_IF_EMPTY}

	# check GC + GC -> GC
	${CC} ${CCFLAGS} foo-gc-only.o bar-gc-only.o runtime.c -dynamiclib -o libfoobar.dylib 
	${FAIL_IF_BAD_MACHO} libfoobar.dylib
	otool -o libfoobar.dylib | grep -A4 _image | grep flags | grep 0x6  | ${FAIL_IF_EMPTY}

	# check RR + GC/RR -> RR
	${CC} ${CCFLAGS} foo.o bar-gc.o runtime.c -dynamiclib -o libfoobar.dylib 
	${FAIL_IF_BAD_MACHO} libfoobar.dylib
	otool -o libfoobar.dylib | grep -A4 _image | grep flags | grep 0x[26]  | ${FAIL_IF_STDIN}

	# check GC/RR + RR -> RR
	${CC} ${CCFLAGS} bar-gc.o foo.o runtime.c -dynamiclib -o libfoobar.dylib 
	${FAIL_IF_BAD_MACHO} libfoobar.dylib
	otool -o libfoobar.dylib | grep -A4 _image | grep flags | grep 0x[26]  | ${FAIL_IF_STDIN}

	# check GC + GC/RR -> GC
	${CC} ${CCFLAGS} foo-gc-only.o bar-gc.o runtime.c -dynamiclib -o libfoobar.dylib  
	${FAIL_IF_BAD_MACHO} libfoobar.dylib
	otool -o libfoobar.dylib | grep -A4 _image | grep flags | grep 0x6  | ${FAIL_IF_EMPTY}

	# check RR + GC -> error
	${FAIL_IF_SUCCESS} ${CC} ${CCFLAGS} foo.o bar-gc-only.o runtime.c -dynamiclib -o libfoobar.dylib 2> fail.log

	# check cmd line GC/RR, GC/RR + RR -> error
	${FAIL_IF_SUCCESS} ${CC} ${CCFLAGS} foo-gc.o foo.o runtime.c -dynamiclib -o libfoobar.dylib -Wl,-objc_gc 2> fail.log
	
	# check GC/RR + compaction
	${CC} ${CCFLAGS} foo-gc.o bar-gc.o runtime.c -dynamiclib -Wl,-objc_gc_compaction -o libfoobar.dylib 
	otool -o libfoobar.dylib | grep -A4 _image | grep flags | grep 0x12  | ${FAIL_IF_EMPTY}

	# check GC + compaction
	${CC} ${CCFLAGS} foo-gc-only.o bar-gc-only.o runtime.c -dynamiclib -Wl,-objc_gc_compaction -o libfoobar.dylib 
	otool -o libfoobar.dylib | grep -A4 _image | grep flags | grep 0x16  | ${FAIL_IF_EMPTY}

	# none + GC/RR-dylib -> none
	${CC} ${CCFLAGS} foo-gc.o runtime.c -dynamiclib -o libfoo.dylib 
	${CC} ${CCFLAGS} none.c libfoo.dylib -dynamiclib -o libnone.dylib
	size -l libnone.dylib | grep ${IMAGE_INFO} | ${FAIL_IF_STDIN}

	# none + GC-dylib -> none
	${CC} ${CCFLAGS} foo-gc-only.o runtime.c -dynamiclib -o libfoo.dylib 
	${CC} ${CCFLAGS} none.c libfoo.dylib -dynamiclib -o libnone.dylib
	size -l libnone.dylib | grep ${IMAGE_INFO} | ${FAIL_IF_STDIN}

	# none + RR-dylib -> none
	${CC} ${CCFLAGS} foo.o runtime.c -dynamiclib -o libfoo.dylib 
	${CC} ${CCFLAGS} none.c libfoo.dylib -dynamiclib -o libnone.dylib
	size -l libnone.dylib | grep ${IMAGE_INFO} | ${FAIL_IF_STDIN}

	# check RR + GC-dylib -> error
	${CC} ${CCFLAGS} foo-gc-only.o runtime.c -dynamiclib -o libfoo.dylib 
	${FAIL_IF_SUCCESS} ${CC} ${CCFLAGS} bar.o runtime.c -dynamiclib libfoo.dylib -o libbar.dylib 2> fail.log

	# check GC + RR-dylib -> error
	${CC} ${CCFLAGS} foo.o runtime.c -dynamiclib -o libfoo.dylib 
	${FAIL_IF_SUCCESS} ${CC} ${CCFLAGS} bar-gc-only.o runtime.c -dynamiclib libfoo.dylib -o libbar.dylib 2> fail.log

	${PASS_IFF} true

test-good:
	${PASS_IFF} true

clean:
	rm -rf foo*.o bar*.o libfoobar.dylib fail.log libfoo.dylib libnone.dylib
