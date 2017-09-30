/*
 * Copyright (c) 2008-2011 Apple Inc. All rights reserved.
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

#include "auth-common.h"
#include "passdb.h"

#ifdef PASSDB_OD

#include "str.h"
#include "auth-cache.h"
#include "var-expand.h"
#include "password-scheme.h"
#include "db-od.h"
#include "hex-binary.h"

#include <syslog.h>

#include <CoreFoundation/CFString.h>

#include <OpenDirectory/OpenDirectory.h>
#include <OpenDirectory/OpenDirectoryPriv.h>
#include <DirectoryService/DirServicesTypes.h>

struct od_passdb_module
{
	struct	passdb_module module;
	struct	db_od	*od_data;
};

struct od_cram_auth_request {
	struct auth_request auth_request;

	pool_t pool;

	/* requested: */
	char *challenge;

	/* received: */
	char *username;
	char *response;
	unsigned long maxbuf;
};

enum od_qop_option {
	QOP_AUTH		= 0x01,	/* authenticate */
	QOP_AUTH_INT	= 0x02, /* + integrity protection, not supported yet */
	QOP_AUTH_CONF	= 0x04, /* + encryption, not supported yet */

	QOP_COUNT		= 3
};

struct od_digest_auth_request {
	struct auth_request auth_request;

	pool_t pool;
	unsigned int authenticated:1;

	/* requested: */
	char *nonce;
	enum od_qop_option qop;

	/* received: */
	char *username;
	char *cnonce;
	char *nonce_count;
	char *qop_value;
	char *digest_uri; /* may be NULL */
	char *authzid; /* may be NULL, authorization ID */
	unsigned char response[32];
	unsigned long maxbuf;
	unsigned int nonce_found:1;

	/* final reply: */
	char *rspauth;
};

struct od_apop_auth_request {
	struct auth_request auth_request;

	pool_t pool;

	/* requested: */
	char *challenge;

	/* received: */
	unsigned char digest[16];
};


/* ------------------------------------------------------------------
 *	validate_response ()
 * ------------------------------------------------------------------*/

