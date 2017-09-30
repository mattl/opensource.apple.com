#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#define SK_INDEX_NAME	"fts-sk"

void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [options] -q query skindex-path [...]\n",
	    prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -f    perform a fuzzy search\n");
    exit(1);
}

void nonnull(const void *p, const char *tag)
{
    if (p == NULL) {
	fprintf(stderr, "Out of memory:  Can't allocate for %s\n", tag);
	exit(1);
    }
}

Boolean skquery(const char *path, SKIndexRef skiref, CFStringRef query, Boolean fuzzy)
{
    Boolean ok = FALSE;

    SKSearchOptions opts = kSKSearchOptionDefault;
    if (fuzzy)
	opts |= kSKSearchOptionFindSimilar;
    SKSearchRef search = SKSearchCreate(skiref, query, opts);
    if (search != NULL) {
	enum { SK_MAXDOCS = 50 };

	Boolean more;
	do {
	    SKDocumentID results[SK_MAXDOCS];
	    memset(results, 0, sizeof results);
	    float scores[SK_MAXDOCS];
	    memset(scores, 0, sizeof scores);
	    CFIndex found = 0;
	    more = SKSearchFindMatches(search, SK_MAXDOCS, results, scores, 60, &found);
	    for (CFIndex i = 0; i < found; i++)
		printf("(%ld) score %f\n", results[i], scores[i]);

	    if (found > 0)
		ok = TRUE;
	} while (more);

	CFRelease(search);
    } else
	fprintf(stderr, "%s: SKSearchCreate failed\n", path);

    return ok;
}

Boolean skread(const char *path, const char *query, Boolean fuzzy)
{
    Boolean ok = FALSE;
    
    CFStringRef qref = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, query, kCFStringEncodingUTF8, kCFAllocatorNull);
    nonnull(qref, "query");

    CFURLRef index_url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8 *) path, strlen(path), FALSE);
    if (index_url != NULL) {
	SKIndexRef skiref = SKIndexOpenWithURL(index_url, CFSTR(SK_INDEX_NAME), FALSE);
	if (skiref != NULL) {
	    /* SK requires flush before search */
	    if (SKIndexFlush(skiref))
		ok = skquery(path, skiref, qref, fuzzy);
	    else
		fprintf(stderr, "%s: SKIndexFlush failed\n", path);

	    SKIndexClose(skiref);
	} else
	    fprintf(stderr, "%s: SKIndexOpenWithURL failed\n", path);

	CFRelease(index_url);
    }
    
    CFRelease(qref);
    return ok;
}

Boolean skadm(const char *path, const char *query, Boolean fuzzy)
{
    Boolean ok = FALSE;
    unsigned int frag = 1;

    struct stat stbuf;
    if (stat(path, &stbuf) == 0) {
	if (S_ISDIR(stbuf.st_mode)) {
	    char newpath[PATH_MAX];
	    snprintf(newpath, sizeof newpath, "%s/dovecot.skindex", path);
	    ok = skadm(newpath, query, fuzzy);

	    for (;;) {
		snprintf(newpath, sizeof newpath, "%s/dovecot.skindex-%u", path, frag);
		if (stat(newpath, &stbuf) < 0) {
		    if (errno != ENOENT) {
			perror(newpath);
			ok = FALSE;
		    }
		    break;
		}
		if (!skadm(newpath, query, fuzzy))
		    ok = FALSE;
		++frag;
	    }
	} else {
	    printf("%s:\n", path);
	    ok = skread(path, query, fuzzy);
	}
    } else
	perror(path);

    return ok;
}

int main (int argc, char * const argv[])
{
    const char *query = NULL;
    Boolean fuzzy = FALSE;
    Boolean ok = TRUE;
    int c;
    
    while ((c = getopt(argc, argv, "fq:")) != -1) {
	switch (c) {
	case 'f':
	    fuzzy = TRUE;
	    break;
	case 'q':
	    query = optarg;
	    break;
	default:
	    usage(argv[0]);
	}
    }
    if (query == NULL)
	usage(argv[0]);
    
    while (optind < argc) {
	if (!skadm(argv[optind], query, fuzzy))
	    ok = FALSE;
	putchar('\n');
	++optind;
    }

    return !ok;
}
