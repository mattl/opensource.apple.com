
#include <stdio.h>
#include <dns_sd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mach-o/arch.h>
#include "trace.h"
#include "versinfo.h"

typedef struct _CompilerInfo {
    char *path;
    char *versionInfo;
    struct timespec modTime;
    struct _CompilerInfo *next;
} CompilerInfo;


/*
 Invokes the command line given by commandLine (using popen) and returns the
 text of the command's output. Returns NULL if any error is encountered.
 */
static char *dcc_run_simple_command(const char *commandLine)
{
    FILE *output;
    char *versionInfo = NULL;;
    int buffSize = 1024, len = 0;
    
    output = popen(commandLine, "r");
    if (output) {
        versionInfo = (char *)malloc(buffSize * sizeof(char));
        while (versionInfo && !feof(output)) {
            if (len == buffSize-1) {
                char *newBuff = (char *)realloc(versionInfo, buffSize*2);
                if (newBuff) {
                    versionInfo = newBuff;
                    buffSize *= 2;
                } else {
                    free(versionInfo);
                    return NULL;
                }
            }
            len += fread(&versionInfo[len], 1, buffSize-len-1, output);
        }
        pclose(output);
    }
    versionInfo[len]=0;
    return versionInfo;
}

static CompilerInfo *dcc_parse_etc_compilers()
{
    static CompilerInfo *compilers = NULL;
    struct stat sb;
    if (compilers == NULL && stat("/etc/compilers", &sb) == 0) {
        int compilersFD = open("/etc/compilers", O_RDONLY, 0);
        if (compilersFD != 1) {
            char *compilersBuff = (char *)malloc(sb.st_size+1);
            if (compilersBuff) {
                if (read(compilersFD, compilersBuff, sb.st_size) == sb.st_size) {
                    int i, lineCount;
                    compilersBuff[sb.st_size] = 0; // null terminate

                    // change all the newlines to terminators
                    for (i=0; i<sb.st_size; i++) {
                        if (compilersBuff[i] == '\n') {
                            compilersBuff[i] = 0;
                            lineCount++;
                        }
                    }
                    
                    // now we can just parse line by line
                    int lineStart, lineLen, compilerCount = 0;
                    for (lineStart = 0; lineStart < sb.st_size; lineStart+=lineLen+1) {
                        struct stat cc_sb;
                        lineLen = strlen(&compilersBuff[lineStart]);
                        if (lineLen > 0 && compilersBuff[lineStart] != '#' && stat(&compilersBuff[lineStart], &cc_sb) == 0) {
                            // we have not seen this compiler before so allocate a new node
                            // note that we do not fill in the version info here
                            CompilerInfo *ci = (CompilerInfo *)calloc(1, sizeof(CompilerInfo));
                            ci->path = strdup(&compilersBuff[lineStart]);
                            ci->next = compilers;
                            compilers = ci;
                        }
                    }
                }
                free(compilersBuff);
            }
            close(compilersFD);
        }
    }
    return compilers;
}

static CompilerInfo *dcc_compiler_info_for_path(char *compiler)
{
    CompilerInfo *compilers = dcc_parse_etc_compilers();
    while (compilers && strcmp(compilers->path, compiler) != 0)
        compilers = compilers->next;
    return compilers;
}

static char *_dcc_get_compiler_version(CompilerInfo *compiler)
{
    char *result = NULL;
    struct stat sb;
    if (compiler) {
        if (stat(compiler->path, &sb) == 0 && compiler->versionInfo != NULL) {
            // we found the compiler, check that the timestamp is unchanged
            if (memcmp(&sb.st_ctimespec, &compiler->modTime, sizeof(struct timespec)) != 0) {
                // didn't match, so throw away version number
                rs_log_warning("compiler version changed: %s", compiler->path);
                //free(compiler->versionInfo); // leak; should be very uncommon
                compiler->versionInfo = NULL;
            }
        }
        
        if (!compiler->versionInfo) {
            // have to fetch the version number
            int lineLen = strlen(compiler->path);
            char *versionArgs = " -v 2>&1";
            char commandBuff[lineLen+strlen(versionArgs)+1];
            char *versionOutput;
            strcpy(commandBuff, compiler->path);
            strcat(commandBuff, versionArgs);
            versionOutput = dcc_run_simple_command(commandBuff);
            if (versionOutput) {
                char *version = strstr(versionOutput, "gcc version");
                if (version) {
                    int newline = 0;
                    while (version[newline] != '\n' && version[newline] != 0)
                        newline++;
                    compiler->versionInfo = (char *)malloc(newline+1);
                    strncpy(compiler->versionInfo, version, newline);
                    compiler->versionInfo[newline]=0;
                    compiler->modTime = sb.st_ctimespec;
                }
            }
        }
        result = compiler->versionInfo;
    }
    return result;
}

