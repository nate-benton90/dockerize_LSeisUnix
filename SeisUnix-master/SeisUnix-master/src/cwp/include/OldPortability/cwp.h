/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.                       */

/* cwp.h - include file for general purpose CWP stuff */

#ifndef CWP_H
#define CWP_H


/* The bin directory--$B in the Makefiles--check when porting */
#define	CWPBIN	"/usr/local/bin"


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
#include "cwpport.h"	/* only for non-ANSI conforming systems */


/* TYPEDEFS */
 
typedef char *string;
typedef enum {false, true} bool;
typedef enum {BADFILETYPE = -1, TTY, DISK, DIRECTORY, TAPE, PIPE} filetype;
typedef struct _complexStruct { /* complex number */
	float r,i;
} complex;
typedef struct _bmstateStruct { /* big matrix state */
	int l1,l2,m1,m2,nbpe;
	char *p;
} bmstate;
typedef struct _QuestStateStruct { /* quantile estimator state */
	int m0,m1,m2,m3,m4;
	float q0,q1,q2,q3,q4,f0,f1,f2,f3,f4,d0,d1,d2,d3,d4;
} QuestState;
typedef union { /* pointer to arbitrary type */
	char *s;
	short *h;
	unsigned short *u;
	long *l;
	unsigned long *v;
	int *i;
	unsigned int *p;
	float *f;
	double *z;
} mixed;
typedef union { /* storage for arbitrary type */
	char s[8];
	short h;
	unsigned short u;
	long l;
	unsigned long v;
	int i;
	unsigned int p;
	float f;
	double z;
} value;


/* DEFINES */
 
#define PI (3.141592653589793)
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif
#define YES (1)
#define NO (0)
#define STDIN (0)
#define STDOUT (1)
#define STDERR (2)
#define	EOL ('\n')
#define	EOS ('\0')
#define	EOP ('\014')
#define ISIZE sizeof(int)
#define FSIZE sizeof(float)
#define DSIZE sizeof(double)
#define PISIZE sizeof(int *)
#define PFSIZE sizeof(float *)
#define PDSIZE sizeof(double *)
#define SGN(x) ((x) < 0 ? -1.0 : 1.0)
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define NINT(x) ((int)((x)>0.0?(x)+0.5:(x)-0.5))
#define	MAX(x,y) ((x) > (y) ? (x) : (y))
#define	MIN(x,y) ((x) < (y) ? (x) : (y))
#define ISODD(n) ((n) & 01)
#define IN_RANGE(n,lo,hi) ((lo) <= (n) && (n) <= (hi))
#define	STREQ(s,t) (strcmp(s,t) == 0)
#define	STRLT(s,t) (strcmp(s,t) < 0)
#define	STRGT(s,t) (strcmp(s,t) > 0)
#define	DIM(a) (sizeof(a)/sizeof(a[0]))
#define	LOOPDN(r,n) for ((r) = (n)+1; --(r)) > 0; )
#define	LOOPDN2(r,n,j) for ((r) = (n) + (j); ((r) -= (j)) > 0; )
/* Do NOT use next two to extract major & minor numbers if sys/types.h has
   major(x), minor(x) might also in places like <sys/param.h>, <sysmacros.h> */
#define LOWBYTE(w) ((w) & 0xFF)
#define HIGHBYTE(w) LOWBYTE((w) >>8)
#define ISNEGCHAR(c) ((c) & 0x80)
#define SIGNEXTEND(c) (~0xFF | (int) (c))


/* FUNCTION PROTOTYPES */

