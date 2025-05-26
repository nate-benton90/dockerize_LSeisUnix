/* SUBAND: $Revision: 1.6 $ ; $Date: 91/03/03 09:16:01 $	*/

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
" SUBAND - apply bandpass filter with sine-squared taper	\n"
" 								\n"
" suband <stdin >stdout [optional parameters]			\n"
" 							        \n"
" Required parameters:						\n"
" 	if dt is not set in header, then dt is mandatory	\n"
" 							        \n"
" Optional parameters: (nyquist calculated internally)		\n"
" 	f1 = 0.10*(nyquist)	left  low  corner frequency (Hz)\n"
" 	f2 = 0.15*(nyquist)	left  high corner frequency (Hz)\n"
" 	f3 = 0.45*(nyquist)	right low  corner frequency (Hz)\n"
" 	f4 = 0.50*(nyquist)	right high corner frequency (Hz)\n"
" 	dt = (from header)	time sampling rate (sec)	\n"
" 							        \n"
" The filter is applied in frequency domain.			\n"
" 							        \n"
" Example:							\n"
" 	suband <data f1=10 f2=12.5 f3=40 f4=50 | ...		\n"
" 							        \n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Jack
 *
 * Possible optimization: Do assignments instead of crmuls where
 * filter is 0.0.
 */


#define	piby2	1.57079632679490
#define FRAC1	0.10	/* Ratio of default f1 to Nyquist */
#define FRAC2	0.15	/* Ratio of default f2 to Nyquist */
#define FRAC3	0.45	/* Ratio of default f3 to Nyquist */
#define FRAC4	0.50	/* Ratio of default f4 to Nyquist */
#define LOOKFAC	2	/* Look ahead factor for npfao	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */


segy tr;

main(int argc, char **argv)
{
	register float *rt;	/* real trace				*/
	register complex *ct;	/* complex transformed trace		*/
	float *filt;		/* filter array				*/
	float f1;		/* left lower corner frequency		*/
	float f2;		/* left upper corner frequency		*/
	float f4;		/* right lower corner frequency		*/
	float f3;		/* right upper corner frequency		*/
	int if1,if2,if3,if4;	/* integerizations of f1,f2,f3,f4	*/
	float dt;		/* sample spacing			*/
	float nyq;		/* nyquist frequency			*/
	int nt;			/* number of points on input trace	*/
	int nfft;		/* number of points for fft trace	*/
	int nf;			/* number of frequencies (incl Nyq)	*/
	int nfm1;		/* nf-1					*/
	float onfft;		/* reciprocal of nfft			*/
	float df;		/* frequency spacing (from dt)		*/

	FILE *infp=stdin, *outfp=stdout;

	
	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	file2g(infp);
	file2g(outfp);


	/* Get info from first trace */ 
	if (!gettr(&tr))  err("can't get first trace");

	/*
	if (tr.trid && tr.trid != TREAL)
		err("input is not seismic data, trid=%d", tr.trid);
	*/

	nt = tr.ns;
	if (!getparfloat("dt", &dt))	dt = (float)tr.dt/1000000.0;
	if (!dt) err("dt field is zero and not getparred");
	nyq = 0.5/dt;


	/* Set up FFT parameters */
	nfft = npfaro(nt, LOOKFAC * nt);
	if (nfft >= MIN(SU_NFLTS, PFA_MAX))
		err("Padded nt=%d -- too big", nfft);

	nf = nfft/2 + 1;
	nfm1 = nf - 1;
	onfft = 1.0 / (float) nfft;


	/* Get corner frequencies */
	if (!getparfloat("f1", &f1))	f1 = FRAC1 * nyq;
	if (!getparfloat("f2", &f2))	f2 = FRAC2 * nyq;
	if (!getparfloat("f3", &f3))	f3 = FRAC3 * nyq;
	if (!getparfloat("f4", &f4))	f4 = FRAC4 * nyq;
	if (f1 < 0.0 || f1 > f2 || f2 >= f3 || f3 > f4)
		err("Bad filter parameters");


	/* Allocate fft arrays */
	rt   = ealloc1float(nfft);
	ct   = ealloc1complex(nf);
	filt = ealloc1float(nf);


	/* Compute integer frequencies */
	df = onfft / dt;
	if1 = NINT(f1/df);
	if2 = NINT(f2/df);
	if3 = NINT(f3/df);
	if (if3 > nfm1) if3 = nfm1;
	if4 = NINT(f4/df);
	if (if4 > nfm1) if4 = nfm1;


	/* Make filter with scale for inverse transform */
	if (if1 == if2) {
		filt[if1] = onfft;
	} else {
		register float c = piby2 / ((float)(if2 - if1));
		register float s;
		register int i;

		for (i = if1; i <= if2; ++i) {
			s = sin(c*(float)(i - if1));
			filt[i] = s * s * onfft;
		}
	}

	if (if3 == if4) {
		filt[if3] = onfft;
	} else {
		register float c = piby2 / ((float)(if4 - if3));
		register float s;
		register int i;

		for (i = if3; i <= if4; ++i) {
			s = sin(c * (float)(if4 - i));
			filt[i] = s * s * onfft;
		}
	}

	{ register int i;
	  for (i = if2 + 1; i < if3;     ++i)  filt[i] = onfft; 
	  for (i = 0;       i < if1;     ++i)  filt[i] = 0.0; 
	  for (i = if4 + 1; i < nf; ++i)       filt[i] = 0.0; 
	}



	/* Main loop over traces */
	do {
		register int i;

		if(tr.trid==1) {

			/* Load trace into rt (zero-padded) */
			memcpy((char*)rt, (char*)tr.data, nt*FSIZE);
			bzero(rt + nt, (nfft-nt)*FSIZE);

			/* FFT, filter, inverse FFT */
			pfarc(1, nfft, rt, ct);
			for (i = 0; i < nf; ++i)  ct[i] = crmul(ct[i], filt[i]);
			pfacr(-1, nfft, ct, rt);

			/* Load traces back in, recall filter had nfft factor */
			for (i = 0; i < nt; ++i)  tr.data[i] = rt[i];
		}

		puttr(&tr);
	} while (gettr(&tr));

	return EXIT_SUCCESS;
}
