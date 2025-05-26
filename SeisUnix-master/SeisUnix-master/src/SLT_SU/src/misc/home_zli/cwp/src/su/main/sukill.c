/* SUKILL: $Revision: 1.4 $ ; $Date: 91/03/22 16:05:37 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUKILL - zero out traces					\n"
" 								\n"
" sukill <stdin >stdout min= count=1				\n"
" 								\n"
" Required parameters						\n"
" 	min= 		first trace to kill (one-based)		\n"
" 								\n"
" Optional parameters						\n"
" 	count= 1	number of traces to kill 		\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Chris, Jack
 *
 */


segy tr;

main(int argc, char **argv)
{
	register int i;		/* time counter			*/
	register int itr;	/* trace counter		*/
	int min;		/* first trace to zero out	*/
	int count;		/* number of traces to zero out	*/
	int nt;			/* number of time samples	*/

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get parameters */
	MUSTGETPARINT   ("min", &min);
	if (!getparint   ("count", &count))	count = 1;


	/* Echo traces till min if they are there */
	for (itr = 1; itr < min && gettr(&tr); ++itr) {
		puttr(&tr);
	}

	/* Kill "count" traces if they are there
	 * Do first outside loop to get nt    */
	if (gettr(&tr)) nt = tr.ns;

	for (i = 0; i < nt; ++i)
		tr.data[i] = 0.0;
	puttr(&tr);
	++itr;

	for ( ; itr < min + count && gettr(&tr); ++itr) {
		for (i = 0; i < nt; ++i)
			tr.data[i] = 0.0;
		puttr(&tr);
	}


	/* Echo the trailing traces if any */
	while (gettr(&tr)) {
		puttr(&tr);
	}

	return EXIT_SUCCESS;
}
