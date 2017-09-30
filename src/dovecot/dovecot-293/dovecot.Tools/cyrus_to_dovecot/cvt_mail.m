/*
 * Contains: Cyrus to Dovecot maildir mail migration
 * Written by: Michale Dasenbrock
 *
 * Copyright (c) 2010-2013 Apple Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without  
 * modification, are permitted provided that the following conditions  
 * are met:
 * 
 * 1.  Redistributions of source code must retain the above copyright  
 * notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above  
 * copyright notice, this list of conditions and the following  
 * disclaimer in the documentation and/or other materials provided  
 * with the distribution.
 * 3.  Neither the name of Apple Inc. ("Apple") nor the names of its  
 * contributors may be used to endorse or promote products derived  
 * from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS  
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND  
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF  
 * SUCH DAMAGE.
 *
 * To Do:
 *	- Gather stats for messages migrated
 *  - Use syslog to log to dovecot info & error files
 *  - May want to do auto-migration of messages found in mailboxes without cyrus index
 *
 * Not For Open Source
 */

#import	<stdio.h>
#import	<stdlib.h>
#import	<string.h>
#import	<assert.h>
#import	<dirent.h>
#import	<unistd.h>
#import	<sys/param.h>
#import	<sys/stat.h>
#import	<sys/time.h>
#import	<arpa/inet.h>
#import	<sys/syslog.h>
#import	<getopt.h>

#import <Foundation/NSObject.h>
#import	<Foundation/Foundation.h>
#import	<Foundation/NSString.h>
#import	<ServerFoundation/XSTask.h>
#import	<OpenDirectory/NSOpenDirectory.h>
#import	<OpenDirectory/OpenDirectoryPriv.h>
#import	<DirectoryService/DirServicesTypes.h>
#import	<DirectoryService/DirServicesConst.h>

#import	"cvt_mail.h"

#define	VERSION	"2.1"

// ------------------------------------------------------------------

long		g_count			= 0;
long long	g_size			= 0LL;
FILE		*g_maildirsize	= NULL;

NSString *g_user_id = nil;
NSString *g_spool_dir = nil;
NSString *g_src_db_dir = @"/var/imap";
NSString *g_migrate_opt = @"copy";

NSAutoreleasePool	*gPool = nil;

// globals
NSString *g_config_file = IMAP_CONF_FILE;
NSString *g_imap_bins_dir = IMAP_BINS_DIR;

// user opts
int	g_user_opts = kNO_FLAGS;

// tunable output level. maps to syslog levels
int g_verbose = LOG_ERR;

// -----------------------------------------------------------------------------
//	print_string ()

void print_string ( int in_level, const char *in_format, ... )
{
	// log level check
	if ( in_level <= g_verbose ) {
		va_list args;
		va_start( args, in_format );
		vfprintf( stdout, in_format, args );
		va_end( args );
	}
} // print_string

// ------------------------------------------------------------------
//	set_opts_usage ()

void set_opts_usage ( int in_exit_code )
{
	if ( in_exit_code )
		fprintf( stdout, "Error: Missing required argument\n");
	
	fprintf( stdout, "Usage: set_user_mail_opts [options]\n");
	fprintf( stdout, "Options:\n");
	fprintf( stdout, " -a, --alt_store list\n");
	fprintf( stdout, " -a, --alt_store list-tags\n");
	fprintf( stdout, " -a, --alt_store set-tag -t, --tag <tag> -p, --path <path>\n");
	fprintf( stdout, " -a, --alt_store reset-tag -t, --tag <tag> \n");
	fprintf( stdout, " -a, --alt_store <store tag> -u, --user_id <user id>\n");
	fprintf( stdout, " -a, --alt_store <store tag> -g, --user_guid <user guid>\n");
	fprintf( stdout, " -a, --alt_store reset -u, --user_id <user id>\n");
	fprintf( stdout, " -a, --alt_store reset -g, --user_guid <user guid>\n");

	fprintf( stdout, " -f, --auto_fwd list\n");
	fprintf( stdout, " -f, --auto_fwd <email addr> -u, --user_id <user id>\n");
	fprintf( stdout, " -f, --auto_fwd <email addr> -g, --user_guid <user guid>\n");
	fprintf( stdout, " -f, --auto_fwd reset -u, --user_id <user id>\n");
	fprintf( stdout, " -f, --auto_fwd reset -g, --user_guid <user guid>\n");

	fprintf( stdout, "\nOptions:\n");
	fprintf( stdout, " -a, --alt_store list        list all account with alternate mail store locations set\n");
	fprintf( stdout, " -a, --alt_store list-tags   list all alternate mail store locations with tags\n");
	fprintf( stdout, " -a, --alt_store <store tag> -u, --user_id <user id>\n");
	fprintf( stdout, "                             set <user id> to alternate mail store <store tag>\n");
	fprintf( stdout, " -a, --alt_store <store tag> -g, --user_guid <user guid>\n");
	fprintf( stdout, "                             set <user guid> to alternate mail store <store tag>\n");
	fprintf( stdout, " -a, --alt_store reset       -u, --user_id <user id>\n");
	fprintf( stdout, "                             reset <user id> to default mail store <store tag>\n");
	fprintf( stdout, " -a, --alt_store reset       -g, --user_guid <user guid>\n");
	fprintf( stdout, "                             reset <user guid> to default mail store <store tag>\n");

	fprintf( stdout, " -f, --auto_fwd list         list all account with email autoforwarding enabled\n");
	fprintf( stdout, " -f, --auto_fwd <email addr> -u, --user_id <user id>\n");
	fprintf( stdout, "                             set <user id> to autoforward to <email addr>\n");
	fprintf( stdout, " -f, --auto_fwd <email addr> -g, --user_guid <user guid>\n");
	fprintf( stdout, "                             set <user guid> to autoforward to <email addr>\n");
	fprintf( stdout, " -f, --auto_fwd reset        -u, --user_id <user id>\n");
	fprintf( stdout, "                             reset <user id> to no email autoforwarding\n");
	fprintf( stdout, " -f, --auto_fwd reset        -g, --user_guid <user guid>\n");
	fprintf( stdout, "                             reset <user guid> to no email autoforwarding\n");

	[gPool release];
	exit( in_exit_code );
} // set_opts_usage

// ------------------------------------------------------------------
//	mail_data_tool_usage ()

void mail_data_tool_usage ( int in_exit_code )
{
	fprintf(stdout, "Usage: mail_data_tool [options] [-p path]\n");

	fprintf(stdout, "Options:\n");
	fprintf( stdout, " -e, --rename-mailboxes   Rename mail store mailboxes from UID to GUID\n");
	fprintf( stdout, " -f, --fix-permissions    Fix mail data store mailbox permissions\n");
	fprintf( stdout, " -g, --get-guid <UID>     Get GUID from user ID\n");
	fprintf( stdout, " -r, --repair-sent-dates  Repair message sent dates\n");
	fprintf( stdout, " -m, --make-UID-map       Maps UIDs to mailbox GUIDs\n");
	fprintf( stdout, " -u, --get-user-id <GUID> Get user ID from GUID\n");
	fprintf( stdout, " -v, --verbose            Increase output level\n" );

	fprintf(stdout, "Path:\n");
	fprintf( stdout, " -p, --path               Set path when not using default\n");

	[gPool release];
	exit( in_exit_code );
} // mail_data_tool_usage

// ------------------------------------------------------------------
//	usage ()

void usage ( int in_exit_code )
{
	fprintf(stdout, "Usage: cvt_mail_data [options]\n");
	fprintf(stdout, "Options:\n");

	fprintf( stdout, " -a <user ID>   user ID of account to be migrated\n" );
	fprintf( stdout, " -b <imap bin>  path to imapd binaries (default: %s)\n", [g_imap_bins_dir UTF8String] );
	fprintf( stdout, " -c <imap conf> path to imapd.conf config file (default: %s)\n", [g_config_file UTF8String] );
	fprintf( stdout, " -d <db dir>    path to source mail database dir (default: %s)\n", SRC_MAIL_DB_DIR );
	fprintf( stdout, " -s <src dir>   path to source mail data dir (default: %s)\n", SRC_MAIL_DATA_DIR );
	fprintf( stdout, " -t <dest dir>  path to destination mail data dir (default: %s)\n", DST_MAIL_DATA_DIR );
	fprintf( stdout, " -o [copy/move] copy or move mail messages to new destination (default: copy)\n" );
	fprintf( stdout, " -v             increase output level\n" );

	[gPool release];
	exit( in_exit_code );
}

// ------------------------------------------------------------------
//	exit_with_usage ()

void exit_with_usage ( int in_exit_code, const char *in_exit_str )
{
	if ( in_exit_str )
		fprintf( stdout, "%s\n", in_exit_str );

	usage( in_exit_code );
} // exit_with_usage

// ------------------------------------------------------------------
//	exit_with_error ()

void exit_with_error ( int in_exit_code )
{
	[gPool release];
	exit( in_exit_code );
} // exit_with_error

// ------------------------------------------------------------------
//	exit_with_error_string ()

void exit_with_error_string ( int in_exit_code, const char *in_err_str )
{
	fprintf( stdout, "%s\n", in_err_str );
	[gPool release];
	exit( in_exit_code );
} // exit_with_error_string

#pragma mark -

// ------------------------------------------------------------------
//	verify_cstr_path ()
//	- deprecated

int verify_cstr_path ( const char *in_path )
{
	DIR	*dir	= NULL;
	
	if ( (dir = opendir( in_path )) ) {
		closedir( dir );
		return( 0 );
	}

	return( 1 );

} // verify_cstr_path

// ------------------------------------------------------------------
//	verify_file ()

void verify_file ( NSString *in_file_path )
{
	if ( !in_file_path || ![in_file_path length] ||
			![[NSFileManager defaultManager] fileExistsAtPath: in_file_path] ) {
		print_string(LOG_ERR, "Error: file does not exist: %s\n", [in_file_path UTF8String]);
		exit_with_error( 1 );
	}
} // verify_file

// ------------------------------------------------------------------
//	verify_path ()

void verify_path ( NSString *in_dir_path )
{
	BOOL is_dir = NO;
	if ( !in_dir_path || ![in_dir_path length] ||
			![[NSFileManager defaultManager] fileExistsAtPath: in_dir_path isDirectory: &is_dir] || !is_dir ) {
		print_string(LOG_ERR, "Error: invalid path: %s\n", [in_dir_path UTF8String]);
		exit_with_error( 1 );
	}
} // verify_path

// ------------------------------------------------------------------
//	verify_arg ()

void verify_arg ( NSString *in_arg )
{
	if ( !in_arg || ![in_arg length] )
		exit_with_error_string( 1, "Error: missing or empty required argument" );
} // verify_arg

#pragma mark -

// ------------------------------------------------------------------
// map_guid ()
//	-- deprecated: use get_guid_for_uid()

const char *map_guid ( const char *in_user )
{
	NSString *name_str = nil;

	ODNode *od_search_node = [ODNode nodeWithSession: [ODSession defaultSession] type: kODNodeTypeAuthentication error: nil];
	if ( !od_search_node )
		return( NULL );

	ODQuery *od_query = [ODQuery  queryWithNode: od_search_node
								 forRecordTypes: [NSArray arrayWithObject: @kDSStdRecordTypeUsers]
									  attribute: @kDSNAttrRecordName
									  matchType: kODMatchEqualTo
									queryValues: [NSString stringWithUTF8String: in_user]
							   returnAttributes: [NSArray arrayWithObject: @kDS1AttrGeneratedUID]
								 maximumResults: 1
										  error: nil];
	if ( od_query ) {
		NSArray *records = [od_query resultsAllowingPartial: NO error: nil];
		if ( records && [records count] ) {
			ODRecord *od_record = [records objectAtIndex: 0];
			if ( od_record ) {
				// get the real name
				NSArray *values = [od_record valuesForAttribute: @kDS1AttrGeneratedUID error: nil];
				if ( values != nil )
					name_str = [values objectAtIndex: 0];
			}
		}
	}

	if ( name_str && [name_str length] )
		return( [name_str UTF8String] );

	return( NULL );
} // map_guid

