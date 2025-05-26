/* SURECIP: $Revision: 2.6 $ ; $Date: 89/05/25 16:53:36 $	*/

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
SURECIP - sum opposing offsets in prepared data (see below)	\n\
                                				\n\
surecip <stdin >stdout	 		               		\n\
                                				\n\
Sum traces with equal positive and negative offsets (i.e. assume\n\
reciprocity holds). 						\n\
                                				\n\
Usage:								\n\
	suabshw <data >absdata					\n\
	susort cdp offset <absdata | surecip >sumdata		\n\
                                				\n\
Note that this processing stream can be simply evoked by:	\n\
                                				\n\
	recip data sumdata					\n\
                                				\n\
";
/*****************************************************************/

/* Credits:
 *	SEP: Shuki
 *	CWP: Jack
 *
 * Caveat:
 *	The assumption is that this operation is not a mainstay processing
 *	item.  Hence the recommended implemention via the 'recip' shell
 *	script.  If it becomes a mainstay, then a much faster code can
 *	quickly drummed up by incorporating portions of suabshw and
 *	susort.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
    "   $Source: /src/su/src/RCS/surecip.c,v $";
static string revid =
    "   $Revision: 2.6 $ ; $Date: 89/05/25 16:53:36 $";



segy intrace, outtrace;

main(argc, argv)
int argc; char **argv;
{
	int cdpindex;	/* index of cdp header word		*/
	int offindex;	/* index of offset header word		*/
	int nt;		/* number of data points on trace	*/
	int nsegy;	/* number of bytes in the segy		*/
	value cdpval;	/* value of cdp in current gather	*/
	value offval;	/*  ... same for offset			*/
	value cdpvalnew;/* value of cdp in trace being treated	*/
	value offvalnew;/* ... same for offset			*/
	int newtracl;	/* tracl for stacked traces		*/
	int fold;	/* number of traces with same offset 	*/
	float ffold;	/* ... cast to float			*/
	int norm;	/* norm=1 => divide by fold		*/
	int itmp;	/* temporary for swap of sx, gx keys	*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Set parameter */
	if (!igetpar("norm", &norm))	norm = 1;

	/* Get indices */
	cdpindex = getindex("cdp");
	offindex = getindex("offset");

	/* Set up for first trace */
	nsegy = gettr(&intrace);
	nt = intrace.ns;
	gethval(&intrace, cdpindex, &cdpval);
	gethval(&intrace, offindex, &offval);
	bcopy(&intrace, &outtrace, nsegy);

	newtracl = 1;		/* Global initialization */
	fold = 1;		/* Will be re-initialized for each gather */

	/* Loop over traces */
	while (nsegy) {		     /* While previous trace non-empty */
		nsegy = gettr(&intrace);
		gethval(&intrace, cdpindex, &cdpvalnew);
		gethval(&intrace, offindex, &offvalnew);
		if (valcmp("l", cdpval, cdpvalnew) || !nsegy) {	
			/* Either cdpval and cdpvalnew differ,       */
			/* indicating a new gather or nsegy is zero, */
			/* indicating the end of the traces.         */

			/* Add header info and output leftover stack */
			outtrace.nhs = fold;
			outtrace.tracl = newtracl++;
			if (outtrace.sx > outtrace.gx) {
				itmp = outtrace.sx;
				outtrace.sx = outtrace.gx;
				outtrace.gx = itmp;
			}
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
			cdpval = cdpvalnew;
			offval = offvalnew;

		} else {	/* still in same cdp gather */
			if (valcmp("l", offval, offvalnew)) {
				/* offval and offvalnew differ */

				/* Add header info and output stack */
				outtrace.nhs = fold;
				outtrace.tracl = newtracl++;
				if (outtrace.sx > outtrace.gx) {
					itmp = outtrace.sx;
					outtrace.sx = outtrace.gx;
					outtrace.gx = itmp;
				}
				if (norm) {
					ffold = (float) fold;
					if (fold != 1) {
					    vsdiv_(outtrace.data, ONE, &ffold,
						   outtrace.data, ONE, &nt);
					}
				}
				puttr(&outtrace);

				/* Set up for next offset */
				bcopy(&intrace, &outtrace, nsegy);
				fold = 1;
				offval = offvalnew;

			} else { /* same offset within this cdp */

				vadd_(intrace.data, ONE, outtrace.data, ONE,
				      outtrace.data, ONE, &nt);
				fold++;
			}
		}
	}


	return SUCCEED;
}
