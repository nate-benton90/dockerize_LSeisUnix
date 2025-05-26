/* SUABS: $Revision: 2.8 $ ; $Date: 89/05/25 16:49:16 $		*/

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
char *sdoc = "\
								\n\
SUABS - replace data by absolute values 			\n\
								\n\
suabs <stdin >stdout						\n\
								\n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Jack, Chris
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suabs.c,v $";
static string revid =
	"   $Revision: 2.8 $ ; $Date: 89/05/25 16:49:16 $";



segy tr;

main(argc, argv)
int argc; char **argv;
{
	int nt;		/* number of sample points on traces	*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Get nt from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = (int) tr.ns;	/* disaster to pass &ushort */


	/* Loop over the traces */
	do {

		vabs_(tr.data, ONE, tr.data, ONE, &nt);
		puttr(&tr);

	} while (gettr(&tr));


	return SUCCEED;
}
