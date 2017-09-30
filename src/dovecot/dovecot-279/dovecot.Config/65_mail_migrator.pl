#!/usr/bin/perl
#
# Copyright (c) 2010-2012 Apple Inc. All rights reserved.
# 
# IMPORTANT NOTE: This file is licensed only for use on Apple-branded
# computers and is subject to the terms and conditions of the Apple Software
# License Agreement accompanying the package this file is a part of.
# You may not port this file to another platform without Apple's written consent.
# 
# Redistribution and use in source and binary forms, with or without  
# modification, are permitted provided that the following conditions  
# are met:
# 
# 1.  Redistributions of source code must retain the above copyright  
# notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above  
# copyright notice, this list of conditions and the following  
# disclaimer in the documentation and/or other materials provided  
# with the distribution.
# 3.  Neither the name of Apple Inc. ("Apple") nor the names of its  
# contributors may be used to endorse or promote products derived  
# from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND  
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS  
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF  
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND  
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF  
# SUCH DAMAGE.

use strict;

## Migration Script for Mail Services

##################   Input Parameters  #######################
# --purge <0 | 1>   "1" means remove any files from the old system after you've migrated them, "0" means leave them alone.
# --sourceRoot <path> The path to the root of the system to migrate
# --sourceType <System | TimeMachine> Gives the type of the migration source, whether it's a runnable system or a
#                  Time Machine backup.
# --sourceVersion <ver> The version number of the old system (like 10.6.5 or 10.7). Since we support migration from 10.6, 10.7,
#                   and other 10.8 installs.
# --targetRoot <path> The path to the root of the new system.
# --language <lang> A language identifier, such as \"en.\" Long running scripts should return a description of what they're doing
#                   (\"Migrating Open Directory users\"), and possibly provide status update messages along the way. These messages
#                   need to be localized (which is not necessarily the server running the migration script).
#                   This argument will identify the Server Assistant language. As an alternative to doing localization yourselves
#                   send them in English, but in case the script will do this it will need this identifier.
#
# Example: /System/Library/ServerSetup/MigrationExtras/65_mail_migrator.pl --purge 0 --language en --sourceType System --sourceVersion 10.6 --sourceRoot "/Previous System" --targetRoot "/"

############################# System  Constants #############################
my $CAT = "/bin/cat";
my $CP = "/bin/cp";
my $MV = "/bin/mv";
my $RM = "/bin/rm";
my $DSCL = "/usr/bin/dscl";
my $DU = "/usr/bin/du";
my $ECHO = "/bin/echo";
my $GREP = "/usr/bin/grep";
my $CHOWN = "/usr/sbin/chown";
my $LAUNCHCTL = "/bin/launchctl";
my $POSTMAP = "/usr/sbin/postmap";
my $POSTCONF = "/usr/sbin/postconf";
my $POSTFIX = "/usr/sbin/postfix";
my $MKDIR = "/bin/mkdir";
my $PLIST_BUDDY = "/usr/libexec/PlistBuddy";
my $TAR = "/usr/bin/tar";

# ServerRoot paths
my $SERVER_ROOT		= "/Applications/Server.app/Contents/ServerRoot";
my $CERT_ADMIN		= "/Applications/Server.app/Contents/ServerRoot/usr/sbin/certadmin";
my $SERVER_CTL		= "/Applications/Server.app/Contents/ServerRoot/usr/sbin/serverctl";
my $SERVER_ADMIN	= "/Applications/Server.app/Contents/ServerRoot/usr/sbin/serveradmin";
my $CVT_MAIL_DATA	= "/Applications/Server.app/Contents/ServerRoot/usr/bin/cvt_mail_data";
my $MIGRATION_PLIST	= "/Applications/Server.app/Contents/ServerRoot/System/Library/LaunchDaemons/com.apple.mail_migration.plist";

# Cyrus imapd.conf
my $IMAPD_CONF		= "/private/etc/imapd.conf";

# Config & Data Roots
my $MAIL_ROOT		= "/Library/Server/Mail";
my $DATA_ROOT		= "/Library/Server/Mail/Data";
my $CONFIG_ROOT		= "/Library/Server/Mail/Config";
my $POSTFIX_CONFIG	= "/Library/Server/Mail/Config/postfix";
my $DOVECOT_CONFIG	= "/Library/Server/Mail/Config/dovecot";

# Migration logs
my $MIGRATION_LOG_DIR	= "/Library/Logs/Migration";
my $MIGRATION_LOG		= "/Library/Logs/Migration/mailmigrator.log";

# Launchd
my $POSTFIX_LAUNCHD_PLIST	= "/System/Library/LaunchDaemons/org.postfix.master.plist";

############################## Version Consts  ##############################
my $SYS_VERS	= "0";	#10.6.5
my $SYS_MAJOR	= "0";	#10
my $SYS_MINOR	= "0";	# 6
my $SYS_UPDATE	= "-";	# 5
my $SRV_VERS	= "0";	#10.6.5
my $SRV_MAJOR	= "0";	#10
my $SRV_MINOR	= "0";	# 6
my $SRV_UPDATE	= "-";	# 5
my $MIN_VER		= "10.6"; # => 10.6
my $MAX_VER		= "10.9"; # <  10.9

my $TARGET_VER = "10.8";

################################## Globals ##################################
my $g_purge				= 0;	# So we will be default copy the items if
								# there's no option specified.
my $g_source_root		= "";
my $g_source_type		= "";
my $g_source_version	= "";	# This is the version number of the old system
								# passed into us by Server Assistant.
								# [10.6.x, 10.7.x, and potentially 10.8.x]
my $g_source_uuid		= "";
my $g_target_root		= "";
my $g_language			= "en";	# Should be Tier-0 only in iso format
								# [en, fr, de, ja], we default this to English, en.

my $g_xsan_volume		= "null";
my $g_dovecot_ssl_key	= "";
my $g_dovecot_ssl_cert	= "";
my $g_dovecot_ssl_ca	= "";

my @g_partitions		= ();
my @g_clean_partitions	= ();
my $g_default_partition	= "/Library/Server/Mail/Data/mail";
my $g_db_path			= "/private/var/imap";
my $g_imapd_conf		= "/tmp/imapd.conf.tmp";
my $g_migration_plist	= "/var/db/.mailmigration.plist";

my $g_enable_spam		= 0;
my $g_enable_virus		= 0;
my $g_required_hits		= 6.9;
my $g_spam_subj_tag		= "*** JUNK MAIL ***";

my $g_mtn_lion_src		= 0;	# Set to 1 if source is Mountain Lion Server (10.8.x)
my $g_lion_src			= 0;	# Set to 1 if source is Lion Server (10.7.x)
my $g_snow_leopard_src	= 0;	# Set to 1 if source is Snow Leopard Server  (10.6.x)
my $g_leopard_src		= 0;	# Set to 1 if source is Snow Server  (10.5.x)

################################## Strings ##################################
my $g_header_string		= "### DO NOT MANUALLY EDIT THIS FILE ###\n# This file is automatically generated\n# any manual additions to this file will be lost\n\n";

################################### Flags ###################################
my $DEBUG		= 0;
my $FUNC_LOG	= 0;

#############################################################################
# main
#############################################################################

use Foundation;
use File::Copy;
use File::Basename;

if ($ENV{DEBUG} eq 1) {
	$DEBUG = '1'; }

if ( $ENV{FUNC_LOG} eq 1 ) {
	$FUNC_LOG = '1'; }

parse_options();

if ( ${DEBUG} ) {
	print_script_args( @ARGV ); }

if ( !path_exists( $MIGRATION_LOG_DIR ) ) {
	mkdir($MIGRATION_LOG_DIR, 0755);
}

open (LOG_FILE, ">> ${MIGRATION_LOG}") or die("$MIGRATION_LOG: $!\n");

do_migration( @ARGV );

exit();


################################# Functions #################################

#################################################################
# print_message ()

sub print_message ()
{
	my ( $line_0 ) = $_[0];
	my ( $line_1 ) = $_[1];
	my ( $line_2 ) = $_[2];
	my ( $line_3 ) = $_[3];

	log_funct( "print_message" );

	print LOG_FILE "*************************************************************\n";
	if ( ! ("${line_0}" eq "") ) {
		print LOG_FILE "** ${line_0}\n"; }
	if ( ! ("${line_1}" eq "") ) {
		print LOG_FILE "** ${line_1}\n"; }
	if ( ! ("${line_2}" eq "") ) {
		print LOG_FILE "** ${line_2}\n"; }
	if ( ! ("${line_3}" eq "") ) {
		print LOG_FILE "** ${line_3}\n"; }
	print LOG_FILE "** Please refer to the Migration and Upgrade Guide for\n";
	print LOG_FILE "** instructions on how to manually migrate configuration data.\n";
	print LOG_FILE "*************************************************************\n";
} # print_message

################################################################################
# function log

sub log_funct ($)
{
	my( $in_function ) = @_;

	if ( ${FUNC_LOG} ) {
		print LOG_FILE "----------------------\n";
		print LOG_FILE ":: $in_function () ::\n";
	}
}

################################################################################
# log if DEBUG

sub log_debug ($)
{
	my( $in_string ) = @_;

	if ( ${DEBUG} ) {
		print LOG_FILE "$in_string\n";
	}
}

################################################################################
# plist methods

sub obj_value
{
  my ( $object ) = @_;
  return( $object->description()->UTF8String() );
} # obj_value


#################################################################
# get_cert_name ()
#
#	normalize certificate names for comparison

sub get_cert_name ($)
{
	my( $in_cert ) = @_;
	my $out_cert = $in_cert;

	# we will only migrate certificates from /etc/certificates
	if ( substr( $out_cert, 0, 18) eq "/etc/certificates/" ){
		$out_cert = substr( $out_cert, 18 );
	}

	# ket suffix so we can map correct file
	my $cert_suffix = substr( $out_cert, -8 );

	if ( $cert_suffix eq ".key.pem" ) {
		$out_cert = substr( $out_cert, 0, length($out_cert)-49) . ".key.pem";
	} elsif ( $cert_suffix eq "cert.pem" ) {
		$out_cert = substr( $out_cert, 0, length($out_cert)-50) . ".cert.pem";
	} elsif ( $cert_suffix eq "hain.pem" ) {
		$out_cert = substr( $out_cert, 0, length($out_cert)-51) . ".chain.pem";
	} elsif ( $cert_suffix eq "ncat.pem" ) {
		$out_cert = substr( $out_cert, 0, length($out_cert)-52) . ".concat.pem";
	} else {
		$out_cert = "";
	}

	return( $out_cert );
}

#################################################################
# map_ssl_cert ()
#
#	: map default certificate from 10.5 to 10.6/7 certificates

sub map_ssl_cert ($)
{
	my( $in_cert ) = @_;

	log_funct( "map_ssl_cert" );
	log_debug( "Mapping certificate: ${in_cert}" );

	# map from current certificates directory
	chdir( "/private/etc/certificates" );

	my @mail_certs = <*>;
	foreach my $a_cert ( @mail_certs ) {
		chomp($a_cert);
		my $cert_name = get_cert_name($a_cert);

		# looking for key
		if ( $cert_name eq $in_cert ) {
			print LOG_FILE "  Mapping certificate name: ${in_cert} to: ${a_cert}\n";
			return("/etc/certificates/" . $a_cert);
		}
	}

	# Not good, no certificate found in /etc/certificates that matches
	#	configuration settings from previous server
	print LOG_FILE "Warning: No certificate map found for: " . ${in_cert} ."\n";

	return( "" );
} # map_ssl_cert


#################################################################
# set_ssl_certs ()

