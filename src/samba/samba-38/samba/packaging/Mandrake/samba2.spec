%define pkg_name samba
%define ver 2.2.5
%define rel 17mdk
%define vscanver 0.2.5c

# Determine whether this is the system samba or not.
%define build_system	0
%if %build_system
%define samba_major	%{null}
%else
%define samba_major	3
%global _iconsdir	%{_datadir}/icons/
%global _miconsdir	%{_datadir}/icons/mini
%global _liconsdir	%{_datadir}/icons/large
%global _menudir	%{_libdir}/menu
#%%%%global __prefix		/opt/samba%{samba_major}

%global _mandir		%{_datadir}/man
%endif

# 2.2.4 and 1 replace by samba-team at release
%define pversion 3.0alpha22
%define prelease 1
# For testing this setup:
#%define pversion1 2.2.5
#%define prelease1 %(date +%Y%m%d)

#Check to see if p(version|release) has been replaced (1 if replaced)
%define have_pversion %(if [ "%pversion" = `echo "pversion" |tr '[:lower:]' '[:upper:]'` ];then echo 0; else echo 1; fi)
%define have_prelease %(if [ "%prelease" = `echo "prelease" |tr '[:lower:]' '[:upper:]'` ];then echo 0; else echo 1; fi)

# We might have a prerelease:
%define have_pre %(echo %pversion|awk '{p=0} /[a-z,A-Z][a-z,A-Z]/ {p=1} {print p}')
%if %have_pre
%define pre_ver %(perl -e '$name="%pversion"; print ($name =~ /(.*?)[a-z]/);')
%define pre_pre %(echo %pversion|sed -e 's/%pre_ver//g')
%endif


# Check to see if we are running a build from a tarball release from samba.org
# (%have_pversion) If so, disable vscan, unless explicitly requested
# (--with vscan).
%define build_vscan 1
%if %have_pversion
%define build_vscan 0
%{?_with_vscan: %define build_vscan 1}
%endif

# We now do detection of the Mandrake release we are building on:
#%define build_cooker %(if [ `awk '{print $3}' /etc/mandrake-release` = "Cooker" ];then echo 1; else echo 0; fi)
#%define build_cooker %(if [[ `cat /etc/mandrake-release|grep Cooker` ]];then echo 1; else echo 0; fi)
%define build_mdk91 %(if [ `awk '{print $4}' /etc/mandrake-release` = 9.1 ];then echo 1; else echo 0; fi)
%define build_mdk90 %(if [ `awk '{print $4}' /etc/mandrake-release` = 9.0 ];then echo 1; else echo 0; fi)
%define build_mdk83 %(if [ `awk '{print $4}' /etc/mandrake-release` = 8.3 ];then echo 1; else echo 0; fi)
%define build_mdk82 %(if [ `awk '{print $4}' /etc/mandrake-release` = 8.2 ];then echo 1; else echo 0; fi)
%define build_mdk81 %(if [ `awk '{print $4}' /etc/mandrake-release` = 8.1 ];then echo 1; else echo 0; fi)
%define build_mdk80 %(if [ `awk '{print $4}' /etc/mandrake-release` = 8.0 ];then echo 1; else echo 0; fi)
%define build_mdk72 %(if [ `awk '{print $4}' /etc/mandrake-release` = 7.2 ];then echo 1; else echo 0; fi)
%define build_non_default 0

# Default options
%define build_acl 	1
%define build_winbind 	1
%define build_wins 	1
%define build_ldap 	1
%define build_ads	1

# Set defaults for each version
%if %build_mdk91
%endif

%if %build_mdk90
%endif

%if %build_mdk83
%endif

%if %build_mdk82
%endif

%if %build_mdk81
%define build_winbind	0
%define build_wins	0
%endif

%if %build_mdk80
%define build_acl	0
%define build_winbind	0
%define build_wins	0
%endif

%if %build_mdk72
%define build_acl	0
%define build_winbind	0
%define build_wins	0
%endif

# Allow commandline option overrides (borrowed from Vince's qmail srpm):
# To use it, do rpm [-ba|--rebuild] --with 'xxx'
# Check if the rpm was built with the defaults, otherwise we inform the user
%define build_non_default 0
%{?_with_acl: %{expand: %%define build_acl 1}}
%{?_with_acl: %{expand: %%define build_non_default 1}}
%{?_without_acl: %{expand: %%define build_acl 0}}
%{?_without_acl: %{expand: %%define build_non_default 1}}
%{?_with_winbind: %{expand: %%global build_winbind 1}}
%{?_with_winbind: %{expand: %%define build_non_default 1}}
%{?_without_winbind: %{expand: %%define build_winbind 0}}
%{?_without_winbind: %{expand: %%define build_non_default 1}}
%{?_with_wins: %{expand: %%global build_wins 1}}
%{?_with_wins: %{expand: %%define build_non_default 1}}
%{?_without_wins: %{expand: %%global build_wins 0}}
%{?_without_wins: %{expand: %%define build_non_default 1}}
%{?_with_ldap: %{expand: %%global build_ldap 1}}
%{?_with_ldap: %{expand: %%define build_non_default 1}}
%{?_without_ldap: %{expand: %%global build_ldap 0}}
%{?_without_ldap: %{expand: %%define build_non_default 1}}
%{?_with_ads: %{expand: %%define build_ads 1}}
%{?_with_ads: %{expand: %%define build_non_default 1}}
%{?_without_ads: %{expand: %%define build_ads 0}}
%{?_without_ads: %{expand: %%define build_non_default 1}}

# As if that weren't enough, we're going to try building with antivirus
# support as an option also
%define build_fprot 0
%define build_kaspersky 0
%define build_mks 0
%define build_openantivirus 0
%define build_sophos 0
%define build_symantec 0
%define build_trend 0
%if %build_vscan
%{?_with_fprot: %{expand: %%global build_fprot 1}}
%{?_with_kaspersky: %{expand: %%global build_kaspersky 1}}
%{?_with_mks: %{expand: %%global build_mks 1}}
%{?_with_openav: %{expand: %%global build_openantivirus 1}}
%{?_with_sophos: %{expand: %%global build_sophos 1}}
%{?_with_symantec: %{expand: %%global build_symantec 1}}
%{?_with_trend: %{expand: %%global build_trend 1}}
%define vscandir "samba-vscan-%{vscanver}"
%endif
%define vfsdir "examples.bin/VFS"

#Workaround missing macros in 8.x:
%{!?perl_vendorlib: %{expand: %%global perl_vendorlib %{perl_sitearch}/../}}

Summary: Samba SMB server.
Name: %{pkg_name}%{samba_major}
%if %have_pversion && %have_pre
Version: %{pre_ver}
%define source_ver %{pversion}
%endif
%if %have_pversion && !%have_pre
Version: %{pversion}
%define source_ver %{pversion}
%endif
%if !%have_pversion
Version: %{ver}
%define source_ver %{ver}
%endif
%if %have_prelease && !%have_pre
Release: 1.%{prelease}mdk
%endif
%if %have_prelease && %have_pre
Release: 0.%{pre_pre}.%{prelease}mdk
%endif
%if !%have_prelease
Release: %{rel}
%endif
License: GPL
Group: System/Servers
Source: ftp://samba.org/pub/samba/samba-%{source_ver}.tar.bz2
URL:	http://www.samba.org
Source1: samba.log
Source2: mount.smb
Source3: samba.xinetd
Source4: swat_48.xpm.bz2
Source5: swat_32.xpm.bz2
Source6: swat_16.xpm.bz2
Source7: README.%{name}-mandrake-rpm
%if %build_vscan
Source8: samba-vscan-%{vscanver}.tar.bz2
%endif
Source10: samba-print-pdf.sh.bz2
Patch1: smbw.patch.bz2
Patch2: samba-glibc21.patch.bz2
#Patch3: network-recycle_bin.patch.bz2
Patch5: samba-2.2.0-gawk.patch.bz2
Patch12: samba-2.2.0-buildroot.patch.bz2
#Patch16: samba-mkdir.patch.bz2
Patch17: samba-3.0-smbmount-sbin.patch.bz2
#Patches 20-22 to get packaging synced from 2.2.2 to 3.0
Patch20: samba-3.0-smb.conf.patch.bz2
Patch21: samba-3.0-winbind.init.patch.bz2
Patch22: samba-3.0-system-auth-winbind.patch.bz2
Patch23: samba-3alpha20-destdir.patch.bz2
#Patch24: samba-3.0alpha13-installman.patch.bz2
Patch25: samba-3.0alpha13-config_lang.patch.bz2
#Patch 26 belongs with 20-22
Patch26: samba-3.0-smb.init.patch.bz2
Requires: pam >= 0.64, samba-common = %{version}
BuildRequires: pam-devel autoconf readline-devel
%if %build_acl
BuildRequires: libacl-devel
%endif
%if %build_mdk72
BuildRequires: cups-devel
%else
BuildRequires: libcups-devel
%endif
%if %build_ldap
BuildRequires: libldap-devel
%endif
%if %build_ads
BuildRequires: libldap-devel krb5-devel
%endif
BuildRoot: %{_tmppath}/%{name}-root
Prefix: /usr
Prereq: /sbin/chkconfig /bin/mktemp /usr/bin/killall
Prereq: fileutils sed /bin/grep

