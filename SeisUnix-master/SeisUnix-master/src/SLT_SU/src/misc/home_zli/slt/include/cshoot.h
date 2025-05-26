
/* cshoot.h - include file for cshoot */

/* author:	Zhiming Li 	*/
/* 2/10/93 */

#ifndef CSHOOT_H
#define CSHOOT_H

/* FUNCTION PROTOTYPES */

#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

void cshoot(float *xint, float *zint, float *vint, int maxspl, 
	int nint, int *nspls,
        double *a0, double *a1, double *a2, double *a3,
        float *sign, int *norder, int flag,
        float xs, float angmin, float dang, int nang, float *v,
        float xstart, float xend, float dx, 
        float *xray, float *zray, float *tray, int *npts);
void ray2grd(float *xray, float *zray, float *tray, int *npts,
        int nray, int maxp,
        float sx, float sz, float treplace,
        float x0, float z0, float dx, float dz, int nx, int nz,
        float *tgrid, float maxfac);


#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* CSHOOT_H */

