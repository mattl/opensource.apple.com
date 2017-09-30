#
# xbs-compatible wrapper Makefile for dovecot 
#
# WARNING: B&I overrides the perms, owner, and group for everything
# under /Library; see <rdar://problem/8389433>

Project			= dovecot
DELIVERABLE		= dovecot
PROJECT_VERSION		= $(Project)-2.0.19apple1
PigeonholeProject		= $(Project)-pigeonhole
PIGEONHOLE_VERSION		= $(Project)-2.0-pigeonhole-0.2.6

# Configuration values we customize
#
DOVECOT_PUSH_NOTIFY	= $(Project).push-notify
TOOL_DIR		= $(Project).Tools
MIGRATION_TOOL		= cyrus_to_$(Project)
SKVIEW_TOOL		= skview

DATA_DIR=/Library/Server/Mail/Data
CONFIG_DIR=/Library/Server/Mail/Config/

COMMON_EXTRAS_DIR=$(NSLIBRARYDIR)/ServerSetup/CommonExtras
PROMO_EXTRAS_DIR=$(NSLIBRARYDIR)/ServerSetup/PromotionExtras
SETUP_DOVECOT=$(COMMON_EXTRAS_DIR)/65-setup_dovecot.sh
PROMO_DOVECOT=$(PROMO_EXTRAS_DIR)/65-setup_dovecot.sh

# Include common server build variables
-include /AppleInternal/ServerTools/ServerBuildVariables.xcconfig

# Environment is passed to BOTH configure AND make, which can cause problems if these
# variables are intended to help configure, but not override the result.
Environment	= MAKEOBJDIR="$(BuildDirectory)" \
            INSTALL_ROOT="$(DSTROOT)" \
            TMPDIR="$(TMPDIR)" TEMPDIR="$(TMPDIR)"
# This allows extra variables to be passed _just_ to configure.
Extra_Configure_Environment = \
	EXTRA_CFLAGS="-Dopenlog=xs_syslog_openlog -Dsyslog=xs_syslog -Dvsyslog=xs_vsyslog -Dsetlogmask=xs_syslog_setlogmask -Dcloselog=xs_syslog_closelog" \
	LDFLAGS="$(LDFLAGS) -export-dynamic -F/System/Library/PrivateFrameworks -framework CoreDaemon"
# The following were experiments used to avoid having to copy the sources into place.
# They didn't work.
#	EXTRA_CFLAGS="-Wcomment $(addprefix -I,$(shell find $(Sources)/$(Project)/src/lib* -type d -print) $(Sources)/$(Project)/src/deliver $(Sources)/$(Project)/src/mail-common) -Wformat-y2k"
#	EXTRA_LDFLAGS_PROGRAM="-mdynamic-no-pic"

Make_Flags	= -j

ProjectConfig		= $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/$(DELIVERABLE)/$(Project)-config

Common_Configure_Flags	= \
	--prefix=$(SERVER_INSTALL_PATH_PREFIX)$(USRDIR) \
	--sbindir=$(SERVER_INSTALL_PATH_PREFIX)$(USRSBINDIR) \
	--libexecdir=$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR) \
	--sysconfdir=$(CONFIG_DIR) \
	--datarootdir=$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR) \
	--localstatedir=$(SERVER_INSTALL_PATH_PREFIX)/var/$(DELIVERABLE) \
	--disable-dependency-tracking \
	--disable-static
Project_Configure_Flags	=	\
	--with-rundir=/var/run/$(DELIVERABLE) \
	--with-moduledir=$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/$(DELIVERABLE) \
	--with-ssl=openssl \
	--with-gssapi=yes
Pigeonhole_Configure_Flags	= \
	--with-dovecot=../$(Project)

# Include common makefile targets for B&I
include $(MAKEFILEPATH)/CoreOS/ReleaseControl/Common.make

