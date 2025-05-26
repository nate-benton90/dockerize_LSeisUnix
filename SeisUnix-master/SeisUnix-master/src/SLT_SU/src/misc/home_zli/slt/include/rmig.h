
/* rmig.h - include file for rmig */

/* author:	Zhiming Li 	*/
/* 11/24/92 */

#ifndef RMIG_H
#define RMIG_H


/* INCLUDES */
#include "par.h"

/* FUNCTION PROTOTYPES */

#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

void rdlshd(int *ns, int *nl, int *ne, int maxnl, FILE *fp, float *vel);
void rdxt(int ne, int ns, int np, int *nr, float *xs, float *zs, 
	float *xr, float *zr, int *etype, float *xe, float *ze, float *t,
	int *nep, int *ner, int srd0, int srdinc, int nsrd, 
	FILE *fp, int maxnr);
void wtxt(int ne, int ns, int np, int *nr, float *xs, float *zs, 
	float *xr, float *zr, int *etype, float *xe, float *ze, float *t,
	int *nep, int *ner, int srd0, int srdinc, int nsrd, 
	FILE *fp, int maxnr, int nl, float *vel);
void getxrt(FILE *fp, int maxnr, int *nevent, int *nxt,
        float *xrs, float *trs, float *xhs, float *zhs, int sread);
void xt2xrt(int maxnr,int ne, int np, int ppos,
        float *xe, float *ze, float *t, int *ner, int *nep,
        float *xrs, float *trs, float *xhs, float *zhs, int *nxt, int *iray);
void xtupdate(int maxnr,int ne, int np, int ppos,
        float *xe, float *ze, float *t, int *ner, int *nep,
        int *iray);
void findrs(float *r,int nr, float s,
        float *r1, float *r2, float *r3, float *r4, float *dr); 

void evsel(int ng, int ne, int maxnr, int maxnes, int mmap,
        float *sx, float *gx,
        float *xr, float *ts, float *xhs, float *zhs, int *nxt, int *iray,
        float *xe, float *ze, float *te, int *nes, int *ier, int *iee,
        int sortz, float aper);
void taper(int nt, float t0, float dt, float *trace, int nray, float *tray,
        float lpass, float ltaper);



#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* RMIG_H */

