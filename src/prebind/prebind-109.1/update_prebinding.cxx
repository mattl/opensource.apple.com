// -*- mode: C++; c-basic-offset: 4; tab-width: 4 -*- 
//
// update_prebinding [-root directory] [-force] [-debug] [-progress]
//
//		-root dir	used when prebinding a volume that is not the boot volume 
//		-force		forces all dylibs to be re-prebound even if already up to date
//		-debug		more dyld spewage
//		-progess	used by installer to show the percent of work done
//
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>

#ifndef KERN_OTHERARCHHANDLER
#define KERN_OTHERARCHHANDLER 45
#endif


//
//	Prebinding is driven by /var/db/dyld/update-prebinding-paths.txt which contains
//	the list of files to prebind.  
//
//	Leading and trailing white space is ignored
//	Blank lines are ignored
//	Lines starting with # are ignored
//
static void parsePathsFile(const char* preboundDylibsPathsFile, std::vector<const char*>& paths)
{
	// read in whole file
	int fd = open(preboundDylibsPathsFile, O_RDONLY, 0);
	if ( fd == -1 ) {
		fprintf(stderr, "update_prebinding:can't open file: %s", preboundDylibsPathsFile);
		exit(1);
	}
	struct stat stat_buf;
	fstat(fd, &stat_buf);
	char* p = (char*)malloc(stat_buf.st_size);
	if ( p == NULL ) {
		fprintf(stderr, "update_prebinding:malloc failure");
		exit(1);
	}	
	if ( read(fd, p, stat_buf.st_size) != stat_buf.st_size ) {
		fprintf(stderr, "update_prebinding: can't read file: %s", preboundDylibsPathsFile);
		exit(1);
	}	
	::close(fd);
	
	// parse into paths and add to vector
	char * const end = &p[stat_buf.st_size];
	enum { lineStart, inSymbol, inComment } state = lineStart;
	char* symbolStart = NULL;
	for (char* s = p; s < end; ++s ) {
		switch ( state ) {
			case lineStart:
				if ( *s =='#' ) {
					state = inComment;
				}
				else if ( !isspace(*s) ) {
					state = inSymbol;
					symbolStart = s;
				}
				break;
			case inSymbol:
				if ( *s == '\n' ) {
					*s = '\0';
					// removing any trailing spaces
					char* last = s-1;
					while ( isspace(*last) ) {
						*last = '\0';
						--last;
					}
					paths.push_back(symbolStart);
					symbolStart = NULL;
					state = lineStart;
				}
				break;
			case inComment:
				if ( *s == '\n' )
					state = lineStart;
				break;
		}
	}
	// Note: we do not free() the malloc buffer, because the strings in it are used by exec()
}



