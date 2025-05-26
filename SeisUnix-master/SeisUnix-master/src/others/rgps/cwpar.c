/*----------------------------------------------------------------------
 *  RGPG (Reflection Geometry Parameter Solver)
 *  File 4:     CWPAR.C
 *
 *  This code is part of a program capable of calculating full sets of
 *  circle parameters from values for four independent ones, in all but
 *  seven cases out of 274 possible.  Inquiries should be addressed to:
 *
 *  Shelby C. Worley, Colorado School of Mines,
 *  Golden, CO 80401  (shelby@mines.colorado.edu)
 *
 *---------------------------------------------------------------------------
 *  Copyright (c) Colorado School of Mines, 1989.
 *  All rights reserved.
 *                              CWPAR.C
 *      The functions in this file were borrowed with permission by
 *  Shelby Worley from a much larger user interface originated by the
 *  Stanford Exploration Project, and augmented by the Center for Wave
 *  Phenomena at the Colorado School of Mines.
 *      The complete interface is contained in "cwp.tar.Z" and is copyrighted
 *  material used by permission.
 *      The author would like to credit the following workers whose excellent
 *  work has relieved the author of the task of creating yet another user
 *  interface.
 *
 *  Stanford Exploration Project:
 *      Rob Clayton, Jon Clearbout, Stew Levin, Jeff Thorson,
 *      Einer Kjartessen, and Rick Ottolini
 *
 *  Center For Wave Phenomena:
 *      Jack Cohen, Dave Hale, Shuki Ronen, Brian Sumner
 *
 *  Amoco Houston Research Center:
 *      Lyle Meier
 *
 *      The following references were most invaluable in the creation of the
 *  full interface and are cited in the original files:
 *
 *      Plum: "Reliable Data Structures in C"
 *      Kernighan and Ritchie: "The C Programming Language"
 *      Kernighan and Pike, "The UNIX Programming Environment"
 *      Also Rochkind, "Advanced UNIX Programming"
 *      Mark Williams Company, "ANSI C--A Lexical Guide"
 *
 *-------------------------------------------------------------------------*/
 
#include "cwpar.h"
 
/* Temporary measure to get everything to standard output   */
# ifdef __MSDOS__
#   define    STD_ERR   stdout  /* For use in redirection       */
# else
#   define    STD_ERR   stderr
# endif
# define    ERROR   NULL
 
/***** GLOBAL DECLARATIONS *****/
int xargc;
char **xargv;
 
/* INTERNAL prototypes from GETPARS.C */
/* global variables declared and used internally */
static pointer_table *argtbl;   /* parameter table              */
static int nargs;               /* number of args that parse    */
static bool tabled = false;     /* true when parameters tabled  */
 
/* functions declared and used internally */
static int getparindex (int n, char *name);
static int getparinit(void);
static int tabulate (int argc, char **argv);
static int ccount (char c, char *s);
 
 
/*---------------------------------------------------------------------------*/
/* ASKDOC - give selfdoc on user request */
/*---------------------------------------------------------------------------*/
 
void askdoc(int flag)
{
    if (xargc <= flag) selfdoc();
}   /*  END askdoc()    */
 
 
/*---------------------------------------------------------------------------*/
/* ATOPKGE - convert ascii to arithmetic and with error checking */
/*---------------------------------------------------------------------------*/
 
 
double eatod(char *s)
{
    double db;
 
    db = strtod(s,(char **)NULL);
    if (errno==ERANGE)
        if (db==HUGE_VAL)
            err ("%s: eatod: overflow\n",__FILE__);
        else
            db = 0.0;
    return db;
}   /*  END eatod() */
 
/*---------------------------------------------------------------------------*/
/* from INITARGS.C */
/*---------------------------------------------------------------------------*/
 
void initargs(int argc, char **argv)
{
    xargc = argc; xargv = argv;
    return;
}   /*  END initargs()  */
 
/*---------------------------------------------------------------------------*/
/* GETPARS.C */
/*---------------------------------------------------------------------------*/
 
int getpardouble (char *name, double *ptr)
{
    return getnpar(0,name,"d",ptr);
}   /*  END getpardouble () */
 
int getparstring (char *name, char **ptr)
{
    return getnpar(0,name,"s",ptr);
}   /*  END getparstring () */
 
 
int getnpar (int n, char *name, char *type, void *ptr)
{
    int i;                  /* index of name in symbol table        */
    int nval;               /* number of parameter values found     */
    char *aval;             /* ascii field of symbol                */
 
    if (xargc == 1) return 0;
    if (!tabled) getparinit();/* Tabulate command line and parfile */
    i = getparindex(n,name);/* Get parameter index */
    if (i < 0) return 0;    /* Not there */
 
    /* handle string type as a special case, since a string
     * may contain commas.  Therefore, no arrays of strings. */
    if (type[0]=='s') {
        *((char**)ptr) = argtbl[i].asciival;
        return 1;
    }
    /* convert vector of ascii values to numeric values */
    for (nval=0,aval=argtbl[i].asciival; *aval; nval++) {
        if (type[0]=='d') {
            *(double*)ptr = eatod(aval);
            ptr = (double*)ptr+1;
        } else  {
            err("%s: invalid parameter type = %s",__FILE__,type);
        }
        while (*aval++ != ',')
            if (!*aval) break;
    }
    return nval;
}   /*  END getnpar ()  */
 
