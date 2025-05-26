/* SUASCII: $Revision: 1.3 $ ; $Date: 90/12/01 10:55:38 $	*/

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
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUASCII - print non zero header values and data		\n"
" 								\n"
" suascii <stdin >ascii_file					\n"
" 								\n"
" Optional parameter:						\n"
"	bare=0		print headers and data			\n"
"			bare=1 print only data 			\n"
" 								\n"
" Notes: suwind/suus provide trace selection and/or subsampling.\n"
"	 with bare=1 traces are separated by a blank line.	\n"
" 						        	\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Jack
 *
 */


segy tr;

main(int argc, char **argv)
{
	int bare;

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get parameter */
	if (!getparint("bare", &bare))	bare=0;

	/* Loop over traces converting to ascii */
	while (gettr(&tr)) {
		register int i;

		if (!bare)  printheader(&tr);

		for (i = 0; i < (int) tr.ns; ++i) {
			if (!bare)  printf("%5d ", i+1);
			printf("%11.4e\n", tr.data[i]);
		}

		if (bare)  putchar('\n');

	}


	return EXIT_SUCCESS;
}
