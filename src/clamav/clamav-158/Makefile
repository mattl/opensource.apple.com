# xbs-compatible wrapper Makefile for Clam AV
#

Project			= clamav
DELIVERABLE		= clamav
OPEN_SOURCE_VER	= 0.98
PROJECT_VERSION	= $(Project)-$(OPEN_SOURCE_VER)

# Configuration values we customize
#
CLAMAV_OWNER		= _clamav
CLAMAV_GROUP		= _clamav

PROJECT_BIN_DIR		= $(Project).Bin
PROJECT_CONF_DIR	= $(Project).Conf
PROJECT_SETUP_DIR	= $(Project).SetupExtras
PROJECT_LD_DIR		= $(Project).LaunchDaemons
PROJECT_OS_DIR		= $(Project).OpenSourceInfo
PROJECT_PATCH_DIR	= $(Project).Patches

DATA_DIR		= /Library/Server/Mail/Data
CONFIG_DIR		= /Library/Server/Mail/Config

PROMO_DIR			= $(SERVER_INSTALL_PATH_PREFIX)/System/Library/ServerSetup/PromotionExtras
SRC_PROMO_SCRIPT	= service_promotion.pl
DST_PROMO_SCRIPT	= 63-mail_services_virus_scanner.pl

USR_LIBS		= libclamav.6.dylib libclamunrar.6.dylib libclamunrar_iface.6.so

PATCH_FILES		= patch1.txt patch2.txt patch3.txt patch4.txt patch5.txt

CONFIG_ENV		= MAKEOBJDIR="$(BuildDirectory)" \
	        		INSTALL_ROOT="$(DSTROOT)" \
	        		TMPDIR="$(TMPDIR)" TEMPDIR="$(TMPDIR)"

CFLAGS			= -O0 $(RC_CFLAGS)

# Environment is passed to BOTH configure AND make, which can cause problems if these
# variables are intended to help configure, but not override the result.
Environment		= MAKEOBJDIR="$(BuildDirectory)" \
				INSTALL_ROOT="$(DSTROOT)" \
				TMPDIR="$(TMPDIR)" TEMPDIR="$(TMPDIR)"

# This allows extra variables to be passed _just_ to configure.
Extra_Configure_Environment =

EXTRA_CFLAGS=-Dopenlog=xs_syslog_openlog -Dsyslog=xs_syslog -Dvsyslog=xs_vsyslog -Dsetlogmask=xs_syslog_setlogmask -Dcloselog=xs_syslog_closelog
EXTRA_LDFLAGS=-export-dynamic -F/System/Library/PrivateFrameworks -framework CoreDaemon

Make_Flags		=

ProjectConfig		= $(DSTROOT)$(USRINCLUDEDIR)/$(Project)/$(Project)-config

Common_Configure_Flags	= \
	--prefix=$(SERVER_INSTALL_PATH_PREFIX)/ \
	--mandir=$(SERVER_INSTALL_PATH_PREFIX)$(MANDIR) \
	--bindir=$(SERVER_INSTALL_PATH_PREFIX)$(USRBINDIR) \
	--libdir=$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR) \
	--sbindir=$(SERVER_INSTALL_PATH_PREFIX)$(USRSBINDIR) \
	--datadir=$(DATA_DIR)/$(Project) \
	--exec-prefix=$(SERVER_INSTALL_PATH_PREFIX)$(USRDIR) \
	--libexecdir=$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR) \
	--sysconfdir=$(CONFIG_DIR)/$(Project) \
	--datarootdir=$(DATA_DIR)/scanner/$(Project)/share \
	--sharedstatedir=$(DATA_DIR)/scanner/$(Project)/share \
	--localstatedir=$(DATA_DIR)/scanner/$(Project)/state \
	--disable-dependency-tracking
Project_Configure_Flags	=	\
	--includedir=$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR)/$(Project)/include \
	--oldincludedir=$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR)/$(Project)/include \
	--with-dbdir=$(DATA_DIR)/scanner/$(Project) \
	--with-user=$(CLAMAV_OWNER) \
	--with-group=$(CLAMAV_GROUP) \
	--with-gnu-ld
Stataic_Configure_Flags	=	\
	--disable-shared \
	--enable-static

# Include common makefile targets for B&I
include $(MAKEFILEPATH)/CoreOS/ReleaseControl/Common.make

# Override settings from above includes
BuildDirectory		= $(OBJROOT)/Build
Install_Target		= install
TMPDIR			= $(OBJROOT)/Build/tmp

# This needs to be overridden because the project properly uses DESTDIR and
# INSTALL_ROOT (which is included in Environment).
Install_Flags	= DESTDIR="$(DSTROOT)"

# Typically defined in GNUSource.make; duplicated here to effect similar functionality.
Sources			= $(SRCROOT)
Configure		= ./configure
ConfigureProject	= $(Configure)
ProjectConfigStamp	= $(BuildDirectory)/$(Project)/configure-stamp

LIB_TOOL		= $(BuildDirectory)/$(Project)/libtool

# Unbundling install paths
include /AppleInternal/ServerTools/ServerBuildVariables.xcconfig

