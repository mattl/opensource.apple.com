/* Copyright (c) 2006-2010 Dovecot authors, see the included COPYING file */
/*
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

/* SearchKit indexing and searching of not only headers and text but also
   non-text attachments.

   The message uid and header/body indicator is in the name of each document so
   it is quick to map from SK search results to messages, and to filter by
   body-vs.-header.  But expunge is a challenge.  The names of the documents in
   the SK index are not deterministic both because of the random mkstemp()
   suffix and from possible changes to spill_dir over time.  Using
   SKIndexRenameDocument() and SKIndexMoveDocument() to unify the document
   namespace was too slow because both functions flush the index nearly every
   time they're called.  Iterating through all the documents was a non-starter.
   So instead we index one "shadow document" per message, the text of which is
   just "FTS<uid>SK", and which has properties attached that identify every
   regular document associated with that uid.  With this scheme, expunge can
   search for FTS<uid>SK to get the shadow document, and through that the
   regular documents.  All this just to make expunge quick, or at least no
   slower than it needs to be.  Since flushing the index can be very slow
   and the FTS API provides no way to make expunge asynchronous, defer
   expunges until the external updater runs (see fts_enqueue_update()) and
   executes fts_backend_optimize().

   SearchKit does not expose the fd it uses, so locking uses a separate file.
   Conveniently, this allows all locking to happen at the border (the
   fts_backend_sk_* functions) which helps avoid races since SearchKit does not
   offer an API for "open if present otherwise create."

   SearchKit indexes are limited to 32 bits so use multiple indexes
   ("fragments") when necessary.  Only the 0th index contains the meta
   properties.  Locking covers all the indexes together.

   Version 1 indexes only indicate whether terms are in the body or in any
   header.  Version 2 indexes are identical in format to version 1 indexes but
   also indicate whether terms are in the fts_header_want_indexed() headers,
   and whether a header of a given name is present in the message.  To upgrade
   from version 1 to version 2 perform a rescan.  */

#include "lib.h"
#include "str.h"
#include "array.h"
#include "ioloop.h"
#include "unichar.h"
#include "mail-namespace.h"
#include "mail-storage-private.h"
#include "mailbox-list-private.h"
#include "mail-search.h"
#include "nfs-workarounds.h"
#include "safe-mkstemp.h"
#include "file-lock.h"
#include "file-dotlock.h"
#include "write-full.h"
#include "seq-range-array.h"
#include "fts-api.h"
#include "fts-storage.h"
#include "fts-sk-plugin.h"

#include <stdlib.h>
#include <sysexits.h>

/* config.h defines DEBUG to an empty comment which makes CoreServices barf */
#ifdef DEBUG
# undef DEBUG
# define DEBUG 1
#endif

#define dec2str Carbon_conflicting_dec2str
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
	/* <rdar://problem/7819581> */
#include <CoreFoundation/CFPreferences_Private.h>
#undef dec2str

#define __APPLE_API_PRIVATE
#include <sandbox.h>
#undef __APPLE_API_PRIVATE

#define	SK_FILE_NAME			"dovecot.skindex"
#define	SK_LOCK_FILE_NAME		"dovecot.sklock"

#define	SK_LOCK_TIMEOUT_SECS		20
#define	SK_DOTLOCK_STALE_TIMEOUT_SECS	(15 * 60)

#define	SK_DEFAULT_MIN_TERM_LENGTH	1
#define	SK_SEARCH_SECS			5
#define	SK_DEFAULT_SPILL_DIR		"/tmp"
#define	SK_INDEX_SPILL_DIR		"index"
#define	SK_COMPACT_AGE			(2 * 24 * 60 * 60)
#define	SK_COMPACT_EXPUNGES		100
#define	SK_COMPACT_BATCH_SIZE		8

/* Until SearchKit supports more than 32 bits limit the size of each index.
   The exact size isn't important.  What's important is that it, in
   conjunction with lazy flushing, will never allow an index to grow "too big"
   in SK's eyes.  */
#define	SK_FRAGMENT_THRESHOLD_SIZE	((off_t) 1024 * 1024 * 1024)

#define	SK_TEXT_BUFFER			(128 * 1024)

#define	SK_INDEX_NAME			"fts-sk"
#define	SK_SCHEME			"fts-sk"
#define	SK_META_URL			SK_SCHEME"://meta"
#define	SK_META_VERSION			2
#define	SK_VERSION_NAME			"version"
#define	SK_LAST_UID_NAME		"last_uid"
#define	SK_UIDV_NAME			"uidvalidity"
#define	SK_FRAGS_NAME			"fragments"
#define	SK_DOCUMENTS_NAME		"documents"
#define	SK_SHADOW_NAME			"shadow"
#define	SK_EXPUNGED_UIDS_NAME		"expunged_uids"
#define	SK_DEFERRED_SINCE_NAME		"deferred_since"
#define	SK_HEADER_EXISTS_PREFIX		SK_SCHEME"-he-"

#define	SK_TERM_CHARS			"@.-+#$%_&'"	/* arbitrary */

#define	CFRelease_and_null(x)		STMT_START { \
						CFRelease(x); \
						(x) = NULL; \
					} STMT_END

static const char *sk_headers = "hdr";
static const char *sk_body = "body";
static const char *sk_shadow = "shadow";

struct sk_fts_index {
	char *path;
	SKIndexRef skiref;
	SKDocumentRef meta_doc;
};

struct sk_meta_values {
	unsigned int version;
	uint32_t uidvalidity;
	unsigned int fragments;
	uint32_t last_uid;
	CFArrayRef expungedUids;
	time_t deferred_since;
};

struct sk_fts_backend {
	struct fts_backend backend;

	long compact_age;
	int compact_expunges;
	unsigned int lock_timeout_secs;
	unsigned int default_min_term_length;
	unsigned int search_secs;
	char *spill_dir;

	struct mailbox *box;
	char *index_path;
	char *lock_path;
	enum file_lock_method lock_method;
	int lock_fd;
	struct dotlock_settings dotlock_set;
	struct file_lock *file_lock;
	struct dotlock *dotlock;
	int lock_type;

	struct sk_meta_values values;
	uint32_t uidvalidity;
	mode_t create_mode;
	gid_t create_gid;

	ARRAY(struct sk_fts_index) indexes;

	unsigned int debug:1;
};

struct sk_field {
	char *where;
	char *content_type;
	off_t size;

	buffer_t *text;
	string_t *spill_path;
	int spill_fd;

	unsigned int is_text:1;
	unsigned int sealed:1;
};

struct sk_fts_backend_update_context {
	struct fts_backend_update_context uctx;

	uint32_t uid;
	ARRAY(struct sk_field *) fields;
	struct sk_field *cur_field, *cur_field2;

	unsigned int fragment;

	CFMutableArrayRef shadowIds;
	CFMutableArrayRef pendingExpunges;
};

struct sk_lookup_context {
	struct sk_fts_backend *backend;
	ARRAY_TYPE(seq_range) expunged_uids;
	CFMutableStringRef query;
	ARRAY_TYPE(seq_range) definite_uids, maybe_uids;
};

struct sk_search_context {
	const struct sk_fts_index *skindex;
	SKSearchRef search;

	unsigned int done:1;
};

struct sk_fts_expunge_context {
	const struct sk_fts_index *skindex;
	SKSearchRef search;
	CFMutableArrayRef targets;

	unsigned int done:1;
};

static bool sk_extractors_loaded = FALSE;

static const char sk_sandbox_profile[] = "\
(version 1)\n\
(deny default)\n\
(import \"system.sb\")\n\
\n\
; don't spam logs with these denials\n\
(deny mach-per-user-lookup (with no-log))\n\
(deny file-read-metadata\n\
      (regex #\"^/(Volumes|Network)/.\")\n\
      (with no-log))\n\
(deny file-write*\n\
      (subpath \"/private/var/folders\")\n\
      (with no-log))\n\
\n\
(allow file-read*\n\
       (literal \"/\")\n\
       (regex #\"/(Library|Applications)($|/)\")	; explicitly not ^\n\
       (literal \"/private\")\n\
       (regex #\"^/(private/)?(tmp|var)($|/)\"))\n\
\n\
(allow file-read* file-write*\n\
       (subpath (param \"INDEX_DIR\"))\n\
       (subpath (param \"SPILL_DIR\")))\n\
\n\
(allow ipc-posix-shm\n\
       (ipc-posix-name-regex #\"^/tmp/com\\.apple\\.csseed\\.\"))\n\
\n\
(allow mach-lookup\n\
       (global-name \"com.apple.CoreServices.coreservicesd\"))\n\
\n\
(allow signal (target self))\n\
";

static void sk_extractors_load(struct sk_fts_backend *backend)
{
	const char *sandparams[5];
	char *index_path, *real_index_path, *real_spill_path;
	const char *spill_path;
	char *cp, *errorbuf;
	unsigned int slashes;

	if (sk_extractors_loaded)
		return;

	i_assert(!fts_header_want_indexed(sk_headers));
	i_assert(!fts_header_want_indexed(sk_body));
	i_assert(!fts_header_want_indexed(sk_shadow));

	/* SKLoadDefaultExtractorPlugins() only loads the plugins if
	   getuid() != 0.  We need those plugins! */
	if (getuid() == 0)
		i_fatal("fts_sk: bad privileges for extractors");

	/* <rdar://problem/7819581> */
	_CFPreferencesAlwaysUseVolatileUserDomains();

	/* indexer-worker can be shared among multiple users so don't
	   screw the sandbox down tight on a single user's maildir.
	   but also don't open up the entire file system. */
	index_path = i_strdup(backend->index_path);
	*strrchr(index_path, '/') = '\0';
	for (slashes = 0, cp = index_path; *cp; cp++)
		if (*cp == '/')
			++slashes;
	if (slashes < 3)	/* arbitrary but seems reasonable */
		i_fatal("fts_sk: unsafe path for indexing: %s", index_path);

	if (strcmp(backend->spill_dir, SK_INDEX_SPILL_DIR) == 0)
		spill_path = index_path;
	else
		spill_path = backend->spill_dir;

	/* sandbox barfs on symlinks */
	real_index_path = realpath(index_path, NULL);
	if (real_index_path == NULL)
		i_fatal("fts_sk: realpath(%s) failed: %m", index_path);
	real_spill_path = realpath(spill_path, NULL);
	if (real_spill_path == NULL)
		i_fatal("fts_sk: realpath(%s) failed: %m", spill_path);
	sandparams[0] = "INDEX_DIR";
	sandparams[1] = real_index_path;
	sandparams[2] = "SPILL_DIR";
	sandparams[3] = real_spill_path;
	sandparams[4] = NULL;
	errorbuf = NULL;
	if (sandbox_init_with_parameters(sk_sandbox_profile, 0, sandparams,
					 &errorbuf) != 0)
		i_fatal("fts_sk: sandbox_init failed: %s", errorbuf);
	free(real_spill_path);
	free(real_index_path);
	i_free(index_path);

	sandbox_note("fts_sk: loading metadata importers");
	SKLoadDefaultExtractorPlugIns();
	sandbox_note("");

	sk_extractors_loaded = TRUE;
}

