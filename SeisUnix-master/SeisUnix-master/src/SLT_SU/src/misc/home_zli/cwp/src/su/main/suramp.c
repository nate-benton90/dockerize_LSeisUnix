/* SURAMP: $Revision: 1.1 $ ; $Date: 91/02/28 16:21:18 $	*/

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
" SURAMP - Linearly taper the start and/or end of traces to zero.\n"
" 								\n"
" suramp <stdin >stdout [optional parameters]			\n"
" 								\n"
" Required parameters:						\n"
" 	if dt is not set in header, then dt is mandatory	\n"
" 							        \n"
" Optional parameters						\n"
"	tmin=tr.delrt/1000	end of starting ramp (sec)	\n"
"	tmax=(nt-1)*dt		beginning of ending ramp (sec)	\n"
" 	dt = (from header)	sampling interval (sec)		\n"
" 								\n"
" The taper is a linear ramp from 0 to tmin and/or tmax to the	\n"
" end of the trace.  Default is a no-op!			\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Jack, Ken
 */


segy tr;

main(int argc, char **argv)
{
	int nt;		/* number of sample points on traces	*/
	float dt;	/* time sampling interval		*/
	float *taper1;	/* vector of taper weights (up ramp)	*/
	float *taper2;	/* vector of taper weights (down ramp)	*/
	int ntaper1;	/* number of taper weights (up ramp)	*/
	int ntaper2;	/* number of taper weights (down ramp)	*/
	float tmin;	/* end of up ramp			*/
	float tmax;	/* start of down ramp			*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */
	if (!gettr(&tr))  err("can't get first trace");
	nt = (int) tr.ns;
	if (!getparfloat("dt", &dt))	dt = (float)tr.dt/1000000.0;
	if (!dt) err("dt field is zero and not getparred");


	/* Get parameters */
	if (!getparfloat("tmin", &tmin))	tmin = tr.delrt/1000.0;
	if (!getparfloat("tmax", &tmax))	tmax = (nt - 1)*dt;
	ntaper1 = (tmin - tr.delrt/1000.0)/dt;
	ntaper2 = ((nt - 1)*dt - tmax)/dt;


	/* Set up taper weights */
	if (ntaper1) {
		register int i;
		taper1 = ealloc1float(ntaper1);
		for (i = 0; i < ntaper1; ++i)
			taper1[i] = (float) (i+1)/ntaper1;
	}
	if (ntaper2) {
		register int i;
		taper2 = ealloc1float(ntaper2);
		for (i = 0; i < ntaper2; ++i)
			taper2[i] = (float) (ntaper2 - i)/ntaper2;
	}
						
	

	/* Main loop over traces */
	do {
		register int i;
		if (ntaper1) {
			for (i = 0; i < ntaper1; ++i)
				tr.data[i] *= taper1[i];
		}

		if (ntaper2) {
			for (i = 0; i < ntaper2; ++i)
				tr.data[nt - ntaper2 + i] *= taper2[i];
		}

		puttr(&tr);
	} while (gettr(&tr));

	return EXIT_SUCCESS;
}
