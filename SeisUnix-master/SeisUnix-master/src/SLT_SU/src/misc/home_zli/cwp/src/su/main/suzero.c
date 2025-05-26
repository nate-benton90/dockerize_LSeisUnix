/* SUZERO: $Revision: 1.4 $ ; $Date: 91/03/22 16:05:28 $	*/

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
" SUZERO -- zero-out data within a time window	 		\n"
" 								\n"
" suzero itmax= < indata > outdata				\n"
" 								\n"
" Required parameters						\n"
" 	itmax=		last time sample to zero out		\n"
" 								\n"
" Optional parameters						\n"
" 	itmin=0		first time sample to zero out		\n"
" 								\n"
" See also: sukill, sumute					\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Chris
 *
 */


segy tr;

main(int argc, char **argv)
{
	int itmin;		/* first sample to zero out		*/
	int itmax;		/* last sample to zero out	 	*/
	int nt;			/* time samples per trace in input data	*/

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	/* Get information from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;

	/* Get params from user */
	MUSTGETPARINT("itmax", &itmax);
	if (!getparint("itmin", &itmin))	itmin = 0;

	/* Error checking */
	if (itmax > nt)    err("itmax = %d, must be < nt", itmax);
	if (itmin < 0)     err("itmin = %d, must not be negative", itmin);
	if (itmax < itmin) err("itmax < itmin, not allowed");

	/* Main loop over traces */
	do { 
		register int i;
		for (i = itmin; i <= itmax; ++i)  tr.data[i] = 0.0;
		
		puttr(&tr);
	} while(gettr(&tr));


	return EXIT_SUCCESS;
}
