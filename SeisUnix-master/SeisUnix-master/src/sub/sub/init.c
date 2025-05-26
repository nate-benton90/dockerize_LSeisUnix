#include "sub.h" 
#include "functions.h"
#include "x_tab.h"

static struct  {       /* Constants */
        char   *name;
        double cval;
} consts[] = {
       {"pi",    	3.14159265358979323846},
       {"enatural",     2.71828182845904523636},
       {"rad2deg",	57.29577951308232087860},  /* deg/radian */
       {0,       0}
};
typedef struct  {        /* Built-ins */
        char		*name;
        TPackage	(*func)();
} btab;

static btab builtins[] = {

        {"sin",		TPSin},
        {"cos",		TPCos},
        {"atan",       	TPAtan},
        {"log",        	TPLog},
        {"log10", 	TPLog10},
        {"exp",        	TPExp},
        {"sqrt", 	TPSqrt},
        {"int",        	TPInteger},
        {"nint", 	TPNInteger},
        {"abs",        	TPAbs},

        {"strtonum",	TPStrToNum},
        {"floattostr",	TPDNumToStr},
        {"inttostr",	TPINumToStr},
        {"flatten",	TPFlattened},

        {"time",	clocktime},
        {"random",	urandom},
        {"internals",	internals},
        {"exit",	bltin_exit},
        {"size",	object_size},
        {"strlen",	object_size},
        {"length",	object_size},

        {"nargs",	fArg},
        {"ntharg",	fArg},

        {"print",	TPPrint},
        {"fprint",	TPFprint},
	{"fgets",	TPFgets},

        {"popen",	TPPopen},
        {"pclose",	TPPclose},
	{"tmpfile",	TPTmpFile},
        {"fopen",	TPFopen},
        {"fclose",	TPFclose},
        {"fflush",	TPFflush},
	{"fseek",	TPFseek},
	{"ftell",	TPFtell},
	{"rewind",	TPRewind},
	{"fskip",	TPFskip},
	{"fgetbytes",	TPFGetBytes},
	{"fputbytes",	TPFPutBytes},
	{"system",	TPSystem},

        {"gettokenline",
	     		TPGetTokenLine},

        {"traceback",	traceback},

        {"fputrb",	TPPutRB},
	{"output",	TPPutRB},
	{"fgetrb",	TPGetRB},

        {"fvector",	fVector},
        {"sequence",	TPSequence},
        {"max",		TPMax},
        {"min",		TPMin},
        {"sum",		TPSum},

	{"pfnext",	TPpfnext},
	{"pfbest",	TPpfbest},
	{"extend",	TPextend},
	{"fdft",	TPfdft},
	{"idft",	TPidft},

        {"isdouble",	TPisDoubleP},
        {"isstring",	TPisStringP},
        {"isnothing",	TPisNothing},
        {"isarray",	TPisArrayP},
        {"isfvector",	TPisfVectorP},

        {0,		0}
};

static struct {
	char    *name;
	int     kval;
} keywords[] = {
        {"func",	FUNC},
        {"return",	RETURN_T},
        {"if",		IF_T},
        {"else",     	ELSE_T},
        {"while",    	WHILE_T},
        {"for",		FOR_T},
        {"read",	READ},
        {"auto",	AUTO},
        {0,          0},
};

void init(void)  /* install constants and built-ins in table */
{
    long i;
    Symbol *s;

    for (i = 0; keywords[i].name; i++) {
	s = installcons(keywords[i].name);
	s->ttype = keywords[i].kval;
    }
    
    for (i = 0; consts[i].name; i++) {
	s = installcons(consts[i].name);
	s->ttype = VAR;
	s->spk.u.val = consts[i].cval;
	s->spk.type = Double;
    }

    s = installcons("hardware");
    s->ttype = VAR;

/* think broader
#ifdef SUNSYSTEM
    s->spk = cTPackage("sun");
#else
    s->spk = cTPackage("cray");
#endif
*/
#ifdef SUNSYSTEM
    s->spk = cTPackage("sun");
#endif
#ifdef HPUXSYSTEM
    s->spk = cTPackage("hp");
#endif
#ifdef AIXSYSTEM
    s->spk = cTPackage("ibm");
#endif
#ifdef CONVEXSYSTEM
    s->spk = cTPackage("convex");
#endif
#ifdef CRAYSYSTEM
    s->spk = cTPackage("cray");
#endif

    s = installcons("nothing");
    s->ttype = VAR;
    s->spk.type = Nothing;

    s = installcons("stdin");
    s->ttype = VAR;
    s->spk.type = FileStar;
    s->spk.u.fstar = stdin;
    
    s = installcons("stdout");
    s->ttype = VAR;
    s->spk.type = FileStar;
    s->spk.u.fstar = stdout;
    
    s = installcons("stderr");
    s->ttype = VAR;
    s->spk.type = FileStar;
    s->spk.u.fstar = stderr;
    
    for (i = 0; builtins[i].name; i++) {
	s = installcons(builtins[i].name);
	s->ttype = BLTIN;
	s->spk.u.ptr = builtins[i].func;
	s->spk.type = InstP;
    }

}
