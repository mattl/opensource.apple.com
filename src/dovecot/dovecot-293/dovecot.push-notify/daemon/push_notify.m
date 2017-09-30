/*
 * Contains:   Routines Mail Services support for Apple Push Notification Service.
 * Written by: Michael (for addtl writers check SVN comments).
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
 * IMPORTANT NOTE: This file is licensed only for use on Apple-branded
 * computers and is subject to the terms and conditions of the Apple Software
 * License Agreement accompanying the package this file is a part of.
 * You may not port this file to another platform without Apple's written consent.
 * 
 *
 * Note:       When editing this file set Xcode to "Editor uses tabs/width=4".
 *             Any other editor or tab setting may not show this file nicely!
 */

#import "push_notify.h"
#import "APNSNotify.h"
#import "APNSFeedback.h"

#import <asl.h>
#import <sys/stat.h>
#import <sys/param.h>
#import <sys/socket.h>
#import <sys/un.h>

#import <Foundation/Foundation.h>
#import <ServerFoundation/XSTask.h>
#import <OpenDirectory/NSOpenDirectory.h>
#import <DirectoryService/DirServicesConst.h>

int sig_usr1 = 1;
int sig_usr2 = 0;
int got_sighup = 0;
int got_sigterm = 0;

time_t	g_check_feedback_time = 0;

aslclient g_asl_err = NULL;
aslclient g_asl_info = NULL;
aslclient g_asl_debug = NULL;

ODNode	*g_od_search_node = nil;
NSAutoreleasePool *gPool = nil;

// -----------------------------------------------------------------
//	exit_with_error ()

void exit_with_error ( const char *msg, int code )
{
	log_err("%s: Exiting", msg);
	[gPool release];
	exit( code );
} // exit_with_error

// -----------------------------------------------------------------------------
//	open_logs ()

void open_logs ( void )
{
	// get app name
	NSString *app_name = [[[NSProcessInfo processInfo] processName] lastPathComponent];

	// open error log
	g_asl_err = asl_open([app_name UTF8String], "com.apple.push_notify.err", 0);
	if ( !g_asl_err )
		fprintf(stderr, "Could not initialize ASL logging for mail error log.\n" );
	else
		asl_set_filter(g_asl_err, ASL_FILTER_MASK_UPTO(ASL_LEVEL_ERR));

	// open info log
	g_asl_info = asl_open([app_name UTF8String], "com.apple.push_notify.info", 0);
	if ( !g_asl_info )
		fprintf(stderr, "Could not initialize ASL logging for mail info log.\n" );
	else
		asl_set_filter(g_asl_info, ASL_FILTER_MASK_UPTO(ASL_LEVEL_INFO));

	// open debug log
	g_asl_debug = asl_open([app_name UTF8String], "com.apple.push_notify.debug", 0);
	if ( !g_asl_debug )
		fprintf(stderr, "Could not initialize ASL logging for mail debug log.\n" );
	else
		asl_set_filter(g_asl_debug, ASL_FILTER_MASK_UPTO(ASL_LEVEL_DEBUG));
} //open_logs

// -----------------------------------------------------------------------------
//	log_err ()

void log_err ( const char *in_format, ... )
{
	if ( !g_asl_err )
		return;

	va_list args;
	va_start(args, in_format);
	asl_vlog( g_asl_err, NULL, ASL_LEVEL_ERR, in_format, args );
	va_end( args );
} // log_err

// -----------------------------------------------------------------------------
//	log_warning ()

void log_warning ( const char *in_format, ... )
{
	if ( !g_asl_err )
		return;

	va_list args;
	va_start(args, in_format);
	asl_vlog( g_asl_err, NULL, ASL_LEVEL_WARNING, in_format, args );
	va_end( args );
} // log_warning

// -----------------------------------------------------------------------------
//	log_info ()

