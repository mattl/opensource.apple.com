/* -*- c-file-style: "java"; indent-tabs-mode: nil; fill-column: 78 -*-
 * 
 * distcc -- A simple distributed compiler system
 *
 * Copyright (C) 2003, 2005 by Apple Computer, Inc.
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


/**
 * Server functions for file indirection feature.
 **/

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dirent.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "distcc.h"
#include "bulk.h"
#include "config.h"
#include "exitcode.h"
#include "exec.h"
#include "indirect_util.h"
#include "rpc.h"
#include "trace.h"


static const char message_terminator = '\n';



/**
 * Receive messages from a child process (typically GCC 3.3 and later).
 * These messages provide information from the child regarding file
 * indirection.
 * The message includes all bytes on the pipe up to the first message
 * terminator (newline) encountered.
 * Converts the message terminator into a string terminator.
 * Stores the message in buffer.
 *
 * Return 1 on success and 0 on failure.
 * Fails if the message is longer than size
 **/
static int read_from_child(dcc_indirection *indirect, char *buffer, const int size)
{
    int idx = 0;
    int result;

    if ( size <= 0 ) {
        return 0;
    }

    do {
        result = read(indirect->childWrite[0], &buffer[idx], 1);

        if ( result <= 0 || idx >= size ) {
            return 0;
        } else {
            idx += result;
        }
    } while ( buffer[idx - 1] != message_terminator );

    // Straighten out the string termination.
    buffer[idx - 1] = '\0';

    return 1;
}


/**
 * Write message to a child process (typically GCC 3.3 and later).
 * These messages provide information to the child regarding file
 * indirection.
 * The message includes all bytes up to the string terminator.
 * Converts the string terminator into a message terminator (newline).
 * Writes a single message terminator alone if message is
 * NULL.
 *
 * Return 1 on success and 0 on failure.
 **/
static int write_to_child(dcc_indirection *indirect, const char *message)
{
    int result;

    if ( message ) {
        const int length = strlen(message);
        int idx = 0;

        while ( idx < length ) {
            result = write(indirect->parentWrite[1], &message[idx], length - idx);

            if ( result < 0 ) {
                return 0;
            } else {
                idx += result;
            }
        }
    }

    result = write(indirect->parentWrite[1], &message_terminator, 1);

    if ( result < 0 ) {
        return 0;
    }

    return 1;
}


/**
 * Create path_to_dir, including any intervening directories.
 * Behaves similarly to mkdir -p.
 * Permissions are set to ug=rwx,o-rwx.
 *
 * path_to_dir must not be NULL
 **/
static int mkdir_p(const char *path_to_dir)
{
    char *path = strdup(path_to_dir);
    char current_path[MAXPATHLEN];
    char *latest_path_element;

    current_path[0] = '\0';
    latest_path_element = strtok(path, "/");

    while ( strlen(current_path) < MAXPATHLEN - MAXNAMLEN - 1 ) {
        if ( latest_path_element == NULL ) {
            break;
        } else {
            strcat(current_path, "/");
            strcat(current_path, latest_path_element);

            // 504 == \770 == ug=rwx,o-rwx
            if ( mkdir(current_path, 504) == 0 ) {
                rs_trace("Created directory %s", current_path);
            } else {
                if ( errno == EEXIST ) {
                    rs_trace("Directory exists: %s", current_path);
                } else {
                    rs_log_error("Unable to create directory %s: %s",
                                 current_path, strerror(errno));
                    return 0;
                }
            }

            latest_path_element = strtok(NULL, "/");
        }
    }

    free(path);

    if ( latest_path_element == NULL ) {
        return 1;
    } else {
        return 0;
    }
}


/**
 * Creates a temporary file at tmp_path to store
 * checksum.
 **/
static char *dcc_create_checksum_tmpfile(const char *tmp_path,
                                         const char *checksum)
{
    char *checksum_tmpfile = malloc(MAXPATHLEN);
    int   fd;

    if ( checksum_tmpfile == NULL ) {
        rs_log_error("Unable to allocate space for checksum_tmpfile");
        return NULL;
    }

    strncpy(checksum_tmpfile, tmp_path, MAXPATHLEN);
    strcat(checksum_tmpfile, checksum_suffix);

    // do not need to mkdir_p, since this should always happen after
    // the files represented by the checksum are laid down

    fd = open(checksum_tmpfile, O_WRONLY|O_CREAT, 0777);

    if ( fd < 0 ) {
        rs_log_error("Unable to create %s", checksum_tmpfile);
        return NULL; 
    } else {
        size_t checksum_length = strlen(checksum);

        if ( write(fd, checksum, checksum_length) == (ssize_t)checksum_length ){
            rs_trace("Wrote checksum to %s", checksum_tmpfile);
            close(fd);
            return checksum_tmpfile;
        } else {
            rs_log_error("Unable to write checksum to %s", checksum_tmpfile);
            close(fd);
            remove(checksum_tmpfile);
            return NULL;
        }
    }
}


