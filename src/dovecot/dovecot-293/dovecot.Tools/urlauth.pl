#!/usr/bin/perl

# Test URLAUTH support in dovecot.

# Copyright (c) 2013 Apple Inc.  All Rights Reserved.
# 
# @APPLE_LICENSE_HEADER_START@
# 
# This file contains Original Code and/or Modifications of Original Code
# as defined in and that are subject to the Apple Public Source License
# Version 2.0 (the 'License').  You may not use this file except in
# compliance with the License.  Please obtain a copy of the License at
# http://www.opensource.apple.com/apsl/ and read it before using this
# file.
# 
# The Original Code and all software distributed under the License are
# distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
# INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.  Please
# see the License for the specific language governing rights and
# limitations under the License.
# 
# @APPLE_LICENSE_HEADER_END@

use strict;
use IO::Socket::INET;
use Getopt::Long;
use IPC::Open3;
use Digest::HMAC_MD5;
use APR::Base64;
use feature 'state';

sub usage
{
	die <<EOT;
Usage: $0 --host imap-server --user name --password pw
Options:
	--anonuser name		username for SASL ANONYMOUS user
	--anonpassword pw	password for anonuser
	--noburl		test submission BURL
	--mailbox box		append messages to this mailbox
	--quiet
	--warn			warn, not die, on policy violations
	--verbose
EOT
}

my %opts;
GetOptions(\%opts,
    'anonuser=s',
    'anonpassword=s',
    'host=s',
    'mailbox=s',
    'noburl',
    'password=s',
    'quiet',
    'user=s',
    'warn',
    'verbose',
) || usage();

usage() unless $opts{host};
if ($opts{host} !~ /\./) {
	print STDERR "Warning: --host $opts{host} is not fully-qualified and probably won't work.\n";
}
$opts{mailbox} = "INBOX" unless defined($opts{mailbox});
usage() unless $opts{user};
usage() unless $opts{password};

$| = 1;

our $smallbody =<<'EOT';
To: user4@server.urlauth.test
Subject: foo
Message-Id: <20100203003946.0864F851F6@server.urlauth.test>
Date: Tue,  2 Feb 2010 18:39:46 -0600 (CST)
From: root@server.urlauth.test (System Administrator)

bar
EOT

our $mediumbody =<<'EOT';
Subject: medium test message
Date: Thu, 03 Sep 2009 21:37:10 -0500 (CDT)
From: <user1@my_relay_test_domain.test>
To: <user1@gl088116.apple.com>
Message-Id: <200909032137100823.172md14mdtoow@gl088116>
Content-type: text/plain

user1

EOT
$mediumbody .= gentext(1007);

our $largebody = largebody();

my ($imappid, $smtppid);
local $SIG{__DIE__} = sub {
	kill(9, $imappid) if defined $imappid;
	kill(9, $smtppid) if defined $smtppid;
};

my $reply;

# try connecting via imaps, imap + starttls, imap, in that order
my $secure = 0;
my ($to_imap, $from_imap);
print "connecting (imaps)...\n" unless $opts{quiet};
my @imapargv = ("/usr/bin/openssl", "s_client",
		"-connect", "$opts{host}:imaps");
push @imapargv, "-quiet" unless $opts{verbose};
$imappid = open3(\*TO_IMAP, \*FROM_IMAP, \*FROM_IMAP, @imapargv);
sub openssl_happy_or_clean_up
{
	my $label = shift;

	if (!defined($imappid)) {
		print "$label: couldn't run openssl: $!\n" if $opts{verbose};
	} else {
		while ($reply = <FROM_IMAP>) {
			print "<OPENSSL< $reply" if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			return 1 if $reply =~ /^\S+ OK /;
			if ($reply =~ /^connect:/i || $reply =~ /errno/) {
				print "$label: $reply\n" if $opts{verbose};
				last;
			}
		}
		if (!defined($reply)) {
			print "$label: EOF\n" if $opts{verbose};
		}
	}

	close(TO_IMAP);
	close(FROM_IMAP);
	if (defined($imappid)) {
		kill(9, $imappid);
		waitpid($imappid, 0);
		undef $imappid;
	}
	return 0;
}
my $connect_method = "$opts{host}:imaps";
if (openssl_happy_or_clean_up($connect_method)) {
	$to_imap = IO::Handle->new_from_fd(*TO_IMAP, "w");
	$from_imap = IO::Handle->new_from_fd(*FROM_IMAP, "r");
	if (!defined($to_imap) || !defined($from_imap)) {
		die "IO::Handle.new_from_fd: $!\n";
	}
	$secure = 1;
} else {
	print "connecting (imap + starttls)...\n" unless $opts{quiet};
	@imapargv = ("/usr/bin/openssl", "s_client",
		     "-connect", "$opts{host}:imap", "-starttls", "imap");
	push @imapargv, "-quiet" unless $opts{verbose};
	$imappid = open3(\*TO_IMAP, \*FROM_IMAP, \*FROM_IMAP, @imapargv);
	$connect_method = "$opts{host}:imap + starttls";
	if (openssl_happy_or_clean_up($connect_method)) {
		$to_imap = IO::Handle->new_from_fd(*TO_IMAP, "w");
		$from_imap = IO::Handle->new_from_fd(*FROM_IMAP, "r");
		if (!defined($to_imap) || !defined($from_imap)) {
			die "IO::Handle.new_from_fd: $!\n";
		}
		$secure = 1;
	} else {
		print "connecting (imap)...\n" unless $opts{quiet};
		$to_imap = IO::Socket::INET->new(
		    PeerAddr	=> $opts{host},
		    PeerPort	=> 'imap(143)',
		    Proto	=> 'tcp',
		    Type	=> SOCK_STREAM,
		    Timeout	=> 30,
		);
		$from_imap = $to_imap;
		if (!defined($to_imap) || !defined($from_imap)) {
			die "IO::Socket::INET.new: $!\n";
		}

		$reply = $from_imap->getline();
		die "I/O error\n" if $from_imap->error;
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply !~ /\* OK (\[.*\] )?Dovecot.* ready\./) {
			die "Bad greeting: <$reply>\n";
		}
	}
}
$to_imap->autoflush(1);

