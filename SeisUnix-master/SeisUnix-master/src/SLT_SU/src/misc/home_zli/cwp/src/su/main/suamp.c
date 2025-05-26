/* SUAMP: $Revision: 1.5 $ ; $Date: 90/12/18 20:44:43 $		*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines,.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUAMP - output amp, phase, real or imag trace from		\n"
" 	(frequency, x) domain data				\n"
" 								\n"
" suamp <stdin >stdout mode=amp					\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameter:						\n"
" 	mode=amp	output flag		 		\n"
" 	       		amp   = output amplitude traces		\n"
" 	       		phase = output phase traces		\n"
" 	       		real  = output real parts		\n"
" 	       		imag  = output imag parts 		\n"
" 								\n"
" Note:								\n"
" 	The trace returned is half length from 0 to Nyquist. 	\n"
" 								\n"
" Example:							\n"
" 	sufft <data | suamp >amp_traces				\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Shuki, Jack
 *
 * Notes:
 *	If efficiency becomes important consider inverting main loop
 *      and repeating extraction code within the branches of the switch.
 *
 */


#define	REAL	1
#define	IMAG	2
#define	AMP	3
#define	ARG	4

segy tr;

main(int argc, char **argv)
{
	string mode;		/* display: real, imag, amp, arg	*/
	int imode;		/* integer abbrev. for mode in switch	*/
	int nf;			/* number of samples on input trace	*/
	int nfby2;		/* nf/2					*/
	register float *xr;	/* real part of trace			*/
	register float *xi;	/* imaginary part of trace		*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get information from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	if (tr.trid != FUNPACKNYQ)
		err("input not complex freq data, trid=%d", tr.trid);

	nf = tr.ns; /* always even--includes 1 or 2 "wasted" slots */
	nfby2 = nf/2;


	/* Get mode */
	if (!getparstring("mode", &mode))	mode = "amp";

	if (STREQ(mode, "amp"))        imode = AMP;
	else if (STREQ(mode, "phase")) imode = ARG;
	else if (STREQ(mode, "real"))  imode = REAL;
	else if (STREQ(mode, "imag"))  imode = IMAG;
	else     err("unknown mode=\"%s\", see self-doc", mode);
	

	/* Allocate arrays for real and imaginary parts */
	xr = ealloc1float(nfby2);
	xi = ealloc1float(nfby2);

	/* Main loop over traces */
	do {
		register int i;

		/* Separate complex trace into real and imag parts */
		for (i = 0; i < nfby2; ++i) {
			xr[i] = tr.data[2*i];
			xi[i] = tr.data[2*i+1];
		}

		/* Compute the desired half-length trace */
		switch(imode) {
		case REAL:
			for (i = 0; i < nfby2; ++i) {
				tr.data[i] = xr[i];
			}
			tr.trid = REALPART;
		break;
		case IMAG:
			for (i = 0; i < nfby2; ++i) {
				tr.data[i] = xi[i];
			}
			tr.trid = IMAGPART;
		break;
		case AMP:
			for (i = 0; i < nfby2; ++i) {
				float re = xr[i];
				float im = xi[i];
				tr.data[i] = sqrt(re*re+im*im);
			}
			tr.trid = AMPLITUDE;
		break;
		case ARG:
			for (i = 0; i < nfby2; ++i) {
				float re = xr[i];
				float im = xi[i];
				if (re*re+im*im)  tr.data[i] = atan2(im, re);
				else              tr.data[i] = 0.0;
			}
			tr.trid = PHASE;
		break;
		default:
			err("mysterious mode=\"%s\"", mode);
		}

		/* Output the half-length trace */
		tr.ns = nfby2;
		puttr(&tr);

	} while (gettr(&tr));


	return EXIT_SUCCESS;
}
