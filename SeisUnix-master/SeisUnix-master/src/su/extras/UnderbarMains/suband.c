/* SUBAND: $Revision: 2.16 $ ; $Date: 89/09/20 19:35:00 $	*/

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
SUBAND - apply bandpass filter with sine-squared taper		\n\
								\n\
suband <stdin >stdout [optional parameters]			\n\
							        \n\
Required parameters:						\n\
	if dt is not set in header, then dt is mandatory	\n\
							        \n\
Optional parameters: (nyquist calculated internally)		\n\
	f1 = 0.10*(nyquist)	left  low  corner frequency (Hz)\n\
	f2 = 0.15*(nyquist)	left  high corner frequency (Hz)\n\
	f3 = 0.45*(nyquist)	right low  corner frequency (Hz)\n\
	f4 = 0.50*(nyquist)	right high corner frequency (Hz)\n\
	dt = (from header)	time sampling rate (sec)	\n\
							        \n\
The filter is applied in frequency domain.			\n\
							        \n\
Example:							\n\
	suband <data f1=10 f2=12.5 f3=40 f4=50 | ...		\n\
							        \n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Brian, Jack
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
	"   $Source: /src/su/src/RCS/suband.c,v $";
static string revid =
	"   $Revision: 2.16 $ ; $Date: 89/09/20 19:35:00 $";



#define	piby2	1.57079632679490
#define FRAC1	0.10	/* Ratio of default f1 to Nyquist */
#define FRAC2	0.15	/* Ratio of default f2 to Nyquist */
#define FRAC3	0.45	/* Ratio of default f3 to Nyquist */
#define FRAC4	0.50	/* Ratio of default f4 to Nyquist */
#define	FACMAX	12	/* For FFTPACK			  */


segy tr1, tr2;

main(argc, argv)
int argc; char **argv;
{
	bool first=true;	/* flag for first trace			*/
	bool lastzero=false;	/* is this trace a phoney one?		*/
	float *hcos;		/* hold cosines for fft			*/
	float *hsin;		/* hold sines for fft			*/
	register float *xr;	/* real part of trace			*/
	register float *xi;	/* imaginary part of trace		*/
	float *wr;		/* work area for fft			*/
	float *wi;		/* work area for fft			*/
	float *filt;		/* filter array				*/
	float filti;		/* temp for filt[i]			*/
	float f1;		/* left lower corner frequency		*/
	float f2;		/* left upper corner frequency		*/
	float f4;		/* right lower corner frequency		*/
	float f3;		/* right upper corner frequency		*/
	float dt;		/* sample spacing			*/
	float nyq;		/* nyquist frequency			*/
	int nt;			/* number of points on input trace	*/
	int ntsize;		/* nt in bytes				*/
	int nfft;		/* number of points for fft trace	*/
	int nfftsize;		/* nfft in bytes			*/
	int nfby2;		/* nfft/2				*/
	float onfft;		/* reciprocal of nfft			*/
	float df;		/* frequency spacing (from dt)		*/
	float s;		/* storage for sine value		*/
	float c;		/* storage for cosine value		*/
	int if1;		/* integerization of f1			*/
	int if2;		/* integerization of f2			*/
	int if3;		/* integerization of f3			*/
	int if4;		/* integerization of f4			*/
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


	while (gettr(&tr1)) {
		if (first) { /* first trace: initialize */
			first = false;
			if (tr1.trid && tr1.trid != TREAL) {
				err("input is not seismic data, trid=%d",
								tr1.trid);
			}
			nt = tr1.ns;
			if (!fgetpar("dt", &dt))	dt = tr1.dt/1000000.0;
			if (!dt) err("dt field is zero and not getparred");

			nyq = 0.5/dt;

			fftfac_(&nt, &nfft, &nfac, facs);
			if (nfft > SU_NFLTS) {
				err("Padded nt=%d -- too big", nfft);
			}
			nfby2 = nfft / 2;
			ntsize = nt * FSIZE;
			nfftsize = nfft * FSIZE;
			nzeros = nfftsize - ntsize;
			onfft = 1.0 / (float) nfft;


			/* Get corner frequencies */
			if (!fgetpar("f1", &f1))	f1 = FRAC1 * nyq;
			if (!fgetpar("f2", &f2))	f2 = FRAC2 * nyq;
			if (!fgetpar("f3", &f3))	f3 = FRAC3 * nyq;
			if (!fgetpar("f4", &f4))	f4 = FRAC4 * nyq;

			if (f1 < 0.0 || f1 > f2 ||
						f2 >= f3 || f3 > f4) {
				err("Bad filter parameters");
			}


			/* Allocate fft arrays */
			hcos = vec(nfft);  hsin = vec(nfft);
			xr   = vec(nfft);  xi   = vec(nfft);
			wr   = vec(nfft);  wi   = vec(nfft);
			filt = vec(nfby2);


			/* Set up tables */
			ffttab_(&nfft, hcos, hsin);

			/* Compute integer frequencies */
			df = onfft / dt;
			if1 = (int) (f1/df + 0.5);
			if2 = (int) (f2/df + 0.5);
			if3 = (int) (f3/df + 0.5);
			if (if3 > nfby2) if3 = nfby2;
			if4 = (int) (f4/df + 0.5);
			if (if4 > nfby2) if4 = nfby2;


			/* Make filter with scale for inverse transform */
			if (if1 == if2) {
				filt[if1] = onfft;
			} else {
				c = piby2 / ((float)(if2 - if1));
				for (i = if1; i <= if2; ++i) {
					s = sin(c*(float)(i - if1));
					filt[i] = s * s * onfft;
				}
			}

			if (if3 == if4) {
				filt[if3] = onfft;
			} else {
				c = piby2 / ((float)(if4 - if3));
				for (i = if3; i <= if4; ++i) {
					s = sin(c * (float)(if4 - i));
					filt[i] = s * s * onfft;
				}
			}

			for (i = if2 + 1; i < if3; ++i) filt[i] = onfft; 

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
			bzero(xi, nfftsize);
		}


		/* FFT */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);

		/* Apply filter */
		for (i = 1; i < if1; ++i)
			xr[i] = xi[i] = xr[nfft-i] = xi[nfft-i] = 0.0;

		for (i = if4 + 1; i <= nfby2; ++i)
			xr[i] = xi[i] = xr[nfft-i] = xi[nfft-i] = 0.0;

		if (if1 == 0) {
			xr[0] *=  filt[0];
			xi[0] *= -filt[0];
			for (i = 1; i <= if4; ++i) {
				filti = filt[i];
				xr[i]      *=  filti;
				xr[nfft-i] *=  filti;
				xi[i]      *= -filti; /* - for inverse trans */
				xi[nfft-i] *= -filti; /* - for inverse trans */
			}
		} else {
			xr[0] = xi[0] = 0.0;
			for (i = if1; i <= if4; ++i) {
				filti = filt[i];
				xr[i]      *=  filti;
				xr[nfft-i] *=  filti;
				xi[i]      *= -filti; /* - for inverse trans */
				xi[nfft-i] *= -filti; /* - for inverse trans */
			}
		}

		/* Invert */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);


		/* Load traces back in, conjugating for inverse transform */
		/* Recall that the filter had nfft factor                 */
		for (i = 0; i < nt; ++i) {
			tr1.data[i] =  xr[i];
			tr2.data[i] = -xi[i];
		}

		puttr(&tr1);

		if (!lastzero) puttr(&tr2);
	}


	return SUCCEED;
}