print "capability...\n" unless $opts{quiet};
my $tag = "a";
send_data("$tag capability\r\n");
flush();
my $imap_auth_plain = 0;
my $imap_auth_cram_md5 = 0;
my $imap_auth_anonymous = 0;
my $imap_auth_x_plain_submit = 0;
while ($reply = $from_imap->getline()) {
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^$tag /) {
		if ($reply !~ /$tag OK (\[.*\])?/) {
			die "Capability failed: <$reply>\n";
		}
		last;
	}
	$imap_auth_plain = 1 if $reply =~ /CAPABILITY.*AUTH=PLAIN/i;
	$imap_auth_cram_md5 = 1 if $reply =~ /CAPABILITY.*AUTH=CRAM-MD5/i;
	$imap_auth_anonymous = 1 if $reply =~ /CAPABILITY.*AUTH=ANONYMOUS/i;
	$imap_auth_x_plain_submit = 1 if $reply =~ /CAPABILITY.*AUTH=X-PLAIN-SUBMIT/i;
}
die "I/O error\n" if $from_imap->error;
if (!$imap_auth_plain && !$imap_auth_cram_md5) {
	die "$opts{host} supports neither PLAIN nor CRAM-MD5 auth so I don't know how to log in.\n";
}
if (!$imap_auth_anonymous) {
	warn "$opts{host} does not support ANONYMOUS auth (which is normal) so I can't fully test URLAUTH authuser.\n";
} elsif (!defined($opts{anonuser}) || !defined($opts{anonpassword})) {
	die "$opts{host} supports ANONYMOUS auth (cool!) but --anonuser/--anonpassword not specified\n";
}

print "logging in $opts{user}...\n" unless $opts{quiet};
my $imap_auth = $imap_auth_cram_md5 ? "CRAM-MD5" : "PLAIN";
++$tag;
send_data("$tag authenticate $imap_auth\r\n");
flush();
$reply = $from_imap->getline();
die "I/O error\n" if $from_imap->error;
printS($reply) if $opts{verbose};
$reply =~ s/[\r\n]+$//;
if ($reply !~ /^\+/) {
	die "Authenticate failed: <$reply>\n";
}
if ($imap_auth_cram_md5) {
	my ($challenge) = ($reply =~ /^\+ (.*)/);
	$challenge = APR::Base64::decode($challenge);
	print "Decoded challenge: $challenge\n" if $opts{verbose};
	my $digest = Digest::HMAC_MD5::hmac_md5_hex($challenge, $opts{password});
	$imap_auth = APR::Base64::encode("$opts{user} $digest");
} else {
	$imap_auth = APR::Base64::encode("\0$opts{user}\0$opts{password}");
}
$imap_auth .= "\r\n";
send_data($imap_auth);
flush();
my $capability;
while ($reply = $from_imap->getline()) {
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^$tag /) {
		if ($reply !~ /$tag OK /) {
			die "Login failed: <$reply>\n";
		}
		$capability = $reply unless defined $capability;
		last;
	} elsif ($reply =~ /CAPABILITY/) {
		$capability = $reply;
	}
}
die "I/O error\n" if $from_imap->error;
if ($capability =~ /\WURLAUTH(\W|$)/) {
	if (!$secure && $capability !~ /\WSTARTTLS(\W|$)/) {
		policy("$opts{host} has URLAUTH without STARTTLS, oops: <$capability>\n");
		if (!$opts{noburl}) {
			die "SMTP server won't work without STARTTLS; try --noburl\n";
		}
	} # else OK
} else {
	my $why = "";
	$why = " probably because SSL/TLS is disabled" if $capability !~ /\WSTARTTLS(\W|$)/;
	die "No URLAUTH advertised in capability$why: <$capability>\n";
}

print "testing rejection of non-existent mailbox...\n" unless $opts{quiet};
++$tag;
send_data("$tag genurlauth imap://$opts{user}\@$opts{host}/this-mailbox-does-not-exist/;uid=1;urlauth=user+$opts{user} internal\r\n");
flush();
while ($reply = $from_imap->getline()) {
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^$tag /) {
		if ($reply =~ /$tag OK /) {
			die "Genurlauth succeeded but should have failed: <$reply>\n";
		}
		last;
	} elsif ($reply =~ /^\* GENURLAUTH /i) {
		die "Genurlauth returned url but should not have: <$reply>\n";
	}
}
die "I/O error\n" if $from_imap->error;
++$tag;
send_data("$tag resetkey this-mailbox-does-not-exist\r\n");
flush();
while ($reply = $from_imap->getline()) {
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^$tag /) {
		if ($reply =~ /$tag OK /) {
			die "Resetkey succeeded but should have failed: <$reply>\n";
		}
		last;
	}
}
die "I/O error\n" if $from_imap->error;

