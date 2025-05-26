/* include file for extrapolation functions */

#ifndef EXTRAP_H
#define EXTRAP_H


/* INCLUDES */
#include "cwp.h"


/* TYPEDEFS */
typedef struct _eTableStruct { /* extrapolator table */
	int nhmax;	/* maximum half-length of extrapolators */
	int nwdxov;	/* number of frequency*dx/velocity values */
	float dwdxov;	/* frequency*dx/velocity sampling interval */
	float fwdxov;	/* first frequency*dx/velocity */
	float dzodx;	/* ratio of z to x sampling intervals:  dz/dx */
	int   iop;	/* Velocity option qP=1; qSV=2;		*/
	float vpo;      /* vertical P wave velocity		*/
	float vso;      /* vertical S wave velocity		*/
	float  ep;      /* Thomsen parameter epsilon		 */
	float del;      /* Thomsen parameter delta   	         */
	int *nh;	/* array[nwdxov] of half-lengths of extrapolators */
	complex **e;	/* array[nwdxov][nhmax] of extrapolators */
} eTable;


struct struct_type{
float vpo;	/* Vertical P-wave velocity  */
float vso;	/* Vertical S-wave velocity  */
float  ep;	/* Thomsen parameter epsilon */
float del;      /* Thomsen parameter delta   */
double **tr;    /* Derivative coefficients for real part of DEO */
double **ti;    /* Derivative coefficients for imaginary part of DEO */
double **av;    /* Derivative coefficients for filter terms          */
} ;


/* FUNCTIONS */
eTable *etmakvti (int nwdxov, float dwdxov, float fwdxov,
	float dzodx, int nhmax, int optimal, int iop,
	struct struct_type parm);
eTable *etread (FILE *fp);
void etwrite (eTable *et, FILE *fp);
void etextrap (eTable *et, int nx, float wdxov[], complex q[]);
void etextrap2 (eTable *et, int nx, int ny, float **wdxov, complex **q);

#endif /* EXTRAP_H */