/* return number of occurences of parameter name */
int countparname (char *name)
{
    int i,nname;
 
    if (xargc == 1) return 0;
    if (!tabled) getparinit();
    for (i=0,nname=0; i<nargs; ++i)
    if (!strcmp(name,argtbl[i].name))
        ++nname;
    return nname;
}   /*  END countparname () */
 
/* return number of values in n'th occurence of parameter name */
int countnparval (int n, char *name)
{
    int i;
 
    if (xargc == 1) return 0;
    if (!tabled) getparinit();
    i = getparindex(n,name);
    if (i>=0)
        return ccount(',',argtbl[i].asciival) + 1;
    else
        return 0;
}   /*  END countnparval () */
 
/* return number of values in last occurence of parameter name */
int countparval (char *name)
{
    return countnparval(0,name);
}   /*  END countparval ()  */
 
/*
 * Return the index of the n'th occurence of a parameter name, 
 * except if n==0, return the index of the last occurence.
 * Return -1 if the specified occurence does not exist.
 */
static int getparindex (int n, char *name)
{
    int i;
    if (n==0) {
        for (i=nargs-1; i>=0; --i)
        if (!strcmp(name,argtbl[i].name)) break;
            return i;
    } else {
        for (i=0; i<nargs; ++i)
            if (!strcmp(name,argtbl[i].name))
                if (--n==0) break;
                    if (i<nargs)
                        return i;
                    else
                        return -1;
    }
}   /*  END getparindex ()  */
 
/* Initialize getpar */
static int getparinit (void)
{
    static int targc;       /* total number of args                 */
    static char **targv;    /* pointer to arg strings               */
    int argstrlen;
    char *argstr, *pargstr; /* storage for command line and
                                parameter file args                 */
    int i, j;               /* counters                             */
    int tabulate();         /* install symbol table                 */
 
    tabled = true;  /* remember table is built  */
 
    /* Check if xargc was initiated */
    if(!xargc)
        err("%s: xargc=%d -- not initiated in main", __FILE__, xargc);
 
    /* Space needed for command lines */
    for (i = 1, argstrlen = 0; i < xargc; i++) {
        argstrlen += strlen(xargv[i]) + 1;
    }
 
    /* Allocate space for command line and parameter file
        plus nulls at the ends to help with parsing. */
    /* argstr = (char *) calloc((size_t) (1+argstrlen+1), 1); */
    argstr = (char *) ealloc1(1+argstrlen+1, 1);
 
 
    /* Total arg count */
    targc = xargc - 1;
 
    /* Allocate space for total arg pointers */
    targv = (char **) ealloc1(targc, sizeof(char*));
 
    /* Copy command line arguments */
    for (i=0, j = 1, pargstr = argstr + 2; j < xargc; j++) {
# ifdef __MSDOS__
        strcpy(pargstr,xargv[j]);
        strlwr(pargstr);    /* For upper case barbarian users. */
# else
        strcpy(pargstr,xargv[j]);
# endif
        targv[i++] = pargstr;
        pargstr += strlen(xargv[j]) + 1;
    }
 
    /* Allocate space for the pointer table */
    argtbl = (pointer_table*) ealloc1(targc, sizeof(pointer_table));
 
    /* Tabulate targv */
    tabulate(targc, targv);
    return 0;
}   /*  END getparinit ()   */
 
/* Tabulate parameters */
static int tabulate (int argc, char **argv)
{
    int i;
    char *eqptr;
 
    for (i = 0, nargs = 0 ; i < argc; i++) {
        eqptr = strchr(argv[i], '=');
        if (eqptr) {
            argtbl[nargs].name = argv[i];
            argtbl[nargs].asciival = eqptr + 1;
            *eqptr = (char)0;
            nargs++;
        }
    }
    return 0;
}   /*  END tabulate () */
 
/* Count characters in a string */
static int ccount (char c, char *s)
{
    int i, count;
    for (i = 0, count = 0; s[i] != 0; i++)
        if(s[i] == c)
            count++;
    return count;
}   /*  END ccount ()   */
 
/*---------------------------------------------------------------------------*/
/* ERRPKGE - routines for reporting errors */
/*---------------------------------------------------------------------------*/
 
extern int errno;
 