void log_info ( const char *in_format, ... )
{
	if ( !g_asl_info )
		return;

	va_list args;
	va_start(args, in_format);
	asl_vlog( g_asl_info, NULL, ASL_LEVEL_INFO, in_format, args );
	va_end( args );
} // log_info

// -----------------------------------------------------------------------------
//	log_debug ()

void log_debug ( const char *in_format, ... )
{
	if ( !g_asl_debug )
		return;

	if ( sig_usr1 ) {
		va_list args;
		va_start(args, in_format);
		asl_vlog( g_asl_debug, NULL, ASL_LEVEL_DEBUG, in_format, args );
		va_end( args );
	}
} // log_debug

// -----------------------------------------------------------------
//	set_rlimits ()

int set_rlimits ( void )
{
	struct rlimit limit[1] = {{ 0, 0 }};

	if (getrlimit(RLIMIT_CORE, limit) == -1)
		return -1;

	limit->rlim_cur = 0;

	return(setrlimit(RLIMIT_CORE, limit));
} // set_rlimits

// -----------------------------------------------------------------
//	sighup_handler ()

void sighup_handler( int sig __attribute__((unused)) )
{
	got_sighup = 1;
} // sighup_handler


// -----------------------------------------------------------------
//	sigterm_handler ()

void sigterm_handler ( int sig __attribute__((unused)) )
{
	got_sigterm = 1;
} // sigterm_handler


// -----------------------------------------------------------------
//	sigalrm_handler ()

void sigalrm_handler(int sig __attribute__((unused)))
{
	return;
} // sigalrm_handler


// -----------------------------------------------------------------
//	sigusr1_handler ()

void sigusr1_handler(int sig __attribute__((unused)))
{
	if ( sig_usr1 ) {
		sig_usr1 = 0;
		log_info("verbose logging disabled" );
	} else {
		sig_usr1 = 1;
		log_info("verbose logging enabled" );
	}
} //sigusr1_handler


// -----------------------------------------------------------------
//	sigusr2_handler ()

void sigusr2_handler(int sig __attribute__((unused)))
{
	if ( sig_usr2 )
		sig_usr2 = 0;
	else
		sig_usr2 = 1;
} //sigusr2_handler


// -----------------------------------------------------------------
//	sighandler_setup ()

void sighandler_setup ( void )
{
	struct sigaction action;

	sigemptyset( &action.sa_mask );
	action.sa_flags |= SA_RESTART;
	action.sa_handler = sighup_handler;

	if ( sigaction( SIGHUP, &action, NULL ) < 0 )
		exit_with_error("unable to install signal handler for SIGHUP: %m", EXIT_FAILURE);

	action.sa_handler = sigalrm_handler;
	if ( sigaction( SIGALRM, &action, NULL ) < 0 )
		exit_with_error("unable to install signal handler for SIGALRM: %m", EXIT_FAILURE);

	action.sa_handler = sigusr1_handler;
	if ( sigaction( SIGUSR1, &action, NULL ) < 0 )
		exit_with_error("unable to install signal handler for SIGUSR1: %m", EXIT_FAILURE);

	action.sa_handler = sigusr2_handler;
	if ( sigaction( SIGUSR2, &action, NULL ) < 0 )
		exit_with_error("unable to install signal handler for SIGUSR2: %m", EXIT_FAILURE);

	action.sa_handler = sigterm_handler;
	if ( sigaction( SIGTERM, &action, NULL ) < 0 )
		exit_with_error("unable to install signal handler for SIGTERM: %m", EXIT_FAILURE);

	if ( sigaction( SIGINT, &action, NULL) < 0 )
		exit_with_error("unable to install signal handler for SIGINT: %m", EXIT_FAILURE);

} // sighandler_setup


// -----------------------------------------------------------------
//	verify_map_file ()

