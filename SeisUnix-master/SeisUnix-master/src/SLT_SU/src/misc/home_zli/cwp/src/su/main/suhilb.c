/* SUHILB: $Revision: 1.8 $ ; $Date: 90/08/31 17:14:25 $	*/
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
string sdoc = "\
								\n\
SUHILB - Hilbert transform					\n\
								\n\
suhilb <stdin >sdout 						\n\
							        \n\
The filter is applied in frequency domain.			\n\
							        \n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Jack
 *
 * Algorithm:
 *	h(t) = Re[INVFFT{-i sgn(omega)FFT(f)}]
 *	     = Im[INVFFT{sgn(omega)FFT(f)}]
 *
 * This assumes that the forward fft uses the sign +1 in the
 * exponent--and we do this below!
 *
 */


#define LOOKFAC	2	/* Look ahead factor for npfao	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */


segy tr;


main(int argc, char **argv)
{
	register complex *ct;	/* complex trace			*/
	int nt;			/* number of points on input trace	*/
	int nfft;		/* number of points in fft trace	*/
	int nfby2;		/* nfft/2				*/
	int nfby2p1;		/* nfft/2 + 1				*/
	float onfft;		/* 1.0 / nfft				*/
	complex czero;		/* complex zero				*/
	register int i;		/* counter				*/

	
	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */
	if (!gettr(&tr)) err ("can't get first trace");
	nt = tr.ns;


	/* Check that data is correct format */
	if (tr.trid && tr.trid != TREAL) {
		err("input is not seismic data, trid=%d", tr.trid);
	}


	/* Set up for fft */
	nfft = npfao(nt, LOOKFAC * nt);
	if (nfft >= MIN(SU_NFLTS, PFA_MAX))
		err("Padded nt=%d -- too big", nfft);

	nfby2 = nfft/2;
	nfby2p1 = nfft/2 + 1;
	onfft = 1.0 / nfft;
	czero = cmplx(0.0, 0.0);


	/* Allocate fft array */
	ct = ealloc1complex(nfft);


	/* Loop over traces */
	do {
		/* Load traces into ct (zero-padded) */
		for (i = 0; i < nt; ++i)  ct[i] = cmplx(tr.data[i], 0.0);
		for (i = nt; i < nfft; ++i)  ct[i] = czero;

		/* Fft */
		pfacc(1, nfft, ct);

		/* Apply sgn */
		ct[0] = czero;				/* zero at dc     */
		if (!ISODD(nfft)) ct[nfby2] = czero;	/* and at Nyquist */
		for (i = nfby2p1; i < nfft; ++i)  ct[i] = crmul(ct[i], -1.0);


		/* Invert */
		pfacc(-1, nfft, ct);

		/* Take imaginary part, adjust for scale */
		for (i = 0; i < nt; ++i)  tr.data[i] = onfft * ct[i].i;

		puttr(&tr);

	} while (gettr(&tr));


	return EXIT_SUCCESS;
}
