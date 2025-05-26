/* SUSPEC1: $Revision: 1.3 $ ; $Date: 91/02/14 10:22:46 $		*/

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
" SUSPEC1 - Fourier SPECtrum (t -> w) of traces 		\n"
" 								\n"
" suspec1 <infile >outfile 					\n"
" 								\n"
" Note: To facilitate further processing, the sampling interval	\n"
"       in frequency and first frequency (0) are set in the	\n"
"	output header.						\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Dave (algorithm), Jack (reformatting for SU)
 */


#define LOOKFAC	2	/* Look ahead factor for npfaro	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */

segy tr;

main(int argc, char **argv)
{
	register float *rt;	/* real trace				*/
	register complex *ct;	/* complex transformed trace		*/
	int nt;			/* number of points on input trace	*/
	int nfft;		/* number of points on output trace	*/
	int nfby2p1;		/* nfft/2 + 1				*/
	float dt;		/* sample interval in secs		*/
	float d1;		/* output sample interval in Hz		*/
	int ntr=0;		/* number of traces			*/
	register int i;		/* counter				*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&tr))  err("can't get first trace");
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
	nfby2p1 = nfft/2 + 1;
	d1 = 1.0/(nfft*dt);

	rt = ealloc1float(nfft);
	ct = ealloc1complex(nfby2p1);


	/* Main loop over traces */
	do {
		++ntr;

		/* Load trace into rt (zero-padded) */
		memcpy((char*)rt, (char*)tr.data, nt*FSIZE);
		bzero(rt + nt, (nfft - nt)*FSIZE);

		/* FFT */
		pfarc(1, nfft, rt, ct);

		/* Compute amplitude spectrum */
		for (i = 0; i < nfby2p1; ++i)  tr.data[i] = fcabs(ct[i]);

		/* Set header values */
		tr.ns = nfby2p1;
		tr.dt = 0;	  /* d1=df is now the relevant step size */
		tr.trid = AMPLITUDE;
		tr.d1 = d1;
		tr.f1 = 0.0;

		puttr(&tr);

	} while (gettr(&tr));


	return EXIT_SUCCESS;
}