%description
Samba provides an SMB server which can be used to provide
network services to SMB (sometimes called "Lan Manager")
clients, including various versions of MS Windows, OS/2,
and other Linux machines. Samba also provides some SMB
clients, which complement the built-in SMB filesystem
in Linux. Samba uses NetBIOS over TCP/IP (NetBT) protocols
and does NOT need NetBEUI (Microsoft Raw NetBIOS frame)
protocol.

Samba-3.0 features working NT Domain Control capability and
includes the SWAT (Samba Web Administration Tool) that
allows samba's smb.conf file to be remotely managed using your
favourite web browser. For the time being this is being
enabled on TCP port 901 via xinetd. SWAT is now included in
it's own subpackage, samba-swat.

Please refer to the WHATSNEW.txt document for fixup information.
This binary release includes encrypted password support.

Please read the smb.conf file and ENCRYPTION.txt in the
docs directory for implementation details.

%if !%build_system
NOTE: This is a prerelease of samba-%{samba_major}, not intended
for production use. If something breaks, file a bug report.
%endif

%if %build_non_default
WARNING: This RPM was built with command-line options. Please
see README.%{name}-mandrake-rpm in the documentation for
more information.
%endif

%package server
URL:	http://www.samba.org
Summary: Samba (SMB) server programs.
Requires: %{name}-common = %{version}
Group: Networking/Other
Provides: samba
Obsoletes: samba
%if %build_system
Obsoletes: samba-server-ldap
Obsoletes: samba3-server
%else
Provides: samba-server
%endif

%description server
Samba-server provides a SMB server which can be used to provide
network services to SMB (sometimes called "Lan Manager")
clients. Samba uses NetBIOS over TCP/IP (NetBT) protocols
and does NOT need NetBEUI (Microsoft Raw NetBIOS frame)
protocol.

Samba-3.0 features working NT Domain Control capability and
includes the SWAT (Samba Web Administration Tool) that
allows samba's smb.conf file to be remotely managed using your
favourite web browser. For the time being this is being
enabled on TCP port 901 via xinetd. SWAT is now included in
it's own subpackage, samba-swat.

Please refer to the WHATSNEW.txt document for fixup information.
This binary release includes encrypted password support.

Please read the smb.conf file and ENCRYPTION.txt in the
docs directory for implementation details.

%if %build_ldap
This package was compiled with LDAP support, which means that 
passwords can be stored in LDAP or in smbpasswd files.
To migrate your passwords from smbpasswd into LDAP, try
examples/LDAP/import_smbpasswd.pl using:
%{_datadir}/%{name}/scripts/import_smbpasswd.pl </etc/%{name}/smbpasswd

Scripts for managing users in LDAP have been added to 
%{_datadir}/%{name}/scripts, configuration is in /etc/%{name}/smbldap_conf.pm
%endif

%if !%build_system
NOTE: This is a prerelease of samba-%{samba_major}, not intended
for production use. If something breaks, file a bug report.
%endif

%package client
URL:	http://www.samba.org
Summary: Samba (SMB) client programs.
Group: Networking/Other
Requires: %{name}-common = %{version}
Obsoletes: smbfs
%if %build_system
Obsoletes: samba3-client
%else
Provides: samba-client
%endif

%description client
Samba-client provides some SMB clients, which complement the built-in
SMB filesystem in Linux. These allow the accessing of SMB shares, and
printing to SMB printers.

%if !%build_system
NOTE: This is a prerelease of samba-%{samba_major}, not intended
for production use. If something breaks, file a bug report.
%endif

%package common
URL:	http://www.samba.org
Summary: Files used by both Samba servers and clients.
Group: System/Servers
%if %build_system
Obsoletes: samba-common-ldap
Obsoletes: samba3-common
%else
Provides: samba-common
%endif

%description common
Samba-common provides files necessary for both the server and client
packages of Samba.

%if !%build_system
NOTE: This is a prerelease of samba-%{samba_major}, not intended
for production use. If something breaks, file a bug report.
%endif

%package doc
URL:	http://www.samba.org
Summary: Documentation for Samba servers and clients.
Group: System/Servers
Requires: %{name}-common = %{version}
%if %build_system
Obsoletes: samba3-doc
%else
Provides: samba-doc
%endif

%description doc
Samba-doc provides documentation files for both the server and client
packages of Samba.

%if !%build_system
NOTE: This is a prerelease of samba-%{samba_major}, not intended
for production use. If something breaks, file a bug report.
%endif

%package swat
URL:	http://www.samba.org
Summary: The Samba Web Administration Tool.
Requires: %{name}-server = %{version}
Requires: xinetd
Group: System/Servers
%if %build_system
Obsoletes: samba-swat-ldap
Obsoletes: samba3-swat
%else
Provides: samba-swat
%endif

%description swat
SWAT (the Samba Web Administration Tool) allows samba's smb.conf file
to be remotely managed using your favourite web browser. For the time
being this is being enabled on TCP port 901 via xinetd. Note that
SWAT does not use SSL encryption, nor does it preserve comments in
your smb.conf file. Webmin uses SSL encryption by default, and
preserves comments in configuration files, even if it does not display
them, and is therefore the preferred method for remotely managing
Samba.

%if !%build_system
NOTE: This is a prerelease of samba-%{samba_major}, not intended
for production use. If something breaks, file a bug report.
%endif

%if %build_winbind
%package winbind
URL:	http://www.samba.org
Summary: Samba-winbind daemon, utilities and documentation
Group: System/Servers
Requires: %{name}-common = %{version}
%description winbind
Provides the winbind daemon and testing tools to allow authentication 
and group/user enumeration from a Windows or Samba domain controller.
%endif

%if !%build_system
NOTE: This is a prerelease of samba-%{samba_major}, not intended
for production use. If something breaks, file a bug report.
%endif

%if %build_wins
%package -n nss_wins%{samba_major}
URL:	http://www.samba.org
Summary: Name Service Switch service for WINS
Group: System/Servers
Requires: %{name}-common = %{version}
PreReq: glibc
%description -n nss_wins%{samba_major}
Provides the libnss_wins shared library which resolves NetBIOS names to 
IP addresses.
%endif

%if !%build_system
NOTE: This is a prerelease of samba-%{samba_major}, not intended
for production use. If something breaks, file a bug report.
%endif

#Antivirus packages:
%if %build_fprot
%package vscan-fprot
Summary: On-access virus scanning for samba using FPROT
Group: System/Servers
Requires: %{name}-server = %{version}
Autoreq: 0
%description vscan-fprot
A vfs-module for samba to implement on-access scanning using the
FPROT antivirus software (which must be installed to use this).
%endif

%if %build_kaspersky
%package vscan-kaspersky
Summary: On-access virus scanning for samba using Kaspersky
Group: System/Servers
Requires: %{name}-server = %{version}
Autoreq: 0
%description vscan-kaspersky
A vfs-module for samba to implement on-access scanning using the
Kaspersky antivirus software (which must be installed to use this).
%endif

%if %build_mks
%package vscan-mks
Summary: On-access virus scanning for samba using MKS
Group: System/Servers
Requires: %{name}-server = %{version}
Autoreq: 0
%description vscan-mks
A vfs-module for samba to implement on-access scanning using the
MKS antivirus software (which must be installed to use this).
%endif

%if %build_openantivirus
%package vscan-openantivirus
Summary: On-access virus scanning for samba using OpenAntivirus
Group: System/Servers
Requires: %{name}-server = %{version}
Autoreq: 0
%description vscan-openantivirus
A vfs-module for samba to implement on-access scanning using the
OpenAntivirus antivirus software (which must be installed to use this).
%endif

%if %build_sophos
%package vscan-sophos
Summary: On-access virus scanning for samba using Sophos
Group: System/Servers
Requires: %{name}-server = %{version}
Autoreq: 0
%description vscan-sophos
A vfs-module for samba to implement on-access scanning using the
Sophos antivirus software (which must be installed to use this).
%endif

%if %build_symantec
%package vscan-symantec
Summary: On-access virus scanning for samba using Symantec
Group: System/Servers
Requires: %{name}-server = %{version}
Autoreq: 0
%description vscan-symantec
A vfs-module for samba to implement on-access scanning using the
Symantec antivirus software (which must be installed to use this).
%endif

%if %build_trend
%package vscan-trend
Summary: On-access virus scanning for samba using Trend
Group: System/Servers
Requires: %{name}-server = %{version}
Autoreq: 0
%description vscan-trend
A vfs-module for samba to implement on-access scanning using the
Trend antivirus software (which must be installed to use this).
%endif

%prep
# Build a summary of how this RPM was built:
%if %build_acl
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --with acl"
%else
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --without acl"
%endif
%if %build_winbind
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --with winbind"
%else
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --without winbind"
%endif
%if %build_wins
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --with wins"
%else
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --without wins"
%endif
%if %build_ldap
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --with ldap"
%else
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --without ldap"
%endif 
%if %build_ads
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --with ads"
%else
RPM_EXTRA_OPTIONS="$RPM_EXTRA_OPTIONS --without ads"
%endif

