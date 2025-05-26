/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

/* par.h - include file for getpar, selfdoc, and error handling functions */

#ifndef PAR_H
#define PAR_H


/* INCLUDES */

#include "cwp.h"
#include <fcntl.h>	/* non-ANSI */
#include <unistd.h>	/* non-ANSI */
#include <sys/types.h>	/* non-ANSI */


/* GLOBAL DECLARATIONS */
extern int xargc; extern char **xargv;


/* TYPEDEFS */
typedef enum {false, true} cwp_Bool;
typedef char *cwp_String;

typedef enum {BADFILETYPE = -1,
	TTY, DISK, DIRECTORY, TAPE, PIPE, FIFO, SOCKET, SYMLINK} FileType;
	
/* define structures for Hale's modeling */
typedef struct ReflectorSegmentStruct {
	float x;	/* x coordinate of segment midpoint */
	float z;	/* z coordinate of segment midpoint */
	float s;	/* x component of unit-normal-vector */
	float c;	/* z component of unit-normal-vector */
} ReflectorSegment;
typedef struct ReflectorStruct {
	int ns;			/* number of reflector segments */
	float ds;		/* segment length */
	float a;		/* amplitude of reflector */
	ReflectorSegment *rs;	/* array[ns] of reflector segments */
} Reflector;
typedef struct WaveletStruct {
	int lw;			/* length of wavelet */
	int iw;			/* index of first wavelet sample */
	float *wv;		/* wavelet sample values */
} Wavelet;


/* DEFINES */

/* getpar macros */
#define MUSTGETPARINT(x,y) if(!getparint(x,y)) err("must specify %s=",x)
#define MUSTGETPARFLOAT(x,y) if(!getparfloat(x,y)) err("must specify %s=",x)
#define MUSTGETPARSTRING(x,y) if(!getparstring(x,y)) err("must specify %s=",x)

#define STDIN (0)
#define STDOUT (1)
#define STDERR (2)


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
int getparstringarray (char *name, char **p);
int getnparint (int n, char *name, int *p);
int getnparuint (int n, char *name, unsigned int *p);
int getnparshort (int n, char *name, short *p);
int getnparushort (int n, char *name, unsigned short *p);
int getnparlong (int n, char *name, long *p);
int getnparulong (int n, char *name, unsigned long *p);
int getnparfloat (int n, char *name, float *p);
int getnpardouble (int n, char *name, double *p);
int getnparstring (int n, char *name, char **p);
int getnparstringarray (int n, char *name, char **p);
int getnpar (int n, char *name, char *type, void *ptr);
int countparname (char *name);
int countparval (char *name);
int countnparval (int n, char *name);

/* For ProMAX */
void getPar(char *name, char *type, void *ptr);

/* errors and warnings */
void err (char *fmt, ...);
void syserr (char *fmt, ...);
void warn (char *fmt, ...);

/* self documentation */
void pagedoc (void);
void requestdoc (int i);

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
char *etmpnam(char *namebuffer);
void *emalloc(size_t size);
void *erealloc(void *memptr, size_t size);
void *ecalloc(size_t count, size_t size);
size_t efread(void *bufptr, size_t size, size_t count, FILE *stream);
size_t efwrite(void *bufptr, size_t size, size_t count, FILE *stream);

/* some SUN users may need to comment out the next two items, */
/* if your system does not have "fgetpos()" and "fsetpos()" defined. */
/* You will also need to comment out the lines defining the functions */
/* efgetpos() and efsetpos() in CWPROOT/src/par/lib/subcalls.c */

/* Modified: 21 June 1995: */
/* so that setting -DSUN_A in Makefile.config should make this unnecessary */
/* CWP: John Stockwell */

#ifndef SUN_A
int efgetpos(FILE *stream, fpos_t *position);
int efsetpos(FILE *stream, const fpos_t *position);
#endif

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

void ****ealloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size);
void *****ealloc5 (size_t n1, size_t n2, size_t n3, size_t n4, size_t n5, size_t size);
int ****ealloc4int(size_t n1, size_t n2, size_t n3, size_t n4);
int *****ealloc5int(size_t n1, size_t n2, size_t n3, size_t n4, size_t n5);
float ****ealloc4float(size_t n1, size_t n2, size_t n3, size_t n4);
float *****ealloc5float(size_t n1, size_t n2, size_t n3, size_t n4, size_t n5);

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

/* file type checking */
FileType filestat(int fd);
char *printstat(int fd);

/* Hale's modeling code */
void decodeReflectors (int *nrPtr,
	float **aPtr, int **nxzPtr, float ***xPtr, float ***zPtr);
int decodeReflector (char *string,
	float *aPtr, int *nxzPtr, float **xPtr, float **zPtr);
void breakReflectors (int *nr, float **ar, 
	int **nu, float ***xu, float ***zu);
void makeref (float dsmax, int nr, float *ar, 
	int *nu, float **xu, float **zu, Reflector **r);
void raylv2 (float v00, float dvdx, float dvdz,
	float x0, float z0, float x, float z,
	float *c, float *s, float *t, float *q);
void addsinc (float time, float amp,
	int nt, float dt, float ft, float *trace);
void makericker (float fpeak, float dt, Wavelet **w);


/* smoothing routines */
void dlsq_smoothing (int nt, int nx, int ift, int ilt, int ifx, int ilx,
        float r1, float r2, float rw, float **traces);
void SG_smoothing_filter (int np, int nl, int nr, int ld, int m, float *filter);
void rwa_smoothing_filter (int flag, int nl, int nr, float *filter);
void gaussian2d_smoothing (int nx, int nt, int nsx, int nst, float **data);
void gaussian1d_smoothing (int ns, int nsr, float *data);
void smooth_histogram (int nintlh, float *pdf);

#ifdef __cplusplus  /* if C++ (external C linkage is being specified) */
}
#endif

#endif /* PAR_H */