print "testing rejection of other user...\n" unless $opts{quiet};
++$tag;
send_data("$tag genurlauth imap://this-user-does-not-exist\@$opts{host}/$opts{mailbox}/;uid=1;urlauth=user+this-user-does-not-exist internal\r\n");
flush();
while ($reply = $from_imap->getline()) {
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^$tag /) {
		if ($reply =~ /$tag OK /) {
			die "Genurlauth succeeded but should have failed: <$reply>\n";
		}
		last;
	} elsif ($reply =~ /^\* GENURLAUTH /i) {
		die "Genurlauth returned url but should not have: <$reply>\n";
	}
}
die "I/O error\n" if $from_imap->error;

my @sizes = ('small', 'medium', 'large');
my $selected = 0;
my ($anonymous_url, $authuser_url, $user_url, $submit_url);
for my $s (1..@sizes) {
	my $msgsize = $sizes[$s - 1];
	no strict 'refs';
	my $msgbody = ${"${msgsize}body"};
	use strict 'refs';
	$msgbody =~ s/\n/\r\n/g;
	my $msgbytes = length $msgbody;

	print "appending a $msgsize message to $opts{mailbox}...\n" unless $opts{quiet};
	++$tag;
	send_data("$tag append $opts{mailbox} {$msgbytes}\r\n");
	flush();
	$reply = $from_imap->getline();
	die "I/O error\n" if $from_imap->error;
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply !~ /^\+/) {
		die "Append failed: <$reply>\n";
	}
	send_data("$msgbody\r\n");
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK /) {
				die "Append failed: <$reply>\n";
			}
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;
	my ($uidvalidity, $uid) = ($reply =~ /\[APPENDUID (\d+) (\d+)\]/);
	die "Append reply missing APPENDUID: <$reply>\n" unless defined $uid;

	my @url_tests = (
	    {
		urlauth	=> "anonymous",
		urlref	=> \$anonymous_url
	    }, {
		urlauth	=> "authuser",
		urlref	=> \$authuser_url
	    }, {
		urlauth	=> "user+$opts{user}",
		urlref	=> \$user_url
	    }
	);
	for my $testref (@url_tests) {
		my %test = %$testref;
		my $urlref = $test{urlref};
		print "genurlauth $test{urlauth}...\n" unless $opts{quiet};
		++$tag;
		send_data("$tag genurlauth imap://$opts{user}\@$opts{host}/$opts{mailbox};uidvalidity=$uidvalidity/;uid=$uid;urlauth=$test{urlauth} internal\r\n");
		flush();
		undef $$urlref;
		while ($reply = $from_imap->getline()) {
			printS($reply) if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			if ($reply =~ /^$tag /) {
				if ($reply !~ /$tag OK /) {
					die "Genurlauth failed: <$reply>\n";
				}
				last;
			} elsif ($reply =~ /^\* GENURLAUTH "(.*)"/i ||
				 $reply =~ /^\* GENURLAUTH (.*)/i) {
				$$urlref = $1;
			}
		}
		die "I/O error\n" if $from_imap->error;
		die "Genurlauth returned no URL\n" unless defined $$urlref;

		print "urlfetch (should succeed)...\n" unless $opts{quiet};
		++$tag;
		send_data("$tag urlfetch $$urlref\r\n");
		flush();
		my $verify = "";
		while ($reply = $from_imap->getline()) {
			printS($reply) if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			if ($reply =~ /^$tag /) {
				if ($reply !~ /$tag OK /) {
					die "Urlfetch failed: <$reply>\n";
				}
				last;
			} elsif ($reply =~ /^\* URLFETCH "(.*)" {(\d+)}/i ||
				 $reply =~ /^\* URLFETCH (.*) {(\d+)}/i) {
				if ($1 ne $$urlref) {
					die "Urlfetch returned wrong url $1, expected $$urlref\n";
				} elsif ($2 != $msgbytes) {
					die "Urlfetch returned wrong size $2, expected $msgbytes\n";
				}
			} else {
				$verify .= "$reply\r\n";
			}
		}
		die "I/O error\n" if $from_imap->error;
		$verify =~ s/\r\n$//;
		die "Urlfetch returned wrong message body:\nGot:\n$verify\nExpected:\n$msgbody\n"
			unless $verify eq $msgbody;
	}

	if (!$selected) {
		print "select...\n" unless $opts{quiet};
		++$tag;
		send_data("$tag select $opts{mailbox}\r\n");
		flush();
		my $verify = "";
		my $has_urlmech = 0;
		while ($reply = $from_imap->getline()) {
			printS($reply) if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			if ($reply =~ /^$tag /) {
				if ($reply !~ /$tag OK /) {
					die "Select failed: <$reply>\n";
				}
				last;
			} elsif ($reply =~ /^\* OK \[UIDVALIDITY (\d+)\]/i) {
				$verify = $1;
			} elsif ($reply =~ /^\* OK \[URLMECH\s.*INTERNAL/i) {
				die "Select returned extra URLMECH\n" if $has_urlmech;
				$has_urlmech = 1;
			}
		}
		die "I/O error\n" if $from_imap->error;
		die "Select returned wrong UIDVALIDITY $verify expected $uidvalidity\n"
			unless $verify == $uidvalidity;

		# dovecot < 2.2 (which offers X-PLAIN-SUBMIT auth) should send URLMECH reply
		# dovecot >= 2.2 (which doesn't) doesn't
		if ($imap_auth_x_plain_submit) {
			die "Select returned no untagged URLMECH\n" unless $has_urlmech;
		}

		$selected = 1;
	}

	print "genurlauth submit+...\n" unless $opts{quiet};
	++$tag;
	send_data("$tag genurlauth imap://$opts{user}\@$opts{host}/$opts{mailbox};uidvalidity=$uidvalidity/;uid=$uid;urlauth=submit+$opts{user} internal\r\n");
	flush();
	undef $submit_url;
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK /) {
				die "Genurlauth failed: <$reply>\n";
			}
			last;
		} elsif ($reply =~ /^\* GENURLAUTH "(.*)"/i ||
			 $reply =~ /^\* GENURLAUTH (.*)/i) {
			$submit_url = $1;
		}
	}
	die "I/O error\n" if $from_imap->error;
	die "Genurlauth returned no URL\n" unless defined $submit_url;

	print "urlfetch (should fail)...\n" unless $opts{quiet};
	++$tag;
	send_data("$tag urlfetch \"$submit_url\"\r\n");
	flush();
	my $failed_as_expected = 0;
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK /) {
				die "Urlfetch failed: <$reply>\n";
			}
			last;
		} elsif ($reply =~ /^\* URLFETCH .* {\d+}/i) {
			die "Urlfetch succeeded but should have failed: <$reply>\n";
		} elsif ($reply =~ /^\* URLFETCH .* NIL/i) {
			$failed_as_expected = 1;
		}
	}
	die "I/O error\n" if $from_imap->error;
	die "Urlfetch returned no untagged response\n" unless $failed_as_expected;

	if (!$opts{noburl}) {
		print "idle...\n" unless $opts{quiet};
		++$tag;
		send_data("$tag idle\r\n");
		flush();
		while ($reply = $from_imap->getline()) {
			printS($reply) if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			if ($reply =~ /^\+ /) {
				last;
			}
		}
		die "I/O error\n" if $from_imap->error;

		print "connecting to submission server...\n" unless $opts{quiet};
		my @smtpargv = ("/usr/bin/openssl", "s_client",
		    "-connect", "$opts{host}:submission", "-starttls", "smtp");
		push @smtpargv, "-quiet" unless $opts{verbose};
		$smtppid = open3(\*TO_SUBMIT, \*FROM_SUBMIT, \*FROM_SUBMIT, @smtpargv)
			or die("/usr/bin/openssl: $!\n");
		while ($reply = <FROM_SUBMIT>) {
			$reply =~ s/[\r\n]+$//;
			print "<SMTP< $reply\n" if $opts{verbose};
			if ($reply =~ /^\d+ /) {
				if ($reply !~ /^2/) {
					die "Bad submission greeting: <$reply>\n";
				}
				last;
			}
		}
		die "I/O error\n" unless defined $reply;

		print "sending ehlo...\n" unless $opts{quiet};
		my $tos = "ehlo foo.bar";
		print ">SMTP> $tos\n" if $opts{verbose};
		print TO_SUBMIT "$tos\r\n";
		my $submit_auth_plain = 0;
		my $submit_auth_cram_md5 = 0;
		my $submit_burl = 0;
		while ($reply = <FROM_SUBMIT>) {
			$reply =~ s/[\r\n]+$//;
			print "<SMTP< $reply\n" if $opts{verbose};
			$submit_auth_plain = 1 if $reply =~ /^\d+.AUTH.*PLAIN/i;
			$submit_auth_cram_md5 = 1 if $reply =~ /^\d+.AUTH.*CRAM-MD5/i;
			if ($reply =~ /^\d+.BURL/) {
				if ($reply !~ /^\d+.BURL$/) {
					die "Unexpected BURL arguments: <$reply>\n";
				}
				$submit_burl = 1;
			}
			if ($reply =~ /^\d+ /) {
				if ($reply !~ /^2/) {
					die "Ehlo failed: <$reply>\n";
				}
				last;
			}
		}
		die "I/O error\n" unless defined $reply;
		if (!$submit_auth_plain && !$submit_auth_cram_md5) {
			die "Submission server supports neither PLAIN nor CRAM-MD5 auth so I don't know how to log in.\n";
		}
		if (!$submit_burl) {
			die "Submission server does not support BURL\n";
		}

		print "logging in $opts{user}...\n" unless $opts{quiet};
		my $submit_auth = $submit_auth_cram_md5 ? "CRAM-MD5" : "PLAIN";
		$tos = "auth $submit_auth";
		print ">SMTP> $tos\n" if $opts{verbose};
		print TO_SUBMIT "$tos\r\n";
		while ($reply = <FROM_SUBMIT>) {
			$reply =~ s/[\r\n]+$//;
			print "<SMTP< $reply\n" if $opts{verbose};
			if ($reply =~ /^\d+/) {
				if ($reply !~ /^3/) {
					die "Auth failed: <$reply>\n";
				}
				last;
			}
		}
		die "I/O error\n" unless defined $reply;
		if ($submit_auth_cram_md5) {
			my ($challenge) = ($reply =~ /^\d+ (.*)/);
			$challenge = APR::Base64::decode($challenge);
			print "Decoded challenge: $challenge\n" if $opts{verbose};
			my $digest = Digest::HMAC_MD5::hmac_md5_hex($challenge, $opts{password});
			$tos = APR::Base64::encode("$opts{user} $digest");
		} else {
			$tos = APR::Base64::encode("\0$opts{user}\0$opts{password}");
		}
		print ">SMTP> $tos\n" if $opts{verbose};
		print TO_SUBMIT "$tos\r\n";
		while ($reply = <FROM_SUBMIT>) {
			$reply =~ s/[\r\n]+$//;
			print "<SMTP< $reply\n" if $opts{verbose};
			if ($reply =~ /^\d+ /) {
				if ($reply !~ /^2/) {
					die "Auth failed: <$reply>\n";
				}
				last;
			}
		}
		die "I/O error\n" unless defined $reply;

		my @burl_tests = (
		    {
			urlauth	=> "anonymous",
			url	=> $anonymous_url,
			cont	=> 1,
			pass	=> 1
		    }, {
			urlauth	=> "authuser",
			url	=> $authuser_url,
			cont	=> 1,
			pass	=> 1
		    }, {
			urlauth	=> "user+$opts{user}",
			url	=> $user_url,
			cont	=> 0,
			pass	=> 0
		    }, {
			urlauth	=> "submit+$opts{user}",
			url	=> "\"$submit_url\"",
			cont	=> 0,
			pass	=> 1
		    }
		);
		for my $testref (@burl_tests) {
			my %test = %$testref;
			print "resending ehlo...\n" unless $opts{quiet};
			$tos = "ehlo foo.bar";
			print ">SMTP> $tos\n" if $opts{verbose};
			print TO_SUBMIT "$tos\r\n";
			my $submit_burl_imap = 0;
			while ($reply = <FROM_SUBMIT>) {
				$reply =~ s/[\r\n]+$//;
				print "<SMTP< $reply\n" if $opts{verbose};
				if ($reply =~ /^\d+.BURL/) {
					if ($reply !~ /^\d+.BURL imap$/) {
						die "Unexpected BURL arguments: <$reply>\n";
					}
					$submit_burl_imap = 1;
				}
				if ($reply =~ /^\d+ /) {
					if ($reply !~ /^2/) {
						die "Ehlo failed: <$reply>\n";
					}
					last;
				}
			}
			die "I/O error\n" unless defined $reply;
			if (!$submit_burl_imap) {
				die "Submission server does not support BURL imap\n";
			}

			print "mail from...\n" unless $opts{quiet};
			$tos = "mail from: fnargle";
			print ">SMTP> $tos\n" if $opts{verbose};
			print TO_SUBMIT "$tos\r\n";
			while ($reply = <FROM_SUBMIT>) {
				$reply =~ s/[\r\n]+$//;
				print "<SMTP< $reply\n" if $opts{verbose};
				if ($reply =~ /^\d+ /) {
					if ($reply !~ /^2/) {
						die "Mail failed: <$reply>\n";
					}
					last;
				}
			}
			die "I/O error\n" unless defined $reply;

			print "rcpt to...\n" unless $opts{quiet};
			$tos = "rcpt to: $opts{user}";
			print ">SMTP> $tos\n" if $opts{verbose};
			print TO_SUBMIT "$tos\r\n";
			while ($reply = <FROM_SUBMIT>) {
				$reply =~ s/[\r\n]+$//;
				print "<SMTP< $reply\n" if $opts{verbose};
				if ($reply =~ /^\d+ /) {
					if ($reply !~ /^2/) {
						die "Rcpt failed: <$reply>\n";
					}
					last;
				}
			}
			die "I/O error\n" unless defined $reply;

			print "burl $test{urlauth} (should " . ($test{pass} ? "succeed" : "fail") . ")...\n" unless $opts{quiet};
			$tos = "burl $test{url}";
			$tos .= " last" unless $test{cont};
			print ">SMTP> $tos\n" if $opts{verbose};
			print TO_SUBMIT "$tos\r\n";
			while ($reply = <FROM_SUBMIT>) {
				$reply =~ s/[\r\n]+$//;
				print "<SMTP< $reply\n" if $opts{verbose};
				if ($reply =~ /^\d+ /) {
					if ($reply =~ /^3/) {
						die "Burl caused continuation: <$reply>\n";
					}
					if ($reply =~ /^2/) {
						if (!$test{pass}) {
							die "Burl succeeded but should have failed: <$reply>\n";
						}
					} else {
						if ($test{pass}) {
							die "Burl failed: <$reply>\n";
						}
					}
					last;
				}
			}
			die "I/O error\n" unless defined $reply;
		}

		print "quit...\n" unless $opts{quiet};
		$tos = "quit";
		print ">SMTP> $tos\n" if $opts{verbose};
		print TO_SUBMIT "$tos\r\n";
		while ($reply = <FROM_SUBMIT>) {
			$reply =~ s/[\r\n]+$//;
			print "<SMTP< $reply\n" if $opts{verbose};
			if ($reply =~ /^\d+ /) {
				if ($reply !~ /^2/) {
					die "Quit failed: <$reply>\n";
				}
				last;
			}
		}
		die "I/O error\n" unless defined $reply;

		close(TO_SUBMIT);
		close(FROM_SUBMIT);
		waitpid($smtppid, 0);
		undef $smtppid;

		print "waiting for message delivery...\n" unless $opts{quiet};
		while ($reply = $from_imap->getline()) {
			printS($reply) if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			if ($reply =~ /^\* \d+ RECENT/i) {
				last;
			}
		}
		die "I/O error\n" if $from_imap->error;
		send_data("done\r\n");
		flush();
		while ($reply = $from_imap->getline()) {
			printS($reply) if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			if ($reply =~ /^$tag /) {
				if ($reply !~ /$tag OK /) {
					die "Idle failed: <$reply>\n";
				}
				last;
			}
		}
		die "I/O error\n" if $from_imap->error;

		print "uid fetch...\n" unless $opts{quiet};
		++$tag;
		++$uid;
		send_data("$tag uid fetch $uid body[]\r\n");
		flush();
		my $verify = "";
		while ($reply = $from_imap->getline()) {
			printS($reply) if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			if ($reply =~ /^$tag /) {
				if ($reply !~ /$tag OK /) {
					die "UID fetch failed: <$reply>\n";
				}
				last;
			} elsif ($reply =~ /^\* \d+ FETCH .*{(\d+)}/i) {
				if ($1 < $msgbytes) {
					die "UID fetch returned wrong size $1, expected >= $msgbytes\n";
				}
			} else {
				$verify .= "$reply\r\n";
			}
		}
		$verify =~ s/\)\r\n$//;
		die "I/O error\n" if $from_imap->error;
		die "UID fetch returned wrong message body:\nGot:\n$verify\nExpected:\n$msgbody\n"
			unless message_fuzzy_equal($verify, $msgbody);
	}

	print "urlfetch forged (should fail)...\n" unless $opts{quiet};
	++$tag;
	my $badurl = $user_url;
	$badurl =~ s,/$opts{mailbox};,/this-mailbox-does-not-exist;,;
	send_data("$tag urlfetch \"$badurl\"\r\n");
	flush();
	$failed_as_expected = 0;
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK /) {
				die "Urlfetch failed: <$reply>\n";
			}
			last;
		} elsif ($reply =~ /^\* URLFETCH .* {\d+}/i) {
			die "Urlfetch succeeded but should have failed: <$reply>\n";
		} elsif ($reply =~ /^\* URLFETCH .* NIL/i) {
			$failed_as_expected = 1;
		}
	}
	die "I/O error\n" if $from_imap->error;
	die "Urlfetch returned no untagged response\n" unless $failed_as_expected;

	print "resetkey...\n" unless $opts{quiet};
	++$tag;
	send_data("$tag resetkey $opts{mailbox}\r\n");
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK /) {
				die "Resetkey failed: <$reply>\n";
			}
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;

	print "urlfetch user+$opts{user} (should fail)...\n" unless $opts{quiet};
	++$tag;
	send_data("$tag urlfetch \"$user_url\"\r\n");
	flush();
	$failed_as_expected = 0;
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK /) {
				die "Urlfetch failed: <$reply>\n";
			}
			last;
		} elsif ($reply =~ /^\* URLFETCH .* {\d+}/i) {
			die "Urlfetch succeeded but should have failed: <$reply>\n";
		} elsif ($reply =~ /^\* URLFETCH .* NIL/i) {
			$failed_as_expected = 1;
		}
	}
	die "I/O error\n" if $from_imap->error;
	die "Urlfetch returned no untagged response\n" unless $failed_as_expected;
}

