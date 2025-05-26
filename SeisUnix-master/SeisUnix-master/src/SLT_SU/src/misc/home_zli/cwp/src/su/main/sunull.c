/* SUNULL: $Revision: 1.1 $ ; $Date: 91/03/22 16:41:17 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUNULL - create null (all zeroes) traces	 		\n"
" 								\n"
" sunull count= >outdata					\n"
" 								\n"
" Required parameter						\n"
" 	nt=		number of samples per trace		\n"
" 								\n"
" Optional parameters						\n"
" 	ntr=5		number of null traces to create		\n"
" 	dt=0.004	time sampling interval			\n"
" 								\n"
"Rationale: It is sometimes useful to insert null traces	\n"
"	 between \"panels\" in a shell loop.			\n"
" 								\n"
" See also: sukill, sumute, suzero				\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Jack
 *
 */


segy tr;

main(int argc, char **argv)
{
	int nt, ntr;
	register int i, itr;
	float dt;
	int idt;
	
	/* Initialize */
	initargs(argc, argv);
	askdoc(0);

	/* Get parameters */
	MUSTGETPARINT("nt", &nt);
	if (!getparint("ntr", &ntr))	ntr = 5;
	if (!getparfloat("dt", &dt))	dt = .004;
	idt = 1000000.0 * dt;

	/* Main loop over traces */
	for (itr = 0; itr < ntr; ++itr) {
		for (i = 0; i <= nt; ++i)  tr.data[i] = 0.0;
		tr.tracl = itr + 1;
		tr.ns = nt;
		tr.dt = idt;
		puttr(&tr);
	};


	return EXIT_SUCCESS;
}
