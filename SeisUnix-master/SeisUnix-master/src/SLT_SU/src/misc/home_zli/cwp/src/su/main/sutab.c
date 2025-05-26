/* SUTAB: $Revision: 2.5 $ ; $Date: 89/05/25 16:54:13 $			*/

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

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUTAB - print non zero header values and data for non-graphic terminals	\n\
									\n\
sutab <stdin itmin=0 itmax=last_sample count=all			\n\
									\n\
Required parameters:							\n\
	none								\n\
							        	\n\
Optional parameters: 							\n\
	itmin=0			first time sample (zero-based) to plot	\n\
	itmax= (last sample)	last time sample (zero-based) to plot	\n\
	count= (all traces)	number of traces to plot		\n\
							        	\n\
Example:								\n\
	sutab <DATA itmin=32 itmax=63 count=10				\n\
Requests tab plot of samples 32 to 63 on the first 10 traces of DATA.	\n\
							        	\n\
";
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Shuki, Jack
 *
 */


segy tr;

main(int argc, char **argv)
{
	int itmin;		/* smallest sample (zero-based)	to plot	*/
	int itmax;		/* largest sample (zero-based) to plot	*/
	int nt;			/* number of samples			*/
	int count;		/* number of traces to plot		*/
	register int itr;	/* trace counter			*/
	bool plotall;		/* plot all the traces			*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Set number of traces to plot */
	plotall = false;
	if (!igetpar("count", &count)) plotall = true;


	/* Loop over traces */
	for (itr = 0; (plotall || itr < count) && gettr(&tr); itr++) {

		nt = (int) tr.ns;	/* Cast from unsigned */
		if (itr == 0) {	/* Awkward to do a gettr outside loop */
			if (!igetpar("itmin", &itmin))	itmin = 0;
			if (!igetpar("itmax", &itmax))	itmax = nt - 1;
			if (itmin >= nt - 1 || itmin < 0) {
				err("itmin=%d, require 0 < itmin < %d",
							itmin, nt - 1);
			}
			if (itmax >= nt) {
				itmax = nt - 1;
			}
			if (itmax < 0) {
				err("itmax=%d, require itmax > 0", itmax);
			}
			if (itmin > itmax) {
				itmin = itmax;
			}
		}

		printheader(&tr);

		tabplot(&tr, itmin, itmax);

	}


	return EXIT_SUCCESS;
}