// ------------------------------------------------------------------
//	get_guid_for_uid ()

NSString *get_guid_for_uid ( NSString *in_uid )
{
	NSString *name_str = nil;

	ODNode *od_search_node = [ODNode nodeWithSession: [ODSession defaultSession] type: kODNodeTypeAuthentication error: nil];
	if ( !od_search_node )
		return( nil );

	ODQuery *od_query = [ODQuery  queryWithNode: od_search_node
								 forRecordTypes: [NSArray arrayWithObject: @kDSStdRecordTypeUsers]
									  attribute: @kDSNAttrRecordName
									  matchType: kODMatchEqualTo
									queryValues: in_uid
							   returnAttributes: [NSArray arrayWithObject: @kDS1AttrGeneratedUID]
								 maximumResults: 1
										  error: nil];
	if ( od_query ) {
		NSArray *records = [od_query resultsAllowingPartial: NO error: nil];
		if ( records && [records count] ) {
			ODRecord *od_record = [records objectAtIndex: 0];
			if ( od_record ) {
				// get the real name
				NSArray *values = [od_record valuesForAttribute: @kDS1AttrGeneratedUID error: nil];
				if ( values != nil )
					name_str = [values objectAtIndex: 0];
			}
		}
	}

	return( name_str );
} // get_guid_for_uid

// ------------------------------------------------------------------
// map_userid ()
//	- deprecated: use get_guid_for_uid ()

const char *map_userid ( const char *in_guid )
{
	NSString *name_str = nil;

	ODNode *od_search_node = [ODNode nodeWithSession: [ODSession defaultSession] type: kODNodeTypeAuthentication error: nil];
	if ( !od_search_node )
		return( NULL );

	ODQuery *od_query = [ODQuery  queryWithNode: od_search_node
								 forRecordTypes: [NSArray arrayWithObject: @kDSStdRecordTypeUsers]
									  attribute: @kDS1AttrGeneratedUID
									  matchType: kODMatchEqualTo
									queryValues: [NSString stringWithUTF8String: in_guid]
							   returnAttributes: [NSArray arrayWithObject: @kDSNAttrRecordName]
								 maximumResults: 1
										  error: nil];
	if ( od_query != nil ) {
		NSArray *od_records	= [od_query resultsAllowingPartial: NO error: nil];
		if ( (od_records != nil) && [od_records count] ) {
			ODRecord *od_record = [od_records objectAtIndex: 0];
			if ( od_record ) {
				// get the real name
				NSArray *values = [od_record valuesForAttribute: @kDSNAttrRecordName error: nil];
				if ( values )
					name_str = [values objectAtIndex: 0];
			}
		}
	}

	if ( name_str && [name_str length] )
		return( [name_str UTF8String] );

	return( NULL );
} // map_userid

// ------------------------------------------------------------------
// get_uid_for_guid ()

NSString *get_uid_for_guid ( NSString *in_guid )
{
	NSString *name_str = nil;

	ODNode *od_search_node = [ODNode nodeWithSession: [ODSession defaultSession] type: kODNodeTypeAuthentication error: nil];
	if ( !od_search_node )
		return( nil );

	ODQuery *od_query = [ODQuery  queryWithNode: od_search_node
								 forRecordTypes: [NSArray arrayWithObject: @kDSStdRecordTypeUsers]
									  attribute: @kDS1AttrGeneratedUID
									  matchType: kODMatchEqualTo
									queryValues: in_guid
							   returnAttributes: [NSArray arrayWithObject: @kDSNAttrRecordName]
								 maximumResults: 1
										  error: nil];
	if ( od_query ) {
		NSArray *od_records	= [od_query resultsAllowingPartial: NO error: nil];
		if ( od_records && [od_records count] ) {
			ODRecord *od_record = [od_records objectAtIndex: 0];
			if ( od_record ) {
				// get the real name
				NSArray *values = [od_record valuesForAttribute: @kDSNAttrRecordName error: nil];
				if ( values )
					name_str = [values objectAtIndex: 0];
			}
		}
	}

	return( name_str );
} // get_uid_for_guid

#pragma mark -

// ------------------------------------------------------------------
//	map_mailbox_to_guid ()

void map_mailbox_to_guid ( NSString *in_path, NSString *in_uid )
{
	NSString *guid_str = get_guid_for_uid( in_uid );
	if ( !guid_str ) {
		print_string( LOG_ERR, "No GUID found for: %s\n", [in_uid UTF8String] );
		return;
	}

	NSError *ns_err = nil;
	NSFileManager *file_mgr	= [NSFileManager defaultManager];
	NSString *dst_path = [in_path stringByAppendingPathComponent: guid_str];
	NSString *src_path = [in_path stringByAppendingPathComponent: in_uid];
	if ( ![file_mgr moveItemAtPath: src_path toPath: dst_path error: &ns_err] )
		print_string( LOG_ERR, "Unable to rename path: %s to: %s\n",  [src_path UTF8String], [dst_path UTF8String] );
	else
		print_string( LOG_INFO, "Mapping mailbox: %s to: %s\n",  [src_path UTF8String], [dst_path UTF8String] );
} // map_mailbox_to_guid

// ------------------------------------------------------------------
//	make_guid_uid_map ()

void make_guid_uid_map ( NSString *in_path )
{
	int i = 0;
	NSError *ns_err = nil;
	NSFileManager *file_mgr = [NSFileManager defaultManager];
	NSDictionary  *dir_attrs = [NSDictionary dictionaryWithObjectsAndKeys :
										@"_dovecot", NSFileOwnerAccountName,
										@"mail", NSFileGroupOwnerAccountName,
										[NSNumber numberWithUnsignedLong: 0700], NSFilePosixPermissions, nil];

	// make dest map dir
	NSString *map_path = [in_path stringByAppendingPathComponent: @"users"];
	BOOL is_dir = NO;
	if ( ![file_mgr fileExistsAtPath: map_path isDirectory: &is_dir] || !is_dir ) {
		if ( ![file_mgr createDirectoryAtPath: map_path withIntermediateDirectories: NO attributes: dir_attrs error: &ns_err] && ns_err ) {
			print_string( LOG_ERR, "Error: cannot create directory %s (%s)\n", [map_path UTF8String], [[ns_err localizedDescription]UTF8String] );
			return;
		}
	}

	// read GUID mailbox names
	NSArray *mailboxes = [file_mgr contentsOfDirectoryAtPath: in_path error: nil];
	int count = [mailboxes count];

	for ( i = 0; i < count; i++ ) {
		// full mailbox path to rename
		NSString *guid = [mailboxes objectAtIndex: i];
		NSString *uid = get_uid_for_guid( guid );

		// skip map directory
		if ( [guid isEqualToString: @"users"] )
			continue;

		if ( uid ) {
			NSString *src_path = [NSString stringWithFormat: @"../%@", guid];
			NSString *dst_path = [map_path stringByAppendingPathComponent: uid];
			if ( ![file_mgr createSymbolicLinkAtPath: dst_path withDestinationPath: src_path error: &ns_err] && ns_err )
				print_string( LOG_WARNING, "Error: cannot create map directory: %s (%s)\n", [dst_path UTF8String], [[ns_err localizedDescription]UTF8String] );
		} else
			print_string( LOG_WARNING, "Error: cannot create map directory: %s/<user-id> (No UID found for GUID: %s)\n", [map_path UTF8String], [guid UTF8String] );
	}

} // make_guid_uid_map

// ------------------------------------------------------------------
//	rename_mailboxes ()

void rename_mailboxes ( NSString *in_path )
{
	int i;
	NSFileManager *file_mgr = [NSFileManager defaultManager];
	NSArray *mailboxes = [file_mgr contentsOfDirectoryAtPath: in_path error: nil];
	int count = [mailboxes count];

	for ( i = 0; i < count; i++ ) {
		// full mailbox path to rename
		NSString *mb_path = [in_path stringByAppendingPathComponent: [mailboxes objectAtIndex: i]];

		// directories only
		BOOL is_dir = NO;
		if ( [file_mgr fileExistsAtPath: mb_path isDirectory: &is_dir] && is_dir )
			map_mailbox_to_guid( in_path, [mailboxes objectAtIndex: i] );
	}
} // rename_mailboxes

#pragma mark -

// -----------------------------------------------------------------
//	set_attributes

void set_attributes ( NSString *in_path, NSString *in_owner, NSString *in_group, int in_perms )
{
	NSDictionary *nsDict_attrs = [NSDictionary dictionaryWithObjectsAndKeys :
									in_owner, NSFileOwnerAccountName,
									in_group, NSFileGroupOwnerAccountName,
									[NSNumber numberWithUnsignedLong: in_perms], NSFilePosixPermissions, nil];

	[[NSFileManager defaultManager] setAttributes: nsDict_attrs ofItemAtPath: in_path error: nil];
} // set_attributes

#pragma mark -

// ------------------------------------------------------------------
// fts_escape ()

static void fts_escape ( char *out_str, const char *in_orig )
{
	char *p = out_str;
	static const char *hexchars = "0123456789abcdef";

	while ( *in_orig != '\0' ) {
		unsigned char c = *in_orig;
		if ((c >= 'A' && c <= 'Z') ||
		    (c >= 'a' && c <= 'z') ||
		    (c >= '0' && c <= '9'))
			*p++ = c;
		else {
			*p++ = '%';
			*p++ = hexchars[(c >> 4) & 0xf];
			*p++ = hexchars[c & 0xf];
		}
		++in_orig;
	}
} // fts_escape

// ------------------------------------------------------------------
// set_fts_update_index_file ()

static void set_fts_update_index_file ( const char * in_src_path, const char *in_dst_path )
{
	static char path[PATH_MAX];
	static char account[PATH_MAX];
	static char mailbox[PATH_MAX];

	// get the user ID from the source path
	NSString *ns_str = [NSString stringWithCString: in_src_path encoding: NSUTF8StringEncoding];
	NSArray *ns_arry = [ns_str componentsSeparatedByCharactersInSet: [NSCharacterSet characterSetWithCharactersInString: @"/"]];

	memset( account, 0, PATH_MAX );
	fts_escape( account, [[ns_arry objectAtIndex: 0]UTF8String] );

	// get the '.' delimited mailbox name
	ns_str = [NSString stringWithCString: in_dst_path encoding: NSUTF8StringEncoding];
	ns_str = [ns_str stringByTrimmingCharactersInSet: [NSCharacterSet characterSetWithCharactersInString: @"/"]];
	ns_arry = [ns_str componentsSeparatedByCharactersInSet: [NSCharacterSet characterSetWithCharactersInString: @"/"]];

	memset( mailbox, 0, PATH_MAX );
	if ( [ns_arry count] <= 1 )
		strlcpy( mailbox, "INBOX", PATH_MAX );
	else
		fts_escape( mailbox, [[[ns_arry objectAtIndex: 1] stringByTrimmingCharactersInSet: [NSCharacterSet characterSetWithCharactersInString: @"."]] UTF8String] );

	snprintf( path, PATH_MAX, "/var/db/dovecot.fts.update/%s.%s", account, mailbox );

	print_string( LOG_DEBUG, "creating fts update index file: %s\n", path );

	int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600);
	if ( (fd < 0) && (errno != EEXIST) )
		print_string( LOG_ERR, "fts: open(%s, O_CREAT) failed: %m\n", path );
} // set_fts_update_index_file

#pragma mark -