print "logout...\n" unless $opts{quiet};
++$tag;
send_data("$tag logout\r\n");
flush();
while ($reply = $from_imap->getline()) {
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^$tag /) {
		if ($reply !~ /$tag OK (\[.*\])?/) {
			die "Logout failed: <$reply>\n";
		}
		last;
	}
}
die "I/O error\n" if $from_imap->error;

$to_imap->close();
if (defined($imappid)) {
	$from_imap->close();
	waitpid($imappid, 0);
	undef $imappid;
}

my @reconnect_tests;
if (defined($opts{anonuser}) && defined($opts{anonpassword})) {
	push @reconnect_tests, {
		auth		=> "login",
		user		=> $opts{anonuser},
		password	=> $opts{anonpassword},
		tests		=> [
		    {
			urlauth		=> "anonymous",
			pass_genurlauth	=> $imap_auth_x_plain_submit,
			pass_urlfetch	=> 1
		    }, {
			urlauth		=> "authuser",
			pass_genurlauth	=> $imap_auth_x_plain_submit,
			pass_urlfetch	=> 1
		    }, {
			urlauth		=> "user+$opts{anonuser}",
			pass_genurlauth	=> $imap_auth_x_plain_submit,
			pass_urlfetch	=> 1
		    }, {
			urlauth		=> "submit+$opts{anonuser}",
			pass_genurlauth	=> $imap_auth_x_plain_submit,
			pass_urlfetch	=> 0
		    }
		]
	};
}
if ($imap_auth_anonymous) {
	push @reconnect_tests, {
		auth		=> "anonymous",
		user		=> "urlauth.pl",
		tests		=> [
		    {
			urlauth		=> "anonymous",
			pass_genurlauth	=> $imap_auth_x_plain_submit,
			pass_urlfetch	=> 1
		    }, {
			urlauth		=> "authuser",
			pass_genurlauth	=> $imap_auth_x_plain_submit,
			pass_urlfetch	=> 0
		    }, {
			urlauth		=> "user+$opts{anonuser}",
			pass_genurlauth	=> $imap_auth_x_plain_submit,
			pass_urlfetch	=> 1
		    }, {
			urlauth		=> "submit+$opts{anonuser}",
			pass_genurlauth	=> $imap_auth_x_plain_submit,
			pass_urlfetch	=> 0
		    }
		]
	};
}

