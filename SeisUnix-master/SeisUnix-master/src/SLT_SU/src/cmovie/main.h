

#ifndef MAIN_H
#define MAIN_H

/*--------------------------------------------------------------------*\
                          basic definitions
\*--------------------------------------------------------------------*/

/*----------------------------------------------*/
/* allocate memory for count objects of a class */
/*----------------------------------------------*/

#define NEW(class,var,count)    {\
    extern int _alloc;\
    var=(class)malloc((count)*sizeof(var[0]));\
    _alloc += (count)*sizeof(var[0]);\
    if (var == 0) err ("cant allocate %d bytes for var; %d already allocated",(count)*sizeof(var[0]),_alloc);\
    if (memwatch) (void)printf ("malloc %s=%d\n","var",(count)*sizeof(var[0]));\
    }\


/*---------------------*/
/* clear object memory */
/*---------------------*/

#define FILL(var,size,val)      {register byte bp, be; for (bp=(byte)var, be=bp+(size)*sizeof(var[0]); bp<be;) *bp++ = val;}


/*------------------------*/
/* release object storage */
/*------------------------*/

#define FREE(var)       if (var) {free(var); var=0; if (memwatch) printf ("free %s\n","var");}

/* alias message to user interface message */

/*----------------------*/
/* draw or erase choice */
/*----------------------*/

#define DRAW    1
#define ERASE   0
#define NO_INDEX    -1

/*--------*/
/* getpar */
/*--------*/

#ifdef SU

#define GETPARFLOAT(name,type,var)  getparfloat (name,var)
#define GETPARINT(name,type,var)    getparint (name,var)
#define GetparInit  initargs
#define GETPARSTRING(name,type,var)    getparstring (name,&var)



#else

#define GETPARSTRING    getpar
#define GETPARFLOAT     getpar
#define GETPARINT       getpar

#endif

extern int memwatch;

/* main.c */
int      main(int argc, char **argv);
void      MainFirst(void);

#endif