sub set_ssl_certs ()
{
	log_funct( "set_ssl_certs" );

	print LOG_FILE "Migrating SSL certificates\n";

	#################################
	# Default Certificate validataion
	#################################

	## certificate sanity check
	# first get default certificates (we may need these later)
	my $default_cert = qx( $CERT_ADMIN --default-certificate-path );
	chomp($default_cert);
	print LOG_FILE "  default-certificate: $default_cert\n";

	my $default_key = qx( $CERT_ADMIN --default-private-key-path );
	chomp($default_key);
	print LOG_FILE "  default-private-key: $default_key\n";

	my $default_ca = qx( $CERT_ADMIN --default-certificate-authority-chain-path );
	chomp($default_ca);
	print LOG_FILE "  default-certificate-authority-chain: $default_ca\n";

	##################################
	# POP/IMAP Certificate validataion
	##################################

	# migrate cyrus if from Leopard Server only
	print LOG_FILE "Getting current POP/IMAP server certificate settings\n";

	my $valid_imap_key = 0;
	my $imap_key = qx( grep "^ssl_key " "${g_target_root}${DOVECOT_CONFIG}/conf.d/10-ssl.conf" | sed 's/ssl_key//' | sed 's/=//' | sed 's/ //g' | sed 's/<//' );
	chomp($imap_key);
	if ( $imap_key eq "" ) {
		$imap_key = qx( grep "^#ssl_key " "${g_target_root}${DOVECOT_CONFIG}/conf.d/10-ssl.conf" | sed 's/#ssl_key//' | sed 's/=//' | sed 's/ //g' | sed 's/<//' );
		chomp($imap_key);
	}

	my $valid_imap_cert = 0;
	my $imap_cert = qx( grep "^ssl_cert " "${g_target_root}${DOVECOT_CONFIG}/conf.d/10-ssl.conf" | sed 's/ssl_cert//' | sed 's/=//' | sed 's/ //g' | sed 's/<//' );
	chomp($imap_cert);
	if ( $imap_cert eq "" ) {
		$imap_cert = qx( grep "^#ssl_cert " "${g_target_root}${DOVECOT_CONFIG}/conf.d/10-ssl.conf" | sed 's/#ssl_cert//' | sed 's/=//' | sed 's/ //g' | sed 's/<//' );
		chomp($imap_cert);
	}

	my $valid_imap_ca = 0;
	my $imap_ca = qx( grep "^ssl_ca " "${g_target_root}${DOVECOT_CONFIG}/conf.d/10-ssl.conf" | sed 's/ssl_ca//' | sed 's/=//' | sed 's/ //g' | sed 's/<//' );
	chomp($imap_ca);
	if ( $imap_ca eq "" ) {
		$imap_ca = qx( grep "^#ssl_ca " "${g_target_root}${DOVECOT_CONFIG}/conf.d/10-ssl.conf" | sed 's/#ssl_ca//' | sed 's/=//' | sed 's/ //g' | sed 's/<//' );
		chomp($imap_ca);
	}

	# validate current IMAP certificates
	if ( !($imap_key eq "") && path_exists($imap_key) ) {
		print LOG_FILE "  ssl_key: $imap_key\n";
		$valid_imap_key = 1;
	}

	if ( !($imap_cert eq "") && path_exists($imap_cert) ) {
		print LOG_FILE "  ssl_cert: $imap_cert\n";
		$valid_imap_cert = 1;
	}

	if ( !($imap_ca eq "") && path_exists($imap_ca) ) {
		print LOG_FILE "  ssl_ca: $imap_ca\n";
		$valid_imap_ca = 1;
	}

	# log success if all certificates map
	if ( ($valid_imap_key == 1) && ($valid_imap_cert == 1) && ($valid_imap_ca == 1) ) {
		print LOG_FILE "Using existing POP/IMAP SSL certificates:\n";
		print LOG_FILE "  ssl_key: $imap_key\n";
		print LOG_FILE "  ssl_cert: $imap_cert\n";
		print LOG_FILE "  ssl_ca: $imap_ca\n";
		qx( ${SERVER_ADMIN} settings mail:imap:tls_key_file = $imap_key );
		qx( ${SERVER_ADMIN} settings mail:imap:tls_cert_file = $imap_cert );
		qx( ${SERVER_ADMIN} settings mail:imap:tls_ca_file = $imap_ca );
	} else {
		# first see if we care 
		my $use_ssl = qx( grep "^ssl =" "${g_target_root}${DOVECOT_CONFIG}/conf.d/10-ssl.conf" | sed 's/ssl//' | sed 's/=//' | sed 's/ //g' );
		chomp($use_ssl);

		# map certificates when using SSL and the oritinal certificates cannot be found
		if ( ($use_ssl eq "yes") || ($use_ssl eq "required") ) {
			# list missing certificates
			print LOG_FILE "Missing existing POP/IMAP SSL certificates:\n";
			if ( $valid_imap_key != 1) {
				print LOG_FILE "  ssl_key: $imap_key\n";
			}
			if ( $valid_imap_cert != 1) {
				print LOG_FILE "  ssl_cert: $imap_cert\n";
			}
			if ( $valid_imap_ca != 1 ) {
				print LOG_FILE "  ssl_ca: $imap_ca\n";
			}

			# get current settings
			my $imap_server_opts = qx( ${SERVER_ADMIN} settings mail:imap:tls_server_options );
			chomp($imap_server_opts);

			# try to map names to existing certificate
			print LOG_FILE "Attempting to map missing certificates:\n";
			my $ssl_key = map_ssl_cert( get_cert_name( $imap_key ) );
			my $ssl_cert = map_ssl_cert( get_cert_name( $imap_cert ) );
			my $ssl_ca = map_ssl_cert( get_cert_name( $imap_ca ) );
			# use mapped certificates if they all exist
			if ( path_exists($ssl_key) && path_exists($ssl_cert) && path_exists($ssl_ca) ) {
				print LOG_FILE "Using mapped POP/IMAP services certificate settings\n  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_key_file = $ssl_key >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_cert_file = $ssl_cert >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_ca_file = $ssl_ca >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings $imap_server_opts >> ${MIGRATION_LOG} );
			} elsif ( path_exists($default_cert) && path_exists($default_key) && path_exists($default_ca) ) {
				# some mappings failed so setting to default
				print LOG_FILE "Setting default certificates for POP/IMAP services\n  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_key_file = $default_key >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_cert_file = $default_cert >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_ca_file = $default_ca >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings $imap_server_opts >> ${MIGRATION_LOG} );
			} else {
				# Worst case: no certificate mappings and no default certificates found
				print LOG_FILE "ERROR: No default certificats found.  Disabling SSL for POP/IMAP services\n  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_key_file = "" >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_cert_file = "" >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_ca_file = "" >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:imap:tls_server_options = "none" >> ${MIGRATION_LOG} );
			}
		} else {
			print LOG_FILE "  ";
			qx( ${SERVER_ADMIN} settings mail:imap:tls_server_options = "none" >> ${MIGRATION_LOG} );
			print LOG_FILE "POP/IMAP SSL settings not set: ssl = $use_ssl\n";
		}
	}

	###############################
	# SMTP Certificate validataion
	###############################

	print LOG_FILE "Getting current SMTP service certificate settings\n";

	# get current SMTP certificates
	my $valid_smtp_key = 0;
	my $smtp_key = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" smtpd_tls_key_file );
	chomp($smtp_key);

	my $valid_smtp_cert = 0;
	my $smtp_cert = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" smtpd_tls_cert_file );
	chomp($smtp_cert);

	my $valid_smtp_ca = 0;
	my $smtp_ca = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" smtpd_tls_CAfile );
	chomp($smtp_ca);

	# validate current SMTP certificates
	if ( !($smtp_key eq "") && path_exists($smtp_key) ) {
		print LOG_FILE "  smtpd_tls_key_file: $smtp_key\n";
		$valid_smtp_key = 1;
	}

	if ( !($smtp_cert eq "") && path_exists($smtp_cert) ) {
		print LOG_FILE "  smtpd_tls_cert_file: $smtp_cert\n";
		$valid_smtp_cert = 1;
	}

	if ( !($smtp_ca eq "") && path_exists($smtp_ca) ) {
		print LOG_FILE "  smtpd_tls_CAfile: $smtp_ca\n";
		$valid_smtp_ca = 1;
	}

	# log success if all certificates map
	if ( ($valid_smtp_key == 1) && ($valid_smtp_cert == 1) && ($valid_smtp_ca == 1) ) {
		print LOG_FILE "Using existing SMTP SSL certificates:\n";
		print LOG_FILE "  smtpd_tls_key_file: $smtp_key\n";
		print LOG_FILE "  smtpd_tls_cert_file: $smtp_cert\n";
		print LOG_FILE "  smtpd_tls_CAfile: $smtp_ca\n";
	} else {
		# first see if we care 
		my $use_tls = qx( grep "^smtpd_use_tls =" "${g_target_root}${POSTFIX_CONFIG}/main.cf" | sed 's/smtpd_use_tls//' | sed 's/=//' | sed 's/ //g' );
		chomp($use_tls);

		# map certificates when using SSL but oritinals cannot be found
		if ( $use_tls eq "yes" ) {
			# list missing certificates
			print LOG_FILE "Missing existing SMTP SSL certificates:\n";
			if ( $valid_smtp_key != 1 ) {
				print LOG_FILE "  smtpd_tls_key_file: $smtp_key\n";
			}
			if ( $valid_smtp_cert != 1 ) {
				print LOG_FILE "  smtpd_tls_cert_file: $smtp_cert\n";
			}
			if ( $valid_smtp_ca != 1 ) {
				print LOG_FILE "  smtpd_tls_CAfile: $smtp_ca\n";
			}

			# get current settings
			my $smtp_server_opts = qx( ${SERVER_ADMIN} settings mail:postfix:tls_server_options );
			chomp($smtp_server_opts);

			# try to map names to existing certificate
			print LOG_FILE "Attempting to map missing certificates:\n";
			my $ssl_key = map_ssl_cert( get_cert_name( $smtp_key ) );
			my $ssl_cert = map_ssl_cert( get_cert_name( $smtp_cert ) );
			my $ssl_ca = map_ssl_cert( get_cert_name( $smtp_ca ) );

			# use mapped certificates if they all exist
			if ( path_exists($ssl_key) && path_exists($ssl_cert) && path_exists($ssl_ca) ) {
				print LOG_FILE "Mapping SMTP service certificate settings\n  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_key_file = $ssl_key >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_cert_file = $ssl_cert >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_key_file = $ssl_ca >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings $smtp_server_opts >> ${MIGRATION_LOG} );
			} elsif ( path_exists($default_cert) && path_exists($default_key) && path_exists($default_ca) ) {
				# some mappings failed so setting to default
				print LOG_FILE "Setting default certificates for SMTP services\n  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_key_file = $default_key >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_cert_file = $default_cert >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_key_file = $default_ca >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings $smtp_server_opts >> ${MIGRATION_LOG} );
			} else {
				# Worst case: no certificate mappings and no default certificates found
				print LOG_FILE "ERROR: No default certificats found.  Disabling SSL for SMTP services\n  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_key_file = "" >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_cert_file = "" >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:smtpd_tls_key_file = "" >> ${MIGRATION_LOG} );
				print LOG_FILE "  ";
				qx( ${SERVER_ADMIN} settings mail:postfix:tls_server_options = none >> ${MIGRATION_LOG} );
			}
		} else {
			print LOG_FILE "  ";
			qx( ${SERVER_ADMIN} settings mail:postfix:tls_server_options = none >> ${MIGRATION_LOG} );
			print LOG_FILE "SMTP SSL settings not set: smtpd_use_tls = $use_tls\n";
		}
	}
	print LOG_FILE "Migrating of SSL certificates complete\n";
} # get_ssl_certs


#################################################################
# migrate_db_update_times ()

sub migrate_db_update_times ()
{
	log_funct( "migrate_db_update_times" );

	print LOG_FILE "Migrating virus database upgrade interval\n";

	open(CLAMAV_PLIST, "<${g_source_root}" . "/System/Library/LaunchDaemons/org.clamav.freshclam.plist");
	while( <CLAMAV_PLIST> ) {
		# store $_ value because subsequent operations may change it
		my( $line ) = $_;

		# strip the trailing newline from the line
		chomp( $line );

		my $key = index($line, "-c ");
		if ( ${key} != -1 ) {
			my $value;
			if ( substr( ${line}, ${key}+4, 1 ) eq "<" ) {
				$value = substr( ${line}, ${key}+3, 1 );
			} else {
				$value = substr( ${line}, ${key}+3, 2 );
			}
			qx( ${SERVER_ADMIN} settings mail:postfix:virus_db_update_days = ${value} >> ${MIGRATION_LOG} );
		}
	}
	close( CLAMAV_PLIST );
} # migrate_db_update_times


#################################################################
# migrate_log_settings ()

sub migrate_log_settings ()
{
	log_funct( "migrate_log_settings" );

	# look for syslog.conf in source-root or in target-root with ~previous extension
	my $syslog_conf_path = ${g_target_root} . "/private/etc/syslog.conf~previous";
	if ( !path_exists( ${syslog_conf_path} ) ) {
		print LOG_FILE "Note: previous syslog.conf: " . ${syslog_conf_path} . ", not found\n";

		$syslog_conf_path = ${g_source_root} . "/private/etc/syslog.conf";
		if ( !path_exists( ${syslog_conf_path} ) ) {
			print LOG_FILE "Note: previous syslog.conf: " . ${syslog_conf_path} . ", not found\n";
			print LOG_FILE "Log level settings not migrated\n";
			return;
		}
	}

	print LOG_FILE "Migrating log level settings from: ${syslog_conf_path}\n";

	# get mail. & local6. log settings
	open(SYS_LOG, "<${syslog_conf_path}");
	while( <SYS_LOG> ) {
		# store $_ value because subsequent operations may change it
		my( $line ) = $_;

		# strip the trailing newline from the line
		chomp( $line );

		my $key = "";
		my $value = "";
		my $offset = 0;
		if ( substr( ${line}, 0, 5) eq "mail." ) {
			${offset} = 5;
			${key} = "mail:postfix:log_level";
		}
		elsif (substr( ${line}, 0, 7) eq "local2." ) {
			${offset} = 7;
			${key} = "mail:postfix:virus_log_level";
		}
		elsif (substr( ${line}, 0, 7) eq "local6." ) {
			${offset} = 7;
			${key} = "mail:imap:log_level";
		}

		if ( ${offset} != 0 ) {
			SWITCH: {
				if ( substr( ${line}, ${offset}, 3) eq "err" ) {
					${value} = "err";
					last SWITCH;
				}
				if ( substr( ${line}, ${offset}, 4) eq "crit" ) {
					${value} = "crit";
					last SWITCH;
				}
				if ( substr( ${line}, ${offset}, 4) eq "warn" ) {
					${value} = "warn";
					last SWITCH;
				}
				if ( substr( ${line}, ${offset}, 6) eq "notice" ) {
					${value} = "notice";
					last SWITCH;
				}
				if ( substr( ${line}, ${offset}, 4) eq "info" ) {
					${value} = "info";
					last SWITCH;
				}
				if ( substr( ${line}, ${offset}, 1) eq "*" ) {
					${value} = "debug";
					last SWITCH;
				}
				last SWITCH;
			}
			if ( !(${value} eq "") && !(${key} eq "") ) {
				qx( ${SERVER_ADMIN} settings ${key} = ${value} >> ${MIGRATION_LOG} );
			}
		}
	}

	# close it, close it up again
	close( SYS_LOG );

} # migrate_log_settings


#################################################################
# set_log_defaults ()

sub set_log_defaults ()
{
	qx( ${SERVER_ADMIN} settings mail:imap:log_level = "info" >> ${MIGRATION_LOG} );
	qx( ${SERVER_ADMIN} settings mail:postfix:log_level = "info" >> ${MIGRATION_LOG} );
	qx( ${SERVER_ADMIN} settings mail:postfix:spam_log_level = "info" >> ${MIGRATION_LOG} );
	qx( ${SERVER_ADMIN} settings mail:postfix:virus_log_level = "info" >> ${MIGRATION_LOG} );
	qx( ${SERVER_ADMIN} settings mail:postfix:virus_db_log_level = "info" >> ${MIGRATION_LOG} );
} # set_log_defaults


#################################################################
# load_postfix ()