static void sk_nonnull(const void *ptr, const char *tag)
{
	if (ptr == NULL)
		i_fatal_status(FATAL_OUTOFMEM,
			       "fts_sk: %s failed: Out of memory", tag);
}

static int sk_set_options(struct sk_fts_backend *backend, const char *str,
			  const char **error_r)
{
	const char *const *tmp;

	for (tmp = t_strsplit_spaces(str, " "); *tmp != NULL; tmp++) {
		if (strncasecmp(*tmp, "compact_age_days=", 17) == 0) {
			const char *p = *tmp + 17;
			long val = atoi(p) * 24 * 60 * 60;
			if (val <= 0) {
				*error_r = t_strdup_printf(
					"fts_sk: Invalid compact_age_days: %s",
					p);
				return -1;
			}
			backend->compact_age = val;
		} else if (strncasecmp(*tmp, "compact_expunges=", 17) == 0) {
			const char *p = *tmp + 17;
			int val = atoi(p);
			if (val <= 0) {
				*error_r = t_strdup_printf(
					"fts_sk: Invalid compact_expunges: %s",
					p);
				return -1;
			}
			backend->compact_expunges = val;
		} else if (strncasecmp(*tmp, "lock_timeout_secs=", 18) == 0) {
			const char *p = *tmp + 18;
			int val = atoi(p);
			if (val < 0) {
				*error_r = t_strdup_printf(
					"fts_sk: Invalid lock_timeout_secs: %s",
					p);
				return -1;
			}
			backend->lock_timeout_secs = val;
		} else if (strncasecmp(*tmp, "min_term_length=", 16) == 0) {
			const char *p = *tmp + 16;
			int val = atoi(p);
			if (val <= 0) {
				*error_r = t_strdup_printf(
					"fts_sk: Invalid min_term_length: %s",
					p);
				return -1;
			}
			backend->default_min_term_length = val;
		} else if (strncasecmp(*tmp, "indexer_timeout_secs=", 21) == 0) {
			i_warning("fts_sk: indexer_timeout_secs setting is "
				  "obsolete; use fts_index_timeout instead");
		} else if (strncasecmp(*tmp, "search_secs=", 12) == 0) {
			const char *p = *tmp + 12;
			int val = atoi(p);
			if (val < 0) {
				*error_r = t_strdup_printf(
					"fts_sk: Invalid search_secs: %s", p);
				return -1;
			}
			backend->search_secs = val;
		} else if (strncasecmp(*tmp, "spill_dir=", 10) == 0) {
			const char *p = *tmp + 10;
			if (strcasecmp(p, "index") == 0) {
				i_free(backend->spill_dir);
				backend->spill_dir =
					i_strdup(SK_INDEX_SPILL_DIR);
			} else if (*p == '/') {
				i_free(backend->spill_dir);
				backend->spill_dir = i_strdup(p);
			} else {
				*error_r = t_strdup_printf(
					"fts_sk: Invalid spill_dir: %s "
					"(need \"index\" or an absolute path)",
					p);
				return -1;
			}
		} else {
			*error_r = t_strdup_printf(
				"fts_sk: Invalid setting: %s", *tmp);
			return -1;
		}
	}
	return 0;
}

static CFDictionaryRef sk_index_properties(struct sk_fts_backend *backend)
{
	CFMutableDictionaryRef properties;
	CFNumberRef minTermLength;
	int max_terms;
	CFNumberRef maxTerms;

	properties =
		CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
					  &kCFTypeDictionaryKeyCallBacks,
					  &kCFTypeDictionaryValueCallBacks);
	sk_nonnull(properties, "CFDictionaryCreateMutable(index properties)");

	minTermLength = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type,
				       &backend->default_min_term_length);
	sk_nonnull(minTermLength, "CFNumberCreate(min_term_length)");
	CFDictionaryAddValue(properties, kSKMinTermLength, minTermLength);
	CFRelease_and_null(minTermLength);

	max_terms = 0;
	maxTerms = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type,
				  &max_terms);
	sk_nonnull(maxTerms, "CFNumberCreate(max_terms)");
	CFDictionaryAddValue(properties, kSKMaximumTerms, maxTerms);
	CFRelease_and_null(maxTerms);

	CFDictionaryAddValue(properties, kSKProximityIndexing, kCFBooleanTrue);
	CFDictionaryAddValue(properties, kSKTermChars, CFSTR(SK_TERM_CHARS));

	return properties;
}

static CFDictionaryRef sk_meta_properties(struct sk_fts_backend *backend,
					  const struct sk_meta_values *values)
{
	CFMutableDictionaryRef metaprops;
	unsigned int fragments;
	CFNumberRef versionNum, lastUid, uidValidity, fragmentCount,
		deferredSince;

	metaprops =
		CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
					  &kCFTypeDictionaryKeyCallBacks,
					  &kCFTypeDictionaryValueCallBacks);
	sk_nonnull(metaprops, "CFDictionaryCreateMutable(metaprops)");

	versionNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type,
				    &values->version);
	sk_nonnull(versionNum, "CFNumberCreate(version)");
	CFDictionaryAddValue(metaprops, CFSTR(SK_VERSION_NAME), versionNum);
	CFRelease_and_null(versionNum);

	uidValidity = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type,
				     &values->uidvalidity);
	sk_nonnull(uidValidity, "CFNumberCreate(uidvalidity)");
	CFDictionaryAddValue(metaprops, CFSTR(SK_UIDV_NAME), uidValidity);
	CFRelease_and_null(uidValidity);

	fragments = array_count(&backend->indexes);
	fragmentCount = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type,
				       &fragments);
	sk_nonnull(fragmentCount, "CFNumberCreate(fragments)");
	CFDictionaryAddValue(metaprops, CFSTR(SK_FRAGS_NAME), fragmentCount);
	CFRelease_and_null(fragmentCount);

	lastUid = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type,
				 &values->last_uid);
	sk_nonnull(lastUid, "CFNumberCreate(last_uid)");
	CFDictionaryAddValue(metaprops, CFSTR(SK_LAST_UID_NAME), lastUid);
	CFRelease_and_null(lastUid);

	if (values->expungedUids != NULL)
		CFDictionaryAddValue(metaprops, CFSTR(SK_EXPUNGED_UIDS_NAME),
				     values->expungedUids);
	else {
		/* SearchKit seems to keep removed metaprops dict entries,
		   so save an empty array */
		CFArrayRef empty = CFArrayCreate(kCFAllocatorDefault, NULL, 0,
						 &kCFTypeArrayCallBacks);
		CFDictionaryAddValue(metaprops, CFSTR(SK_EXPUNGED_UIDS_NAME),
				     empty);
		CFRelease_and_null(empty);
	}

	i_assert(sizeof values->deferred_since == sizeof (long));
	deferredSince = CFNumberCreate(kCFAllocatorDefault, kCFNumberLongType,
				       &values->deferred_since);
	sk_nonnull(deferredSince, "CFNumberCreate(deferred_since)");
	CFDictionaryAddValue(metaprops, CFSTR(SK_DEFERRED_SINCE_NAME),
			     deferredSince);
	CFRelease_and_null(deferredSince);

	return metaprops;
}

static void sk_meta_values_init(struct sk_meta_values *values)
{
	memset(values, 0, sizeof *values);
	values->version = SK_META_VERSION;
	values->last_uid = (uint32_t) -1;
}

static void sk_meta_values_deinit(struct sk_meta_values *values)
{
	if (values->expungedUids != NULL)
		CFRelease_and_null(values->expungedUids);
}

static int sk_save_meta_values(struct sk_fts_backend *backend)
{
	struct sk_meta_values *values = &backend->values;
	const struct sk_fts_index *skindex = array_idx(&backend->indexes, 0);

	if (values->last_uid == 0) {
		/* need to do this only once */
		if (!SKIndexAddDocumentWithText(skindex->skiref,
						skindex->meta_doc,
						NULL, TRUE)) {
			i_error("fts_sk: initializing meta props in %s failed",
				skindex->path);
			return -1;
		}
	}

	/* meta_doc must be indexed before setting its properties */
	CFDictionaryRef metaprops = sk_meta_properties(backend, values);
	SKIndexSetDocumentProperties(skindex->skiref, skindex->meta_doc,
				     metaprops);
	CFRelease_and_null(metaprops);
	return 1;
}

static int sk_load_meta_values(struct sk_fts_backend *backend)
{
	const struct sk_fts_index *skindex = array_idx(&backend->indexes, 0);
	struct sk_meta_values *values = &backend->values;
	CFDictionaryRef metaprops;
	CFNumberRef num;
	int ret = 0;

	metaprops = SKIndexCopyDocumentProperties(skindex->skiref,
						  skindex->meta_doc);
	if (metaprops == NULL) {
		i_error("fts_sk: meta properties missing from %s",
			skindex->path);
		return -1;
	}

	sk_meta_values_init(values);

	num = (CFNumberRef) CFDictionaryGetValue(metaprops,
						 CFSTR(SK_VERSION_NAME));
	if (num == NULL || !CFNumberGetValue(num, kCFNumberSInt32Type,
					     &values->version))
		ret = -1;

	num = (CFNumberRef) CFDictionaryGetValue(metaprops,
						 CFSTR(SK_UIDV_NAME));
	if (num == NULL || !CFNumberGetValue(num, kCFNumberSInt32Type,
					     &values->uidvalidity))
		ret = -1;

	num = (CFNumberRef) CFDictionaryGetValue(metaprops,
						 CFSTR(SK_FRAGS_NAME));
	if (num == NULL || !CFNumberGetValue(num, kCFNumberSInt32Type,
					     &values->fragments))
		ret = -1;

	num = (CFNumberRef) CFDictionaryGetValue(metaprops,
						 CFSTR(SK_LAST_UID_NAME));
	if (num == NULL || !CFNumberGetValue(num, kCFNumberSInt32Type,
					     &values->last_uid))
		ret = -1;

	values->expungedUids = (CFArrayRef) CFDictionaryGetValue(metaprops,
					    CFSTR(SK_EXPUNGED_UIDS_NAME));
	if (values->expungedUids != NULL)
		CFRetain(values->expungedUids);

	num = (CFNumberRef) CFDictionaryGetValue(metaprops,
						 CFSTR(SK_DEFERRED_SINCE_NAME));
	i_assert(sizeof values->deferred_since == sizeof (long));
	if (num == NULL || !CFNumberGetValue(num, kCFNumberLongType,
					     &values->deferred_since))
		ret = -1;

	CFRelease_and_null(metaprops);

	if (ret < 0)
		sk_meta_values_deinit(values);
	return ret;
}

