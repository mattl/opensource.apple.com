##
# Makefile for Twisted
##

# Project info
Project               = Twisted
ProjectName           = Twisted
UserType              = Developer
ToolType              = Library

PythonHome	= $(shell python -c 'import site; print site.here')

# Automatic Extract & Patch
AEP		= YES
AEP_Project	= $(Project)
AEP_Version	= 2.0.1
AEP_ProjVers	= $(AEP_Project)-$(AEP_Version)
AEP_Filename	= $(AEP_ProjVers).tar.bz2
AEP_ExtractDir	= $(AEP_ProjVers)
AEP_Patches	=
#AEP_Patches    = proxy65.patch

ifeq ($(suffix $(AEP_Filename)),.bz2)
    AEP_ExtractOption = j
else
    AEP_ExtractOption = z
endif

# Open Source configuration directories
OSV	= /usr/local/OpenSourceVersions
OSL	= /usr/local/OpenSourceLicenses

# Include common makefile targets for B&I
include $(MAKEFILEPATH)/CoreOS/ReleaseControl/Common.make


install:: install-strip install-plist

extract_source::
ifeq ($(AEP),YES)
	@echo "Extracting source for $(Project)..."
	$(TAR) -C $(OBJROOT) -$(AEP_ExtractOption)xf $(SRCROOT)/$(AEP_Filename)
	$(RMDIR) $(OBJROOT)/$(AEP_Project)
	$(MV) $(OBJROOT)/$(AEP_ExtractDir) $(OBJROOT)/$(AEP_Project)
	for patchfile in $(AEP_Patches); do \
	   echo $$patchfile; \
	   cd $(OBJROOT)/$(AEP_Project) && patch -lp0 < $(SRCROOT)/patches/$$patchfile; \
	done
endif

install-build: extract_source
	$(_v) cd $(OBJROOT)/$(AEP_Project) && CFLAGS="$(CFLAGS)" python setup.py install --root $(DSTROOT) --install-lib $(PythonHome)

install-strip: install-build
	$(STRIP) -Sx $(DSTROOT)/$(PythonHome)/twisted/internet/cfsupport.so
	$(STRIP) -Sx $(DSTROOT)/$(PythonHome)/twisted/protocols/_c_urlarg.so
	$(STRIP) -Sx $(DSTROOT)/$(PythonHome)/twisted/spread/cBanana.so

install-plist:
	$(MKDIR) $(DSTROOT)/$(OSV)
	$(INSTALL_FILE) $(SRCROOT)/$(ProjectName).plist $(DSTROOT)/$(OSV)/$(ProjectName).plist
	$(MKDIR) $(DSTROOT)/$(OSL)
	$(INSTALL_FILE) $(OBJROOT)/$(AEP_Project)/LICENSE $(DSTROOT)/$(OSL)/$(ProjectName).txt

clean::
	$(_v) if [ -d $(OBJROOT)/$(AEP_Project) ]; then \
	    cd $(OBJROOT)/$(AEP_Project) && python setup.py clean; \
	fi
