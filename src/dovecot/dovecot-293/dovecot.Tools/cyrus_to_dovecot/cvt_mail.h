/*
 * Contains: Header definitions for Cyrus to Dovecot maildir mail migration
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
 */

#ifndef __CVT_MAIL_H__
#define	__CVT_MAIL_H__

// ------------------------------------------------------------------

#define BUF_SIZE		4096

#define CYRUS_FOLDERMAX	4096
#define CYRUS_SEENMAX	64000

// ------------------------------------------------------------------
// Definitions from cyrus mailbox.h
//	Needed for parsing the index and header files

#define PRIME (2147484043UL)

#define MAX_USER_FLAGS (16*8)

#define FLAG_ANSWERED (1<<0)
#define FLAG_FLAGGED (1<<1)
#define FLAG_DELETED (1<<2)
#define FLAG_DRAFT (1<<3)

typedef enum {
	kNO_FLAGS	= 0x00000000,
	kE_FLAG		= 0x00000001,
	kF_FLAG		= 0x00000002,
	kG_FLAG		= 0x00000004,
	kI_FLAG		= 0x00000008,
	kL_FLAG		= 0x00000010,
	kM_FLAG		= 0x00000020,
	kP_FLAG		= 0x00000040,
	kR_FLAG		= 0x00000080,
	kU_FLAG		= 0x00000100,
} TOOL_FLAGS;

#define IMAP_CONF_FILE	@"/etc"
#define IMAP_BINS_DIR	@"/usr/bin/cyrus/bin"

const char *SRC_MAIL_DB_DIR		= "/var/imap";
const char *SRC_MAIL_DATA_DIR	= "/var/spool/imap";
const char *DST_MAIL_DATA_DIR	= "/Library/Server/Mail/Data/mail";

#define MAILBOX_HEADER_MAGIC ("\241\002\213\015Cyrus mailbox header\n" \
     "\"The best thing about this system was that it had lots of goals.\"\n" \
     "\t--Jim Morris on Andrew\n")

#define	DEFAULT_DATA_LOCATION		@"/Library/Server/Mail/Data/mail"
#define	DOVECOT_PARTITION_MAPS		@"/Library/Server/Mail/Config/dovecot/partition_map.conf"
#define	MAIL_MIGRATION_PLIST		@"/Library/Server/Mail/Data/db/.mailmigration.plist"
#define	MAIL_USER_SETTINGS_PLIST	@"/Library/Server/Mail/Data/db/.mailusersettings.plist"

#define MAIL_TOOL_LOG_FILE			"/Library/Logs/Mail/mail-tool.log"

/* Migration key/values */
#define	kXMLKeyMigrationFlag			@"kMigrationFlag"
	#define	kXMLValueAcctMigrated			@"AcctMigrated"
	#define	kXMLValueAcctNotMigrated		@"AcctNotMigrated"

struct index_header {
	uint32_t	generation_no;
	uint32_t	format;
	uint32_t	minor_version;

	uint32_t	start_offset;
	uint32_t	record_size;
	uint32_t	exists;
	uint32_t	last_appenddate;
	uint32_t	last_uid;
	uint32_t	quota_mailbox_used64;
	uint32_t	quota_mailbox_used;
	uint32_t	pop3_last_login;
	uint32_t	uidvalidity;

	uint32_t	deleted;
	uint32_t	answered;
	uint32_t	flagged;

	uint32_t	pop3_new_uidl_or_mailbox_options;
	uint32_t	leaked_cache;
	uint32_t	highestmodseq64;
	uint32_t	highestmodseq;
	uint32_t	spare0;
	uint32_t	spare1;
	uint32_t	spare2;
	uint32_t	spare3;
	uint32_t	spare4;
};

struct index_entry {
	uint32_t	uid;
	uint32_t	internaldate;
	uint32_t	sentdate;
	uint32_t	size;
	uint32_t	header_size;
	uint32_t	content_offset;
	uint32_t	cache_offset;
	uint32_t	last_updated;
	uint32_t	system_flags;
	uint32_t	user_flags[ MAX_USER_FLAGS / 32 ];
	uint32_t	content_lines;
	uint32_t	cache_version;
	uint8_t		message_uuid[ 12 ];
	uint32_t	modseq64;
	uint32_t	modseq;
};

// ------------------------------------------------------------------
//	Local structs

struct s_seen_uids {
	unsigned long uid_min;
	unsigned long uid_max;
};

struct s_seen_line {
	NSString *mbox_uid_str;
	char	 *mbox_uid;
	NSString *seen_uids_str;
	char	 *seen_uids;
};

struct s_seen_data
{
	int seen_count;
	int uid_flag;
	int uid_count;
	struct s_seen_line seen_array[ CYRUS_FOLDERMAX + 1 ];
	struct s_seen_uids uid_array[ CYRUS_SEENMAX + 1 ];
};

// ------------------------------------------------------------------
// Utility functions

char   *cpy_str				( const char *in_str );
char   *read_line			( FILE *in_file );

#endif // __CVT_MAIL_H__