static int sk_delete(struct sk_fts_backend *backend)
{
	struct sk_fts_index *skindex;
	unsigned int count, i;
	int ret = 0;

	skindex = array_get_modifiable(&backend->indexes, &count);
	for (i = 0; i < count; i++) {
		if (unlink(skindex[i].path) == 0) {
			i_info("fts_sk: deleted %s", skindex[i].path);
			if (ret == 0)
				ret = 1;
		} else if (errno != ENOENT) {
			i_error("fts_sk: unlink(%s) failed: %m",
				skindex[i].path);
			ret = -1;
		}
		i_assert(skindex[i].skiref == NULL);
		if (i > 0) {
			i_free(skindex[i].path);
			i_assert(skindex[i].meta_doc == NULL);
		}
	}
	array_delete(&backend->indexes, 1, count - 1);

	return ret;
}

static void sk_close(struct sk_fts_backend *backend)
{
	struct sk_fts_index *skindex;

	array_foreach_modifiable(&backend->indexes, skindex) {
		if (skindex->skiref != NULL) {
			SKIndexClose(skindex->skiref);
			skindex->skiref = NULL;
		}
	}
}

static int sk_flush(struct sk_fts_backend *backend)
{
	const struct sk_fts_index *skindex;
	int ret = 0;

	array_foreach(&backend->indexes, skindex) {
		if (!SKIndexFlush(skindex->skiref)) {
			i_error("fts_sk: SKIndexFlush(%s) failed",
				skindex->path);
			ret = -1;
		}
	}

	return ret;
}

static bool sk_is_open(struct sk_fts_backend *backend)
{
	struct sk_fts_index *skindex =
		array_idx_modifiable(&backend->indexes, 0);
	return skindex->skiref != NULL;
}

static int sk_open(struct sk_fts_backend *backend, bool writable,
		   bool meta_only)
{
	struct sk_fts_index *skindex;
	size_t path_len;
	CFURLRef url;
	int error_ret = -1;
	unsigned int fragments = 0, i;
	string_t *fragment_path;

	skindex = array_idx_modifiable(&backend->indexes, 0);
	if (skindex->skiref != NULL)
		return 1;

	path_len = strlen(skindex->path);
	url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
		(const UInt8 *) skindex->path, path_len, FALSE);
	sk_nonnull(url, "CFURLCreateFromFileSystemRepresentation(path)");

	/* open the first index */
	sandbox_note(t_strdup_printf("fts_sk: opening \"%s\" writable %d",
				     skindex->path, writable));
	skindex->skiref = SKIndexOpenWithURL(url, CFSTR(SK_INDEX_NAME),
					     writable);
	sandbox_note("");
	CFRelease_and_null(url);
	if (skindex->skiref == NULL)
		return -1;

	if (sk_load_meta_values(backend) < 0) {
		sk_close(backend);
		return -1;
	}

	/* prepare index fragment paths in case we need to call sk_delete() */
	fragment_path = t_str_new(path_len + 5);
	str_append(fragment_path, skindex->path);
	for (i = 1; fragments == 0 || i < fragments; i++) {
		str_truncate(fragment_path, path_len);
		str_printfa(fragment_path, "-%u", i);

		if (fragments == 0) {
			struct stat stbuf;

			if (stat(str_c(fragment_path), &stbuf) < 0) {
				if (errno != ENOENT)
					i_error("fts_sk: stat(%s): %m",
						str_c(fragment_path));
				break;
			}
		}

		if (i < array_count(&backend->indexes))
			i_assert((array_idx(&backend->indexes, i))->path != 
				 NULL);
		else {
			skindex = array_append_space(&backend->indexes);
			skindex->path = i_strdup(str_c(fragment_path));
		}
	}

	/* the array may have been reallocated */
	skindex = array_get_modifiable(&backend->indexes, &fragments);

	/* sanity check */
	if (backend->values.version > SK_META_VERSION) {
		i_warning("fts_sk: %s is version %d which is "
			  "newer than version %d; ignoring",
			  skindex->path, backend->values.version,
			  SK_META_VERSION);
		error_ret = 0;
		sk_close(backend);
	} else if (backend->values.version < SK_META_VERSION) {
		/* FIXME when appropriate:  upgrade old index to new
		   version.  No upgrade from v1 to v2; that is, if v1
		   remain v1. */
	}
	if (backend->values.uidvalidity != backend->uidvalidity) {
		sk_close(backend);
		sk_delete(backend);
	}
	/* the array may have been reallocated */
	skindex = array_get_modifiable(&backend->indexes, &fragments);
	i_assert(fragments > 0);
	if (skindex->skiref == NULL)
		return error_ret;

	if (meta_only)
		return 1;

	/* open the other fragments */
	for (i = 1; i < fragments; i++) {
		url = CFURLCreateFromFileSystemRepresentation(
			kCFAllocatorDefault, (const UInt8 *) skindex[i].path,
			strlen(skindex[i].path), FALSE);
		sk_nonnull(url, "CFURLCreateFromFileSystemRepresentation(fragment_path)");
		sandbox_note(t_strdup_printf("fts_sk: opening \"%s\" writable %d",
					     skindex[i].path, writable));
		skindex[i].skiref = SKIndexOpenWithURL(url,
						       CFSTR(SK_INDEX_NAME),
						       writable);
		sandbox_note("");
		CFRelease_and_null(url);

		if (skindex[i].skiref == NULL) {
			i_error("fts_sk: failed to open index fragment %s",
				skindex[i].path);
			break;
		}
	}
	if (i < fragments) {
		sk_close(backend);
		return -1;
	}

	return 1;
}

static int sk_create(struct sk_fts_backend *backend, unsigned int fragno)
{
	struct sk_fts_index *skindex;
	CFURLRef url;

	skindex = array_idx_modifiable(&backend->indexes, fragno);
	i_assert(skindex->skiref == NULL);

	url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
		(const UInt8 *) skindex->path, strlen(skindex->path), FALSE);
	sk_nonnull(url, "CFURLCreateFromFileSystemRepresentation(path)");

	/* Don't adjust umask since we can't set the mode til after. */
	CFDictionaryRef properties = sk_index_properties(backend);
	sandbox_note(t_strdup_printf("fts_sk: creating \"%s\"",
				     skindex->path));
	skindex->skiref = SKIndexCreateWithURL(url,
					       CFSTR(SK_INDEX_NAME),
					       kSKIndexInverted,
					       properties);
	sandbox_note("");
	CFRelease_and_null(properties);
	CFRelease_and_null(url);
	if (skindex->skiref != NULL) {
		if (chmod(skindex->path, backend->create_mode) < 0)
			i_warning("fts_sk: chmod(%s, 0%o) failed: %m",
			       skindex->path, backend->create_mode);
		if (backend->create_gid != (gid_t) -1 &&
		    chown(skindex->path, (uid_t) -1,
			  backend->create_gid) < 0)
			i_warning("fts_sk: chown(%s, -1, %d) failed: %m",
			       skindex->path, backend->create_gid);

		if (fragno == 0) {
			sk_meta_values_init(&backend->values);
			backend->values.uidvalidity = backend->uidvalidity;
			backend->values.last_uid = 0;
			if (sk_save_meta_values(backend) < 0 ||
			    sk_flush(backend) < 0) {
				sk_close(backend);
				sk_delete(backend);
			}
		} else if (sk_flush(backend) < 0) {
			sk_close(backend);
			sk_delete(backend);
		}
	}

	return skindex->skiref == NULL ? -1 : 1;
}

static int sk_open_or_create(struct sk_fts_backend *backend, bool meta_only)
{
	int ret;

	ret = sk_open(backend, TRUE, meta_only);
	if (ret < 0)		/* maybe it doesn't exist; try creating it */
		ret = sk_create(backend, 0);
	if (ret <= 0) {
		/* self-heal when index is damaged */
		if (sk_delete(backend) > 0)
			ret = sk_create(backend, 0);
	}
	if (ret <= 0)
		i_error("fts_sk: Could not open or create %s",
			(array_idx(&backend->indexes, 0))->path);

	return ret;
}

static void sk_doc_mkname(string_t *name, uint32_t uid, const char *where)
{
	/* putting the uid and the where in the url is crucial */
	str_printfa(name, "uid=%u&where=%s", uid, where);
}

static SKDocumentRef sk_create_document(string_t *doc_path)
{
	CFURLRef doc_url;
	SKDocumentRef doc;

	doc_url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
		str_data(doc_path), str_len(doc_path), FALSE);
	if (doc_url == NULL) {
		i_error("fts_sk: CFURLCreateWithString(%s) failed",
			str_c(doc_path));
		return NULL;
	}

	doc = SKDocumentCreateWithURL(doc_url);
	if (doc == NULL)
		i_error("fts_sk: SKDocumentCreateWithURL(%s) failed",
			str_c(doc_path));

	CFRelease_and_null(doc_url);
	return doc;
}

static SKDocumentRef sk_create_document_immediate(uint32_t uid,
						  const char *where)
{
	string_t *path;

	path = t_str_new(64);
	str_append(path, "/"SK_DOCUMENTS_NAME"/");
	sk_doc_mkname(path, uid, where);

	return sk_create_document(path);
}

static const char *sk_clean_term(const char *term, bool *quote)
{
	string_t *clean;

	if (strcmp(term, "NOT") == 0 || strcmp(term, "AND") == 0 ||
	    strcmp(term, "OR") == 0) {
		/* quote around SearchKit operators */
		*quote = TRUE;
		return term;
	}

	clean = t_str_new(64);
	while (*term) {
		char c = *term;
		unsigned int len = uni_utf8_char_bytes(c);
		if (len == 1) {
			if ((unsigned char) c <= '\"' ||
			    strchr("&|*()", c) != NULL) {
				/* escape SearchKit operators */
				str_append_c(clean, '\\');
			}
		} else if (len == 0)
			len = 1;
		str_append_n(clean, term, len);
		term += len;
	}

	return str_c(clean);
}

