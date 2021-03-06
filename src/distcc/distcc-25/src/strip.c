/* -*- c-file-style: "java"; indent-tabs-mode: nil; fill-column: 78; -*-
 * 
 * distcc -- A simple distributed compiler system
 *
 * Copyright (C) 2002, 2003 by Martin Pool <mbp@samba.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */



#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>

#include "distcc.h"
#include "trace.h"
#include "io.h"
#include "util.h"
#include "exitcode.h"
#include "strip.h"


/**
 * Strip arguments like -D and -I from a command line, because they do
 * not need to be passed across the wire.  This covers options for
 * both the preprocess and link phases, since they should never happen
 * remotely.
 *
 * In the case where we inadvertently do cause preprocessing to happen
 * remotely, it is possible that omitting these options will make
 * failure more obvious and avoid false success.
 *
 * Giving -L on a compile-only command line is a bit wierd, but it is
 * observed to happen in Makefiles that are not strict about CFLAGS vs
 * LDFLAGS, etc.
 *
 * NOTE: gcc-3.2's manual in the "preprocessor options" section
 * describes some options, such as -d, that only take effect when
 * passed directly to cpp.  When given to gcc they have different
 * meanings.
 **/
int dcc_strip_local_args(char **from, char ***out_argv)
{
    char **to;
    int from_i, to_i;
    int from_len;

    dcc_trace_argv("original arguments", from);

    from_len = dcc_argv_len(from);
    *out_argv = to = malloc((from_len + 1) * sizeof (char *));

    if (!to) {
        rs_log_error("failed to allocate space for arguments");
        return EXIT_OUT_OF_MEMORY;
    }
    
    /* skip through argv, copying all arguments but skipping ones that
     * ought to be omitted */
    for (from_i = to_i = 0; from[from_i]; from_i++) {
        if (str_equal("-D", from[from_i])
            || str_equal("-I", from[from_i])
            || str_equal("-U", from[from_i])
            || str_equal("-L", from[from_i])
            || str_equal("-l", from[from_i])
            || str_equal("-MF", from[from_i])
            || str_equal("-MT", from[from_i])
            || str_equal("-MQ", from[from_i])
            || str_equal("-include", from[from_i])
            || str_equal("-imacros", from[from_i])
            || str_equal("-iprefix", from[from_i])
            || str_equal("-iwithprefix", from[from_i])
            || str_equal("-isystem", from[from_i])
            || str_equal("-iwithprefixbefore", from[from_i])
            || str_equal("-idirafter", from[from_i])) {
            /* skip next word, being option argument */
            if (from[from_i+1])
                from_i++;
        }
        else if (str_startswith("-Wp,", from[from_i])
                 || str_startswith("-Wl,", from[from_i])
                 || str_startswith("-D", from[from_i])
#if defined(DARWIN)
                 || str_startswith("-F", from[from_i])
#endif // DARWIN
                 || str_startswith("-U", from[from_i])
                 || str_startswith("-I", from[from_i])
                 || str_startswith("-l", from[from_i])
                 || str_startswith("-L", from[from_i])) {
            /* Something like "-DNDEBUG" or
             * "-Wp,-MD,.deps/nsinstall.pp".  Just skip this word */
            ;
        }
        else if (str_equal("-undef", from[from_i])
                 || str_equal("-nostdinc", from[from_i])
                 || str_equal("-nostdinc++", from[from_i])
                 || str_equal("-MD", from[from_i])
                 || str_equal("-MMD", from[from_i])
                 || str_equal("-MG", from[from_i])
                 || str_equal("-MP", from[from_i])) {
            /* Options that only affect cpp; skip */
            ;
        }
        else {
            to[to_i++] = from[from_i];
        }
    }
    
    /* NULL-terminate */
    to[to_i] = NULL;

    dcc_trace_argv("result", to);

    return 0;
}



/**
 * Remove "-o" options from argument list.
 *
 * Structurally similar to dcc_strip_local_args()
 **/
int dcc_strip_dasho(char **from, char ***out_argv)
{
    char **to;
    int from_i, to_i;
    int from_len;

    from_len = dcc_argv_len(from);
    *out_argv = to = malloc((from_len + 1) * sizeof (char *));

    if (!to) {
        rs_log_error("failed to allocate space for arguments");
        return EXIT_OUT_OF_MEMORY;
    }
    
    /* skip through argv, copying all arguments but skipping ones that
     * ought to be omitted */
    for (from_i = to_i = 0; from[from_i]; ) {
        if (!strcmp(from[from_i], "-o")) {
            /* skip "-o  FILE" */
            from_i += 2;
        }
        else if (str_startswith("-o", from[from_i])) {
            /* skip "-oFILE" */
            from_i++;
        }
        else {
            to[to_i++] = from[from_i++];
        }
    }

    /* NULL-terminate */
    to[to_i] = NULL;

    dcc_trace_argv("result", to);

    return 0;
}



