/* SUENV:  $Revision: 2.11 $ ; $Date: 89/09/20 19:35:18 $	*/

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

/*************** self documentation **************/
string sdoc = "\
						\n\
SUENV - Complex envelope or phase time trace	\n\
						\n\
suenv <stdin >stdout mode=amp			\n\
						\n\
Required parameters:				\n\
	none					\n\
						\n\
Optional parameter:				\n\
	mode = amp	output flag 		\n\
	       		amp = envelope traces	\n\
	       		phase = phase traces	\n\
					       	\n\
";
/*************************************************/

/* Credits:
 *	CWP: Jack
 *
 * Algorithm:
 *	c(t) = CABS( [INVFFT{ 2 * STEP(omega)*FFT(f) }] )
 *
 *
 * Caveat:
 *	No phase unwrapping precautions are taken in the mode=phase
 *	branch.
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suenv.c,v $";
static string revid =
	"   $Revision: 2.11 $ ; $Date: 89/09/20 19:35:18 $";



#define	FACMAX		12		/* For FFTPACK	*/
#define	AMP		 1
#define	ARG		 2

segy tr;


main(argc, argv)
int argc; char **argv;
{
	string mode;		/* display: real, imag, amp, arg	*/
	int imode;		/* integer abbrev. for mode in switch	*/
	float *hcos;		/* hold cosines for fft			*/
	float *hsin;		/* hold sines for fft			*/
	register float *xr;	/* real part of trace			*/
	register float *xi;	/* imaginary part of trace		*/
	float xri;		/* temporary for xr[i]			*/
	float xii;		/* temporary for xi[i]			*/
	float xrtmp;		/* temporary for xr[nfby2]		*/
	float *wr;		/* work area for fft			*/
	float *wi;		/* work area for fft			*/
	int nt;			/* number of points on input trace	*/
	int ntsize;		/* nt in bytes				*/
	int nfft;		/* number of points on output trace	*/
	int nfftsize;		/* nfft in bytes			*/
	int nfby2;		/* nfft/2				*/
	float onfft;		/* 1.0 / nfft				*/
	float onfby2;		/* 2.0 / nfft				*/
	int nfac;		/* number of factors of nfft		*/
	int facs[FACMAX];	/* contains factors of nfft		*/
	int nzeros;		/* number of padded zeroes in bytes	*/
	register int i;		/* counter				*/

	
	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	if (tr.trid && tr.trid != TREAL) {
		err("input is not seismic data, trid=%d", tr.trid);
	}

	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;

	/* Get mode */
	if (!sgetpar("mode", &mode))	mode = "amp";

	if (STREQ(mode, "amp"))        imode = AMP;
	else if (STREQ(mode, "phase")) imode = ARG;
	else err("unknown mode=\"%s\"", mode);

	/* Set up for fft */
	fftfac_(&nt, &nfft, &nfac, facs);
	if (nfft > SU_NFLTS) {
		err("Padded nt=%d -- too big", nfft);
	}
	nfby2 = nfft / 2;
	onfft = 1.0 / (float) nfft;
	onfby2 = 2.0 * onfft;
	ntsize = nt * FSIZE;
	nfftsize = nfft * FSIZE;
	nzeros = nfftsize - ntsize;

	/* Allocate fft arrays */
	hcos = vec(nfft);	hsin = vec(nfft);
	xr   = vec(nfft);	xi   = vec(nfft);
	wr   = vec(nfft);	wi   = vec(nfft);

	/* Set up tables */
	ffttab_(&nfft, hcos, hsin);


	/* Loop over traces */
	do {
		/* Load traces into xr, xi (zero-padded) */
		bcopy(tr.data, xr, ntsize);
		bzero(xr + nt, nzeros);
		bzero(xi, nfftsize);

		/* Fft */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);

		/* Apply 2*step, conjugate and scale for inverse fft */
		xr[0] *= onfft;  /* use 1*step at zero frequency */
		xi[0] *= onfft;
		xrtmp = xr[nfby2]; /* save for n even case */
		for (i = 1; i <= nfby2; ++i) {
			xr[i] *=  onfby2; /* equivalent to *= (2/nfft) */
			xi[i] *= -onfby2;
			xr[nfft - i] = xi[nfft - i] = 0.0;
		}
		if (!ISODD(nfft)) { /* n/2 wiped above to correctly do n odd */
			xr[nfby2] = xrtmp * onfby2;/* xi[nfby2] = 0 is right */
		}

		/* Invert */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);

		/* Form absolute value or phase value */
		for (i = 0; i < nt; ++i) {
			xri =  xr[i];
			xii = -xi[i]; /* minus for inverse fft */
			switch(imode) {
			case AMP:
				tr.data[i] = sqrt(xri*xri + xii*xii);
				tr.trid = ENVELOPE;
			break;
			case ARG:
				tr.data[i] = atan2(xii, xri);
				tr.trid = INSTPHASE;
			break;
			default:
				err("%s: mysterious mode=\"%s\"",
							__LINE__, mode);
			}

		}

		puttr(&tr);

	} while (gettr(&tr));


	return SUCCEED;
}