%if %build_non_default
echo "Building a non-default rpm with the following command-line arguments:"
echo "$RPM_EXTRA_OPTIONS"
echo "This rpm was built with non-default options, thus, to build ">%{SOURCE7}
echo "an identical rpm, you need to supply the following options">>%{SOURCE7}
echo "at build time: $RPM_EXTRA_OPTIONS">>%{SOURCE7}
echo -e "\n%{name}-%{version}-%{release}\n">>%{SOURCE7}
%else 
echo "This rpm was built with default options">%{SOURCE7}
echo -e "\n%{name}-%{version}-%{release}\n">>%{SOURCE7}
%endif

%if %build_vscan
%setup -q -a 8 -n %{pkg_name}-%{source_ver}
%else
%setup -q -n %{pkg_name}-%{source_ver}
%endif
#%patch111 -p1
%patch1 -p1 -b .smbw
#%patch3 -p1 -b .net-r_bin
%patch5 -p1 -b .gawk
#%patch6 -p1
%patch12 -p1 -b .buildroot
%patch17 -p1 -b .sbin
%patch20 -p1 -b .samba222
%patch21 -p1 -b .samba222
%patch22 -p1 -b .samba222
%patch23 -p1 -b .destdir
#%patch24 -p1 -b .langen
%patch25 -p1 -b .multilang
%patch26 -p1 -b .samba222
cp %{SOURCE7} .

# Make a copy of examples so that we have a clean one for doc:
cp -a examples examples.bin

%if %build_vscan
# put antivirus files in examples.bin/VFS/
for av in fprot kaspersky mks openantivirus sophos symantec trend; do
	cp -a %{vscandir}/$av %{vfsdir}
done
%endif

# Edit some files when not building system samba:
%if !%build_system
perl -pi -e 's/%{pkg_name}/%{name}/g' source/auth/pampass.c
%endif

%build
#%serverbuild
(cd source
autoconf
CPPFLAGS="-I/usr/include/openssl"; export CPPFLAGS
CFLAGS="$RPM_OPT_FLAGS"
%configure      --prefix=%{_prefix} \
                --with-fhs \
                --libdir=/etc/%{name} \
                --sysconfdir=/etc/%{name} \
                --localstatedir=/var \
                --with-configdir=/etc/%{name} \
                --with-codepagedir=/var/lib/%{name}/codepages \
                --with-privatedir=/etc/%{name} \
                --with-swatdir=%{_datadir}/swat%{samba_major} \
                --with-smbmount \
                --with-syslog \
                --with-automount \
                --with-pam \
                --with-vfs \
                --with-utmp \
                --with-msdfs \
                --with-smbwrapper \
		--with-manpages-langs=en	\
		--with-logfilebase=/var/log/%{name} \
		--with-lockdir=/var/cache/%{name} \
		--with-piddir=/var/run/%{name} \
%if !%build_system
		--program-suffix=%{samba_major} \
%endif		
%if %build_acl
		--with-acl-support      \
%endif
%if !%build_ldap
		--with-ldap=no		\
%endif
%if !%build_ads
		--with-ads=no	\
%endif
                --with-quotas
#                --with-pam_smbpass \
#		--with-manpages-langs=en,ja,pl	\

#make CFLAGS="$RPM_OPT_FLAGS -D_GNU_SOURCE" all
make CFLAGS="$RPM_OPT_FLAGS -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE" \
	all smbfilter smbwrapper smbcacls pam_smbpass nsswitch nsswitch/libnss_wins.so debug2html
# Build VFS modules (experimental)
cd ../%vfsdir
%configure	--prefix=%{prefix} \
		--mandir=%{prefix}/share/man
make
#make CFLAGS="$RPM_OPT_FLAGS -I../../source -I../../source/include -I../../source/ubiqx \
#	-I../../source/smbwrapper -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
)

# Build mkntpasswd in examples/LDAP/ for smbldaptools
(
cd examples.bin/LDAP/smbldap-tools/mkntpwd
make
)

# Build antivirus vfs objects:
%if %build_fprot
echo "Building fprot"
%endif
%if %build_kaspersky
echo "Building Kaspersky"
(cd %{vfsdir}/kaspersky;make)
%endif
%if %build_mks
echo "Building mks"
(cd %{vfsdir}/mks;make)
%endif
%if %build_openantivirus
echo "Building OpenAntivirus"
(cd %{vfsdir}/openantivirus;make)
%endif
%if %build_sophos
echo "building sophos"
(cd %{vfsdir}/sophos;make)
%endif
%if %build_symantec
echo "Building symantec"
(cd %{vfsdir}/symantec;make)
%endif
%if %build_trend
echo "Building Trend"
(cd %{vfsdir}/trend;make)
%endif

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT

# Any entries here mean samba makefile is *really* broken:
mkdir -p $RPM_BUILD_ROOT/etc/%{name}
mkdir -p $RPM_BUILD_ROOT/%{_datadir}

(cd source
make DESTDIR=$RPM_BUILD_ROOT install)

#need to stay
mkdir -p $RPM_BUILD_ROOT/sbin
mkdir -p $RPM_BUILD_ROOT/etc/{logrotate.d,pam.d,xinetd.d}
mkdir -p $RPM_BUILD_ROOT/etc/rc.d/init.d
mkdir -p $RPM_BUILD_ROOT/var/cache/%{name}
mkdir -p $RPM_BUILD_ROOT/var/log/%{name}
mkdir -p $RPM_BUILD_ROOT/var/run/%{name}
mkdir -p $RPM_BUILD_ROOT/var/spool/%{name}
mkdir -p $RPM_BUILD_ROOT/var/lib/%{name}/{netlogon,profiles,printers}
mkdir -p $RPM_BUILD_ROOT/var/lib/%{name}/printers/{W32X86,WIN40,W32ALPHA,W32MIPS,W32PPC}
mkdir -p $RPM_BUILD_ROOT/var/lib/%{name}/codepages/src
mkdir -p $RPM_BUILD_ROOT/lib/security
mkdir -p $RPM_BUILD_ROOT%{_prefix}/lib
mkdir -p $RPM_BUILD_ROOT%{_libdir}/%{name}/vfs
mkdir -p $RPM_BUILD_ROOT%{_datadir}/%{name}/scripts

#smbwrapper and pam_winbind not handled by make, pam_smbpass.so doesn't build
install -m 755 source/bin/smbwrapper.so $RPM_BUILD_ROOT%{_libdir}/smbwrapper%{samba_major}.so
#install -m 755 source/bin/pam_smbpass.so $RPM_BUILD_ROOT/lib/security/pam_smbpass.so
install -m 755 source/nsswitch/pam_winbind.so $RPM_BUILD_ROOT/lib/security/pam_winbind%{samba_major}.so

# Install VFS modules
#install -m755 %vfsdir/audit.so $RPM_BUILD_ROOT/%{_libdir}/samba/vfs
#for i in block recycle
#do
# install -m755 %vfsdir/$i/$i.so $RPM_BUILD_ROOT/%{_libdir}/samba/vfs
#done

# Antivirus support:
#	mkdir -p $RPM_BUILD_ROOT%{_libdir}/samba/vfs/vscan
	for av in fprotd kavp mksd oav sophos symantec trend; do
		if [ -d %{vfsdir}/$av -a -e %{vfsdir}/$av/vscan-$av.so ];then
			cp %{vfsdir}/$av/vscan-$av.so \
				$RPM_BUILD_ROOT%{_libdir}/samba/vfs/
		fi
	done
	
#These scripts are not handled by make:
#for i in addtosmbpass mksmbpasswd.sh smbtar convert_smbpasswd
for i in addtosmbpass mksmbpasswd.sh convert_smbpasswd
do
  install -m755 source/script/$i $RPM_BUILD_ROOT/%{_bindir}
done

#libnss_* not handled by make:
# Install the nsswitch library extension file
for i in wins winbind; do
  install -m755 source/nsswitch/libnss_${i}.so $RPM_BUILD_ROOT/lib/libnss_${i}%{samba_major}.so
done
# Make link for wins and winbind resolvers
( cd $RPM_BUILD_ROOT/lib; ln -s libnss_wins%{samba_major}.so libnss_wins%{samba_major}.so.2; ln -s libnss_winbind%{samba_major}.so libnss_winbind%{samba_major}.so.2)

# Install other stuff

#        install -m644 examples/VFS/recycle/recycle.conf $RPM_BUILD_ROOT/etc/samba/
        install -m644 packaging/Mandrake/smb.conf $RPM_BUILD_ROOT/etc/%{name}/smb.conf
        install -m644 packaging/Mandrake/smbusers $RPM_BUILD_ROOT/etc/%{name}/smbusers
        install -m755 packaging/Mandrake/smbprint $RPM_BUILD_ROOT/%{_bindir}
        #install -m755 packaging/RedHat/smbadduser $RPM_BUILD_ROOT/usr/bin
        install -m755 packaging/Mandrake/findsmb $RPM_BUILD_ROOT/%{_bindir}
        install -m755 packaging/Mandrake/smb.init $RPM_BUILD_ROOT/etc/rc.d/init.d/smb%{samba_major}
        install -m755 packaging/Mandrake/smb.init $RPM_BUILD_ROOT/%{_sbindir}/%{name}
	install -m755 packaging/Mandrake/winbind.init $RPM_BUILD_ROOT/etc/rc.d/init.d/winbind%{samba_major}
	install -m755 packaging/Mandrake/winbind.init $RPM_BUILD_ROOT/%{_sbindir}/winbind%{samba_major}
        install -m644 packaging/Mandrake/samba.pamd $RPM_BUILD_ROOT/etc/pam.d/%{name}
	install -m644 packaging/Mandrake/system-auth-winbind.pamd $RPM_BUILD_ROOT/etc/pam.d/system-auth-winbind%{samba_major}