static int validate_response ( struct auth_request *in_request,
							   const char *in_user,
							   const char *in_chal,
							   const char *in_resp,
							   const char *in_auth_type )
{
	struct passdb_module	 *_module		= in_request->passdb->passdb;
	struct od_passdb_module *module		= (struct od_passdb_module *)_module;
	struct db_od			 *user_info		= module->od_data;

	/* look up the user */
	struct od_user *db_user_info = db_od_user_lookup( in_request, module->od_data, in_request->user, TRUE );
	if ( !db_user_info ) {
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		return( PASSDB_RESULT_USER_UNKNOWN );
	}

	if ( !is_acct_enabled( in_request, db_user_info->acct_state, db_user_info->record_name) ) {
		db_od_user_unref(&db_user_info);
		auth_request_log_error(in_request, "od", "validate response: lookup failed for user: %s", in_request->user );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		return( PASSDB_RESULT_USER_DISABLED );
	}

	CFStringRef cf_str_user = CFStringCreateWithCString( NULL, in_user, kCFStringEncodingUTF8 );
	if ( !cf_str_user ) {
		auth_request_log_error(in_request, "od", "validate response: unable to create CFString for user: %s", in_user);
		db_od_user_unref(&db_user_info);
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		return( PASSDB_RESULT_INTERNAL_FAILURE );
	}

	CFErrorRef cf_err_ref = NULL;
	CFTypeRef cf_type_val[] = { CFSTR(kDSAttributesStandardAll) };
	CFArrayRef cf_arry_attr = CFArrayCreate( NULL, cf_type_val, 1, &kCFTypeArrayCallBacks );
	ODRecordRef od_rec_ref = ODNodeCopyRecord( user_info->od_node_ref, CFSTR(kDSStdRecordTypeUsers), cf_str_user, cf_arry_attr, &cf_err_ref );
	CFRelease( cf_arry_attr );
	if ( !od_rec_ref ) {
		db_od_print_cf_error( in_request, cf_err_ref, "validate response: unable to lookup user record" );
		CFRelease( cf_str_user );
		db_od_user_unref(&db_user_info);
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		return( PASSDB_RESULT_INTERNAL_FAILURE );
	}

	ODAuthenticationType od_auth_type;
	if ( strcmp( in_auth_type, "CRAM-MD5" ) == 0 )
		od_auth_type = kODAuthenticationTypeCRAM_MD5;
	else if ( strcmp( in_auth_type, "DIGEST-MD5" ) == 0 )
		od_auth_type = kODAuthenticationTypeDIGEST_MD5;
	else if ( strcmp( in_auth_type, "APOP" ) == 0 )
		od_auth_type = kODAuthenticationTypeAPOP;
	else {
		CFRelease( cf_str_user );
		CFRelease( od_rec_ref );
		db_od_user_unref(&db_user_info);
		auth_request_log_error(in_request, "od", "validate response: unsupported authentication mechanism: %s", in_auth_type );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		return( PASSDB_RESULT_SCHEME_NOT_AVAILABLE );
	}

	/* add user to auth buffer */
	CFMutableArrayRef auth_arry = CFArrayCreateMutable( NULL, 4, &kCFTypeArrayCallBacks );
	CFArrayInsertValueAtIndex( auth_arry, 0, cf_str_user );
	CFRelease( cf_str_user );

	/* add challenge to auth buffer */
	auth_request_log_debug(in_request, "od", "challenge: %s", in_chal);
	CFStringRef cf_str_chal = CFStringCreateWithCString( NULL, in_chal, kCFStringEncodingUTF8 );
	if ( !cf_str_chal ) {
		CFRelease( od_rec_ref );
		CFRelease( auth_arry );
		auth_request_log_debug(in_request, "od", "validate response: failed to create auth challenge CF string" );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		return( PASSDB_RESULT_INTERNAL_FAILURE );
	}
	CFArrayInsertValueAtIndex( auth_arry, 1, cf_str_chal );
	CFRelease( cf_str_chal );

	/* add response to auth buffer */
	auth_request_log_debug(in_request, "od", "response: %s", in_resp);
	CFStringRef cf_str_resp = CFStringCreateWithCString( NULL, in_resp, kCFStringEncodingUTF8 );
	if ( !cf_str_resp ) {
		CFRelease( od_rec_ref );
		CFRelease( auth_arry );
		auth_request_log_debug(in_request, "od", "validate response: failed to create auth response CF string" );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		return( PASSDB_RESULT_INTERNAL_FAILURE );
	}
	CFArrayInsertValueAtIndex( auth_arry, 2, cf_str_resp );
	CFRelease( cf_str_resp );

	/* add http method to auth buffer */
	if ( strcmp( in_auth_type, "DIGEST-MD5" ) == 0 ) {
		auth_request_log_debug(in_request, "od", "method: AUTHENTICATE");
		CFStringRef cf_str_uri = CFStringCreateWithCString( NULL, "AUTHENTICATE", kCFStringEncodingUTF8 );
		if ( !cf_str_uri ) {
			CFRelease( od_rec_ref );
			CFRelease( auth_arry );
			auth_request_log_debug(in_request, "od", "validate response: failed to create auth response CF string" );
			send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
			return( PASSDB_RESULT_INTERNAL_FAILURE );
		}
		CFArrayInsertValueAtIndex( auth_arry, 3, cf_str_uri );
		CFRelease( cf_str_uri );
	}

	/* make the "3 AM" call */
	CFArrayRef cf_arry_resp	= NULL;
	ODContextRef od_context_ref	= NULL;
	bool auth_result = ODRecordVerifyPasswordExtended( od_rec_ref, od_auth_type, auth_arry,
														&cf_arry_resp, &od_context_ref, &cf_err_ref );

	CFRelease( od_rec_ref );
	CFRelease( auth_arry );

	if ( !auth_result ) {
		/* print any OD error */
		if ( cf_err_ref ) {
			db_od_print_cf_error( in_request, cf_err_ref, "validate response: authentication error" );
			CFRelease( cf_err_ref );
		}
		db_od_user_unref( &db_user_info );
		auth_request_log_error(in_request, "od", "authentication failed for user=%s, method=%s", in_user, in_auth_type );
		return( PASSDB_RESULT_PASSWORD_MISMATCH );
	}

	/* if digest-md5 auth, get server response */
	if ( strcmp( in_auth_type, "DIGEST-MD5" ) == 0 ) {
		if ( !cf_arry_resp ) {
			db_od_user_unref( &db_user_info );
			auth_request_log_error(in_request, "od", "DIGEST-MD5 authentication error: missing server response" );
			return( PASSDB_RESULT_INTERNAL_FAILURE );
		}

		CFDataRef cf_data = CFArrayGetValueAtIndex(cf_arry_resp, 0);
		if ( !cf_data || !CFDataGetLength(cf_data) ) {
			db_od_user_unref( &db_user_info );
			auth_request_log_error(in_request, "od", "DIGEST-MD5 authentication error: missing server response" );
			return( PASSDB_RESULT_INTERNAL_FAILURE );
		}

		struct od_digest_auth_request *digest_auth = (struct od_digest_auth_request *)in_request;
		digest_auth->rspauth = p_strconcat(digest_auth->pool, "", CFDataGetBytePtr(cf_data), NULL);
	} else if ( cf_arry_resp )
		CFRelease( cf_arry_resp );

	/* check sacl settings */
	if ( !(db_user_info->acct_state & account_enabled) ) {
		db_od_user_unref( &db_user_info );
		auth_request_log_error(in_request, "od", "mail is not enabled for user=%s (method=%s)", in_user, in_auth_type );
		return( PASSDB_RESULT_USER_DISABLED );
	} else
		push_notify_init( db_user_info );

	/* memory clenup */
	db_od_user_unref( &db_user_info );

	/* all is right with the world */
	auth_request_log_info(in_request, "od", "password verification succeeded for user=%s (method=%s)", in_user, in_auth_type );
	send_server_event( in_request, OD_AUTH_SUCCESS, in_request->user, net_ip2addr(&in_request->remote_ip) );
	return( PASSDB_RESULT_OK );
} /* validate_response */