void verify_map_file ( void )
{
	NSDictionary *attr = [[NSDictionary dictionaryWithObjectsAndKeys:
						   @"root", NSFileOwnerAccountName,
						   @"mail", NSFileGroupOwnerAccountName,
						   [NSNumber numberWithUnsignedLong: 0640], NSFilePosixPermissions,
						    nil]autorelease];

	if ( ![[NSFileManager defaultManager] fileExistsAtPath: DEVICE_MAPS_PATH] ) {
		NSDictionary *a_dict = [[NSDictionary dictionaryWithObjectsAndKeys: nil] autorelease];
		[a_dict writeToFile: DEVICE_MAPS_PATH atomically: YES];
	}

	[[NSFileManager defaultManager] setAttributes: attr ofItemAtPath: DEVICE_MAPS_PATH error: nil];
} // verify_map_file


// -----------------------------------------------------------------
//	setup_socket_path ()

void setup_socket_path ( const char *in_path )
{
	NSString		*ns_str_path	= [[NSString stringWithUTF8String: in_path]autorelease];
	NSDictionary	*ns_dir_attr	= [[NSDictionary dictionaryWithObjectsAndKeys:
									   @"_dovecot", NSFileOwnerAccountName,
									   @"mail", NSFileGroupOwnerAccountName,
									   [NSNumber numberWithUnsignedLong: 0750], NSFilePosixPermissions,
									   nil]autorelease];

	if ( ![[NSFileManager defaultManager] fileExistsAtPath: ns_str_path isDirectory: nil] )
		[[NSFileManager defaultManager] createDirectoryAtPath: ns_str_path withIntermediateDirectories: YES attributes: ns_dir_attr error: nil];
	else
		[[NSFileManager defaultManager] setAttributes: ns_dir_attr ofItemAtPath: ns_str_path error: nil];
} // setup_socket_path


// -----------------------------------------------------------------
//	get_socket ()

int get_socket ( int in_buff_size )
{
	const char		   *socket_path	= "/var/dovecot/push_notify";
	struct sockaddr_un	sock_addr;

	log_debug("opening socket: %s", socket_path);

	// setup socket directories
	setup_socket_path("/var/dovecot");

	int out_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (out_socket < 0) {
		log_err("open socket: \"%s\" failed", socket_path);
		return( -1 );
	}

	if ( in_buff_size ) {
		int rc = 0;
		int optval = 0;
		socklen_t optlen;
		if ( sig_usr1 ) {
			rc = getsockopt(out_socket, SOL_SOCKET, SO_RCVBUF, (char *)&optval, &optlen);
			if ( !rc )
				log_debug("socket get size: %d", optval );
		}

		optlen = sizeof(in_buff_size);
		rc = setsockopt(out_socket, SOL_SOCKET, SO_RCVBUF, &in_buff_size, optlen);
		if ( !rc )
			log_debug("socket receive buffer size: %d", in_buff_size);
		else
			log_err("setsockopt(SO_RCVBUF) failed: %d", rc);
	}

	// bind it to a local file
	sock_addr.sun_family = AF_UNIX;
	strlcpy(sock_addr.sun_path, socket_path, sizeof(sock_addr.sun_path));
	unlink(sock_addr.sun_path);

	int len = sizeof(sock_addr.sun_family) + strlen(sock_addr.sun_path) + 1;
	int result = bind(out_socket, (struct sockaddr *)&sock_addr, len);
	if (result < 0) {
		log_err("bind() to socket: \"%s\" failed: %m", socket_path );
		return( -1 );
	}

	// setup socket permissions
	chmod( sock_addr.sun_path, 0666 );

	log_debug("socket opened: %d: %s", sock_addr.sun_family, sock_addr.sun_path );

	return( out_socket );
} // get_socket

// -----------------------------------------------------------------
//	od_init ()

