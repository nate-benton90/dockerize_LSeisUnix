/* SUXCOR: $Revision: 1.3 $ ; $Date: 92/10/26 14:15:06 $		*/

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
#include "header.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" SUXCOR - correlation with user-supplied filter			",
" 									",
" suxcor <stdin >stdout  filter= [optional parameters]			",
" 									",
" Required parameters: ONE of						",
"	sufile=         file containing SU trace to use as filter	",
"	filter= 	user-supplied correlation filter (ascii)	",
" 									",
" Optional parameters:							",
"	first=1		supplied trace is default first element of	",
" 			correlation.  =0 for it to be second.		",
" 									",
" Trace header fields accessed: ns					",
" Trace header fields modified: ns					",
" 									",
" Notes: It is quietly assumed that the time sampling interval on the	",
" single trace and the output traces is the same as that on the traces	",
" in the input file.  The sufile may actually have more than one trace,	",
" but only the first trace is used.					",
" 									",
" Examples:								",
"	suplane | suwind min=12 max=12 >TRACE				",
"	suxcor<DATA sufile=TRACE | ...					",
" Here, the su data file, \"DATA\", is correlated trace by trace with the",
" the single su trace, \"TRACE\".					",
" 									",
"	suxcor<DATA filter=1,2,1 | ...					",
" Here, the su data file, \"DATA\", is correlated trace by trace with the",
" the filter shown.							",
" 									",
NULL};
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Jack K. Cohen, Michel Dietrich
 *
 *  CAVEATS: no space-variable or time-variable capacity.
 *     The more than one trace allowed in sufile is the
 *     beginning of a hook to handle the spatially variant case.
 */


segy intrace, outtrace, sutrace;

main(int argc, char **argv)
{
	int nt;			/* number of points on input traces	*/
	int ntout;		/* number of points on output traces	*/
	float *filter;		/* filter coefficients			*/
	int nfilter;		/* length of input wavelet in samples	*/
	String sufile;		/* name of file containing one SU trace */
	FILE *fp;		/* ... its file pointer			*/
	int first;		/* correlation order flag		*/


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&intrace)) err("can't get first trace");
	nt = intrace.ns;


	/* Get parameters and set up filter array */
	if (!getparint("first", &first))  first = 1;
	if (!getparstring("sufile", &sufile)) {
		if (!(nfilter = countparval("filter")))
			err("must specify filter= desired filter");
		filter = ealloc1float(nfilter);	getparfloat("filter", filter);
	} else {
		fp = efopen(sufile, "r");
		fgettr(fp, &sutrace);
		nfilter = sutrace.ns;
		filter = ealloc1float(nfilter);
		memcpy((char*)filter, (char*)sutrace.data, nfilter*FSIZE);
	}
	
	
	/* Set output trace length */
	ntout = nt + nfilter - 1;


	/* Main loop over traces */
	if (first) {
		do {
			xcor(nfilter, 0, filter,
			     nt, 0, intrace.data, 
                     	     ntout, -nfilter + 1, outtrace.data);        


			memcpy((char*)&outtrace, (char*)&intrace, HDRBYTES);
			outtrace.ns = ntout; 
			puttr(&outtrace);

		} while (gettr(&intrace));
	} else {
		do {
			xcor(nt, 0, intrace.data, 
			     nfilter, 0, filter,
                     	     ntout, -nt + 1, outtrace.data);        

			memcpy((char*)&outtrace, (char*)&intrace, HDRBYTES);
			outtrace.ns = ntout; 
			puttr(&outtrace);

		} while (gettr(&intrace));
	}


	return EXIT_SUCCESS;
}