static void sk_build_query(CFMutableStringRef query,
			   const struct mail_search_arg *arg)
{
	const char *term = arg->value.str;
	const char **words;
	unsigned int count, i;

	if (arg->fuzzy) {
		CFStringAppendCString(query, term, kCFStringEncodingUTF8);
		return;
	}

	words = t_strsplit_spaces(term, " \t");
	for (count = 0; words[count] != NULL; ++count)
		;

	if (count > 1)
		CFStringAppend(query, CFSTR("("));

	for (i = 0; i < count; i++) {
		bool quote = FALSE;
		const char *word = sk_clean_term(words[i], &quote);

		if (i > 0)
			CFStringAppend(query, CFSTR(" "));

		if (quote) {
			/* SearchKit can't do subphrase matches so no wildcards */
			CFStringAppend(query, CFSTR("\""));
			CFStringAppendCString(query, word, kCFStringEncodingUTF8);
			CFStringAppend(query, CFSTR("\""));
		} else {
			/* IMAP requires substring match always */
			CFStringAppend(query, CFSTR("*"));
			CFStringAppendCString(query, word, kCFStringEncodingUTF8);
			CFStringAppend(query, CFSTR("*"));
		}
	}

	if (count > 1)
		CFStringAppend(query, CFSTR(")"));
}


static void sk_search_do(struct sk_lookup_context *lctx,
			 struct sk_search_context *sctx,
			 const struct mail_search_arg *arg,
			 ARRAY_TYPE(fts_score_map) *score_map)
{
	struct sk_fts_backend *backend = lctx->backend;
	enum { SK_MAXDOCS = 50 };
	SKDocumentID results[SK_MAXDOCS];
	float scores[SK_MAXDOCS];
	CFStringRef names[SK_MAXDOCS];
	CFIndex found = 0, i;

	memset(results, 0, sizeof results);
	if (score_map != NULL)
		memset(scores, 0, sizeof scores);
	memset(names, 0, sizeof names);
	sctx->done = !SKSearchFindMatches(sctx->search, SK_MAXDOCS, results,
					  score_map != NULL ? scores : NULL,
					  backend->search_secs, &found);
	SKIndexCopyInfoForDocumentIDs(sctx->skindex->skiref, found, results,
				      names, NULL);
	for (i = 0; i < found; i++) {
		char name[128], where[128], *amp;
		uint32_t uid = 0;
		ARRAY_TYPE(seq_range) *uids = NULL;

		if (names[i] == NULL)
			continue;

		if (!CFStringGetCString(names[i], name, sizeof name,
					kCFStringEncodingUTF8)) {
			CFRelease_and_null(names[i]);
			continue;
		}
		CFRelease_and_null(names[i]);

		where[0] = '\0';
		if (sscanf(name, "uid=%u&where=%s", &uid, where) != 2 ||
		    uid == 0)
			continue;

		/* version 1 indexes included &seq=%u, absent from version 2,
		   also there might be &rand= */
		if ((amp = strchr(where, '&')) != NULL)
			*amp = '\0';

		if (strcmp(where, sk_shadow) == 0)
			continue;	/* shadows are internal only */
		if (seq_range_exists(&lctx->expunged_uids, uid))
			continue;

		switch (arg->type) {
		case SEARCH_TEXT:
			/* definite match anywhere in message */
			uids = &lctx->definite_uids;
			break;
		case SEARCH_BODY:
			/* definite match if in body */
			if (strcmp(where, sk_body) == 0)
				uids = &lctx->definite_uids;
			break;
		case SEARCH_HEADER:
		case SEARCH_HEADER_ADDRESS:
		case SEARCH_HEADER_COMPRESS_LWSP:
			if (backend->values.version == 1) {
				/* version 1 indexes only indicate body vs.
				   any-header; maybe match if in any header */
				if (strcmp(where, sk_headers) == 0)
					uids = &lctx->maybe_uids;
			} else if (fts_header_want_indexed(arg->hdr_field_name)) {
				/* version 2 indexes indicate body, each of the
				   fts_header_want_indexed() headers, and
				   any-header; definite match if in indexed
				   header */
				if (strcmp(where, arg->hdr_field_name) == 0)
					uids = &lctx->definite_uids;
			} else {
				/* maybe match if in any header */
				if (strcmp(where, sk_headers) == 0)
					uids = &lctx->maybe_uids;
			}
			break;
		default:
			i_unreached();
		}
		if (uids == NULL)
			continue;

		i_assert(array_is_created(uids));
		seq_range_array_add(uids, uid);

		if (score_map != NULL) {
			struct fts_score_map *score =
				array_append_space(score_map);
			score->uid = uid;
			score->score = scores[i];
		}
	}
}

static int sk_search(struct sk_lookup_context *lctx,
		     const struct mail_search_arg *arg,
		     ARRAY_TYPE(fts_score_map) *score_map)
{
	struct sk_fts_backend *backend = lctx->backend;
	unsigned int count, i;
	const struct sk_fts_index *skindex;
	ARRAY(struct sk_search_context) searches;
	struct sk_search_context *sctx;
	bool more;

	CFStringReplaceAll(lctx->query, CFSTR(""));
	sk_build_query(lctx->query, arg);

	skindex = array_get(&backend->indexes, &count);
	t_array_init(&searches, count);
	for (i = 0; i < count; i++) {
		SKSearchOptions opts = kSKSearchOptionDefault;
		SKSearchRef search;

		if (score_map == NULL)
			opts |= kSKSearchOptionNoRelevanceScores;
		if (arg->fuzzy)
			opts |= kSKSearchOptionFindSimilar;
		search = SKSearchCreate(skindex[i].skiref, lctx->query, opts);
		if (search == NULL) {
			char qbuf[1024];
			if (!CFStringGetCString(lctx->query, qbuf, sizeof qbuf,
						kCFStringEncodingUTF8))
				strcpy(qbuf, "(unknown)");
			i_error("fts_sk: SKSearchCreate(%s, %s) failed",
				skindex[i].path, qbuf);
			break;
		}

		sctx = array_append_space(&searches);
		sctx->skindex = &skindex[i];
		sctx->search = search;
	}
	if (i < count) {
		array_foreach_modifiable(&searches, sctx)
			CFRelease_and_null(sctx->search);
		return -1;
	}

	/* search across all the indexes in parallel */
	sctx = array_get_modifiable(&searches, &count);
	do {
		more = FALSE;
		for (i = 0; i < count; i++) {
			if (sctx[i].done)
				continue;
			sk_search_do(lctx, &sctx[i], arg, score_map);
			if (sctx[i].done)
				CFRelease_and_null(sctx[i].search);
			else
				more = TRUE;
		}
	} while (more);

	return 1;
}

static struct sk_lookup_context *sk_lookup_new(struct sk_fts_backend *backend)
{
	struct sk_meta_values *values = &backend->values;
	struct sk_lookup_context *lctx;
	unsigned int count, i;

	/* SK requires flush before search */
	if (sk_flush(backend) < 0)
		return NULL;

	lctx = i_new(struct sk_lookup_context, 1);
	lctx->backend = backend;
	lctx->query = CFStringCreateMutable(kCFAllocatorDefault, 0);
	sk_nonnull(lctx->query, "CFStringCreateMutable(query)");
	i_array_init(&lctx->definite_uids, 128);
	i_array_init(&lctx->maybe_uids, 128);

	/* searching may find uids that have been expunge-deferred;
	   skip those results */
	count = values->expungedUids != NULL ?
		CFArrayGetCount(values->expungedUids) : 0;
	i_array_init(&lctx->expunged_uids, count);
	for (i = 0; i < count; i++) {
		CFNumberRef num = (CFNumberRef)
			CFArrayGetValueAtIndex(values->expungedUids, i);
		uint32_t uid;

		if (num == NULL ||
		    !CFNumberGetValue(num, kCFNumberSInt32Type, &uid))
			uid = 0;
		if (uid != 0)
			seq_range_array_add(&lctx->expunged_uids, uid);
	}

	return lctx;
}

static void sk_lookup_free(struct sk_lookup_context **_lctx)
{
	struct sk_lookup_context *lctx = *_lctx;

	*_lctx = NULL;
	array_free(&lctx->expunged_uids);
	CFRelease_and_null(lctx->query);
	array_free(&lctx->definite_uids);
	array_free(&lctx->maybe_uids);
	i_free(lctx);
}

static int sk_lookup_arg(struct sk_lookup_context *lctx,
			 struct mail_search_arg *arg, bool and_args,
			 struct fts_result *result)
{
	struct sk_fts_backend *backend = lctx->backend;
	const char *orig_arg_str = arg->value.str;
	int ret;

	switch (arg->type) {
	case SEARCH_TEXT:
	case SEARCH_BODY:
		/* we can search for these */
		break;
	case SEARCH_HEADER:
	case SEARCH_HEADER_ADDRESS:
	case SEARCH_HEADER_COMPRESS_LWSP:
		if (*arg->value.str == '\0') {
			/* to search for the existence of a header give SK a
			   special string */
			arg->value.str = t_strconcat(SK_HEADER_EXISTS_PREFIX,
						     arg->hdr_field_name,
						     NULL);
		}
		/* we can search for these */
		break;
	default:
		/* ignore all other args */
		return 0;
	}

	array_clear(&lctx->definite_uids);
	array_clear(&lctx->maybe_uids);
	ret = sk_search(lctx, arg, &result->scores);
	arg->value.str = orig_arg_str;

	/* begin code copied from squat_lookup_arg(); not also merging scores
	   because (a) how do you score a not-match? and (b) scores array is not
	   a source of truth for the results.  FIXME? */
	if (arg->match_not) {
		/* definite -> non-match
		   maybe -> maybe
		   non-match -> maybe */
		array_clear(&lctx->maybe_uids);

		seq_range_array_add_range(&lctx->maybe_uids, 1,
					  backend->values.last_uid);
		seq_range_array_remove_seq_range(&lctx->maybe_uids,
						 &lctx->definite_uids);
		array_clear(&lctx->definite_uids);
	}
	if (and_args) {
		/* AND:
		   definite && definite -> definite
		   definite && maybe -> maybe
		   maybe && maybe -> maybe */

		/* put definites among maybies, so they can be intersected */
		seq_range_array_merge(&result->maybe_uids,
				      &result->definite_uids);
		seq_range_array_merge(&lctx->maybe_uids,
				      &lctx->definite_uids);

		seq_range_array_intersect(&result->maybe_uids,
					  &lctx->maybe_uids);
		seq_range_array_intersect(&result->definite_uids,
					  &lctx->definite_uids);
		/* remove duplicate maybies that are also definites */
		seq_range_array_remove_seq_range(&result->maybe_uids,
						 &result->definite_uids);
	} else {
		/* OR:
		   definite || definite -> definite
		   definite || maybe -> definite
		   maybe || maybe -> maybe */

		/* remove maybies that are now definites */
		seq_range_array_remove_seq_range(&lctx->maybe_uids,
						 &result->definite_uids);
		seq_range_array_remove_seq_range(&result->maybe_uids,
						 &lctx->definite_uids);

		seq_range_array_merge(&result->definite_uids,
				      &lctx->definite_uids);
		seq_range_array_merge(&result->maybe_uids,
				      &lctx->maybe_uids);
	}
	/* end copy */

