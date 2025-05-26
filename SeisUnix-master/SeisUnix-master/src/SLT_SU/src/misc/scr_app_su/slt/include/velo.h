/* Copyright (c)       , 1992	*/
/* All rights reserved.                       */

/* velo.h - include file for general purpose velocity subroutine */

#ifndef VELO_H
#define VELO_H


/* INCLUDES */

#include "par.h"


/* FUNCTION PROTOTYPES */

#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

void printvs3d(float x, float y,int nvt, int *times,int *vrms,FILE *outfp);
void printsc3d(float x,float y,int nvt,float *times,float *scales,FILE *outfp);
void printhvel(int cdp, int ntv, float *tout, float *vout, FILE *outfp);
void printhvelxy(int cdp, float x, float y, int ntv, 
	float *tout, float *vout, FILE *outfp);
void printhvel2(float line,float xline,int ntv,
	float *tout,float *vout,FILE *outfp);
void vs3dread(FILE *infp, float *xs, float *ys, float *ts, float *vs,
        int *nxy, int *nps, int maxp, int maxnxy);
void hvelread(FILE *infp, int *cdp, float *ts, float *vs,
        int *ncdp, int *nps, int maxnp, int maxncdp);

/* read in VELO cards */
void veloread(FILE *infp, int *cdps, float *zpicks, float *vpicks, int *ncdps,
        int *nps, int maxp, int maxcdp);
/* read in VELF cards */
void velfread(FILE *infp, int *cdps, float *tpicks, float *vpicks, int *ncdps,
      int *nps, int maxp, int maxcdp);
/* read in DVDZ cards */
void dvdzread(FILE *infp, int *cdps, float *ztps, float *zbts, float *dvdzs,
        int *ncdps, int *nps, int maxp, int maxcdp);
/* velocity conversion and interpolation */
void vconint(float *depth, float *vel, int npairs, float *z, float *vtable,
        int nz, int invtyp, int otvtyp, float *dvdz);
/* velocity gradient interpolation */
void dvdzint(float *ztop, float *zbot, float *dvdz, int npairs, float *z,
        float *dvdztable, int nz);
/* depth-velocity picks within a layer */
void zandva(float ztop, float zbot, float *zp, float *vp, int np,
        float *zz, float *vaa, int *nn);
/* velocity smoothing functions */ 
void vsm3d(float ***v, int n3, int n2, int n1, int iter, int depth,
         float r3, float r2, float r1, float lambda, int slowness);
void wavel(int n1, int n2, int n3, float d1, float d2, float d3,
        int time, float *wl, float ***v);
/* velocity inversion removal */
void removeinv(float *t, float *v, int *nps, int nf, int maxntv, int rminv);


#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* VELO_H */