/* allocate and free multi-dimensional arrays */
void *alloc1 (size_t n1, size_t size);
void **alloc2 (size_t n1, size_t n2, size_t size);
void ***alloc3 (size_t n1, size_t n2, size_t n3, size_t size);
void ****alloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size);
void free1 (void *p);
void free2 (void **p);
void free3 (void ***p);
void free4 (void ****p);
int *alloc1int (size_t n1);
int **alloc2int (size_t n1, size_t n2);
int ***alloc3int (size_t n1, size_t n2, size_t n3);
float *alloc1float (size_t n1);
float **alloc2float (size_t n1, size_t n2);
float ***alloc3float (size_t n1, size_t n2, size_t n3);
double *alloc1double (size_t n1);
double **alloc2double (size_t n1, size_t n2);
double ***alloc3double (size_t n1, size_t n2, size_t n3);
complex *alloc1complex (size_t n1);
complex **alloc2complex (size_t n1, size_t n2);
complex ***alloc3complex (size_t n1, size_t n2, size_t n3);
void free1int (int *p);
void free2int (int **p);
void free3int (int ***p);
void free1float (float *p);
void free2float (float **p);
void free3float (float ***p);
void free1double (double *p);
void free2double (double **p);
void free3double (double ***p);
void free1complex (complex *p);
void free2complex (complex **p);
void free3complex (complex ***p);

/* complex number manipulation */
complex cadd (complex a, complex b);
complex csub (complex a, complex b);
complex cmul (complex a, complex b);
complex cdiv (complex a, complex b);
float fcabs (complex z);
complex cmplx (float re, float im);
complex cneg (complex z);
complex conjg (complex z);
complex csqrt (complex z);
complex cexp (complex z);
complex crmul (complex a, float x);

/* big matrix handler */
bmstate *bmalloc (int nbpe, int n1, int n2);
void bmfree (bmstate *state);
void bmread (bmstate *state, int dir, int k1, int k2, int n, void *v);
void bmwrite (bmstate *state, int dir, int k1, int k2, int n, void *v);

/* interpolation */
float fsinc (float x);
double dsinc (double x);
void mksinc (float d, int lsinc, float sinc[]);
void ints8r (int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, int nxout, float xout[], float yout[]);
void ints8c (int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, int nxout, float xout[], complex yout[]);
void intt8r (int ntable, float table[][8],
	int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, int nxout, float xout[], float yout[]);
void intt8c (int ntable, float table[][8],
	int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, int nxout, float xout[], complex yout[]);
void shfs8r (float dx, int nxin, float fxin, float yin[], 
	float yinl, float yinr, int nxout, float fxout, float yout[]);
void xindex (int nx, float ax[], float x, int *index);
void intl2b (int nxin, float dxin, float fxin,
	int nyin, float dyin, float fyin, signed char *zin,
	int nxout, float dxout, float fxout,
	int nyout, float dyout, float fyout, signed char *zout);
void intlin (int nin, float xin[], float yin[], float yinl, float yinr,
	int nout, float xout[], float yout[]);
void intcub (int ideriv, int nin, float xin[], float ydin[][4],
	int nout, float xout[], float yout[]);
void cakima (int n, float x[], float y[], float yd[][4]);
void cmonot (int n, float x[], float y[], float yd[][4]);
void csplin (int n, float x[], float y[], float yd[][4]);
void yxtoxy (int nx, float dx, float fx, float y[], 
	int ny, float dy, float fy, float xylo, float xyhi, float x[]);

/* Butterworth filters */
void bfhighpass (int npoles, float f3db, int n, float p[], float q[]);
void bflowpass (int npoles, float f3db, int n, float p[], float q[]);
void bfdesign (float fpass, float apass, float fstop, float astop,
	int *npoles, float *f3db);

/* differentiator approximations */
void mkdiff (int n, float a, float h, int l, int m, float d[]);
void mkhdiff (float h, int l, float d[]);

/* general signal processing */
void conv (int lx, int ifx, float x[], int ly, int ify, float y[],
	int lz, int ifz, float z[]);
void xcor (int lx, int ifx, float x[], int ly, int ify, float y[],
	int lz, int ifz, float z[]);
void hilbert (int n, float x[], float y[]);

/* sorting and searching */
void hpsort (int n, float a[]);
void qksort (int n, float a[]);
void qkfind (int m, int n, float a[]);
void qkisort (int n, float a[], int i[]);
void qkifind (int m, int n, float a[], int i[]);

/* statistics */
float quest (float p, int n, float x[]);
QuestState *questinit (float p, int n, float x[]);
float questupdate (QuestState *state, int n, float x[]);