	return ret < 0 ? -1 : 1;
}

static int sk_lookup(struct sk_lookup_context *lctx,
		     struct mail_search_arg *args, bool and_args,
		     struct fts_result *result)
{
	struct mail_search_arg *arg;
	bool first = TRUE;
	int ret;

	/* Solr can search for terms in different parts of a document at once,
	   e.g. hdr:foo+AND+body:bar, so fts-solr makes only two search passes:
	   one for definite matches (like body:bar or to:bar) and one for maybe
	   matches (like hdr:foo).  Squat searches for terms singly so
	   fts-squat merges the results after each term.  SK can search for
	   multiple terms at once but not if they're isolated to different
	   parts of the document, so e.g. "foo AND bar" works only if both are
	   wanted in the body or both in a header.  Also, SK does not support
	   naked "NOT foo"; it needs a non-inverted search term from which to
	   exclude the NOT terms.  Furthermore, we find out where a term
	   appears only after the SK search is finished.  Therefore we follow
	   the fts-squat example:  search for each term separately and merge
	   the results.  This also allows for individual fuzzy terms. */
	for (arg = args; arg; arg = arg->next) {
		ret = sk_lookup_arg(lctx, arg, first ? FALSE : and_args,
				    result);
		if (ret < 0)
			return -1;
		if (ret > 0) {
			arg->match_always = TRUE;
			first = FALSE;
		}
	}
	return 0;
}

static void sk_update_save_id(struct sk_fts_backend_update_context *uctx,
			      SKDocumentID id)
{
	CFNumberRef num;

	if (uctx->shadowIds == NULL) {
		uctx->shadowIds =
			CFArrayCreateMutable(kCFAllocatorDefault,
					     0, &kCFTypeArrayCallBacks);
		sk_nonnull(uctx->shadowIds, "CFArrayCreateMutable(shadowIds)");
	}

	num = CFNumberCreate(kCFAllocatorDefault, kCFNumberCFIndexType, &id);
	sk_nonnull(num, "CFNumberCreate(id)");
	CFArrayAppendValue(uctx->shadowIds, num);
	CFRelease_and_null(num);
}

static struct sk_field *sk_field_new(const char *where,
				      const char *content_type)
{
	struct sk_field *field;

	field = i_new(struct sk_field, 1);
	field->where = str_lcase(i_strdup(where));
	field->content_type = i_strdup(content_type);
	field->spill_fd = -1;
	if (strncasecmp(content_type, "text/", 5) == 0 ||
	    strcasecmp(content_type, "text") == 0)
		field->is_text = TRUE;
	return field;
}

static void sk_field_free(struct sk_field **_field)
{
	struct sk_field *field = *_field;

	*_field = NULL;
	i_free(field->where);
	i_free(field->content_type);
	if (field->text != NULL)
		buffer_free(&field->text);
	if (field->spill_fd >= 0) {
		if (close(field->spill_fd) < 0)
			i_error("fts_sk: close(%s) failed: %m",
				str_c(field->spill_path));
		field->spill_fd = -1;

		if (unlink(str_c(field->spill_path)) < 0 &&
		    errno != ENOENT)
			i_error("fts_sk: unlink(%s) failed: %m",
				str_c(field->spill_path));
		str_free(&field->spill_path);
	}
	i_free(field);
}

static struct sk_field *
sk_update_field_get(struct sk_fts_backend_update_context *uctx,
		    const char *where, const char *content_type)
{
	struct sk_field * const *fieldp;
	struct sk_field *new_field;

	array_foreach(&uctx->fields, fieldp) {
		struct sk_field *field = *fieldp;
		if (!field->sealed &&
		    strcasecmp(field->where, where) == 0 &&
		    strcasecmp(field->content_type, content_type) == 0)
			return field;
	}

	new_field = sk_field_new(where, content_type);
	array_append(&uctx->fields, &new_field, 1);
	return new_field;
}

static int sk_update_field_spill(struct sk_fts_backend_update_context *uctx,
				  struct sk_field *field)
{
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) uctx->uctx.backend;

	if (field->spill_fd < 0) {
		char *dir = backend->spill_dir;
		if (strcmp(dir, SK_INDEX_SPILL_DIR) == 0)
			dir = backend->index_path;
		field->spill_path = str_new(default_pool, 256);
		str_printfa(field->spill_path, "%s/", dir);
		sk_doc_mkname(field->spill_path, uctx->uid, field->where);
		str_append(field->spill_path, "&rand=");
		field->spill_fd = safe_mkstemp(field->spill_path, 0600,
					       (uid_t) -1, backend->create_gid);
		if (field->spill_fd < 0) {
			i_error("fts_sk: safe_mkstemp(%s) failed: %m",
				str_c(field->spill_path));
			str_free(&field->spill_path);
			return -1;
		}
	}
	if (field->text != NULL) {
		size_t text_size = 0;
		const void *text_data = buffer_get_data(field->text,
							&text_size);
		if (write_full(field->spill_fd, text_data, text_size) < 0) {
			i_error("fts_sk: write_full(%s, %"PRIuSIZE_T") failed: %m",
				str_c(field->spill_path), text_size);
			return -1;
		}
		buffer_free(&field->text);
	}
	return 1;
}

static int sk_update_field_append(struct sk_fts_backend_update_context *uctx,
				  struct sk_field *field,
				  const void *data, size_t size)
{
	/* buffer small text for use with SKIndexAddDocumentWithText().
	   spill large text or non-text for use with SKIndexAddDocument(). */

	if (field->spill_fd < 0 && field->is_text) {
		size_t length = field->text != NULL ?
			buffer_get_used_size(field->text) : 0;
		if (length + size <= SK_TEXT_BUFFER) {
			if (field->text == NULL)
				field->text =
					buffer_create_dynamic(default_pool,
							      SK_TEXT_BUFFER);
			buffer_append(field->text, data, size);
			field->size += size;
			return 1;
		}
	}

	if (sk_update_field_spill(uctx, field) < 0)
		return -1;
	if (write_full(field->spill_fd, data, size) < 0) {
		i_error("fts_sk: write_full(%s, %"PRIuSIZE_T") failed: %m",
			str_c(field->spill_path), size);
		return -1;
	}
	field->size += size;
	return 1;
}

static int sk_update_field_commit(struct sk_fts_backend_update_context *uctx,
				  struct sk_field *field)
{
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) uctx->uctx.backend;
	const struct sk_fts_index *skindex = array_idx(&backend->indexes,
						       uctx->fragment);
	int ret = 0;

	if (field->text != NULL) {
		size_t text_size = 0;
		const void *text_data = buffer_get_data(field->text,
							&text_size);
		SKDocumentRef doc;
		CFStringRef text;

		i_assert(field->spill_fd < 0);
		if (text_size == 0)
			return 1;

		doc = sk_create_document_immediate(uctx->uid, field->where);
		if (doc == NULL)
			return -1;

		text = CFStringCreateWithBytesNoCopy(kCFAllocatorDefault,
						     text_data, text_size,
						     kCFStringEncodingUTF8,
						     FALSE, kCFAllocatorNull);
		if (text != NULL) {
			if (SKIndexAddDocumentWithText(skindex->skiref, doc,
						       text, TRUE)) {
				sk_update_save_id(uctx,
					SKIndexGetDocumentID(skindex->skiref,
							     doc));
				ret = 1;
			} else {
				i_error("fts_sk: SKIndexAddDocumentWithText(path=%s, size=%lu) failed",
					skindex->path, text_size);
				ret = -1;
			}

			buffer_free(&field->text);
			field->size = 0;

			CFRelease_and_null(text);
		} else {
			/* maybe non-text chars. try SKIndexAddDocument() */
			(void) sk_update_field_spill(uctx, field);
		}
		CFRelease_and_null(doc);
	}
	if (field->spill_fd >= 0) {
		SKDocumentRef doc;
		CFStringRef type_str;
		Boolean added;

		i_assert(field->text == NULL);

		doc = sk_create_document(field->spill_path);
		if (doc == NULL)
			return -1;

		type_str = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault,
							   field->content_type,
							   kCFStringEncodingUTF8,
							   kCFAllocatorNull);
		sk_nonnull(type_str, "CFStringCreateWithCStringNoCopy(content-type)");

		if (close(field->spill_fd) < 0)
			i_error("fts_sk: close(%s) failed: %m",
				str_c(field->spill_path));
		field->spill_fd = -1;

		sk_extractors_load(backend);
		sandbox_note(t_strdup_printf("indexing \"%s\" type \"%s\"",
					     str_c(field->spill_path),
					     field->content_type));
		added = SKIndexAddDocument(skindex->skiref, doc, type_str,
					   TRUE);
		sandbox_note("");
		if (added) {
			sk_update_save_id(uctx,
					  SKIndexGetDocumentID(skindex->skiref,
							       doc));
			ret = 1;
		} else {
			i_error("fts_sk: SKIndexAddDocument(%s) failed",
				field->content_type);
			ret = -1;
		}
		CFRelease_and_null(type_str);
		CFRelease_and_null(doc);

		if (unlink(str_c(field->spill_path)) < 0)
			i_error("fts_sk: unlink(%s) failed: %m",
				str_c(field->spill_path));
		str_free(&field->spill_path);
		field->size = 0;
	}
	return ret;
}

static int sk_update_shadow(struct sk_fts_backend_update_context *uctx)
{
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) uctx->uctx.backend;
	const struct sk_fts_index *skindex = array_idx(&backend->indexes,
						       uctx->fragment);
	SKDocumentRef shadow;
	CFStringRef text;
	CFMutableDictionaryRef properties;

	if (uctx->shadowIds == NULL)
		return 0;

	shadow = sk_create_document_immediate(uctx->uid, sk_shadow);
	if (shadow == NULL) {
		CFRelease_and_null(uctx->shadowIds);
		return -1;
	}

	text = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
					CFSTR("FTS%uSK"), uctx->uid);
	if (text == NULL) {
		CFRelease_and_null(shadow);
		CFRelease_and_null(uctx->shadowIds);
		return -1;
	}

	if (!SKIndexAddDocumentWithText(skindex->skiref, shadow, text, TRUE)) {
		CFRelease_and_null(text);
		CFRelease_and_null(shadow);
		CFRelease_and_null(uctx->shadowIds);
		return -1;
	}
	CFRelease_and_null(text);

	/* shadow properties include shadow document's id */
	sk_update_save_id(uctx, SKIndexGetDocumentID(skindex->skiref, shadow));

	properties = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
					       &kCFTypeDictionaryKeyCallBacks,
					       &kCFTypeDictionaryValueCallBacks);
	sk_nonnull(properties, "CFDictionaryCreateMutable(shadow properties)");
	CFDictionaryAddValue(properties, CFSTR(SK_SHADOW_NAME),
			     uctx->shadowIds);
	CFRelease_and_null(uctx->shadowIds);

	SKIndexSetDocumentProperties(skindex->skiref, shadow, properties);

	CFRelease_and_null(properties);
	CFRelease_and_null(shadow);
	return 1;
}