for my $reconnect_testref (@reconnect_tests) {
	my %reconnect_test = %$reconnect_testref;
	print "reconnecting...\n" unless $opts{quiet};
	if ($secure) {
		$imappid = open3(\*TO_IMAP, \*FROM_IMAP, \*FROM_IMAP, @imapargv);
		if (openssl_happy_or_clean_up($connect_method)) {
			$to_imap = IO::Handle->new_from_fd(*TO_IMAP, "w");
			$from_imap = IO::Handle->new_from_fd(*FROM_IMAP, "r");
			if (!defined($to_imap) || !defined($from_imap)) {
				die "IO::Handle.new_from_fd: $!\n";
			}
		} else {
			die "can't reconnect via $connect_method\n";
		}
	} else {
		$to_imap = IO::Socket::INET->new(
		    PeerAddr	=> $opts{host},
		    PeerPort	=> 'imap(143)',
		    Proto	=> 'tcp',
		    Type	=> SOCK_STREAM,
		    Timeout	=> 30,
		);
		$from_imap = $to_imap;
		if (!defined($to_imap) || !defined($from_imap)) {
			die "IO::Socket::INET.new: $!\n";
		}

		$reply = $from_imap->getline();
		die "I/O error\n" if $from_imap->error;
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply !~ /\* OK (\[.*\] )?Dovecot.* ready\./) {
			die "Bad greeting: <$reply>\n";
		}
	}
	$to_imap->autoflush(1);

	if ($reconnect_test{auth} eq "anonymous") {
		$imap_auth = "ANONYMOUS";
	} elsif ($imap_auth_cram_md5) {
		$imap_auth = "CRAM-MD5";
	} else {
		$imap_auth = "PLAIN";
	}
	print "authenticate $imap_auth $reconnect_test{user}...\n" unless $opts{quiet};
	++$tag;
	send_data("$tag authenticate $imap_auth\r\n");
	flush();
	$reply = $from_imap->getline();
	die "I/O error\n" if $from_imap->error;
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply !~ /^\+/) {
		die "Authenticate failed: <$reply>\n";
	}
	if ($imap_auth eq "ANONYMOUS") {
		$imap_auth = APR::Base64::encode($reconnect_test{user});
	} elsif ($imap_auth_cram_md5) {
		my ($challenge) = ($reply =~ /^\+ (.*)/);
		$challenge = APR::Base64::decode($challenge);
		print "Decoded challenge: $challenge\n" if $opts{verbose};
		my $digest = Digest::HMAC_MD5::hmac_md5_hex($challenge, $reconnect_test{password});
		$imap_auth = APR::Base64::encode("$reconnect_test{user} $digest");
	} else {
		$imap_auth = APR::Base64::encode("\0$reconnect_test{user}\0$reconnect_test{password}");
	}
	$imap_auth .= "\r\n";
	send_data($imap_auth);
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK (\[.*\] )?Logged in/) {
				die "Login failed: <$reply>\n";
			}
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;

	my $msgbody = $smallbody;
	$msgbody =~ s/\n/\r\n/g;
	my $msgbytes = length $msgbody;

	print "appending a small message to $opts{mailbox}...\n" unless $opts{quiet};
	++$tag;
	send_data("$tag append $opts{mailbox} {$msgbytes}\r\n");
	flush();
	$reply = $from_imap->getline();
	die "I/O error\n" if $from_imap->error;
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply ne "+ OK") {
		die "Append failed: <$reply>\n";
	}
	send_data("$msgbody\r\n");
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK /) {
				die "Append failed: <$reply>\n";
			}
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;
	my ($uidvalidity, $uid) = ($reply =~ /\[APPENDUID (\d+) (\d+)\]/);
	die "Append reply missing APPENDUID: <$reply>\n" unless defined $uid;

	for my $testref (@{$reconnect_test{tests}}) {
		my %test = %$testref;

		print "genurlauth $test{urlauth} (should " . ($test{pass_genurlauth} ? "succeed" : "fail") . ")...\n" unless $opts{quiet};
		++$tag;
		send_data("$tag genurlauth imap://$opts{anonuser}\@$opts{host}/$opts{mailbox}/;uid=$uid;urlauth=$test{urlauth} internal\r\n");
		flush();
		my $url;
		while ($reply = $from_imap->getline()) {
			printS($reply) if $opts{verbose};
			$reply =~ s/[\r\n]+$//;
			if ($reply =~ /^$tag /) {
				if ($reply =~ /$tag OK /) {
					if (!$test{pass_genurlauth}) {
						die "Genurlauth succeeded but should have failed: <$reply>\n";
					}
				} else {
					if ($test{pass_genurlauth}) {
						die "Genurlauth failed: <$reply>\n";
					}
				}
				last;
			} elsif ($reply =~ /^\* GENURLAUTH "(.*)"/i ||
				 $reply =~ /^\* GENURLAUTH (.*)/i) {
				$url = $1;
			}
		}
		die "I/O error\n" if $from_imap->error;
		next unless $test{pass_genurlauth};
		die "Genurlauth returned no URL\n" unless defined $url;

		print "urlfetch (should " . ($test{pass_urlfetch} ? "succeed" : "fail") . ")...\n" unless $opts{quiet};
		++$tag;
		send_data("$tag urlfetch $url\r\n");
		flush();
		if ($test{pass_urlfetch}) {
			my $verify = "";
			while ($reply = $from_imap->getline()) {
				printS($reply) if $opts{verbose};
				$reply =~ s/[\r\n]+$//;
				if ($reply =~ /^$tag /) {
					if ($reply !~ /$tag OK /) {
						die "Urlfetch failed: <$reply>\n";
					}
					last;
				} elsif ($reply =~ /^\* URLFETCH "(.*)" {(\d+)}/i ||
					 $reply =~ /^\* URLFETCH (.*) {(\d+)}/i) {
					if ($1 ne $url) {
						die "Urlfetch returned wrong url $1, expected $url\n";
					} elsif ($2 != $msgbytes) {
						die "Urlfetch returned wrong size $2, expected $msgbytes\n";
					}
				} else {
					$verify .= "$reply\r\n";
				}
			}
			die "I/O error\n" if $from_imap->error;
			$verify =~ s/\r\n$//;
			die "Urlfetch returned wrong message body:\nGot:\n$verify\nExpected:\n$msgbody\n"
				unless $verify eq $msgbody;
		} else {
			my $failed_as_expected = 0;
			while ($reply = $from_imap->getline()) {
				printS($reply) if $opts{verbose};
				$reply =~ s/[\r\n]+$//;
				if ($reply =~ /^$tag /) {
					if ($reply !~ /$tag OK /) {
						die "Urlfetch failed: <$reply>\n";
					}
					last;
				} elsif ($reply =~ /^\* URLFETCH .* {\d+}/i) {
					die "Urlfetch succeeded but should have failed: <$reply>\n";
				} elsif ($reply =~ /^\* URLFETCH .* NIL/i) {
					$failed_as_expected = 1;
				}
			}
			die "I/O error\n" if $from_imap->error;
			die "Urlfetch returned no untagged response\n" unless $failed_as_expected;
		}
	}

	print "logout...\n" unless $opts{quiet};
	++$tag;
	send_data("$tag logout\r\n");
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($reply !~ /$tag OK (\[.*\])?/) {
				die "Logout failed: <$reply>\n";
			}
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;

	$to_imap->close();
	if (defined($imappid)) {
		$from_imap->close();
		waitpid($imappid, 0);
		undef $imappid;
	}
}