// ------------------------------------------------------------------
//	mailbox_make_uniqueid ()
//
//	Function from cyrus mailbox.c
//

void mailbox_make_uniqueid ( char *name, unsigned long in_uidvalidity, char *uniqueid )
{
	u_int32_t hash = 0;

	while (*name) {
		hash *= 251;
		hash += *name++;
		hash %= PRIME;
	}
	sprintf( uniqueid, "%08lx%08lx", (unsigned long)hash, in_uidvalidity );
} // mailbox_make_uniqueid

#pragma mark -

// ------------------------------------------------------------------
//	read_seen_file ()

int read_seen_file ( NSString *in_seen_file, struct s_seen_data **out_seen_data )
{
	NSFileManager *file_mgr = [NSFileManager defaultManager];

	// sanity check
	if ( !in_seen_file || ![file_mgr fileExistsAtPath: in_seen_file] )
		return( -1 );

	// make out data blob
	*out_seen_data = malloc( sizeof(struct s_seen_data) );
	if ( !*out_seen_data ) {
		print_string( LOG_ERR, "%s: memory allocation error\n",  __FUNCTION__ );
		return( -1 );
	}
	// set to default
	(*out_seen_data)->seen_count = -1;
	(*out_seen_data)->uid_flag = -1;

	// read seen file
	NSString *seen_data = [NSString stringWithContentsOfFile: in_seen_file encoding: NSUTF8StringEncoding error: nil];

	NSScanner *data_scnr = [NSScanner scannerWithString: seen_data];
	[data_scnr setCaseSensitive: YES];
	[data_scnr setCharactersToBeSkipped: nil];

	// scan seen file
	int i = 0;
	NSCharacterSet	*cs_EOL = [NSCharacterSet characterSetWithCharactersInString: @"\r\n"];
	while ( ![data_scnr isAtEnd] ) {
		NSString *line_str = nil;
		[data_scnr scanUpToCharactersFromSet: cs_EOL intoString: &line_str];
		[data_scnr scanCharactersFromSet: cs_EOL intoString: nil];

		NSArray *tokens = [line_str componentsSeparatedByCharactersInSet: [NSCharacterSet characterSetWithCharactersInString: @"\t"]];

		// if we have valid tokens
		if ( [tokens count] ) {
			// save mailbox uid tag
			(*out_seen_data)->seen_array[i].mbox_uid = cpy_str( [[tokens objectAtIndex: 0]UTF8String] );
			(*out_seen_data)->seen_array[i].mbox_uid_str = [[[tokens objectAtIndex: 0]copy]retain];

			// tokenize other bits
			tokens = [[tokens objectAtIndex: 1] componentsSeparatedByCharactersInSet: [NSCharacterSet characterSetWithCharactersInString: @" \t"]];

			// we only want the seen uids
			if( [tokens count] == 5 ) {
				(*out_seen_data)->seen_array[i].seen_uids = cpy_str( [[tokens objectAtIndex: 4]UTF8String] );
				(*out_seen_data)->seen_array[i].seen_uids_str = [[[tokens objectAtIndex: 4]copy]retain];
			} else {
				(*out_seen_data)->seen_array[i].seen_uids = cpy_str( "" );
				(*out_seen_data)->seen_array[i].seen_uids_str = nil;
			}
			i++;
		}
	}

	(*out_seen_data)->seen_count = i;

	return( 0 );
} // read_seen_file

// ------------------------------------------------------------------
//	parse_seen_data ()

int parse_seen_data ( const char *in_mailbox,
					  const unsigned long in_uidvalidity,
					  struct s_seen_data *in_seen_data )
{
	int		index		= 0;
	char   *left_str	= NULL;
	char   *right_str	= NULL;
	char   *tuple_str	= NULL;
	char   *token_str	= NULL;

	if ( !in_seen_data ) {
		print_string( LOG_DEBUG, "No seen data found for %s\n", in_mailbox );
		return( -1 );
	}

	char mbox[PATH_MAX + 1];
	snprintf( mbox, PATH_MAX, "user.%s", in_mailbox );

	// Make mailbox uid
	char uniqueid[17];
	mailbox_make_uniqueid( mbox, in_uidvalidity, uniqueid );

	// Find the seen line for this mailbox
	int i;
	in_seen_data->uid_flag = -1;
	for ( i = 0; i < in_seen_data->seen_count; i++ ) {
		if ( strcmp( uniqueid, in_seen_data->seen_array[ i ].mbox_uid ) == 0 )
			in_seen_data->uid_flag = i;
	}

	if ( (in_seen_data->uid_flag) >= 0 && (in_seen_data->seen_count != -1) ) {
		if ( (tuple_str = cpy_str( in_seen_data->seen_array[ in_seen_data->uid_flag ].seen_uids )) ) {
			// Scan across seen string parsing out seen uid's
			//	ie. 3:4,8:9,11:12,15:16,20  or 1,3,5,7,9,11

			// Comma separated token string
			token_str = strtok( tuple_str, "," );

			i = 0;
			while ( token_str ) {
				// Bail if we have exceeded max range
				assert( i < CYRUS_SEENMAX );
				
				char *str = cpy_str( token_str );

				// Look for a ':' separator
				char *p = strchr( str, ':' );
				index = -1;
				if ( p != NULL )
					index = p - str;

				// If ':' was not found
				if ( index == -1 ) {
					left_str = cpy_str( str );
					right_str = cpy_str( str );
				} else {
					// We found the ':', get uid min/max values
					if ( index >= strlen( str ) )
						left_str = cpy_str( str );
					else if ( index <= 0 )
						left_str = NULL;
					else {
						left_str = malloc( index + 1 );
						if ( left_str != NULL ) {
							memset( left_str, 0, (size_t)index + 1 );
							memcpy( left_str, str, (size_t)index );
						}
					}

					if ( index >= strlen( str ) )
						right_str = NULL;
					else if ( index < 0 )
						right_str = cpy_str( str );
					else {
						size_t str_len = ( strlen( str ) - index - 1 );
						right_str = malloc( str_len + 1 );
						if ( right_str != NULL )
						{
							memset( right_str, 0, str_len + 1 );
							memcpy( right_str, str +(index + 1), str_len );

						}
					}
				}

				in_seen_data->uid_array[ i ].uid_min = atol( left_str );
				in_seen_data->uid_array[ i ].uid_max = atol( right_str );
				i++;

				// Free the strings
				if ( str ) {
					free( str );
					str = NULL;
				}

				if ( left_str ) {
					free( left_str );
					left_str = NULL;
				}

				if ( right_str ) {
					free( right_str );
					right_str = NULL;
				}

				// next token
				token_str = strtok( NULL, "," );
			}

			free( tuple_str );
			in_seen_data->uid_count = i;
		}
		else
			in_seen_data->uid_count = -1;
	}

	return( 0 );
} // parse_seen_data

// ------------------------------------------------------------------
//	is_seen ()

int is_seen ( unsigned long in_uid, struct s_seen_data *in_seen_data )
{
	if ( !in_seen_data || (in_seen_data->uid_flag == -1) )
		return( 0 );

	int i;
	for ( i = 0; i < in_seen_data->uid_count; i++ ) {
		if ( (in_uid >= in_seen_data->uid_array[i].uid_min) &&
				(in_uid <= in_seen_data->uid_array[i].uid_max) )
			return( 1 );
	}

	return( 0 );
} // is_seen

// ------------------------------------------------------------------
//	free_seen_data ()

void free_seen_data ( struct s_seen_data *in_seen_data )
{
	if ( !in_seen_data )
		return;

	int i;
	for ( i = 0; i < in_seen_data->seen_count; i++ ) {
		if ( in_seen_data->seen_array[i].mbox_uid )
			free( in_seen_data->seen_array[i].mbox_uid );
		if ( in_seen_data->seen_array[i].mbox_uid_str )
			[in_seen_data->seen_array[i].mbox_uid_str release];

		if ( in_seen_data->seen_array[i].seen_uids )
			free( in_seen_data->seen_array[i].seen_uids );
		if ( in_seen_data->seen_array[i].seen_uids_str )
			[in_seen_data->seen_array[i].seen_uids_str release];
	}
	free( in_seen_data );
} // free_seen_data

// ------------------------------------------------------------------
//	set_quota ()

int set_quota ( NSString *in_dst_root, char *in_dir )
{
	FILE		*src_file	= NULL;
	long long	max_quota	= 0;
	char		*p_line		= NULL;
	char		dst_path[ PATH_MAX + 1 ];

	// Get source path from base path + first letter of account name + account name
	//	ie. /var/imap/quota/j/user.joe
	NSString *src_path = [NSString stringWithFormat: @"%@/quota/%@/user.%@", g_src_db_dir, [g_user_id substringToIndex: 1], g_user_id];

	// Create destination path from target directory + directory name
	snprintf( dst_path, PATH_MAX, "%s/%s/maildirsize", [in_dst_root UTF8String], in_dir );

	// Open soruce file
	src_file = fopen( [src_path UTF8String], "r" );
	if ( !src_file ) {
		print_string( LOG_WARNING, "Warning: Could not open quota file: %s\n", src_path );
		return( -1 );
	}

	g_maildirsize = fopen( dst_path, "w" );
	if ( !g_maildirsize ) {
		print_string( LOG_WARNING, "Warning: Could not open quota file: %s\n", dst_path );
		fclose( src_file );
		return( -1 );
	}

	// read "quota used"
	//	- this value is not used, will calculate from actual messages migrated
	p_line = read_line( src_file );

	// Read "max quota"
	p_line = read_line( src_file );
	if ( p_line != NULL ) {
		// convert K to bytes
		if ( atol( p_line ) != 2147483647 )
			max_quota = atoll( p_line ) * 1024LL;
		free( p_line );
		p_line = NULL;
	}

	// set max quota in .../maildirsize file
	print_string( LOG_INFO, "Setting mail quota to: %lld (bytes)\n", max_quota );
	fprintf( g_maildirsize, "%lldS\n", max_quota );

	fclose( src_file );

	return( 0 );

} // set_quota

// ------------------------------------------------------------------
//	set_subscribe ()

void set_subscribe ( NSString *in_dst_path )
{
	// mutable string for new subscribed data
	NSMutableString *new_sub_data = [[[NSMutableString alloc] init]autorelease];

	// get source path from base path + first letter of account name + account name
	//	ie. /var/imap/user/j/joe.sub
	NSString *src_path = [NSString stringWithFormat: @"%@/user/%@/%@.sub", g_src_db_dir, [g_user_id substringToIndex: 1], g_user_id];

	// read .sub file
	NSString *sub_data = [NSString stringWithContentsOfFile: src_path encoding: NSUTF8StringEncoding error: nil];

	// parse sub file for subscribed mailboxes
	NSString *user_tag = [NSString stringWithFormat: @"user.%@.", g_user_id];

	NSScanner *sub_data_scnr = [NSScanner scannerWithString: sub_data];
	[sub_data_scnr setCaseSensitive: YES];
	[sub_data_scnr setCharactersToBeSkipped: nil];

	// Get first Received: header
	NSCharacterSet	*cs_WS = [NSCharacterSet whitespaceCharacterSet];
	NSCharacterSet	*cs_EOL = [NSCharacterSet characterSetWithCharactersInString: @"\r\n"];
	while ( ![sub_data_scnr isAtEnd] ) {
		NSString *line_str = nil;
		[sub_data_scnr scanUpToCharactersFromSet: cs_EOL intoString: &line_str];
		[sub_data_scnr scanCharactersFromSet: cs_EOL intoString: nil];

		if ( [line_str hasPrefix: user_tag] ) {
			line_str = [line_str substringFromIndex: [user_tag length]];
			line_str = [line_str stringByTrimmingCharactersInSet: cs_WS];
			[new_sub_data appendString: [NSString stringWithFormat: @"%@\n", line_str]];
		}
	}

	// subscriptions dest file path
	NSString *dst_path = [in_dst_path stringByAppendingPathComponent: @"subscriptions"];
	[new_sub_data writeToFile: dst_path atomically: YES encoding: NSUTF8StringEncoding error: nil];
	set_attributes ( dst_path, @"_dovecot", @"mail", 0600 );
} // set_subscribe