static int sk_update_assign_fragment(struct sk_fts_backend_update_context *uctx,
				     off_t incr)
{
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) uctx->uctx.backend;
	struct sk_fts_index *skindex;
	unsigned int count, i;
	struct stat stbuf;

	/* continue to use same fragment? */
	skindex = array_get_modifiable(&backend->indexes, &count);
	if (uctx->fragment != (unsigned int) -1) {
		if (uctx->fragment >= count)
			i_panic("fts_sk: uctx->fragment=%u >= count=%u",
				uctx->fragment, count);

		if (stat(skindex[uctx->fragment].path, &stbuf) < 0) {
			i_error("fts_sk: stat(%s): %m",
				skindex[uctx->fragment].path);
			return -1;
		}
		if (stbuf.st_size + incr < SK_FRAGMENT_THRESHOLD_SIZE)
			return 1;
	}

	/* use the first fragment smaller than the threshold */
	for (i = 0; i < count; i++) {
		if (i == uctx->fragment)
			continue;

		if (stat(skindex[i].path, &stbuf) < 0) {
			i_error("fts_sk: stat(%s): %m", skindex[i].path);
			return -1;
		}
		if (stbuf.st_size + incr < SK_FRAGMENT_THRESHOLD_SIZE) {
			uctx->fragment = i;
			return 1;
		}
	}

	/* create a new fragment */
	skindex = array_append_space(&backend->indexes);
	skindex->path = i_strdup_printf("%s-%u",
					(array_idx(&backend->indexes, 0))->path,
					count);
	if (sk_create(backend, count) < 0) {
		i_error("fts_sk: could not create new index fragment %s",
			skindex->path);
		return -1;
	}
	uctx->fragment = count;

	/* register the new fragment in the meta properties */
	if (sk_save_meta_values(backend) < 0) {
		sk_close(backend);
		(void) unlink(skindex->path);
		return -1;
	}

	return 1;
}

static int sk_update_build_commit(struct sk_fts_backend_update_context *uctx)
{
	struct sk_field * const *fieldp;
	off_t size = 0;
	int ret = 1;

	array_foreach(&uctx->fields, fieldp) {
		struct sk_field *field = *fieldp;
		size += field->size;
	}
	if (sk_update_assign_fragment(uctx, size) < 0)
		return -1;

	array_foreach(&uctx->fields, fieldp) {
		struct sk_field *field = *fieldp;
		if (sk_update_field_commit(uctx, field) < 0)
			ret = -1;
	}
	if (sk_update_shadow(uctx) < 0)
		ret = -1;
	return ret;
}

static bool sk_needs_compact(struct sk_fts_backend *backend)
{
	struct sk_meta_values *values = &backend->values;
	CFIndex num_expunged;
	time_t deferred_age;

	if (values->expungedUids == NULL)
		return FALSE;

	/* compact only if there are enough expunges to make it worthwhile,
	   or if the expunges are getting long in the tooth */
	num_expunged = CFArrayGetCount(values->expungedUids);
	deferred_age = ioloop_time - values->deferred_since;
	if (num_expunged == 0 ||
	    (num_expunged < backend->compact_expunges &&
	     deferred_age < backend->compact_age))
		return FALSE;

	return TRUE;
}

static void sk_shadow_expunge(const struct sk_fts_index *skindex,
			      SKDocumentRef shadow, CFMutableArrayRef targets)
{
	CFDictionaryRef properties;
	CFArrayRef ids;
	CFIndex count, i;

	properties = SKIndexCopyDocumentProperties(skindex->skiref, shadow);
	if (properties == NULL)
		return;

	ids = (CFArrayRef) CFDictionaryGetValue(properties,
						CFSTR(SK_SHADOW_NAME));
	if (ids == NULL) {
		/* may not be shadow properties; ignore */
		CFRelease_and_null(properties);
		return;
	}

	count = CFArrayGetCount(ids);
	for (i = 0; i < count; i++) {
		CFNumberRef num;
		SKDocumentID id;
		SKDocumentRef doc;

		num = CFArrayGetValueAtIndex(ids, i);
		if (!CFNumberGetValue(num, kCFNumberCFIndexType, &id))
			continue;

		doc = SKIndexCopyDocumentForDocumentID(skindex->skiref, id);
		if (doc == NULL)
			continue;

		CFArrayAppendValue(targets, doc);
		CFRelease_and_null(doc);
	}
	CFRelease_and_null(properties);
}

static void sk_expunge_do(struct sk_fts_backend *backend,
			  struct sk_fts_expunge_context *ectx)
{
	enum { SK_MAXDOCS = 5 };
	SKDocumentID results[SK_MAXDOCS];
	CFIndex found = 0, i;

	memset(results, 0, sizeof results);
	ectx->done = !SKSearchFindMatches(ectx->search, SK_MAXDOCS, results,
					  NULL, backend->search_secs, &found);
	for (i = 0; i < found; i++) {
		SKDocumentRef doc;
		CFIndex count;

		doc = SKIndexCopyDocumentForDocumentID(ectx->skindex->skiref,
						       results[i]);
		if (doc == NULL)
			continue;

		/* have (probably) a shadow doc; remove the documents
		   that its properties identify */
		sk_shadow_expunge(ectx->skindex, doc, ectx->targets);

		/* also remove the shadow document itself */
		count = CFArrayGetCount(ectx->targets);
		if (count > 0 && results[i] !=
		    SKIndexGetDocumentID(ectx->skindex->skiref,
			CFArrayGetValueAtIndex(ectx->targets, count - 1)))
			CFArrayAppendValue(ectx->targets, doc);

		CFRelease_and_null(doc);
	}
}

static void sk_update_expunge_defer(struct sk_fts_backend_update_context *uctx,
				    uint32_t uid)
{
	CFNumberRef num;

	if (uctx->pendingExpunges == NULL) {
		uctx->pendingExpunges =
			CFArrayCreateMutable(kCFAllocatorDefault, 0,
					     &kCFTypeArrayCallBacks);
		sk_nonnull(uctx->pendingExpunges,
			   "CFArrayCreateMutable(pendingExpunges)");
	}

	num = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &uid);
	sk_nonnull(num, "CFNumberCreate(uid)");
	CFArrayAppendValue(uctx->pendingExpunges, num);
	CFRelease_and_null(num);
}

static int sk_update_expunge_commit(struct sk_fts_backend_update_context *uctx)
{
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) uctx->uctx.backend;
	struct sk_meta_values *values = &backend->values;

	if (uctx->pendingExpunges == NULL)
		return 0;

	if (values->expungedUids != NULL) {
		CFIndex count = CFArrayGetCount(values->expungedUids);
		if (count > 0) {
			CFArrayAppendArray(uctx->pendingExpunges,
					   values->expungedUids,
					   CFRangeMake(0, count));
		} else {
			/* this marks the beginning of this set of deferred
			   expunges */
			values->deferred_since = ioloop_time;
		}
		CFRelease_and_null(values->expungedUids);
	} else {
		/* this marks the beginning of this set of deferred
		   expunges */
		values->deferred_since = ioloop_time;
	}
	values->expungedUids = uctx->pendingExpunges;
	uctx->pendingExpunges = NULL;

	if (sk_needs_compact(backend))
		fts_enqueue_update(backend->box);

	return 1;
}

static int sk_compact_batch(struct sk_fts_backend *backend, uint32_t *uids,
			    unsigned int uids_count)
{
	const struct sk_fts_index *skindex;
	unsigned int count, i;
	CFMutableStringRef query;
	ARRAY(struct sk_fts_expunge_context) expunges;
	struct sk_fts_expunge_context *ectx;
	bool more;
	int ret = 0;

	/* first find the shadow documents for the uids */
	query = CFStringCreateMutable(kCFAllocatorDefault, 0);
	sk_nonnull(query, "CFStringCreateMutable(query)");
	for (i = 0; i < uids_count; i++) {
		if (uids[i] == (uint32_t) -1)
			continue;
		CFStringAppendFormat(query, NULL, CFSTR("FTS%uSK "), uids[i]);
	}
	if (CFStringGetLength(query) == 0) {
		CFRelease_and_null(query);
		return 0;
	}
	skindex = array_get(&backend->indexes, &count);
	t_array_init(&expunges, count);
	for (i = 0; i < count; i++) {
		SKSearchRef search;

		search = SKSearchCreate(skindex[i].skiref, query,
					kSKSearchOptionNoRelevanceScores |
					kSKSearchOptionSpaceMeansOR);
		if (search == NULL) {
			char qbuf[1024];
			if (!CFStringGetCString(query, qbuf, sizeof qbuf,
						kCFStringEncodingUTF8))
				strcpy(qbuf, "(unknown)");
			i_error("fts_sk: SKSearchCreate(%s, %s) failed",
				skindex[i].path, qbuf);
			break;
		}

		ectx = array_append_space(&expunges);
		ectx->skindex = &skindex[i];
		ectx->search = search;
		ectx->targets = CFArrayCreateMutable(kCFAllocatorDefault, 0,
						     &kCFTypeArrayCallBacks);
		sk_nonnull(ectx->targets, "CFArrayCreateMutable(targets)");
	}
	CFRelease_and_null(query);
	if (i < count) {
		array_foreach_modifiable(&expunges, ectx) {
			CFRelease_and_null(ectx->search);
			CFRelease_and_null(ectx->targets);
		}
		return -1;
	}

	/* search across all the indexes in parallel */
	ectx = array_get_modifiable(&expunges, &count);
	do {
		more = FALSE;
		for (i = 0; i < count; i++) {
			if (ectx[i].done)
				continue;
			sk_expunge_do(backend, &ectx[i]);
			if (ectx[i].done)
				CFRelease_and_null(ectx[i].search);
			else
				more = TRUE;
		}
	} while (more);

	/* now that the search is done, remove all the targeted documents */
	for (i = 0; i < count; i++) {
		CFMutableArrayRef targets = ectx[i].targets;
		CFIndex tcount = CFArrayGetCount(targets);
		CFIndex t;

		for (t = 0; t < tcount; t++) {
			SKDocumentRef doc = (SKDocumentRef)
				CFArrayGetValueAtIndex(targets, t);
			(void) SKIndexRemoveDocument(skindex[i].skiref, doc);
		}

		CFRelease_and_null(ectx[i].targets);
	}

	return ret;
}