/* ------------------------------------------------------------------
 *	od_verify_plain ()
 * ------------------------------------------------------------------*/

static void od_verify_plain ( struct auth_request *in_request, const char *in_passwd, verify_plain_callback_t *callback )
{
	struct passdb_module	*_module	= in_request->passdb->passdb;
	struct od_passdb_module	*module		= (struct od_passdb_module *)_module;
	struct db_od			*info		= module->od_data;
	CFErrorRef				cf_err_ref	= NULL;

	struct od_user *db_user_info = db_od_user_lookup( in_request, module->od_data, in_request->user, TRUE );
	if ( !db_user_info ) {
		auth_request_log_error(in_request, "od", "verify plain: lookup failed for user: %s", in_request->user );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		callback( PASSDB_RESULT_USER_UNKNOWN, in_request );
		return;
	}

	if ( !is_acct_enabled( in_request, db_user_info->acct_state, db_user_info->record_name) ) {
		auth_request_log_error(in_request, "od", "verify plain: user account: %s not enabled for mail", db_user_info->record_name );
		db_od_user_unref ( &db_user_info );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		callback( PASSDB_RESULT_USER_DISABLED, in_request );
		return;
	}

	if (in_request->skip_password_check) {
		/* master/submit login */
		i_assert(in_request->master_user != NULL || in_request->submit_user != NULL);
		db_od_user_unref ( &db_user_info );
		send_server_event( in_request, OD_AUTH_SUCCESS, in_request->user, net_ip2addr(&in_request->remote_ip) );
		callback( PASSDB_RESULT_OK, in_request );
		return;
	}

	CFStringRef cf_str_user = CFStringCreateWithCString( NULL, db_user_info->record_name, kCFStringEncodingUTF8 );
	if ( !cf_str_user ) {
		auth_request_log_error(in_request, "od", "verify plain: unable to create CFStringRef for user: %s", db_user_info->record_name );
		db_od_user_unref ( &db_user_info );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		callback( PASSDB_RESULT_INTERNAL_FAILURE, in_request );
		return;
	}

	CFTypeRef cf_type_val[] = { CFSTR(kDSAttributesStandardAll) };
	CFArrayRef cf_arry_attr = CFArrayCreate( NULL, cf_type_val, 1, &kCFTypeArrayCallBacks);
	ODRecordRef od_rec_ref = ODNodeCopyRecord( info->od_node_ref, CFSTR(kDSStdRecordTypeUsers), cf_str_user, cf_arry_attr, &cf_err_ref );
	CFRelease( cf_str_user );
	CFRelease( cf_arry_attr );
	if ( !od_rec_ref ) {
		db_od_print_cf_error( in_request, cf_err_ref, "verify plain: unable to lookup user record" );
		db_od_user_unref ( &db_user_info );
		if ( cf_err_ref )
			CFRelease( cf_err_ref );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		callback( PASSDB_RESULT_USER_UNKNOWN, in_request );
		return;
	}

	CFStringRef cf_str_pwd = CFStringCreateWithCString( NULL, in_passwd, kCFStringEncodingUTF8 );
	if ( !cf_str_pwd ) {
		auth_request_log_error(in_request, "od", "verify plain: unable to create CFStringRef for user: %s", db_user_info->record_name );
		db_od_user_unref ( &db_user_info );
		CFRelease( od_rec_ref );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		callback( PASSDB_RESULT_INTERNAL_FAILURE, in_request );
		return;
	}

	if ( ODRecordVerifyPassword( od_rec_ref, cf_str_pwd, &cf_err_ref ) ) {
		if ( !(db_user_info->acct_state & account_enabled) ) {
			auth_request_log_error(in_request, "od", "verify plain: password mismatch for user: %s", db_user_info->record_name );
			send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
			callback( PASSDB_RESULT_PASSWORD_MISMATCH, in_request );
		} else {
			push_notify_init( db_user_info );
			send_server_event( in_request, OD_AUTH_SUCCESS, in_request->user, net_ip2addr(&in_request->remote_ip) );
			callback( PASSDB_RESULT_OK, in_request );
		}
	} else {
		if ( cf_err_ref ) {
			db_od_print_cf_error( in_request, cf_err_ref, "verify plain: authentication failed" );
			CFRelease( cf_err_ref );
		} else
			auth_request_log_error(in_request, "od", "verify plain: password mismatch for user: %s", db_user_info->record_name );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		callback( PASSDB_RESULT_PASSWORD_MISMATCH, in_request );
	}

	/* clean up allocs */
	db_od_user_unref ( &db_user_info );

	CFRelease( od_rec_ref );
	CFRelease( cf_str_pwd );
} /* od_verify_plain */