// ------------------------------------------------------------------
// create_maildir_dirs ()

int create_maildir_dirs ( NSString *in_path )
{
	NSError *ns_err = nil;
	NSFileManager *file_mgr = [NSFileManager defaultManager];
	NSDictionary  *dir_attrs = [NSDictionary dictionaryWithObjectsAndKeys :
										@"_dovecot", NSFileOwnerAccountName,
										@"mail", NSFileGroupOwnerAccountName,
										[NSNumber numberWithUnsignedLong: 0700], NSFilePosixPermissions, nil];

	int i = 0;
	NSString *a_path = in_path;
	NSArray *an_array = [NSArray arrayWithObjects: @"/", @"cur", @"new", @"tmp", nil];
	int cnt = [an_array count];
	for ( i = 0; i < cnt; i++ ) {
		a_path = [in_path stringByAppendingPathComponent: [an_array objectAtIndex: i]];
		if ( ![file_mgr fileExistsAtPath: a_path] ) {
			if ( ![file_mgr createDirectoryAtPath: a_path withIntermediateDirectories: NO attributes: dir_attrs error: &ns_err] && ns_err ) {
				print_string( LOG_ERR, "Error: cannot create directory %s (%s)\n", [a_path UTF8String], [[ns_err localizedDescription]UTF8String] );
				return( -1 );
			}
		}
	}
		
	return( 0 );
} // create_maildir_dirs

// ------------------------------------------------------------------
//	set_dovecot_keywords ()

int set_dovecot_keywords ( const char *in_cy_header_path, char *in_dest_path )
{
	unsigned int	i		= 0;
	char		   *keyword			= NULL;
	FILE		   *keyword_file	= NULL;
	FILE		   *hdr_file		= NULL;
	char			dst_file_path [ PATH_MAX + 1 ];
	char			line_buf[ BUF_SIZE ];

	// Now let's write us some dovecot keywords
	hdr_file = fopen( in_cy_header_path, "r" );
	if ( hdr_file != NULL ) {
		sprintf( dst_file_path, "%s/%s", in_dest_path, "dovecot-keywords" );

		// Skip past header tag
		fseek( hdr_file, sizeof( MAILBOX_HEADER_MAGIC ) - 1, SEEK_SET );

		// Get second line past header tag
		if ( (fgets( line_buf, sizeof( line_buf ), hdr_file ) != NULL) &&
			 (fgets( line_buf, sizeof( line_buf ), hdr_file ) != NULL) &&
			 (*line_buf != '\0') )
		{
			if ( line_buf[ strlen( line_buf ) - 1 ] == '\n' )
				line_buf[ strlen( line_buf ) -1 ] = '\0';

			// Tokenize this line to get keywords
			keyword = strtok( line_buf, " ");
			if ( keyword != NULL ) {
				keyword_file = fopen( dst_file_path, "w" );
				if ( keyword_file == NULL ) {
					print_string( LOG_ERR, "Error: could not open: %s\n",  dst_file_path );
					return( 1 );
				}

				// Write the keywords to the dovecot file
				do {
					if ( *keyword != '\0' )
						fprintf( keyword_file, "%u %s\n", i, keyword );

					i++;
				} while ( (keyword = strtok( NULL, " " ) ) != NULL );

				fclose( keyword_file );
			}
		}
		fclose( hdr_file );
	}

	return( 0 );
} // set_dovecot_keywords

// ------------------------------------------------------------------
//	migrate_message ()

int migrate_message ( char *in_src, unsigned long in_date, char *in_dst, char *in_flags, unsigned long *out_size )
{
	NSError *ns_err	= nil;
	NSFileManager	*nsFileMgr	= [NSFileManager defaultManager];

	// get file size
	struct stat file_stat;
	stat( in_src, &file_stat );
	*out_size = file_stat.st_size;

	// Destination path name
	NSString *dst_path = [NSString stringWithFormat: @"%s%llu%s", in_dst, file_stat.st_size, in_flags ];
	if ( [g_migrate_opt isEqualToString: @"copy"] ) {
		print_string( LOG_INFO, "copying: %s to: %s\n", in_src, [dst_path UTF8String] );

		if ( ![nsFileMgr copyItemAtPath: [NSString stringWithUTF8String: in_src] toPath: dst_path error: &ns_err ] )
			print_string( LOG_ERR, "Error: Failed to move: %s (%s)\n", in_src, [[ns_err localizedFailureReason] UTF8String] );
		else
			set_attributes( dst_path, @"_dovecot", @"mail", 0600 );
	} else {
		print_string( LOG_INFO, "moving: %s  to: %s\n", in_src, [dst_path UTF8String] );

		if ( [nsFileMgr moveItemAtPath: [NSString stringWithUTF8String: in_src] toPath: dst_path error: &ns_err ] == NO )
			print_string( LOG_ERR, "Error: Failed to move: %s (%s)\n", in_src, [[ns_err localizedFailureReason] UTF8String] );
		else
			set_attributes( dst_path, @"_dovecot", @"mail", 0600 );
	}

	// fix file mod times, this is the imap "received time"
	struct timeval time_val[2];
	time_val[0].tv_sec = file_stat.st_atime;
	time_val[1].tv_sec = in_date;
	utimes( [dst_path UTF8String], time_val );
	
	return( 0 );
} // migrate_message

// ------------------------------------------------------------------
// migrate_mail ()

int migrate_mail ( char *in_src_path, char *in_dest_path, int is_root, struct s_seen_data *in_seen_data )
{
	char	src_mb_path   [ PATH_MAX + 1 ];
	char	cy_index_path [ PATH_MAX + 1 ];
	char	cy_header_path[ PATH_MAX + 1 ];
	char	src_msg_path  [ PATH_MAX + 1 ];
	char	msg_flags_str [ PATH_MAX + 1 ];

	print_string( LOG_DEBUG, "Migrating mailbox: %s  to: %s\n",  in_src_path, in_dest_path );

	// Get cyrus mailbox index file path
	snprintf( cy_index_path, PATH_MAX, "%s/cyrus.index", in_src_path );

	// Get cyrus mailbox header file path
	snprintf( cy_header_path, PATH_MAX, "%s/cyrus.header", in_src_path );

	// read cyrus index version
	FILE *hdr_file = NULL;
	if ( (hdr_file = fopen( cy_index_path, "r")) ) {
		unsigned long	uidvalidity = 0;

		// get the version number for the cyrus minor_version in header
		struct index_header cy_header;
		memset( &cy_header, 0, sizeof( cy_header ) );
		fread( &cy_header, 3*4, 1, hdr_file );

		unsigned long ver = ntohl( cy_header.minor_version );
		fseek( hdr_file, 0, SEEK_SET );

		print_string( LOG_DEBUG, "cyrus index version: %lu\n", ver );

		// check for the only two we care about
		switch ( ver ) {
			case 2:
				fread( &cy_header, 44, 1, hdr_file );
				uidvalidity = (unsigned long)ntohl(cy_header.pop3_last_login);
				break;
			case 3:
				fread( &cy_header, 56, 1, hdr_file );
				uidvalidity = (unsigned long)ntohl(cy_header.pop3_last_login);
				break;
			case 4:
				fread( &cy_header, 76, 1, hdr_file );
				uidvalidity = (unsigned long)ntohl(cy_header.pop3_last_login);
				break;
			case 6:
				fread( &cy_header, 80, 1, hdr_file );
				uidvalidity = (unsigned long)ntohl(cy_header.uidvalidity);
				break;
			case 9:
				fread( &cy_header, 96, 1, hdr_file );
				uidvalidity = (unsigned long)ntohl(cy_header.uidvalidity);
				break;
			default:
				exit_with_error_string( 1, [[NSString stringWithFormat:
												@"Unsupported cyrus mailbox header version: %lu\n", ver]UTF8String] );
		}

		char dst_file_path[PATH_MAX + 1];
		sprintf( dst_file_path, "%s/dovecot-uidlist", in_dest_path );
		FILE *uid_file = fopen( dst_file_path, "w" );
		if ( !uid_file )
			exit_with_error_string( 1, [[NSString stringWithFormat: @"Error: could not open: %s\n", dst_file_path]UTF8String] );

		// Copy over uid data from cyrus to maildir mailbox
		fprintf( uid_file, "1 %lu %lu\n", uidvalidity, (unsigned long)ntohl(cy_header.last_uid) + 1 );

		// Parse cyrus seen db file
		strlcpy( src_mb_path, in_src_path, PATH_MAX );

		// Just get the relative mailbox path
		unsigned int len = [g_spool_dir length];
		char *mb_path = src_mb_path + len;

		// Convert any /'s to .'s in the mailbox path name
		for ( len = 0; len < strlen( mb_path ); len++ ) {
			if ( mb_path[ len ]=='/' )
				mb_path[ len ]='.';
		}
		mb_path++;

		// do seen file data parsing
		parse_seen_data( mb_path, uidvalidity, in_seen_data );

		struct index_entry cy_index_entry;
		memset( &cy_index_entry, 0, sizeof( cy_index_entry ) );
		fseek( hdr_file, htonl(cy_header.start_offset), SEEK_SET );

		long total_msgs = 0;
		long long total_bytes = 0LL;

		// Scan through the cyrus header file and migrate a mail file for each entry
		while( fread( &cy_index_entry, ntohl( cy_header.record_size ), 1, hdr_file) ) {
			// Make target file path
			sprintf( dst_file_path, "%s/cur/%lu.cyrus.%lu,S=",
									 in_dest_path,
									(unsigned long)ntohl( cy_index_entry.internaldate ),
									(unsigned long)ntohl( cy_index_entry.uid ) );

			// Set maildir flags
			strlcpy( msg_flags_str, ":2,", PATH_MAX );

			// Draft flag
			if ( (ntohl(cy_index_entry.system_flags) & FLAG_DRAFT) )
				strcat( msg_flags_str, "D" );

			// Flagged flag
			if ( (ntohl(cy_index_entry.system_flags) & FLAG_FLAGGED) )
				 strcat( msg_flags_str, "F" );

			// Draft flag
			if ( (ntohl(cy_index_entry.system_flags) & FLAG_ANSWERED) )
				 strcat( msg_flags_str, "R" );

			// Seen flag
			if ( is_seen( (unsigned long)(ntohl(cy_index_entry.uid)), in_seen_data) )
				strcat( msg_flags_str, "S" );

			// Draft flag
			if ( (ntohl(cy_index_entry.system_flags) & FLAG_DELETED) )
				 strcat( msg_flags_str, "T" );

			// Set dovecot keywords
			int i;
			char keyword[2] = {'\0', '\0'};
			for ( i = 0; i < MAX_USER_FLAGS && i <= 'z'-'a'; i++) {
				if ( (htonl( cy_index_entry.user_flags[i / 8 ]) & (1 << (i % 8))) != 0 ) {
					keyword[ 0 ] = 'a' + i;
					strcat( msg_flags_str, keyword );
				}
			}

			// Full source path
			sprintf( src_msg_path, "%s/%lu.", in_src_path, (unsigned long)ntohl(cy_index_entry.uid) );

			// Now do the actual copy/move of the cyrus mail file
			unsigned long msg_size = 0;
			migrate_message( src_msg_path, (unsigned long)ntohl(cy_index_entry.internaldate), dst_file_path, msg_flags_str, &msg_size );

			// Set uid in dovecot dovecot-uidlist
			fprintf( uid_file, "%lu %s%lu\n", (unsigned long)ntohl(cy_index_entry.uid), strrchr(dst_file_path, '/') + 1, msg_size );

			total_bytes += msg_size;
			total_msgs++;
		}

		if ( total_bytes != 0 ) {
			if ( g_maildirsize != NULL )
				fprintf( g_maildirsize, "%-13lld %-13ld\n", total_bytes, total_msgs );
		}

		print_string( LOG_INFO, "- mailbox: %s\n",  in_src_path );
		print_string( LOG_INFO, "  messages: %lu size: %llu bytes\n",  total_msgs, total_bytes );

		g_count += total_msgs;
		g_size += total_bytes;

		fclose( hdr_file );
		fclose( uid_file );

		set_attributes ( [NSString stringWithFormat: @"%s/dovecot-uidlist", in_dest_path], @"_dovecot", @"mail", 0600 );
	} else {
		// Hmmmm, no cyrus.index file here
		//	Do we even want to mess with this mailbox or just log it and move on....
		//	For now, check for mail and log that there may be un-migrated messages

		// Checking if there may be mail here
		DIR *p_dir = NULL;
		if ( (p_dir = opendir(in_src_path)) ) {
			struct dirent  *dir_entry;
			while ( (dir_entry = readdir(p_dir)) ) {
				if ( (dir_entry->d_name[strlen( dir_entry->d_name ) - 1] == '.') && (dir_entry->d_name[0] != '.') ) {
					// There may be messages here
					print_string( LOG_WARNING, "Warning: Missing cyrus index file and mailbox may cntain messages in: %s\n",  in_src_path );
					break;
				}
			}
			closedir( p_dir );
		}
	}

	// Now let's write us some dovecot keywords
	return( set_dovecot_keywords( cy_header_path, in_dest_path ) );

} // migrate_mail

