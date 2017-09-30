##
# Makefile for Apple Release Control (Archive Extraction & Patch)
#
# Copyright (c) 2005 Apple Computer, Inc.
#
# @APPLE_LICENSE_HEADER_START@
# 
# Portions Copyright (c) 2005 Apple Computer, Inc.  All Rights
# Reserved.  This file contains Original Code and/or Modifications of
# Original Code as defined in and that are subject to the Apple Public
# Source License Version 1.1 (the "License").  You may not use this file
# except in compliance with the License.  Please obtain a copy of the
# License at http://www.apple.com/publicsource and read it before using
# this file.
# 
# The Original Code and all software distributed under the License are
# distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
# INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
# License for the specific language governing rights and limitations
# under the License.
# 
# @APPLE_LICENSE_HEADER_END@
##
# This header must be included after GNUsource.make or Common.make
#
# Set these variables as needed before including this file:
#  AEP_Version - open source project version, used in archive name and
#                extracted directory
#  AEP_Patches - list of file names (from patches subdirectory) to
#                run thru patch and apply to extracted sources
#  AEP_LicenseFile - full path to license file
#  AEPBuildInSources - build in Sources directory instead of BuildDirectory
#
# The following variables will be defined if empty:
#  AEP_Project           [ $(Project)                             ]
#  AEP_Version           [ <no default>                           ]
#  AEP_ProjVers          [ $(AEP_Project)-$(AEP_Version)          ]
#  AEP_Filename          [ $(AEP_ProjVers).tar.gz                 ]
#  AEP_ExtractDir        [ $(AEP_ProVers)                         ]
#  AEP_Patches           [ <list of patch file to apply>          ]
#  AEP_LicenseFile       [ $(SRCROOT)/$(ProjectName).txt          ]
#
##

GnuAfterInstall += install-open-source-files


#
# Define AEP variables
#
ifndef AEP_Project
    AEP_Project		= $(Project)
endif

ifndef AEP_ProjVers
  ifdef AEP_Version
    AEP_ProjVers	= $(AEP_Project)-$(AEP_Version)
  else
    AEP_ProjVers	= $(AEP_Project)
  endif
endif

ifndef AEP_Filename
    AEP_Filename	= $(wildcard $(AEP_ProjVers).tar.[gb]z*)
endif
ifeq ($(suffix $(AEP_Filename)),.bz2)
    AEP_ExtractOption	= j
else
    AEP_ExtractOption	= z
endif

ifndef AEP_ExtractDir
    AEP_ExtractDir	= $(AEP_ProjVers)
endif
ifndef AEP_Patches
    AEP_Patches		=
endif

ifndef AEP_LicenseFile
    AEP_LicenseFile	= $(SRCROOT)/$(ProjectName).txt
endif


#AEP_ExtractRoot		= $(SRCROOT)
AEP_ExtractRoot		= $(OBJROOT)

# Redefine the Sources directory defined elsewhere
# ...but save the version of ConfigStamp (based on Sources)
# GNUSource.make uses.
GNUConfigStamp		:= $(ConfigStamp)
Sources			= $(AEP_ExtractRoot)/$(AEP_Project)

ifeq ($(AEP_BuildInSources),YES)
    BuildDirectory	= $(Sources)
endif


# Open Source configuration directories
OSVDIR	= /usr/local/OpenSourceVersions
OSLDIR	= /usr/local/OpenSourceLicenses


#
# AEP targets
#
.PHONY: extract-source install-open-source-files

$(GNUConfigStamp): extract-source

extract-source::
ifeq ($(AEP),YES)
	@echo "Extracting source for $(Project)..."
	$(MKDIR) $(AEP_ExtractRoot)
	$(TAR) -C $(AEP_ExtractRoot) -$(AEP_ExtractOption)xf $(SRCROOT)/$(AEP_Filename)
	$(RMDIR) $(Sources)
	$(_v) $(RM) $(GNUConfigStamp)
	$(MV) $(AEP_ExtractRoot)/$(AEP_ExtractDir) $(Sources)
	for patchfile in $(AEP_Patches); do \
	   echo $$patchfile; \
	   cd $(Sources) && patch -lp1 < $(SRCROOT)/patches/$$patchfile; \
	done
endif

install-open-source-files::
	@echo "Installing Apple-internal open-source documentation..."
	$(MKDIR) $(DSTROOT)/$(OSVDIR)
	$(INSTALL_FILE) $(SRCROOT)/$(ProjectName).plist $(DSTROOT)/$(OSVDIR)/$(ProjectName).plist
ifneq ($(AEP_LicenseFile),)
	$(MKDIR) $(DSTROOT)/$(OSLDIR)
	$(INSTALL_FILE) $(AEP_LicenseFile) $(DSTROOT)/$(OSLDIR)/$(ProjectName).txt
endif

clean::
	$(_v) if [ -d $(Sources) ]; then \
	    cd $(Sources) && make clean; \
	fi