/**
 * Pulls a checksum for the previously requested file over the socket
 * specified by netfd.  Stores the checksum at
 * tmp_path.
 **/
static char *dcc_pull_checksum(int netfd, const char *tmp_path)
{
    unsigned checksum_length = 0;

    if ( dcc_r_token_int(netfd, checksum_length_token, &checksum_length)
         || checksum_length <= 0 ) {
        rs_log_error("No incoming checksum for path %s", tmp_path);
        return NULL;
    } else {
        char *checksum = malloc(checksum_length + 1);

        if ( dcc_readx(netfd, checksum, checksum_length) ) {
            rs_log_error("Unable to read checksum for path %s", tmp_path);
            free(checksum);
            return NULL;
        } else {
            char *tmp_checksum_filename = dcc_create_checksum_tmpfile(tmp_path,
                                                                      checksum);
            free(checksum);
            return tmp_checksum_filename;
        }
    }
}


/**
 * Pushes the existing checksum for the previously requested file
 * over the socket specified by netfd.
 * hostname used strictly to disambiguate logging.
 **/
static int dcc_push_checksum(int netfd, const char *hostname,
                               const char *checksum)
{
    size_t checksum_length = ( checksum == NULL ) ? 0 : strlen(checksum) + 1;

    if ( dcc_x_token_int(netfd, checksum_length_token, checksum_length) ) {
        rs_log_error("Unable to transmit checksum length %u to %s",
                     checksum_length, hostname);
        return 0;
    } else if ( checksum_length > 0 ) {
        if ( dcc_writex(netfd, checksum, checksum_length) ) {
            rs_log_error("Unable to transmit checksum \"%s\" to %s",
                         checksum, hostname);
            return 0;
        }
    }

    return 1;
}


/**
 * Reads the contents of cached_path_checksum.
 * cached_path_checksum_size is used as a hint for the size
 * of the buffer to return.  The returned buffer must be freed by the caller.
 **/
static char *dcc_read_checksum(const char *cached_path_checksum,
                               size_t cached_path_checksum_size)
{
    int   remaining_buffer = cached_path_checksum_size + 1;
    char *checksum         = malloc(remaining_buffer);

    if ( checksum == NULL ) {
        rs_log_error("Unable to allocate space for checksum_tmpfile");
    } else {
        int checksum_file = open(cached_path_checksum, O_RDONLY, 0777);

        if ( checksum_file <= 0 ) {
            rs_log_error("Unable to open %s", cached_path_checksum);
            free(checksum);
            checksum = NULL;
        } else {
            int num_read = -1;

            while ( remaining_buffer > 0 &&
                    ( num_read = read(checksum_file, checksum,
                                      remaining_buffer) ) > 0 ) {
                remaining_buffer -= num_read;
            }

            checksum[cached_path_checksum_size] = '\0';

            if ( num_read < 0 ||
                 strlen(checksum) < cached_path_checksum_size ) {
                rs_log_error("Unable to read checksum from %s",
                             cached_path_checksum);
                free(checksum);
                checksum = NULL;
            }

            close(checksum_file);
        }
    }

    return checksum;
}


/**
 * Pulls the previously requested directory over the socket specified by
 * netfd.  Currently, this directory cannot contain any
 * subdirectories.  tmp_path specifies the location on the
 * server where the directory will be stored.  hostname is
 * used only to disambiguate logging.
 **/