XCODEBUILD	= $(USRBINDIR)/xcodebuild
# Override settings from above includes
BuildDirectory	= $(OBJROOT)/Build
Install_Target	= install
TMPDIR		= $(OBJROOT)/Build/tmp
# This needs to be overridden because the project properly uses DESTDIR and
# INSTALL_ROOT (which is included in Environment).
Install_Flags   = DESTDIR="$(DSTROOT)"
# Typically defined in GNUSource.make; duplicated here to effect similar functionality.
Sources			= $(SRCROOT)
Configure		= ./configure
ConfigureProject	= $(Configure)
ConfigurePigeonhole		= $(Configure)
ProjectConfigStamp	= $(BuildDirectory)/$(Project)/configure-stamp
PigeonholeConfigStamp	= $(BuildDirectory)/$(PigeonholeProject)/configure-stamp

.PHONY: build_dovecot build_pigeonhole build_daemon build_tools
.PHONY: archive-strip-binaries install-extras install-man install-startup-files
.PHONY: install-open-source-files

default : clean configure_dovecot build_dovecot

install :: build_dovecot build_pigeonhole \
		build_daemon build_tools archive-strip-binaries \
		install-extras install-man install-startup-files

install-no-clean :: build_dovecot build_pigeonhole


$(BuildDirectory)/$(Project)/$(Configure):
	$(_v) cd "$(BuildDirectory)/$(Project)" && autoconf

$(ProjectConfigStamp) :
	@echo "***** Configuring $(Project), version $(PROJECT_VERSION)"
	$(_v) $(MKDIR) $(BuildDirectory)/$(Project) $(TMPDIR)
	$(_v) $(CP) $(Sources)/$(Project) $(BuildDirectory)
	$(_v) cd $(BuildDirectory)/$(Project) && $(Environment) $(Extra_Configure_Environment) $(ConfigureProject) $(Common_Configure_Flags) $(Project_Configure_Flags)
	$(_v) touch $@
	@echo "***** Configuring $(Project) complete."

$(PigeonholeConfigStamp) : $(ProjectConfig)
	@echo "***** Configuring $(PigeonholeProject): Version: $(PIGEONHOLE_VERSION)"
	$(_v) $(MKDIR) $(BuildDirectory)/$(PigeonholeProject)
	$(_v) $(CP) $(Sources)/$(PigeonholeProject) $(BuildDirectory)
	$(_v) cd $(BuildDirectory)/$(PigeonholeProject) && $(Environment) $(Extra_Configure_Environment) $(ConfigurePigeonhole) $(Common_Configure_Flags) $(Pigeonhole_Configure_Flags)
	$(_v) touch $@
	@echo "***** Configuring $(PigeonholeProject) complete."

build_dovecot : $(ProjectConfigStamp) $(TMPDIR)
	@echo "***** Building $(Project)"
	@echo "***** creating MIG API files "
	$(_v) cd $(BuildDirectory)/$(Project)/src/auth && mig -v /usr/local/include/opendirectory/DSlibinfoMIG.defs
	@echo "***** creating MIG API files complete"
	$(_v) $(MAKE) -C $(BuildDirectory)/$(Project) $(Make_Flags) $(Install_Flags) $(Install_Target)
	$(_v) $(MV) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRSBINDIR)/$(DELIVERABLE)" "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRSBINDIR)/$(DELIVERABLE)d"
	@echo "***** Building $(Project) complete."

$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/$(DELIVERABLE)/$(Project)-config:
	$(_v) $(MAKE) build_dovecot

build_pigeonhole : $(PigeonholeConfigStamp) $(ProjectConfig)
	@echo "***** Building $(PigeonholeProject)"
	$(_v) $(MAKE) -C $(BuildDirectory)/$(PigeonholeProject) $(Make_Flags) $(Install_Flags) $(Install_Target)
	$(_v) $(CP) $(BuildDirectory)/$(PigeonholeProject)/src/plugins/lda-sieve/.libs/lib90_sieve_plugin.so* \
		$(SYMROOT)
	@echo "***** Building $(PigeonholeProject) complete."

