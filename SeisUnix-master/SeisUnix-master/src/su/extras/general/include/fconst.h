/* fconst.h - include file for Fortran constants used from C
 *
 * $Author: jkc $
 * $Source: /src/general/include/RCS/fconst.h,v $
 * $Revision: 1.3 $ ; $Date: 88/12/20 22:04:06 $
 */

#ifndef FCONST_H
#define FCONST_H


/* DECLARATIONS */
extern int f_one;
extern int f_zero;
extern int f_mone;
extern float f_fone;
extern float f_fzero;
extern float f_fmone;


/* DEFINES */
#define ONE	(&f_one)
#define ZERO	(&f_zero)
#define MONE	(&f_mone)
#define FONE	(&f_fone)
#define FZERO	(&f_fzero)
#define FMONE	(&f_fmone)

#endif FCONST_H
