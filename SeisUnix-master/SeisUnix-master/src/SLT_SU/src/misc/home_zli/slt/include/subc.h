/* Copyright (c)       , 1992	*/
/* All rights reserved.                       */

/* subc.h - include file for general purpose c subroutine */

#ifndef SUBC_H
#define SUBC_H


/* INCLUDES */

#include "par.h"


/* FUNCTION PROTOTYPES */

#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

/* detect if an ieee floating point number is finite */
int ieeefinite(float *x);

/* fortran callable print messages */ 
void msgsc (char *message, float ff);
void msgscf (char *message, float ff);
void msgsci (char *message, int ff);
void msgscs (char *message, char *str);

/* fortran callable complex to complex 2d fft */
void pfa2cc_(int *isign,int *idim,int *n1,int *n2,complex *cz);
/* fortran callable complex to real 2d fft */
void pfa2cr_(int *isign,int *idim,int *n1,int *n2,complex *cz,float *rz);

/* a fortran interface to cwp's qkisort c subroutine */
void qkisort_(int *n, float *a, int *i);
void qkfind_(int *m, int *n, float *a);

/* linear interpolation */
void intsln (int nxin, float dxin, float fxin, float yin[],
        float yinl, float yinr, int nxout, float xout[], float yout[]);

/* linear fit */
void linefit(float *x, float *y, int n, float *a, float *b);

#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* SUBC_H */
