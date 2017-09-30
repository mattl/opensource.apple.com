#!/usr/bin/perl

# Test CATENATE support in dovecot.
# Use --init to initialize the template files in the mail store.

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
	--appends n	number of APPENDs
	--bufsiz n	output buffer size
	--buftag	tag output buffer flushes
	--init		initialize template messages
	--mailbox box	append messages to this mailbox
	--messages n	messages per MULTIAPPEND
	--parts n	parts per message
	--quiet
	--select box	select this mailbox before appending
	--select-size n	virtual size of message with UID=1 in selected mailbox
	--store path	path to mail store for the user
	--verbose
EOT
}

my %opts;
GetOptions(\%opts,
    'appends=i',
    'bufsiz=i',
    'buftag',
    'host=s',
    'init',
    'mailbox=s',
    'messages=i',
    'parts=i',
    'password=s',
    'quiet',
    'select=s',
    'select-size=i',
    'store=s',
    'user=s',
    'verbose',
) || usage();

$opts{appends} = 1000 unless defined($opts{appends});
usage() unless $opts{host};
$opts{mailbox} = "INBOX" unless defined($opts{mailbox});
usage() unless $opts{user};
usage() unless $opts{password};
if (defined($opts{select}) && !defined($opts{'select-size'})) {
	print STDERR "--select needs --select-size\n";
	usage();
}

my $checksizes = 0;
if (defined($opts{store})) {
	my $myhost = `hostname`;
	chomp $myhost;
	if ($opts{host} eq 'localhost' or $opts{host} eq $myhost) {
		$checksizes = 1;
	} else {
		die "must run on server to check sizes\n";
	}
}

$| = 1;

# These must match the constants in cmd-append.c (dovecot < 2.2).
# Change both copies (cmd-append.c and here) for better testing coverage.
# Not sure if these apply to dovecot >= 2.2.
my $MAX_URL_LITERAL_SIZE = 2048;	# for testing: 20
my $MAX_CATENATE_MSG_SIZE = 4294967295;	# for testing: 8000000
my $MAX_CATENATE_PARTS = 50;		# for testing: 6

my $smallbody =<<'EOT';
Subject: small test message
Date: Thu, 03 Sep 2009 21:37:10 -0500 (CDT)
From: <user1@my_relay_test_domain.test>
To: <user1@server.catenate.test>
Message-Id: <200909032137100823.172md14mdtoow@gl088116>
Content-type: text/plain

user1

EOT
$smallbody .= gentext(1007);
$smallbody =~ s/\n/\r\n/g;

my $largebody = largebody();
$largebody =~ s/\n/\r\n/g;

my %template_urls = ("/templates/;uid=1" => length($smallbody),
		     "/templates/;uid=2" => length($largebody));

my $imappid;
local $SIG{__DIE__} = sub {
	kill(9, $imappid) if defined $imappid;
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
if (openssl_happy_or_clean_up("$opts{host}:imaps")) {
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
	if (openssl_happy_or_clean_up("$opts{host}:imap + starttls")) {
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
		if ($reply !~ /\* OK/) {
			die "Bad greeting: <$reply>\n";
		}
	}
}
$to_imap->autoflush(1);

print "capability...\n" unless $opts{quiet};
send_data("c capability\r\n");
flush();
my $imap_auth_plain = 0;
my $imap_auth_cram_md5 = 0;
my $imap_auth_x_plain_submit = 0;
while ($reply = $from_imap->getline()) {
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^c /) {
		if ($reply !~ /c OK (\[.*\])?/) {
			die "Capability failed: <$reply>\n";
		}
		last;
	}
	$imap_auth_plain = 1 if $reply =~ /CAPABILITY.*AUTH=PLAIN/i;
	$imap_auth_cram_md5 = 1 if $reply =~ /CAPABILITY.*AUTH=CRAM-MD5/i;
	$imap_auth_x_plain_submit = 1 if $reply =~ /CAPABILITY.*AUTH=X-PLAIN-SUBMIT/i;
}
die "I/O error\n" if $from_imap->error;
if (!$imap_auth_plain && !$imap_auth_cram_md5) {
	die "$opts{host} supports neither PLAIN nor CRAM-MD5 auth so I don't know how to log in.\n";
}