int od_init ( void )
{
	int	rc = 0;

	log_debug("initializing open directory session" );

	NSError *nsError = nil;
	g_od_search_node = [ODNode nodeWithSession: [ODSession defaultSession] type: kODNodeTypeAuthentication error: &nsError];
	if ( g_od_search_node == nil ) {
		rc = 1;
		if (nsError != nil)
			log_err("Error: unable to open search node: %s", [[nsError localizedDescription] UTF8String]);
		else
			log_err("Error: unable to open search node");
	}

	log_debug("open directory session initialized successfully" );

	return( rc );
} // od_init

// -----------------------------------------------------------------
//	get_user_guid ()

int get_user_guid ( const char *in_user, char *out_guid )
{
	int			result				= 0;
	NSAutoreleasePool  *my_pool		= [[NSAutoreleasePool alloc] init];

	log_debug("getting GUID for user: %s, from directory", in_user );

	ODQuery *od_query = [ODQuery queryWithNode: g_od_search_node
								forRecordTypes: [NSArray arrayWithObject: @kDSStdRecordTypeUsers]
									 attribute: @kDSNAttrRecordName
									 matchType: kODMatchEqualTo
								   queryValues: [NSString stringWithUTF8String: in_user]
							  returnAttributes: [NSArray arrayWithObject: @kDS1AttrGeneratedUID]
								maximumResults: 1
										 error: nil];
	if ( od_query != nil ) {
		NSArray *nsArray_records = [od_query resultsAllowingPartial: NO error: nil];
		if ( (nsArray_records != nil) && [nsArray_records count] ) {
			ODRecord *od_record = [nsArray_records objectAtIndex: 0];
			if ( od_record != nil ) {
				// get the real name
				NSArray *nsArray_values = [od_record valuesForAttribute: @kDS1AttrGeneratedUID error: nil];
				if ( nsArray_values != nil ) {
					NSString *nsStr_name = [nsArray_values objectAtIndex: 0];

					if ( (nsStr_name != nil) && [nsStr_name length] ) {
						strlcpy( out_guid, [nsStr_name UTF8String], GUID_BUFF_SIZE );
						result = 1;
					}
				}
			}
		}
	}

	if ( result )
		log_debug("GUID: %s, for user: %s, found", out_guid, in_user );
	else
		log_debug("No GUID: for user: %s, found", in_user );

	[my_pool release];

	return( result );
} // get_user_guid

// -----------------------------------------------------------------
//	received_data_callback ()

