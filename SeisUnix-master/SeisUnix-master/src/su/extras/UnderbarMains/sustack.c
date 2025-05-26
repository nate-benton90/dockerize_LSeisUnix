/* SUSTACK: $Revision: 2.6 $ ; $Date: 89/05/25 16:53:52 $	*/

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
SUSTACK - stack adjacent traces having the same key header word	\n\
								\n\
sustack <input >output key=cdp norm=1 verbose=0			\n\
							        \n\
Required parameters:						\n\
	none							\n\
							        \n\
Optional parameters: 						\n\
	key = cdp	header key word to stack on		\n\
	norm = 1	the default is to divide by the number	\n\
			of traces stacked; norm = 0 selects no	\n\
			division				\n\
	verbose = 0	verbose = 1 echos information		\n\
							        \n\
Note: 	The offset field is set to zero on the output traces.	\n\
	Sushw can be used afterwards if this is not acceptable.	\n\
								\n\
";
/*****************************************************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack
 *
 * Caution: fold does not take into account possible muting of wide offset
 *          shallow traces during or after the nmo processing.
 *
 * Note:
 *	The offset field is set to zero on the output traces.  Sushw can
 *	be used afterwards if this is not acceptable.
 *
 * Note:
 *	The "valxxx" subroutines are in su/lib/valpkge.c.  In particular,
 *      "valcmp" shares the annoying attribute of "strcmp" that
 *		if (valcmp(type, val, valnew) {
 *			...
 *		}
 *	will be performed when val and valnew are different.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sustack.c,v $";
static string revid =
	"   $Revision: 2.6 $ ; $Date: 89/05/25 16:53:52 $";



segy intrace, outtrace;

main(argc, argv)
int argc; char **argv;
{
	string key;	/* header key word from segy.h		*/
	string type;	/* ... its type				*/
	int index;	/* ... its index			*/
	int nt;		/* number of data points on trace	*/
	int nsegy;	/* number of bytes in the segy		*/
	value val;	/* value of key in current gather	*/
	value valnew;	/* value of key in trace being treated	*/
	int fold;	/* number of traces with same key value	*/
	float ffold;	/* ... cast to float			*/
	int norm;	/* norm=1 => divide by fold		*/
	int newtracl;	/* tracl for stacked traces		*/
	int verbose;	/* verbose flag				*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Set parameters */
	if (!igetpar("verbose", &verbose))	 verbose = 0;
	if (!igetpar("norm", &norm))		 norm = 1;
	if (!sgetpar("key", &key))		 key = "cdp";

	type = hdtype(key);
	index = getindex(key);

	/* Set up for first trace (must compare new key field each time) */
	nsegy = gettr(&intrace);
	nt = intrace.ns;
	bcopy(&intrace, &outtrace, nsegy);

	newtracl = 1;		/* Global initialization */
	fold = 1;		/* Will be re-initialized for each gather */

	/* Loop over traces */
	gethval(&intrace, index, &val);
	while (nsegy) {		     /* While previous trace non-empty */
		nsegy = gettr(&intrace);
		gethval(&intrace, index, &valnew);
		if (valcmp(type, val, valnew) || !nsegy) {	
			/* Either val and valnew differ, indicating a  */
			/* new gather or nsegy is zero, indicating the */
		        /* end of the traces.                          */
			if (verbose) {
				fprintf(stderr, "val=");
				fprintfval(stderr, type, val);
				fprintf(stderr, "\tfold=%d\n", fold);
			}

			/* Add header info and output stack */
			outtrace.nhs = fold;
			outtrace.tracl = newtracl++;
			outtrace.offset = 0;
			if (norm) {
				ffold = (float) fold;
				if (fold != 1) {
					vsdiv_(outtrace.data, ONE, &ffold,
					       outtrace.data, ONE, &nt);
				}
			}
			puttr(&outtrace);

			/* Set up for next gather */
			bcopy(&intrace, &outtrace, nsegy);
			fold = 1;
			val = valnew;

		} else {	/* still in same gather */
			vadd_(intrace.data, ONE, outtrace.data, ONE,
						 outtrace.data, ONE, &nt);
			fold++;
		}
	}


	return SUCCEED;
}