/* ------------------------------------------------------------------
 *	od_lookup_credentials ()
 * ------------------------------------------------------------------*/

static void od_lookup_credentials ( struct auth_request *in_request, lookup_credentials_callback_t *callback )
{
	int auth_response = 0;

	if ( in_request->mech == NULL || in_request->mech->mech_name == NULL ) {
		auth_request_log_debug(in_request, "od", "lookup credentials: authentication failed: unknown mechanism" );
		send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
		callback( PASSDB_RESULT_SCHEME_NOT_AVAILABLE, (const unsigned char *)kCRAM_MD5_AuthFailed, strlen( kCRAM_MD5_AuthFailed ), in_request );
		return;
	}

	auth_request_log_debug(in_request, "od", "lookup credentials: auth mech=%s", in_request->mech->mech_name);
	if ( strcmp( in_request->mech->mech_name, "CRAM-MD5" ) == 0 ) {
		struct od_cram_auth_request *md5_auth = (struct od_cram_auth_request *)in_request;

		auth_request_log_debug(in_request, "od", "lookup credentials: username=%s, challenge=%s, response=%s", md5_auth->username, md5_auth->challenge, md5_auth->response );

		auth_response = validate_response( in_request, md5_auth->username, md5_auth->challenge, md5_auth->response, in_request->mech->mech_name );
		if ( auth_response == PASSDB_RESULT_OK ) {
			send_server_event( in_request, OD_AUTH_SUCCESS, in_request->user, net_ip2addr(&in_request->remote_ip) );
			callback( PASSDB_RESULT_OK, (const unsigned char *)kCRAM_MD5_AuthSuccess, strlen( kCRAM_MD5_AuthSuccess ), in_request );
		} else {
			send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
			callback( auth_response, (const unsigned char *)kCRAM_MD5_AuthFailed, strlen( kCRAM_MD5_AuthFailed ), in_request );
		}
		return;
	} else if ( strcmp( in_request->mech->mech_name, "DIGEST-MD5" ) == 0 ) {
		struct od_digest_auth_request *digest_auth = (struct od_digest_auth_request *)in_request;

		/* recreate challange & response */
		const char *chal = t_strconcat( "realm=\"", digest_auth->auth_request.realm, "\",nonce=\"", digest_auth->nonce, "\",qop=\"auth\",algorithm=md5-sess,charset=utf-8", NULL );
		const char *resp = t_strconcat( "charset=utf-8,username=\"", digest_auth->username, "\",realm=\"", digest_auth->auth_request.realm, "\",nonce=\"",
									   		digest_auth->nonce, "\",cnonce=\"", digest_auth->cnonce, "\",nc=", digest_auth->nonce_count, ",qop=", digest_auth->qop_value,
									   		",digest-uri=\"", digest_auth->digest_uri, "\",response=", digest_auth->response, ",algorithm=md5-sess", NULL );

		auth_request_log_debug(in_request, "od", "lookup credentials: user name: %s", digest_auth->username );
		auth_request_log_debug(in_request, "od", "lookup credentials: challenge: %s", chal );
		auth_request_log_debug(in_request, "od", "lookup credentials:  response: %s", resp );

		auth_response = validate_response( in_request, digest_auth->username, chal, resp, in_request->mech->mech_name );
		if ( auth_response == PASSDB_RESULT_OK ) {
			send_server_event( in_request, OD_AUTH_SUCCESS, in_request->user, net_ip2addr(&in_request->remote_ip) );
			callback( PASSDB_RESULT_OK, (const unsigned char *)kDIGEST_MD5_AuthSuccess, strlen( kDIGEST_MD5_AuthSuccess ), in_request );
		} else {
			send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
			callback( auth_response, (const unsigned char *)kDIGEST_MD5_AuthFailed, strlen( kDIGEST_MD5_AuthFailed ), in_request );
		}
		return;
	} else if ( strcmp( in_request->mech->mech_name, "APOP" ) == 0 ) {
		struct od_apop_auth_request *apop_auth = (struct od_apop_auth_request *)in_request;

		const char *apop_response = binary_to_hex( apop_auth->digest, sizeof(apop_auth->digest) );
		auth_request_log_debug(in_request, "od", "lookup credentials: username=%s, challenge=%s, response=%s", in_request->user, apop_auth->challenge, apop_response);

		auth_response = validate_response( in_request, in_request->user, apop_auth->challenge, apop_response, in_request->mech->mech_name );
		if ( auth_response == PASSDB_RESULT_OK ) {
			send_server_event( in_request, OD_AUTH_SUCCESS, in_request->user, net_ip2addr(&in_request->remote_ip) );
			callback( auth_response, (const unsigned char *)kCRAM_APOP_AuthSuccess, strlen( kCRAM_APOP_AuthSuccess ), in_request );
		} else {
			send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
			callback( auth_response, (const unsigned char *)kCRAM_APOP_AuthFailed, strlen( kCRAM_APOP_AuthFailed ), in_request );
		}
		return;
	}

	auth_request_log_debug(in_request, "od", "lookup credentials: unknown user: %s", in_request->user );
	send_server_event( in_request, OD_AUTH_FAILURE, in_request->user, net_ip2addr(&in_request->remote_ip) );
	callback( PASSDB_RESULT_USER_UNKNOWN, NULL, 0, in_request );
} /* od_lookup_credentials */


