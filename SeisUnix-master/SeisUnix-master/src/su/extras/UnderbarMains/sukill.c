/* SUKILL: $Revision: 2.9 $ ; $Date: 89/05/25 16:52:09 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (isis!csm9a!jkcohen)
 *----------------------------------------------------------------------
 */

#include "cwp.h"
#include "segy.h"
#include "fconst.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUKILL - zero out traces					\n\
								\n\
sukill <stdin >stdout min= count=1				\n\
								\n\
Required parameters						\n\
	min = 		first trace to kill (one-based)		\n\
								\n\
Optional parameters						\n\
	count = 1	number of traces to kill 		\n\
								\n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Chris, Jack
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sukill.c,v $";
static string revid =
	"   $Revision: 2.9 $ ; $Date: 89/05/25 16:52:09 $";



segy tr;

main(argc, argv)
int argc; char **argv;
{
	register int itr;	/* trace counter		*/
	int min;		/* first trace to zero out	*/
	int count;		/* number of traces to zero out	*/
	int nt;			/* number of time samples	*/

	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID)) ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Get parameters */
	MUSTIGETPAR("min", &min);
	if (!igetpar("count", &count))	count = 1;


	/* Echo traces till min if they are there */
	for (itr = 1; itr < min && gettr(&tr); ++itr) {
		puttr(&tr);
	}

	/* Kill "count" traces if they are there
	 * Do first outside loop to get nt    */
	if (gettr(&tr)) nt = tr.ns;
	vfill_(FZERO, tr.data, ONE, &nt); /* tr.data[] = 0.0; */
	puttr(&tr);
	++itr;

	for ( ; itr < min + count && gettr(&tr); ++itr) {
		vfill_(FZERO, tr.data, ONE, &nt); 
		puttr(&tr);
	}


	/* Echo the trailing traces if any */
	while (gettr(&tr)) {
		puttr(&tr);
	}


	return SUCCEED;
}
