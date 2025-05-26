/* Copyright (c)        , 1992.*/
/* All rights reserved.                       */

/* usgrid.h - include file for unscaled grid header */

#ifndef USGRID_H
#define USGRID_H


/* INCLUDES */
#include "ghdr.h"
#include "gridhd.h"
#include "usgridhd.h"
#include "par.h"

/* defines */
#define getusghdr(x)    fgetusghdr(stdin, (x))
#define putusghdr(x)    fputusghdr(stdout, (x))


/* FUNCTION PROTOTYPES */

#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

void ghed2usghed(ghed *gh, usghed *usgh);
void usghed2ghed(usghed *usgh, ghed *gh);
int fgetusghdr(FILE *gfp, usghed *usgh);
int fputusghdr(FILE *gfp, usghed *usgh);

#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* USGRID_H */
