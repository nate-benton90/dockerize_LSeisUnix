/* SUUS: $Revision: 2.8 $ ; $Date: 89/11/07 16:27:02 $	*/

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
#include "header.h"
#include "fconst.h"

/*********************** self documentation ******/
string sdoc = "\
						\n\
SUUS - under sample traces			\n\
						\n\
suus <stdin >stdout j=2 s=0 strict=0	 	\n\
						\n\
Required parameters:				\n\
	none					\n\
					        \n\
Optional parameters: 				\n\
	j = 2	Select every jth time sample	\n\
	s = 0	First sample to select		\n\
	strict = 0  Subsample by averaging	\n\
		    = 1 just take every jth	\n\
					        \n\
";
/*************************************************/

/* Credits:
 *	SEP: Shuki
 *	CWP: Jack, Norm
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suus.c,v $";
static string revid =
	"   $Revision: 2.8 $ ; $Date: 89/11/07 16:27:02 $";



segy intrace, outtrace;

main(argc, argv)
int argc; char **argv;
{
	int s;		/* first time jt to accept		*/
	int j;		/* modulus				*/
	int strict;	/* flag for mode of subsampling		*/
	int nt;		/* number of samples on output trace	*/
	int dt;		/* sample rate on output trace		*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Get parameters */
	if (!igetpar("s", &s))			s = 0;
	if (!igetpar("j", &j))			j = 2;
	if (!igetpar("strict", &strict))	strict = 0;
	if (s < 0) err("s=%d, should be >= 0", s);
	if (j < 2) err("j=%d, should be >= 2", j);

	/* Get nt and dt off first trace to compute new nt, dt */
	if (!gettr(&intrace)) err("can't get first trace");
	if (s >= intrace.ns) err("s=%d, should be < %d", intrace.ns);
	nt = (intrace.ns - s) / j;
	dt = intrace.dt * j;

	do {
		bcopy(&intrace, &outtrace, HDRBYTES);
		outtrace.ns = nt;
		outtrace.dt = dt;
		if (strict) {
			vmov_(intrace.data + s, &j, outtrace.data, ONE, &nt);
		} else {
			/* local variable */
			register int it; /* counter on outtrace	*/

			for (it = 0; it < nt; ++it) {
				/* local variables */
				register int jt; /* counter on intrace	*/
				register int jj; /* counter up to j	*/
				register float sum = 0.0;

				/* compute first location on intrace to use */
				jt = s + j*it;

				/* get average of intrace values */
				for (jj = 0; jj < j; ++jj) {
					sum += intrace.data[jt + jj];
				}
				outtrace.data[it] = sum / (float) j;
			}
		}
		puttr(&outtrace);
	} while (gettr(&intrace));


	return SUCCEED;
}