static int sk_compact(struct sk_fts_backend *backend)
{
	struct sk_meta_values *values = &backend->values;
	CFIndex num_expunged, num_compacted;
	const struct sk_fts_index *skindex;
	int ret = 0;

	if (!sk_needs_compact(backend))
		return 0;

	num_expunged = CFArrayGetCount(values->expungedUids);

	/* SK requires flush before search. Technically this should flush
	   before each search, but flushing is slow and we may be expunging a
	   large number of messages. */
	if (sk_flush(backend) < 0)
		return -1;

	num_compacted = 0;
	do {
		CFIndex to_compact, u;
		uint32_t uids[SK_COMPACT_BATCH_SIZE];

		to_compact = num_expunged - num_compacted;
		if (to_compact > SK_COMPACT_BATCH_SIZE)
			to_compact = SK_COMPACT_BATCH_SIZE;
		for (u = 0; u < to_compact; u++) {
			CFNumberRef num = (CFNumberRef)
				CFArrayGetValueAtIndex(values->expungedUids,
						       num_compacted + u);
			if (num == NULL ||
			    !CFNumberGetValue(num, kCFNumberSInt32Type,
					      &uids[u]))
				uids[u] = (uint32_t) -1;
		}

		if (sk_compact_batch(backend, uids, to_compact) < 0)
			ret = -1;

		num_compacted += to_compact;
	} while (num_compacted < num_expunged);

	if (ret >= 0) {
		CFRelease_and_null(values->expungedUids);
		values->deferred_since = 0;
		ret = sk_save_meta_values(backend);
	}

	array_foreach(&backend->indexes, skindex) {
		if (!SKIndexCompact(skindex->skiref)) {
			i_error("fts_sk: SKIndexCompact(%s) failed",
				skindex->path);
			ret = -1;
		}
	}

	return ret;
}

static bool sk_is_locked(struct sk_fts_backend *backend, int lock_type)
{
	i_assert(lock_type == F_RDLCK || lock_type == F_WRLCK);

	if (backend->lock_type == F_UNLCK) {
		i_assert(backend->file_lock == NULL &&
			 backend->dotlock == NULL);
		return FALSE;
	}
	i_assert(backend->file_lock != NULL || backend->dotlock != NULL);

	if (lock_type == F_WRLCK && backend->lock_type != F_WRLCK)
		return FALSE;

	return TRUE;
}

static int sk_lock(struct sk_fts_backend *backend, int lock_type)
{
	int ret;
	const char *lock_path;

	i_assert(!sk_is_locked(backend, F_RDLCK));

	if (backend->lock_method != FILE_LOCK_METHOD_DOTLOCK) {
		lock_path = backend->lock_path;
		if (backend->lock_fd == -1) {
			backend->lock_fd = open(lock_path, O_RDWR | O_CREAT,
						0600);
			if (backend->lock_fd == -1) {
				i_error("fts_sk: creat(%s) failed: %m",
					lock_path);
				return -1;
			}
		}

		ret = file_wait_lock_multiclient(backend->lock_fd, lock_path,
						 lock_type,
						 backend->lock_method,
						 backend->lock_timeout_secs,
						 &backend->file_lock,
						 POINTER_CAST_TO(backend,
								 uintmax_t));
	} else {
		lock_path = (array_idx(&backend->indexes, 0))->path;
		ret = file_dotlock_create_multiclient(&backend->dotlock_set,
						      lock_path, 0,
						      &backend->dotlock,
						      POINTER_CAST_TO(backend,
								      uintmax_t));

	}
	if (ret == 0) {
		i_warning("fts_sk: %s: Locking timed out", lock_path);
		return 0;
	}
	if (ret < 0)
		return -1;

	backend->lock_type = lock_type;
	return 1;
}

static void sk_unlock(struct sk_fts_backend *backend)
{
	if (backend->file_lock != NULL)
		file_unlock_multiclient(&backend->file_lock,
					POINTER_CAST_TO(backend, uintmax_t));

	if (backend->dotlock != NULL)
		(void) file_dotlock_delete_multiclient(&backend->dotlock,
						       POINTER_CAST_TO(backend,
								       uintmax_t));

	backend->lock_type = F_UNLCK;
}

static int sk_lock_and_open(struct sk_fts_backend *backend, int lock_type,
			    bool meta_only)
{
	int ret = -1;

	i_assert(lock_type == F_RDLCK || lock_type == F_WRLCK);
	if (lock_type == F_RDLCK) {
		if (sk_lock(backend, F_RDLCK) <= 0)
			return -1;

		ret = sk_open(backend, FALSE, meta_only);
		if (ret <= 0)
			sk_unlock(backend);
		if (ret == 0)
			return -1;
	}
	if (ret < 0) {
		if (sk_lock(backend, F_WRLCK) <= 0)
			return -1;

		ret = sk_open_or_create(backend, meta_only);
	}
	if (ret <= 0) {
		sk_unlock(backend);
		return -1;
	}

	return 1;
}

static void sk_unset_box(struct sk_fts_backend *backend)
{
	struct sk_fts_index *skindex;

	/* just in case */
	sk_close(backend);
	sk_unlock(backend);

	array_foreach_modifiable(&backend->indexes, skindex) {
		i_free(skindex->path);
		if (skindex->meta_doc != NULL)
			CFRelease_and_null(skindex->meta_doc);
	}
	array_clear(&backend->indexes);

	if (backend->lock_fd != -1) {
		if (close(backend->lock_fd) < 0)
			i_error("fts_sk: close(%s) failed: %m",
				backend->lock_path);
		backend->lock_fd = -1;
	}
	if (backend->lock_path != NULL)
		i_free(backend->lock_path);
	if (backend->index_path != NULL)
		i_free(backend->index_path);

	backend->box = NULL;
}

static void sk_set_box(struct sk_fts_backend *backend, struct mailbox *box)
{
	const struct mailbox_permissions *perm;
	struct mail_storage *storage;
	struct mailbox_status status;
	const char *path;
	struct sk_fts_index *skindex;
	CFURLRef meta_url;

	if (backend->box == box)
		return;
	sk_unset_box(backend);
	if (box == NULL)
		return;
	backend->box = box;

	perm = mailbox_get_permissions(box);
	backend->create_mode = perm->file_create_mode;
	backend->create_gid = perm->file_create_gid;

	storage = mailbox_get_storage(box);
	if (mailbox_get_path_to(box, MAILBOX_LIST_PATH_TYPE_INDEX, &path) <= 0)
		i_unreached();	/* fts already checked this */

	backend->index_path = i_strdup(path);
	backend->lock_path = i_strconcat(path, "/"SK_LOCK_FILE_NAME, NULL);
	backend->lock_method = storage->set->parsed_lock_method;
	if (backend->lock_method == FILE_LOCK_METHOD_FCNTL) {
		/* POSIX requires that *all* of a process's fcntl locks on a
		   file are removed when the process closes *any* descriptor
		   for that file.  This breaks multiclient locking, so instead
		   use flock locks.  See fcntl(2). */
		backend->lock_method = FILE_LOCK_METHOD_FLOCK;
	}
	backend->dotlock_set.stale_timeout = SK_DOTLOCK_STALE_TIMEOUT_SECS;
	backend->dotlock_set.use_excl_lock = storage->set->dotlock_use_excl;
	backend->dotlock_set.nfs_flush = storage->set->mail_nfs_index;
	backend->lock_type = F_UNLCK;

	mailbox_get_status(box, STATUS_UIDVALIDITY, &status);
	backend->uidvalidity = status.uidvalidity;

	backend->debug = storage->set->mail_debug;

	skindex = array_append_space(&backend->indexes);
	skindex->path = i_strconcat(path, "/"SK_FILE_NAME, NULL);
	meta_url = CFURLCreateWithString(kCFAllocatorDefault,
					 CFSTR(SK_META_URL), NULL);
	sk_nonnull(meta_url, "CFURLCreateWithString(meta_url)");
	skindex->meta_doc = SKDocumentCreateWithURL(meta_url);
	sk_nonnull(skindex->meta_doc, "SKDocumentCreateWithURL(meta_url)");
	CFRelease_and_null(meta_url);

	backend->dotlock_set.timeout = backend->lock_timeout_secs;
}

static struct fts_backend *fts_backend_sk_alloc(void)
{
	struct sk_fts_backend *backend;

	backend = i_new(struct sk_fts_backend, 1);
	backend->backend = fts_backend_sk;
	return &backend->backend;
}

static int fts_backend_sk_init(struct fts_backend *_backend,
			       const char **error_r)
{
	struct sk_fts_backend *backend = (struct sk_fts_backend *) _backend;
	const char *env;

	backend->compact_age = SK_COMPACT_AGE;
	backend->compact_expunges = SK_COMPACT_EXPUNGES;
	backend->lock_timeout_secs = SK_LOCK_TIMEOUT_SECS;
	backend->default_min_term_length = SK_DEFAULT_MIN_TERM_LENGTH;
	backend->search_secs = SK_SEARCH_SECS;
	backend->spill_dir = i_strdup(SK_DEFAULT_SPILL_DIR);
	backend->lock_fd = -1;
	i_array_init(&backend->indexes, 4);

	env = mail_user_plugin_getenv(_backend->ns->user, "fts_sk");
	if (env == NULL)
		return 0;
	return sk_set_options(backend, env, error_r);
}

static void fts_backend_sk_deinit(struct fts_backend *_backend)
{
	struct sk_fts_backend *backend = (struct sk_fts_backend *) _backend;

	/* just in case */
	sk_unset_box(backend);

	i_free(backend->spill_dir);
	array_free(&backend->indexes);
	i_free(backend);
}

static int fts_backend_sk_get_last_uid(struct fts_backend *_backend,
				       struct mailbox *box,
				       uint32_t *last_uid_r)
{
	struct sk_fts_backend *backend = (struct sk_fts_backend *) _backend;

	sk_set_box(backend, box);
	if (sk_lock_and_open(backend, F_RDLCK, TRUE) < 0) {
		sk_unset_box(backend);
		return -1;
	}

	*last_uid_r = backend->values.last_uid;

	sk_unset_box(backend);
	return 0;
}

static struct fts_backend_update_context *
fts_backend_sk_update_init(struct fts_backend *_backend)
{
	struct sk_fts_backend_update_context *uctx;

	uctx = i_new(struct sk_fts_backend_update_context, 1);
	uctx->uctx.backend = _backend;
	i_array_init(&uctx->fields, 16);
	uctx->fragment = (unsigned int) -1;
	return &uctx->uctx;
}