static void dcc_pull_directory_file(int netfd, const char *hostname,
                                   const char *tmp_path)
{
    int cwd = open(".", O_RDONLY, 0777);

    if ( chdir(tmp_path) ) {
        rs_log_error("Unable to chdir to %s", tmp_path);
    } else {
        unsigned incomingBytes;

        if ( dcc_r_token_int(netfd, result_name_token, &incomingBytes)
             || incomingBytes <= 0 ) {
            rs_log_error("No incoming file from %s for directory %s",
                         hostname, tmp_path);
        } else {
            char filename[incomingBytes];

            if ( dcc_readx(netfd, filename, incomingBytes) ) {
                rs_log_error("Unable to read filename from %s for file in directory %s", hostname, tmp_path);
            } else {
                if (dcc_r_token_int(netfd, result_item_token, &incomingBytes)) {
                    rs_log_error("Unable to read length from %s for file %s in %s", hostname, filename, tmp_path);
                } else {
                    if ( dcc_r_file_timed(netfd, filename, incomingBytes, DCC_COMPRESS_LZO1X) ) {
                        rs_log_error("Failed to retrieve from %s incoming file %s/%s", hostname, tmp_path, filename);
                    } else {
                        rs_log_info("Stored incoming file from %s at %s in %s",
                                    hostname, filename, tmp_path);
                    }
                }
            }
        }
    }

    fchdir(cwd);
    close(cwd);
}


/**
 * Remove path (not including intervening directories).
 * All files in path are removed, as well.
 * Does not remove subdirectories of path.
 **/
static int dcc_rmdir(const char *path) {
    int    numFiles;
    char **filenames = dcc_filenames_in_directory(path, &numFiles);

    if ( filenames != NULL ) {
        int dir = open(path, O_RDONLY, 0777);

        if ( dir > 0 ) {
            int cwd = open(".", O_RDONLY, 0777);

            if ( fchdir(dir) == 0 ) {
                int i;

                for ( i = 0; i < numFiles && filenames[i] != NULL; i++ ) {
                    if ( unlink(filenames[i]) ) {
                        rs_log_error("Unable to remove %s from %s",
                                     filenames[i], path);
                    } else {
                        rs_trace("Removed %s from %s", filenames[i], path);
                    }

                    free(filenames[i]);
                }
            }

            fchdir(cwd);
            close(cwd);
            close(dir);
        }

        free(filenames);
    }

    return rmdir(path);
}


/**
 * Handles a pull operation, as specified by the server's child (typically
 * gcc 3.3).  Pulled files are stored in a per-host cache.
 **/