//
// Currently some options are passed to dyld as environment variables and others in a flags parameter
// The flags parameter is a 32 char string of zeros and ones.  See _dyld_update_prebinding() in dyld.
//
static void parseCommandLineOptions(int argc, const char* argv[], char flags[])
{
	int flagsLen = strlen(flags);
	for(int i=1; i < argc; ++i) {
		const char* arg = argv[i];
		if ( arg[0] == '-' ) {
			// --root <path> or --root <path>
			if ( (strcmp(arg, "-root") == 0) || (strcmp(&arg[1], "-root") == 0) ) {
				const char* rootPath = argv[++i];
				if ( (rootPath == NULL) || (rootPath[0] != '/') ) {
					fprintf(stderr, "update_prebinding: -root requires an absolute path to follow\n");
					exit(1);
				}
				setenv("DYLD_ROOT_PATH", rootPath, 1);
			}
			// --force or -force
			else if ( (strcmp(arg, "-force") == 0) || (strcmp(&arg[1], "-force") == 0) ) {
				setenv("DYLD_IGNORE_PREBINDING", "all", 1);
			}
			// --debug or -debug
			else if ( (strcmp(arg, "-debug") == 0) || (strcmp(&arg[1], "-debug") == 0) ) {
				setenv("DYLD_PREBIND_DEBUG", "1", 1);
			}
			// --progress or -progress
			else if ( (strcmp(arg, "-progress") == 0) || (strcmp(&arg[1], "-progress") == 0) ) {
				flags[flagsLen-2] = '1';	//  UPDATE_PREBINDING_PROGRESS 0x00000002
			}
			// --dry-run or -dry-run
			else if ( (strcmp(arg, "-dry-run") == 0) || (strcmp(&arg[1], "-dry-run") == 0) ) {
				flags[flagsLen-1] = '1';	//  UPDATE_PREBINDING_DRY_RUN 0x00000001
			}
			else {
				fprintf(stderr, "update_prebinding: warning: unknown option %s\n", arg);
			}
		}
		else {
			fprintf(stderr, "update_prebinding: warning: unknown argument %s\n", arg);
		}
	}
}

	
//
//	Run a sub-process to completion.
//		path is the binary to run.
//		args is the command line arguments to it
//		affinity means set the affinity sysctl() before the exec()
//
static int run(const char* path, const std::vector<const char*>& args, bool affinity=false)
{
	// build NULL termintated argv[] with argv[0] being path to exectuable 
	const char* argv[args.size()+2];
	argv[0] = path;
	int i=1;
	for(std::vector<const char*>::const_iterator it=args.begin(); it != args.end(); ++it) {
		argv[i++] = *it;
	}
	argv[i] = NULL;

	// fork and exec
	pid_t pid = fork();
	switch(pid) {
	case -1:	// error
		return -1;
	case 0:		// child
		if ( affinity ) {
#if __i386__
			int mib[] = { CTL_KERN, KERN_AFFINITY, 1, 1 };
			int namelen = 4;
			sysctl(mib, namelen, NULL, NULL, NULL, 0);
#endif
		}
		execv(path, (char**)argv);
		_exit(127);
	default:	// parent 
		int childStatus;
		if (waitpid (pid, &childStatus, 0) != pid)
			return -1;
		return childStatus;
	}
}


#if __i386__
static bool getRosettaPath(char* path)
{
    int mib[2] = {CTL_KERN, KERN_OTHERARCHHANDLER}; 
    size_t len = PATH_MAX;
    return ( sysctl(mib, 2, (void*)path, &len, 0, 0) == 0 );
}
#endif

int main(int argc, const char* argv[])
{
	// parse command line options
	char flags[] = "00000000000000000000000000000000";
	parseCommandLineOptions(argc, argv, flags);
	
	// read and parse /var/db/dyld/update-prebinding-paths.txt
	std::vector<const char*> coreArgs;
	coreArgs.reserve(1000);
	parsePathsFile("/var/db/dyld/update-prebinding-paths.txt", coreArgs);
	
	// add flags as last parameter
	coreArgs.push_back(flags);
	
	// run update_prebinding_core
	int result = run("/usr/bin/update_prebinding_core", coreArgs);
	if ( result != 0 )
		fprintf(stderr, "update_prebinding: error %d running update_prebinding_core\n", result);
	
#if __i386__
	// tell Rosetta to flush its caches <rdar://problem/4100886>
	std::vector<const char*> rosettaArgs;
	rosettaArgs.push_back("-prebindOAH");
	char rosettaPath[PATH_MAX];
	if ( getRosettaPath(rosettaPath) ) {
		result = run(rosettaPath, rosettaArgs, false);
		if ( result != 0 ) 
			fprintf(stderr, "update_prebinding: error %d running %s -prebindOAH\n", result, rosettaPath);
	}
	else {
		fprintf(stderr, "update_prebinding: error could not locate Rosetta\n");
	}
	
	// need to run update_prebinding_core again on ppc side rdar://problem/4004904
	result = run("/usr/bin/update_prebinding_core", coreArgs, true);
	if ( result != 0 )
		fprintf(stderr, "update_prebinding: error %d running update_prebinding_core\n", result);
#endif	
	
	// rdar://problem/3963439 delete kernel preheat cache
	unsetenv("DYLD_ROOT_PATH");
	unsetenv("DYLD_PREBIND_DEBUG");
	std::vector<const char*> rmArgs;
	rmArgs.push_back("-c");
	rmArgs.push_back("/bin/rm -f /var/vm/app_profile/*{_names,_data}");
	run("/bin/sh", rmArgs);
	
	return result;
}