build_daemon : $(ProjectConfig)
	@echo "***** Building $(DOVECOT_PUSH_NOTIFY)"
	$(_v) cd "$(SRCROOT)/$(DOVECOT_PUSH_NOTIFY)/daemon" \
		&& $(XCODEBUILD) $(Install_Target)	\
			SRCROOT="$(SRCROOT)/$(DOVECOT_PUSH_NOTIFY)/daemon"	\
			OBJROOT=$(OBJROOT)		\
			SYMROOT=$(SYMROOT)		\
			DSTROOT="$(DSTROOT)"		\
			RC_CFLAGS="$(RC_CFLAGS)"	\
			RC_ARCHS="$(RC_ARCHS)"
	@echo "***** Building $(DOVECOT_PUSH_NOTIFY) complete."

build_tools :
	@echo "***** Building $(MIGRATION_TOOL)"
	$(_v) cd "$(SRCROOT)/$(TOOL_DIR)/$(MIGRATION_TOOL)" \
		&& $(XCODEBUILD) $(Install_Target)	\
			SRCROOT="$(SRCROOT)/$(TOOL_DIR)/$(MIGRATION_TOOL)"	\
			OBJROOT=$(OBJROOT)		\
			SYMROOT=$(SYMROOT)		\
			DSTROOT="$(DSTROOT)"		\
			RC_CFLAGS="$(RC_CFLAGS)"	\
			RC_ARCHS="$(RC_ARCHS)"
	@echo "***** Building $(MIGRATION_TOOL) complete."
	@echo "***** Building $(SKVIEW_TOOL)"
	$(_v) cd "$(SRCROOT)/$(TOOL_DIR)/$(SKVIEW_TOOL)" \
		&& $(XCODEBUILD) $(Install_Target)	\
			SRCROOT="$(SRCROOT)/$(TOOL_DIR)/$(SKVIEW_TOOL)"	\
			OBJROOT=$(OBJROOT)		\
			SYMROOT=$(SYMROOT)		\
			DSTROOT="$(DSTROOT)"		\
			RC_CFLAGS="$(RC_CFLAGS)"	\
			RC_ARCHS="$(RC_ARCHS)"
	@echo "***** Building $(SKVIEW_TOOL) complete."

# Custom configuration:
#
#

lib_cleanup :
	@echo "***** Cleaning up files not intended for installation"
	$(_v) $(RMDIR) $(SYMROOT)/usr/include
	$(_v) $(RMDIR) $(DSTROOT)/usr/include
	$(_v) $(RMDIR) "$(SYMROOT)$(RUNDIR)"
	$(_v) $(RMDIR) "$(DSTROOT)$(RUNDIR)"
	@echo "***** Cleaning up complete."