// ------------------------------------------------------------------
// scan_account ()

int scan_account ( NSString *in_cy_spool_dir,
				   NSString *in_dst_root,
				   NSString *in_acct_dir,
				   struct s_seen_data *in_seen_data )
{
	DIR			   *p_dir		= NULL;
	struct dirent  *dir_entry;
	struct stat		stat_buf;

	static int is_root_mailbox = 1;
	static char src_path	[ PATH_MAX + 1 ]	= "";
	static char dst_path	[ PATH_MAX + 1 ]	= "";
	static char full_path	[ PATH_MAX + 1 ]	= "";
	static char src_mb_path	[ PATH_MAX + 1 ]	= "";

	char *cc = NULL;
	int rc = 0;
	const char *dst_acct = NULL;

	NSFileManager *file_mgr = [NSFileManager defaultManager];

	if ( is_root_mailbox ) {
		if ( verify_cstr_path( [in_acct_dir UTF8String] ) != 0 ) {
			print_string( LOG_WARNING, "Warning: unable to verify mailbox path: %s\n", [in_acct_dir UTF8String] );
			[gPool release];
			exit(1);
		}

		// check for flat seen file
		NSString *seen_file = [NSString stringWithFormat: @"%@/user/%@/%@.seen.flat", g_src_db_dir, [g_user_id substringToIndex: 1], g_user_id];
		if ( ![file_mgr fileExistsAtPath: seen_file] ) {
			// see if skiplist seen file exists
			NSString *skiplist_seen = [NSString stringWithFormat: @"%@/user/%@/%@.seen", g_src_db_dir, [g_user_id substringToIndex: 1], g_user_id];
			if ( [file_mgr fileExistsAtPath: skiplist_seen] ) {
				NSString *err_str = nil;
				NSString *result_str = nil;
				NSString *cvt_cyrusdb = [NSString stringWithFormat: @"%@/cvt_cyrusdb", g_imap_bins_dir];
				NSString *imapd_conf = [NSString stringWithFormat: @"%@/imapd.conf", g_config_file];
				NSArray *args = [NSArray arrayWithObjects: @"-C", imapd_conf, skiplist_seen, @"skiplist", seen_file, @"flat", nil];
				XSRunTask( cvt_cyrusdb, args, nil, &result_str, &err_str, nil );
				if ( err_str && [err_str length] )
					print_string(LOG_ERR, "error creating flat seen file from %s: %s", [skiplist_seen UTF8String], [err_str UTF8String] );
				if ( result_str && [result_str length] )
					print_string(LOG_INFO, "cvt_cyrusdb: %s", [result_str UTF8String] );
			}
		}

		read_seen_file( seen_file, &in_seen_data );
		if ( !in_seen_data )
			print_string( LOG_WARNING, "Warning: No seen file found for for: %s\n", [g_user_id UTF8String] );
	}

	// Drop in if it's a directory
	if ( (stat( [in_acct_dir UTF8String], &stat_buf ) == 0) && ((stat_buf.st_mode & S_IFMT) == S_IFDIR) ) {
		// Add '/' if we now have a path name
		if ( strlen( src_path ) != 0 )
			strcat( src_path, "/" );

		// Append source mailbox to path
		strcat( src_path, [in_acct_dir UTF8String] );

		// Mailboxes are separate with a '.'
		if ( strlen( dst_path ) != 0 )
			strcat( dst_path, "." );


		// Append destination mailbox name, either account ID or GUID if it exists
		NSString *guid_str = get_guid_for_uid( in_acct_dir );
		if ( is_root_mailbox && ((guid_str = get_guid_for_uid(in_acct_dir)) != nil) )
			dst_acct = [guid_str UTF8String];
		else
			dst_acct = [in_acct_dir UTF8String];

		strcat( dst_path, dst_acct );

		// Append a '/' to the base mailbox name
		//	We should only hit this once per account
		if ( !strcmp( dst_path, dst_acct) )
			strcat( dst_path, "/" );

		src_mb_path[0]='\0';
		strcat( src_mb_path, [in_cy_spool_dir UTF8String] );
		strcat( src_mb_path, "/" );
		strcat( src_mb_path, src_path );

		// Make the full path
		snprintf( full_path, PATH_MAX, "%s/%s", [in_dst_root UTF8String], dst_path );

		// create maildir directories
		create_maildir_dirs( [NSString stringWithUTF8String: full_path] );

		// Migrate cyrus quota info
		if ( is_root_mailbox )
			set_quota( in_dst_root, dst_path );

		// Set subscribed mailboxes
		if ( is_root_mailbox )
			set_subscribe( [NSString stringWithUTF8String: full_path] );

		// copy and/or migrate the mailfiles
		migrate_mail( src_mb_path, full_path, is_root_mailbox, in_seen_data );

		// create update index file for background processing
		set_fts_update_index_file( src_path, dst_path );

		if ( is_root_mailbox )
			is_root_mailbox = 0;

		// Scan mailboxes
		if ( chdir( [in_acct_dir UTF8String] ) == 0 ) {
			if ( (p_dir = opendir( "." )) ) {
				// Recursively scan across all mailboxes 
				while( (dir_entry = readdir(p_dir)) ) {
					// Skip the '.' & '..' directories
					if ( strcmp( dir_entry->d_name, "." ) &&  strcmp( dir_entry->d_name, "..") )
						scan_account( in_cy_spool_dir, in_dst_root,  [NSString stringWithUTF8String: dir_entry->d_name], in_seen_data );
				}
				closedir( p_dir );
			}

			// Backing out, terminate source mailbox path
			if ( (cc = strrchr( src_path, '/' )) )
				*cc = '\0';

			// Backing out, terminate destination mailbox path
			if ( (cc = strrchr( dst_path, '.')) )
				*cc='\0';

			chdir( ".." );
		}
	}

	return( rc );
} // scan_account

#pragma mark -

// ------------------------------------------------------------------
//	read_line ()

char *read_line ( FILE *in_file )
{
	int		c			= 0;
	int		line_len	= -1;
	int		buf_size	= 1024;
	char   *out_buf		= NULL;

	if ( in_file && !feof( in_file ) ) {
		out_buf = (char *)malloc( buf_size );
		for( ;; ) {
			c = fgetc( in_file );
			line_len++;

			if ( line_len >= buf_size ) {
				buf_size += 4096;
				print_string( LOG_DEBUG, "%s: realloc buffer: current size: %d -- new size: %d\n", __FUNCTION__, line_len, buf_size );

				out_buf = realloc( out_buf, buf_size );
				if ( out_buf == NULL )
					return( NULL );
			}
			out_buf[ line_len ] = (char)c;

			// Check for line termination or EOF
			if ( (c == '\n') || (c == '\r') || (c == '\0') || (c == EOF) ) {
				if( c == '\r' ) {
					c = fgetc( in_file );
					if ( c  !='\n' )
						ungetc(c, in_file);
				}
				// Terminate the string
				out_buf[ line_len ] = '\0';

				return( out_buf );
			}
		}
	}

	return( NULL );
} // read_line

// ------------------------------------------------------------------
//	cpy_str ()

char * cpy_str ( const char *in_str )
{
	int		len			= 0;
	char	*out_str	= NULL;

	if ( in_str != NULL ) {
		len = strlen( in_str ) + 1;
		out_str = malloc( len );
		if ( out_str == NULL ) {
			print_string( LOG_DEBUG, "%s: memory allocation error\n",  __FUNCTION__ );
		} else
			strlcpy( out_str, in_str,  len );
	}
	return( out_str );
} // cpy_str

#pragma mark -

// ------------------------------------------------------------------
// set_dovecot_dir_and_file_attributes ()

void set_dovecot_dir_and_file_attributes ( NSString *in_path )
{
	BOOL			is_dir		= NO;
	NSError			*nsError	= nil;
	NSArray			*nsArry		= nil;
	NSString		*nsStr_name	= nil;
	NSString		*nsStr_path	= nil;
	NSEnumerator	*nsEnum		= nil;
	NSFileManager	*nsFileMgr	= [NSFileManager defaultManager];
	NSDictionary	*nsDict_dir	= [NSDictionary dictionaryWithObjectsAndKeys:
									@"_dovecot", NSFileOwnerAccountName,
									@"mail", NSFileGroupOwnerAccountName,
									[NSNumber numberWithUnsignedLong:0700], NSFilePosixPermissions, nil];
	NSDictionary	*nsDict_file= [NSDictionary dictionaryWithObjectsAndKeys :
									@"_dovecot", NSFileOwnerAccountName,
									@"mail", NSFileGroupOwnerAccountName,
									[NSNumber numberWithUnsignedLong:0600], NSFilePosixPermissions, nil];

	if ( [nsFileMgr fileExistsAtPath: in_path isDirectory : &is_dir ] && is_dir ) {
		[nsFileMgr setAttributes: nsDict_dir ofItemAtPath: in_path error: &nsError];

		nsArry = [nsFileMgr contentsOfDirectoryAtPath: in_path error: &nsError];
		nsEnum = [nsArry objectEnumerator];
		while ( (nsStr_name = [nsEnum nextObject]) ) {
			nsStr_path = [in_path stringByAppendingPathComponent: nsStr_name];
			if ( [nsFileMgr fileExistsAtPath: nsStr_path isDirectory : &is_dir ] && is_dir )
				set_dovecot_dir_and_file_attributes( nsStr_path );
			else
				[nsFileMgr setAttributes: nsDict_file ofItemAtPath: nsStr_path error: &nsError];
		}
	}
	else
		[nsFileMgr setAttributes: nsDict_file ofItemAtPath: nsStr_path error: &nsError];
} // set_dovecot_dir_and_file_attributes

// ------------------------------------------------------------------
//	fix_data_store_dir_file_attributes ()

