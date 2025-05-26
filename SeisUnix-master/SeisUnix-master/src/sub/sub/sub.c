#include "sub.h"
#include "version.h"
extern int yydebug;
extern int printSymbols;


extern int getopt (int, char *const *, const char *);

int main(int argc, char **argv)
{
    extern int gargc;
    extern char** gargv;
    extern char    *progname;

    int c;
    static char* optstring = "vsV";
    extern int optind;

#ifdef ENABLE_DEBUGGING
    yydebug = 1;
#endif
    progname = argv[0];

    while((c = getopt(argc, argv, optstring)) != -1)
	switch(c) {
	  case 'v':
	    verbose = 1;
	    break;
	  case 's':
	    printSymbols = 1;
	    break;
	case 'V':
	    fprintf(stderr,
		    "sub info: version %s, patch level %d, rev. data %s\n",
		    VERSION, PATCHLEVEL, REVDATE);
	    return 0;
	  default:
	    break;
	}

    if(optind >= argc) {
	fprintf(stderr, "sub: nothing to do (no script).\n");
	return (1);
    } else {
	gargv = argv + optind;
	gargc = argc - optind;
    }
    init();
    return run();
}

