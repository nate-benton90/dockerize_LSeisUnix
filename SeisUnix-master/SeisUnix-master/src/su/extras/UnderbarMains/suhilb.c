/* SUHILB: $Revision: 2.14 $ ; $Date: 89/09/20 19:36:11 $	*/
#include "cwp.h"
/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (isis!csm9a!jkcohen)
 *----------------------------------------------------------------------
 */

#include "segy.h"
#include "fconst.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUHILB - Hilbert transform					\n\
								\n\
suhilb <stdin >stdout						\n\
							        \n\
The filter is applied in frequency domain.			\n\
							        \n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Jack
 *
 * Algorithm:
 *	h(t) = Im[INVFFT{sgn(omega)FFT(f)}]
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suhilb.c,v $";
static string revid =
	"   $Revision: 2.14 $ ; $Date: 89/09/20 19:36:11 $";



#define	FACMAX		12		/* For FFTPACK	*/

segy tr;


main(argc, argv)
int argc; char **argv;
{
	float *hcos;		/* hold cosines for fft			*/
	float *hsin;		/* hold sines for fft			*/
	register float *xr;	/* real part of trace			*/
	register float *xi;	/* imaginary part of trace		*/
	float *wr;		/* work area for fft			*/
	float *wi;		/* work area for fft			*/
	int nt;			/* number of points on input trace	*/
	int ntsize;		/* nt in bytes				*/
	int nfft;		/* number of points in fft trace	*/
	uint nfftsize;		/* nfft in bytes			*/
	int nfby2;		/* nfft/2				*/
	int nfback;		/* nfft - nfby2				*/
	float monfft;		/* -1.0 / nfft				*/
	int nfac;		/* number of factors of nfft		*/
	int facs[FACMAX];	/* contains factors of nfft		*/
	int nzeros;		/* number of padded zeroes in bytes	*/
	register int i;		/* counter				*/

	
	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID)) ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	if (tr.trid && tr.trid != TREAL) {
		err("input is not seismic data, trid=%d", tr.trid);
	}

	/* Get info from first trace */
	if (!gettr(&tr)) err ("can't get first trace");
	nt = tr.ns;

	/* Set up for fft */
	fftfac_(&nt, &nfft, &nfac, facs);
	if (nfft > SU_NFLTS) {
		err("Padded nt=%d -- too big", nfft);
	}
	nfby2 = nfft / 2;
	nfback = nfft - nfby2;
	monfft = -1.0 / (float) nfft;
	ntsize = nt * FSIZE;
	nfftsize = nfft * FSIZE;
	nzeros = nfftsize - ntsize;

	/* Allocate fft arrays */
	hcos = vec(nfft);	hsin = vec(nfft);
	xr   = vec(nfft); 	xi   = vec(nfft);
	wr   = vec(nfft);	wi   = vec(nfft);


	/* Set up tables */
	ffttab_(&nfft, hcos, hsin);


	/* Loop over traces */
	do {
		/* Load traces into xr, xi (zero-padded) */
		bcopy(tr.data, xr, ntsize);
		bzero(xr + nt, nzeros);
		bzero(xi, (int) nfftsize);


		/* Fft */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);


		/* Apply sgn and take conjugate for inverse fft          */
		/* Apply inverse scale later, since have to *= -1 anyway */
		/* Positive frequencies -- just conjugate            */
		/* Negative frequencies -- sgn and conj cancel on xi */
		xi[0] *= -1.0;
		for (i = 0; i <= nfby2; ++i) {
			xi[i]        *= -1.0;
			xr[nfft - i] *= -1.0;
		}
		/* Note: for n even, we have turned the sign of xr[nfby2] */
		/*       around, but this term contributes 0 to the       */
		/*       imaginary part of the inverse transform anyway   */

		/* Invert */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);

		/* Take imaginary part, adjust for conjugate and scale */
		/* for (i = 0; i < nt; ++i) tr.data[i] = -xi[i]/nfft;  */
		vsmul_(xi, ONE, &monfft, tr.data, ONE, &nt);

		puttr(&tr);

	} while (gettr(&tr));


	return SUCCEED;
}
