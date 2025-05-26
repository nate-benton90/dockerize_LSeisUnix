/* Copyright (c) Colorado School of Mines, 1990. */
/* All rights reserved.                       */

/* par.h - include file for getpar, selfdoc, and error handling functions */

#ifndef PAR_H
#define PAR_H


/* INCLUDES */

#include "cwp.h"
#include <errno.h>
#include <fcntl.h>


/* GLOBAL DECLARATIONS */
 
#ifdef __cplusplus  /* if C++, use extern */
extern int xargc;  extern char **xargv;
#else
#if LIB_OBJ
extern int xargc;  extern char **xargv;
#else
int xargc;  char **xargv;
#endif

#endif


/* DEFINES */

/* getpar macros */
#define MUSTGETPARINT(x,y) if(!getparint(x,y)) err("must specify %s=",x)
#define MUSTGETPARFLOAT(x,y) if(!getparfloat(x,y)) err("must specify %s=",x)
#define MUSTGETPARSTRING(x,y) if(!getparstring(x,y)) err("must specify %s=",x)

/* obsolute macros - do not use these! */
#define MUSTSGETPAR(x,y) if(!sgetpar(x,y)) err("need %s=",x)
#define MUSTHGETPAR(x,y) if(!hgetpar(x,y)) err("need %s=",x)
#define MUSTUGETPAR(x,y) if(!ugetpar(x,y)) err("need %s=",x)
#define MUSTLGETPAR(x,y) if(!lgetpar(x,y)) err("need %s=",x)
#define MUSTVGETPAR(x,y) if(!vgetpar(x,y)) err("need %s=",x)
#define MUSTIGETPAR(x,y) if(!igetpar(x,y)) err("need %s=",x)
#define MUSTPGETPAR(x,y) if(!pgetpar(x,y)) err("need %s=",x)
#define MUSTFGETPAR(x,y) if(!fgetpar(x,y)) err("need %s=",x)
#define MUSTZGETPAR(x,y) if(!zgetpar(x,y)) err("need %s=",x)


/* define some variables for sun */ 
#define fpos_t long
#define SEEK_END 2

#ifdef PAR_CHECK

typedef struct {
   char* name;
   char  type;
   char* min;
   char* max;
}PAR;

#endif

/* FUNCTION PROTOTYPES */

