##
# Makefile for ntp
##

# Project info
Project           = ntp
UserType          = Administration
ToolType          = Services
Extra_Configure_Flags = --disable-nls --disable-dependency-tracking --with-openssl-libdir=/usr/lib --with-openssl-incdir=/usr/include/openssl --with-crypto=openssl
Extra_Environment = AUTOCONF="$(Sources)/missing autoconf"	\
                    AUTOHEADER="$(Sources)/missing autoheader"	\
                    ac_cv_decl_syscall=no			\
                    ac_cv_header_netinfo_ni_h=no		\
                    LIBMATH=""
GnuAfterInstall   = install-man-pages rm-tickadj classic-install-path

# It's a GNU Source project
include $(MAKEFILEPATH)/CoreOS/ReleaseControl/GNUSource.make

MANPAGES = man/ntp-genkeys.8 \
	man/ntpd.8 \
	man/ntpdate.8 \
	man/ntpdc.8 \
	man/ntpq.8 \
	man/ntptime.8 \
	man/ntptrace.8

# Automatic Extract & Patch
AEP            = YES
AEP_Project    = $(Project)
AEP_Version    = 4.2.0
AEP_ProjVers   = $(AEP_Project)-$(AEP_Version)
AEP_Filename   = $(AEP_ProjVers).tar.gz
AEP_ExtractDir = $(AEP_ProjVers)
AEP_Patches    = ntp_stdlib.patch NLS_EAP.patch NLS_PR-4237140.patch

ifeq ($(suffix $(AEP_Filename)),.bz2)
AEP_ExtractOption = j
else
AEP_ExtractOption = z
endif

# Extract the source.
install_source::
ifeq ($(AEP),YES)
	$(TAR) -C $(SRCROOT) -$(AEP_ExtractOption)xf $(SRCROOT)/$(AEP_Filename)
	$(RMDIR) $(SRCROOT)/$(AEP_Project) 
	$(MV) $(SRCROOT)/$(AEP_ExtractDir) $(SRCROOT)/$(AEP_Project)
	for patchfile in $(AEP_Patches); do \
	cd $(SRCROOT)/$(Project) && patch -p0 < $(SRCROOT)/patches/$$patchfile; \
	done
endif

install-man-pages:
	install -d $(DSTROOT)/usr/share/man/man8
	install -c -m 444 $(SRCROOT)/$(MANPAGES) $(DSTROOT)/usr/share/man/man8/

# since the configure script is totally *broken*
# w/ regards to enabling/disabling tickadj, we remove it
# after-the-fact, by hand.
rm-tickadj:
	rm $(DSTROOT)/usr/bin/tickadj


classic-install-path:
	mkdir -p $(DSTROOT)/usr/sbin
	$(MV) $(DSTROOT)/usr/bin/ntpd $(DSTROOT)/usr/sbin/ntpd
	$(MV) $(DSTROOT)/usr/bin/ntpdate $(DSTROOT)/usr/sbin/ntpdate
	$(MV) $(DSTROOT)/usr/bin/ntpdc $(DSTROOT)/usr/sbin/ntpdc
	$(MV) $(DSTROOT)/usr/bin/ntptrace $(DSTROOT)/usr/sbin/ntptrace