/* ------------------------------------------------------------------
 *	od_preinit ()
 * ------------------------------------------------------------------*/

static struct passdb_module * od_preinit ( pool_t pool, const char *in_args )
{
	struct od_passdb_module		*module;
	const char *const *str;

	module			= p_new( pool, struct od_passdb_module, 1 );
	module->module.cache_key = OD_USER_CACHE_KEY;
	module->module.default_pass_scheme = OD_DEFAULT_PASS_SCHEME;
	module->module.blocking = FALSE;

	if ( in_args != NULL )
	{
		for ( str = t_strsplit(in_args, " "); *str != NULL; str++ )
		{
			if (!strncmp(*str, "pos_cache_ttl=", 14)) {
				int ttl = atoi(*str + 14);
				if (ttl >= 0)
					od_pos_cache_ttl = ttl;
			} else if (!strncmp(*str, "neg_cache_ttl=", 14)) {
				int ttl = atoi(*str + 14);
				if (ttl >= 0)
					od_neg_cache_ttl = ttl;
			} else if (!strncmp(*str, "use_getpwnam_ext=", 17))
				od_use_getpwnam_ext = strcmp(*str + 17, "yes") == 0 ? TRUE : FALSE;
			else if (!strncmp(*str, "blocking=", 9))
				module->module.blocking = strcmp(*str + 9, "yes") == 0 ? TRUE : FALSE;
		}
	}
	if (global_auth_settings->debug)
		i_debug( "od-debug: passdb-od: args=%s", in_args );

	module->od_data	= db_od_init( FALSE );

	return( &module->module );
} /* od_preinit */


/* ------------------------------------------------------------------
 *	od_init ()
 * ------------------------------------------------------------------*/

static void od_init ( struct passdb_module *_module )
{
	struct od_passdb_module *module = (struct od_passdb_module *)_module;

	db_od_do_init( module->od_data );
} /* od_init */


/* ------------------------------------------------------------------
 *	od_deinit ()
 * ------------------------------------------------------------------*/

static void od_deinit ( struct passdb_module *_module )
{
	struct od_passdb_module *module = (struct od_passdb_module *)_module;

	db_od_unref( &module->od_data );
	close_server_event_port();
} /* od_deinit */


struct passdb_module_interface passdb_od = {
	"od",

	od_preinit,
	od_init,
	od_deinit,

	od_verify_plain,
	od_lookup_credentials,
	NULL
};

#else
struct passdb_module_interface passdb_od = {
	.name = "od"
};
#endif