print "logging in...\n" unless $opts{quiet};
my $imap_auth = $imap_auth_cram_md5 ? "CRAM-MD5" : "PLAIN";
send_data("a authenticate $imap_auth\r\n");
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
	if ($reply =~ /^a /) {
		if ($reply !~ /a OK /) {
			die "Login failed: <$reply>\n";
		}
		$capability = $reply unless defined $capability;
		last;
	} elsif ($reply =~ /CAPABILITY/) {
		$capability = $reply;
	}
}
die "I/O error\n" if $from_imap->error;
die "No CATENATE advertised in capability: <$capability>\n"
	unless $capability =~ /\WCATENATE(\W|$)/;

my $expect_OK;
my $explanation;

if ($opts{init}) {
	print "deleting old templates mailbox...\n" unless $opts{quiet};
	send_data("d delete templates\r\n");
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^d /) {
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;
	print "creating templates mailbox...\n" unless $opts{quiet};
	send_data("c create templates\r\n");
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^c /) {
			if ($reply !~ /^c OK/) {
				die "Create failed: <$reply>\n";
			}
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;

	$opts{mailbox} = "templates";
	$opts{messages} = 1;
	$opts{parts} = -1;
	$opts{text} = $smallbody;
	$expect_OK = 1;
	append(1);
	die if !$expect_OK;
	$opts{text} = $largebody;
	$expect_OK = 1;
	append(2);
	die if !$expect_OK;

	print "logout...\n" unless $opts{quiet};
	send_data("z logout\r\n");
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^z /) {
			if ($reply !~ /z OK (\[.*\])?/) {
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
	print "success\n" unless $opts{quiet};
	exit 0;
}

if (defined($opts{select})) {
	print "selecting $opts{select}...\n" unless $opts{quiet};
	send_data("b select $opts{select}\r\n");
	flush();
	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^b /) {
			if ($reply !~ /b OK (\[.*\] )?Select completed/) {
				die "Select failed: <$reply>\n";
			}
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;
}

my $ok = 1;
for my $append (1..$opts{appends}) {
	$expect_OK = 1;
	undef $explanation;
	if (append($append) < 0) {
		$ok = 0;
		last;
	}
}

my $catch_up = 0;	# with openssl, dies from SIGPIPE
if ($catch_up) {
	print "catching up with server output...\n" unless $opts{quiet};
	eval {
		local $SIG{ALRM} = sub { die "alarm\n"; };
		my $cruft = 0;
		alarm(1);
		while ($reply = $from_imap->getline()) {
			printS($reply);
			$cruft = 1;
			alarm(1);
		}
		alarm(0);
		die "cruft\n" if $cruft;
	};
	if ($ok && $@ =~ /cruft/) {
		die "Unexpected server output.\n";
	}
}

print "sending noop...\n" unless $opts{quiet};
send_data("y noop\r\n");
flush();
while ($reply = $from_imap->getline()) {
	my $orig_reply = $reply;
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^y /) {
		if ($reply =~ /^y OK/) {
			print "Noop succeeded.\n" unless $opts{quiet};
		} else {
			die "Noop failed: <$reply>\n";
		}
		last;
	} elsif (!$catch_up) {
		printS($orig_reply);
	}
}
die "I/O error\n" if $from_imap->error;

