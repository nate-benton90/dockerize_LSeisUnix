/* wgc.h - include file for wgc code4 to sun format connversion functions */

#ifndef WGC_H
#define WGC_H


/* INCLUDES */

#include "cwp.h"
#include <errno.h>
#include <fcntl.h>


/* GLOBAL DECLARATIONS */
 
/* int xargc;  char **xargv; */


/* FUNCTION PROTOTYPES */

#ifdef __cplusplus  /* if C++, specify external C linkage */
extern "C" {
#endif

/* ascii to ebcdic */
int fascii_(unsigned char *in,unsigned char *out,int length,int ibitof); 
/* ebcdix to ascii */
int tascii_(unsigned char *in,unsigned char *out,int length,int ibiton); 

#ifdef __cplusplus  /* if C++ (external C linkage is being specified) */
}
#endif

#endif /* WGC_H */