char *dcc_get_compiler_version(char *compilerPath)
{
    return _dcc_get_compiler_version(dcc_compiler_info_for_path(compilerPath));
}

int dcc_is_allowed_compiler(char *path)
{
    return dcc_compiler_info_for_path(path) != NULL;
}

/*
 This function returns a list of compiler version strings to use in the
 TXT record. It returns a NULL terminated array of version strings, or
 NULL if an error is encountered.
 */
char **dcc_get_all_compiler_versions(void)
{
    CompilerInfo *compilers = dcc_parse_etc_compilers();
    char **result = NULL;
    if (result == NULL) {
        struct stat sb;
        CompilerInfo *ci;
        int i, j, count = 0;
        for (ci = compilers; ci != NULL; ci=ci->next)
            count++;
        
        result = (char **)calloc(count+1, sizeof(char *));
        for (i=0, ci = compilers; ci != NULL; ci = ci->next) {
            char *version = _dcc_get_compiler_version(ci);
            if (version) {
                for (j=0; j<i; j++) {
                    if (strcmp(result[j], version) == 0)
                        break;
                }
                if (j == i)
                    result[i++] = version;
            }
        }
    }
    return result;
}


char *dcc_get_system_version(void)
{
    static char *ret = NULL;
    if (ret == NULL) {
        char *sw_vers = dcc_run_simple_command("/usr/bin/sw_vers");
        if (sw_vers) {
            char *prodVers, *prodVersStr = "ProductVersion:";
	    char *buildVers, *buildVersStr = "BuildVersion:";
            char *nl;
            char archbuf[32];
            
            prodVers = strstr(sw_vers, prodVersStr);
	    buildVers = strstr(sw_vers, buildVersStr);

            if (prodVers) {
                // find the start of the actual version string
                prodVers += strlen(prodVersStr);
                while (isspace(*prodVers))
                    prodVers++;
                // change the newline to a null terminator
                nl = prodVers;
                while (*nl != 0 && *nl != '\n')
                    nl++;
                *nl = 0;
            } else {
                prodVers = "Unknown";
                rs_log_warning("failed to parse ProcuctVersion from sw_vers");
            }

	    if (buildVers) { 
	      buildVers += strlen(buildVersStr);
	      while (isspace(*buildVers))
		buildVers++;
	      nl = buildVers;
	      while (*nl != 0 && *nl != '\n')
		nl++;
	      *nl = 0;
	    }
	    else {
	      buildVers = "Unknown";
	      rs_log_warning("failed to parse BuildVersion from sw_vers");
	    }
            
            const NXArchInfo *myArch = NXGetLocalArchInfo();
            const char *archName;
            if (myArch) {
                switch (myArch->cputype) {
                    case CPU_TYPE_POWERPC:
                        archName = "ppc";
                        break;
                    case CPU_TYPE_I386:
                        archName = "i386";
                        break;
                    default:
                        // don't know the name, just the number
                        archName = archbuf;
                        sprintf(archbuf, "%d", myArch->cputype);
                        rs_log_warning("unknown cputype: %d", myArch->cputype);
                        break;
                }
            } else {
                rs_log_warning("failed to get arch info");
                archName = "unknown";
            }
            
            // construct a string of the form 10.x.y (ppc)
            ret = malloc(strlen(prodVers) + strlen(buildVers) + strlen(archName) + strlen(" ()") + 1);
            sprintf(ret, "%s (%s, %s)", prodVers, buildVers, archName);
            free(sw_vers);
        }
    }
    return ret;
}