static int dcc_handle_pull(dcc_indirection *indirect)
{
    char  *actual_path          = NULL;
    size_t hostname_length      = strlen(indirect->hostname) + 1;
    char   response[MAXPATHLEN], pull_lock[MAXPATHLEN], cached_path_checksum[MAXPATHLEN];
    const char  *tempdir;

    if (dcc_get_tmp_top(&tempdir))
        return -1;
    size_t tempdir_length       = strlen(tempdir) + 1;
    int    pull_lockfd;
    
    strcpy(pull_lock, tempdir);
    strcat(pull_lock, "/");
    strcat(pull_lock, indirect->hostname);
    mkdir_p(pull_lock);
    strcat(pull_lock, "/pull_lockfile");
    pull_lockfd = open(pull_lock, O_WRONLY|O_CREAT|O_EXLOCK, 0777);

    if ( read_from_child(indirect, response, MAXPATHLEN) ) {
        size_t response_length = strlen(response) + 1;
        size_t total_length  = tempdir_length + hostname_length
            + response_length;
        char *cached_path = malloc(total_length);
        int    cached_path_exists;
        int    cached_path_checksum_exists;
        size_t cached_path_checksum_size;
        char  *checksum = NULL;
        struct stat sb;
        
        rs_trace("Attempting to pull %s", response);
        
        strcpy(cached_path, tempdir);
        if (indirect->hostname[0] != '/')
            strcat(cached_path, "/");
        strcat(cached_path, indirect->hostname);
        if (response[0] != '/')
            strcat(cached_path, "/");
        strcat(cached_path, response);
        
        strncpy(cached_path_checksum, cached_path, total_length);
        strncat(cached_path_checksum, checksum_suffix, checksum_suffix_length);
        
        cached_path_exists = ( stat(cached_path, &sb) == 0 );
        cached_path_checksum_exists = ( stat(cached_path_checksum, &sb) == 0 );
        cached_path_checksum_size = sb.st_size;
        
        if ( cached_path_exists && cached_path_checksum_exists &&
             cached_path_checksum_size > 0 ) {
            checksum = dcc_read_checksum(cached_path_checksum,
                                         cached_path_checksum_size);
        }
        
        if ( dcc_x_token_int(indirect->out_fd, indirection_request_token,
                             indirection_request_pull)
             || dcc_x_token_int(indirect->out_fd, operation_pull_token, response_length)
             || dcc_writex(indirect->out_fd, response, response_length) ) {
            rs_log_error("Unable to transmit filename to %s", indirect->hostname);
            actual_path = response;
        } else {
            if ( ! dcc_push_checksum(indirect->out_fd, indirect->hostname, checksum) ) {
                actual_path = response;
            }
        }
        
        if ( checksum != NULL ) {
            free(checksum);
            checksum = NULL;
        }
        
        if ( actual_path == NULL ) {
            unsigned result_type = result_type_nothing;
            
            if ( dcc_r_token_int(indirect->in_fd, result_type_token, &result_type) ) {
                rs_log_error("Unable to read result type from %s", indirect->hostname);
                actual_path = response;
            } else if ( result_type == result_type_nothing ||
                        result_type == result_type_checksum_only ) {
                // the client says that it has nothing or nothing newer
                // than what the server has 
                
                if ( cached_path_exists && cached_path_checksum_exists ) {
                    rs_log_info("Using cached version: %s", cached_path);
                    actual_path = cached_path;
                } else {
                    rs_log_error("Unable to find %s on %s", response, indirect->hostname);
                    actual_path = response;
                }
                
                if ( result_type == result_type_checksum_only ) {
                    char *tmpName;
                    dcc_make_tmpnam(indirect->hostname, "pullfile", &tmpName);
                    char *tmpChecksumName = NULL;
                    char *lastSlash       = strrchr(tmpName, '/');
                    
                    lastSlash[0] = '\0';
                    
                    if ( mkdir_p(tmpName) ) {
                        lastSlash[0] = '/';
                        
                        tmpChecksumName = dcc_pull_checksum(indirect->in_fd, tmpName);
                        
                        if ( tmpChecksumName != NULL ) {
                            
                            if ( rename(tmpChecksumName, cached_path_checksum)){
                                rs_log_error("Failed to move %s to %s: %s",
                                             tmpChecksumName,
                                             cached_path_checksum,
                                             strerror(errno));
                            } else {
                                rs_trace("Moved %s to %s", tmpChecksumName,
                                         cached_path_checksum);
                            }
                                                        
                            free(tmpChecksumName);
                        }
                    } else {
                        rs_log_error("Unable to create directory %s", tmpName);
                    }
                }
            } else if ( result_type == result_type_file ) {
                char *tmpChecksumName = NULL;
                char *tmpName;
                // FIXME: check return
                dcc_make_tmpnam(indirect->hostname, "pullfile", &tmpName);
                char *lastSlash       = strrchr(tmpName, '/');
                unsigned   incomingBytes;
                
                if ( dcc_r_token_int(indirect->in_fd, result_item_token, &incomingBytes)
                     || incomingBytes <= 0 ) {
                    rs_log_error("No incoming file for path %s", response);
                    actual_path = response;
                } else {
                    lastSlash[0] = '\0';
                    
                    if ( mkdir_p(tmpName) ) {
                        lastSlash[0] = '/';
                        if ( dcc_r_file_timed(indirect->in_fd, tmpName, incomingBytes, DCC_COMPRESS_LZO1X)
                             == 0 ) {
                            rs_log_info("Stored incoming file at %s", tmpName);
                            tmpChecksumName = dcc_pull_checksum(indirect->in_fd, tmpName);
                        } else {
                            rs_log_error("Failed to retrieve incoming file %s",
                                         tmpName);
                            actual_path = response;
                        }
                    } else {
                        rs_log_error("Unable to create directory %s", tmpName);
                        actual_path = response;
                    }
                    
                }
                
                if ( actual_path == NULL ) {
                    // move the temp files into the cached location
                    int xfd;
                    
                    lastSlash = strrchr(cached_path, '/');
                    
                    lastSlash[0] = '\0';
                    
                    if ( ! mkdir_p(cached_path) ) {
                        rs_log_error("Unable to create directory %s",
                                     cached_path);
                    }
                    
                    lastSlash[0] = '/';
                    
                    if ( rename(tmpName, cached_path) ) {
                        rs_log_error("Failed to move %s to %s: %s", tmpName,
                                     cached_path, strerror(errno));
                        actual_path = response;
                    } else {
                        rs_trace("Moved %s to %s", tmpName, cached_path);
                        actual_path = cached_path;
                        
                        if ( tmpChecksumName != NULL ) {                  
                            if ( rename(tmpChecksumName, cached_path_checksum)){
                                rs_log_error("Failed to move %s to %s: %s",
                                             tmpChecksumName,
                                             cached_path_checksum,
                                             strerror(errno));
                            } else {
                                rs_trace("Moved %s to %s", tmpChecksumName,
                                         cached_path_checksum);
                            }
                        }
                    }
                }
                
                // cleanup
                
                if ( tmpChecksumName != NULL ) {
                    free(tmpChecksumName);
                }
                
            } else if ( result_type == result_type_dir ) {
                char *tmpChecksumName = NULL;
                char *tmpName;
                // FIXME: check return
                dcc_make_tmpnam(indirect->hostname, "pullfile", &tmpName);
                unsigned   fileCount;
                
                if ( dcc_r_token_int(indirect->in_fd, result_count_token, &fileCount)
                     || fileCount <= 0 ) {
                    rs_log_error("No incoming files for path %s", response);
                    actual_path = response;
                } else {
                    if ( mkdir_p(tmpName) ) {
                        int i;
                        
                        for ( i = 0; i < fileCount; i++ ) {
                            dcc_pull_directory_file(indirect->in_fd, indirect->hostname, tmpName);
                        }
                        
                        tmpChecksumName = dcc_pull_checksum(indirect->in_fd, tmpName);
                    } else {
                        rs_log_error("Unable to create directory %s", tmpName);
                        actual_path = response;
                    }
                }
                
                if ( actual_path == NULL ) {
                    // move the temp files into the cached location
                    int xfd;
                    
                    if ( dcc_rmdir(cached_path) ) {
                        char *lastSlash = strrchr(cached_path, '/');
                        
                        rs_log_error("Unable to remove %s", cached_path);
                        
                        lastSlash[0] = '\0';
                        
                        if ( ! mkdir_p(cached_path) ) {
                            rs_log_error("Unable to create directory %s",
                                         cached_path);
                            actual_path = response;
                        }
                        
                        lastSlash[0] = '/';
                    }
                    
                    if ( rename(tmpName, cached_path) ) {
                        rs_log_error("Failed to move %s to %s: %s", tmpName,
                                     cached_path, strerror(errno));
                        actual_path = response;
                    } else {
                        rs_trace("Moved %s to %s", tmpName, cached_path);
                        
                        actual_path = cached_path;
                        
                        if ( tmpChecksumName != NULL ) {                  
                            if ( rename(tmpChecksumName, cached_path_checksum)){
                                rs_log_error("Failed to move %s to %s: %s",
                                             tmpChecksumName,
                                             cached_path_checksum,
                                             strerror(errno));
                            } else {
                                rs_trace("Moved %s to %s", tmpChecksumName,
                                         cached_path_checksum);
                            }
                        }
                    }
                }
                
                // cleanup
                
                if ( tmpChecksumName != NULL ) {
                    free(tmpChecksumName);
                }
                
            }
        }
        
    } else {
        rs_log_error("Unable to receive %s request", operation_pull_token);
        actual_path = (char *) "UNKNOWN";
    }
    
    if ( write_to_child(indirect, actual_path) ) {
        rs_log_info("Using %s", actual_path);
    } else {
        rs_log_error("Unable to contact child for path %s", actual_path);
    }
    
    // cleanup
    
    if ( actual_path != response ) {
        free(actual_path);
    }
    if (pull_lockfd != -1)
        close(pull_lockfd);
    return 0;
}


