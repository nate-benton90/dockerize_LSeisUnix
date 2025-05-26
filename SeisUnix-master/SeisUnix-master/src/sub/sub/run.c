#include "sub.h"
#include "functions.h"
#include "support.h"
#include "x_tab.h"

char 	**gargv;                /* global argument list */
int 	gargc;

int 	printSymbols = 0;

extern FILE	*fin;	/* input file pointer */

void fpecatch(int sig)    /* catch floating point exceptions */
{
       execerror("floating point exception caught by fpecatch()");
}

int run(void)   /* execute until EOF */
{
    Symbol*	Begin;
    Symbol*	OnTrace;
    Symbol*	End;

    Symbol*	Tr;
    Symbol*	Argc;
    Symbol*	Argv;
    TPackage	av;
    int		i;

    Symbol*	s;

    signal(SIGFPE, fpecatch);

    UfhState = Compiling;

    if((fin=fopen(gargv[0], "r")) == NULL) {
	fprintf(stderr, "%s: can't open %s\n", progname, infile);
	exit(1);
    }

    Argc = install("argc");
    Argc->ttype = VAR;
    Argc->spk = dTPackage((double) gargc);

    Argv = install("argv");
    Argv->ttype = VAR;
    av.type = TPackageP;
    av.size = gargc;
    av.u.tpa = (TPackage*) emalloc(av.size * sizeof(TPackage));
    for(i = 0; i < gargc; i++)
	av.u.tpa[i] = cTPackage(gargv[i]);
    Argv->spk = av;

    Tr = install("tr");
    Tr->ttype = VAR;

    initcode();

    s = installlc("SourceText");
    s->ttype = VAR;
    s->spk = cTPackage("");

    yyparse();

    if(printSymbols) {
	dumpSymbols(stderr);
	return 0;
    }

    infile = 0;

    s = lookup("sourcetext");
    s->spk = cTPackage(SourceText);

    Begin = lookup("begin");
    if( (Begin != NULL) && (Begin->ttype != FUNCTION) )
	return 1;

    OnTrace = lookup("ontrace");
    if( (OnTrace != NULL) && (OnTrace->ttype != FUNCTION) )
	return 1;

    End = lookup("end");
    if( (End != NULL) && (End->ttype != FUNCTION) )
	return 1;

    UfhState = Executing;

    if(Begin != NULL)
	callSymbol(Begin, 0);

    for(;;) {
	deleteTP(Tr->spk);
	Tr->spk = TPGetRBChunk(stdin);
	if(Tr->spk.type == Nothing)
	    break;
	if(OnTrace != NULL)
	    callSymbol(OnTrace, 0);
    }
    if(End != NULL)
	callSymbol(End, 0);
    exitproconly();
    return 0;
}