void received_data_callback (CFSocketRef s, CFSocketCallBackType callbackType, CFDataRef address, const void *in_data, void *in_info)
{
	int					a_socket		= -1;
	msg_data_t			message_data;
	NSMutableDictionary	*user_acct_map	= nil;
	NSMutableDictionary	*device_map_data= nil;
	char				guid[GUID_BUFF_SIZE];

	// read from unix socket
	a_socket = CFSocketGetNative(s);
	NSData *msg_data = (NSData *)in_data;

	ssize_t msg_size = [msg_data length];
	if ( msg_size != sizeof(msg_data_t) ) {
		log_err("Error: received invalid messagege size. %d != %d", (int)sizeof(msg_data_t), (int)msg_size);
		return;
	}

	APNSNotify *apn_conn = (APNSNotify *)in_info;
	if ( msg_size == 0 ) {
		log_err("Error: missing notification context info");
		return;
	}

	// extract message data
	[msg_data getBytes: &message_data length: sizeof(msg_data_t)];

	switch ( message_data.msg[0] ) {
		case '1':
			// create node, user ID only
			// Depricated in 10.7
			log_warning("create node: depricated in OS X Server 10.7");
			break;

		case '2':
			// register from IMAP ID command
			log_debug("register device: user: %s, account-id: %s, device-token: %s", message_data.d1, message_data.d2, message_data.d3);

			if (!strlen(message_data.d1) || !strlen(message_data.d2) || !strlen(message_data.d3)) {
				log_err("Error: missing registration data");
				return;
			}

			device_map_data = [NSMutableDictionary dictionaryWithContentsOfFile: DEVICE_MAPS_PATH];
			if ( !device_map_data ) {
				device_map_data = [[[NSMutableDictionary alloc] init] autorelease];
			}

			user_acct_map = [[NSDictionary dictionaryWithObjectsAndKeys:
							[NSString stringWithUTF8String: message_data.d2], @"aps-account-id",
							[NSString stringWithUTF8String: message_data.d3], @"aps-device-token",
							nil]autorelease];

			if ( get_user_guid(message_data.d1, guid) ) {
				NSString *guid_str = [NSString stringWithUTF8String: guid];

				NSMutableArray *devices = [device_map_data objectForKey: guid_str];
				if ( !devices ) {
					devices = [[[NSMutableArray alloc] init] autorelease];
				} else if ( [devices containsObject: user_acct_map]) {
					return;
				} else {
					NSEnumerator *nsEnum = [devices objectEnumerator];
					NSMutableDictionary *acct_map = nil;
					while ( (acct_map = [nsEnum nextObject]) ) {
						NSString *token = [acct_map objectForKey: @"aps-device-token"];
						NSString *acct_id = [acct_map objectForKey: @"aps-account-id"];
						if ( token && acct_id ) {
							NSString *time_stamp = [acct_map objectForKey: @"time-stamp"];
							if ( time_stamp ) {
								[acct_map removeObjectForKey: @"time-stamp"];
								log_debug("device: %s re-registered", [token UTF8String] );
								[device_map_data writeToFile: DEVICE_MAPS_PATH atomically: YES];
								return;
							}
						}
					}
				}

				// add device
				[devices addObject: user_acct_map];

				[device_map_data setObject: devices forKey: [NSString stringWithUTF8String: guid]];
				[device_map_data writeToFile: DEVICE_MAPS_PATH atomically: YES];
			}
			break;

		case '3':
			// publish/notify to node
			log_debug("send notification: user: %s", message_data.d1);

			if ( get_user_guid(message_data.d1, guid) ) {
				device_map_data = [NSMutableDictionary dictionaryWithContentsOfFile: DEVICE_MAPS_PATH];
				if ( device_map_data ) {
					NSArray *devices = [device_map_data objectForKey: [NSString stringWithUTF8String: guid]];
					if ( devices ) {
						NSEnumerator *nsEnum = [devices objectEnumerator];
						while ( (user_acct_map = [nsEnum nextObject]) ) {
							NSString *token = [user_acct_map objectForKey: @"aps-device-token"];
							NSString *acct_id = [user_acct_map objectForKey: @"aps-account-id"];
							if ( token && acct_id ) {
								NSString *time_stamp = [user_acct_map objectForKey: @"time-stamp"];
								if ( !time_stamp ) {
									log_debug("sending notification to: %s account-id: %s device-token: %s",
											message_data.d1, [acct_id UTF8String], [token UTF8String]);
									NSString *payload = [NSString stringWithFormat: @"{ \"aps\" : { \"account-id\" : \"%@\" } }", acct_id];
									log_debug("notification payload: %s", [payload UTF8String] );
									[apn_conn send_notification: payload to_device: token];
								} else
									log_debug("Warning: device not registered: %s", [token UTF8String]);
							} else
								log_debug("Warning: missing device and/or account id for: %s", guid);
						}
					} else
						log_debug("Warning: no device map found for: %s", guid);
				} else
					log_debug("Warning: no device maps file: %s", [DEVICE_MAPS_PATH UTF8String]);
			} else
				log_debug("Warning: no GUID found for: %s", message_data.d1);

			break;

		default:
			log_err("Error: unknown message type: %c", message_data.msg[0]);
			break;
	}
} // received_data_callback


// -----------------------------------------------------------------
//	main ()

