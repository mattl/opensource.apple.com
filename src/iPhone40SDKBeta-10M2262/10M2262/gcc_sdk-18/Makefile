##
# gcc_sdk Makefile
##

# Project info
Project               = gcc_sdk
ToolType              = Libraries

# Set these variables as needed, then include this file, then:
#
#  Install_Prefix        [ $(USRDIR)                              ]
#  Install_Source        [ $(NSSOURCEDIR)/Commands/$(ProjectName) ]
#
# Additional variables inherited from ReleaseControl/Common.make
##

ifndef CoreOSMakefiles
CoreOSMakefiles = $(MAKEFILEPATH)/CoreOS
endif

include $(CoreOSMakefiles)/ReleaseControl/Common.make

##
# My variables
##

Sources     = $(SRCROOT)/$(Project)

ifndef Install_Prefix
Install_Prefix = $(USRDIR)
endif
ifndef Install_Source
Install_Source = $(NSSOURCEDIR)/$(ToolType)/$(ProjectName)
endif

RC_Install_Prefix = $(DSTROOT)$(USRDIR)
ifneq ($(Install_Source),)
RC_Install_Source = $(DSTROOT)$(Install_Source)
endif

# -arch arguments are different than configure arguments. We need to
# translate them.

TRANSLATE_ARCH:=$(SED) -e s/ppc/powerpc/ -e s/i386/i686/

TARGETS := $(shell echo $(RC_ARCHS) | $(TRANSLATE_ARCH))

##
# Targets
##

.PHONY: almostclean

install:: build
	$(_v)- $(CHOWN) -R $(Install_User):$(Install_Group) $(DSTROOT)

build::
	$(_v) $(MKDIR) $(BuildDirectory)
	cd $(BuildDirectory) && $(SRCROOT)/sdk $(DSTROOT) "$(TARGETS)"

almostclean::

clean::
	rm -rf .svn
