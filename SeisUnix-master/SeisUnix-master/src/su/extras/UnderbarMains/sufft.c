/* SUFFT: $Revision: 2.8 $ ; $Date: 89/09/20 19:35:21 $		*/

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

#include "cwp.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUFFT - fft time to frequency					\n\
								\n\
suftt <stdin >sdout sign=1					\n\
								\n\
Required parameters:						\n\
	none							\n\
								\n\
Optional parameter:						\n\
	sign = 1	sign in exponent of fft			\n\
								\n\
Sufft performs an fft using an optimal or near-optimal trace	\n\
length, N, composed of a product of the factors 2, 3, 4, 5 and	\n\
6.  N is greater than or equal the input trace length, tr.ns by	\n\
at most 500 sample points.					\n\
								\n\
The output is `packed' frequency traces.			\n\
								\n\
Note:								\n\
   For even length N, a ``packed'' frequency trace is:		\n\
   xr[0],xr[N/2],xr[1],xi[1], ..., xr[N/2 -1],xi[N/2 -1]	\n\
   (the second entry is exceptional).				\n\
							        \n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Shuki, Chris, Jack, Frank
 *
 * Note:
 *	For even length N, a ``packed'' frequency trace is:
 *	xr[0],xr[N/2],xr[1],xi[1], ..., xr[N/2 -1],xi[N/2 -1]
 *	(the second entry is exceptional).
 *
 *	For odd length N, a ``packed'' frequency trace is:
 *	xr[0],xr[(N-1)/2],xr[1],xi[1], ..., [(N-1)/2 -1],
 *	xi[(N-1)/2 -1],xi[(N-1)/2] (the second and last
 *	entries are exceptional)
 *
 *	Because of the forward search for optimal length
 *	N, it is likely that the odd N case never occurs in
 *	current implementation.  However, the code
 *	supports odd N to admit the possibility of later
 *	supporting user specification or properties of N.
 *
 * Caveat:
 *	This routine uses a highly efficient complex fft.  To fully
 *	exploit it, we transform two real traces at a time.  It would
 *	be better to develop an analogous real fft and avoid the
 *	tricky coding involved.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sufft.c,v $";
static string revid =
	"   $Revision: 2.8 $ ; $Date: 89/09/20 19:35:21 $";



#define	FACMAX		12		/* For FFTPACK	*/

segy tr1;
segy tr2;

main(argc, argv)
int argc; char **argv;
{
	bool first=true;	/* flag for first trace			*/
	bool lastzero=false;	/* is this trace a phoney one?		*/
	float *hcos;		/* hold cosines for fft			*/
	float *hsin;		/* hold sines for fft			*/
	float *xr;		/* real part of trace			*/
	float *xi;		/* imaginary part of trace		*/
	float *wr;		/* work area for fft			*/
	float *wi;		/* work area for fft			*/
	int nt;			/* number of points on input trace	*/
	int ntsize;		/* nt*sizeof(float)			*/
	int nfft;		/* number of points on output trace	*/
	uint nfftsize;		/* nt*sizeof(float)			*/
	int nfm1;		/* nfft - 1 				*/
	int nfby2;		/* nfft / 2				*/
	int nfby2p1;		/* (nfft/2) + 1				*/
	int nfac;		/* number of factors of nfft		*/
	int facs[FACMAX];	/* contains factors of nfft		*/
	int nzeros;		/* number of padded zeroes*sizeof(float)*/
	int sign;		/* sign in exponent of transform	*/
	register int i;		/* counter				*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	while (gettr(&tr1)) {
		if (first) { /* first trace: initialize */
			first = false;
			if (tr1.trid && tr1.trid != TREAL) {
			    err("input is not time domain data, trid=%d",
								tr1.trid);
			}
			nt = tr1.ns;
			fftfac_(&nt, &nfft, &nfac, facs);
			if (nfft > SU_NFLTS) {
				err("Padded nt=%d -- too big", nfft);
			}
			nfm1 = nfft - 1;
			nfby2 = nfft / 2;
			nfby2p1 = nfby2 + 1;
			ntsize = nt * FSIZE;
			nfftsize = nfft * FSIZE;
			nzeros = nfftsize - ntsize;

			/* Set sign in exponent of transform */
			if (!igetpar("sign", &sign))	sign = 1;
			if (sign != 1 && sign != -1) {
			    err("sign = %d must be 1 or -1", sign);
			}

			/* Allocate fft arrays */
			hcos = vec(nfft);  hsin = vec(nfft);
			xr   = vec(nfft);  xi   = vec(nfft);
			wr   = vec(nfft);  wi   = vec(nfft);

			/* Set up tables */
			ffttab_(&nfft, hcos, hsin);

		} /* End of initialization for first trace */

		/* Load traces into xr, xi (zero-padded) */
		if (gettr(&tr2)) {
			bcopy(tr1.data, xr, ntsize);
			bzero(xr + nt, nzeros);
			bcopy(tr2.data, xi, ntsize);
			bzero(xi + nt, nzeros);
		} else { /* odd number of traces; make zero trace */
			lastzero = true;
			bcopy(tr1.data, xr, ntsize);
			bzero(xr + nt, nzeros);
			bzero(xi, (int) nfftsize);
		}

		/* Fft */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);

		/* Untwin the traces and store the non-redundant values */
		tr1.data[0] = xr[0];
		tr1.data[1] = 0.0;
		tr2.data[0] = xi[0];
		tr2.data[1] = 0.0;
		for (i = 1; i < nfby2; i++) {
			tr1.data[2*i]   = 0.5*(xr[i] + xr[nfft-i]);
			tr1.data[2*i+1] = sign * 0.5*(xi[i] - xi[nfft-i]);
			tr2.data[2*i]   = 0.5*(xi[i] + xi[nfft-i]);
			tr2.data[2*i+1] = sign * 0.5*(-xr[i] + xr[nfft-i]);
		}

		/* Pack the exceptional values */
		if (ISODD(nfft)) {
			tr1.data[1] = 0.5 * (xr[nfby2] + xr[nfft - nfby2]);
			tr2.data[1] = 0.5 * (xi[nfby2] + xi[nfft - nfby2]);
			tr1.data[nfm1] = sign * 0.5 *
						(xi[nfby2] - xi[nfby2p1]);
			tr2.data[nfm1] = sign * 0.5 *
						(-xr[nfby2] + xr[nfby2p1]);
		} else {
			tr1.data[1] = xr[nfby2];
			tr2.data[1] = xi[nfby2];
		}
		tr2.trid = tr1.trid = FPACK;
		tr2.ns = tr1.ns = nfft;
		puttr(&tr1);
		if (!lastzero) {
			puttr(&tr2);
		}
	}


	return SUCCEED;
}