void fix_data_store_dir_file_attributes ( NSString *in_path )
{
	int i;
	NSFileManager *file_mgr = [NSFileManager defaultManager];
	NSArray *mailboxes = [file_mgr contentsOfDirectoryAtPath: in_path error: nil];
	int count = [mailboxes count];

	for ( i = 0; i < count; i++ ) {
		BOOL is_dir = NO;
		// full mailbox path to rename
		NSString *mb_path = [in_path stringByAppendingPathComponent: [mailboxes objectAtIndex: i]];

		// directories only
		if ( [file_mgr fileExistsAtPath: mb_path isDirectory: &is_dir] && is_dir )
			set_dovecot_dir_and_file_attributes( mb_path );
	}
} // fix_data_store_dir_file_attributes

// -----------------------------------------------------------------
//	write_settings

void write_settings ( NSDictionary *in_dict )
{
	[in_dict writeToFile: MAIL_USER_SETTINGS_PLIST atomically: YES];

	NSDictionary		*nsDict_attrs	= [NSDictionary dictionaryWithObjectsAndKeys :
												@"_postfix", NSFileOwnerAccountName,
												@"mail", NSFileGroupOwnerAccountName,
												[NSNumber numberWithUnsignedLong: 0660], NSFilePosixPermissions,
												nil];

	[[NSFileManager defaultManager] setAttributes: nsDict_attrs ofItemAtPath: MAIL_USER_SETTINGS_PLIST error: nil];
} // write_settings

// ------------------------------------------------------------------
//	is_index_file ()

BOOL is_index_file ( NSString *in_msg_path )
{
	// get file name from path
	NSString *file_name = [in_msg_path lastPathComponent];
	if ( !file_name )
		return( NO );

	if ( [file_name isEqualToString: @"dovecot.index"] ||
			[file_name isEqualToString: @"dovecot.index.log"] ||
				[file_name isEqualToString: @"dovecot.index.cache"] )
		return( YES );

	return( NO );
} // is_index_file

// ------------------------------------------------------------------
//	is_msg_file ()

BOOL is_msg_file ( NSString *in_msg_path )
{
	// remove message file name
	NSString *tmp_path = [in_msg_path stringByDeletingLastPathComponent];
	if ( !tmp_path )
		return( NO );

	// get where message lives: new, cur, or tmp
	NSString *container = [tmp_path lastPathComponent];
	if ( !container )
		return( NO );

	if ( [container isEqualToString: @"new"] ||
			[container isEqualToString: @"cur"] ||
				[container isEqualToString: @"tmp"] )
		return( YES );

	return( NO );
} // is_msg_file

// ------------------------------------------------------------------
//	get_date_str ()

NSString *get_date_str ( NSString *in_date )
{
	NSString *out_r = nil;
	NSCharacterSet	*cs_WS = [NSCharacterSet whitespaceCharacterSet];

	NSString *tmp_str = [in_date copy];
	while ( [tmp_str rangeOfString: @";" options: NSLiteralSearch].location != NSNotFound ) {
		NSScanner *line_scnr = [NSScanner scannerWithString: tmp_str];
		[line_scnr setCaseSensitive: YES];
		[line_scnr setCharactersToBeSkipped: nil];

		if ( [line_scnr scanUpToString: @";" intoString: nil ] ) {
			[line_scnr scanString: @";" intoString: nil ];
			[line_scnr scanCharactersFromSet: cs_WS intoString: nil];

			[line_scnr scanUpToString: @"\r\n" intoString: &out_r];
			tmp_str = [NSString stringWithString: out_r];
		}
	}
	return( out_r );
} // get_date_str

// ------------------------------------------------------------------
//	get_message_date ()

NSDate *get_message_date ( NSString *in_msg_path )
{
	BOOL hit = NO;
	BOOL done = NO;
	NSMutableString	*rcvd_header = [[[NSMutableString alloc] init]autorelease];
	NSCharacterSet	*cs_EOL = [NSCharacterSet characterSetWithCharactersInString: @"\r\n"];
	NSCharacterSet	*cs_WS = [NSCharacterSet whitespaceCharacterSet];

	NSError *ns_err = nil;
	NSString *msg_data = [NSString stringWithContentsOfFile: in_msg_path encoding: NSASCIIStringEncoding error: &ns_err];
	if ( !msg_data ) {
		if ( ns_err && ([[ns_err localizedFailureReason] length]) )
			print_string( LOG_ERR, "read failed for: %s (%s)\n", [in_msg_path UTF8String], [[ns_err localizedFailureReason] UTF8String] );
		else
			print_string( LOG_ERR, "read failed for: %s\n", [in_msg_path UTF8String] );
		return( nil );
	}

	NSScanner *msg_scnr = [NSScanner scannerWithString: msg_data];
	[msg_scnr setCaseSensitive: YES];
	[msg_scnr setCharactersToBeSkipped: nil];

	NSString *hdr_data = nil;
	[msg_scnr scanUpToString: @"\n\n" intoString: &hdr_data];
	if ( !hdr_data || ![hdr_data length] )
		return( nil );

	NSScanner *hdr_scnr = [NSScanner scannerWithString: hdr_data];
	[hdr_scnr setCaseSensitive: YES];
	[hdr_scnr setCharactersToBeSkipped: nil];

	// Get first Received: header
	while ( ![hdr_scnr isAtEnd] & !done ) {
		NSString *line_str = nil;
		[hdr_scnr scanUpToCharactersFromSet: cs_EOL intoString: &line_str];
		[hdr_scnr scanCharactersFromSet: cs_EOL intoString: nil];

		if ( [line_str hasPrefix: @"Received:"] && !hit ) {
			hit = YES;
			[rcvd_header appendString: line_str];
		} else if ( hit == YES ) {
			if ( [line_str hasPrefix: @" "] || [line_str hasPrefix: @"\t"] ) {
				NSString *tmp_str = nil;
				NSScanner *line_scnr = [NSScanner scannerWithString: line_str];
				[line_scnr setCaseSensitive: YES];
				[line_scnr setCharactersToBeSkipped: nil];

				[line_scnr scanCharactersFromSet: cs_WS intoString: nil];
				[line_scnr scanUpToString: @"EOL" intoString: &tmp_str];

				[rcvd_header appendString: @" "];
				[rcvd_header appendString: tmp_str];
			} else {
				[rcvd_header appendString: @"\r\n"];
				break;
			}
		}
	}

	NSString *date_str = nil;
	if ( [rcvd_header length] ) {
		date_str = get_date_str( rcvd_header );
	}

	// failed to get the date string because either Received: header doesn't exist
	//	or invalid/missing date string in Received: header
	//	get the date from the Date header
	if ( !date_str ) {
		NSScanner *hdr_scnr = [NSScanner scannerWithString: hdr_data];
		[hdr_scnr setCaseSensitive: YES];
		[hdr_scnr setCharactersToBeSkipped: nil];

		// Get first Received: header
		while ( ![hdr_scnr isAtEnd] && !done ) {
			NSString *line_str = nil;
			[hdr_scnr scanUpToCharactersFromSet: cs_EOL intoString: &line_str];
			[hdr_scnr scanCharactersFromSet: cs_EOL intoString: nil];

			if ( [line_str hasPrefix: @"Date:"] ) {
				NSScanner *line_scnr = [NSScanner scannerWithString: line_str];
				[line_scnr setCaseSensitive: YES];
				[line_scnr setCharactersToBeSkipped: nil];
				
				[line_scnr scanString: @"Date:" intoString: nil ];
				[line_scnr scanUpToString: @"" intoString: &date_str];
				date_str = [[date_str stringByTrimmingCharactersInSet: cs_WS]copy];

				print_string( LOG_INFO, "date str from     Date header: %s  : %s\n", [date_str UTF8String], [[in_msg_path lastPathComponent] UTF8String] );
				done = YES;
			}
		}
	} else
		print_string( LOG_DEBUG, "date str from Received header: %s  : %s\n", [date_str UTF8String], [[in_msg_path lastPathComponent] UTF8String] );

	// bail if no date string can be found
	if ( !date_str ) {
		print_string( LOG_DEBUG, "No date information in received header fournd for: %s\n", [[in_msg_path lastPathComponent] UTF8String] );
		return( nil );
	}

	NSDateFormatter *date_fmt = [[NSDateFormatter alloc] init];
	[date_fmt setFormatterBehavior:NSDateFormatterBehavior10_4];
	if ( [date_str rangeOfString: @"(" options: NSLiteralSearch].location == NSNotFound )
		[date_fmt setDateFormat: @"EEE, dd MMM yyyy HH:m:ss vvvv"];
	else if ( isdigit([date_str characterAtIndex: 1]) )
		[date_fmt setDateFormat: @"dd MMM yyyy HH:m:ss vvvv (zzz)"];
	else
		[date_fmt setDateFormat: @"EEE, dd MMM yyyy HH:m:ss vvvv (zzz)"];

	return( [date_fmt dateFromString: date_str] );
} // get_message_date

// ------------------------------------------------------------------
//	fix_message_dates ()

void fix_message_dates ( NSString *in_path )
{
	BOOL is_dir = NO;
	NSError *ns_err = nil;
	NSFileManager *nsFileMgr = [NSFileManager defaultManager];

	if ( [nsFileMgr fileExistsAtPath: in_path isDirectory: &is_dir] ) {
		if ( is_dir ) {
			NSArray *dir_ent_arry = [nsFileMgr contentsOfDirectoryAtPath: in_path error: &ns_err];
			NSEnumerator *ns_enum = [dir_ent_arry objectEnumerator];
			NSString *dir_entry	= nil;
			while ( (dir_entry = [ns_enum nextObject]) ) {
				NSString *a_path = [in_path stringByAppendingPathComponent: dir_entry];
				fix_message_dates( a_path );
			}
		} else {
			if ( is_msg_file(in_path) ) {
				NSMutableDictionary *attr_dict = [NSMutableDictionary dictionaryWithDictionary:
													[nsFileMgr attributesOfItemAtPath: in_path error: &ns_err]];
				NSDate *ns_date = [attr_dict objectForKey: @"NSFileModificationDate"];
				if ( ns_date ) {
					NSDate *ns_date_new = get_message_date( in_path );
					if ( ns_date_new != nil ) {
						[attr_dict setObject: ns_date_new forKey: @"NSFileModificationDate"];
						[nsFileMgr setAttributes: attr_dict ofItemAtPath: in_path error: &ns_err];
					}
				} 
			} else if ( is_index_file(in_path) ) {
				print_string( LOG_DEBUG, "cleaning index file: %s\n", [in_path UTF8String] );
				if ( ![nsFileMgr removeItemAtPath: in_path error: &ns_err] ) {
					if ( ns_err && ([[ns_err localizedFailureReason] length]) )
						print_string( LOG_ERR, "delete failed for: %s (%s)\n", [in_path UTF8String], [[ns_err localizedFailureReason] UTF8String] );
					else
						print_string( LOG_ERR, "delete failed for: %s\n", [in_path UTF8String] );
				}
			}
		}
	}
} // fix_message_dates

#pragma mark -

// ------------------------------------------------------------------
//	get_alt_data_stores

NSDictionary *get_alt_data_stores ( int in_print )
{
	NSError *ns_err = nil;
	NSMutableDictionary	*out_dict = [[[NSMutableDictionary alloc] init] autorelease];

	if ( in_print ) {
		printf("\n" );
		printf("alternate data store locations and tags\n" );
		printf("---------------------------------------\n" );
	}

	NSString *file_data = [NSMutableString stringWithContentsOfFile: @"/Library/Server/Mail/Config/dovecot/partition_map.conf" encoding: NSUTF8StringEncoding error: &ns_err];
	if ( file_data && [file_data length] ) {
		NSString *map_str = nil;
		file_data = [file_data stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceAndNewlineCharacterSet]];
		NSArray *alt_stores = [file_data componentsSeparatedByCharactersInSet: [NSCharacterSet newlineCharacterSet]];
		NSEnumerator *ns_enum = [alt_stores objectEnumerator];
		while ( (map_str = [ns_enum nextObject]) ) {
			NSArray *mapping = [map_str componentsSeparatedByCharactersInSet: [NSCharacterSet characterSetWithCharactersInString: @":"]];
			[out_dict setObject: [mapping objectAtIndex: 1] forKey: [mapping objectAtIndex: 0]];
			if ( in_print )
				printf("tag:%s  path:%s \n", [[mapping objectAtIndex: 0]UTF8String], [[mapping objectAtIndex: 1]UTF8String] );
		}
	}

	return( out_dict );
} // get_alt_data_stores