/* Prime Factor FFTs */
int npfa (int nmin);
int npfao (int nmin, int nmax);
int npfar (int nmin);
int npfaro (int nmin, int nmax);
void pfacc (int isign, int n, complex z[]);
void pfarc (int isign, int n, float rz[], complex cz[]);
void pfacr (int isign, int n, complex cz[], float rz[]);
void pfa2cc (int isign, int idim, int n1, int n2, complex z[]);
void pfa2rc (int isign, int idim, int n1, int n2, float rz[], complex cz[]);
void pfa2cr (int isign, int idim, int n1, int n2, complex cz[], float rz[]);
void pfamcc (int isign, int n, int nt, int k, int kt, complex z[]);

/* BLAS (Basic Linear Algebra Subroutines adapted from LINPACK FORTRAN) */
int isamax (int n, float *sx, int incx);
float sasum (int n, float *sx, int incx);
void saxpy (int n, float sa, float *sx, int incx, float *sy, int incy);
void scopy (int n, float *sx, int incx, float *sy, int incy);
float sdot (int n, float *sx, int incx, float *sy, int incy);
float snrm2 (int n, float *sx, int incx);
void sscal (int n, float sa, float *sx, int incx);
void sswap (int n, float *sx, int incx, float *sy, int incy);
int idamax (int n, double *sx, int incx);
double dasum (int n, double *sx, int incx);
void daxpy (int n, double sa, double *sx, int incx, double *sy, int incy);
void dcopy (int n, double *sx, int incx, double *sy, int incy);
double ddot (int n, double *sx, int incx, double *sy, int incy);
double dnrm2 (int n, double *sx, int incx);
void dscal (int n, double sa, double *sx, int incx);
void dswap (int n, double *sx, int incx, double *sy, int incy);

/* LINPACK functions (adapted from LINPACK FORTRAN) */
void sgeco (float **a, int n, int *ipvt, float *rcond, float *z);
void sgefa (float **a, int n, int *ipvt, int *info);
void sgesl (float **a, int n, int *ipvt, float *b, int job);
void sqrdc (float **x, int n, int p, float *qraux, int *jpvt,
	float *work, int job);
void sqrsl (float **x, int n, int k, float *qraux,
	float *y, float *qy, float *qty,
	float *b, float *rsd, float *xb, int job, int *info);
void sqrst (float **x, int n, int p, float *y, float tol,
	float *b, float *rsd, int *k,
	int *jpvt, float *qraux, float *work);
void dgeco (double **a, int n, int *ipvt, double *rcond, double *z);
void dgefa (double **a, int n, int *ipvt, int *info);
void dgesl (double **a, int n, int *ipvt, double *b, int job);

/* other linear system solvers */
void stoepd (int n, double r[], double g[], double f[], double a[]);
void stoepf (int n, float r[], float g[], float f[], float a[]);
void vanded (int n, double v[], double b[], double x[]);
void vandef (int n, float v[], float b[], float x[]);
void tridif (int n, float a[], float b[], float c[], float r[], float u[]);

/* graphics utilities */
void rfwtva (int n, float z[], float zmin, float zmax, float zbase,
	int yzmin, int yzmax, int xfirst, int xlast,
	int wiggle, int nbpr, unsigned char *bits);
void scaxis (float x1, float x2, int *nxnum, float *dxnum, float *fxnum);
int yclip (int nx, float dx, float fx, float y[], float ymin, float ymax,
	float xc[], float yc[]);

/* special functions */
float airya (float x);
float airyb (float x);

/* timers */
float cpusec (void);
float wallsec (void);

/* pseudo-random numbers */
float franuni (void);
void sranuni (int seed);
float frannor (void);
void srannor (int seed);

/* miscellaneous */
void pp1d (FILE *fp, char *title, int lx, int ifx, float x[]);
void pplot1 (FILE *fp, char *title, int nx, float ax[]);
filetype filestat(int fd);
string printstat(int fd);


/* EXTERNS - for compatibility with some older programs till updated */
/* Zero-based vector and matrix allocation */
extern float *vec(), *re_vec(), **mat();
extern int *ivec(), *re_ivec(), **imat();
extern double *dvec(), *re_dvec(), **dmat();
extern void free_vec(), free_ivec(), free_dvec();
extern void free_mat(), free_imat(), free_dmat();

#endif /* CWP_H */
