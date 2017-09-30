##
# Makefile for net-snmp
##

# General project info
Project		= net-snmp
ProjectName	= net_snmp
UserType	= Administration
ToolType	= Commands
Submission	= 19.1

#
# Settings for the net-snmp project.
#
Extra_Configure_Flags	= --disable-dependency-tracking \
			--prefix=/usr \
			--sysconfdir=/etc \
			--with-persistent-directory=/var/db/net-snmp \
			--with-defaults \
			--with-mib-modules=host \
			--with-out-mib-modules="mibII/icmp host/hr_swrun" \
			--with-sys-contact="postmaster@example.com" \
			--enable-mini-agent \
			--without-kmem-usage
#			--enable-developer
#			--enable-ipv6

Extra_CC_Flags		= -DBUILD=$(Submission) \
			-DMACOSX_DEPLOYMENT_TARGET=10.4 \
			-I/System/Library/Frameworks/System.framework/PrivateHeaders
Extra_Environment	= AR="$(SRCROOT)/ar.sh"
GnuAfterInstall		= install-macosx install-startup install-man

# Startup / launchd items
LAUNCHDDIR	= $(NSSYSTEMDIR)$(NSLIBRARYSUBDIR)/LaunchDaemons
#LaunchdConfigs	= org.net-snmp.snmpd.plist
SYSTEM_STARTUP_DIR = $(NSSYSTEMDIR)$(NSLIBRARYSUBDIR)/StartupItems
StartupItem	= SNMP

# Other configuration files
CONFIGDIR	= /private/etc/
CONFIGFILES	= snmpd.conf
CONFIGTOOL	= $(USRBINDIR)/net-snmp-config

# Binaries to strip
STRIPPED_BINS	= encode_keychange snmpbulkget snmpbulkwalk snmpdelta snmpdf \
			snmpget snmpget snmpgetnext snmpinform snmpnetstat \
			snmpset snmpstatus snmptable snmptest snmptranslate \
			snmptrap snmpusm snmpvacm snmpwalk
STRIPPED_SBINS	= snmpd snmptrapd
STRIPPED_LIBS	= libnetsnmp libnetsnmpagent libnetsnmphelpers libnetsnmpmibs


# Automatic Extract & Patch
AEP		= YES
AEP_Version	= 5.2.1
AEP_Patches	= NLS_TigerBuild.patch BO_darwin_snmp.patch \
			kernel_nlist.patch \
			NLS_PR-3962010.patch NLS_PR-4059242.patch \
			NLS_wrap.patch \
			NLS_PR-4133730.patch NLS_PR-4269071.patch \
			NLS_PR-3881250.patch NLS_PR-4046983.patch


# Local targets that must be defined before including the following
# files to get the dependency order correct
.PHONY: $(GnuAfterInstall)

install::

configure:: extract-source


# Include common makefile targets for B&I
include $(MAKEFILEPATH)/CoreOS/ReleaseControl/GNUSource.make
include AEP.make

# Override settings from above include
DESTDIR	= $(DSTROOT)

# This project must be built in the source directory because the real
# project (with configure) is there.
BuildDirectory	= $(Sources)

# These need to be overridden to match the project's use of DESTDIR.
#Install_Flags   = DESTDIR="$(DSTROOT)"

Install_Flags	= prefix=$(DSTROOT)/usr \
			exec_prefix=$(DSTROOT)/usr \
			bindir=$(DSTROOT)/usr/bin \
			sbindir=$(DSTROOT)/usr/sbin \
			sysconfdir=$(DSTROOT)/etc \
			datadir=$(DSTROOT)/usr/share \
			includedir=$(DSTROOT)/usr/include/net-snmp \
			libdir=$(DSTROOT)/usr/lib \
			libexecdir=$(DSTROOT)/usr/libexec \
			localstatedir=$(DSTROOT)/usr/share \
			mandir=$(DSTROOT)/usr/share/man \
			infodir=$(DSTROOT)/usr/share/info

Install_Target = install 

