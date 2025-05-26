/* SUSTACK: $Revision: 1.6 $ ; $Date: 90/11/28 11:19:50 $	*/

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
string sdoc =
" 								\n"
" SUSTACK - stack adjacent traces having the same key header word\n"
" 								\n"
" sustack <input >output key=cdp normpow=1.0 verbose=0		\n"
" 							        \n"
" Required parameters:						\n"
" 	none							\n"
" 							        \n"
" Optional parameters: 						\n"
" 	key=cdp		header key word to stack on		\n"
" 	normpow=1.0	each sample is divided by the		\n"
"			normpow'th number of non-zero values	\n"
"			stacked (normpow=0 selects no division)	\n"
" 	verbose=0	verbose = 1 echos information		\n"
" 							        \n"
" Note:	The offset field is set to zero on the output traces.	\n"
" 	Sushw can be used afterwards if this is not acceptable.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack, Dave
 *
 * Note:
 *	The "valxxx" subroutines are in su/lib/valpkge.c.  In particular,
 *      "valcmp" shares the annoying attribute of "strcmp" that
 *		if (valcmp(type, val, valnew) {
 *			...
 *		}
 *	will be performed when val and valnew are different.
 */


segy intrace, outtrace;

main(int argc, char **argv)
{
	String key;	/* header key word from segy.h		*/
	String type;	/* ... its type				*/
	int index;	/* ... its index			*/
	int nt;		/* number of data points on trace	*/
	int nsegy;	/* number of bytes in the segy		*/
	Value val;	/* value of key in current gather	*/
	Value valnew;	/* value of key in trace being treated	*/
	int fold;	/* number of traces stacked		*/
	int *nnz;	/* number of non-zero values stacked	*/
	float normpow;	/* divide by nnz[i]^normpow		*/
	int newtracl;	/* tracl for stacked traces		*/
	int verbose;	/* verbose flag				*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Set parameters */
	if (!getparint   ("verbose", &verbose))	 verbose = 0;
	if (!getparfloat ("normpow", &normpow))	 normpow = 1.0;
	if (!getparstring("key", &key))		 key = "cdp";

	type = hdtype(key);
	index = getindex(key);

	/* Set up for first trace (must compare new key field each time) */
	nsegy = gettr(&intrace);
	nt = intrace.ns;
	memcpy((char*)&outtrace, (char*)&intrace, nsegy);
	nnz = ealloc1int(nt);
	{ register int i;
	  for (i = 0; i < nt; ++i)  nnz[i] = 0;
	}
	fold = 1;


	/* Loop over traces */
	newtracl = 1;
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
			if (normpow && fold != 1) {
			        register int i;
				for (i = 0; i < nt; ++i) {
				    float nnzi = nnz[i];
				    if (nnzi)
					outtrace.data[i] /= pow(nnzi, normpow);
				}
			}
			puttr(&outtrace);

			/* Set up for next gather */
			memcpy((char*)&outtrace, (char*)&intrace, nsegy);
			{ register int i;
			  for (i = 0; i < nt; ++i)  nnz[i] = 0;
			}
			fold = 1;
			val = valnew;

		} else {	/* still in same gather */
			register int i;
			for (i = 0; i < nt; ++i) {
				float datum = intrace.data[i];
				if (!CLOSETO(datum, 0.0))  ++nnz[i];
				outtrace.data[i] += datum;
			}
			++fold;
		}
	}


	return EXIT_SUCCESS;
}
