/* SUMUTE: $Revision: 1.5 $ ; $Date: 90/12/21 21:30:03 $	*/

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
" SUMUTE - mute above user-defined polygonal line		\n"
" 								\n"
" sumute <stdin >stdout xmute= tmute= [ntaper=0]		\n"
" 								\n"
" Mute above t values obtained by linear interpolation from	\n"
" the given (xmute, tmute) values.				\n"
" 								\n"
" Required parameters:						\n"
" 	xmute=		array of offset values 			\n"
" 	tmute=		array of corresponding time values (sec)\n"
" 								\n"
" Optional parameter:						\n"
"	ntaper=0	number of points to taper before hard	\n"
"			mute (sine squared taper)		\n"
" 								\n"
" Trace header fields accessed: dt, delrt, offset		\n"
" Trace header fields modified: none				\n"
" 								\n"
" Note: the tmute interpolant is extrapolated to the left by	\n"
"       the smallest time sample on the trace and to the right	\n"
"	by the last value given in the tmute array.		\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	SEP: Shuki
 *	CWP: Jack, Dave
 *
 */


segy tr;

main(int argc, char **argv)
{
	float *xmute, *tmute, *taper;
	int ntmute, nxmute, ntaper;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get parameters */
	if (!(nxmute = countparval("xmute"))) err("must give xmute= vector");
	if (!(ntmute = countparval("tmute"))) err("must give tmute= vector");
	if (nxmute != ntmute) err("lengths of xmute, tmute must be the same");
	xmute = ealloc1float(ntmute);	getparfloat("xmute", xmute);
	tmute = ealloc1float(ntmute);	getparfloat("tmute", tmute);
	if (!getparint("ntaper", &ntaper))	ntaper = 0;


	/* Set up taper weights if tapering requested */
	if (ntaper) {
		register int k;
		taper = ealloc1float(ntaper);
		for (k = 0; k < ntaper; ++k) {
			float s = sin((k+1)*PI/(2*ntaper));
			taper[k] = s*s;
		}
	}

						
	/* Get info from first trace */
	if (!gettr(&tr)) err("can't read first trace");
	if (!tr.dt) err("dt header field must be set");


	/* Loop over traces */
	do {
		float x    = (float) tr.offset;
		float tmin = (float) tr.delrt/1000.0;
		float dt   = (float) tr.dt/1000000.0;
		float t;
		int nmute;
		register int i;

		intlin(nxmute, xmute, tmute, tmin, tmute[nxmute-1], 1, &x, &t); 
		nmute = NINT((t - tmin)/dt);
		bzero(tr.data, nmute*FSIZE);
		for (i = 0; i < ntaper; ++i)  tr.data[i+nmute] *= taper[i];
		puttr(&tr);
	} while (gettr(&tr));

	
	return EXIT_SUCCESS;
}
