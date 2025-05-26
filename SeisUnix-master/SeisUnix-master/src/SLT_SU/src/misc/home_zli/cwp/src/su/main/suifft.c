/* SUIFFT: $Revision: 1.6 $ ; $Date: 90/12/03 11:00:49 $	*/

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
" SUIFFT - fft complex frequency traces to real time traces	\n"
" 								\n"
" suiftt <stdin >sdout sign=-1					\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameter:						\n"
" 	sign=-1		sign in exponent of inverse fft		\n"
" 								\n"
" Output traces are normalized by 1/N where N is the fft size.	\n"
" 								\n"
" Note: sufft | suifft is not quite a no-op since the trace	\n"
" 	length will usually be longer due to fft padding.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Shuki, Chris, Jack
 *
 */


#define PFA_MAX	720720		/* Largest allowed fft	*/

segy tr;

main(int argc, char **argv)
{
	register complex *ct;	/* complex input trace			*/
	register float *rt;	/* real output trace			*/
	int nfft;		/* fft size 				*/
	int nf;			/* number of frequencies		*/
	int sign;		/* sign in exponent of transform	*/
	float onfft;		/* 1.0/nfft				*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&tr))  err("can't get first trace");
	if (tr.trid != FUNPACKNYQ)
		err("input not complex freq data, trid=%d", tr.trid);
	nfft = tr.ns - 2; /* see sufft */
	nf = nfft/2 + 1;
	onfft = 1.0/nfft;


	/* Set sign in exponent of transform */
	if (!getparint   ("sign", &sign)) sign = -1;
	if (sign != 1 && sign != -1)  err("sign = %d must be 1 or -1", sign);


	/* Allocate fft arrays */
	ct   = ealloc1complex(nf);
	rt   = ealloc1float(nfft);


	/* Main loop over traces */
	do {
		register int i;

		/* Load traces into ct (pfa fills in negative freqs) */
		for (i = 0; i < nf; ++i) {
			ct[i].r = tr.data[2*i];
			ct[i].i = tr.data[2*i+1];
		}


		/* Inverse FFT */
		pfacr(sign, nfft, ct, rt);


		/* Load back and scale for inverse fft */
		for (i = 0; i < nfft; i++)  tr.data[i] = rt[i] * onfft;

		tr.trid = TREAL;
		tr.ns = nfft;

		puttr(&tr);

	} while (gettr(&tr));


	return EXIT_SUCCESS;
}
