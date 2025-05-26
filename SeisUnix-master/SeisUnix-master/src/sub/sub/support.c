#include "sub.h"

char    *progname	= "";
int     lineno		= 1;
char    *infile		= "";

char*	LastPushedVar 	= 0;

UfhStateType UfhState = Idle;

static char* UfhStateName(UfhStateType s)
{
    switch( s ) {
      case Idle: return "idle";
      case Compiling: return "compiling a script";
      case Executing: return "executing a compiled script";
      default:
	execerror("UfhStateName: impossible state value");
    }
    return 0;
}

void execerror(va_alist)
va_dcl
{
    va_list ap;
    char* s;
    va_start(ap);
    s = va_arg(ap, char*);
    (void) fprintf(stderr, "\n%s:  ", progname);
    (void) vfprintf(stderr, s, ap);
    (void) fprintf(stderr, "\n");
    va_end(ap);

    fprintf(stderr, "Program was %s", UfhStateName(UfhState));
    if (infile) {
	fprintf(stderr, " in file \"%s\"", infile);
	fprintf(stderr, " near line %d", lineno);
    }
    fprintf(stderr, "\n");
    if(UfhState == Executing) {
	fprintf(stderr, "\tFunction: \"%s\"", currentFunction()->name);
	if(LastPushedVar != 0)
	    fprintf(stderr, "   Last variable seen: \"%s\".",
		    LastPushedVar);
	fprintf(stderr, "\n");
    }
    exitprocandexit(1);
}

void yyerror(char *s)     /* report compile-time error */ 
        
{
	execerror("%s", s);
}


char *emalloc(unsigned int n)        /* check return from malloc */
                   
{
        char *p;

        p = malloc(n);
        if (p == 0)
                execerror("emalloc: asked for %d; out of memory", n);
        return p;
}

int strcasecmp(const char* a, const char* b)
{
    while(*a != '\0') {
	if(*b == '\0') return toupper(*a);
	if(toupper(*a) != toupper(*b)) return(toupper(*a) - toupper(*b));
	a++;
	b++;
    }
    return -toupper(*b);
}

void exitproconly(void)
{
    ;
}

void exitprocandexit(int exitarg)
{
    exitproconly();
    exit(exitarg);
}