#
        install -m644 $RPM_SOURCE_DIR/samba.log $RPM_BUILD_ROOT/etc/logrotate.d/%{name}
#	install -m644 packaging/Mandrake/samba-slapd-include.conf $RPM_BUILD_ROOT%{_sysconfdir}/%{name}/samba-slapd.include

# Install smbldap-tools scripts:
#for i in examples/LDAP/smbldap-tools/*.pl; do
#	install -m 750 $i $RPM_BUILD_ROOT/%{_datadir}/%{name}/scripts/
#done

#install -m 750 examples/LDAP/smbldap-tools/smbldap_tools.pm $RPM_BUILD_ROOT/%{_datadir}/%{name}/scripts/

# The conf file	
#install -m 640 examples/LDAP/smbldap-tools/smbldap_conf.pm $RPM_BUILD_ROOT/%{_sysconfdir}/%{name}

# Link both smbldap*.pm into vendor-perl (any better ideas?)
#mkdir -p %{buildroot}/%{perl_vendorlib}
#ln -s %{_sysconfdir}/%{name}/smbldap_conf.pm $RPM_BUILD_ROOT/%{perl_vendorlib}
#ln -s %{_datadir}/%{name}/scripts/smbldap_tools.pm $RPM_BUILD_ROOT/%{perl_vendorlib}

#mkntpwd
#install -m750 examples.bin/LDAP/smbldap-tools/mkntpwd/mkntpwd %{buildroot}/%{_sbindir}

# Samba smbpasswd migration script:
install -m700 examples/LDAP/export_smbpasswd.pl $RPM_BUILD_ROOT/%{_datadir}/%{name}/scripts/
install -m700 examples/LDAP/import_smbpasswd.pl $RPM_BUILD_ROOT/%{_datadir}/%{name}/scripts/



# make a conf file for winbind from the default one:
	cat packaging/Mandrake/smb.conf|sed -e  's/^;  winbind/  winbind/g;s/^;  obey pam/  obey pam/g; s/^;   printer admin = @"D/   printer admin = @"D/g;s/^;   password server = \*/   password server = \*/g;s/^;  template/  template/g; s/^   security = user/   security = domain/g' > packaging/Mandrake/smb-winbind.conf
        install -m644 packaging/Mandrake/smb-winbind.conf $RPM_BUILD_ROOT/etc/%{name}/smb-winbind.conf

# Link smbmount to /sbin/mount.smb and /sbin/mount.smbfs
#I don't think it's possible for make to do this ...
        ln -s $RPM_BUILD_ROOT/%{_bindir}/smbmount%{samba_major} $RPM_BUILD_ROOT/sbin/mount.smb%{samba_major}
        ln -s $RPM_BUILD_ROOT/%{_bindir}/smbmount%{samba_major} $RPM_BUILD_ROOT/sbin/mount.smbfs%{samba_major}
        echo 127.0.0.1 localhost > $RPM_BUILD_ROOT/etc/%{name}/lmhosts

# Link smbspool to CUPS (does not require installed CUPS)

        mkdir -p $RPM_BUILD_ROOT/%{_libdir}/cups/backend
        ln -s %{_bindir}/smbspool $RPM_BUILD_ROOT/%{_libdir}/cups/backend/smb%{samba_major}

# xinetd support

        mkdir -p $RPM_BUILD_ROOT/etc/xinetd.d
        install -m644 %{SOURCE3} $RPM_BUILD_ROOT/etc/xinetd.d/swat%{samba_major}

# menu support

mkdir -p $RPM_BUILD_ROOT%{_menudir}
cat > $RPM_BUILD_ROOT%{_menudir}/%{name} << EOF
?package(%{name}):command="gnome-moz-remote http://localhost:901/" needs="gnome" \
icon="swat%{samba_major}.xpm" section="Configuration/Networking" title="Samba Configuration" \
longtitle="The Swat Samba Administration Tool"
?package(%{name}):command="sh -c '\$BROWSER http://localhost:901/'" needs="x11" \
icon="swat%{samba_major}.xpm" section="Configuration/Networking" title="Samba Configuration" \
longtitle="The Swat Samba Administration Tool"
EOF

mkdir -p $RPM_BUILD_ROOT%{_liconsdir} $RPM_BUILD_ROOT%{_iconsdir} $RPM_BUILD_ROOT%{_miconsdir}

bzcat %{SOURCE4} > $RPM_BUILD_ROOT%{_liconsdir}/swat%{samba_major}.xpm
bzcat %{SOURCE5} > $RPM_BUILD_ROOT%{_iconsdir}/swat%{samba_major}.xpm
bzcat %{SOURCE6} > $RPM_BUILD_ROOT%{_miconsdir}/swat%{samba_major}.xpm

bzcat %{SOURCE10}> $RPM_BUILD_ROOT%{_datadir}/%{name}/scripts/print-pdf

# Fix configs when not building system samba:
%if !%build_system
# Work around non-existent but claimed --program-suffix support :-(
for dir in %{_sbindir} %{_bindir};do
    FILES=`find %{buildroot}$dir -type f |grep -v "\."|grep -v %{samba_major}$`
    for OLD in $FILES; do
        NEW=`echo ${OLD}%{samba_major}`
        mv $OLD $NEW
    done
done
# And the man pages too:
FILES=`find %{buildroot}%{_mandir} -type f `
for OLD in $FILES; do
    BASE=`perl -e '$name="'${OLD}'"; print "",($name =~ /(.*?)\.[0-9]/), "\n";'`
    EXT=`echo $OLD|sed -e 's,'${BASE}',,g'`
    NEW=`echo ${BASE}%{samba_major}${EXT}`
    mv $OLD $NEW
done		
# Replace paths in config files and init scripts:
for i in smb winbind;do
	perl -pi -e 's,/subsys/'$i',/subsys/'$i'%{samba_major},g' $RPM_BUILD_ROOT/etc/rc.d/init.d/${i}%{samba_major}
done
for i in /etc/%{name}/smb.conf /etc/rc.d/init.d/smb%{samba_major} %{_sbindir}/%{name} /etc/rc.d/init.d/winbind%{samba_major} %{_sbindir}/winbind%{samba_major} /etc/logrotate.d/%{name} /etc/xinetd.d/swat%{samba_major}; do
	perl -pi -e 's,/%{pkg_name},/%{name},g; s,smbd,%{_sbindir}/smbd%{samba_major},g; s,nmbd,%{_sbindir}/nmbd%{samba_major},g; s,winbindd,%{_sbindir}/winbindd%{samba_major},g; s,/usr/sbin/swat,%{_sbindir}/swat%{samba_major},g' $RPM_BUILD_ROOT/$i;
done
# Fix pam files
perl -pi -e 's/winbind/winbind%{samba_major}/g' $RPM_BUILD_ROOT/etc/pam.d/system-auth-winbind%{samba_major}
# Fix xinetd file for swat:
perl -pi -e 's,/usr/sbin,%{_sbindir},g' $RPM_BUILD_ROOT/etc/xinetd.d/swat%{samba_major}
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%post server

/sbin/chkconfig --level 35 smb%{samba_major} on

# Add a unix group for samba machine accounts
groupadd -frg 421 machines

