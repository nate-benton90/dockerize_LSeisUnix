/* SEGYCLEAN: $Revision: 1.1 $ ; $Date: 91/03/03 14:42:21 $		*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *---------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SEGYCLEAN - zero out unassigned portion of header		\n"
" 								\n"
" segyclean <stdin >stdout 					\n"
" 								\n"
"								\n"
" Since \"foreign\" SEG-Y tapes	may use the unassigned portion	\n"
" of the trace headers and since SU now uses it too, this	\n"
" program zeros out the fields meaningful to SU.		\n"
" 								\n"
"  Example:							\n"
"  	segyread trmax=200 | segyclean | suximage		\n"
"								\n"
; /**************** end self doc ********************************/

/* Credits:
 *	CWP: Jack
 *
 */


segy tr;

/* Prototypes */


main(int argc, char **argv)
{

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	while (gettr(&tr)) {
		tr.f1 = 0.0;
		tr.d1 = 0.0;
		tr.f2 = 0.0;
		tr.d2 = 0.0;
		tr.ungpow = 0.0;
		tr.unscale = 0.0;
		tr.mark = 0;

		puttr(&tr);
	}


	return EXIT_SUCCESS;
}
