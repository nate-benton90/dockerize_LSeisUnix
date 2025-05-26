/* Copyright (c)        , 1992.*/
/* All rights reserved.                       */

/* grid.h - include file for grid package */

#ifndef GRID_H
#define GRID_H


/* INCLUDES */
#include "ghdr.h"
#include "gridhd.h"
#include "par.h"

/* defines */
#define getghdr(x)    fgetghdr(stdin, (x))
#define putghdr(x)    fputghdr(stdout, (x))


/* FUNCTION PROTOTYPES */

#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

/* grid file routines */
int fgetghdr(FILE *gfp, ghed *gh);
int fputghdr(FILE *gfp, ghed *gh);
void fromghdr(ghed *gh, float *scale, int *dtype, int *n1, int *n2, int *n3,
        int *n4, int *n5, float *d1, float *d2, float *d3, float *d4,
        float *d5, float *o1, float *o2, float *o3, float *o4, float *o5,
        float *dcdp2, float *dline3, float *ocdp2, float *oline3,
	float *gmin, float *gmax, int *orient, int *gtype);
void toghdr(ghed *gh, float *scale, int *dtype, int *n1, int *n2, int *n3,
        int *n4, int *n5, float *d1, float *d2, float *d3, float *d4,
        float *d5, float *o1, float *o2, float *o3, float *o4, float *o5,
        float *dcdp2, float *dline3, float *ocdp2, float *oline3,
	float *gmin, float *gmax, int *orient, int *gtype);
void getgval(ghed *gh, char *key, float *val);
void putgval(ghed *gh, char *key, float val);
int getgindex(char *key);
char *getgkey(int index);
char *ghdtype(char *key);
void sminmax(short *sa, int n, float *gmin, float *gmax);
void fminmax(float *fa, int n, float *gmin, float *gmax);
void cminmax(complex *ca, int n, float *gmin, float *gmax);

#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* GRID_H */