print "All tests passed.\n";
exit 0;

sub flush
{
	send_data(undef);
}

sub send_data
{
	my $data = shift;

	my $bufsiz = 1024;
	state $buf = "";

	my $flush;
	if (defined($data)) {
		$buf .= $data;
		$flush = length($buf) >= $bufsiz;
	} else {
		$flush = 1;
	}

	if ($flush && length($buf)) {
		printC($buf) if $opts{verbose};
		$to_imap->print($buf);
		$buf = "";
	}
}

sub printC
{
	my $msg = shift;
	printX("C", $msg);
}

sub printS
{
	printX("S", @_);
}

sub printX
{
	my $tag = shift;
	my $msg = shift;

	state $lastdir = "";
	state $lastmsg = "\n";

	if ($tag eq "C") {
		if ($lastdir ne "C") {
			print "~NO LINE TERMINATOR~\n" if $lastmsg !~ /\n$/;
			print ">"x72 . "\n";
			$lastdir = "C";
		}
	} else {
		if ($lastdir ne "S") {
			print "~NO LINE TERMINATOR~\n" if $lastmsg !~ /\n$/;
			print "<"x72 . "\n";
			$lastdir = "S";
		}
	}
	print $msg;
	$lastmsg = $msg;
}

sub message_fuzzy_equal
{
	my $actual = shift;
	my $expected = shift;

	# SMTP adds/modifies headers; perform fuzzy match
	$actual =~ s/\*\*\*JUNK MAIL\*\*\* //i;
	$actual =~ s/^Return-Path: [^\n]+\n//mgi;
	$expected =~ s/^Return-Path: [^\n]+\n//mgi;
	return 1 if $actual =~ /\Q$expected\E$/;
	return 0;
}

