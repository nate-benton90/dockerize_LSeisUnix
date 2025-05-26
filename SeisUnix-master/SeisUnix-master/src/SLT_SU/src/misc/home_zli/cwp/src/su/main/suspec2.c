/* SUSPEC2: $Revision: 1.6 $ ; $Date: 91/02/27 10:51:47 $		*/

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
" SUSPEC2 - 2-d Fourier SPECtrum of data set			\n"
" 								\n"
" suspec2 <infile >outfile [optional parameters]		\n"
"								\n"
" Optional parameters:						\n"
"								\n"
" dt=from header		time sampling interval		\n"
" dx=from header(d2) or 1.0	spatial sampling interval	\n"
"								\n"
" Note: To facilitate further processing, the sampling intervals\n"
"       in frequency and wavenumber as well as the first	\n"
"	frequency (0) and the first wavenumber are set in the	\n"
"	output header (as respectively d1, d2, f1, f2).		\n"
" 								\n"
" Note: The relation: w = 2 pi F is well known, but there	\n"
"	doesn't	seem to be a commonly used letter corresponding	\n"
"	to F for the spatial conjugate transform variable.  We	\n"
"	use K for this.  More specifically we assume a phase:	\n"
"		i(w t - k x) = 2 pi i(F t - K x).		\n"
"	and F, K define our notion of frequency, wavenumber.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Dave (algorithm), Jack (reformatting for SU)
 */


#define PFA_MAX	720720	/* Largest allowed nfft	          */

segy intrace, outtrace;

main(int argc, char **argv)
{
	int nt,nx;		/* numbers of samples			*/
	float dt,dx;		/* sampling intervals			*/
	float d1,d2;		/* output intervals in F, K		*/
	float f1,f2;		/* output first samples in F, K		*/
	int it,ix;		/* sample indices			*/
	int ntfft,nxfft;	/* dimensions after padding for FFT	*/
	int nF,nK;		/* transform (output) dimensions	*/
	int iF,iK;		/* transform sample indices		*/
	register complex **ct;	/* complex FFT workspace		*/
	register float **rt;	/* float FFT workspace			*/
	FILE *tracefp;		/* temp file to hold traces		*/


	/* Hook up getpar to handle the parameters */
	initargs(argc,argv);
	askdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&intrace))  err("can't get first trace");
	nt = intrace.ns;


	/* dt is used only to set output header value d1 */
	if (!getparfloat("dt", &dt)) {
		if (intrace.dt) { /* is dt field set? */
			dt = (float) intrace.dt / 1000000.0;
		} else { /* dt not set, assume 4 ms */
			dt = 0.004;
			warn("tr.dt not set, assuming dt=0.004");
		}
	}
	if (!getparfloat("dx",&dx)) {
		if (intrace.d2) { /* is d2 field set? */
			dx = intrace.d2;
		} else {
			dx = 1.0;
			warn("tr.d2 not set, assuming d2=1.0");
		}
	}


	/* Store traces in tmpfile while getting a count */
	/*tracefp = etmpfile();*/
	tracefp = etempfile(NULL);
	nx = 0;
	do { 
		++nx;
		efwrite(intrace.data, FSIZE, nt, tracefp);
	} while (gettr(&intrace));


	/* Determine lengths for prime-factor FFTs */
	ntfft = npfar(nt);
	nxfft = npfa(nx);
	if (ntfft >= MIN(SU_NFLTS, PFA_MAX)) err("Padded nt=%d--too big",ntfft);
	if (nxfft >= MIN(SU_NFLTS, PFA_MAX)) err("Padded nx=%d--too big",nxfft);


	/* Determine output header values */
	d1 = 1.0/(ntfft*dt);
	d2 = 1.0/(nxfft*dx);
	f1 = 0.0;
	f2 = -1.0/(2*dx);


	/* Determine complex transform sizes */
	nF = ntfft/2+1;
	nK = nxfft;


	/* Allocate space */
	ct = alloc2complex(nF, nK);
	rt = alloc2float(ntfft, nxfft);


	/* Load traces into fft arrays and close tmpfile */
	rewind(tracefp);
	for (ix=0; ix<nx; ++ix) {

		efread(rt[ix], FSIZE, nt, tracefp);

                /* if ix odd, negate to center transform of dimension 2 */
                if (ISODD(ix))
			for (it=0; it<nt; ++it)  rt[ix][it] = -rt[ix][it];

		/* pad dimension 1 with zeros */
		for (it=nt; it<ntfft; ++it)  rt[ix][it] = 0.0;
	}
	efclose(tracefp);


	/* Pad dimension 2 with zeros */
	for (ix=nx; ix<nxfft; ++ix)
		for (it=0; it<ntfft; ++it)  rt[ix][it] = 0.0;

	
	/* Fourier transform dimension 1 */
	pfa2rc(1,1,ntfft,nx,rt[0],ct[0]);
	

	/* Fourier transform dimension 2 */
	pfa2cc(-1,2,nF,nxfft,ct[0]);
	

	/* Compute and output amplitude spectrum */
	for (iK=0; iK<nK; ++iK) {
		for (iF=0; iF<nF; ++iF)  outtrace.data[iF] = fcabs(ct[iK][iF]);

		/* set header values */
		outtrace.tracl = iK + 1;
		outtrace.ns = nF;
		outtrace.dt = 0;  /* d1 is now the relevant step size */
		outtrace.trid = KOMEGA;
		outtrace.d1 = d1;
		outtrace.f1 = f1;
		outtrace.d2 = d2;
		outtrace.f2 = f2;

		puttr(&outtrace);
	}
}
