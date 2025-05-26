/* cwpdefs.h - include file for CWP C programs
 *
 * $Author: jkc $
 * $Source: /src/general/include/RCS/cwpdefs.h,v $
 * $Revision: 2.12 $ ; $Date: 89/09/20 18:54:33 $
 */

#ifndef CWPDEFS_H
#define CWPDEFS_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>		/* for ushort, etc.	*/
#include "limits.h"
#include "portdefs.h"


/* TYPEDEFS */
typedef enum {false, true} bool;

typedef enum {BADFILETYPE = -1, TTY, DISK, DIRECTORY, TAPE, PIPE} filetype;

typedef char *string;

typedef struct {   /* complex type with float components */
	float r;   /* real part		*/
	float i;   /* imaginary part	*/
} fcomplex;

typedef union { /* pointer to arbitrary type */
	char	*s;
	short	*h;
	ushort	*u;
	long	*l;
	ulong	*v;
	int	*i;
	uint	*p;
	float	*f;
	double	*z;
} mixed;

typedef union { /* storage for arbitrary type */
	char	s[8];
	short	h;
	ushort	u;
	long	l;
	ulong	v;
	int	i;
	uint	p;
	float	f;
	double	z;
} value;


/* DEFINES */
#define		public
#define		private		static
#define		TRUE		(1)
#define		FALSE		(0)
#define		SUCCEED		(0)
#define		FAIL		(1)
#define		YES		(1)
#define		NO		(0)
#define		STDIN		(0)
#define		STDOUT		(1)
#define		STDERR		(2)
#define		SEEK_SET	(0)  /* seek relative to start of file	*/
#define		SEEK_CUR	(1)  /* seek relative to current position */
#define		SEEK_END	(2)  /* seek relative to end of file	*/
#define		EOL		('\n')
#define		EOS		('\0')
#define		EOP		('\014')

#define ISIZE	sizeof(int)
#define FSIZE	sizeof(float)
#define DSIZE	sizeof(double)
#define PISIZE	sizeof(int *)
#define PFSIZE	sizeof(float *)
#define PDSIZE	sizeof(double *)

#define SGN(x)		((x) < 0 ? -1.0 : 1.0)
#define ABS(x)		((x) < 0 ? -(x) : (x))

#define	MAX(x, y)	((x) > (y) ? (x) : (y))
#define	MIN(x, y)	((x) < (y) ? (x) : (y))

#define ISODD(n)	((n) & 01)
#define IN_RANGE(n, lo, hi)	((lo) <= (n) && (n) <= (hi))

#define	STREQ(s, t)	(strcmp(s, t) == 0)
#define	STRLT(s, t)	(strcmp(s, t) < 0)
#define	STRGT(s, t)	(strcmp(s, t) > 0)

#define	DIM(a)		(sizeof(a)/sizeof(a[0]))

#define	LOOPDN(r, n)		for ((r) = (n)+1; --(r)) > 0; )
#define	LOOPDN2(r, n, j)	for ((r) = (n) + (j); ((r) -= (j)) > 0; )

/* st is the structure tag or typedef, m is the field
 * To use, define the variable _aligned (cf. portdefs.h) as:
 * static STRICT_ALIGN _aligned = 0;
 */
#define	OFFSET(st,m) ((char*) &((st*)&_aligned)->m - (char*)(st*)&_aligned)

#define		LOWBYTE(w)	((w) & 0xFF)
#define 	HIGHBYTE(w)	LOWBYTE((w) >>8)
#define		ISNEGCHAR(c)	((c) & 0x80)
#define		SIGNEXTEND(c)	(~0xFF | (int) (c))



/* EXTERNS */

/* Zero-based vector and matrix allocation */
extern float *vec(), *re_vec(), **mat();
extern int *ivec(), *re_ivec(), **imat();
extern double *dvec(), *re_dvec(), **dmat();
extern void free_vec(), free_ivec(), free_dvec();
extern void free_mat(), free_imat(), free_dmat();

#endif CWPDEFS_H