#
# Post-install targets
#
install-macosx:
	@echo "Reorganizing install for Mac OS X..."
	if [ -d $(DSTROOT)/etc ]; then 				\
		$(MKDIR) -m 755 $(DSTROOT)/private;		\
		$(MV) $(DSTROOT)/etc $(DSTROOT)/private;	\
	fi
	if [ -d $(DSTROOT)/var ]; then 				\
		$(MKDIR) -m 755 $(DSTROOT)/private;		\
		$(MV) $(DSTROOT)/var $(DSTROOT)/private;	\
	fi
	@echo "Stripping unstripped binaries..."
	for file in $(STRIPPED_BINS); \
	do \
		$(STRIP) $(DSTROOT)$(USRBINDIR)/$${file}; \
	done
	for file in $(STRIPPED_SBINS); \
	do \
		$(STRIP) $(DSTROOT)$(USRSBINDIR)/$${file}; \
	done
	for file in $(STRIPPED_LIBS); \
	do \
		$(STRIP) -x $(DSTROOT)$(USRLIBDIR)/$${file}.$(AEP_Version).dylib; \
		$(LN) -sf $${file}.$(AEP_Version).dylib $(DSTROOT)$(USRLIBDIR)/$${file}.5.dylib; \
	done
	$(FIND) $(DSTROOT)$(USRINCLUDEDIR)/net-snmp -type f -exec chmod 644 {} \;
	$(FIND) $(DSTROOT)$(SHAREDIR)/snmp -type f -exec chmod 644 {} \;
	$(RM) $(DSTROOT)$(USRLIBDIR)/*.a $(DSTROOT)$(USRLIBDIR)/*.la
	$(LN) -s net-snmp $(DSTROOT)$(USRINCLUDEDIR)/ucd-snmp 
	$(FIND) $(DSTROOT)$(MANDIR) -type f -exec chmod 644 {} \;
	@echo "Eliminating build architecture flags..."
	$(MV) $(DSTROOT)$(CONFIGTOOL) $(DSTROOT)$(CONFIGTOOL).old
	$(SED) -Ee 's/-arch [-_a-z0-9]{3,10}//g' $(DSTROOT)$(CONFIGTOOL).old > $(DSTROOT)$(CONFIGTOOL)
	$(CHMOD) 755 $(DSTROOT)$(CONFIGTOOL)
	$(RM) $(DSTROOT)$(CONFIGTOOL).old
	@echo "Installing snmpd configuration files..."
	$(INSTALL_DIRECTORY) $(DSTROOT)$(CONFIGDIR)
	for file in $(CONFIGFILES); \
	do \
		$(INSTALL_FILE) $${file} $(DSTROOT)$(CONFIGDIR); \
		if [ "${file##*.}" != "default" ]; then \
			$(INSTALL_FILE) $${file} $(DSTROOT)$(CONFIGDIR)$${file}.default; \
		fi; \
	done

install-startup:
ifdef LaunchdConfigs
	@echo "Installing launchd configuration files..."
	$(INSTALL_DIRECTORY) $(DSTROOT)$(LAUNCHDDIR)
	$(INSTALL_FILE) $(LaunchdConfigs) $(DSTROOT)$(LAUNCHDDIR)
endif
ifdef StartupItem
	@echo "Installing StartupItem..."
	$(INSTALL_DIRECTORY) $(DSTROOT)$(SYSTEM_STARTUP_DIR)/$(StartupItem)
	$(INSTALL_SCRIPT) $(StartupItem) $(DSTROOT)$(SYSTEM_STARTUP_DIR)/$(StartupItem)
	$(INSTALL_FILE) StartupParameters.plist $(DSTROOT)$(SYSTEM_STARTUP_DIR)/$(StartupItem)
	$(INSTALL_DIRECTORY) $(DSTROOT)$(SYSTEM_STARTUP_DIR)/$(StartupItem)/Resources/English.lproj
	$(INSTALL_FILE) Localizable.strings $(DSTROOT)$(SYSTEM_STARTUP_DIR)/$(StartupItem)/Resources/English.lproj
endif

#
# Install any man pages at the top-level directory or its "man" subdirectory
#
ManPages := $(wildcard *.[1-9] man/*.[1-9])

install-man:
ifdef ManPages
	for _page in $(ManPages); do					\
		_section_dir=$(Install_Man)/man$${_page##*\.};		\
		$(INSTALL_DIRECTORY) $(DSTROOT)$${_section_dir};	\
		$(INSTALL_FILE) $${_page} $(DSTROOT)$${_section_dir};	\
	done
endif