void err(char *fmt, ...)
{
    va_list args;
 
    if (EOF == fflush(stdout))
        fprintf(STD_ERR, "\nerr: fflush failed on stdout");
# ifdef __MSDOS__
    fprintf(STD_ERR, "%s: ", strrchr(xargv[0],'\\')+1);
# else
    fprintf(stderr, "\n%s: ", xargv[0]);
# endif
    va_start(args,fmt);
    vfprintf(STD_ERR, fmt, args);
    va_end(args);
    if (errno) fprintf(STD_ERR, " (%s)", strerror(errno));
        fprintf(STD_ERR, "\n\n");
    exit(1);
}   /*  END err()   */
 
 
 
void warn(char *fmt, ...)
{
    va_list args;
 
    if (EOF == fflush(stdout))
        fprintf(STD_ERR, "\nwarn: fflush failed on stdout");
# ifdef __MSDOS__
    fprintf(STD_ERR, "\n%s: ", strrchr(xargv[0],'\\')+1);
# else
    fprintf(stderr, "\n%s: ", xargv[0]);
# endif
    va_start(args,fmt);
    vfprintf(STD_ERR, fmt, args);
    va_end(args);
    fprintf(STD_ERR, "\n\n");
    return;
}   /*  END warn()  */
 
void syserr(char *fmt, ...)
{
    va_list args;
 
    if (EOF == fflush(stdout))
        fprintf(STD_ERR, "\nsyserr: fflush failed on stdout");
    fprintf(STD_ERR, "\n%s: ", xargv[0]);
    va_start(args,fmt);
    vfprintf(STD_ERR, fmt, args);
    va_end(args);
    fprintf(STD_ERR, " (%s)\n", strerror(errno));
    exit(EXIT_FAILURE);
}   /*  END syserr()    */
 
 
/*---------------------------------------------------------------------------*/
/* SELFDOC - print self documentation string */
/*---------------------------------------------------------------------------*/
 
# ifdef __MSDOS__
 
void selfdoc(void)
{
    extern char *sdoc;
    char buff[80], c, *pd, *ps;
    int i, j, len;
 
    efflush(stdout);
    fputs("\nSend to printer ? [y|N] ",stderr);
    c = toupper(getch());
    if (c=='Y')
        fputs(sdoc,stdprn);
    else  {
        fputs("\n\n\n",stdout);
        len = strlen(sdoc);
        pd = buff;
        ps = sdoc;
        for (i=0, j=1; i<len; i++)  {
            if ( (*pd++ = *ps++) == '\n')  {
                *pd = '\0';
                fputs(buff,stdout);
                pd = buff;
                if (++j == 21 )  {
                    j = 0;
                    (void) getch();
                }
            }
        }
    }
    exit(EXIT_FAILURE);
}   /*  END (MS-DOS)selfdoc()   */
 
# else
 
void selfdoc(void)
{
    extern char *sdoc;
    FILE *fp;
 
    efflush(stdout);
    fp = epopen("more -18 1>&2", "w");
    (void) fprintf(fp, "%s", sdoc);
    epclose(fp);
 
    exit(EXIT_FAILURE);
}   /*  END (UNIX)selfdoc()   */
 
# endif
 
/*---------------------------------------------------------------------------*/
/* SUBCALLS - routines for system functions with error checking */
/*---------------------------------------------------------------------------*/
 
int efflush(FILE *stream)
{
    int status;
 
    if (EOF == (status = fflush(stream)))
        err("%s: efflush: fflush failed", __FILE__);
    return status;
}   /*  END efflush()   */
 
void *emalloc(size_t size)
{
    void *memptr;
 
    if (ERROR == (memptr = malloc(size)))
        err("%s : emalloc: malloc failed", __FILE__);
    return memptr;
}   /*  END emalloc()   */
 
# ifndef __MSDOS__
FILE *epopen(char *command, char *type)
{
        FILE *stream;
 
        if (ERROR == (stream = popen(command, type)))
                      err("%s: epopen: popen failed", __FILE__);
        return stream;
}   /*  END epopen()    */
 
int epclose(FILE *stream)
{
        int status;
 
        if (EOF == (status = pclose(stream)))
                      err("%s: epclose: pclose failed", __FILE__);
        return status;
}   /*  END epclose()   */
# endif
 
/*---------------------------------------------------------------------------*/
/* EALLOC.C */
/*---------------------------------------------------------------------------*/
 
/* allocate a 1-d array */
void *ealloc1 (size_t n1, size_t size)
{
    void *p;
 
    if (ERROR == (p=alloc1(n1, size)))
        syserr("%s: alloc1 failed", __FILE__);
    return p;
}   /*  END ealloc1 ()  */
 
/* ALLOC.C */
void *alloc1 (size_t n1, size_t size)
{
    return (malloc(n1*size));
}   /*  END alloc1 ()   */
 
 
