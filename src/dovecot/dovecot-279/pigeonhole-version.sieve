# Report the installed version of dovecot-pigeonhole (Sieve engine).
# To use:  $ sieve-test -D pigeonhole-version.sieve /dev/null

require "vnd.dovecot.debug";
require "variables";
require "environment";

if environment :matches "name" "*" {
	debug_log "Sieve name is ${1}";
} else {
	debug_log "no Sieve name!";
}
if environment :matches "version" "*" {
	debug_log "Sieve version ${1}";
} else {
	debug_log "no Sieve version!";
}