sub load_postfix ()
{
	log_funct( "load_postfix" );
	print LOG_FILE "Reloading current SMPT instance\n";

	# stop postfix
	if ( path_exists( "${POSTFIX_LAUNCHD_PLIST}" ) ) {
		log_debug( "- Stopping postfix" );
		qx( ${LAUNCHCTL} load -w ${POSTFIX_LAUNCHD_PLIST} >>"${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );
	}
} # load_postfix


#################################################################
# unload_postfix ()

sub unload_postfix ()
{
	log_funct( "unload_postfix" );
	print LOG_FILE "Unloading current SMPT instance\n";

	# stop postfix
	if ( path_exists( "${POSTFIX_LAUNCHD_PLIST}" ) ) {
		log_debug( "- Stopping postfix" );
		qx( ${LAUNCHCTL} unload -w ${POSTFIX_LAUNCHD_PLIST} >>"${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );
	}
} # unload_postfix


################################################################################
# get old server version parts

sub get_server_version ($)
{
	my ($VERS) = @_;
	log_funct( "get_server_version" );

	my @SRV_VER_PARTS = split(/\./, $VERS); 

	log_debug( sprintf("sourceVersion: %s", "${VERS}") );
	log_debug( sprintf("  major : %s", ${SRV_VER_PARTS}[0]) );
	log_debug( sprintf("  minor : %s", ${SRV_VER_PARTS}[1]) );
	log_debug( sprintf("  update: %s", ${SRV_VER_PARTS}[2]) );

	$SRV_MAJOR = ${SRV_VER_PARTS}[0];
	$SRV_MINOR = ${SRV_VER_PARTS}[1];
	$SRV_UPDATE = ${SRV_VER_PARTS}[2];
} # get_server_version


#################################################################
# get_mail_partitions ()

sub get_mail_partitions ()
{
	log_funct( "get_mail_partitions" );
	log_debug( "- opening: ${g_source_root} . ${IMAPD_CONF}" );

	#	do -not- check for open failure, custom mail partitions will
	#	 simply not get migrated if /etc/impad.conf does not exist
	open( cyrus_imapd, "<${g_source_root}" . ${IMAPD_CONF} );
	while( <cyrus_imapd> )
	{
		# store $_ value because subsequent operations may change it
		my( $config_key ) = $_;
		my( $data_path ) = $_;

		# strip the trailing newline from the line
		chomp($config_key);
		chomp($data_path);

		if ( $config_key =~ s/:.*// )
		{
			if ( $data_path =~ s/^.*:// )
			{
				# trim whitespace
				$data_path =~ s/^\s+//;
				$data_path =~ s/\s+$//;

				my $partition_tag = substr( $config_key, 0, 10 );
				if ( $partition_tag eq "partition-" )
				{
					$partition_tag = substr( $config_key, 10, (length($config_key) - 10) );
					if ( "$partition_tag" eq "default" )
					{
						# only do migration on non-Xsan volumes
						if ( ! (substr( "${data_path}", 0, length($g_xsan_volume)) eq $g_xsan_volume) ) {
							$g_default_partition = "${data_path}";
						} else {
							&print_message( "Warning:", "Mail data was located on an Xsan volume:", "  ${data_path}", "This data will need to be migrated manually" );
						}
						log_debug( "- default partition: \"${data_path}" );
					} else {
						# only do migration on non-Xsan volumes
						if ( ! (substr( "${data_path}", 0, length($g_xsan_volume)) eq $g_xsan_volume) ) {
							push( @g_partitions, "${data_path}" );
						} else {
							&print_message( "Warning:", "Mail data was located on an Xsan volume:", "  ${data_path}", "This data will need to be migrated manually" );
						}
						log_debug( "- alt partition: \"${data_path}\"" );
					}
				}
			}
		}
	}

	close( cyrus_imapd );

} # get_mail_partitions


#################################################################
# do_xsan_check ()

sub do_xsan_check ()
{
	log_funct( "do_xsan_check" );

	my $MOUNTS_TXT = "/Library/Server/Mail/Config/dovecot/mount.txt";

	qx( mount -v > ${MOUNTS_TXT} );

	open( VOLUMES, "< ${MOUNTS_TXT}" ) or die "can't open ${MOUNTS_TXT}: $!";
	while( <VOLUMES> )
	{
		# store $_ value because subsequent operations may change it
		my( $TYPE ) = $_;
		my( $VOL_NAME ) = $_;

		# strip the trailing newline from the line
		chomp( $TYPE );
		chomp( $VOL_NAME );

		log_debug( sprintf( "- Volume: %s\n", ${VOL_NAME}) );

		# get the volume type from mount text:
		#	ie. /dev/disk0s4 on /Volumes/Leopard-GM (hfs, local, journaled)

		$TYPE =~ s/^.*\(//;
		$TYPE =~ s/,.*//;

		# if it is of acfs type, then is an Xsan volume
		if ( ${TYPE} eq "acfs" )
		{
			log_debug( sprintf( "- Xsan volume: %s\n", ${VOL_NAME}) );

			# get the volume name
			$VOL_NAME =~ s/^.*on //;
			$VOL_NAME =~ s/ .*//;
			$g_xsan_volume = $VOL_NAME;

			log_debug( sprintf( "- Xsan volume name: %s\n", ${g_xsan_volume}) );

			# only 1 xsan supported so break if we find 1
			last;
		}
	}

	close( VOLUMES );

	if ( path_exists("${MOUNTS_TXT}") ) {
		unlink("${MOUNTS_TXT}"); }

} # do_xsan_check


#################################################################
# do_cyrus_db_cleanup ()

sub do_cyrus_db_cleanup ()
{
	log_funct( "do_cyrus_db_cleanup" );

	# nothing to clean up if in-place-upgrade
	if ( "${g_source_root}" eq "/Previous System" )
	{
		$g_db_path = "/Previous System/private/var/imap";
		return( 0 );
	}

	my $exit_code = 1;

	log_debug( "- opening: ${g_source_root} . ${IMAPD_CONF}" );

	open( cyrus_imapd, "<${g_source_root}" . ${IMAPD_CONF} );
	while( <cyrus_imapd> )
	{
		# store $_ value because subsequent operations may change it
		my( $config_key ) = $_;
		my( $config_value ) = $_;

		# strip the trailing newline from the line
		chomp($config_key);
		chomp($config_value);

		log_debug( "- line: $config_key" );

		if ( $config_key =~ s/:.*// )
		{
			if ( $config_key eq "configdirectory" )
			{
				if ( $config_value =~ s/^.*:// )
				{
					# trim whitespace
					$config_value =~ s/^\s+//;
					$config_value =~ s/\s+$//;

					# set global cyrus database path
					my $volume_tag = substr( $config_value, 0, 8 );
					if ( ${volume_tag} eq "/Volumes" )
					{
						$g_db_path = "$config_value";
					} else {
						$g_db_path = "${g_source_root}${config_value}";
					}

					if ( path_exists( "${g_db_path}" ) )
					{
						# copy cyrus db to /var/imap
						qx( ${CP} -rpfv "${g_db_path}" "/private/var/imap" 2>&1 >> ${MIGRATION_LOG} );

						log_debug( "- copy user database from: ${g_db_path}" );

						$exit_code = 0;
					} else {
						&print_message( "Error:", "Missing cyrus database: ${g_db_path}" );
					}
				}
			}
		}
	}

	close( cyrus_imapd );

	if ( $exit_code != 0 ) {
		&print_message( "Error:", "Missing configdirectory key in ${g_source_root}" ); }

	return( $exit_code );
} # do_cyrus_db_cleanup


#################################################################
# do_dovecot_data_migration ()
#
#  Note: Routine has been deprecated 

sub do_dovecot_data_migration ($)
{
	my( $src_path ) = $_[0];
	my( $dst_path ) = $_[1];

	log_funct( "do_dovecot_data_migration" );

	# create a temporary imapd.conf
	if ( path_exists( "${g_imapd_conf}" ) ) {
		qx( ${RM} -rf "${g_imapd_conf}" >> ${MIGRATION_LOG} ); }

	# stuff temp imapd.conf with path to database and default mail store
	#	this is all that is needed for migrating seen flags
	open ( IMAPD_CONF, ">> ${g_imapd_conf}" ) or die("g_imapd_conf: $!\n");
	print IMAPD_CONF "admins: _cyrus\n";
	print IMAPD_CONF "postmaster: postmaster\n";
	print IMAPD_CONF "configdirectory: ${g_db_path}\n";
	print IMAPD_CONF "defaultpartition: default\n";
	print IMAPD_CONF "partition-default: ${src_path}\n";
	close( IMAPD_CONF );

	# are we moving or copying messages
	#	default is to move unless purge is set to 0
	my $cvt_flag = "-m";
	if ( $g_purge == 0 ) {
		$cvt_flag = "-c";
	}

	# migrate default partition
	chdir( "${src_path}/user" ) or die "can't chdir ${src_path}/user: $!";

	my $cyrus_bins;
	if ( path_exists( "${g_source_root}/usr/bin/cyrus/bin" ) ) {
		$cyrus_bins = "${g_source_root}/usr/bin/cyrus/bin";
	} else {
		$cyrus_bins = "/usr/bin/cyrus/bin";
	}

	my @mail_accts = <*>;
	foreach my $user_id (@mail_accts)
	{
		print LOG_FILE "Migrating user account: ${user_id} to: ${dst_path}\n";

		# get first initial from user account name
		#	used for mapping into cyrus database directory layout
		my $user_tag = substr( $user_id, 0, 1 );

		log_debug( "- verifying user seen db: \"${g_db_path}/user/${user_tag}/${user_id}.seen\"" );

		if ( path_exists( "${g_db_path}/user/${user_tag}/${user_id}.seen" ) )
		{
			# Convert from skiplist to flat
			qx( "${cyrus_bins}/cvt_cyrusdb" -C "${g_imapd_conf}" "${g_db_path}/user/${user_tag}/${user_id}.seen" skiplist "${g_db_path}/user/${user_tag}/${user_id}.seen.flat" flat >> "${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );

			# Migrate mail data
			if ( ${DEBUG} ) {
				print LOG_FILE "-  ${CVT_MAIL_DATA} -g ${cvt_flag} -d ${g_db_path} -s ${src_path} -t ${dst_path} -a ${user_id}\n";
				qx( ${CVT_MAIL_DATA} -g ${cvt_flag} -d "${g_db_path}" -s "${src_path}" -t "${dst_path}" -a ${user_id} >> "${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );
			} else {
				qx( ${CVT_MAIL_DATA} ${cvt_flag} -d "${g_db_path}" -s "${src_path}" -t "${dst_path}" -a ${user_id} >> "${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );
			}

			# clean up
			qx( ${RM} "${g_db_path}/user/${user_tag}/${user_id}.seen.flat" >> "${MIGRATION_LOG}" );
		} else {
			# Do mail migration without setting any seen flags
			if ( ${DEBUG} ) {
				print LOG_FILE "-  ${CVT_MAIL_DATA} -g ${cvt_flag} -d ${g_db_path} -s ${src_path} -t ${dst_path} -a ${user_id}\n";
				qx( ${CVT_MAIL_DATA} -g ${cvt_flag} -d "${g_db_path}" -s "${src_path}" -t "${dst_path}" -a ${user_id} >> "${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );
			} else {
				qx( ${CVT_MAIL_DATA} ${cvt_flag} -d "${g_db_path}" -s "${src_path}" -t "${dst_path}" -a ${user_id} >> "${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );
			}
		}

		# do user id to guid mapping
		my $user_guid = qx( ${CVT_MAIL_DATA} -i ${user_id} );
		chomp( ${user_guid} );
		if ( substr(${user_guid}, 0, 13) eq "No GUID found" ) {
			qx( chown -R _dovecot:mail "${dst_path}/${user_id}" >>"${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );
		} else {
			qx( chown -R _dovecot:mail "${dst_path}/${user_guid}" >>"${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );
		}
	}

	# clean up temp imapd.conf
	if ( path_exists( "${g_imapd_conf}" ) ) {
		qx( ${RM} -rf "${g_imapd_conf}" >> ${MIGRATION_LOG} ); }

} # do_dovecot_data_migration


#################################################################
# do_cyrus_dovecot_migration ()

sub do_cyrus_dovecot_migration ()
{
	my $INDEX=0;

	log_funct( "do_cyrus_dovecot_migration" );
 
	# get partitions from imapd.conf
	get_mail_partitions();

	# set partition paths to be migrated later
	qx( ${PLIST_BUDDY} -c 'Add :sourceRoot string ${g_source_root}' ${g_migration_plist} );
	qx( ${PLIST_BUDDY} -c 'Add :config_directory string ${g_db_path}' ${g_migration_plist} );
	qx( ${PLIST_BUDDY} -c 'Add :default_partition string ${g_default_partition}' ${g_migration_plist} );
	qx( ${PLIST_BUDDY} -c 'Add :alternate_partitions array' ${g_migration_plist} );

	# set alt partition info to migration plist
	foreach ( @g_partitions ) {
		my( $a_partition ) = $_;
		qx( ${PLIST_BUDDY} -c 'Add :alternate_partitions:${INDEX} string ${a_partition}' ${g_migration_plist} );
		${INDEX} = ${INDEX} + 1;
	}
} # do_cyrus_dovecot_migration


#################################################################
# do_cyrus_config_check ()

sub do_cyrus_config_check ($)
{
	my ( $arg_1 ) = @_;

	log_funct( "do_cyrus_config_check" );

	log_debug( "- Checking for: %s\n${g_source_root}${IMAPD_CONF}" );

	if ( ! path_exists( "${g_source_root}${IMAPD_CONF}" ) )
	{
		&print_message( "Error:", "Missing configuration file: ${g_source_root}" . "${IMAPD_CONF}", "No ${$arg_1} data was migrated." );
		return( 1 );
	}

	return( 0 );
} # do_cyrus_config_check


#################################################################
# migrate_cyrus_config ()

sub migrate_cyrus_config ()
{
	log_funct( "migrate_cyrus_config" );

	if ( do_cyrus_config_check( "configuration" ) ) {
		return;
	}

	#################################################################
	# Setup temp partition file
	my $clear_auth_enabled = "yes";
	my $quota_warn_value = 90;	# 10.6 default
	my $INDEX=0;
	my $TMP_FILE="/Library/Server/Mail/Config/dovecot/tmp-partitions.txt";
	if ( path_exists("${TMP_FILE}") ) {
		unlink("${TMP_FILE}"); }

	open( CYRUS_CONFIG, "<${g_source_root}" . ${IMAPD_CONF} );
	while( <CYRUS_CONFIG> ) {
		# store $_ value because subsequent operations may change it
		my( $config_key ) = $_;
		my( $config_value ) = $_;

		# strip the trailing newline from the line
		chomp($config_key);
		chomp($config_value);

		log_debug( "- line: $config_key" );

		if ( $config_key =~ s/:.*// )
		{
			if ( $config_value =~ s/^.*:// )
			{
				# trim whitespace
				$config_value =~ s/^\s+//;
				$config_value =~ s/\s+$//;

				log_debug( "- key: $config_key,  value: $config_value" );

				my $TAG = substr( $config_key, 0, 10 );
				if ( $TAG eq "partition-" ) {
					$TAG = substr( $config_key, 10, (length($config_key) - 10) );

					log_debug( "- tag: $TAG,  value: $config_value" );

					if ( "$TAG" eq "default" )
					{
						my $new_default;
						if ($config_value eq "/var/spool/imap") {
							$new_default = "/Library/Server/Mail/Data/mail";
						} else {
							$new_default = "$config_value/dovecot";
						}
						qx( ${SERVER_ADMIN} settings mail:imap:partition-default = \"$new_default\" >> ${MIGRATION_LOG} );
					} else {
						qx( /bin/echo "mail:imap:partitions:_array_index:${INDEX}:path = \"${config_value}/dovecot\"" >> ${TMP_FILE} );
						qx( /bin/echo "mail:imap:partitions:_array_index:${INDEX}:partition = \"${TAG}\"" >> ${TMP_FILE} );
						$INDEX = $INDEX + 1;
					}
				} else {
					SWITCH: {
						if ( $config_key eq "imap_auth_clear" ) {
							if ( $config_value eq "no" ) {
								$clear_auth_enabled = "no";
							}
							last SWITCH;
						}
						if ( $config_key eq "imap_auth_gssapi" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_gssapi = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "imap_auth_cram_md5" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_cram_md5 = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "imap_auth_login" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_login = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "imap_auth_plain" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_plain = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "pop_auth_apop" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:pop_auth_apop = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "lmtp_over_quota_perm_failure" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:lmtp_over_quota_perm_failure = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "enable_imap" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:enable_imap = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "enable_pop" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:enable_pop = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "enable_quota_warnings" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:enable_quota_warnings = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "quota_enforce_restrictions" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:quota_enforce_restrictions = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "quotawarn" ) {
							$quota_warn_value = $config_value;
							last SWITCH;
						}
						if ( $config_key eq "log_rolling_days" ) {
							qx( ${SERVER_ADMIN} settings mail:postfix:log_rolling_days = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "log_rolling_days_enabled" ) {
							qx( ${SERVER_ADMIN} settings mail:postfix:log_rolling_days_enabled = $config_value >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "lmtp_luser_relay" ) {
							qx( ${SERVER_ADMIN} settings mail:imap:lmtp_luser_relay = $config_value >> ${MIGRATION_LOG} );
							qx( ${SERVER_ADMIN} settings mail:imap:lmtp_luser_relay_enabled = yes >> ${MIGRATION_LOG} );
							qx( ${SERVER_ADMIN} settings mail:postfix:luser_relay_enabled = yes >> ${MIGRATION_LOG} );
							last SWITCH;
						}
						if ( $config_key eq "tls_server_options" ) {
							if ( ($config_value eq "use") || ($config_value eq "require") )
							{
								if ( !($g_dovecot_ssl_key eq "") && !($g_dovecot_ssl_cert eq "") )
								{
									qx( ${SERVER_ADMIN} settings mail:imap:tls_server_options = $config_value >> ${MIGRATION_LOG} );
									qx( ${SERVER_ADMIN} settings mail:imap:tls_key_file = /etc/certificates/${g_dovecot_ssl_key} >> ${MIGRATION_LOG} );
									qx( ${SERVER_ADMIN} settings mail:imap:tls_cert_file = /etc/certificates/${g_dovecot_ssl_cert} >> ${MIGRATION_LOG} );
									qx( ${SERVER_ADMIN} settings mail:imap:tls_ca_file = /etc/certificates/${g_dovecot_ssl_ca} >> ${MIGRATION_LOG} );
								} else {
									qx( ${SERVER_ADMIN} settings mail:imap:tls_server_options = "none" >> ${MIGRATION_LOG} );
									&print_message( "Warning:", "SSL for POP/IMAP was configured with: $config_value.",
													"The migration script was unable determine SSL certificate mapping",
													"You will need to manually enable SSL for POP/IMAP" );
								}
							}
						}
						last SWITCH;
					}
				}
			}
		}
	}
	close( CYRUS_CONFIG );

	# set quota warning level
	qx( ${SERVER_ADMIN} settings mail:imap:quotawarn = $quota_warn_value >> ${MIGRATION_LOG} );

	# set clear auth 'after' all other settings to make sure PLAIN is now enabled
	#	clear requires that PLAIN be enabled in dovecot
	qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_clear = $clear_auth_enabled >> ${MIGRATION_LOG} );

	# set alt mail store locations
	if ( path_exists("${TMP_FILE}") ) {
		qx( ${SERVER_ADMIN} settings < "${TMP_FILE}" >> ${MIGRATION_LOG} );
		unlink("${TMP_FILE}");
	}
} # migrate_cyrus_config


#################################################################
# sedconf ()

sub sedconf
{
	die unless @_ == 6;
	my $conf = shift or die;
	my $cmt = shift;
	my $key = shift or die;
	my $action = shift or die;
	my $value = shift;
	my $secpat = shift;

	log_funct( "sedconf" );

	my $srcpath = "${g_target_root}${DOVECOT_CONFIG}/$conf";
	if (!open(SRC, "<", $srcpath)) {
		print LOG_FILE "can't read config file $srcpath: $!\n";
		return;
	}

	my $dstpath = "${g_target_root}${DOVECOT_CONFIG}/$conf.new";
	if (!open(DST, ">", $dstpath)) {
		print LOG_FILE "can't create config file $dstpath: $!\n";
		close SRC;
		return;
	}

	log_debug( "src path: $srcpath" );
	log_debug( "dst path: $dstpath" );
	log_debug( "    conf: $conf" );
	log_debug( "     cmt: $cmt" );
	log_debug( "     key: $key" );
	log_debug( "  action: $action" );
	log_debug( "    conf: $conf" );
	log_debug( "   value: $value" );
	log_debug( "  secpat: $secpat" );

	my $cmtpat = "";
	$cmtpat = qr{(?:#\s*)?} if $cmt;

	my $done = 0;
	my $unneeded = 0;
	my @section;
	while (my $line = <SRC>) {
		chomp $line;
		$line =~ s/^(\s*)//;
		my $indent = $1;

		if ($line =~ /^([^#]+){/) {
			# begin conf section
			push @section, $1;
		} elsif ($line =~ /^}/) {
			# end conf section
			pop @section;
		} elsif ($done) {
			# skip
		} elsif (!defined($secpat) || (@section == 1 && $section[0] =~ $secpat)) {
			if ($action eq "=") {
				# replace value
				log_debug( "    line: $line" );
				if ($line =~ s/^$cmtpat($key\s*=\s*).*$/$1$value/) {
					$done = 1;
				}
			} elsif ($action eq "+") {
				# append to list value if not already there
				log_debug( "    line: $line" );
				if ($line =~ /$key\s*=.*(\s|=)$value(\s|$)/) {
					$unneeded = 1;
				} elsif ($line =~ s/^$cmtpat($key\s*=\s*.*)/$1 $value/) {
					$done = 1;
				}
			} elsif ($action eq "-") {
				# remove from list value
				log_debug( "    line: $line" );
				if ($line =~ s/^$cmtpat($key\s*=\s*)$value(\s.*|$)/$1$2/ ||
				    $line =~ s/^$cmtpat($key\s*=.*)\s$value(\s.*|$)/$1$2/) {
					$done = 1;
				} elsif ($line =~ /^$cmtpat$key\s*=/) {
					$unneeded = 1;
				}
			} else {
				die;
			}
		}
		print DST "$indent$line\n";
	}

	close DST;
	close SRC;

	if (!$done) {
		if (!$unneeded) {
			print LOG_FILE "key \"$key\" not found in $srcpath, can't change value ($action $value)\n";
		}
		unlink($dstpath);
		return;
	}

	my $savedir = "${g_target_root}${DOVECOT_CONFIG}/pre-migrate";
	mkdir($savedir, 0755);
	mkdir("$savedir/conf.d", 0755);
	my $savepath = "$savedir/$conf";
	if (!rename($srcpath, $savepath)) {
		print LOG_FILE "can't rename $srcpath -> $savepath: $!\n";
		return;
	}
	if (!rename($dstpath, $srcpath)) {
		print LOG_FILE "can't rename $dstpath -> $srcpath: $!\n";
		return;
	}
}

#################################################################
# copy_dovecot_config ()

sub copy_dovecot_config ()
{
	log_funct( "copy_dovecot_config" );

	# copy 10.7.x config settings into default config location
	qx(${CP} -rpf "${g_source_root}/private/etc/dovecot" "${g_target_root}${CONFIG_ROOT}/" );
} 


#################################################################
# copy_dovecot_config ()

sub update_dovecot_config_paths ()
{
	log_funct( "update_dovecot_config_paths" );
	print LOG_FILE "Updating POP/IMAP configuation paths\n";

	# modify paths to point to new conig location
	my $src = "${g_target_root}${CONFIG_ROOT}/dovecot/conf.d/auth-od.conf.ext";
	my $dst = "${g_target_root}${CONFIG_ROOT}/dovecot/conf.d/auth-od.conf.ext.new";
	my $value = qx( ${GREP} "partition=/etc/dovecot/partition_map.conf" $src );
	if ( !($value eq "") ) {
		qx( /usr/bin/sed -e "s/\\/etc\\/dovecot\\/partition_map.conf/\\/Library\\/Server\\/Mail\\/Config\\/dovecot\\/partition_map.conf/" "$src" > "$dst" );
		move( "$src",  "$src" . ".$g_source_version" . ".orig" );
		move( "$dst", "$src" );
	}

	# modify paths to point to new conig location
	$src = "${g_target_root}${CONFIG_ROOT}/dovecot/conf.d/auth-submit.conf.ext";
	$dst = "${g_target_root}${CONFIG_ROOT}/dovecot/conf.d/auth-submit.conf.ext.new";
	my $value = qx( ${GREP} "/etc/dovecot/submit.passdb" $src );
	if ( !($value eq "") ) {
		qx( /usr/bin/sed -e "s/\\/etc\\/dovecot\\/submit.passdb/\\/Library\\/Server\\/Mail\\/Config\\/dovecot\\/submit.passdb/" "$src" > "$dst" );
		move( "$src",  "$src" . ".$g_source_version" . ".orig" );
		move( "$dst", "$src" );
	}

	# if data vol was set to /var/spool/imap/dovecot, then make target /Library/Server/Mail/Data/mail
	my $mail_loc = qx( ${SERVER_ADMIN} settings mail:imap:partition-default );
	chomp($mail_loc);
	my $offset = index($mail_loc, "/var/spool/imap/dovecot/mail");
	if ( index($mail_loc, "/var/spool/imap/dovecot/mail") > 0 ) {
		qx( ${SERVER_ADMIN} settings mail:imap:partition-default = /Library/Server/Mail/Data/mail );
	}

	$src = "${g_target_root}${CONFIG_ROOT}/dovecot/conf.d/10-master.conf";
	$dst = "${g_target_root}${CONFIG_ROOT}/dovecot/conf.d/10-master.conf.new";
	qx( /usr/bin/sed -e "1,/    #user =/s/    #user =/    user = _dovecot/" "$src" > "$dst" );
	move( "$src",  "$src" . ".$g_source_version" . ".orig" );
	move( "$dst", "$src" );
} 

#################################################################
# migrate_dovecot_config ()

sub migrate_dovecot_config ()
{
	log_funct( "migrate_dovecot_config" );

	print LOG_FILE "Migrating previous POP/IMAP configuation settings";

	my ($hostname) = qx($GREP "^myhostname *=" "${g_target_root}${POSTFIX_CONFIG}/main.cf" 2>>$MIGRATION_LOG | sed 's,.*= *,,');
	chomp $hostname;
	if (!defined($hostname) || $hostname eq "") {
		$hostname = qx(hostname);
		chomp $hostname;
	}

	my $oldtag;
	if ($g_source_version =~ /^10(\.\d+)+$/) {
		$oldtag = $g_source_version;
	} else {
		$oldtag = "old";
	}
	if (path_exists("${g_source_root}/private/etc/dovecot")) {
		qx(${RM} -rf "${g_target_root}${DOVECOT_CONFIG}/$oldtag" >> ${MIGRATION_LOG} 2>> ${MIGRATION_LOG});
		qx(${CP} -rpf "${g_source_root}/private/etc/dovecot" "${g_target_root}${DOVECOT_CONFIG}/$oldtag" >> ${MIGRATION_LOG} 2>> ${MIGRATION_LOG});
	}

	# All the config files that may change.  Others not listed won't.
	my $imap_conf =		"dovecot.conf";
	my $imap_conf_auth =	"conf.d/10-auth.conf";
	my $imap_conf_logging =	"conf.d/10-logging.conf";
	my $imap_conf_mail =	"conf.d/10-mail.conf";
	my $imap_conf_master =	"conf.d/10-master.conf";
	my $imap_conf_ssl =	"conf.d/10-ssl.conf";
	my $imap_conf_lda =	"conf.d/15-lda.conf";
	my $imap_conf_imap =	"conf.d/20-imap.conf";
	my $imap_conf_lmtp =	"conf.d/20-lmtp.conf";
	my $imap_conf_plugin =	"conf.d/90-plugin.conf";
	my $imap_conf_quota =	"conf.d/90-quota.conf";
	my $imap_conf_od =	"conf.d/auth-od.conf.ext";
	my @conf_files = ($imap_conf,
			  $imap_conf_auth,
			  $imap_conf_logging,
			  $imap_conf_mail,
			  $imap_conf_master,
			  $imap_conf_ssl,
			  $imap_conf_lda,
			  $imap_conf_imap,
			  $imap_conf_lmtp,
			  $imap_conf_plugin,
			  $imap_conf_quota,
			  $imap_conf_od);

	# All the config parameters that Server Admin or one of the mail
	# setup scripts could have changed, in the order in which they
	# appear in the config file.  "Hot" means uncommented.
	my %val = ("protocols"			=> undef,
		   "disable_plaintext_auth"	=> undef,
		   "ssl"			=> undef,
		   "ssl_cert"			=> undef,
		   "ssl_key"			=> undef,
		   "ssl_ca"			=> undef,
		   "mail_location"		=> undef,
		   "mail_debug"			=> undef,
		   "mmap_disable"		=> undef,
		   "dotlock_use_excl"		=> undef,
		   "max_mail_processes"		=> undef,
		   "aps_topic"			=> undef,
		   "postmaster_address"		=> undef,
		   "hostname"			=> undef,
		   "lda_plugins"		=> undef,
		   "auth_debug"			=> undef,
		   "auth_debug_passwords"	=> undef,
		   "auth_mechanisms"		=> undef,
		   "userdb_od_args"		=> undef,
		   "quota_warning"		=> undef,
		   "quota_warning2"		=> undef);
	my %hot;

	# determine all options set in old config file(s)
	my $from_dovecot2 = path_exists("${g_target_root}${DOVECOT_CONFIG}/$oldtag/conf.d");
	for my $file (@conf_files) {
		my $dcold = "${g_target_root}${DOVECOT_CONFIG}/$oldtag/$file";
		if (open(DCOLD, "<", $dcold)) {
			my @section;
			while (my $line = <DCOLD>) {
				chomp $line;
				$line =~ s/^\s+//;
				my $hot = !($line =~ s/^#\s*//);

				if ($hot && $line =~ /^([^#]+){/) {
					# begin conf section
					push @section, $1;
				} elsif ($hot && $line =~ /^}/) {
					# end conf section
					pop @section;
				} elsif ($line =~ /^(protocols)\s*=\s*(.*)/ && ($hot || !defined($hot{$1}))) {
					die unless exists $val{$1};	# check for typos
					@{$val{$1}} = split(/\s+/, $2);
					$hot{$1} = $hot;
				} elsif (@section == 0 &&
					 $line =~ /^(disable_plaintext_auth |
						     ssl |
						     mail_location |
						     mail_debug |
						     mmap_disable |
						     dotlock_use_excl |
						     max_mail_processes |
						     auth_debug |
						     auth_debug_passwords)\s*=\s*(.*)/x &&
					 ($hot || !defined($hot{$1}))) {
					die unless exists $val{$1};	# check for typos
					$val{$1} = $2;
					$hot{$1} = $hot;
				} elsif ($line =~ /^ssl_disable\s*=\s*(.*)/ && ($hot || !defined($hot{ssl}))) {
					if ($1 eq "yes") {
						$val{ssl} = "no";
					} elsif ($hot{protocols} && grep { $_ eq "imap" || $_ eq "pop3" } @{$val{protocols}}) {
						$val{ssl} = "yes";
					} else {
						$val{ssl} = "required";
					}
					$hot{ssl} = $hot;
				} elsif ($line =~ /^(ssl_(?:cert|key|ca))(?:_file)?\s*=\s*(.*)/ && ($hot || !defined($hot{$1}))) {
					die unless exists $val{$1};	# check for typos
					$val{$1} = $2;
					$hot{$1} = $hot;
				} elsif (@section == 1 && $section[0] =~ /^protocol\s+imap\s+/ &&
					 $line =~ /^(aps_topic)\s*=\s*(.*)/x &&
					 ($hot || !defined($hot{$1}))) {
					die unless exists $val{$1};	# check for typos
					$val{$1} = $2;
					$hot{$1} = $hot;
				} elsif (@section == 1 && $section[0] =~ /^protocol\s+lda\s+/ &&
					 $line =~ /^(postmaster_address |
						     hostname)\s*=\s*(.*)/x &&
					 ($hot || !defined($hot{$1}))) {
					die unless exists $val{$1};	# check for typos
					$val{$1} = $2;
					$hot{$1} = $hot;
				} elsif (@section == 1 && $section[0] =~ /^protocol\s+lda\s+/ &&
					 $line =~ /^mail_plugins\s*=\s*(.*)/ &&
					 ($hot || !defined($hot{lda_plugins}))) {
					@{$val{lda_plugins}} = split(/\s+/, $1);
					$hot{lda_plugins} = $hot;
				} elsif (@section == 1 && $section[0] =~ /^auth\s+default\s+/ &&
					 $line =~ /^mechanisms\s*=\s*(.*)/ &&
					 ($hot || !defined($hot{auth_mechanisms}))) {
					@{$val{auth_mechanisms}} = split(/\s+/, $1);
					$hot{auth_mechanisms} = $hot;
				} elsif (@section == 2 && $section[0] =~ /^auth\s+default\s+/ && $section[1] =~ /^userdb\s+od\s+/ &&
					 $line =~ /^args\s*=\s*(.*)/ &&
					 ($hot || !defined($hot{userdb_od_args}))) {
					@{$val{userdb_od_args}} = split(/\s+/, $1);
					$hot{userdb_od_args} = $hot;
				} elsif (@section == 1 && $section[0] =~ /^plugin\s+$/ &&
					 $line =~ /^(quota_warning2?)\s*=\s*(.*)/ &&
					 ($hot || !defined($hot{$1}))) {
					die unless exists $val{$1};	# check for typos
					$val{$1} = $2;
					$hot{$1} = $hot;
				}
			}
			close(DCOLD);
		} elsif ($file !~ /conf\.d/ || $from_dovecot2) {
			print LOG_FILE "can't read $dcold: $!\n";
		}
	}

	# convert options
	if ($hot{ssl_cert} && $val{ssl_cert} !~ /^</) {
		$val{ssl_cert} = "<$val{ssl_cert}";
	}
	if ($hot{ssl_key} && $val{ssl_key} !~ /^</) {
		$val{ssl_key} = "<$val{ssl_key}";
	}
	if ($hot{ssl_ca} && $val{ssl_ca} !~ /^</) {
		$val{ssl_ca} = "<$val{ssl_ca}";
	}
	if ($hot{quota_warning} && $val{quota_warning} =~ /storage=(\d+)%/) {
		$val{quota_warning} = "storage=$1%% quota-exceeded \%u";
	}
	if ($hot{quota_warning2} && $val{quota_warning2} =~ /storage=(\d+)%/) {
		$val{quota_warning2} = "storage=$1%% quota-warning \%u";
	}

	# set appropriate options in new config files
	sedconf($imap_conf,		1, "protocols",			"-", "imap",					undef)				if $hot{protocols}
		and !grep { $_ eq "imap" || $_ eq "imaps" } @{$val{protocols}};
	sedconf($imap_conf,		1, "protocols",			"-", "pop3",					undef)				if $hot{protocols}
		and !grep { $_ eq "pop3" || $_ eq "pop3s" } @{$val{protocols}};
	sedconf($imap_conf,		1, "protocols",			"+", "sieve",					undef)				if $hot{protocols}
		and grep { $_ eq "managesieve" } @{$val{protocols}};
	sedconf($imap_conf_auth,	1, "disable_plaintext_auth",	"=", $val{disable_plaintext_auth},		undef)				if $hot{disable_plaintext_auth};
	sedconf($imap_conf_ssl,		1, "ssl",			"=", $val{ssl},						undef)				if $hot{ssl};
	sedconf($imap_conf_ssl,		1, "ssl_cert",			"=", $val{ssl_cert},			undef)				if $hot{ssl_cert};
	sedconf($imap_conf_ssl,		1, "ssl_key",			"=", $val{ssl_key},				undef)				if $hot{ssl_key};
	sedconf($imap_conf_ssl,		1, "ssl_ca",			"=", $val{ssl_ca},				undef)				if $hot{ssl_ca};
	sedconf($imap_conf_mail,	0, "mail_location",		"=", $val{mail_location},		undef)				if $hot{mail_location};
	sedconf($imap_conf_logging,	1, "mail_debug",		"=", $val{mail_debug},			undef)				if $hot{mail_debug};
	sedconf($imap_conf_mail,	1, "mmap_disable",		"=", $val{mmap_disable},		undef)				if $hot{mmap_disable};
	sedconf($imap_conf_mail,	1, "dotlock_use_excl",	"=", $val{dotlock_use_excl},	undef)				if $hot{dotlock_use_excl};
	#sedconf($imap_conf_master,	1, "user",				"=", "_dovecot_xxx",			qr{^service\s+auth\s+});
	sedconf($imap_conf_master,	1, "process_limit",		"=", $val{max_mail_processes},	qr{^service\s+(imap|pop3)\s+})	if $hot{max_mail_processes};
	sedconf($imap_conf,			1, "aps_topic",			"=", $val{aps_topic},			undef)				if $hot{aps_topic};
	sedconf($imap_conf_lda,		1, "postmaster_address","=", $val{postmaster_address},	undef)				if $hot{postmaster_address}
		and $val{postmaster_address} !~ /example\.com/;
	sedconf($imap_conf_lda,		1, "hostname",			"=", $val{hostname},			undef)				if $hot{hostname};
	sedconf($imap_conf_imap,	1, "mail_plugins",		"+", "urlauth",					qr{^protocol\s+imap\s+})	if $hot{ssl}
		and ($val{ssl} eq "yes" || $val{ssl} eq "required");
	sedconf($imap_conf_lda,		1, "mail_plugins",		"+", "sieve",					qr{^protocol\s+lda\s+})		if $hot{lda_plugins}
		and grep { $_ eq "cmusieve" } @{$val{lda_plugins}};
	sedconf($imap_conf_lda,		1, "mail_plugins",		"+", "push_notify",				qr{^protocol\s+lda\s+})		if $hot{lda_plugins}
		and grep { $_ eq "push_notify" } @{$val{lda_plugins}};
	sedconf($imap_conf_logging,	1, "auth_debug",		"=", $val{auth_debug},				undef)				if $hot{auth_debug};
	sedconf($imap_conf_logging,	1, "auth_debug_passwords",	"=", $val{auth_debug_passwords},		undef)				if $hot{auth_debug_passwords};
	if ($hot{auth_mechanisms}) {
		sedconf($imap_conf_auth, 1, "auth_mechanisms",	"-", "cram-md5",				undef)
			if !grep { $_ eq "cram-md5" } @{$val{auth_mechanisms}};
		sedconf($imap_conf_auth, 1, "auth_mechanisms",	"+", $_,					undef)
			for @{$val{auth_mechanisms}};
	}
	sedconf($imap_conf_od,		1, "args",			"=", join(" ", @{$val{userdb_od_args}}),	qr{^userdb\s+})			if $hot{userdb_od_args};
	sedconf($imap_conf_quota,	1, "quota_warning",	"=", $val{quota_warning},			qr{^plugin\s+})			if $hot{quota_warning};
	sedconf($imap_conf_quota,	1, "quota_warning2","=", $val{quota_warning2},			qr{^plugin\s+})			if $hot{quota_warning2};
	qx($CP -f "${g_target_root}${DOVECOT_CONFIG}/$oldtag/partition_map.conf" "${g_target_root}${DOVECOT_CONFIG}/partition_map.conf");

	# Create submit.passdb with either the same password postfix is
	# configured for, or an unguessable random password.
	if (!path_exists("${g_target_root}${DOVECOT_CONFIG}/submit.passdb")) {
		my $pw;
		if (defined($hostname) && $hostname ne "" && path_exists("${g_target_root}${POSTFIX_CONFIG}/submit.cred")) {
			($pw) = qx($GREP "^$hostname|submit|" "${g_target_root}${POSTFIX_CONFIG}/submit.cred" 2>>$MIGRATION_LOG | sed 's,.*|,,');
			chomp $pw;
		}
		if (!defined($pw) || $pw eq "") {
			($pw) = qx(dd if=/dev/urandom bs=256 count=1 2>>$MIGRATION_LOG | env LANG=C tr -dc a-zA-Z0-9 | cut -b 1-22);
			chomp $pw;
		}
		if (defined($pw) && $pw ne "") {
			my $spnew = "${g_target_root}${DOVECOT_CONFIG}/submit.passdb";
			if (open(SPNEW, ">", $spnew)) {
				print SPNEW "submit:{PLAIN}$pw\n";
				close(SPNEW);
			} else {
				print LOG_FILE "can't write $spnew: $!\n";
			}
			qx( ${CHOWN} :mail "${g_target_root}${DOVECOT_CONFIG}/submit.passdb" >> "${MIGRATION_LOG}" 2>> "${MIGRATION_LOG}" );
			chmod(0640, "${g_target_root}${DOVECOT_CONFIG}/submit.passdb");
		}
	}

	# dovecot notify
	if ( path_exists( "${g_target_root}${DOVECOT_CONFIG}/notify" ) ) {
		qx( ${CHOWN} _dovecot:mail "${g_target_root}${DOVECOT_CONFIG}/notify" >> "${MIGRATION_LOG}" 2>> "${MIGRATION_LOG}" );
	}

	# mailusers.plist
	unlink("${g_target_root}/private/var/db/.mailusers.plist");

	# dovecot.fts.update
	mkdir("${g_target_root}/private/var/db/dovecot.fts.update");
	qx(${CHOWN} _dovecot:mail "${g_target_root}/private/var/db/dovecot.fts.update" >> "${MIGRATION_LOG}" 2>> "${MIGRATION_LOG}");
	chmod(0770, "${g_target_root}/private/var/db/dovecot.fts.update");

} # migrate_dovecot_config


#################################################################
# migrate_postfix_spool ()

sub migrate_postfix_spool ()
{
	log_funct( "migrate_postfix_spool" );

	print LOG_FILE "Begin SMTP spool data migraiont\n";

	my $src_spool_var = "${g_source_root}/private/var/spool/postfix";
	my $src_spool_lib = "${g_source_root}/Library/Server/Mail/Data/spool";
	my $dst_spool	  = "${g_target_root}/Library/Server/Mail/Data/spool";

	# create dest dir if it doesn't exist
	if ( !path_exists($dst_spool) ) {
		qx(${MKDIR} -p -m 755 "${g_target_root}/Library/Server/Mail/Data/spool");
	}

	# copy from 10.7 or 10.6 src spools
	if ( path_exists(${src_spool_lib}) ) {
		print LOG_FILE "migrating postfix spool data from: $src_spool_lib to: $dst_spool\n";
		qx( rsync -av ${src_spool_lib} ${dst_spool} >> ${MIGRATION_LOG} );
	} elsif ( path_exists(${src_spool_var}) ) {
		print LOG_FILE "migrating postfix spool data from: $src_spool_lib to: $dst_spool\n";
		qx( rsync -av ${src_spool_var} ${dst_spool} >> ${MIGRATION_LOG} );
	} else {
		print LOG_FILE "Warning: no source mail spool found\n";
	}

	print LOG_FILE "Finished SMTP spool data migraiont\n";

} # migrate_postfix_spool


#################################################################
# migrate_postfix_config ()

sub migrate_postfix_config ()
{
	log_funct( "migrate_postfix_config" );
	print LOG_FILE "Migrating SMTP configuration\n";

	# keep a copy of default 10.8 default file
	if ( path_exists( "${g_target_root}${POSTFIX_CONFIG}/main.cf.default" ) ) {
		qx( ${CP} -f "${g_target_root}${POSTFIX_CONFIG}/main.cf.default" "${g_target_root}${POSTFIX_CONFIG}/main.cf.default.$TARGET_VER" >> ${MIGRATION_LOG} 2>> "${MIGRATION_LOG}" );
	}

	if ( path_exists( "${g_target_root}${POSTFIX_CONFIG}/master.cf.default" ) ) {
		qx( ${CP} -f "${g_target_root}${POSTFIX_CONFIG}/master.cf.default" "${g_target_root}${POSTFIX_CONFIG}/master.cf.default.$TARGET_VER" >> ${MIGRATION_LOG} 2>> "${MIGRATION_LOG}" );
	}

	# copy config files from previous system
	print LOG_FILE "Restoring SMTP service configuration from:\n";
	if ( path_exists("${g_source_root}${POSTFIX_CONFIG}") ) {
		print LOG_FILE "       source: ${g_source_root}${POSTFIX_CONFIG}\n";
		print LOG_FILE "  destination: ${g_target_root}${POSTFIX_CONFIG}\n";
		print LOG_FILE "           by: ${CP} -rpfv ${g_source_root}${POSTFIX_CONFIG} ${g_target_root}/Library/Server/Mail/Config/\n";

		qx( ${CP} -rpfv "${g_source_root}${POSTFIX_CONFIG}" "${g_target_root}/Library/Server/Mail/Config/" >> ${MIGRATION_LOG} 2>> "${MIGRATION_LOG}" );
	} elsif ( path_exists("${g_source_root}/private/etc/postfix") ) {
		print LOG_FILE "       source: ${g_source_root}/private/etc/postfix\n";
		print LOG_FILE "  destination: ${g_target_root}${POSTFIX_CONFIG}\n";
		print LOG_FILE "           by: ${CP} -rpfv ${g_source_root}/private/etc/postfix ${g_target_root}/Library/Server/Mail/Config/\n";

		print LOG_FILE "Copying postfix configuration: ${g_source_root}/private/etc/postfix to: ${g_target_root}${POSTFIX_CONFIG}\n";
		qx( ${CP} -rpfv "${g_source_root}/private/etc/postfix" "${g_target_root}/Library/Server/Mail/Config/" >> ${MIGRATION_LOG} 2>> "${MIGRATION_LOG}" );
	} 

	# now copy main & master cf files from ~orig
	print LOG_FILE "Restoring SMTP configuration files\n";
	if ( path_exists("${g_source_root}/private/etc/postfix~orig/main.cf") ) {
		# move previous copy out of the way
		if ( path_exists("${g_target_root}${POSTFIX_CONFIG}/main.cf") ) {
			move( "${g_target_root}${POSTFIX_CONFIG}/main.cf", "${g_target_root}${POSTFIX_CONFIG}/main.cf.$g_source_version" );
		}
		copy( "${g_source_root}/private/etc/postfix~orig/main.cf", "${g_target_root}${POSTFIX_CONFIG}/main.cf" );
	}

	if ( path_exists("${g_source_root}/private/etc/postfix~orig/master.cf") ) {
		# move previous copy out of the way
		if ( path_exists("${g_target_root}${POSTFIX_CONFIG}/master.cf") ) {
			move( "${g_target_root}${POSTFIX_CONFIG}/master.cf", "${g_target_root}${POSTFIX_CONFIG}/master.cf.$g_source_version" );
		}
		copy( "${g_source_root}/private/etc/postfix~orig/master.cf", "${g_target_root}${POSTFIX_CONFIG}/master.cf" );
	}

	# clean up deprecated keys
	print LOG_FILE "Removing deprecated SMTP configuration keys\n";

	my $cnt = 0;
	my $src = "${g_target_root}${POSTFIX_CONFIG}/main.cf";
	my @deprecated_keys = ("^virus_db_update_enabled", "^virus_db_last_update", "^smtpd_tls_common_name", "^spam_domain_name");

	# make a copy of original
	copy( "$src",  "$src.$g_source_version.orig" );

	for ( $cnt = 0; $cnt < 3; $cnt++) {
		my $value = qx( ${GREP} $deprecated_keys[$cnt] $src );
		if ( !($value eq "") ) {
			my $dst = "${g_target_root}${POSTFIX_CONFIG}/main.cf.um.tmp";
			qx( /usr/bin/sed -e "/$deprecated_keys[$cnt]/d" "$src" > "$dst" );
			move( "$dst", "$src" );
		}
	}

	# fix virtual maps
	my $dst = "${g_target_root}${POSTFIX_CONFIG}/main.cf.um.tmp";
	my $grep_value = qx( ${GREP} "hash:/etc/postfix/virtual_users" $src );
	if ( !($grep_value eq "") ) {
		qx( /usr/bin/sed -e "s/hash:\\/etc\\/postfix\\/virtual_users/hash:\\/Library\\/Server\\/Mail\\/Config\\/postfix\\/virtual_users/" "$src" > "$dst" );
		move( "$dst", "$src" );
	}

	# fix virtual maps
	my $grep_value = qx( ${GREP} "hash:/etc/postfix/virtual_domains" $src );
	if ( !($grep_value eq "") ) {
		qx( /usr/bin/sed -e "s/hash:\\/etc\\/postfix\\/virtual_domains/hash:\\/Library\\/Server\\/Mail\\/Config\\/postfix\\/virtual_domains/" "$src" > "$dst" );
		move( "$dst", "$src" );
	}

	# fix smtpdreject
	my $grep_value = qx( ${GREP} "hash:/etc/postfix/smtpdreject" $src );
	if ( !($grep_value eq "") ) {
		qx( /usr/bin/sed -e "s/hash:\\/etc\\/postfix\\/smtpdreject/hash:\\/Library\\/Server\\/Mail\\/Config\\/postfix\\/smtpdreject/" "$src" > "$dst" );
		move( "$dst", "$src" );
	}

	# fix smtpdreject
	my $grep_value = qx( ${GREP} "cidr:/etc/postfix/smtpdreject.cidr" $src );
	if ( !($grep_value eq "") ) {
		qx( /usr/bin/sed -e "s/cidr:\\/etc\\/postfix\\/smtpdreject.cidr/cidr:\\/Library\\/Server\\/Mail\\/Config\\/postfix\\/smtpdreject.cidr/" "$src" > "$dst" );
		move( "$dst", "$src" );
	}

	# clean legacy smtp_fallback
	$src = "${g_target_root}${POSTFIX_CONFIG}/master.cf";
	my $value = qx( ${GREP} "o fallback_relay=" $src );
	if ( !($value eq "") ) {
		# fix existing master.cf
		my $dst = "${g_target_root}${POSTFIX_CONFIG}/master.cf.um.tmp";
		qx( /usr/bin/sed -e "s/o fallback_relay=/o smtp_fallback_relay=/" "$src" > "$dst" );
		move( "$src",  "$src" . ".$g_source_version" . ".orig" );
		move( "$dst", "$src" );
	}

	print LOG_FILE "Updating SMTP configuration\n";
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e data_directory=/Library/Server/Mail/Data/mta );
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e config_directory=/Library/Server/Mail/Config/postfix );
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e queue_directory=/Library/Server/Mail/Data/spool );

	# setup default relay host sasl directory
	if (!path_exists("${g_target_root}${POSTFIX_CONFIG}/sasl")) {
		mkdir "${g_target_root}${POSTFIX_CONFIG}/sasl";
	}

	# set new smtp_sasl_password_maps if enabled
	my $smtp_sasl_password_maps = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" smtp_sasl_password_maps );
	chomp $smtp_sasl_password_maps;
	if ( $smtp_sasl_password_maps eq "hash:/etc/postfix/sasl/passwd" ) {
		qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e smtp_sasl_password_maps=hash:/Library/Server/Mail/Config/postfix/sasl/passwd );
	}

	# aliases is in /etc/postfix but aliases.db is in /etc and needs updating
	qx( /usr/sbin/postalias "${g_target_root}/private/etc/aliases" >> ${MIGRATION_LOG} 2>> "${MIGRATION_LOG}" );

	# reset $recipient_canonical_maps
	my $recipient_canonical_maps = 	qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" recipient_canonical_maps );
	chomp $recipient_canonical_maps;
	if ( $recipient_canonical_maps eq "hash:/etc/postfix/system_user_maps" ) {
		print LOG_FILE "resetting SMTP recipient canonical maps: recipient_canonical_maps=hash:/Library/Server/Mail/Config/postfix/system_user_maps\n";
		qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e recipient_canonical_maps=hash:/Library/Server/Mail/Config/postfix/system_user_maps );
	}

	# update main.cf
	if (!qx($GREP "imap_submit_cred_file *=" "${g_target_root}${POSTFIX_CONFIG}/main.cf" 2>>$MIGRATION_LOG)) {
		my $mcapp = "${g_target_root}${POSTFIX_CONFIG}/main.cf";
		if (open(MCAPP, ">>", $mcapp)) {
			print MCAPP <<'EOT';
# (APPLE) Credentials for using URLAUTH with IMAP servers.
imap_submit_cred_file = /Library/Server/Mail/Config/postfix/submit.cred

EOT
			close(MCAPP);
		} else {
			print LOG_FILE "can't append to $mcapp: $!\n";
		}
	} else {
		qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e imap_submit_cred_file=/Library/Server/Mail/Config/postfix/submit.cred );
	}

	if (!qx($GREP sacl-cache "${g_target_root}${POSTFIX_CONFIG}/main.cf" 2>>$MIGRATION_LOG)) {
		my $mcapp = "${g_target_root}${POSTFIX_CONFIG}/main.cf";
		if (open(MCAPP, ">>", $mcapp)) {
			print MCAPP <<'EOT';
# (APPLE) The SACL cache caches the results of Mail Service ACL lookups.
# Tune these to make the cache more responsive to changes in the SACL.
# The cache is only in memory, so bouncing the sacl-cache service clears it.
use_sacl_cache = yes
# sacl_cache_positive_expire_time = 7d
# sacl_cache_negative_expire_time = 1d
# sacl_cache_disabled_expire_time = 1m

EOT
			close(MCAPP);
		} else {
			print LOG_FILE "can't append to $mcapp: $!\n";
		}
	}

	# Create submit.cred with either the same password dovecot is
	# configured for, or an unguessable random password.
	if (!path_exists("${g_target_root}${POSTFIX_CONFIG}/submit.cred")) {
		my ($hostname) = qx($GREP "^myhostname *=" "${g_target_root}${POSTFIX_CONFIG}/main.cf" 2>>$MIGRATION_LOG | sed 's,.*= *,,');
		chomp $hostname;
		if (!defined($hostname) || $hostname eq "") {
			($hostname) = qx(hostname);
			chomp $hostname;
		}
		my $pw;
		if (path_exists("${g_target_root}${DOVECOT_CONFIG}/submit.passdb")) {
			($pw) = qx($GREP "^submit:" "${g_target_root}${DOVECOT_CONFIG}/submit.passdb" 2>>$MIGRATION_LOG | sed 's,.*},,');
			chomp $pw;
		}
		if (!defined($pw) || $pw eq "") {
			($pw) = qx(dd if=/dev/urandom bs=256 count=1 2>>$MIGRATION_LOG | env LANG=C tr -dc a-zA-Z0-9 | cut -b 1-22);
			chomp $pw;
		}
		if (defined($pw) && $pw ne "" && defined($hostname) && $hostname ne "") {
			my $scnew = "${g_target_root}${POSTFIX_CONFIG}/submit.cred";
			if (open(SCNEW, ">", $scnew)) {
				print SCNEW "submitcred version 1\n";
				print SCNEW "$hostname|submit|$pw\n";
				close(SCNEW);
			} else {
				print LOG_FILE "can't write $scnew: $!\n";
			}
			chmod(0600, "${g_target_root}${POSTFIX_CONFIG}/submit.cred");
		}
	}

	# remove mailman from alias_maps
	my $alias_maps = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" alias_maps );
	chomp($alias_maps);
	my $line = $alias_maps;
	$line =~ s/,hash:\/var\/mailman\/data\/aliases//;
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e alias_maps='$line' );

} # migrate_postfix_config


#################################################################
# update_master_cf ()

sub update_master_cf ()
{
	my $has_dovecot = 0;
	my $has_greylist = 0;

	# validating master.cf
	print LOG_FILE "Updating ${POSTFIX_CONFIG}/master.cf\n";
	qx( ${SERVER_ADMIN} command mail:command = validateMasterCf >> ${MIGRATION_LOG} );

	# disable virus scanning to allow for config file update
	print LOG_FILE "Disabling junk mail & virus scanning\n";
	qx( ${SERVER_ADMIN} settings mail:postfix:virus_scan_enabled = no >> ${MIGRATION_LOG} );
	qx( ${SERVER_ADMIN} settings mail:postfix:spam_scan_enabled = no >> ${MIGRATION_LOG} );

	# check to see if dovecot is already defined
	open( MASTER_CF, "<${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf" ) or die "can't open ${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf: $!";
	while( <MASTER_CF> )
	{
		# store $_ value because subsequent operations may change it
		my($a_line) = $_;

		# strip the trailing newline from the line
		chomp($a_line);

		if ( substr( ${a_line}, 0, 7) eq "dovecot" ) {
			$has_dovecot = 1;
		}
		if ( substr( ${a_line}, 0, 6) eq "policy" ) {
			$has_greylist = 1;
		}
	}
	close(MASTER_CF);

	if ( path_exists( "${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf.out" ) ) {
		unlink( "${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf.out" );
	}

	my $tlsmgr = 0;
	my $skip_line = 0;
	my $skip_comment = 0;
	my $update_deliver = 0;

	open( MASTER_CF, "<${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf" ) or die "can't open ${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf: $!";
	open (MASTER_CF_OUT, ">${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf.out" ) or die "can't open ${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf.out: $!";
	while( <MASTER_CF> )
	{
		# store $_ value because subsequent operations may change it
		my($a_line) = $_;

		# strip the trailing newline from the line
		chomp($a_line);

		if ( (substr( ${a_line}, 0, 6) eq "tlsmgr") || (substr( ${a_line}, 0, 7) eq "#tlsmgr") )
		{
			if ( $tlsmgr == 0 )
			{
				if ( substr( ${a_line}, 0, 7) eq "#tlsmgr" )
				{
					print MASTER_CF_OUT "${a_line}";
					print MASTER_CF_OUT "\n";
				}
				print MASTER_CF_OUT "tlsmgr    unix  -       -       n       1000?   1       tlsmgr";
				print MASTER_CF_OUT "\n";
				$tlsmgr = 1;
			}
		}
		elsif (substr( ${a_line}, 0, 32) eq "# === End auto-generated section")
		{
			print MASTER_CF_OUT "#submission inet  n       -       n       -       -       smtpd" . "\n";
			print MASTER_CF_OUT "#  -o smtpd_tls_security_level=encrypt" . "\n";

			print MASTER_CF_OUT "${a_line}";
			print MASTER_CF_OUT "\n";
		}
		elsif (substr( ${a_line}, 0, 10) eq "submission")
		{
			# skip enabled submission settings
			$skip_line = 1;
		}
		elsif ( $skip_line == 1 )
		{
			if ( (substr( ${a_line}, 0, 1) eq " ") || (substr( ${a_line}, 0, 1) eq "	") ) {
				# keep skipping until we hit non-whitespace line
			} else {
				$skip_line = 0;
				print MASTER_CF_OUT "${a_line}";
				print MASTER_CF_OUT "\n";
			}
		}
		elsif (substr( ${a_line}, 0, 11) eq "#submission")
		{
			# skip enabled submission settings
			$skip_comment = 1;
		}
		elsif ( $skip_comment == 1 )
		{
			if ( (substr( ${a_line}, 0, 2) eq "# ") || (substr( ${a_line}, 0, 2) eq "#	") ) {
				# keep skipping until we hit non-whitespace line
			} else {
				$skip_comment = 0;
				print MASTER_CF_OUT "${a_line}";
				print MASTER_CF_OUT "\n";
			}
		}
		elsif (substr($a_line, 0, 7) eq "dovecot")
		{
			# update deliver to dovecot-lda
			$update_deliver = 1;
			print MASTER_CF_OUT "${a_line}";
			print MASTER_CF_OUT "\n";
		}
		elsif ( $update_deliver == 1 )
		{
			# skip comments
			my $line = $a_line;
			$line =~ s/^\s+//;
			if (index($line, "#") == 0) {
				print MASTER_CF_OUT "$a_line\n";
				next;
			}

			# remove -n and/or -s options
			$line = $a_line;
			$line =~ s/-[ns] //g;
			$line =~ s/[ \t]-[ns]//g;

			# skip valid settins that are not the deliver path
			if ((index($line, " ") == 0 || index($line, "\t") == 0) && (index($line, "/dovecot/deliver") == -1)) {
				print MASTER_CF_OUT "$line\n";
				next;
			}

			# this is the line we care about
			if ((index($line, "/dovecot/deliver")) > 0) {
				# change deliver to dovecot-lda
				$line =~ s/deliver/dovecot-lda/;

				print MASTER_CF_OUT "$line\n";
				next;
			}

			print MASTER_CF_OUT "$line\n";
			$update_deliver = 0;
		}
		elsif ( (index(${a_line}, "o fallback_relay=")) > 0 )
		{
			# change fallback_relay= to o smtp_fallback_relay=
			my $line = $a_line;
			$line =~ s/o fallback_relay=/o smtp_fallback_relay=/;

			print MASTER_CF_OUT "$line\n";
		}
		elsif ( !("${a_line}" eq "") )
		{
			print MASTER_CF_OUT "${a_line}";
			print MASTER_CF_OUT "\n";
		}
	}

	# add dovecot 
	if ( $has_dovecot == 0 )
	{
		print MASTER_CF_OUT "#" . "\n";
		print MASTER_CF_OUT "# Dovecot" . "\n";
		print MASTER_CF_OUT "#" . "\n";
		print MASTER_CF_OUT "dovecot   unix  -       n       n       -       25      pipe" . "\n";
		print MASTER_CF_OUT "  flags=DRhu user=_dovecot:mail argv=/usr/libexec/dovecot/dovecot-lda -d \${user}" . "\n";
	}

	# add greylist policy
	if ( $has_dovecot == 0 )
	{
		print MASTER_CF_OUT "#" . "\n";
		print MASTER_CF_OUT "# Greylist policy server" . "\n";
		print MASTER_CF_OUT "#" . "\n";
		print MASTER_CF_OUT "policy    unix  -       n       n       -       -       spawn" . "\n";
		print MASTER_CF_OUT "  user=nobody:mail argv=/usr/bin/perl /usr/libexec/postfix/greylist.pl" . "\n";
	}

	close( MASTER_CF );
	close( MASTER_CF_OUT );

	unlink("${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf");
	move( "${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf.out", "${g_target_root}" . ${CONFIG_ROOT} . "/postfix/master.cf");

} # update_master_cf


#################################################################
# migrate_cyrus_data ()

sub migrate_cyrus_data ()
{
	log_funct( "migrate_cyrus_data" );

	if ( do_cyrus_config_check( "configuration" ) ) {
		return;
	}
	do_xsan_check();
	do_cyrus_db_cleanup();
	do_cyrus_dovecot_migration();
} # migrate_cyrus_data


#################################################################
# migrate_dovecot_data ()

sub migrate_dovecot_data ()
{
	log_funct( "migrate_dovecot_data" );
	print LOG_FILE "Migrating POP/IMAP mail data\n";

	# migrate sieve scripts
	if ( ! path_exists( "${g_target_root}${DATA_ROOT}/rules" ) ) {
		qx( mkdir -p "${g_target_root}${DATA_ROOT}/rules" >> "${MIGRATION_LOG}" );
	}

	if ( path_exists( "${g_source_root}/private/var/spool/imap/dovecot/sieve-scripts" ) ) {
		print LOG_FILE "Migrating custom mail rules from: ${g_source_root}/private/var/spool/imap/dovecot/sieve-scripts\n";
		qx( rsync -av "${g_source_root}/private/var/spool/imap/dovecot/sieve-scripts/" "${g_target_root}${DATA_ROOT}/rules/" >> "${MIGRATION_LOG}" );
	} elsif ( path_exists( "${g_source_root}${DATA_ROOT}/rules" ) ) {
		print LOG_FILE "Migrating custom mail rules from: ${g_source_root}${DATA_ROOT}/rules\n";
		qx( rsync -av "${g_source_root}${DATA_ROOT}/rules/" "${g_target_root}${DATA_ROOT}/rules/" >> "${MIGRATION_LOG}" );
	}

	print LOG_FILE "Begin mail message data migration\n";

	# get default path
	open( PARTITIONS, "<${g_source_root}" . "/private/etc/dovecot/partition_map.conf" );
	while( <PARTITIONS> )
	{
		# store $_ value because subsequent operations may change it
		my( $a_line ) = $_;

		# strip the trailing newline from the line
		chomp( $a_line );

		my $offset = index($a_line, ":");
		if ( $offset != -1 )
		{
			# strip the quotes
			my $a_path = substr( $a_line, $offset + 1 );

			# is it local
			if ( ($a_path =~ m,^/var/spool/imap/dovecot/mail,) || ($a_path =~ m,^/private/var/spool/imap/dovecot/mail,)) {
				print LOG_FILE "Copying mail data from:\n";
				print LOG_FILE "       source: $a_path\n";
				print LOG_FILE "  destination: ${DATA_ROOT}/mail\n";
				print LOG_FILE "           by: ${CP} -rpfv ${g_source_root}${a_path}/* ${g_target_root}$DATA_ROOT/mail\n";

				# create default mail data store destination
				qx( mkdir -p "${g_target_root}$DATA_ROOT/mail" >> "${MIGRATION_LOG}" );

				# copy the mail
				qx( ${CP} -rpfv "${g_source_root}${a_path}/"* "${g_target_root}$DATA_ROOT/mail" >> "${MIGRATION_LOG}" );

				push( @g_clean_partitions, "${g_source_root}" . $a_path );

				# reset default path
				print LOG_FILE "Setting default mail partition to: $DATA_ROOT/mail\n";
				qx( ${SERVER_ADMIN} settings mail:imap:partition-default = "$DATA_ROOT/mail" >> ${MIGRATION_LOG} );
			} elsif ( (substr($a_path, 0, 5) eq "/var/") || (substr($a_path, 0, 5) eq "/etc/") ) {
				if ( path_exists( "${g_source_root}/private" . $a_path) ) {
					qx( mkdir -p "${g_target_root}/private${a_path}" >> "${MIGRATION_LOG}" );
					qx( ${CP} -rpfv "${g_source_root}/private${a_path}/"* "${g_target_root}/private${a_path}" >> "${MIGRATION_LOG}" );

					push( @g_clean_partitions, "${g_source_root}/private" . $a_path );
				}
			} elsif ($a_path =~ m,^/Library/Server/Mail/Data/,) {
				qx( mkdir -p "${g_target_root}${a_path}" >> "${MIGRATION_LOG}" );
				qx( ${CP} -rpfv "${g_source_root}${a_path}/"* "${g_target_root}${a_path}" >> "${MIGRATION_LOG}" );

				push( @g_clean_partitions, "${g_source_root}" . $a_path );
			}
			qx( ${CHOWN} -R _dovecot:mail "${g_target_root}${a_path}" >> "${MIGRATION_LOG}" );
		}
	}
	close( PARTITIONS );

	print LOG_FILE "Mail message data migration complete\n";

	# set ownership on default dovecot mail data store & sive scripts
	qx( ${CHOWN} -R _dovecot:mail "${g_target_root}/Library/Server/Mail/Data/mail" >> "${MIGRATION_LOG}" );
	qx( ${CHOWN} -R _dovecot:mail "${g_target_root}/Library/Server/Mail/Data/rules" >> "${MIGRATION_LOG}" );

} # migrate_dovecot_data


#################################################################
# escape_str ()
#	only a-z, A-Z, 0-9 and % allowed
#	and all other characters are hex-encoded

sub escape_str
{
	my $s = shift;
	$s =~ s/([^a-zA-Z0-9])/sprintf("%%%02x", ord($1))/eg;
	return $s;
} # escape_str


#################################################################
# scan_mail_acct ()
#	scan mail account for sub mailboxes and create indexes for each

sub scan_mail_acct
{
	my $in_dir = $_[0];
	my $clean_name = escape_str($_[1]);
	my $dst_path = $g_target_root . "/var/db/dovecot.fts.update";

	if (!opendir(MAIL_ACCT_DIR, $in_dir)) {
		print LOG_FILE "cannot open mailbox: $in_dir\n";
		return;
	}

	my @mailboxes = readdir(MAIL_ACCT_DIR);
	closedir(MAIL_ACCT_DIR);

	# create index for INBOX
	if(open(MY_FILE, ">$dst_path/" . $clean_name . ".INBOX")) {
		print MY_FILE "\n";
		close(MY_FILE);
	}

	$in_dir .= "/";

	# create index for any sub-mailbox
	my $file = "";
	foreach $file (@mailboxes) {
		my $a_path = $in_dir.$file;
		if (-d $a_path) {
			if (($file ne ".") && ($file ne "..")) { 
				if (substr($file, 0, 1) eq ".") {
					$file = substr($file, 1, length($file) -1);
					if (open(MY_FILE, ">$dst_path/$clean_name." . escape_str($file))) {
						print MY_FILE "\n";
						close(MY_FILE);
					}
				}
			}
		}
	}
} # scan_mail_acct

#################################################################
# uuidof volume mapping

sub uuidof
{
	my $volume = shift;

	my $uuid = "";
	if (defined($volume) && $volume ne "" && -e $volume) {
		my @infos = qx(/usr/sbin/diskutil info "$volume");
		for (@infos) {
			if (/\s*Volume UUID:\s*([0-9A-F]{8}(-[0-9A-F]{4}){3}-[0-9A-F]{12})/) {
				$uuid = $1;
				last;
			}
		}
	}
	return $uuid;
}

#################################################################
# create_fts_indexes ()
#	create fts indexes for all mail accounts

sub create_fts_indexes ()
{
	log_funct( "create_fts_indexes" );

	# create indexes for accounts on all partitions
	open(PARTITIONS, "<$g_target_root" . ${CONFIG_ROOT} . "/dovecot/partition_map.conf");
	while(<PARTITIONS>) {
		my($a_line) = $_;
		chomp($a_line);

		# get partition path
		my $offset = index($a_line, ":");
		if ($offset != -1) {
			# strip the quotes
			my $a_path = substr($a_line, $offset + 1);

			# get list of user mail account
			if (!opendir(MAIL_DIRS, $a_path)) {
				print LOG_FILE "cannot open: $a_path\n";
				next;
			}
			my @acct_dirs= readdir(MAIL_DIRS);
			closedir(MAIL_DIRS);

			# make the fts indexes for valid user accounts
			my $file;
			foreach $file (@acct_dirs) {
				next unless $file =~ /^[A-F0-9-]+$/;
				if(($file ne ".") && ($file ne "..")) { 
					# convert GUID to valid user ID
					my $user_id = qx(${CVT_MAIL_DATA} -u $file);
					if (substr($user_id, 0, 16) ne "No user id found") {
						chomp($user_id);
						scan_mail_acct($a_path . "/" . $file, $user_id);
					}
				}
			}
		}
	}
	close(PARTITIONS);
} # create_fts_indexes

################################################################################
# We only want to run this script if the previous system version is greater
#  than or equal to 10.6 and less than 10.8!

sub is_valid_version () 
{
	log_funct( "is_valid_version" );

	my ( $valid ) = 0;

	if ( (substr(${g_source_version}, 0, 4) >= ${MIN_VER}) && (substr(${g_source_version}, 0, 4) < ${MAX_VER}) ) {
		$valid = 1;
    	log_debug( "- valid: ${g_source_version}");

		if ( substr(${g_source_version}, 0, 4) eq "10.8" ) {
			$g_mtn_lion_src = 1;
		} elsif ( substr(${g_source_version}, 0, 4) eq "10.7" ) {
			$g_lion_src = 1;
		} elsif ( substr(${g_source_version}, 0, 4) eq "10.6" ) {
			$g_snow_leopard_src = 1;
		} elsif ( substr(${g_source_version}, 0, 4) eq "10.5" ) {
			$g_leopard_src = 1;
		}
	} else {
		printf( "- Version supplied was not valid: %s\n", $g_source_version );
	}

	return( ${valid} );
} # is_valid_version


################################################################################
# verify the language setting

sub is_valid_language () 
{
	log_funct( "is_valid_language" );

	my ( $valid ) = 0;

	if ( (${g_language} eq "en") || (${g_language} eq "fr") ||
		 (${g_language} eq "de") || (${g_language} eq "ja") ) {
		$valid = 1;
    	log_debug( "- valid: ${g_language}");
	} elsif ( ${g_language} eq "" ) {
		$valid = 1;
		$g_language = "en";
    	print LOG_FILE "No language specified.  Defaulting to: ${g_language}\n";
	}

	return( ${valid} );
} # is_valid_language


################################################################################
# parse_options()
#	Takes a list of possible options and a boolean indicating
#	whether the option has a value following, and sets up an associative array
#	%opt of the values of the options given on the command line. It removes all
#	the arguments it uses from @ARGV and returns them in @optArgs.

sub parse_options
{
    my (@optval) = @_;
    my ($opt, @opts, %valFollows, @newargs);

    while (@optval) {
		$opt = shift(@optval);
		push(@opts,$opt);
		$valFollows{$opt} = shift(@optval);
    }

    my @optArgs = ();
    my %opt = ();

	my $arg;
    arg: while (defined($arg = shift(@ARGV))) {
		foreach my $opt (@opts) {
			if ($arg eq $opt) {
				push(@optArgs, $arg);
				if ($valFollows{$opt}) {
					$opt{$opt} = shift(@ARGV);
					push(@optArgs, $opt{$opt});
				} else {
					$opt{$opt} = 1;
				}
				next arg;
			}
		}
		push(@newargs,$arg);
    }
    @ARGV = @newargs;
} # parse_options


################################################################################
# print script argument keys and values

sub print_script_args ()
{
	my %script_args = @_;
	while(my ($theKey, $theVal) = each (%script_args)) {
		print "$theKey: $theVal\n";
	}
} # print_script_args


################################################################################
# check if a path exists

sub path_exists ($)
{
	log_funct( "path_exists" );

	my $exists = 0;
	my ($in_path) = @_;

	if (-e "${in_path}") {
		$exists = 1;
    	log_debug( "- path exists: ${in_path}");
	} else {
    	log_debug( "- path does not exist: ${in_path}");
	}

	return( $exists );
} # path_exists


################################################################################
# check if a path's parent exists

sub parent_exists () 
{
    log_funct( "parent_exists" );

	my ($out_val) = 0;
	my ($in_path) = @_;

	my $parent_path = qx( /usr/bin/dirname "${in_path}" );
	chomp $parent_path;

	if ( -e "${parent_path}" )
	{
		$out_val = 1;
    	log_debug( " path exists: ${in_path}");
	} else {
		log_debug( " path does not exist: ${in_path}");
	}

	return( $out_val );
} # parent_exists


################################################################################
# Create the parent directory

sub create_parent_dir ()
{
	log_funct( "create_parent_dir" );

	my ($out_val) = 0;
	my ($in_path) = @_;

	my $parent_dir = qx(/usr/bin/dirname "${in_path}");
	chomp($parent_dir);

   	log_debug( "- parent_dir: ${parent_dir}");

	qx( /bin/mkdir -p "${parent_dir}" >> "${MIGRATION_LOG}" );

	if ( -e "${parent_dir}" ) {
		$out_val = 1; }

	return( ${out_val} );
} # create_parent_dir


################################################################################
# check if a path exists

sub do_cleanup ($)
{
	log_funct( "do_cleanup" );

	my ($in_path) = @_;

	if ( path_exists("${in_path}") ) {
		print LOG_FILE "Removing source: ${in_path}\n";
		qx( ${RM} -rf "${in_path}" 2>&1 >> ${MIGRATION_LOG} );
	}
} # do_cleanup


################################################################################
# Show usage

sub Usage()
{
	print("--purge <0 | 1>   \"1\" means remove any files from the old system after you've migrated them, \"0\" means leave them alone." . "\n");
	print("--sourceRoot <path> The path to the root of the system to migrate" . "\n");
	print("--sourceType <System | TimeMachine> Gives the type of the migration source, whether it's a runnable system or a " . "\n");
	print("                  Time Machine backup." . "\n");
	print("--sourceVersion <ver> The version number of the old system (like 10.6.7 or 10.7). Since we support migration from 10.6, 10.7," . "\n");
	print("                  and other 10.8 installs." . "\n");
	print("--targetRoot <path> The path to the root of the new system." . "\n");
	print("--language <lang> A language identifier, such as \"en.\" Long running scripts should return a description of what they're doing " . "\n");
	print("                  (\"Migrating Open Directory users\"), and possibly provide status update messages along the way. These messages " . "\n");
	print("                  need to be localized (which is not necessarily the server running the migration script). " . "\n");
	print("                  This argument will identify the Server Assistant language. As an alternative to doing localization yourselves " . "\n");
	print("                  send them in English, but in case the script will do this it will need this identifier." . "\n");
	print(" " . "\n");
	exit( 1 );
} # Usage

################################################################################
# do_migration()

sub do_migration()
{
	my %script_args = @_;
	my $enabled_state = "SERVICE_DISABLE";
	my $service_state = "STOPPED";

	##  Set the globals with the user options
	$g_purge = $script_args{"--purge"};
	$g_source_root = $script_args{"--sourceRoot"};
	$g_source_type = $script_args{"--sourceType"};
	$g_source_version = $script_args{"--sourceVersion"};
	$g_target_root = $script_args{"--targetRoot"};
	$g_language = $script_args{"--language"};

	## log migration start time
	my $start_time = localtime();
	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Begin Mail Migration: $start_time\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	##  log settings
	print LOG_FILE "purge: ${g_purge}\n";
	print LOG_FILE "sourceRoot: ${g_source_root}\n";
	print LOG_FILE "sourceType: ${g_source_type}\n";
	print LOG_FILE "sourceVersion: ${g_source_version}\n";
	print LOG_FILE "targetRoot: ${g_target_root}\n";
	print LOG_FILE "language: ${g_language}\n";

	##  verify source volume
	if ( ! path_exists( "${g_source_root}" ) ) {
		print LOG_FILE "Source for upgrade/migration: ${g_source_root} does not exist.\n";
		print( "Source for upgrade/migration: ${g_source_root} does not exist.\n" );
		Usage();
	}

	##  verify target volume
	if ( ! path_exists("${g_target_root}") ) {
		print LOG_FILE "Destination for upgrade/migration: ${g_target_root} does not exist.\n";
		print( "Destination for upgrade/migration: ${g_target_root} does not exist.\n" );
		Usage();
	}

	##  verify language setting
	if ( ! is_valid_language() ) {
		print LOG_FILE "Did not supply a valid language for the --language parameter, needs to be one of [en|fr|de|ja]\n";
		print( "Did not supply a valid language for the --language parameter, needs to be one of [en|fr|de|ja]\n" );
		Usage();
	}

	##  check version info
	if ( ! is_valid_version() ) {
		print( "Did not supply a valid version for the --sourceVersion parameter, needs to be >= ${MIN_VER} and < ${MAX_VER}\n" );
		Usage();
	}

	## Parse ServerVersion passed in.
	get_server_version( $g_source_version );

	if ( !("${g_source_root}" eq "/Library/Server/Previous") ) {
		$g_source_uuid = uuidof($g_source_root);
	}

	## set migration plist info
	if ( path_exists(${g_migration_plist}) ) {
		qx( ${RM} -f ${g_migration_plist} );
	}

	## create migration plist
	qx( ${PLIST_BUDDY} -c 'Add :purge integer ${g_purge}' ${g_migration_plist} );
	qx( ${PLIST_BUDDY} -c 'Add :sourceVersion string ${g_source_version}' ${g_migration_plist} );
	qx( ${PLIST_BUDDY} -c 'Add :sourceRoot string ${g_source_root}' ${g_migration_plist} );
	qx( ${PLIST_BUDDY} -c 'Add :targetRoot string ${g_target_root}' ${g_migration_plist} );

	qx( ${PLIST_BUDDY} -c 'Add :sourceUUID string ${g_source_uuid}' ${g_migration_plist} )
		if $g_source_uuid ne "";

	# enable migration launchd plist
	qx( ${PLIST_BUDDY} -c 'Set :Disabled bool false' ${MIGRATION_PLIST} );

	## reset local users migration flag
	qx( ${CVT_MAIL_DATA} -k reset );

	## stop postfix
	unload_postfix();

	######################
	# General settings
	######################

	# clean up deprecated keys
	if ( "${g_target_root}" eq "/" ) {
		print LOG_FILE "Updating postfix desktop config\n";

		my $cnt = 0;
		my $src = "/private/etc/postfix/main.cf";
		my @deprecated_keys = ("^virus_db_update_enabled", "^virus_db_last_update", "^smtpd_tls_common_name", "^spam_domain_name");

		# make a copy of original
		copy( "$src",  "$src.$g_source_version.orig" );

		for ( $cnt = 0; $cnt < 3; $cnt++) {
			my $value = qx( ${GREP} $deprecated_keys[$cnt] $src );
			if ( !($value eq "") ) {
				my $dst = "/private/etc/postfix/main.cf.um.tmp";
				qx( /usr/bin/sed -e "/$deprecated_keys[$cnt]/d" "$src" > "$dst" );
				move( "$dst", "$src" );
			}
		}

		# clean legacy smtp_fallback
		$src = "/private/etc/postfix/master.cf";
		my $value = qx( ${GREP} "o fallback_relay=" $src );
		if ( !($value eq "") ) {
			# fix existing master.cf
			my $dst = "/private/etc/postfix/master.cf.um.tmp";
			qx( /usr/bin/sed -e "s/o fallback_relay=/o smtp_fallback_relay=/" "$src" > "$dst" );
			move( "$src",  "$src" . ".$g_source_version" . ".orig" );
			move( "$dst", "$src" );
		}

		# make sure local postfix is up to date
		qx( ${POSTFIX} upgrade-configuration >> "${MIGRATION_LOG}" );
		qx( ${POSTFIX} check >> "${MIGRATION_LOG}" );
		qx( ${POSTFIX} set-permissions >> "${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );

		print LOG_FILE "Updating postfix desktop config complete\n";
	}

	# if not migrating to root volume
	if ( "${g_target_root}" ne "/" ) {
		# need to create and link Mail dir to receive default setup info
		if ( path_exists( "${MAIL_ROOT}" ) ) {
			move( "${MAIL_ROOT}",  "${MAIL_ROOT}.um.tmp" );
		}

		# create dest mail root if it doesn't exist
		if ( !path_exists( "${g_target_root}${MAIL_ROOT}" ) ) {
			mkdir( "${g_target_root}${MAIL_ROOT}", 0755 );
		}
		qx( /bin/ln -s "${g_target_root}${MAIL_ROOT}" "${MAIL_ROOT}" );
	}

	## setup config files
	qx( "/Applications/Server.app/Contents/ServerRoot/System/Library/ServerSetup/PromotionExtras/60-setup_mail_service.sh" >> "${MIGRATION_LOG}" );
	qx( "/Applications/Server.app/Contents/ServerRoot/System/Library/ServerSetup/PromotionExtras/61-setup_amavisd.sh" >> "${MIGRATION_LOG}" );
	qx( "/Applications/Server.app/Contents/ServerRoot/System/Library/ServerSetup/PromotionExtras/62-setup_spamassassin.sh" >> "${MIGRATION_LOG}" );
	qx( "/Applications/Server.app/Contents/ServerRoot/System/Library/ServerSetup/PromotionExtras/63-setup_clamav.sh" >> "${MIGRATION_LOG}" );
	qx( "/Applications/Server.app/Contents/ServerRoot/System/Library/ServerSetup/PromotionExtras/64-setup_postfix.sh" >> "${MIGRATION_LOG}" );
	qx( "/Applications/Server.app/Contents/ServerRoot/System/Library/ServerSetup/PromotionExtras/65-setup_dovecot.sh" >> "${MIGRATION_LOG}" );

	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Migrate Mail Service Settings\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	# migrate general settings plist
	print LOG_FILE "Migrating cached mail service settings file\n";
	if ( path_exists("${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist") ) {
		# save current copy
		print LOG_FILE "Saving current cached settings file: ${g_target_root}${CONFIG_ROOT}/MailServicesOther.${TARGET_VER}.plist\n";
		copy("${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist", "${g_target_root}${CONFIG_ROOT}/MailServicesOther.${TARGET_VER}.plist" );
	}

	# migrate MailServicesOther.plist
	if ( path_exists("${g_source_root}/private/etc/MailServicesOther.plist") ) {
		# copy existing other settings plist
		copy("${g_source_root}/private/etc/MailServicesOther.plist", "${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist" );
	} else {
		print LOG_FILE "Warning: Missing cached settings file: ${g_source_root}/private/etc/MailServicesOther.plist.  Using defaults\n";

		# copy default MailServicesOther.plist from Source Root if needed
		if ( !path_exists("${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist") ) {
			if ( path_exists("${SERVER_ROOT}/private/etc/MailServicesOther.plist.default") ) {
				copy("${SERVER_ROOT}/private/etc/MailServicesOther.plist.default", "${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist" );
			}
		}
	}

	# get current service state
	$service_state = qx( ${PLIST_BUDDY} -c 'Print :service_state' "${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist" );
	chomp( $service_state );
	print LOG_FILE "Previous mail services state: $service_state\n";

	# set service enabled state
	$enabled_state = qx( ${PLIST_BUDDY} -c 'Print :state' "${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist" );
	chomp( $enabled_state );
	print LOG_FILE "Previous mail service enabled state: $enabled_state\n";
	qx( ${PLIST_BUDDY} -c 'Add :serviceState string $enabled_state' $g_migration_plist );

	######################
	# amavisd
	######################

	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Migrate Junkmail & Virus Service Settings and Data\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	if ( "${g_target_root}" eq "/" ) {
		# if boot vol, save spam & virus settings to re-enable later
		open( AMAVIS_CONF, "<${g_source_root}" . "/private/etc/amavisd.conf" );
		while( <AMAVIS_CONF> ) {
			# store $_ value because subsequent operations may change it
			my( $a_line ) = $_;

			# strip the trailing newline from the line
			chomp($a_line);

			# looking for: #@bypass_spam_checks_maps  = (1);
			my $a_key = index( ${a_line}, "\@bypass_spam_checks_maps" );
			my $a_val = index( ${a_line}, "=" );
			if ( ($a_key != -1) && ($a_val != -1) ) {
				if ( substr( ${a_line}, 0, 1) eq "#" ) {
					print LOG_FILE "Junk mail scanning enabled in: ${g_source_root} \n";
					$g_enable_spam = 1;
				}
			}

			# looking for: #@bypass_virus_checks_maps = (1);
			$a_key = index( ${a_line}, "\@bypass_virus_checks_maps" );
			$a_val = index( ${a_line}, "=" );
			if ( ($a_key != -1) && ($a_val != -1) ) {
				if ( substr( ${a_line}, 0, 1) eq "#" ) {
					print LOG_FILE "Virus scanning enabled in: ${g_source_root} \n";
					$g_enable_virus = 1;
				}
			}

			# get previous hit level
			$a_key = index( ${a_line}, "\$sa_tag2_level_deflt" );
			$a_val = index( ${a_line}, "=" );
			if ( ($a_key != -1) && ($a_val != -1) ) {
				$a_line =~ s/^.*\=//;
				$a_line =~ s/;.*//;
				$a_line =~ s/^\s+//; #remove leading spaces
				$a_line =~ s/\s+$//; #remove trailing spaces
				$g_required_hits = $a_line;
			}

			# get previous spam subject tag
			$a_key = index( ${a_line}, "\$sa_spam_subject_tag" );
			$a_val = index( ${a_line}, "=" );
			if ( ($a_key != -1) && ($a_val != -1) ) {
				$g_spam_subj_tag = $a_line;
				$g_spam_subj_tag =~ s/^.*\=//;
				$g_spam_subj_tag =~ s/;.*//;
				$g_spam_subj_tag =~ s/^\s+//; #remove leading spaces
				$g_spam_subj_tag =~ s/\s+$//; #remove trailing spaces
				$g_spam_subj_tag =~ s/^\'//; #remove leading '
				$g_spam_subj_tag =~ s/\'//; #remove trailing '
			}
		}
		close(AMAVIS_CONF);
	} else {
		my $src_file = "${g_source_root}/private/etc/MailServicesOther.plist";
		my $src_dict = NSDictionary->dictionaryWithContentsOfFile_( ${src_file} );
		if ( $src_dict && $$src_dict ) {
			my $postfix_dict = $src_dict->objectForKey_( "postfix" );
			if ( $postfix_dict && $$postfix_dict) {
				if ( $postfix_dict->isKindOfClass_( NSDictionary->class ) ) {
					$g_enable_spam = obj_value( $postfix_dict->objectForKey_( "spam_enabled") );
				}
			}
		}
	}

	# change home directory for amavisd user
	qx(	${DSCL} localhost change /Local/Default/Users/_amavisd NFSHomeDirectory /var/virusmails /Library/Server/Mail/Data/scanner/amavis );

	# get learn junk mail users
	my $learn_jm_sh = "$g_source_root/private/etc/mail/spamassassin/learn_junk_mail.sh";
	if ( path_exists( $learn_jm_sh ) ) {
		my $jm_user = qx( grep "^JUNK_MAIL_USER" $learn_jm_sh );
		chomp( $jm_user );
		if ( $jm_user ne "" ) {
			$jm_user =~ s/^.*\=//;
			$jm_user =~ s/^\s+//; #remove leading spaces
			$jm_user =~ s/\s+$//; #remove trailing spaces
		} else {
			$jm_user = "junkmail";
		}

		print LOG_FILE "Setting junk mail user account to: $jm_user\n";
		qx( ${PLIST_BUDDY} -c 'Add junk_mail:junk_mail_userid string $jm_user' "${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist" );

		my $not_jm_user = qx( grep "^NOT_JUNK_MAIL_USER" $learn_jm_sh );
		chomp( $not_jm_user );
		if ( $not_jm_user ne "" ) {
			$not_jm_user =~ s/^.*\=//;
			$not_jm_user =~ s/^\s+//; #remove leading spaces
			$not_jm_user =~ s/\s+$//; #remove trailing spaces
		} else {
			$not_jm_user = "notjunkmail";
		}

		print LOG_FILE "Setting not junk mail user account to: $not_jm_user\n";
		qx( ${PLIST_BUDDY} -c 'Add junk_mail:not_junk_mail_userid string notjunkmail' "${g_target_root}${CONFIG_ROOT}/MailServicesOther.plist" );
	}

	# migrate Spam Assassin config files
	my $sa_local_cf = "$g_source_root/private/etc/mail/spamassassin/local.cf";
	if ( path_exists( $sa_local_cf ) ) {
		copy($sa_local_cf, "${g_target_root}${CONFIG_ROOT}/spamassassin/local.cf" );
		qx( ${SERVER_ADMIN} settings mail:postfix:bayes_path = "${DATA_ROOT}/scanner/amavis/.spamassassin/bayes" >> ${MIGRATION_LOG} );
	}

	# set hit level & tag
	qx( ${SERVER_ADMIN} settings mail:postfix:required_hits = "$g_required_hits" >> ${MIGRATION_LOG} );
	qx( ${SERVER_ADMIN} settings mail:postfix:spam_subject_tag = "$g_spam_subj_tag" >> ${MIGRATION_LOG} );

	# removed amavisd from launchd overrides.plist
	qx( ${PLIST_BUDDY} -c "delete :org.amavis.amavisd" /var/db/launchd.db/com.apple.launchd/overrides.plist );
	qx( ${PLIST_BUDDY} -c "delete :org.amavis.amavisd_cleanup" /var/db/launchd.db/com.apple.launchd/overrides.plist );

	######################
	# clamav
	######################

	# migrate database upgrade check times
	migrate_db_update_times();

	######################
	# postfix
	######################

	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Migrate SMTP Service Settings and Data\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	# migrate postfix config
	migrate_postfix_config();

	# migrate postfix spool data
	migrate_postfix_spool();

	# upgrade master.cf settings
	update_master_cf();

	# set keys in main.cf
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e mail_owner=_postfix >> "${MIGRATION_LOG}" );
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e setgid_group=_postdrop >> "${MIGRATION_LOG}" );
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e mailbox_transport=dovecot >> "${MIGRATION_LOG}" );

	# enable IPv4 & IPv6
	my @inet_protocols = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" inet_protocols );
	chomp(@inet_protocols);
	# check for custom settings and only change if current settings are IPv4 or misconfigured
	if ( (@inet_protocols > 0) && (($inet_protocols[0] eq "ipv4") || ($inet_protocols[0] eq "")) ) {
		qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e inet_protocols=all >> "${MIGRATION_LOG}" );
	}

	my $virt_mb = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" virtual_mailbox_domains );
	chomp($virt_mb);
	my $virt_trans = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" virtual_transport );
	chomp($virt_trans);
	if ( (index($virt_mb, "hash:/Library/Server/Mail/Config/postfix/virtual_domains") != -1) && (index($virt_trans, "lmtp:unix:") != -1) )
	{
		qx( ${SERVER_ADMIN} settings mail:postfix:enable_virtual_domains = yes >> ${MIGRATION_LOG} );
		qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e virtual_transport=virtual >> "${MIGRATION_LOG}" );
		qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e virtual_mailbox_domains=$virt_mb >> "${MIGRATION_LOG}" );
	} else {
		qx( ${SERVER_ADMIN} settings mail:postfix:enable_virtual_domains = no >> ${MIGRATION_LOG} );
	}

	# check for system_user_maps
	if ( !path_exists( "${g_target_root}${POSTFIX_CONFIG}/system_user_maps" ) ) {
		open( SYS_USER_MAPS, "> ${g_target_root}${POSTFIX_CONFIG}/system_user_maps" );
		print SYS_USER_MAPS ${g_header_string};
		close SYS_USER_MAPS;
	}
	if ( !path_exists( "${g_target_root}${POSTFIX_CONFIG}/system_user_maps.db" ) ) {
		qx( ${POSTMAP} "${g_target_root}${POSTFIX_CONFIG}/system_user_maps" );
	}

	# update using postfix built-in commands
	qx( ${POSTFIX} -c "${g_target_root}${POSTFIX_CONFIG}" upgrade-configuration >> "${MIGRATION_LOG}" );
	qx( ${POSTFIX} -c "${g_target_root}${POSTFIX_CONFIG}" check >> "${MIGRATION_LOG}" );
	qx( ${POSTFIX} -c "${g_target_root}${POSTFIX_CONFIG}" set-permissions >> "${MIGRATION_LOG}" 2>>"${MIGRATION_LOG}" );

	######################
	# config settings
	######################

	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Migrate POP/IMAP Service Settings and Data\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	if ( $g_lion_src || $g_mtn_lion_src ) {
		# copy dovecot config settings
		copy_dovecot_config();

		# update config paths
		update_dovecot_config_paths();

		# migrate dovecot mail data
		migrate_dovecot_data();

		# creat fts indexes
		create_fts_indexes();
	}
	elsif ( $g_snow_leopard_src )
	{
		# migrate dovecot config settings
		migrate_dovecot_config();

		# update config paths
		update_dovecot_config_paths();

		# migrate dovecot mail data
		migrate_dovecot_data();

		# creat fts indexes
		create_fts_indexes();

		# setup gssapi hostname if enabled
		my $imap_gssapi_auth =  qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_gssapi );
		chomp($imap_gssapi_auth);

		my $pop3_gssapi_auth =  qx( ${SERVER_ADMIN} settings mail:imap:pop_auth_gssapi );
		chomp($pop3_gssapi_auth);
		if ( ($pop3_gssapi_auth eq "mail:imap:pop_auth_gssapi = yes") || ($imap_gssapi_auth eq "mail:imap:imap_auth_gssapi = yes")) {
			qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_gssapi = yes );
		}
	} else {
		# if dest vol != boot vol, then do not migrate config settings
		if ( "${g_target_root}" eq "/" ) {
			# move cyrus config settings to dovecot
			migrate_cyrus_config();
		} else {
			&print_message( "Warning:", "Configuration migration is only supported for boot volume.", "No configuration data was migrated." );
		}

		# migrate cyrus mail data
		migrate_cyrus_data();

		# setup gssapi hostname if enabled
		my $imap_gssapi_auth =  qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_gssapi );
		chomp($imap_gssapi_auth);

		my $pop3_gssapi_auth =  qx( ${SERVER_ADMIN} settings mail:imap:pop_auth_gssapi );
		chomp($pop3_gssapi_auth);
		if ( ($pop3_gssapi_auth eq "mail:imap:pop_auth_gssapi = yes") || ($imap_gssapi_auth eq "mail:imap:imap_auth_gssapi = yes")) {
			qx( ${SERVER_ADMIN} settings mail:imap:imap_auth_gssapi = yes );
		}
	}

	# set notification service
	if ( path_exists( "$g_source_root/private/etc/dovecot/dovecot.conf" ) ) {
		my $aps_topic = qx( grep "^aps_topic " "$g_source_root/private/etc/dovecot/dovecot.conf" );
		chomp( $aps_topic );
		print LOG_FILE "Mail notification services aps topic: $aps_topic\n";
		if ( $aps_topic ne "" ) {
			qx( ${SERVER_ADMIN} settings mail:imap:notification_server_enabled = yes >> ${MIGRATION_LOG} );
		} else {
			qx( ${SERVER_ADMIN} settings mail:imap:notification_server_enabled = no >> ${MIGRATION_LOG} );
		}
	} else {
		print LOG_FILE "Mail notification services not migrated.  Missing: $g_source_root/private/etc/dovecot/dovecot.conf\n";
	}

	# remove mailman cron job
	print LOG_FILE "mailman cron cleanup...\n";
	qx( /usr/bin/crontab -u mailman -l >> ${MIGRATION_LOG} );
	qx( /usr/bin/crontab -u mailman /dev/null >> ${MIGRATION_LOG} );
	print LOG_FILE "mailman cron cleanup complete\n";

	############################
	# migrate log level settings
	############################

	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Migrate SSL Certificate Settings\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	set_ssl_certs();

	# setup migration launchd plist
	qx( ${PLIST_BUDDY} -c 'Add :Disabled bool true' ${MIGRATION_PLIST} );
	qx( ${PLIST_BUDDY} -c 'Add :RunAtLoad bool true' ${MIGRATION_PLIST} );
	qx( ${PLIST_BUDDY} -c 'Add :Label string com.apple.mail_migration' ${MIGRATION_PLIST} );
	qx( ${PLIST_BUDDY} -c 'Add :Program string /Applications/Server.app/Contents/ServerRoot/usr/libexec/dovecot/mail_data_migrator.pl' ${MIGRATION_PLIST} );
	qx( ${PLIST_BUDDY} -c 'Add :ProgramArguments string /Applications/Server.app/Contents/ServerRoot/usr/libexec/dovecot/mail_data_migrator.pl' ${MIGRATION_PLIST} );

	############################
	# migrate log level settings
	############################

	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Migrate Log Level\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	# migrate log level settings if later than 10.8
	#	otherwise set to default "info" setting
	set_log_defaults();

	######################
	# enable settings
	######################

	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Enabling Service Settings\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	# set junk mail filtering if enabled
	if ( $g_enable_spam == 1 ) {
		qx( ${SERVER_ADMIN} settings mail:postfix:smtp_uce_controlls = 1 >> ${MIGRATION_LOG} );
		qx( ${SERVER_ADMIN} settings mail:postfix:spam_scan_enabled = yes >> ${MIGRATION_LOG} );
	}

	# set virus scanning if enabled
	if ( $g_enable_virus == 1 ) {
		qx( ${SERVER_ADMIN} settings mail:postfix:virus_scan_enabled = yes >> ${MIGRATION_LOG} );
	}

	# set SMTP enabled
	qx( ${SERVER_ADMIN} settings mail:postfix:enable_smtp = yes >> ${MIGRATION_LOG} );

	# set auto-auth
	qx( ${SERVER_ADMIN} settings mail:global:auto_auth = yes >> ${MIGRATION_LOG} );

	# set postfix owner & group
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e mail_owner=_postfix >> ${MIGRATION_LOG} );
	qx( ${POSTCONF} -c "${g_target_root}${POSTFIX_CONFIG}" -e setgid_group=_postdrop >> ${MIGRATION_LOG} );

	# reset host & domain names for all mail services
	my $host_name = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" myhostname );
	chomp($host_name);
	qx( ${SERVER_ADMIN} settings mail:postfix:myhostname = $host_name >> ${MIGRATION_LOG} );

	my $domain_name = qx( ${POSTCONF} -h -c "${g_target_root}${POSTFIX_CONFIG}" mydomain );
	chomp($domain_name);
	qx( ${SERVER_ADMIN} settings mail:postfix:mydomain = $domain_name >> ${MIGRATION_LOG} );

	######################
	# clean up
	######################

	if ( ($g_purge == 1) && !("${g_target_root}" eq "${g_source_root}") ) {
		print LOG_FILE "-------------------------------------------------------------\n";
		print LOG_FILE "Cleaning Up ...\n";
		print LOG_FILE "-------------------------------------------------------------\n";

		do_cleanup( "${g_source_root}/private/etc/imapd.conf" );
		do_cleanup( "${g_source_root}/private/etc/cyrus.conf" );
		do_cleanup( "${g_source_root}/private/var/spool/postfix" );
		do_cleanup( "${g_source_root}/private/etc/postfix" );
		do_cleanup( "${g_source_root}/private/var/amavis" );
		do_cleanup( "${g_source_root}/private/etc/MailServicesOther.plist" );
		do_cleanup( "${g_source_root}/private/etc/amavisd.conf" );
	}

	######################
	# set service state
	######################

	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Setting Mail Service State\n";
	print LOG_FILE "-------------------------------------------------------------\n";

	# wstart services on boot vol migrations only
	if ( "${g_target_root}" eq "/" ) {
		# start mail service after migration is complete
		if ( ($service_state eq "RUNNING") || ($service_state eq "STARTING") ) {
			print LOG_FILE "Starting mail services\n";
			qx( ${SERVER_ADMIN} start mail >> ${MIGRATION_LOG} );
		} else {
			# reload postfix in passive mode
			load_postfix();
		}
	} else {
		# if not migrating to root volume, clean up link
		if ( path_exists( ${MAIL_ROOT} ) && (-l ${MAIL_ROOT}) ) {
			unlink( ${MAIL_ROOT} )
		}

		if ( path_exists( "${MAIL_ROOT}.um.tmp" ) ) {
			move( "${MAIL_ROOT}.um.tmp",  "${MAIL_ROOT}" );
		}

		qx( "${SERVER_ROOT}/System/Library/ServerSetup/.UninstallExtras/64-reset_postfix.sh" >> "${MIGRATION_LOG}" );
	}

	my $end_time = localtime();
	print LOG_FILE "-------------------------------------------------------------\n";
	print LOG_FILE "Mail Migration Complete: $end_time\n";
	print LOG_FILE "-------------------------------------------------------------\n";
} # do_migration