archive-strip-binaries: $(SYMROOT)
	@echo "***** Archiving, dSYMing and stripping binaries..."
	$(_v) $(FIND) $(OBJROOT) -name '*.dSYM' -exec $(CP) {} $(SYMROOT) \;
	$(_v) for file in $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRBINDIR)/* $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRSBINDIR)/* $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/*;\
	do \
		if test -L $${file}; then \
			echo "Skipping symlink $${file}"; \
			continue; \
		fi; \
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
	$(_v) for file in $$( $(FIND) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRLIBDIR)/$(DELIVERABLE) -type f \( -name '*.so' -o -name '*.dylib' \) );\
	do \
		if test -L $${file}; then \
			echo "Skipping symlink $${file}"; \
			continue; \
		fi; \
		echo "Processing $${file##*/} (from $${file})";	\
		if [ ! -e "$(SYMROOT)/$${file##*/}" ]; then	\
			echo "  $(CP) $${file} $(SYMROOT)";	\
			$(CP) $${file} $(SYMROOT);	\
		fi;	\
		if [ -e "$(SYMROOT)/$${file##*/}.dSYM" ]; then	\
			echo "...dSYM already copied.";	\
		else	\
			echo "  $(DSYMUTIL) --out=$(SYMROOT)/$${file##*/}.dSYM $${file}";\
			$(DSYMUTIL) --out=$(SYMROOT)/$${file##*/}.dSYM $${file};\
		fi;	\
		$(STRIP) -rx $${file};	\
	done
	@echo "***** Archiving, dSYMing and stripping binaries complete."

install-strip :
	@echo "***** Stripping language libraries..."
	@echo "***** Stripping language libraries complete."

install-open-source-files:
	@echo "***** Installing open source configuration files..."
	$(_v) $(INSTALL_DIRECTORY) $(DSTROOT)$(USRDIR)/local/OpenSourceVersions
	$(_v) $(INSTALL_DIRECTORY) $(DSTROOT)$(USRDIR)/local/OpenSourceLicenses
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/dovecot.OpenSourceInfo/$(DELIVERABLE).plist" \
				  "$(DSTROOT)$(USRDIR)/local/OpenSourceVersions"
	$(_v) $(INSTALL_FILE) "$(SRCROOT)/dovecot.OpenSourceInfo/$(DELIVERABLE).txt" \
				  "$(DSTROOT)$(USRDIR)/local/OpenSourceLicenses"
	@echo "***** Installing open source configuration files complete."

install-extras : install-open-source-files lib_cleanup
	@echo "***** Installing extras..."
	$(_v) if [ ! -d "$(DSTROOT)$(CONFIG_DIR)" ]; then	\
		echo "$(MKDIR) $(DSTROOT)$(CONFIG_DIR)";		\
		$(MKDIR) "$(DSTROOT)$(CONFIG_DIR)";		\
	fi
	#$(_v) if [ -e "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(ETCDIR)" -a "$(ETCDIR)" != "/etc" ]; then	\
	#	echo "$(MV) $(DSTROOT)/etc/* $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(ETCDIR)";	\
	#	$(MV) "$(DSTROOT)/etc/*" "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(ETCDIR)";		\
	#	echo "$(RMDIR) $(DSTROOT)/etc";				\
	#	$(RMDIR) "$(DSTROOT)/etc";				\
	#fi
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)" \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/conf.d" \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/example-config" \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/example-config/conf.d" \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/default" \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/default/conf.d"
	$(_v) $(INSTALL_FILE) $(BuildDirectory)/$(Project)/doc/example-config/*.conf* \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/example-config"
	$(_v) $(INSTALL_FILE) $(BuildDirectory)/$(Project)/doc/example-config/conf.d/*.conf* \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/example-config/conf.d"
	$(_v) $(INSTALL_FILE) $(SRCROOT)/dovecot.Config/*.conf* \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/default"
	$(_v) $(INSTALL_FILE) $(SRCROOT)/dovecot.Config/conf.d/*.conf* \
			"$(DSTROOT)$(CONFIG_DIR)/$(DELIVERABLE)/default/conf.d"
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/ServerSetup/CommonExtras"
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/ServerSetup/MigrationExtras"
	$(_v) $(INSTALL_DIRECTORY) "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/ServerSetup/PromotionExtras"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/dovecot.Config/65_mail_migrator.pl" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/ServerSetup/MigrationExtras/65_mail_migrator.pl"
	$(_v) (/bin/echo "#!/bin/sh" > "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SETUP_DOVECOT)") 
	$(_v) (/bin/echo "#" >> "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SETUP_DOVECOT)")
	$(_v) (/bin/echo "" >> "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SETUP_DOVECOT)")
	$(_v) (/bin/echo "_server_root=$(SERVER_INSTALL_PATH_PREFIX)" >> "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SETUP_DOVECOT)")
	$(_v) (/bin/cat "$(SRCROOT)/dovecot.Config/SetupDovecot.sh" >> "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SETUP_DOVECOT)")
	$(_v) (/bin/chmod 755 "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SETUP_DOVECOT)")
	install -m 0755	"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SETUP_DOVECOT)" "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(PROMO_DOVECOT)"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/dovecot.Config/migrate_partition_mail_data" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/migrate_partition_mail_data.sh"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/dovecot.Config/migrate_mail_data.pl" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/migrate_mail_data.pl"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/dovecot.Config/mail_data_migrator.pl" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/mail_data_migrator.pl"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/dovecot.Config/migrate_single_user_mail_data" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/migrate_single_user_mail_data.sh"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/dovecot.Config/quota-warning.sh" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/quota-warning.sh"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/dovecot.Config/quota-exceeded.sh" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/quota-exceeded.sh"
	$(_v) $(CHMOD) u+w $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/quota-*.sh
	$(_v) $(CHOWN) root:mail "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/deliver"
	$(_v) $(CHMOD) 0750 "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/deliver"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/$(TOOL_DIR)/update-fts-index.pl" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/update-fts-index.pl"
	$(_v) $(INSTALL_SCRIPT) "$(SRCROOT)/$(TOOL_DIR)/mail_preflight" \
			"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(LIBEXECDIR)/$(DELIVERABLE)/mail_preflight"
	$(_v) (cd "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(USRBINDIR)" && $(LN) -s cvt_mail_data set_user_mail_opts)
	@echo "***** Installing extras complete."

install-man :
	@echo "***** Installing man pages..."
	$(_v) perl -p -i -e '$$pass = 1 if /FILES/; s/dovecot/dovecotd/g unless $$pass;' "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR)/man/man1/dovecot.1"
	$(_v) (cd "$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(SHAREDIR)/man" && $(MKDIR) -p man8 && $(LN) -s ../man1/dovecot.1 man8/dovecotd.8)
	@echo "***** Installing man pages complete."

install-startup-files :
	@echo "***** Installing Startup Item..."
	$(_v) $(INSTALL_DIRECTORY) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons
	$(_v) $(INSTALL_FILE) \
		$(SRCROOT)/dovecot.LaunchDaemons/org.dovecot.$(DELIVERABLE)d.plist \
		$(SRCROOT)/dovecot.LaunchDaemons/com.apple.mail_migration.plist \
		$(SRCROOT)/dovecot.LaunchDaemons/org.dovecot.fts.update.plist \
		$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons

	# move config files under SERVER_INSTALL_PATH_PREFIX
	$(_v) $(INSTALL_DIRECTORY) $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(ETCDIR)
	$(_v) $(MV) $(DSTROOT)$(CONFIG_DIR)/dovecot $(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(ETCDIR)
	$(_v) $(RMDIR) $(DSTROOT)/Library

	# set server root path to launchd bins
	$(_v) /usr/libexec/PlistBuddy -c 'Set :ProgramArguments:0 \
		$(SERVER_INSTALL_PATH_PREFIX)/usr/libexec/dovecot/mail_preflight' \
		"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons/org.dovecot.dovecotd.plist"
	$(_v) /usr/libexec/PlistBuddy -c 'Set :ProgramArguments:1 \
		$(SERVER_INSTALL_PATH_PREFIX)/usr/sbin/dovecotd' \
		"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons/org.dovecot.dovecotd.plist"

	$(_v) /usr/libexec/PlistBuddy -c 'Set :Program \
		$(SERVER_INSTALL_PATH_PREFIX)/usr/libexec/dovecot/update-fts-index.pl' \
		"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons/org.dovecot.fts.update.plist"
	$(_v) /usr/libexec/PlistBuddy -c 'Set :ProgramArguments:0 \
		$(SERVER_INSTALL_PATH_PREFIX)/usr/libexec/dovecot/update-fts-index.pl' \
		"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons/org.dovecot.fts.update.plist"

	$(_v) /usr/libexec/PlistBuddy -c 'Set :Program \
		$(SERVER_INSTALL_PATH_PREFIX)/usr/sbin/push_notify' \
		"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons/com.apple.push_notify.plist"
	$(_v) /usr/libexec/PlistBuddy -c 'Set :ProgramArguments:0 \
		$(SERVER_INSTALL_PATH_PREFIX)/usr/sbin/push_notify' \
		"$(DSTROOT)$(SERVER_INSTALL_PATH_PREFIX)$(NSLIBRARYDIR)/LaunchDaemons/com.apple.push_notify.plist"

	@echo "***** Installing Startup Item complete."

$(DSTROOT) $(TMPDIR) :
	$(_v) if [ ! -d $@ ]; then	\
		$(MKDIR) $@;	\
	fi