# Migrate tdb's from /var/lock/samba (taken from official samba spec file):
for i in /var/lock/samba/*.tdb
do
if [ -f $i ]; then
	newname=`echo $i | sed -e's|var\/lock\/samba|var\/cache\/samba|'`
	echo "Moving $i to $newname"
	mv $i $newname
fi
done

# Remove the transient tdb files (modified from version in off. samba spec:
for TDB in brlock unexpected locking messages; do
        if [ -e /var/cache/%{name}/$TDB.tdb ]; then
                rm -f /var/cache/%{name}/$TDB.tdb;
        fi;
done

if [ -d /var/lock/samba ]; then
        rm -rf /var/lock/samba
fi

%post common
# Basic migration script for pre-2.2.1 users,
# since smb config moved from /etc to /etc/samba

# Let's create a proper /etc/samba/smbpasswd file
[ -f /etc/%{name}/smbpasswd ] || {
	echo "Creating password file for samba..."
	touch /etc/%{name}/smbpasswd
}

# And this too, in case we don't have smbd to create it for us
[ -f /var/cache/%{name}/unexpected.tdb ] || {
	touch /var/cache/%{name}/unexpected.tdb
}

# Let's define the proper paths for config files
perl -pi -e 's/(\/etc\/)(smb)/\1%{name}\/\2/' /etc/%{name}/smb.conf

# Fix the logrotate.d file from smb and nmb to smbd and nmbd
if [ -f /etc/logrotate.d/samba ]; then
        perl -pi -e 's/smb /smbd /' /etc/logrotate.d/samba
        perl -pi -e 's/nmb /nmbd /' /etc/logrotate.d/samba
fi

# And not loose our machine account SID
[ -f /etc/MACHINE.SID ] && mv -f /etc/MACHINE.SID /etc/%{name}/ ||:

%if %build_winbind
%post winbind
if [ $1 = 1 ]; then
    /sbin/chkconfig winbind%{samba_major} on
    cp -af /etc/nsswitch.conf /etc/nsswitch.conf.rpmsave
    cp -af /etc/nsswitch.conf /etc/nsswitch.conf.rpmtemp
    for i in passwd group;do
        grep ^$i /etc/nsswitch.conf |grep -v 'winbind%{samba_major}' >/dev/null
        if [ $? = 0 ];then
            echo "Adding a winbind%{samba_major} entry to the $i section of /etc/nsswitch.conf"
            awk '/^'$i'/ {print $0 " winbind%{samba_major}"};!/^'$i'/ {print}' /etc/nsswitch.conf.rpmtemp >/etc/nsswitch.conf;
	    cp -af /etc/nsswitch.conf /etc/nsswitch.conf.rpmtemp
        else
            echo "$i entry found in /etc/nsswitch.conf"
        fi
    done
    if [ -f /etc/nsswitch.conf.rpmtemp ];then rm -f /etc/nsswitch.conf.rpmtemp;fi
fi

%preun winbind
if [ $1 = 0 ]; then
	echo "Removing winbind%{samba_major} entries from /etc/nsswitch.conf"
	perl -pi -e 's/ winbind%{samba_major}//' /etc/nsswitch.conf

	/sbin/chkconfig winbind%{samba_major} reset
fi
%endif %build_winbind

%if %build_wins
%post -n nss_wins%{samba_major}
if [ $1 = 1 ]; then
    cp -af /etc/nsswitch.conf /etc/nsswitch.conf.rpmsave
    grep '^hosts' /etc/nsswitch.conf |grep -v 'wins%{samba_major}' >/dev/null
    if [ $? = 0 ];then
        echo "Adding a wins entry to the hosts section of /etc/nsswitch.conf"
        awk '/^hosts/ {print $0 " wins%{samba_major}"};!/^hosts/ {print}' /etc/nsswitch.conf.rpmsave >/etc/nsswitch.conf;
    else
        echo "wins entry found in /etc/nsswitch.conf"
    fi
#    else
#        echo "Upgrade, leaving nsswitch.conf intact"
fi

%preun -n nss_wins%{samba_major}
if [ $1 = 0 ]; then
	echo "Removing wins entry from /etc/nsswitch.conf"
	perl -pi -e 's/ wins%{samba_major}//' /etc/nsswitch.conf
#else
#	echo "Leaving /etc/nsswitch.conf intact"
fi
%endif %build_wins

%preun

/sbin/chkconfig --level 35 smb%{samba_major} reset

if [ $1 = 0 ] ; then
%_preun_service smb%{samba_major}
#    /sbin/chkconfig --level 35 smb reset
# Let's not loose /var/cache/samba

#    for i in browse.dat wins.dat brlock.tdb unexpected.tdb connections.tdb \
#locking.tdb messages.tdb;do
#        if [ -e /var/cache/samba/$i ]; then
#                mv -f /var/cache/samba/$i /var/cache/samba/$i.BAK
#        fi;
#    done
    if [ -d /var/log/%{name} ]; then
      rm -rf /var/log/%{name}/*
    fi
    if [ -d /var/cache/%{name} ]; then
      mv -f /var/cache/%{name} /var/cache/%{name}.BAK
    fi
fi

%post swat
if [ -f /var/lock/subsys/xinetd ]; then
        service xinetd reload >/dev/null 2>&1 || :
fi
%update_menus

%postun swat

# Remove swat entry from xinetd
if [ $1 = 0 -a -f /etc/xinetd.conf ] ; then
rm -f /etc/xinetd.d/swat%{samba_major}
	service xinetd reload &>/dev/null || :
fi

if [ "$1" = "0" -a -x /usr/bin/update-menus ]; then /usr/bin/update-menus || true ; fi

%clean_menus

%triggerpostun -- samba < 1.9.18p7

if [ $1 != 0 ]; then
    /sbin/chkconfig --level 35 smb on
fi

%triggerpostun -- samba < 2.0.5a-3, samba >= 2.0.0

if [ $1 != 0 ]; then
        [ ! -d /var/lock/samba ] && mkdir -m 0755 /var/lock/samba ||:
        [ ! -d /var/spool/samba ] && mkdir -m 1777 /var/spool/samba ||:
        [ -f /etc/inetd.conf ] && chmod 644 /etc/services /etc/inetd.conf ||:
fi

%files server
%defattr(-,root,root)
#%attr(-,root,root) /sbin/*
%{_sbindir}/%{name}
%{_sbindir}/smbd%{samba_major}
%{_sbindir}/nmbd%{samba_major}
#%{_sbindir}/smbcontrol
#%{_sbindir}/mkntpwd
#%{prefix}/bin/addtosmbpass
%{_bindir}/mksmbpasswd.sh
%{_bindir}/smbstatus%{samba_major}
%{_bindir}/convert_smbpasswd%{samba_major}
%{_bindir}/pdbedit%{samba_major}
%{_bindir}/smbgroupedit%{samba_major}
#%attr(755,root,root) /lib/security/pam_smbpass*
#/usr/share/swat
%attr(-,root,root) %config(noreplace) /etc/%{name}/smbusers
%attr(-,root,root) %config /etc/rc.d/init.d/smb%{samba_major}
%attr(-,root,root) %config(noreplace) /etc/logrotate.d/%{name}
%attr(-,root,root) %config(noreplace) /etc/pam.d/%{name}
#%attr(-,root,root) %config(noreplace) /etc/%{name}/samba-slapd.include
%{_mandir}/man1/smbstatus*.1*
%{_mandir}/man5/smbpasswd*.5*
%{_mandir}/man7/samba*.7*
%{_mandir}/man8/smbd*.8*
%{_mandir}/man8/nmbd*.8*
%{_mandir}/man1/smbcontrol*.1*
%{_mandir}/man5/lmhosts*.5*
%{_mandir}/man5/smb.conf*.5*
%attr(775,root,root) %dir /var/lib/%{name}/netlogon
%attr(775,root,root) %dir /var/lib/%{name}/profiles
%attr(775,root,root) %dir /var/lib/%{name}/printers/*
%dir /var/cache/%{name}
%dir /var/log/%{name}
%attr(1777,root,root) %dir /var/spool/%{name}
%dir %{_datadir}/%{name}/scripts
#%attr(0755,root,root) %{_datadir}/%{name}/scripts/print-pdf
#%attr(0750,root,root) %{_datadir}/%{name}/scripts/smbldap*.pl
#%attr(0640,root,root) %config(noreplace) %{_sysconfdir}/%{name}/smbldap_conf.pm
#%attr(0644,root,root) %{_datadir}/%{name}/scripts/smbldap_tools.pm
#%{perl_vendorlib}/*.pm
%attr(0700,root,root) %{_datadir}/%{name}/scripts/*port_smbpasswd.pl


%files doc
%defattr(-,root,root)
%doc README COPYING Manifest Read-Manifest-Now
%doc WHATSNEW.txt Roadmap
%doc README.%{name}-mandrake-rpm
%doc docs
%doc examples
%doc swat/README
%attr(-,root,root) %{_datadir}/swat%{samba_major}/using_samba/*

%files swat
%defattr(-,root,root)
%config(noreplace) /etc/xinetd.d/swat%{samba_major}
#%attr(-,root,root) /sbin/*
%{_sbindir}/swat%{samba_major}
%{_menudir}/%{name}
%{_miconsdir}/*.xpm
%{_liconsdir}/*.xpm
%{_iconsdir}/*.xpm
%attr(-,root,root) %{_datadir}/swat%{samba_major}/help/*
%attr(-,root,root) %{_datadir}/swat%{samba_major}/images/*
%attr(-,root,root) %{_datadir}/swat%{samba_major}/include/*
%{_mandir}/man8/swat*.8*

%files client
%defattr(-,root,root)
%ifnarch alpha
/sbin/mount.smb%{samba_major}
/sbin/mount.smbfs%{samba_major}
%attr(755,root,root) %{_bindir}/smbmount%{samba_major}
%attr(4755,root,root) %{_bindir}/smbumount%{samba_major}
#%attr(4755,root,root) %{_sbindir}/smbmnt
%{_mandir}/man8/smbmnt*.8*
%{_mandir}/man8/smbmount*.8*
%{_mandir}/man8/smbumount*.8*
%endif
%{_bindir}/nmblookup%{samba_major}
%{_bindir}/findsmb%{samba_major}
%{_bindir}/smbclient%{samba_major}
%{_bindir}/smbprint%{samba_major}
%{_bindir}/smbtar%{samba_major}
%{_bindir}/smbspool%{samba_major}
# Link of smbspool to CUPS
/%{_libdir}/cups/backend/smb%{samba_major}
/%{_mandir}/man1/nmblookup*.1*
/%{_mandir}/man1/findsmb*.1*
/%{_mandir}/man1/smbclient*.1*
/%{_mandir}/man1/smbtar*.1*

%files common
%defattr(-,root,root)
%dir /var/cache/%{name}
%dir /var/log/%{name}
%dir /var/run/%{name}
#%{_bindir}/make_smbcodepage
#%{_bindir}/make_unicodemap
%{_bindir}/testparm%{samba_major}
%{_bindir}/testprns%{samba_major}
#%{_bindir}/make_printerdef
%{_bindir}/rpcclient%{samba_major}
%{_bindir}/smbsh%{samba_major}
%{_bindir}/smbpasswd%{samba_major}
%{_bindir}/net%{samba_major}
%{_bindir}/smbtree%{samba_major}
%{_libdir}/smbwrapper%{samba_major}.so
%attr(-,root,root) %config(noreplace) /etc/%{name}/smb.conf
%attr(-,root,root) %config(noreplace) /etc/%{name}/smb-winbind.conf
%attr(-,root,root) %config(noreplace) /etc/%{name}/lmhosts
%attr(-,root,root) /var/lib/%{name}/codepages
%{_mandir}/man1/make_smbcodepage*.1*
%{_mandir}/man1/make_unicodemap*.1*
%{_mandir}/man1/testparm*.1*
%{_mandir}/man1/smbsh*.1*
%{_mandir}/man1/testprns*.1*
%{_mandir}/man5/smb.conf*.5*
%{_mandir}/man5/lmhosts*.5*
%{_mandir}/man8/smbpasswd*.8*
/%{_mandir}/man1/smbcacls*.1*

%if %build_winbind
%files winbind
%defattr(-,root,root)
%{_sbindir}/winbindd%{samba_major}
%{_bindir}/wbinfo%{samba_major}
%attr(755,root,root) /lib/security/pam_winbind*
%attr(755,root,root) /lib/libnss_winbind%{samba_major}*
%attr(-,root,root) %config /etc/rc.d/init.d/winbind%{samba_major}
%attr(-,root,root) %config(noreplace) /etc/pam.d/system-auth-winbind*
%{_mandir}/man8/winbindd*.8*
%{_mandir}/man1/wbinfo*.1*
%endif

%if %build_wins
%files -n nss_wins%{samba_major}
%defattr(-,root,root)
%attr(755,root,root) /lib/libnss_wins%{samba_major}.so*
%endif

#Files for antivirus support:
%if %build_fprot
%files vscan-fprot
%defattr(-,root,root)
%{_libdir}/%{name}/vfs/vscan-fprotd.so
%doc examples.bin/VFS/fprot/INSTALL
%endif

%if %build_kaspersky
%files vscan-kaspersky
%defattr(-,root,root)
%{_libdir}/%{name}/vfs/vscan-kavp.so
%doc examples.bin/VFS/kaspersky/INSTALL
%endif

%if %build_mks
%files vscan-mks
%defattr(-,root,root)
%{_libdir}/%{name}/vfs/vscan-mksd.so
%doc examples.bin/VFS/mks/INSTALL
%endif

%if %build_openantivirus
%files vscan-openantivirus
%defattr(-,root,root)
%{_libdir}/%{name}/vfs/vscan-oav.so
%doc examples.bin/VFS/openantivirus/INSTALL
%endif

%if %build_sophos
%files vscan-sophos
%defattr(-,root,root)
%{_libdir}/%{name}/vfs/vscan-sophos.so
%doc examples.bin/VFS/sophos/INSTALL
%endif

%if %build_symantec
%files vscan-symantec
%defattr(-,root,root)
%{_libdir}/%{name}/vfs/vscan-symantec.so
%doc examples.bin/VFS/symantec/INSTALL
%endif

%if %build_trend
%files vscan-trend
%defattr(-,root,root)
%{_libdir}/%{name}/vfs/vscan-trend.so
%doc examples.bin/VFS/trend/INSTALL
%endif

%changelog
* Mon Oct 28 2002 Buchan Milne <bgmilne@linux-mandrake.com> 3.0-0.alpha20.3mdk
- Fix mount.smbfs3 pointing to smbmount not in package
- Remove unnecessary lines from install (now done by make)
- Build with ldap and ads on all releases by default
- Put av-stuff back

* Mon Oct 21 2002 Buchan Milne <bgmilne@linux-mandrake.com> 3.0-0.alpha20.2mdk
- When not building as system samba, avoid conflicting with system samba
- Macro-ize as much as possible for above (aka finish cleanups)
- Fix paths in init scripts and logrotate and xinetd
- Fix provides and obsoletes so as to provide samba, but not obsolete
  current stable until we have a stable release (when it's the system samba).
- Add warnings to descriptions when not system samba.
- This is now parallel installable with the normal samba release, for easy
  testing. It shouldn't touch existing installations. Of course, only
  one samba at a time on the same interface!

* Sat Sep 28 2002 Buchan Milne <bgmilne@linux-mandrake.com> 3.0-0.alpha20.1mdk
- Merge with 2.2.6pre2.2mdk
- Detect alpha- and beta-, along with pre-releases

* Tue Feb 05 2002 Buchan Milne <bgmilne@cae.co.za> 3.0-alpha14-0.1mdk
- Sync with 2.2.3-2mdk (new --without options, detect when 
  building for a different distribution.

* Mon Feb 04 2002 Buchan Milne <bgmilne@cae.co.za> 3.0-alpha14-0.0mdk
- Sync with 2.2.2-10mdk, which added build-time options --with ldap,
  winbind, acl, wins, mdk72, mdk80, mdk81, mdk82, cooker. Added
  warning in description if built with these options.

* Wed Jan 23 2002 Buchan Milne <bgmilne@cae.co.za> 3.0-alpha13-0.2mdk
- Added %%if's for build_ads, which hopefully will add Active Directory
  Support (by request).

* Thu Jan 17 2002 Buchan Milne <bgmilne@cae.co.za> 3.0-alpha13-0.1mdk
- More syncing with 2.2 rpm (post and postun scripts)
- Testing without ldap

* Thu Jan 17 2002 Buchan Milne <bgmilne@cae.co.za> 3.0-alpha13-0.0mdk
- 3.0-alpha13
- Fixed installman.sh patch.

* Wed Jan 09 2002 Buchan Milne <bgmilne@cae.co.za> 3.0-alpha12-0.1mdk
- Fixed %post and %preun for nss_wins, added %post and %preun for
  samba-winbind (chkconfig and winbind entries in nsswitch.conf)

* Sun Dec 23 2001 Buchan Milne <bgmilne@cae.co.za> 3.0-alpha12-0.0mdk
- 3.0-alpha12
- Sync up with changes made in 2.2.2 to support Mandrake 8.0, 7.2
- Added new subpackage for swat
- More %if's for ldap.

* Thu Dec 20 2001 Buchan Milne <bgmilne@cae.co.za> 3.0-alpha11-0.0mdk
- 3.0-alpha11

* Wed Dec 19 2001 Buchan Milne <bgmilne@cae.co.za> 3.0alpha10-0.0mdk
- 3.0-alpha10

* Tue Dec 18 2001 Buchan Milne <bgmilne@cae.co.za> 3.0alpha9-0.0mdk
- 3.0-alpha9

* Mon Dec 17 2001 Buchan Milne <bgmilne@cae.co.za> 3.0alpha8-0.1mdk
- Added net command to %files common, pdbedit and smbgroupedit to
  %files, s/%{prefix}\/bin/%{_bindir}/ (the big cleanup).
  Added patch to smb.init from 2.2.2 (got missed with 3.0-alpha1 patches)

* Sun Dec 16 2001 Buchan Milne <bgmilne@cae.co.za> 3.0alpha8-0.0mdk
- Patch for installman.sh to handle lang=en correctly (p24)
- added --with-manpages-langs=en,ja,pl (translated manpages), but there
  aren't any manpages for these languages yet ... so we still
  need %dir and %doc entries for them ...
- patch (p25) to configure.in to support more than 2 languages.
- addtosmbpass seems to have returned for now, but make_* have disappeared!

* Fri Dec 14 2001 Buchan Milne <bgmilne@cae.co.za> 3.0alpha6-0.0mdk
- DESTDIR patch for Makefile.in (p23), remove a lot of %install scripts
  this forces move of smbcontrol and smbmnt to %{prefix}/bin
  removed --with-pam_smbpass as it doesn't compile.

* Thu Dec 06 2001 Buchan Milne <bgmilne@cae.co.za> 3.0-0.0alpha1mdk
- Samba 3.0alpha1 released (we missed Samba 3.0alpha0!)
- Redid smbmount-sbin patch and smb.conf patch (20), removed xfs quota patch 
  (applied upstream), removed ook-patch (codepage directory totally different).
- Added winbind.init (21) and system-auth-winbind.pamd (22). Patches 20-23 
  should be applied upstream before 3.0 ships ...

* Wed Dec 05 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.2-6mdk
- fixed typo in system-auth-winbind.pamd (--Thanks J. Gluck).
- fixed %post xxx problem (smb not started in chkconfig --Thanks Viet & B. Kenworthy).

* Fri Nov 23 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.2-5mdk
- Had to remove the network recycle bin patch: it seems to mess up 
  file deletion from windows (files appear to be "already in use")

* Tue Nov 13 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.2-4mdk
- added network recycle bin patch:
  <http://www.amherst.edu/~bbstone/howto/samba.html>
- added "recycle bin = .recycled" parameter in smb.conf [homes].
- fixed winbind/nss_wins perms (oh no I don't own that stuff ;o)

* Mon Nov 12 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.2-3mdk
- added %build 8.0 and 7.2, for tweakers to play around.
- changed configure options:
  . removed --with-mmap, --with-netatalk (obsolete).
  . added --with-msdfs, --with-vfs (seems stable, but still need testing).

* Mon Nov 12 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.2-2mdk
- rebuilt with winbind and nss_wins enabled.

* Wed Oct 31 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.2-1mdk
- Rebuilt on cooker.

* Wed Oct 31 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.992mdk
- Patch for smb.conf to fix incorrect lpq command, typo in winbind,
  and add sample linpopup command. Added print driver directories.
- New XFS quota patch (untested!, samba runs, but do quotas work? We
  can't check yet since the kernel doesn't seem to support XFS quotas!)

* Fri Oct 19 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.2-0.99mdk
- New samba.spec, almost ready for winbind operations. OLA for Buchan Milne
  Who did a tremendous integration work on 2.2.2.
  Rebuild on cooker, please test XFS (ACLs and quotas) again...
  
* Mon Oct 15 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.9mdk
- Samba-2.2.2. released! Use %defines to determine which subpackages
  are built and which Mandrake release we are buiding on/for (hint: define 
  build_mdk81 1 for Mandrake 8.1 updates)

* Sun Oct 14 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.20011014mdk
- %post and %postun for nss_wins

* Wed Oct 10 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.20011010mdk
- New CVS snapshot, /etc/pam.d/system-auth-winbind added
  with configuration to allow easy winbind setup.
  
* Sun Oct 7 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.20011007mdk
- Added new package nss_wins and moved smbpasswd to common (required by
  winbind).

* Sat Oct 6 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.20011006mdk
- Added new package winbind.

* Mon Oct 1 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.20011001mdk
- Removed patch to smb init.d file (applied in cvs)

* Sun Sep 30 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.20010930mdk
- Added winbind init script, which still needs to check for running nmbd.

* Thu Sep 27 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.2-0.20010927mdk
- Built from samba-2.2.2-pre cvs, added winbindd, wbinfo, nss_winbind and 
  pam_winbind, moved pam_smbpass from samba-common to samba. We still
  need a start-up script for winbind, or need to modify existing one.
  
* Mon Sep 10 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1a-15mdk
- Enabled acl support (XFS acls now supported by kernel-2.4.8-21mdk thx Chmou)
  Added smbd patch to support XFS quota (Nathan Scott)
  
* Mon Sep 10 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1a-14mdk
- Oops! smbpasswd created in wrong directory...

* Tue Sep 06 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1a-13mdk
- Removed a wrong comment in smb.conf.
  Added creation of smbpasswd during install.

* Mon Aug 27 2001 Pixel <pixel@mandrakesoft.com> 2.2.1a-12mdk
- really less verbose %%post

* Sat Aug 25 2001 Geoffrey Lee <snailtalk@mandrakesoft.com> 2.2.1a-11mdk
- Fix shared libs in /usr/bin silliness.

* Thu Aug 23 2001 Pixel <pixel@mandrakesoft.com> 2.2.1a-10mdk
- less verbose %%post

* Wed Aug 22 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.1a-9mdk
- Added smbcacls (missing in %files), modification to smb.conf: ([printers]
  is still needed, even with point-and-print!, user add script should
  use name and not gid, since we may not get the gid . New script for
  putting manpages in place (still need to be added in %files!). Moved
  smbcontrol to sbin and added it and its man page to %files.

* Wed Aug 22 2001 Pixel <pixel@mandrakesoft.com> 2.2.1a-8mdk
- cleanup /var/lib/samba/codepage/src

* Tue Aug 21 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1a-7mdk
- moved codepage generation to %install and codepage dir to /var/lib/samba

* Tue Aug 21 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1a-6mdk
- /lib/* was in both samba and samba-common
  Introducing samba-doc: "alas, for the sake of thy modem, shalt thou remember
  when Samba was under the Megabyte..."

* Fri Aug 03 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1a-5mdk
- Added "the gc touch" to smbinit through the use of killall -0 instead of
  grep cupsd | grep -v grep (too many greps :o)

* Wed Jul 18 2001 Stefan van der Eijk <stefan@eijk.nu> 2.2.1a-4mdk
- BuildRequires: libcups-devel
- Removed BuildRequires: openssl-devel

* Fri Jul 13 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1a-3mdk
- replace chkconfig --add/del with --level 35 on/reset.

* Fri Jul 13 2001 Geoffrey Lee <snailtalk@mandrakesoft.cm> 2.2.1a-2mdk
- Replace discription s/inetd/xinetd/, we all love xinetd, blah.

* Thu Jul 12 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.1a-1mdk
- Bugfix release. Fixed add user script, added print$ share and printer admin
  We need to test interaction of new print support with CUPS, but printer
  driver uploads should work.

* Wed Jul 11 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-17mdk
- fixed smb.conf a bit, rebuilt on cooker.

* Tue Jul 10 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.1-16mdk
- Finally, samba 2.2.1 has actually been release. At least we were ready!
  Cleaned up smb.conf, and added some useful entries for domain controlling.
  Migrated changes made in samba's samba2.spec for 2.2.1  to this file.
  Added groupadd command in post to create a group for samba machine accounts.
  (We should still check the postun, samba removes pam, logs and cache)

* Tue Jun 26 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-15mdk
- fixed smbwrapper compile options.

* Tue Jun 26 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-14mdk
- added LFS support.
  added smbwrapper support (smbsh)

* Wed Jun 20 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-13mdk
- /sbin/mount.smb and /sbin/mount.smbfs now point to the correct location
  of smbmount (/usr/bin/smbmount)

* Tue Jun 19 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-12mdk
- smbmount and smbumount are now in /usr/bin and SUID.
  added ||: to triggerpostun son you don't get error 1 anymore when rpm -e
  Checked the .bz2 sources with file *: everything is OK now (I'm so stupid ;o)!

* Tue Jun 19 2001 Geoffrey Lee <snailtalk@mandrakesoft.com> 2.2.1-11mdk
- s/Copyright/License/;
- Stop Sylvester from pretending .gz source to be .bz2 source via filename
  aka really bzip2 the source.

* Mon Jun 18 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-10mdk
- changed Till's startup script modifications: now samba is being reloaded
  automatically 1 minute after it has started (same reasons as below in 9mdk)
  added _post_ and _preun_ for service smb
  fixed creation of /var/lib/samba/{netlogon,profiles} (%dir was missing)

* Thu Jun 14 2001 Till Kamppeter <till@mandrakesoft.com> 2.2.1-9mdk
- Modified the Samba startup script so that in case of CUPS being used as
  printing system Samba only starts when the CUPS daemon is ready to accept
  requests. Otherwise the CUPS queues would not appear as Samba shares.

* Mon Jun 11 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-8mdk
- patched smbmount.c to have it call smbmnt in sbin (thanks Seb).

* Wed May 30 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-7mdk
- put SWAT menu icons back in place.

* Mon May 28 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-6mdk
- OOPS! fixed smbmount symlinks

* Mon May 28 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-5mdk
- removed inetd postun script, replaced with xinetd.
  updated binary list (smbcacls...)
  cleaned samba.spec

* Mon May 28 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.1-4mdk
- Changed configure options to point to correct log and codepage directories,
  added crude script to fix logrotate file for new log file names, updated
  patches to work with current CVS.

* Thu May 24 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-3mdk
- Cleaned and updated the %files section.

* Sat May 19 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.1-2mdk
- Moved all samba files from /etc to /etc/samba (Thanks DomS!).
  Fixed fixinit patch (/etc/samba/smb.conf)

* Fri May 18 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.1-1mdk
- Now use packaging/Mandrake/smb.conf, removed unused and obsolete
  patches, moved netlogon and profile shares to /var/lib/samba in the
  smb.conf to match the spec file. Added configuration for ntlogon to
  smb.conf. Removed pam-foo, fixinit and makefilepath patches. Removed
  symlink I introduced in 2.2.0-1mdk

* Thu May 3 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.0-5mdk
- Added more configure options. Changed Description field (thx John T).

* Wed Apr 25 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.0-4mdk
- moved netlogon and profiles to /var/lib/samba by popular demand ;o)

* Tue Apr 24 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.0-3mdk
- moved netlogon and profiles back to /home.

* Fri Apr 20 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.2.0-2mdk
- fixed post inetd/xinetd script&

* Thu Apr 19 2001 Buchan Milne <bgmilne@cae.co.za> 2.2.0-1mdk
- Upgrade to 2.2.0. Merged most of 2.0.7-25mdk's patches (beware
  nasty "ln -sf samba-%{ver} ../samba-2.0.7" hack to force some patches
  to take. smbadduser and addtosmbpass seem to have disappeared. Moved
  all Mandrake-specific files to packaging/Mandrake and made patches
  from those shipped with samba. Moved netlogon to /home/samba and added
  /home/samba/profiles. Added winbind,smbfilter and debug2html to make command.

* Thu Apr 12 2001 Frederic Crozat <fcrozat@mandrakesoft.com> 2.0.7-25mdk
- Fix menu entry and provide separate menu entry for GNOME
  (nautilus doesn't support HTTP authentication yet)
- Add icons in package

* Fri Mar 30 2001 Frederic Lepied <flepied@mandrakesoft.com> 2.0.7-24mdk
- use new server macros

* Wed Mar 21 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-23mdk
- check whether /etc/inetd.conf exists (upgrade) or not (fresh install).

* Thu Mar 15 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-22mdk
- spec cosmetics, added '-r' option to lpr-cups command line so files are
  removed from /var/spool/samba after printing.

* Tue Mar 06 2001 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-21mdk
- merged last rh patches.

* Thu Nov 23 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-20mdk
- removed dependencies on cups and cups-devel so one can install samba without using cups
- added /home/netlogon

* Mon Nov 20 2000 Till Kamppeter <till@mandrakesoft.com> 2.0.7-19mdk
- Changed default print command in /etc/smb.conf, so that the Windows
  driver of the printer has to be used on the client.
- Fixed bug in smbspool which prevented from printing from a
  Linux-Samba-CUPS client to a Windows server through the guest account.

* Mon Oct 16 2000 Till Kamppeter <till@mandrakesoft.com> 2.0.7-18mdk
- Moved "smbspool" (Samba client of CUPS) to the samba-client package

* Sat Oct 7 2000 Stefan van der Eijk <s.vandereijk@chello.nl> 2.0.7-17mdk
- Added RedHat's "quota" patch to samba-glibc21.patch.bz2, this fixes
  quota related compile problems on the alpha.

* Wed Oct 4 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-16mdk
- Fixed 'guest ok = ok' flag in smb.conf

* Tue Oct 3 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-15mdk
- Allowed guest account to print in smb.conf
- added swat icon in menu

* Tue Oct 3 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-14mdk
- Removed rh ssl patch and --with-ssl flag: not appropriate for 7.2

* Tue Oct 3 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-13mdk
- Changed fixinit patch.
- Changed smb.conf for better CUPS configuration.
- Thanks Fred for doing this ---vvv.

* Tue Oct  3 2000 Frederic Lepied <flepied@mandrakesoft.com> 2.0.7-12mdk
- menu entry for web configuration tool.
- merge with rh: xinetd + ssl + pam_stack.
- Added smbadduser rh-bugfix w/o relocation of config-files.

* Mon Oct  2 2000 Frederic Lepied <flepied@mandrakesoft.com> 2.0.7-11mdk
- added build requires on cups-devel and pam-devel.

* Mon Oct  2 2000 Till Kamppeter <till@mandrakesoft.com> 2.0.7-10mdk
- Fixed smb.conf entry for CUPS: "printcap name = lpstat", "lpstats" was
  wrong.

* Mon Sep 25 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-9mdk
- Cosmetic changes to make rpmlint more happy

* Wed Sep 11 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-8mdk
- added linkage to the using_samba book in swat

* Fri Sep 01 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-7mdk
- Added CUPS support to smb.conf
- Added internationalization options to smb.conf [Global]

* Wed Aug 30 2000 Till Kamppeter <till@mandrakesoft.com> 2.0.7-6mdk
- Put "smbspool" to the files to install

* Wed Aug 30 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-5mdk
- Did some cleaning in the patches

* Fri Jul 28 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-4mdk
- relocated man pages from /usr/man to /usr/share/man for compatibility reasons

* Fri Jul 28 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-3mdk
- added make_unicodemap and build of unicode_map.$i in the spec file

* Fri Jul 28 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-2mdk
- renamed /etc/codepage/codepage.$i into /etc/codepage/unicode_map.$i to fix smbmount bug.

* Fri Jul 07 2000 Sylvestre Taburet <staburet@mandrakesoft.com> 2.0.7-1mdk
- 2.0.7

* Wed Apr 05 2000 Francis Galiegue <fg@mandrakesoft.com> 2.0.6-4mdk

- Titi sucks, does not put versions in changelog
- Fixed groups for -common and -client
- /usr/sbin/samba is no config file

* Thu Mar 23 2000 Thierry Vignaud <tvignaud@mandrakesoft.com>
- fix buggy post install script (pixel)

* Fri Mar 17 2000 Francis Galiegue <francis@mandrakesoft.com> 2.0.6-2mdk

- Changed group according to 7.1 specs
- Some spec file changes
- Let spec-helper do its job

* Thu Nov 25 1999 Chmouel Boudjnah <chmouel@mandrakesoft.com>
- 2.0.6.

* Tue Nov  2 1999 Chmouel Boudjnah <chmouel@mandrakesoft.com>
- Merge with rh changes.
- Split in 3 packages.

* Fri Aug 13 1999 Pablo Saratxaga <pablo@@mandrakesoft.com>
- corrected a bug with %post (the $1 parameter is "1" in case of
  a first install, not "0". That parameter is the number of packages
  of the same name that will exist after running all the steps if nothing
  is removed; so it is "1" after first isntall, "2" for a second install
  or an upgrade, and "0" for a removal)

* Wed Jul 28 1999 Pablo Saratxaga <pablo@@mandrakesoft.com>
- made smbmnt and smbumount suid root, and only executable by group 'smb'
  add to 'smb' group any user that should be allowed to mount/unmount
  SMB shared directories

* Fri Jul 23 1999 Chmouel Boudjnah <chmouel@mandrakesoft.com>
- 2.0.5a (bug security fix).

* Wed Jul 21 1999 Axalon Bloodstone <axalon@linux-mandrake.com>
- 2.0.5
- cs/da/de/fi/fr/it/tr descriptions/summaries

* Sun Jun 13 1999 Bernhard Rosenkränzer <bero@mandrakesoft.com>
- 2.0.4b
- recompile on a system that works ;)

* Wed Apr 21 1999 Chmouel Boudjnah <chmouel@mandrakesoft.com>
- Mandrake adaptations.
- Bzip2 man-pages.

* Fri Mar 26 1999 Bill Nottingham <notting@redhat.com>
- add a mount.smb to make smb mounting a little easier.
- smb filesystems apparently do not work on alpha. Oops.

* Thu Mar 25 1999 Bill Nottingham <notting@redhat.com>
- always create codepages

* Tue Mar 23 1999 Bill Nottingham <notting@redhat.com>
- logrotate changes

* Sun Mar 21 1999 Cristian Gafton <gafton@redhat.com>
- auto rebuild in the new build environment (release 3)

* Fri Mar 19 1999 Preston Brown <pbrown@redhat.com>
- updated init script to use graceful restart (not stop/start)

* Tue Mar  9 1999 Bill Nottingham <notting@redhat.com>
- update to 2.0.3

* Thu Feb 18 1999 Bill Nottingham <notting@redhat.com>
- update to 2.0.2

* Mon Feb 15 1999 Bill Nottingham <notting@redhat.com>
- swat swat

* Tue Feb  9 1999 Bill Nottingham <notting@redhat.com>
- fix bash2 breakage in post script

* Fri Feb  5 1999 Bill Nottingham <notting@redhat.com>
- update to 2.0.0

* Mon Oct 12 1998 Cristian Gafton <gafton@redhat.com>
- make sure all binaries are stripped

* Thu Sep 17 1998 Jeff Johnson <jbj@redhat.com>
- update to 1.9.18p10.
- fix %triggerpostun.

* Tue Jul 07 1998 Erik Troan <ewt@redhat.com>
- updated postun triggerscript to check $0
- clear /etc/codepages from %preun instead of %postun

* Mon Jun 08 1998 Erik Troan <ewt@redhat.com>
- made the %postun script a tad less agressive; no reason to remove
  the logs or lock file (after all, if the lock file is still there,
  samba is still running)
- the %postun and %preun should only exectute if this is the final
  removal
- migrated %triggerpostun from Red Hat's samba package to work around
  packaging problems in some Red Hat samba releases

* Sun Apr 26 1998 John H Terpstra <jht@samba.anu.edu.au>
- minor tidy up in preparation for release of 1.9.18p5
- added findsmb utility from SGI package

* Wed Mar 18 1998 John H Terpstra <jht@samba.anu.edu.au>
- Updated version and codepage info.
- Release to test name resolve order

* Sat Jan 24 1998 John H Terpstra <jht@samba.anu.edu.au>
- Many optimisations (some suggested by Manoj Kasichainula <manojk@io.com>
- Use of chkconfig in place of individual symlinks to /etc/rc.d/init/smb
- Compounded make line
- Updated smb.init restart mechanism
- Use compound mkdir -p line instead of individual calls to mkdir
- Fixed smb.conf file path for log files
- Fixed smb.conf file path for incoming smb print spool directory
- Added a number of options to smb.conf file
- Added smbadduser command (missed from all previous RPMs) - Doooh!
- Added smbuser file and smb.conf file updates for username map
