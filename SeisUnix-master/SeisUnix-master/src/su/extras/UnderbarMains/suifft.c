/* SUIFFT: $Revision: 2.8 $ ; $Date: 89/09/20 19:36:14 $	*/

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
SUIFFT - fft frequency to time					\n\
								\n\
suiftt <stdin >sdout sign=-1					\n\
								\n\
Required parameters:						\n\
	none							\n\
								\n\
Optional parameter:						\n\
	sign = -1	sign in exponent of fft			\n\
								\n\
The input is assumed to be `packed' frequency traces as produced\n\
by sufft.  The output is time domain data.  See the self-doc for\n\
sufft for further details.					\n\
								\n\
The output traces are normalized by 1/N where N is the fft size.\n\
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
 *	exploit it, we transform two traces at a time.  It would
 *	be better to develop an analogous real fft and avoid the
 *	tricky coding involved.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suifft.c,v $";
static string revid =
	"   $Revision: 2.8 $ ; $Date: 89/09/20 19:36:14 $";



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
	float *x1;		/* 1st trace and later re part of twin	*/
	float *x2;		/* 2nd trace and later im part of twin	*/
	float *x1r;		/* re part of unpacked 1st trace	*/
	float *x1i;		/* im part of unpacked 1st trace	*/
	float *x2r;		/* re part of unpacked 2nd trace	*/
	float *x2i;		/* im part of unpacked 2nd trace	*/
	float *wr;		/* work area for fft			*/
	float *wi;		/* work area for fft			*/
	int nt;			/* number of points on input trace	*/
	int ntsize;		/* nt*sizeof(float)			*/
	int ntby2;		/* nt / 2				*/
	int ntby2p1;		/* (nt/2) + 1				*/
	int ntm1;		/* nt - 1				*/
	int nfft;		/* number of points on output trace	*/
	int nfac;		/* number of factors of nfft		*/
	int facs[FACMAX];	/* contains factors of nfft		*/
	int sign;		/* sign in exponent of transform	*/
	register int i;		/* counter				*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID)) ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	while (gettr(&tr1)) {
		if (first) { /* first trace: initialize */
			first = false;
			if (tr1.trid != FPACK) {
				err("input not packed freq data, trid=%d",
								tr1.trid);
			}
			nt = tr1.ns;
			ntsize = nt * FSIZE;
			ntby2 = tr1.ns/2;
			ntby2p1 = ntby2 + 1;
			ntm1 = tr1.ns - 1;

			fftfac_(&nt, &nfft, &nfac, facs);
			if (nfft > SU_NFLTS) {
				err("Padded nt=%d -- too big", nfft);
			}

			/* Set sign in exponent of transform */
			if (!igetpar("sign", &sign)) sign = -1;
			if (sign != 1 && sign != -1) {
			    err("sign = %d must be 1 or -1", sign);
			}

			/* Allocate fft arrays */
			hcos = vec(nfft);  hsin = vec(nfft);
			x1   = vec(nfft);  x2   = vec(nfft);
			x1r  = vec(nfft);  x1i  = vec(nfft);
			x2r  = vec(nfft);  x2i  = vec(nfft);
			wr   = vec(nfft);  wi   = vec(nfft);

			/* Set up tables */
			ffttab_(&nfft, hcos, hsin);

		} /* End of initialization for first trace */

		/* Load traces into x1, x2 */
		if (gettr(&tr2)) {
			bcopy(tr1.data, x1, ntsize);
			bcopy(tr2.data, x2, ntsize);
		} else { /* odd number of traces; make zero trace */
			lastzero = true;
			bcopy(tr1.data, x1, ntsize);
			bzero(x2, ntsize);
		}
		/* Unpack the traces into real and imag parts */
		x1r[0] = x1[0];
		x2r[0] = x2[0];
		x1i[0] = 0.0;
		x2i[0] = 0.0;
		x1r[ntby2] = x1[1];
		x2r[ntby2] = x2[1]; 
		if (ISODD(nt)) {
			x1i[ntby2] = sign * x1[ntm1];
			x2i[ntby2] = sign * x2[ntm1];
			x1r[ntby2p1] =  x1r[ntby2];
			x1i[ntby2p1] = -x1i[ntby2];
			x2r[ntby2p1] =  x2r[ntby2];
			x2i[ntby2p1] = -x2i[ntby2];
		} else {
			x1i[ntby2] = 0.0;
			x2i[ntby2] = 0.0;
		}
		for (i = 1; i < ntby2; i++) {
			x1r[i]    =  x1[2*i]         ;	x1r[nt-i] =  x1r[i];
			x1i[i]    =  sign * x1[2*i+1];	x1i[nt-i] = -x1i[i];
			x2r[i]    =  x2[2*i]         ;	x2r[nt-i] =  x2r[i];
			x2i[i]    =  sign * x2[2*i+1];	x2i[nt-i] = -x2i[i];
		}

		/* Twin the traces */
		x1[0] = x1r[0];
		x2[0] = x2r[0];
		for (i = 1; i < ntby2; i++) {
			x1[i] = x1r[i] - x2i[i];
			x2[i] = x2r[i] + x1i[i];
			x1[nt-i] = x1r[i] + x2i[i];
			x2[nt-i] = x2r[i] - x1i[i];
		}
		if (ISODD(nt)){ /* these come from back half */
			x1[ntby2] = x1r[ntby2p1] + x2i[ntby2p1];
			x2[ntby2] = x2r[ntby2p1] - x1i[ntby2p1];
			x1[ntby2p1] = x1r[ntby2] + x2i[ntby2];
			x2[ntby2p1] = x2r[ntby2] - x1i[ntby2];
		} else {
			x1[ntby2] = x1r[ntby2];
			x2[ntby2] = x2r[ntby2];
		}

		/* Fft */
		fft_(x1, x2, wr, wi, &nfft, facs, &nfac, hcos, hsin);

		for (i = 0; i < nt; i++) {
			tr1.data[i] = x1[i] / ((float) nfft);
			tr2.data[i] = x2[i] / ((float) nfft);
		}

		tr2.trid = tr1.trid = TREAL;
		tr2.ns = tr1.ns = nfft;
		puttr(&tr1);
		if (!lastzero) {
			puttr(&tr2);
		}
	}


	return SUCCEED;
}
