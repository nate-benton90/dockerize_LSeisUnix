/* Copyright (c)        , 1992.*/
/* All rights reserved.                       */

/* comva.h - include file for comva package */

#ifndef COMVA_H
#define COMVA_H


/* INCLUDES */
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "par.h"

/* defines */
#define getghdr(x)    fgetghdr(stdin, (x))
#define putghdr(x)    fputghdr(stdout, (x))


/* FUNCTION PROTOTYPES */

#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

/* ifile/hfile read */
void ifileread(FILE *ifilefp, int *nhs, float *xpicks, float *zpicks,
        float *veltop, float *velbot, int *difs,
        float *dens, float *qval, float *pois,
        float *velavgs, float *dvdzs, char *hnames,
        int *hnums, int *npicks, int maxhs, int maxpks,
	float *xmin, float *xmax, float *zmin, float *zmax,
        int *cdpxmin, int *cdpxmax, float *vmin, float *vmax,
        char *vtype, char *dunits, char *zattrib, float *dcdp,
        int *ihfile);
/* read in VELO cards */ 
void veloread(FILE *infp, int *cdps, float *zpicks, float *vpicks, int *ncdps,
      	int *nps, int maxp, int maxcdp);
/* read in VELF cards */
void velfread(FILE *infp, int *cdps, float *tpicks, float *vpicks, int *ncdps,
      int *nps, int maxp, int maxcdp);
/* read in DVDZ cards */ 
void dvdzread(FILE *infp, int *cdps, float *ztps, float *zbts, float *dvdzs, 
	int *ncdps, int *nps, int maxp, int maxcdp);
/* read in DIPS cards */ 
void dipsread(FILE *infp, int *cdps, float *zps, float *dipl, float *dipr, 
	int *ncdps, int *nps, int maxp, int maxcdp);
/* dips grid */
void dipsgrid(char *dfile, float *dipl, float *dipr,
        float z0, float dz, int nz, int fcdp, int dcdp, int ncdp);
void dipsgrid_cpp(char *dfile, float *dipl, float *dipr,
        double z0, double dz, int nz, int fcdp, int dcdp, int ncdp);
/* velocity conversion and interpolation */
void vconint(float *depth, float *vel, int npairs, float *z, float *vtable,
	int nz, int invtyp, int otvtyp, float *dvdz);
/* velocity gradient interpolation */
void dvdzint(float *ztop, float *zbot, float *dvdz, int npairs, float *z,
	float *dvdztable, int nz);
/* compute midpoint coordinate from source and receiver coordinates */ 
float xmcor(int sx, int gx, short scalco);
/* estimate v(z) = v0 + a (z-z0) */
void estvi_(float *z, float *va, int *n, int *ibfit, float *v0, float *a); 
/* velocity conversion */
void vconvert(float *tin, float *vin, int nin, int ivtype, int ittype,
	float *tout, float *vout, int nout, int ovtype, int ottype);
/* depth-velocity picks within a layer */
void zandva(float ztop, float zbot, float *zp, float *vp, int np,
        float *zz, float *vaa, int *nn);


#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* COMVA_H */