/**
 * Thread task for handling indirection requests from a child process
 * (typically gcc 3.3).  Communication between the two processes occurs over
 * a parent-child pipe pair.  Only "pull" operations are currently implemented.
 * Invokes dcc_handle_pull to do so.
 * ifd is a pointer to the file descriptor for the socket
 * that the client and server use to communicate.
 **/
static void *dcc_handle_indirection(dcc_indirection *indirect)
{
    char *hostname = (char *) "unknown";
    struct sockaddr_in sain;
    size_t sain_length = sizeof(sain);
    size_t actual_length = sain_length;
    int result;
    
    // Grab the incoming hostname for use in creating a cache path on this
    // host for the desired files.

    if ( getpeername(indirect->in_fd, (struct sockaddr *) &sain, (socklen_t *) &actual_length)
         == 0 && actual_length <= sain_length ) {
        indirect->hostname = inet_ntoa(sain.sin_addr);
    } else {
        rs_log_error("Unable to determine remote hostname; using \"unknown\"");
    }

    do {
        char response[10];

        if ( (result = read_from_child(indirect, response, 10)) ) {
            if ( strcmp(operation_pull_token, response) == 0 ) {
                dcc_handle_pull(indirect);
            } else if ( strcmp(operation_push_token, response) == 0 ) {
                rs_log_error("Unsupported operation: %s", response);
            } else if ( strcmp(operation_both_token, response) == 0 ) {
                rs_log_error("Unsupported operation: %s", response);
            } else if ( strcmp(operation_version_token, response) == 0 ) {
                if ( read_from_child(indirect, response, 10) ) {
                    if ( strcmp(indirection_protocol_version, response) == 0 ) {
                        write_to_child(indirect, "OK");
                        continue;
                    }
                }

                write_to_child(indirect, "NO");
            } else {
                rs_log_error("Unsupported operation: %s", response);
            }
        }
    } while (result);
}