.PHONY: build-clamav
.PHONY: archive-strip-binaries install-extras install-man install-startup-files
.PHONY: install-open-source-files

default : clean build-clamav

install :: build-clamav archive-strip-binaries \
		install-extras install-startup-files \
		lib_cleanup

install-no-clean :: build-clamav

build-clamav :: extract-sources make-clamav

###################

extract-sources : $(TMPDIR)
	@echo "***** Extracting sources from: $(CLAMAV_TAR_GZ)"
	$(_v) cd $(BuildDirectory) && cp -rf $(Sources)/$(PROJECT_BIN_DIR)/$(PROJECT_VERSION) .
	$(_v) $(MV) $(BuildDirectory)/$(PROJECT_VERSION) $(BuildDirectory)/$(Project)
	@echo "***** Extracting sources complete."

make-clamav : $(TMPDIR)
	@echo "***** Building $(Project)"
	@echo "***** Applying project patches: $(PROJECT_PATCH)"
	$(_v) cd "$(BuildDirectory)/$(Project)" && for file in $(PATCH_FILES);\
	do \
		echo "*----------------------------" ; \
		echo "* Applying patch: $${file}" ; \
		patch -p1 < "$(SRCROOT)/$(PROJECT_PATCH_DIR)/$${file}" ; \
	done
	@echo "***** Applying project patches complete."
	@echo "*----------------------------"
	@echo "***** Configuring $(Project) shared, version $(PROJECT_VERSION)"
	$(_v) cd $(BuildDirectory)/$(Project) && $(CONFIG_ENV) $(Extra_Configure_Environment) $(ConfigureProject) $(Common_Configure_Flags) $(Project_Configure_Flags)
	$(_v) touch $@
	@echo "***** Configuring $(Project) shared complete."
	@echo "***** Patching $(LIB_TOOL)"
	$(_v) cd $(BuildDirectory)/$(Project)
	if grep -qs 'LTCFLAGS=\"-g -O2\"' $(LIB_TOOL) ; then \
		mv $(LIB_TOOL) $(LIB_TOOL).bak ; \
		sed -e 's/LTCFLAGS=\"-g -O2\"/LTCFLAGS=\"$(CFLAGS) $(EXTRA_CFLAGS)\"/g' $(LIB_TOOL).bak > $(LIB_TOOL) ; \
	fi
	@echo "***** Patching $(LIB_TOOL) complete."
	@echo "***** Making $(Project)"
	$(_v) $(MAKE) -C $(BuildDirectory)/$(Project) CFLAGS="$(CFLAGS) $(EXTRA_CFLAGS)" CXXFLAGS="$(CFLAGS) $(EXTRA_CFLAGS)" LDFLAGS="$(LDFLAGS) $(EXTRA_LDFLAGS)" $(Make_Flags) $(Install_Flags) $(Install_Target)
	@echo "***** Making $(Project) complete."
	$(_v) cd $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR) && for file in $(USR_LIBS);\
	do \
		echo "Processing $${file##*/} (from $${file})"; \
		if [ ! -e "$(SYMROOT)/$${file##*/}" ]; then     \
			echo "  $(CP) $${file} $(SYMROOT)";     \
			$(CP) $${file} $(SYMROOT);      \
		fi;     \
		if [ -e "$(SYMROOT)/$${file##*/}.dSYM" ]; then  \
			echo "...odd, dSYM already exists.";    \
		else    \
			echo "  $(DSYMUTIL) --out=$(SYMROOT)/$${file##*/}.dSYM $${file}";\
			$(DSYMUTIL) --out=$(SYMROOT)/$${file##*/}.dSYM $${file};\
		fi;     \
		$(STRIP) -rx $${file};   \
	done
	@echo "***** Cleaning sources for: $(Project)"
	$(_v) cd $(BuildDirectory)/$(Project) && make distclean
	@echo "***** Cleaning sources complete."
	@echo "***** Configuring $(Project) static, version $(PROJECT_VERSION)"
	$(_v) cd $(BuildDirectory)/$(Project) && $(CONFIG_ENV) $(Extra_Configure_Environment) $(ConfigureProject) $(Common_Configure_Flags) $(Project_Configure_Flags) $(Stataic_Configure_Flags)
	$(_v) touch $@
	@echo "***** Configuring $(Project) static complete."
	@echo "***** Patching $(LIB_TOOL)"
	$(_v) cd $(BuildDirectory)/$(Project)
	if grep -qs 'LTCFLAGS=\"-g -O2\"' $(LIB_TOOL) ; then \
		mv $(LIB_TOOL) $(LIB_TOOL).bak ; \
		sed -e 's/LTCFLAGS=\"-g -O2\"/LTCFLAGS=\"$(CFLAGS) $(EXTRA_CFLAGS)\"/g' $(LIB_TOOL).bak > $(LIB_TOOL) ; \
	fi
	@echo "***** Patching $(LIB_TOOL) complete."
	@echo "***** Making $(Project)"
	$(_v) $(MAKE) -C $(BuildDirectory)/$(Project) CFLAGS="$(CFLAGS) $(EXTRA_CFLAGS)" CXXFLAGS="$(CFLAGS) $(EXTRA_CFLAGS)" LDFLAGS="$(LDFLAGS) $(EXTRA_LDFLAGS)" $(Make_Flags) $(Install_Flags) $(Install_Target)
	@echo "***** Making $(Project) complete."
	@echo "***** Building $(Project) complete."

$(ProjectConfig): $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/$(Project)/$(Project)-config
	$(_v) $(CP) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/$(Project)/$(Project)-config" $@

$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/$(Project)/$(Project)-config:
	$(_v) $(MAKE) build-clamav

# Custom configuration

lib_cleanup :
	@echo "***** Cleaning up files not intended for installation"
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/libclamav.6.dylib
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/libclamav.a
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/libclamav.dylib
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/libclamav.la
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/libclamunrar.a
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/libclamunrar_iface.a
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/libclamunrar.la
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/libclamunrar_iface.la
	$(_v) $(RMDIR) $(SYMROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR)/$(Project)/include
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR)/$(Project)/include
	$(_v) $(RMDIR) $(SYMROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRBINDIR)/clamav-config
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRBINDIR)/clamav-config
	$(_v) $(RMDIR) $(SYMROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/pkgconfig
	$(_v) $(RMDIR) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/pkgconfig
	$(_v) $(RMDIR) $(DSTROOT)/Library
	@echo "***** Cleaning up complete."

archive-strip-binaries: $(SYMROOT)
	@echo "***** Archiving, dSYMing and stripping binaries..."
	$(_v) for file in $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRBINDIR)/* $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRSBINDIR)/*;\
	do \
		echo "Processing $${file##*/} (from $${file})";	\
		if [ ! -e "$(SYMROOT)/$${file##*/}" ]; then	\
			echo "  $(CP) $${file} $(SYMROOT)";	\
			$(CP) $${file} $(SYMROOT);	\
		fi;	\
		if [ -e "$(SYMROOT)/$${file##*/}.dSYM" ]; then	\
			echo "...odd, dSYM already exists.";	\
		else	\
			echo "  $(DSYMUTIL) --out=$(SYMROOT)/$${file##*/}.dSYM $${file}";\
			$(DSYMUTIL) --out=$(SYMROOT)/$${file##*/}.dSYM $${file};\
		fi;	\
		$(STRIP) -S $${file};	\
	done
	@echo "***** Archiving, dSYMing and stripping binaries complete."

install-open-source-files:
	@echo "***** Installing open source configuration files..."
	$(_v) $(INSTALL_DIRECTORY) $(DSTROOT)$(USRDIR)/local/OpenSourceVersions
	$(_v) $(INSTALL_DIRECTORY) $(DSTROOT)$(USRDIR)/local/OpenSourceLicenses
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/$(PROJECT_OS_DIR)/$(Project).plist" "$(DSTROOT)$(USRDIR)/local/OpenSourceVersions"
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/$(PROJECT_OS_DIR)/$(Project).txt" "$(DSTROOT)$(USRDIR)/local/OpenSourceLicenses"
	@echo "***** Installing open source configuration files complete."

install-extras : install-open-source-files
	@echo "***** Installing extras..."
	# install directories
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(ETCDIR)"
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR)/sandbox"
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/ServerSetup/PromotionExtras"

	# Service configuration files
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/$(PROJECT_CONF_DIR)/clamd.conf.default" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(ETCDIR)/clamd.conf.default"
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/$(PROJECT_CONF_DIR)/freshclam.conf.default" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(ETCDIR)/freshclam.conf.default"

	# Service promotion script
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/$(PROJECT_SETUP_DIR)/$(SRC_PROMO_SCRIPT)" \
			"$(DSTROOT)$(PROMO_DIR)/$(DST_PROMO_SCRIPT)"
	$(SILENT) (/bin/chmod 755 "$(DSTROOT)$(PROMO_DIR)/$(DST_PROMO_SCRIPT)")

	# Sandbox setup
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/$(PROJECT_SETUP_DIR)/clamd.sb" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR)/sandbox/clamd.sb"

	# Install clamd preflight script
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/$(PROJECT_SETUP_DIR)/clamd_preflight" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/clamd_preflight"

	# Install missing man page
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/$(PROJECT_CONF_DIR)/clambc.1" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(MANDIR)/man1/clambc.1"

	@echo "***** Installing extras complete."

install-startup-files :
	@echo "***** Installing Startup Item..."
	$(_v) $(INSTALL_DIRECTORY) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons
	$(_v) $(INSTALL_FILE) \
		$(SRCROOT)/$(PROJECT_LD_DIR)/org.clamav.clamd.plist \
		$(SRCROOT)/$(PROJECT_LD_DIR)/org.clamav.freshclam.plist \
		$(SRCROOT)/$(PROJECT_LD_DIR)/org.clamav.freshclam-init.plist \
		$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons
	@echo "***** Installing Startup Item complete."

$(DSTROOT) $(TMPDIR) :
	$(_v) if [ ! -d $@ ]; then	\
		$(MKDIR) $@;	\
	fi