static int
fts_backend_sk_update_deinit(struct fts_backend_update_context *_uctx)
{
	struct sk_fts_backend_update_context *uctx =
		(struct sk_fts_backend_update_context *)_uctx;
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) _uctx->backend;
	struct sk_field * const *fieldp;
	int ret = 0;

	if (backend->box != NULL) {
		i_assert(sk_is_locked(backend, F_WRLCK));

		if (sk_update_build_commit(uctx) < 0)
			ret = -1;
		if (sk_update_expunge_commit(uctx) < 0)
			ret = -1;
		if (sk_save_meta_values(backend) < 0)
			ret = -1;
		if (sk_flush(backend) < 0)
			ret = -1;

		sk_unset_box(backend);
	}

	if (uctx->shadowIds != NULL)
		CFRelease_and_null(uctx->shadowIds);
	if (uctx->pendingExpunges != NULL)
		CFRelease_and_null(uctx->pendingExpunges);

	array_foreach(&uctx->fields, fieldp) {
		struct sk_field *field = *fieldp;
		sk_field_free(&field);
	}
	array_free(&uctx->fields);

	i_free(uctx);
	return ret;
}

static void
fts_backend_sk_update_set_mailbox(struct fts_backend_update_context *_uctx,
				  struct mailbox *box)
{
	struct sk_fts_backend_update_context *uctx =
		(struct sk_fts_backend_update_context *) _uctx;
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) uctx->uctx.backend;

	if (uctx->uid != 0 && sk_is_open(backend)) {
		i_assert(sk_is_locked(backend, F_WRLCK));

		backend->values.last_uid = uctx->uid;
		(void) sk_update_build_commit(uctx);
	}
	uctx->uid = 0;

	if (sk_is_open(backend)) {
		(void) sk_update_expunge_commit(uctx);
		(void) sk_save_meta_values(backend);
		(void) sk_flush(backend);
	}

	sk_set_box(backend, box);
	if (box == NULL)
		return;

	if (sk_lock_and_open(backend, F_WRLCK, FALSE) < 0) {
		sk_unset_box(backend);
		return;
	}
}

static void
fts_backend_sk_update_expunge(struct fts_backend_update_context *_uctx,
			      uint32_t uid)
{
	struct sk_fts_backend_update_context *uctx =
		(struct sk_fts_backend_update_context *) _uctx;
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) _uctx->backend;

	if (sk_is_open(backend)) {
		i_assert(sk_is_locked(backend, F_WRLCK));
		sk_update_expunge_defer(uctx, uid);
	}
}

static bool
fts_backend_sk_update_set_build_key(struct fts_backend_update_context *_uctx,
				    const struct fts_backend_build_key *key)
{
	struct sk_fts_backend_update_context *uctx =
		(struct sk_fts_backend_update_context *) _uctx;
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) _uctx->backend;

	if (!sk_is_open(backend))
		return FALSE;
	i_assert(sk_is_locked(backend, F_WRLCK));

	if (key->uid != uctx->uid) {
		(void) sk_update_build_commit(uctx);
		uctx->uid = key->uid;
	}

	switch (key->type) {
	case FTS_BACKEND_BUILD_KEY_HDR:
		if (backend->values.version > 1 &&
		    fts_header_want_indexed(key->hdr_name))
			uctx->cur_field2 =
				sk_update_field_get(uctx, key->hdr_name,
						    "text/plain");
		/* fall through */
	case FTS_BACKEND_BUILD_KEY_MIME_HDR:
		uctx->cur_field = sk_update_field_get(uctx, sk_headers,
						      "text/plain");

		/* create a special string to allow searching for the existence
		   of a header */
		if (backend->values.version > 1) {
			string_t *exists = t_str_new(128);
			str_printfa(exists, "%s%s\n", SK_HEADER_EXISTS_PREFIX,
				    key->hdr_name);
			sk_update_field_append(uctx, uctx->cur_field,
					       str_data(exists),
					       str_len(exists));
		}
		break;
	case FTS_BACKEND_BUILD_KEY_BODY_PART:
	case FTS_BACKEND_BUILD_KEY_BODY_PART_BINARY:
		if (strncasecmp(key->body_content_type, "multipart/", 10) == 0)
			return FALSE;	/* ignore multipart preamble and epilogue */

		uctx->cur_field = sk_update_field_get(uctx, sk_body,
						      key->body_content_type);
		break;
	}
	return TRUE;
}

static void
fts_backend_sk_update_unset_build_key(struct fts_backend_update_context *_uctx)
{
	struct sk_fts_backend_update_context *uctx =
		(struct sk_fts_backend_update_context *) _uctx;

	if (uctx->cur_field == NULL)
		return;

	if (uctx->cur_field->is_text) {
		/* There can be multiple duplicate keys (duplicate
		   header lines, multiple MIME body parts).  Make sure
		   they are separated by whitespace. */
		sk_update_field_append(uctx, uctx->cur_field, "\n", 1);
	} else {
		/* Non-text field so neither mangle it with a \n nor
		   ever append to it again.  If another build key with
		   the same content type comes along it will get its own
		   new field.  Binary attachments must be indexed
		   separately from one another even if they have the
		   same content type.  */
		uctx->cur_field->sealed = TRUE;
	}
	uctx->cur_field = NULL;
	if (uctx->cur_field2 != NULL) {
		sk_update_field_append(uctx, uctx->cur_field2, "\n", 1);
		uctx->cur_field2 = NULL;
	}
}

static int
fts_backend_sk_update_build_more(struct fts_backend_update_context *_uctx,
				 const unsigned char *data, size_t size)
{
	struct sk_fts_backend_update_context *uctx =
		(struct sk_fts_backend_update_context *) _uctx;
	struct sk_fts_backend *backend =
		(struct sk_fts_backend *) _uctx->backend;
	int ret = 0;

	if (!sk_is_open(backend))
		return -1;
	i_assert(sk_is_locked(backend, F_WRLCK));

	if (sk_update_field_append(uctx, uctx->cur_field, data, size) < 0)
		ret = -1;
	if (uctx->cur_field2 != NULL &&
	    sk_update_field_append(uctx, uctx->cur_field2, data, size) < 0)
		ret = -1;
	return ret;
}


static int fts_backend_sk_refresh(struct fts_backend *_backend ATTR_UNUSED)
{
	return 0;
}

static int fts_backend_sk_rescan(struct fts_backend *_backend)
{
	struct sk_fts_backend *backend = (struct sk_fts_backend *) _backend;
	struct mailbox_list_iterate_context *iter;
	const struct mailbox_info *info;
	struct mailbox *box;
	int ret = 0;

	iter = mailbox_list_iter_init(backend->backend.ns->list, "*",
				      MAILBOX_LIST_ITER_SKIP_ALIASES |
				      MAILBOX_LIST_ITER_NO_AUTO_BOXES);
	while ((info = mailbox_list_iter_next(iter)) != NULL) {
		if ((info->flags &
		     (MAILBOX_NONEXISTENT | MAILBOX_NOSELECT)) != 0)
			continue;

		box = mailbox_alloc(info->ns->list, info->vname, 0);
		if (mailbox_open(box) == 0) {
			sk_set_box(backend, box);
			/* delete the indexes and enqueue a rebuild */
			if (sk_delete(backend) < 0)
				ret = -1;
			sk_unset_box(backend);
			fts_enqueue_update(box);
		}
		mailbox_free(&box);
	}
	if (mailbox_list_iter_deinit(&iter) < 0)
		ret = -1;
	return ret;
}

static int fts_backend_sk_optimize(struct fts_backend *_backend)
{
	struct sk_fts_backend *backend = (struct sk_fts_backend *) _backend;
	struct mailbox_list_iterate_context *iter;
	const struct mailbox_info *info;
	struct mailbox *box;
	int ret;
	bool ok = TRUE;

	iter = mailbox_list_iter_init(backend->backend.ns->list, "*",
				      MAILBOX_LIST_ITER_SKIP_ALIASES |
				      MAILBOX_LIST_ITER_NO_AUTO_BOXES);
	while ((info = mailbox_list_iter_next(iter)) != NULL) {
		if ((info->flags &
		     (MAILBOX_NONEXISTENT | MAILBOX_NOSELECT)) != 0)
			continue;

		box = mailbox_alloc(info->ns->list, info->vname, 0);
		if (mailbox_open(box) == 0) {
			sk_set_box(backend, box);
			if (sk_lock_and_open(backend, F_WRLCK, FALSE) >= 0) {
				ret = sk_compact(backend);
				sk_close(backend);
				if (ret < 0) {
					sk_delete(backend);
					ok = FALSE;
				}
				sk_unlock(backend);
			}
			sk_unset_box(backend);
		}
		mailbox_free(&box);
	}
	if (mailbox_list_iter_deinit(&iter) < 0)
		ok = FALSE;

	return ok ? 0 : -1;
}

static int
fts_backend_sk_lookup(struct fts_backend *_backend, struct mailbox *box,
		      struct mail_search_arg *args, bool and_args,
		      struct fts_result *result)
{
	struct sk_fts_backend *backend = (struct sk_fts_backend *) _backend;
	struct sk_lookup_context *lctx;
	int ret = -1;

	sk_set_box(backend, box);
	if (sk_lock_and_open(backend, F_RDLCK, FALSE) > 0) {
		lctx = sk_lookup_new(backend);
		if (lctx != NULL) {
			ret = sk_lookup(lctx, args, and_args, result);
			sk_lookup_free(&lctx);
		}
	}
	sk_unset_box(backend);
	result->scores_sorted = FALSE;
	return ret;
}


struct fts_backend fts_backend_sk = {
	.name = "sk",
	.flags = FTS_BACKEND_FLAG_BINARY_MIME_PARTS |
		 FTS_BACKEND_FLAG_FUZZY_SEARCH,

	{
		fts_backend_sk_alloc,
		fts_backend_sk_init,
		fts_backend_sk_deinit,
		fts_backend_sk_get_last_uid,
		fts_backend_sk_update_init,
		fts_backend_sk_update_deinit,
		fts_backend_sk_update_set_mailbox,
		fts_backend_sk_update_expunge,
		fts_backend_sk_update_set_build_key,
		fts_backend_sk_update_unset_build_key,
		fts_backend_sk_update_build_more,
		fts_backend_sk_refresh,
		fts_backend_sk_rescan,
		fts_backend_sk_optimize,
		fts_backend_default_can_lookup,
		fts_backend_sk_lookup,
		NULL,		/* lookup_multi */
		NULL		/* lookup_done */
	}
};