#ifdef __cplusplus  /* if C++, specify external C linkage */
extern "C" {
#endif

/* getpar parameter parsing */
void initargs (int argc, char **argv);
int getparint (char *name, int *p);
int getparuint (char *name, unsigned int *p);
int getparshort (char *name, short *p);
int getparushort (char *name, unsigned short *p);
int getparlong (char *name, long *p);
int getparulong (char *name, unsigned long *p);
int getparfloat (char *name, float *p);
int getpardouble (char *name, double *p);
int getparstring (char *name, char **p);
int getnparint (int n, char *name, int *p);
int getnparuint (int n, char *name, unsigned int *p);
int getnparshort (int n, char *name, short *p);
int getnparushort (int n, char *name, unsigned short *p);
int getnparlong (int n, char *name, long *p);
int getnparulong (int n, char *name, unsigned long *p);
int getnparfloat (int n, char *name, float *p);
int getnpardouble (int n, char *name, double *p);
int getnparstring (int n, char *name, char **p);

int getpar(char *name, char *type, void *p) ;
int auxgetpar(char *aux, char *name, char *type, void *p) ;

int auxgetparint (char *aux, char *name, int *p);
int auxgetparuint (char *aux, char *name, unsigned int *p);
int auxgetparshort (char *aux, char *name, short *p);
int auxgetparushort (char *aux, char *name, unsigned short *p);
int auxgetparlong (char *aux, char *name, long *p);
int auxgetparulong (char *aux, char *name, unsigned long *p);
int auxgetparfloat (char *aux, char *name, float *p);
int auxgetpardouble (char *aux, char *name, double *p);
int auxgetparstring (char *aux, char *name, char **p);

int countparname (char *name);
int countparval (char *name);
int countnparval (int n, char *name);

/* obsolete getpar functions - do not use these! */
int hgetpar (char *s, short *p);
int ugetpar (char *s, ushort *p);
int igetpar (char *s, int *p);
int pgetpar (char *s, unsigned int *p);
int lgetpar (char *s, long *p);
int vgetpar (char *s, unsigned long *p);
int fgetpar (char *s, float *p);
int dgetpar (char *s, double *p);
int sgetpar (char *s, char **p);

/* errors and warnings */
void err (char *fmt, ...);
void warn (char *fmt, ...);

/* these two are OBSOLESCENT */
void syserr (char *fmt, ...);
void syswarn (char *fmt, ...);

/* self documentation */
void pagedoc (void);
void requestdoc (int i);

void selfdoc (void);
void askdoc (int i);

/* system calls with error trapping */
int ecreat(char *path, int perms);
int efork(void);
int eopen(char *path, int flags, int perms);
int eclose(int fd);
int eunlink(char *path);
long elseek(int fd, long offset, int origin);
int epipe(int fd[2]);
int eread(int fd, char *buf, int nbytes);
int ewrite(int fd, char *buf, int nbytes);
int pread(int fd, char *buf, int nbytes); /* NOT a sys call; drop this? */

/* system subroutine calls with error trapping */
FILE *efopen(const char *file, const char *mode);
FILE *efreopen(const char *file, const char *mode, FILE *stream1);
FILE *efdopen(int fd, const char *mode);
FILE *epopen(char *command, char *type);
int efclose(FILE *stream);
int epclose(FILE *stream);
int efflush(FILE *stream);
int eremove(const char *file);
int erename(const char *oldfile, const char* newfile);
int efseek(FILE *stream, long offset, int origin);
void erewind(FILE *stream);
long eftell(FILE *stream);
FILE *etmpfile(void);
FILE *etempfile(char *scrdir);
char *etempnam(char *scrdir, char *pfx);
char *etmpnam(char *namebuffer);
void *emalloc(size_t size);
void *erealloc(void *memptr, size_t size);
void *ecalloc(size_t count, size_t size);
int efgetpos(FILE *stream, fpos_t *position);
int efsetpos(FILE *stream, const fpos_t *position);
size_t efread(void *bufptr, size_t size, size_t count, FILE *stream);
size_t efwrite(void *bufptr, size_t size, size_t count, FILE *stream);
size_t pfread(void *bufptr, size_t size, size_t count, FILE *stream);

/* allocation with error trapping */
void *ealloc1 (size_t n1, size_t size);
void *erealloc1 (void *v, size_t n1, size_t size);
void **ealloc2 (size_t n1, size_t n2, size_t size);
void ***ealloc3 (size_t n1, size_t n2, size_t n3, size_t size);
void ****ealloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size);
int *ealloc1int(size_t n1);
int *erealloc1int(int *v, size_t n1);
int **ealloc2int(size_t n1, size_t n2);
int ***ealloc3int(size_t n1, size_t n2, size_t n3);
float *ealloc1float(size_t n1);
float *erealloc1float(float *v, size_t n1);
float **ealloc2float(size_t n1, size_t n2);
float ***ealloc3float(size_t n1, size_t n2, size_t n3);
double *ealloc1double(size_t n1);
double *erealloc1double(double *v, size_t n1);
double **ealloc2double(size_t n1, size_t n2);
double ***ealloc3double(size_t n1, size_t n2, size_t n3);
complex *ealloc1complex(size_t n1);
complex *erealloc1complex(complex *v, size_t n1);
complex **ealloc2complex(size_t n1, size_t n2);
complex ***ealloc3complex(size_t n1, size_t n2, size_t n3);

/* string to numeric conversion with error checking */
short eatoh(char *s);
unsigned short eatou(char *s);
int eatoi(char *s);
unsigned int eatop(char *s);
long eatol(char *s);
unsigned long eatov(char *s);
float eatof(char *s);
double eatod(char *s);

#ifdef __cplusplus  /* if C++ (external C linkage is being specified) */
}
#endif

#endif /* PAR_H */
