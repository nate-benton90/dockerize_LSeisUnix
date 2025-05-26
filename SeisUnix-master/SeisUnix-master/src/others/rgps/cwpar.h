/*----------------------------------------------------------------------
 *      RGPG (Reflection Geometry Parameter Solver)
 *      File 2:         CWPAR.H
 *      Copyright (c) Colorado School of Mines, 1989.
 *      All rights reserved.
 *
 *      This code is part of a program capable of calculating full sets of
 *      circle parameters from values for four independent ones, in all but
 *      seven cases out of 274 possible.  Inquiries should be addressed to:
 *
 *      Shelby C. Worley, Colorado School of Mines,
 *      Golden, CO 80401  (shelby@mines.colorado.edu)
 *-----------------------------------------------------------------------
 *
 * This is a header file for a local copy of copyrighted CWP UNIX-based
 * C code.  It's purpose is to gather into one place definitions and
 * prototypes for the many functions that I need in order to use a
 * effective command line parsing system.
 * The functions were selected from the following CWP authored files:
 * getpars.c, askdoc.c, selfdoc.c, subcalls.c, errpkge.c, syscalls.c
 *
 *----------------------------------------------------------------------*/
 
# ifndef        CWPAR_H
# define        CWPAR_H
 
/* INCLUDES */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <float.h>
#include <sys/types.h>
#include <stdarg.h>
# ifdef __MSDOS__
#   include <conio.h>      /* DOS-ONLY io package. */
# endif
 
 
/* DEFINITIONS */
        /* CWP.H        */
 
#define STREQ(s,t) (strcmp(s,t) == 0)
 
 
/* TYPEDEFS    */
        /* PAR.C        */
/* parameter table */
typedef struct {
        char *name;             /* external name of parameter   */
        char *asciival;         /* ascii value of parameter     */
} pointer_table;
 
 
# ifndef BOOL_DEF
# define BOOL_DEF 1
typedef enum {false=0, true=1} bool;
# endif
 
/*****  Prototypes from CWPAR.C  ******/
/* (Arranged in order of their source in CWP files.)  */
 
/* From askdoc.c        */
void askdoc(int flag);
 
/* From atopkge.c.      */
double eatod(char *s);
 
/* From getpars.c       */
int getnpar(int n, char *name, char *type, void *ptr);
int getpardouble(char *name, double *ptr);
int getparstring(char *name, char **ptr);
int getnpar(int n, char *name, char *type, void *ptr);
int countparname(char *name);
int countnparval(int n, char *name);
int countparval(char *name);
 
/* From initargs.c      */
void initargs  (int, char**);
 
/* From errpkge.c       */
void err(char *fmt, ...);
void warn(char *fmt, ...);
void syserr(char *fmt, ...);
 
/* From selfdoc.c       */
void selfdoc (void);
 
/* From subcalls.c      */
int efflush(FILE *stream);
void *emalloc(size_t size);
FILE *epopen(char *, char *);
int epclose(FILE *stream);
 
/* From ealloc.c & alloc.c      */
void *ealloc1(size_t n1, size_t size);
void *alloc1(size_t n1, size_t size);
 
#endif
