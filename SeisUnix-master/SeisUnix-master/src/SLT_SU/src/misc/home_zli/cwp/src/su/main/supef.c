/* SUPEF: $Revision: 1.23 $ ; $Date: 91/03/14 22:29:33 $		*/

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
string sdoc =
" 									\n"
" SUPEF - Wiener predictive error filtering				\n"
" 									\n"
" supef <stdin >stdout  [optional parameters]				\n"
" 									\n"
" Required parameters:							\n"
" 	dt is a mandatory getpar if not set in header	 		\n"
" 									\n"
" Optional parameters:							\n"
" 									\n"
"      minlag=dt	first lag of prediction filter (sec)		\n"
" 									\n"
"      maxlag=trace/20	last lag of prediction filter (sec) [note below]\n"
" 									\n"
"      pnoise=0.001	relative additive noise level			\n"
" 									\n"
"      mincorr=tmin	start of autocorrelation window (sec)		\n"
" 									\n"
"      maxcorr=tmax	end of autocorrelation window (sec)		\n"
" 									\n"
"      showwiener=0	=1 to show Wiener filter on each trace		\n"
" 									\n"
"      showspiker=0	=1 to show spike decon filter on each trace	\n"
" 									\n"
" Trace header fields accessed: ns, dt					\n"
" Trace header fields modified: none					\n"
" 									\n"
" Note: The precise maxlag formula is (tmax-tmin)/20.			\n"
" Caveat: Times are measured relative to the first trace sample.	\n"
" 									\n"
" 	To get the Wiener filters into an ascii file:			\n"
" 	... | supef ... showwiener=1 2>file | ...   (sh or ksh)		\n"
" 	(... | supef ... showwiener=1 | ...) >&file  (csh)		\n"
" 									\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Shuki, Jack, Ken
 *
 *	A. Ziolkowski, "Deconvolution", for value of maxlag default:
 *		page 91: imaxlag < nt/10.  I took nt/20.
 *
 * Notes:
 *	The prediction error filter is 1,0,0...,0,-wiener[0], ...,
 *	so no point in explicitly forming it.
 *
 *	If imaxlag < 2*iminlag - 1, then we don't need to compute the
 *	autocorrelation for lags:
 *		imaxlag-iminlag+1, ..., iminlag-1
 *	It doesn't seem worth the duplicated code to implement this.
 */


#define PNOISE	0.001


segy intrace, outtrace;

main(int argc, char **argv)
{
	int nt;			/* number of points on trace		*/
	float dt;		/* time sample interval (sec)		*/
	float *wiener;		/* Wiener error filter coefficients	*/
	float pnoise;		/* pef additive noise level		*/
	float minlag;		/* start of error filter (sec)		*/
	int iminlag;		/* ... in samples			*/
	float maxlag;		/* end of error filter (sec)		*/
	int imaxlag;		/* ... in samples			*/
	int nlag;		/* length of error filter in samples	*/
	int ncorr;		/* length of corr window in samples	*/
	float *crosscorr;	/* right hand side of Wiener eqs	*/
	float *autocorr;	/* vector of autocorrelations		*/
	float *spiker;		/* spiking decon filter			*/
	float mincorr;		/* start time of correlation window	*/
	int imincorr;		/* .. in samples			*/
	float maxcorr;		/* end time of correlation window	*/
	int imaxcorr;		/* .. in samples			*/
	int showspiker;		/* flag to display spiking filter	*/
	int showwiener;		/* flag to display pred. error filter	*/



	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&intrace)) err("can't get first trace");
	nt = intrace.ns;
	dt = (float)intrace.dt/1000000.0; if (!dt) MUSTGETPARFLOAT ("dt", &dt);


	/* Get parameters */
	if (!getparint("showwiener",  &showwiener))	showwiener = 0;
	if (!getparint("showspiker",  &showspiker))	showspiker = 0;

	if (!getparfloat("pnoise",  &pnoise))	pnoise = PNOISE;

	if (getparfloat("minlag", &minlag))	iminlag = NINT(minlag/dt);
	else					iminlag = 1;
	if (iminlag < 1) err("minlag=%g too small", minlag);

	if (getparfloat("maxlag", &maxlag))	imaxlag = NINT(maxlag/dt);
	else					imaxlag = NINT(0.05 * nt);
	if (imaxlag >= nt) err("maxlag=%g too large", maxlag);
	
	if (iminlag >= imaxlag)
		err("minlag=%g, maxlag=%g", minlag, maxlag);
	
	if (getparfloat("mincorr", &mincorr))	imincorr = NINT(mincorr/dt);
	else					imincorr = 0;
	if (imincorr < 0) err("mincorr=%g too small", mincorr);
	
	if (getparfloat("maxcorr", &maxcorr))	imaxcorr = NINT(maxcorr/dt);
	else					imaxcorr = nt-1;
	if (imaxcorr >= nt) err("maxcorr=%g too large", maxcorr);

	if (imincorr >= imaxcorr)
		err("mincorr=%g, maxcorr=%g", mincorr, maxcorr);
	
	nlag  = imaxlag - iminlag + 1;
	ncorr = imaxcorr - imincorr + 1;


	/* Allocate memory */
	wiener	 = ealloc1float(nlag);
	spiker	 = ealloc1float(nlag);
	autocorr = ealloc1float(imaxlag);


	/* Set pointer to "cross" correlation */
	crosscorr = autocorr + iminlag;



	/* Main loop over traces */
	do {
		static int itr = 0;
		++itr;

		/* Form autocorrelation vector */
		xcor(ncorr, imincorr, intrace.data,
		     ncorr, imincorr, intrace.data,
		     imaxlag, 0, autocorr);


		/* Leave trace alone if autocorr[0] vanishes */
		if (autocorr[0] == 0.0) {
			puttr(&intrace);
			if (showwiener)
				warn("NO Wiener filter, trace: %d", itr);
			if (showspiker)
				warn("NO spiking decon filter, trace: %d", itr);

			continue;
		}


		/* Whiten */
		autocorr[0] *= 1.0 + pnoise;


		/* Get inverse filter by Wiener-Levinson */
		stoepf(nlag, autocorr, crosscorr, wiener, spiker);
		

		/* Convolve pefilter with trace - don't do zero multiplies */
		{ register int i;
		  for (i = 0; i < nt; ++i) {
			register int j;
			register int n = MIN(i, imaxlag); 
			register float sum = intrace.data[i];

			for (j = iminlag; j <= n; ++j)
				sum -= wiener[j-iminlag] * intrace.data[i-j];

			outtrace.data[i] = sum;
		  }
		}


		/* Output filtered trace */
		memcpy((char*)&outtrace, (char*)&intrace, HDRBYTES);
		puttr(&outtrace);


		/* Show pefilter and/or spiker on request */
		if (showwiener) {
			register int i;
			warn("Wiener filter, trace: %d", itr);
			for (i = 0; i < imaxlag; ++i)
				fprintf(stderr, "%10g%c", wiener[i],
					(i%6==5 || i==nlag-1) ? '\n' : ' ');
		}
		
		if (showspiker) {
			register int i;
			warn("spiking decon filter, trace: %d", itr);
			for (i = 0; i < nlag; ++i)
				fprintf(stderr, "%10g%c", spiker[i],
					(i%6==5 || i==nlag-1) ? '\n' : ' ');
		}

	} while (gettr(&intrace));


	return EXIT_SUCCESS;
}