// ------------------------------------------------------------------

void set_alt_data_store_tag ( const char *in_tag, const char *in_path )
{
	BOOL is_set = NO;
	BOOL is_dir = NO;
	NSDictionary *alt_stores = get_alt_data_stores(0);
	NSMutableDictionary *maps = [[alt_stores mutableCopy] autorelease];
	NSMutableString *new_map = [[[NSMutableString alloc]init ]autorelease];
	NSString *path_str = [NSString stringWithCString: in_path encoding: NSUTF8StringEncoding];
	if ( [[NSFileManager defaultManager] fileExistsAtPath: path_str isDirectory: &is_dir ] && is_dir ) {
		NSString *tag_str = [NSString stringWithCString: in_tag encoding: NSUTF8StringEncoding];
		// change existing tag path value
		if ( [maps objectForKey: tag_str] ) {
			[maps setObject: path_str forKey: tag_str];
			is_set = YES;
		}

		// sanity check, make sure "default" exists, otherwise create it
		if ( ![maps objectForKey: @"default"] )
			[maps setObject: DEFAULT_DATA_LOCATION forKey: @"default"];
			
		[new_map appendString: [NSString stringWithFormat: @"default:%@\n", [maps objectForKey: @"default"]]];
		id key = 0;
		NSEnumerator *ns_enum = [[maps allKeys] objectEnumerator];
		while ( (key = [ns_enum nextObject]) ) {
			if ( ![key isEqualToString: @"default"] )
				[new_map appendString: [NSString stringWithFormat: @"%@:%@\n", key, [maps objectForKey: key]]];
		}
		if ( !is_set )
			[new_map appendString: [NSString stringWithFormat: @"%@:%@\n", tag_str, path_str]];
		set_attributes ( path_str, @"_dovecot", @"mail", 0775 );
		[new_map writeToFile: DOVECOT_PARTITION_MAPS atomically: YES encoding: NSUTF8StringEncoding error: nil];
		set_attributes ( DOVECOT_PARTITION_MAPS, @"root", @"wheel", 0644 );
		get_alt_data_stores(1);
	} else
		printf("Error: path: %s does not exist\n", in_path );
} // set_alt_data_store_tag

// ------------------------------------------------------------------

void reset_alt_data_store_tag ( const char *in_tag )
{
	NSString *tag_str = [NSString stringWithCString: in_tag encoding: NSUTF8StringEncoding];
	if ( [tag_str isEqualToString:  @"default"] ) {
		printf("Cannot reset default alternate store tag\n");
		return;
	}

	NSDictionary *maps = get_alt_data_stores(0);
	NSMutableString *new_map = [[[NSMutableString alloc]init ]autorelease];
	if ( [maps objectForKey: tag_str] ) {
		id key = 0;
		NSEnumerator *ns_enum = [[maps allKeys] objectEnumerator];
		while ( (key = [ns_enum nextObject]) ) {
			if ( ![key isEqualToString: tag_str] )
				[new_map appendString: [NSString stringWithFormat: @"%@:%@\n", key, [maps objectForKey: key]]];
		}
		[new_map writeToFile: DOVECOT_PARTITION_MAPS atomically: YES encoding: NSUTF8StringEncoding error: nil];
		set_attributes ( DOVECOT_PARTITION_MAPS, @"root", @"wheel", 0644 );
	} else
		printf("Error: tag does not exist: %s\n", in_tag );

	get_alt_data_stores(1);
} // reset_alt_data_store_tag

// ------------------------------------------------------------------

void list_alt_data_stores( const char *in_guid )
{
	id key = 0;

	printf("\n" );
	printf("local user alternate data store location settings\n" );
	printf("-------------------------------------------------\n" );

	NSDictionary *users_dict = [NSDictionary dictionaryWithContentsOfFile: MAIL_USER_SETTINGS_PLIST];
	if ( users_dict ) {
		NSDictionary *maps = get_alt_data_stores(0);
		if ( in_guid ) {
			NSString *guid = [NSString stringWithCString: in_guid encoding: NSUTF8StringEncoding];
			NSDictionary *user_dict = [users_dict objectForKey: guid];
			if ( !user_dict )
				printf("user: default:%s tag:default path:%s\n", map_userid([key UTF8String]), [[maps objectForKey: @"default"]UTF8String]);
			else {
				NSString *alt_tag = [user_dict objectForKey: @"kAltMailStoreLoc"];
				if ( [maps objectForKey: alt_tag] )
					printf("user:%s store  tag:%s  path:%s\n", map_userid(in_guid), [alt_tag UTF8String], [[maps objectForKey: alt_tag]UTF8String]);
				else
					printf("user:%s store  tag:default  path:%s\n", map_userid(in_guid), [[maps objectForKey: @"default"]UTF8String]);
			}
		} else {
			NSEnumerator *enumer = [[users_dict allKeys] objectEnumerator];
			while ( (key = [enumer nextObject]) ) {
				NSDictionary *user_dict = [users_dict objectForKey: key];
				NSString *alt_tag = [user_dict objectForKey: @"kAltMailStoreLoc"];
				if ( [maps objectForKey: alt_tag] )
					printf("user:%s store  tag:%s  path:%s\n", map_userid([key UTF8String]), [alt_tag UTF8String], [[maps objectForKey: alt_tag]UTF8String]);
				else
					printf("user:%s store  tag:default  path:%s\n", map_userid([key UTF8String]), [[maps objectForKey: @"default"]UTF8String]);
			}
		}
	}
	printf("\n" );
} // list_alt_data_stores

// ------------------------------------------------------------------

void set_alt_data_store ( const char *in_guid, const char *in_store_tag )
{
	NSMutableDictionary *users_dict = [NSMutableDictionary dictionaryWithContentsOfFile: MAIL_USER_SETTINGS_PLIST];
	if ( !users_dict )
		users_dict = [[[NSMutableDictionary alloc] init] autorelease];

	NSString *guid = [NSString stringWithCString: in_guid encoding: NSUTF8StringEncoding];
	NSString *tag = [NSString stringWithCString: in_store_tag encoding: NSUTF8StringEncoding];
	NSDictionary *maps = get_alt_data_stores(0);

	NSMutableDictionary *user_dict = [users_dict objectForKey: guid];
	if ( user_dict ) {
		if ( [maps objectForKey: tag] )
			[user_dict setObject: tag forKey: @"kAltMailStoreLoc"];
		else
			[user_dict setObject: @"default" forKey: @"kAltMailStoreLoc"];
	} else {
		if ( [maps objectForKey: tag] )
			[users_dict setObject: [NSDictionary dictionaryWithObjectsAndKeys:
									tag, @"kAltMailStoreLoc", nil] forKey: guid];
		else
			[users_dict setObject: [NSDictionary dictionaryWithObjectsAndKeys:
									@"default", @"kAltMailStoreLoc", nil] forKey: guid];
	}
	write_settings( users_dict );

	list_alt_data_stores( in_guid );
} // set_alt_data_store

// ------------------------------------------------------------------

void reset_alt_data_stores ( const char *in_guid )
{
	NSMutableDictionary *users_dict = [NSMutableDictionary dictionaryWithContentsOfFile: MAIL_USER_SETTINGS_PLIST];
	if ( !users_dict )
		users_dict = [[[NSMutableDictionary alloc] init] autorelease];

	NSString *guid = [NSString stringWithCString: in_guid encoding: NSUTF8StringEncoding];

	NSMutableDictionary *user_dict = [users_dict objectForKey: guid];
	if ( user_dict ) {
		[user_dict setObject: @"default" forKey: @"kAltMailStoreLoc"];
	} else {
		[users_dict setObject: [NSDictionary dictionaryWithObjectsAndKeys:
								@"default", @"kAltMailStoreLoc", nil] forKey: guid];
	}
	write_settings( users_dict );

	list_alt_data_stores( in_guid );
} // reset_alt_data_stores

// ------------------------------------------------------------------

void list_auto_forwards ( const char *in_guid )
{
	id key = 0;

	printf("\n" );
	printf("local user auto-forward settings\n" );
	printf("--------------------------------\n" );

	NSDictionary *users_dict = [NSDictionary dictionaryWithContentsOfFile: MAIL_USER_SETTINGS_PLIST];
	if ( users_dict ) {
		if ( in_guid ) {
			NSString *guid = [NSString stringWithCString: in_guid encoding: NSUTF8StringEncoding];
			NSDictionary *user_dict = [users_dict objectForKey: guid];
			if ( [[user_dict objectForKey: @"kMailAccountState"] isEqualToString: @"Forward"] )
				printf("user: %s: <%s>\n", map_userid(in_guid), [[user_dict objectForKey: @"kAutoForwardValue"]UTF8String]);
		} else {
			NSEnumerator *enumer = [[users_dict allKeys] objectEnumerator];
			while ( (key = [enumer nextObject]) ) {
				NSDictionary *user_dict = [users_dict objectForKey: key];
				if ( [[user_dict objectForKey: @"kMailAccountState"] isEqualToString: @"Forward"] )
					printf("user: %s: <%s>\n", map_userid([key UTF8String]), [[user_dict objectForKey: @"kAutoForwardValue"]UTF8String]);
			}
		}
	}
	printf("\n" );
} // list_auto_forwards

// ------------------------------------------------------------------

void set_auto_forward( const char *in_guid, const char *in_fwd_addr )
{
	NSMutableDictionary *users_dict = [NSMutableDictionary dictionaryWithContentsOfFile: MAIL_USER_SETTINGS_PLIST];
	if ( !users_dict )
		users_dict = [[[NSMutableDictionary alloc] init] autorelease];

	NSString *guid = [NSString stringWithCString: in_guid encoding: NSUTF8StringEncoding];
	NSString *fwd_addr = [NSString stringWithCString: in_fwd_addr encoding: NSUTF8StringEncoding];
	NSMutableDictionary *user_dict = [users_dict objectForKey: guid];
	if ( user_dict ) {
		[user_dict setObject: @"Forward" forKey: @"kMailAccountState"];
		[user_dict setObject: fwd_addr forKey: @"kAutoForwardValue"];
	} else {
		[users_dict setObject: [NSDictionary dictionaryWithObjectsAndKeys:
									@"Forward", @"kMailAccountState",
									fwd_addr, @"kAutoForwardValue", nil] forKey: guid];
	}
	write_settings( users_dict );

	list_auto_forwards( in_guid );
} //  set_auto_forward


// ------------------------------------------------------------------

void reset_auto_forward( const char *in_guid )
{
	NSMutableDictionary *users_dict = [NSMutableDictionary dictionaryWithContentsOfFile: MAIL_USER_SETTINGS_PLIST];
	if ( !users_dict )
		return;

	NSString *guid = [NSString stringWithCString: in_guid encoding: NSUTF8StringEncoding];
	NSMutableDictionary *user_dict = [users_dict objectForKey: guid];
	if ( user_dict ) {
		[user_dict setObject: @"" forKey: @"kMailAccountState"];
		[user_dict setObject: @"" forKey: @"kAutoForwardValue"];
	}
	write_settings( users_dict );

	list_auto_forwards( in_guid );
} //  reset_auto_forward

// ------------------------------------------------------------------
// set_user_mail_opts ()