print "logout...\n" unless $opts{quiet};
send_data("z logout\r\n");
flush();
while ($reply = $from_imap->getline()) {
	printS($reply) if $opts{verbose};
	$reply =~ s/[\r\n]+$//;
	if ($reply =~ /^z /) {
		if ($reply !~ /z OK (\[.*\])?/) {
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

if ($ok) {
	print "All tests passed.\n";
	exit 0;
} else {
	print "At least one test failed.\n";
	exit 1;
}

sub append
{
	my $append = shift;

	my $tag = "append$append";

	print "$tag...\n" unless $opts{quiet};

	send_data("$tag APPEND $opts{mailbox}");

	my @sizes;
	my $multi = $opts{messages};
	$multi = int(rand(4)) + 1 unless defined $multi;
	for (1..$multi) {
		my $size = 0;
		my $ret = message($tag, \$size);
		return $ret if $ret <= 0;  # aborted message, but keep going
		last if $ret == 2;
		push @sizes, $size;
		# RFC 3502 & 4466 say there's no space or newline here
	}

	send_data("\r\n");
	flush();

	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($reply =~ /^$tag /) {
			if ($expect_OK) {
				if ($reply =~ /$tag OK /) {
					print "Append succeeded\n" unless $opts{quiet};
				} else {
					print STDERR "Append failed but should have succeeded: <$reply>\n";
					if ($reply =~ /can.t open mailbox/i || $reply =~ /mailbox does.?n.t exist/i) {
						print STDERR "Maybe you forgot to --init?\n";
					}
					return -1;
				}
			} else {
				if ($reply =~ /$tag OK /) {
					print STDERR "Append succeeded but should have failed ($explanation): <$reply>\n";
					return -1;
				} else {
					print "Append failed as it should have ($explanation): <$reply>\n" unless $opts{quiet};
					return 0;
				}
			}
			last;
		}
	}
	die "I/O error\n" if $from_imap->error;

	if ($checksizes) {
		print "checking sizes...\n" unless $opts{quiet};

		my $dir = "";
		$dir = "/.$opts{mailbox}" unless $opts{mailbox} =~ /^inbox$/i;
		my $count = @sizes;
		my @messages = `ls -l $opts{store}$dir/new | tail -$count`;
		for (0..$#sizes) {
			if (!defined($messages[$_])) {
				die "message is missing\n";
			}
			my @fields = split(/\s+/, $messages[$_]);
			if ($fields[4] != $sizes[$_]) {
				die "$fields[$#fields]: message size $fields[4], expected $sizes[$_]\n";
			} elsif (!$opts{quiet}) {
				print "$fields[$#fields]: size $fields[4] OK\n";
			}
		}
	}

	return 1;
}

sub message
{
	my $tag = shift;
	my $size_ref = shift;

	if (int(rand(2)) == 0) {
		my $flaglist = " (";
		for (1..int(rand(3) + 1)) {
			my @flags = ("\\Answered", "\\Draft", "\\Flagged",
				     "foobar");
			$flaglist .= $flags[int(rand(@flags))] . " ";
		}
		$flaglist =~ s/ $/)/;

		send_data($flaglist);
	}

	if (int(rand(2)) == 0) {
		my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) =
			localtime(time);
		my @months = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
		my $datetime = sprintf(" \"%2d-%s-%d %02d:%02d:%02d -0600\"",
		    $mday, $months[$mon], $year+1900, $hour, $min, $sec);

		send_data($datetime);
	}

	send_data(" ");

	my $parts = $opts{parts};
	if (!defined($parts)) {
		$parts = int(rand(10)) - 1;
		if (int(rand(50)) == 0) {
			$parts = $MAX_CATENATE_PARTS + 5;
		}
	}
	if ($parts == -1) {
		my @texts = ($smallbody, $largebody, "");
		my $text = $texts[int(rand(@texts))];
		if (defined($opts{text})) {
			# init
			$text = $opts{text};
		} elsif (length($text) > 0) {
			$text = "X-Catenate-Append-Tag: $tag\r\n$text";
		}

		$$size_ref += length($text);
		failif(length($text) == 0, "empty message");
		my $r = int(rand(2));
		if ($r == 0) {
			send_nonsync_literal($text);
		} else {
			my $ret = send_sync_literal($text);
			return $ret if $ret <= 0;
		}
	} else {
		failif($parts == 0, "0 parts");
		send_data("CATENATE (");
		$$size_ref = 0;
		my $deferred_fail = undef;
		for (1..$parts) {
			failif($_ > $MAX_CATENATE_PARTS, "too many parts");
			my $ret = part($tag, $size_ref, \$deferred_fail);
			return $ret if $ret <= 0;
			send_data(" ") if $_ < $parts;
		}
		send_data(")");

		# dovecot < 2.2 (which offers X-PLAIN-SUBMIT auth) should reject empty messages
		# dovecot >= 2.2 (which doesn't) doesn't
		failif($imap_auth_x_plain_submit && $$size_ref == 0, "empty message");

		failif(defined $deferred_fail, $deferred_fail);

		# 0 parts is invalid syntax, so don't MULTIAPPEND after it
		return 2 if $parts == 0;
	}

	return 1;
}

