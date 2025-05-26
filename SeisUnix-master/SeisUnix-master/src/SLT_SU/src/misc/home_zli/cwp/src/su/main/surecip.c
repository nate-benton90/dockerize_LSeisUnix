/* SURECIP: $Revision: 1.7 $ ; $Date: 92/10/26 11:15:09 $	*/

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

/*********************** self documentation **********************/
char *sdoc[] = {
"                                				",
" SURECIP - sum opposing offsets in prepared data (see below)	",
"                                				",
" surecip <stdin >stdout	 		               	",
"                                				",
" Sum traces with equal positive and negative offsets (i.e. assume",
" reciprocity holds). 						",
"                                				",
" Usage:							",
"	suabshw <data >absdata					",
"	susort cdp offset <absdata | surecip >sumdata		",
"                                				",
" Note that this processing stream can be simply evoked by:	",
"                                				",
"	recip data sumdata					",
"                                				",
NULL};
/**************** end self doc ***********************************/

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
 */


segy intrace, outtrace;

main(int argc, char **argv)
{
	int cdpindex;	/* index of cdp header word		*/
	int offindex;	/* index of offset header word		*/
	int nt;		/* number of data points on trace	*/
	int nsegy;	/* number of bytes in the segy		*/
	Value cdpval;	/* value of cdp in current gather	*/
	Value offval;	/*  ... same for offset			*/
	Value cdpvalnew;/* value of cdp in trace being treated	*/
	Value offvalnew;/* ... same for offset			*/
	int newtracl;	/* tracl for stacked traces		*/
	int fold;	/* number of traces with same offset 	*/
	float ffold;	/* ... cast to float			*/
	int norm;	/* norm=1 => divide by fold		*/
	int itmp;	/* temporary for swap of sx, gx keys	*/


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Set parameter */
	if (!getparint("norm", &norm))	norm = 1;

	/* Get indices */
	cdpindex = getindex("cdp");
	offindex = getindex("offset");

	/* Set up for first trace */
	nsegy = gettr(&intrace);
	nt = intrace.ns;
	gethval(&intrace, cdpindex, &cdpval);
	gethval(&intrace, offindex, &offval);
	memcpy((char*)&outtrace, (char*)&intrace, nsegy);

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
				    register int i;
				    for (i = 0; i < nt; ++i)
					outtrace.data[i] /= ffold;
				}
			}
			puttr(&outtrace);
	
			/* Set up for next gather */
			memcpy((char*)&outtrace, (char*)&intrace, nsegy);
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
					    register int i;
					    for (i = 0; i < nt; ++i)
						outtrace.data[i] /= ffold;
					}
				}
				puttr(&outtrace);

				/* Set up for next offset */
				memcpy((char*)&outtrace,(char*)&intrace,nsegy);
				fold = 1;
				offval = offvalnew;

			} else { /* same offset within this cdp */

				register int i;
				for (i = 0; i < nt; ++i)
					outtrace.data[i] += intrace.data[i];

				fold++;
			}
		}
	}


	return EXIT_SUCCESS;
}