sub policy
{
	if ($opts{warn}) {
		warn @_;
	} else {
		die @_;
	}
}

sub gentext
{
	my $size = shift;
	my $text = "";
	while (length($text) < $size) {
		for (1..7) {
			for (1..9) {
				$text .= chr(int(rand(26)) + 97);	# 'a' - 'z'
			}
			$text .= " ";
		}
		$text .= "\n";
	}
	return substr($text, 0, $size) . "\n";
}

sub largebody
{
	my $body =<<'EOT';
Subject: large test message
Date: Mon, 11 Jan 2010 08:29:05 -0600 (CST)
From: <user34@my_relay_test_domain.test>
To: <user18@server.urlauth.test>
Message-Id: <201001110829050698.xbbg761r13pe@server>
Mime-Version: 1.0
Content-Type: multipart/mixed; boundary="yfyadhxohiczseo"

user18
EOT
	for (2062200, 4028, 504, 1031100, 8056) {
		$body .=<<'EOT';

--yfyadhxohiczseo
Content-Type: text/plain; charset="US-ASCII"
Content-Transfer-Encoding: quoted-printable

EOT
		$body .= gentext($_);
	}
	$body .=<<'EOT';

--yfyadhxohiczseo--
EOT
	# 43784 lines
	return $body;
}