int set_user_mail_opts ( int argc, char **argv )
{
	char ch;
	int opt_index = 0;
	int long_val  = 0;

	char *tag		= NULL;
	char *path		= NULL;
	char *user_id	= NULL;
	char *user_guid	= NULL;
	char *fwd_addr	= NULL;
	char *alt_store	= NULL;
	struct option long_options[] = {
		{"alt_store", 1, &long_val, 'a'},
		{"auto_fwd", 1, &long_val, 'f'},
		{"user_guid", 1, &long_val, 'g'},
		{"path", 1, &long_val, 'p'},
		{"tag", 1, &long_val, 't'},
		{"user_id", 1, &long_val, 'u'},
		{0, 0, 0, 0 }
	};

	if ( argc == 1 )
		set_opts_usage(0);

	while ((ch = getopt_long(argc, argv, "ha:f:g:p:t:u:", long_options, &opt_index)) != -1) {
		switch (ch) {
			case 'a':
				alt_store = optarg;
				break;

			case 'f':
				fwd_addr = optarg;
				break;

			case 'g':
				user_guid = optarg;
				break;

			case 'p':
				path = optarg;
				break;

			case 't':
				tag = optarg;
				break;

			case 'u':
				user_id = optarg;
				break;

			case 0:
				switch ( long_val ) {
					case 'a':
						alt_store = optarg;
						break;

					case 'f':
						fwd_addr = optarg;
						break;

					case 'g':
						user_guid = optarg;
						break;

					case 'p':
						path = optarg;
						break;

					case 't':
						tag = optarg;
						break;

					case 'u':
						user_id = optarg;
						break;
				}
				break;
			case 'h':
			default:
				set_opts_usage(0);
		}
	}

	if ( fwd_addr ) {
		const char *guid = NULL;
		if ( !strcasecmp(fwd_addr, "list") ) {
			if ( user_id )
				guid = map_guid( user_id );
			else if ( user_guid )
				guid = user_guid;

			list_auto_forwards( guid );
			return(0);
		} else if ( !strcasecmp(fwd_addr, "reset") ) {
			if ( user_id )
				guid = map_guid( user_id );
			else if ( user_guid )
				guid = user_guid;

			if ( !guid )
				set_opts_usage(1);

			reset_auto_forward( guid );
		} else if (user_id || user_guid) {
			if ( user_id )
				guid = map_guid( user_id );
			else if ( user_guid )
				guid = user_guid;

			if ( !guid )
				set_opts_usage(1);

			set_auto_forward( guid, fwd_addr );
		} else
			set_opts_usage(1);
	} else if ( alt_store ) {
		const char *guid = NULL;
		if ( !strcasecmp(alt_store, "list") ) {
			if ( user_id )
				guid = map_guid( user_id );
			else if ( user_guid )
				guid = user_guid;

			list_alt_data_stores( guid );
			return(0);
		} else if ( !strcasecmp(alt_store, "list-tags") ) {
			get_alt_data_stores(1);
		} else if ( !strcasecmp(alt_store, "set-tag") ) {
			if ( !path || !tag )
				set_opts_usage(1);

			set_alt_data_store_tag( tag, path );
			return(0);
		} else if ( !strcasecmp(alt_store, "reset-tag") ) {
			if ( !tag )
				set_opts_usage(1);

			reset_alt_data_store_tag( tag );
			return(0);
		} else if ( !strcasecmp(alt_store, "reset") ) {
			if ( user_id )
				guid = map_guid( user_id );
			else if ( user_guid )
				guid = user_guid;

			if ( !guid )
				set_opts_usage(1);

			reset_alt_data_stores( guid );
		} else if (user_id || user_guid) {
			if ( user_id )
				guid = map_guid( user_id );
			else if ( user_guid )
				guid = user_guid;

			if ( !guid )
				set_opts_usage(1);

			set_alt_data_store( guid, alt_store );
		} else
			set_opts_usage(1);
	}
	return(0);
} // set_user_mail_opts

// ------------------------------------------------------------------
// mail_data_tool ()

int mail_data_tool ( int argc, char **argv )
{
	char ch;
	int opt_index = 0;
	int long_val  = 0;

	TOOL_FLAGS flags = kNO_FLAGS;

	NSString *uid_str = nil;
	NSString *guid_str = nil;
	NSString *path_str = DEFAULT_DATA_LOCATION;

	struct option long_options[] = {
		{"fix-permissions", 1, &long_val, 'f'},
		{"get-guid", 1, &long_val, 'g'},
		{"map-mailstore-dirs", 1, &long_val, 'm'},
		{"repair-sent-dates", 1, &long_val, 'r'},
		{"path", 1, &long_val, 'p'},
		{"get-user-id", 1, &long_val, 'u'},
		{"version", 1, &long_val, 'V'},
		{"verbose", 1, &long_val, 'v'},
		{0, 0, 0, 0 }
	};

	if ( argc == 1 )
		mail_data_tool_usage(0);

	while ((ch = getopt_long(argc, argv, "efmrVvp:u:g:", long_options, &opt_index)) != -1) {
		switch (ch) {
			case 'e':
				flags |= kE_FLAG;
				break;

			case 'f':
				flags |= kF_FLAG;
				break;

			case 'p':
				path_str = [NSString stringWithUTF8String: optarg];
				break;

			case 'g':
				flags |= kG_FLAG;
				uid_str = [NSString stringWithUTF8String: optarg];
				break;

			case 'm':
				flags |= kM_FLAG;
				break;

			case 'r':
				flags |= kR_FLAG;
				break;

			case 'u':
				flags |= kU_FLAG;
				guid_str = [NSString stringWithUTF8String: optarg];
				break;

			case 'v':
				g_verbose++;
				break;

			case 0:
				switch ( long_val ) {
					case 'f':
						flags |= kF_FLAG;
						break;

					case 'p':
						path_str = [NSString stringWithUTF8String: optarg];
						break;

					case 'r':
						flags |= kR_FLAG;
						break;
				}
				break;

			default:
				mail_data_tool_usage(0);
				break;
		}
	}

	// fix permissions on mail data store files
	if ( flags & kF_FLAG ) {
		verify_arg( path_str );
		verify_path( path_str );
		fix_data_store_dir_file_attributes( path_str );
	}

	// fix message received dates
 	if ( flags & kR_FLAG ) {
		verify_arg( path_str );
		verify_path( path_str );
		fix_message_dates( path_str );
	}

	// rename mailboxes to guid's
 	if ( flags & kE_FLAG ) {
		verify_arg( path_str );
		verify_path( path_str );
		rename_mailboxes( path_str );
	}

	// rename mailboxes to guid's
 	if ( flags & kM_FLAG ) {
		verify_arg( path_str );
		verify_path( path_str );
		make_guid_uid_map( path_str );
	}

	// get user id from guid
 	if ( flags & kU_FLAG ) {
		verify_arg( guid_str );
		NSString *uid_str = get_uid_for_guid( guid_str );
		if ( uid_str )
			fprintf( stdout, "%s\n", [uid_str UTF8String] );
		else
			fprintf( stdout, "No user id found for GUID: %s\n", [guid_str UTF8String] );
	}

	// get guid from user id
 	if ( flags & kG_FLAG ) {
		verify_arg( uid_str );
		NSString *guid_str = get_guid_for_uid( uid_str );
		if ( guid_str )
			fprintf( stdout, "%s\n", [guid_str UTF8String] );
		else
			fprintf( stdout, "No GUID found for user: %s\n", [uid_str UTF8String] );
	}
	return(0);
} // mail_data_tool

// ------------------------------------------------------------------
// cvt_mail_data ()

int cvt_mail_data ( int argc, char **argv )
{
	NSString *src_data_dir = [NSString stringWithUTF8String: SRC_MAIL_DATA_DIR];
	NSString *dst_data_dir = [NSString stringWithUTF8String: DST_MAIL_DATA_DIR];

	gPool = [[NSAutoreleasePool alloc] init];

	if ( geteuid() != 0 ) {
		fprintf( stdout, "%s must be run as root\n", argv[0] );
		exit(0);
	}

	if ( argc == 1 )
		exit_with_usage( 1, NULL );

	int	ch;
	while ( (ch = getopt(argc, argv, "Vva:b:c:d:s:t:o:")) != EOF ) {
		switch( ch ) {
			case 'a': // user id
				g_user_id = [NSString stringWithUTF8String: optarg];
				break;

			case 'b': // path to cyrus imap binaries
				g_imap_bins_dir = [NSString stringWithUTF8String: optarg];
				break;

			case 'c': // path to imapd.conf
				g_config_file = [NSString stringWithUTF8String: optarg];
				break;

			case 'd': // cyrus imap database directory
				g_src_db_dir = [NSString stringWithUTF8String: optarg];
				break;

			case 's': // cyrus imap data spool directory
				src_data_dir = [NSString stringWithUTF8String: optarg];
				break;

			case 't': // destination maildir data directory
				dst_data_dir = [NSString stringWithUTF8String: optarg];
				break;

			case 'o': // migration message option
				g_migrate_opt = [NSString stringWithUTF8String: optarg];
				break;

			case 'v':
				g_verbose++;
				break;

			case '?':
				exit_with_usage( 1, NULL );
				break;

			default:
				usage( 0 );
				break;
		}
	}

	argc -= optind;
	argv += optind;

	// ------------------------------------------------------------------
	// Do arg verification

	verify_path( g_src_db_dir );	// source cyrus db path
	verify_path( src_data_dir );	// source cyrus spool path
	verify_path( dst_data_dir );	// destination mail data path
	verify_path( g_imap_bins_dir );	// path to cyrus binaries

	verify_file( [NSString stringWithFormat: @"%@/imapd.conf", g_config_file] );
	verify_file( [NSString stringWithFormat: @"%@/cvt_cyrusdb", g_imap_bins_dir] );

	verify_arg( g_user_id );		// user id
	NSString *src_mbox_path = [NSString stringWithFormat: @"%@/user/%@", src_data_dir, g_user_id];
	verify_path( src_mbox_path );	// src mail data path
	g_spool_dir = [NSString stringWithFormat: @"%@/users", src_data_dir];

	// verify migration option
	if ( !([g_migrate_opt isEqualToString: @"copy"] || [g_migrate_opt isEqualToString: @"move"]) ) {
		print_string( LOG_ERR, "Error: invalid migration option (must be copy or move)\n" );
		usage(1);
	}

	chdir( [g_spool_dir UTF8String] );
	struct s_seen_data *seen_file = NULL;
	scan_account( g_spool_dir, dst_data_dir, g_user_id, seen_file );
	free_seen_data(seen_file);

	print_string( LOG_INFO, "-------------\n" );
	print_string( LOG_INFO, "totals for: %s\n",  [g_user_id UTF8String] );
	print_string( LOG_ERR,  "messages migrated: %lu (%llu bytes)\n",  g_count, g_size );

	if ( g_maildirsize != NULL ) {
		fclose( g_maildirsize );
		g_maildirsize = NULL;
	}

	print_string( LOG_DEBUG, "Finished migrating user account\n" );

	return( 0 );
} // cvt_mail_data

// ------------------------------------------------------------------
// main ()

int main ( int argc, char **argv )
{
	gPool = [[NSAutoreleasePool alloc] init];

	if ( geteuid() != 0 ) {
		fprintf( stdout, "%s must be run as root\n", argv[0] );
		exit(0);
	}

	if ( strcasestr(argv[0], "cvt_mail_data" ) ) {
		cvt_mail_data( argc, argv );
		exit(0);
	} else if ( strcasestr(argv[0], "mail_data_tool" ) ) {
		mail_data_tool( argc, argv );
		exit(0);
	} else if ( strcasestr(argv[0], "set_user_mail_opts" ) ) {
		set_user_mail_opts( argc, argv );
		exit(0);
	}

	return( 0 );
	[gPool release];
}