/**
 * Closes the child (typically gcc 3.3) end of the parent-child pipe pair used
 * to describe file indirection requests and results.
 **/
void dcc_close_pipe_end_child(dcc_indirection *indirect)
{
    if (indirect->parentWrite[1] != -1) {
        close(indirect->parentWrite[1]);
        indirect->parentWrite[1] = -1;
    }
    if (indirect->childWrite[0] != -1) {
        close(indirect->childWrite[0]);
        indirect->childWrite[0] = -1;
    }
}


/**
 * Closes the parent end of the parent-child pipe pair used to describe file
 * indirection requests and results.
 **/
void dcc_close_pipe_end_parent(dcc_indirection *indirect)
{
    if (indirect->parentWrite[0] != -1) {
        close(indirect->parentWrite[0]);
        indirect->parentWrite[0] = -1;
    }
    if (indirect->childWrite[1] != -1) {
        close(indirect->childWrite[1]);
        indirect->childWrite[1] = -1;
    }
}


/**
 * Creates a parent-child pipe pair to handle communication between the parent
 * (distccd) and the child (typically gcc 3.3).  
 **/
int dcc_prepare_indirect(dcc_indirection *indirect)
{
    indirect->childWrite[0] = -1;
    indirect->childWrite[1] = -1;
    
    int result = pipe(indirect->parentWrite);
    if ( result != 0 ) {
        rs_log_error("Unable to create file indirection pipe set #1: %s", strerror(errno));
        indirect->parentWrite[0] = -1;
        indirect->parentWrite[1] = -1;
    } else {
        result = pipe(indirect->childWrite);
        if ( result != 0 ) {
            rs_log_error("Unable to create file indirection pipe set #2: %s", strerror(errno));
            indirect->childWrite[0] = -1;
            indirect->childWrite[1] = -1;
        }
    }
    if (result != 0) {
        dcc_close_pipe_end_child(indirect);
        dcc_close_pipe_end_parent(indirect);
    }
    return result == 0 ? 0 : EXIT_OUT_OF_MEMORY;
}

void dcc_indirect_child(dcc_indirection *indirect)
{
    char *fdString;
    int result;
    
    if (indirect->parentWrite[0] != -1 && indirect->childWrite[1] != -1) {
        result = asprintf(&fdString, "%d, %d", indirect->parentWrite[0], indirect->childWrite[1]);
        
        if ( result <= 0 ) {
            rs_log_error("Unable to create file indirection pipe description: %s", strerror(errno));
            dcc_close_pipe_end_child(indirect);
            return;
        }
        
        result = setenv("GCC_INDIRECT_FILES", fdString, 1);
        
        if ( result != 0 ) {
            rs_log_error("Unable to set indirection environment variable");
            dcc_close_pipe_end_child(indirect);
            free(fdString);
            return;
        }
    }
}

int dcc_indirect_parent(dcc_indirection *indirect)
{
    int ret;
    dcc_close_pipe_end_parent(indirect);
    // FIXME: need to check result
    dcc_handle_indirection(indirect);
    ret = dcc_x_token_int(indirect->out_fd, indirection_request_token, indirection_complete);
    dcc_close_pipe_end_child(indirect);
    return ret;
}