sub part
{
	my $tag = shift;
	my $size_ref = shift;
	my $deferred_fail_ref = shift;

	if (int(rand(2)) == 0) {
		my @good_urls = keys %template_urls;
		my @bad_urls = ("/nonexistent-folder-and-too-long-as-a-literal/;uid=1",
				"/inbox/;uid=99999999",
				";invalid;");
		my @select_urls = ("/;uid=1");
		if (defined($opts{select})) {
			push @good_urls, @select_urls;
		} else {
			push @bad_urls, @select_urls;
		}
		my @urls = (@good_urls, @bad_urls);
		my $url = $urls[int(rand(@urls))];
		my $bad = grep {$_ eq $url} @bad_urls;
		if (!$bad) {
			if (grep {$_ eq $url} @select_urls) {
				$$size_ref += $opts{'select-size'};
			} else {
				$$size_ref += $template_urls{$url};
			}
		}

		send_data("URL ");
		my $r = int(rand(4));
		if ($r == 0) {
			send_data($url);
		} elsif ($r == 1) {
			send_data("\"$url\"");
		} elsif ($r == 2) {
			failif(length($url) > $MAX_URL_LITERAL_SIZE, "url literal too large");
			send_nonsync_literal($url);
		} else {
			failif(length($url) > $MAX_URL_LITERAL_SIZE, "url literal too large");
			my $ret = send_sync_literal($url);
			return $ret if $ret <= 0;
		}

		if ($bad) {
			if ($imap_auth_x_plain_submit) {
				# dovecot < 2.2 (which offers X-PLAIN-SUBMIT auth) rejects immediately after the first bad URL
				failif(1, "bad url");
			} else {
				# dovecot >= 2.2 (which doesn't) rejects after it finishes reading the last contiguous url
				$$deferred_fail_ref = "bad url";
			}
		}
		failif($$size_ref > $MAX_CATENATE_MSG_SIZE, "message too large");
	} else {
		my @texts = ($smallbody, $largebody, "");
		my $text = $texts[int(rand(@texts))];
		if (length($text) > 0) {
			$text = "X-Catenate-Append-Tag: $tag\r\n$text";
		}

		failif(defined $$deferred_fail_ref, $$deferred_fail_ref);

		$$size_ref += length($text);
		failif($$size_ref > $MAX_CATENATE_MSG_SIZE, "message too large");

		send_data("TEXT ");
		my $r = int(rand(2));
		if ($r == 0) {
			send_nonsync_literal($text);
		} else {
			my $ret = send_sync_literal($text);
			return $ret if $ret <= 0;
		}
	}

	return 1;
}

sub flush
{
	send_data(undef);
}

sub send_data
{
	my $data = shift;

	state $bufsiz = undef;
	state $buf = "";

	my $flush;
	if (defined($data)) {
		if (!defined($bufsiz)) {
			$bufsiz = $opts{bufsiz};
			if (!defined($bufsiz)) {
				my $r = int(rand(3));
				if ($r == 0) {
					$bufsiz = 0;
				} elsif ($r == 1) {
					$bufsiz = int(rand(64)) + 1;
				} else {
					$bufsiz = int(rand(4096)) + 1;
				}
			}
		}

		$buf .= $data;
		$flush = length($buf) >= $bufsiz;
	} else {
		$flush = 1;
	}

	if ($flush && length($buf)) {
		printC($buf) if $opts{verbose};
		$to_imap->print($buf);

		undef $bufsiz;
		$buf = "";
	}
}

sub send_nonsync_literal
{
	my $literal = shift;
	my $len = length($literal);

	send_data("{$len+}\r\n$literal");
}

sub send_sync_literal
{
	my $literal = shift;
	my $len = length($literal);

	send_data("{$len}\r\n");
	flush();

	while ($reply = $from_imap->getline()) {
		printS($reply) if $opts{verbose};
		$reply =~ s/[\r\n]+$//;
		if ($expect_OK) {
			if ($reply !~ /^\+/) {
				print STDERR "Append failed but should have succeeded: <$reply>\n";
				if ($reply =~ /can.t open mailbox/i || $reply =~ /mailbox does.?n.t exist/i) {
					print STDERR "Maybe you forgot to --init?\n";
				}
				return -1;
			}
		} else {
			if ($reply =~ /^\+/) {
				print STDERR "Append succeeded but should have failed ($explanation): <$reply>\n";
				exit 1;
			} else {
				print "Append failed as it should have ($explanation): <$reply>\n" unless $opts{quiet};
				return 0;
			}
		}
		last;
	}
	die "I/O error\n" if $from_imap->error;

	send_data($literal);

	return 1;
}

sub printC
{
	my $msg = shift;
	printX("C", $msg);
	print "~FLUSH~" if $opts{buftag};
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

sub failif
{
	my $what = shift;
	my $why = shift;

	if ($what && $expect_OK) {
		$expect_OK = 0;
		$explanation = $why;
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
To: <user18@server.catenate.test>
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
