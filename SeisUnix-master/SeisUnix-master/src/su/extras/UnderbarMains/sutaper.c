/* SUTAPER: $Revision: 1.10 $ ; $Date: 89/09/20 19:37:38 $	*/

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
SUTAPER - Taper the edge traces of a data panel to zero.	\n\
								\n\
sutaper <stdin >stdout [taper=wt1,wt2,...wtn]			\n\
								\n\
The taper parameter is an (usually) increasing sequence	of	\n\
weights; default is a five point sine-squared taper.		\n\
The taper is symmetrically applied at each end of the data set.	\n\
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
	"   $Source: /src/su/src/RCS/sutaper.c,v $";
static string revid =
	"   $Revision: 1.10 $ ; $Date: 89/09/20 19:37:38 $";



/* Default taper weights; Wk = sin^2(k/6 * pi/2)	*/
#define W1	0.06699
#define W2	0.25
#define W3	0.5
#define W4	0.75
#define W5	0.933
#define NTAPER	5

segy tr;

main(argc, argv)
int argc; char **argv;
{
	int nt;		/* number of sample points on traces	*/
	int itr, ntr;	/* trace counter and total 		*/
	float *taper;	/* vector of taper weights		*/
	int ntaper;	/* number of taper weights		*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Get info from first trace */
	if (!(ntr = gettra(STDIN, &tr, 0))) err("can't get first trace");
	nt = (int) tr.ns;	/* disaster to pass &ushort */


	/* Allocate room for taper vector */
	ntaper = MAX(NTAPER, maxgetpar());  /* upper bound refined later */
	taper = vec(ntaper);


	/* Get user's weights; if none install default taper */
	if (!(ntaper = fgetpar("taper", taper))) {
		ntaper = NTAPER;
		taper[0] = W1;
		taper[1] = W2;
		taper[2] = W3;
		taper[3] = W4;
		taper[4] = W5;
	}


	/* Exact ntaper value now known, check not too big */
	if (ntaper > ntr/2) {
		err("taper vector length, %d, exceeds ntr/2 = %d",
					ntaper,              ntr/2);
	}
						


	/* Loop over the traces, tapering those at the ends */

	/* Take care of the trace already read */
	vsmul_(tr.data, ONE, taper, tr.data, ONE, &nt);
	puttr(&tr);

	/* Taper at the left end of the data set */
	for (itr = 1; itr < ntaper; ++itr) {
		gettr(&tr);
		vsmul_(tr.data, ONE, taper + itr, tr.data, ONE, &nt);
		puttr(&tr);
	}

	/* Pass on traces in the middle of the data set */
	for ( ; itr < ntr - ntaper; ++itr) {
		gettr(&tr);
		puttr(&tr);
	}
	
	/* Taper at the right end of the data set */
	for ( ; itr < ntr; ++itr) {
		gettr(&tr);
		vsmul_(tr.data, ONE, taper + ntr - itr - 1, tr.data, ONE, &nt);
		puttr(&tr);
	}


	return SUCCEED;
}
