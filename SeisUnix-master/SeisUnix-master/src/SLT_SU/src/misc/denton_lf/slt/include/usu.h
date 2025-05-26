/* Copyright (c)        , 1992.*/
/* All rights reserved.                       */

/* usua.h - include file for       /su package */

#ifndef USU_H
#define USU_H


/* INCLUDES */
#include "velo.h"
#include "grid.h"
#include "su.h"
#include "par.h"

/* defines */


/* FUNCTION PROTOTYPES */

#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

/* compute midpoint coordinate from source and receiver coordinates */ 
float xmcor(long sx, long gx, short scalco);
/* (x,y) to (s,l) conversion */
void xy2sl(float x1, float y1, float s1, float l1,
           float x2, float y2, float s2, float l2,
           float x3, float y3, float s3, float l3,
           float x,  float y,  float *s, float *l);
void xy2sldb(double x1, double y1, double s1, double l1,
           double x2, double y2, double s2, double l2,
           double x3, double y3, double s3, double l3,
           double x,  double y,  double *s, double *l);
void sl2xy(float s1, float l1, float x1, float y1,
           float s2, float l2, float x2, float y2,
           float s3, float l3, float x3, float y3,
           float s,  float l,  float *x, float *y);
void sl2xydb(double s1, double l1, double x1, double y1,
           double s2, double l2, double x2, double y2,
           double s3, double l3, double x3, double y3,
           double s,  double l,  double *x, double *y);
/* dip filtering */
void dpf_(int *nslopes,float *smin,float *ds,float *amps,float *bias,
	  int *nt,float *dt,int *nx,float *dx,float *data, complex *cpfft,
	  float *pfft,int *ntfft,int *nxfft);
/* 2D or 3D rho filter */
void f2n3_(float *trace,int *n,int *i3d,int *m,int *l,int *ifirst);
void f2p5_(float *trace,int *n);

/* rmo program subroutine */
void taper(int nl, int n, int nh, float *r);
void bldmd_(complex *arayln, complex *fslice, complex *fpspac,
        int *nrcut, int *nr, int *noff);
void analy_(complex *arayln, complex *arayll, complex *fslice,
        float *wrkbuf, complex *fpspac, float *offset,
        float *pvstart, float *pvincs, int *npvsmp, int *noff,
        float *sfereq, float *prw);
void rmo(float **trg, float **trm, int nt, float dt, float *offset, int noff,
        float tmin, float tmax, float rmin, float rmax, float rbeg, float rend,
        float fmin, float fmax, float ttaper, float amp, int mult, float prw);

/* tar three files to/from a disk dataset name */
void tar3to(char *dsn, char *file1, char *file2, char *file3);
void tar3fr(char *dsn, char *file1, char *file2, char *file3);

/* dipscan subroutine */
void dipscn(float *data, float *amp, float *work, int *nt, int *nx, int *hy,
		float *pxo, float *pyo, float *amo, int *ipow,
		int *ixo, int *dxo, int *nxo,
		float *to, float *px, float *py, int *nto, int *npx, int *npy,
		int *ht, int *hx, float *tt, float *tx, float *ty,
		float *perc, int *ipxo, int *ipyo, float *wk1, float *wk2);

/* integer to value conversion */
void itov(String type, Value *val, int f);

#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* USU_H */
