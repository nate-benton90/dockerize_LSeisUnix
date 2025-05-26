#ifndef BSWAP_H
#define BSWAP_H

#include "sucomp.h"

/* FUNCTION PROTOTYPES */
#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

void cmhconvert(char *cin, char *cout);
void bhconvert(char *cin,char *cout);
void trhdconvert(char *cin,char *cout);
void traceconvert(char *cin,char *cout,int nt);
void bswap8(char *cin,char *cout);
void bswap4(char *cin,char *cout);
void bswap2(char *cin,char *cout);

#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif
