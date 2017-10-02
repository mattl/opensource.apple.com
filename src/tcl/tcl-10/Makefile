##
# Makefile for Tcl
##

# Project info
Project               = tcl
UserType              = Developer
ToolType              = Commands
Configure             = $(Sources)/unix/configure
Extra_Environment     = TCL_LIBRARY="$(NSLIBRARYDIR)/Tcl/$(Version)"	\
			TCL_EXE="$(Tclsh)"
GnuAfterInstall       = int-headers links old-tcllib

# It's a GNU Source project
include $(MAKEFILEPATH)/CoreOS/ReleaseControl/GNUSource.make

Install_Flags = INSTALL_ROOT="$(DSTROOT)" INSTALL_STRIP_LIBRARY="-S -S"

Version = $(shell $(GREP) "TCL_VERSION=" "$(Configure)" | $(CUT) -d '=' -f 2)
Tclsh   = $(shell find /usr/bin -name tcl\* | tail -1)

links:
	$(_v) $(LN) -fs "tclsh$(Version)" "$(DSTROOT)$(USRBINDIR)/tclsh"
	$(_v) $(LN) -fs "libtcl$(Version).dylib" "$(DSTROOT)$(USRLIBDIR)/libtcl.dylib"
	$(_v) $(LN) -fs "tclsh.1" "$(DSTROOT)$(MANDIR)/man1/tclsh$(Version).1"

int-headers:
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)/usr/local/include"
	$(_v) $(INSTALL_FILE) "$(Sources)/generic/tclInt.h" "$(DSTROOT)/usr/local/include"
	$(_v) $(INSTALL_FILE) "$(Sources)/generic/tclIntDecls.h" "$(DSTROOT)/usr/local/include"
	$(_v) $(INSTALL_FILE) "$(Sources)/generic/tclIntPlatDecls.h" "$(DSTROOT)/usr/local/include/"
	$(_v) $(INSTALL_FILE) "$(Sources)/unix/tclUnixPort.h" "$(DSTROOT)/usr/local/include/tclPort.h"

# 3280206
old-tcllib:
	$(_v) cd $(BuildDirectory) && /usr/bin/uudecode $(SRCROOT)/libtcl8.3.dylib.uue
	$(_v) $(INSTALL_DYLIB) "$(BuildDirectory)/libtcl8.3.dylib" "$(DSTROOT)/usr/lib/"