int main ( int argc, char **argv )
{
	int	ch;
	int buff_size = BUFF_SIZE;
	char *value = NULL;

	while ( (ch = getopt(argc, argv, "b:")) != EOF ) {
		switch( ch ) {
			case 'b':
				// buffer size
				value = optarg;
				if ( value ) {
					NSString *value_str = [NSString stringWithCString: value encoding: NSUTF8StringEncoding];
					buff_size = [value_str intValue];
					if ( buff_size < BUFF_SIZE )
						buff_size = BUFF_SIZE;
					else if ( buff_size > MAX_BUF_SIZE )
						buff_size = MAX_BUF_SIZE;
				}
				break;
			default:
				break;
		}
	}

	open_logs();

	// setup the pid file
	char *pidfile = "/var/run/push_notify.pid";
	int pid_fd = open(pidfile, O_CREAT | O_RDWR | O_EXLOCK | O_NONBLOCK, 0644);
	if (pid_fd == -1) {
		fprintf(stderr, "ERROR: can't open pidfile: %m\n");
		exit(EXIT_FAILURE);
	} else {
		char buf[100];
		snprintf(buf, sizeof(buf), "%lu\n", (unsigned long int)getpid());
		if (lseek(pid_fd, 0, SEEK_SET) == -1	||
			ftruncate(pid_fd, 0) == -1		||
			write(pid_fd, buf, strlen(buf)) == -1) {

			fprintf(stderr, "ERROR: unable to write to pid file\n");
			exit(EXIT_FAILURE);
		}
		fsync(pid_fd);
	}

    gPool = [[NSAutoreleasePool alloc] init];

	// begin setup    
	log_info("initializing mail notification services");

	// setrlimit
	set_rlimits();

	// set signal handlers
	sighandler_setup();

	// verify maps file
	verify_map_file();

	// create socket we are going to use for listening
	int a_socket = -1;
	while ( (a_socket = get_socket(buff_size)) < 0 ) {
		sleep(60);
		if (got_sigterm)
			exit(0);
	}

	// initialize open directory
	//	- no need to contiure if OD is failing
	while ( od_init() == 1 ) {
		sleep(60);
		if (got_sigterm)
			exit(0);
	}

	// setup notification & feedback connections
	APNSNotify *apn_conn = [[[APNSNotify alloc] init_with_cert_pref: @"apns:com.apple.mail"] retain];
	APNSFeedback *apn_feedback = [[[APNSFeedback alloc] init_with_connection: apn_conn] retain];

	NSString *topic = [apn_conn topic_name];
	log_info("setting server topic: %s", [topic UTF8String]);

	// set topic in IMAP server to return to client
	XSRunTask(SERVER_ADMIN, [NSArray arrayWithObjects: @"settings", @"mail:imap:aps_topic", @"=", topic, nil], nil, nil, nil, nil);

	// create socket run loop
	CFSocketContext socket_ctx = { 0, apn_conn, NULL, NULL, nil };
	CFSocketRef listenSocket = CFSocketCreateWithNative(NULL, a_socket, kCFSocketDataCallBack, received_data_callback, &socket_ctx);
	CFRunLoopSourceRef rl_source = CFSocketCreateRunLoopSource(NULL, listenSocket, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), rl_source, kCFRunLoopDefaultMode);
	CFRelease(rl_source);

	// begin heavy lifting
	log_info("starting mail notification services");

	// main loop
	for (;;) {
		if ( got_sigterm != 0 ) {
			// Say goodnight, Gracie
			log_info("terminating mail notification services (SIGTERM)");
			close(a_socket);
			exit(EXIT_SUCCESS);
		}

		if ( got_sighup ) {
			// right now this is a do-nothing check
			log_info("SIGHUP received");
			got_sighup = 0;
			g_check_feedback_time = 0;
		}

		if ( time(NULL) > g_check_feedback_time ) {
			[apn_feedback connect];
			g_check_feedback_time = time(NULL) + (3600 * 12);
		}
		CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1, 0);
	}
    return( EXIT_SUCCESS );
}
