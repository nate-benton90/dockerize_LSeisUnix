/* SUFFT: $Revision: 1.13 $ ; $Date: 91/02/14 10:32:34 $		*/

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
" SUFFT - fft real time traces to complex frequency traces	\n"
" 								\n"
" suftt <stdin >sdout sign=1 					\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameters:						\n"
" 	sign=1			sign in exponent of fft		\n"
" 	dt=from header		sampling interval		\n"
" 								\n"
" Trace header fields accessed: ns, dt, trid 			\n"
" Trace header fields modified: ns, dt, trid			\n"
" 								\n"
" Notes: To facilitate further processing, the sampling interval\n"
"       in frequency and first frequency (0) are set in the	\n"
"	output header.						\n"
" 								\n"
"	sufft | suifft is not quite a no-op since the trace	\n"
" 	length will usually be longer due to fft padding.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Shuki, Chris, Jack
 *
 * Note: leave dt set for later inversion
 *
 */


#define LOOKFAC	2	/* Look ahead factor for npfaro	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */

segy tr;

main(int argc, char **argv)
{
	register float *rt;	/* real trace				*/
	register complex *ct;	/* complex transformed trace		*/
	int nt;			/* number of points on input trace	*/
	int nfft;		/* transform length			*/
	int nf;			/* number of frequencies		*/
	int sign;		/* sign in exponent of transform	*/
	float dt;		/* sampling interval in secs		*/
	float d1;		/* output sample interval in Hz		*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&tr))  err("can't get first trace");
	if (tr.trid && tr.trid != TREAL) err("not time domain data");
	nt = tr.ns;


	/* dt is used only to set output header value d1 */
	if (!getparfloat("dt", &dt)) dt = (float) tr.dt/1000000.0;
	if (!dt) {
		dt = .004;
		warn("dt not set, assumed to be .004");
	}


	/* Set up pfa fft */
	nfft = npfaro(nt, LOOKFAC * nt);
	if (nfft >= MIN(SU_NFLTS, PFA_MAX))  err("Padded nt=%d--too big", nfft);
	nf = nfft/2 + 1;
	d1 = 1.0/(nfft*dt);

	if (!getparint("sign", &sign)) sign = 1;
	if (sign != 1 && sign != -1)   err("sign = %d must be 1 or -1", sign);

	rt = ealloc1float(nfft);
	ct = ealloc1complex(nf);


	/* Echo frequency step size (Hz) to par file */
	if (dt)  warn("d1=%f f1=0.0", 1.0/(nfft*dt));



	/* Main loop over traces */
	do {
		register int i;

		/* Load trace into rt (zero-padded) */
		memcpy((char*)rt, (char*)tr.data, nt*FSIZE);
		bzero(rt + nt, (nfft-nt)*FSIZE);

		/* FFT */
		pfarc(sign, nfft, rt, ct);

		/* Store values */
		for (i = 0; i < nf; ++i) {
			tr.data[2*i]   = ct[i].r;
			tr.data[2*i+1] = ct[i].i;
		}

		/* Set header values--npfaro makes nfft even */
		tr.ns = 2 * nf;
		tr.trid = FUNPACKNYQ;
		tr.d1 = d1;
		tr.f1 = 0.0;

		puttr(&tr);

	} while (gettr(&tr));


	return EXIT_SUCCESS;
}
