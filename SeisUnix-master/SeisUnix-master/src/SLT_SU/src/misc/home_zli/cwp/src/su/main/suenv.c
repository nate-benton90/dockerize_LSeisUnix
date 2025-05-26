/* SUENV:  $Revision: 1.6 $ ; $Date: 90/12/18 20:44:58 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*************** self documentation **************/
string sdoc =
" 						\n"
" SUENV - Complex envelope or phase or frequency time trace \n"
" 						\n"
" suenv <stdin >stdout mode=amp			\n"
" 						\n"
" Required parameters:				\n"
" 	none					\n"
" 						\n"
" Optional parameter:				\n"
" 	mode=amp	output flag 		\n"
" 	       		amp = envelope traces	\n"
" 	       		phase = phase traces	\n"
" 	       		freq = frequency traces	\n"
"                       all=amp,phase,frequency \n"
"                           three traces per 	\n"
"                           input trace		\n"
"                           header cdpt=1 amp	\n"
"                                      =2 phase \n"
"                                      =3 freq  \n" 
" 					       	\n"
;
/**************** end self doc *******************/

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
 */


#define	AMP		 1
#define	ARG		 2
#define	FRQ		 3
#define	ALL		 4

#define LOOKFAC	2	/* Look ahead factor for npfao	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */


segy tr;


main(int argc, char **argv)
{
	string mode;		/* display: real, imag, amp, arg	*/
	int imode;		/* integer abbrev. for mode in switch	*/
	register complex *ct;	/* complex trace			*/
	int nt;			/* number of points on input trace	*/
	int nfft;		/* number of points on output trace	*/
	int nfby2;		/* nfft/2				*/
	int nfby2p1;		/* nfft/2 + 1				*/
	float onfft;		/* 1.0 / nfft				*/
	float onfby2;		/* 2.0 / nfft				*/
	complex czero;		/* complex zero				*/
	float odt2;             /* 2.0 / dt				*/

	FILE *infp=stdin, *outfp=stdout;
	
	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	file2g(infp);
	file2g(outfp);

	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;

	if (tr.trid && tr.trid != TREAL) {
		err("input is not seismic data, trid=%d", tr.trid);
	}

	odt2 = 2.0*1000000./(float)tr.dt;
	/* Get mode */
	if (!getparstring("mode", &mode))	mode = "amp";

	if (STREQ(mode, "amp"))        imode = AMP;
	else if (STREQ(mode, "phase")) imode = ARG;
	else if (STREQ(mode, "freq")) imode = FRQ;
	else if (STREQ(mode, "all")) imode = ALL;
	else err("unknown mode=\"%s\"", mode);

	/* Set up for fft */
	nfft = npfao(nt, LOOKFAC * nt);
	if (nfft >= MIN(SU_NFLTS, PFA_MAX))
		err("Padded nt=%d -- too big", nfft);

	nfby2 = nfft/2;
	nfby2p1 = nfby2 + 1;
	onfft = 1.0 / (float) nfft;
	onfby2 = 2.0 * onfft;
	czero = cmplx(0.0, 0.0);


	/* Allocate fft array */
	ct = ealloc1complex(nfft);


	/* Loop over traces */
	do {
		register int i;

		/* Load traces into ct (zero-padded) */
		for (i = 0; i < nt; ++i)  ct[i] = cmplx(tr.data[i], 0.0);
		for (i = nt; i < nfft; ++i)  ct[i] = czero;

		/* Fft */
		pfacc(1, nfft, ct);

		/* Apply 2*step and scale for inverse fft */
		ct[0] = crmul(ct[0], onfft);	/* only scale at dc and Nyq */
		for (i = 1; i <= nfby2; ++i)  ct[i] = crmul(ct[i], onfby2);
		if (!ISODD(nfft)) ct[nfby2] = crmul(ct[nfby2], onfft);
		for (i = nfby2p1; i < nfft; ++i)  ct[i] = czero;

		/* Invert */
		pfacc(-1, nfft, ct);

		/* Form absolute value or phase value */
		switch(imode) {
		case AMP:
			for (i = 0; i < nt; ++i) {
				float re = ct[i].r;
				float im = ct[i].i;
				tr.data[i] = sqrt(re*re + im*im);
			}
			tr.trid = ENVELOPE;
		break;
		case ARG:
			for (i = 0; i < nt; ++i) {
				float re = ct[i].r;
				float im = ct[i].i;
				if (re*re+im*im)  tr.data[i] = atan2(im, re);
				else              tr.data[i] = 0.0;
			}
			tr.trid = INSTPHASE;
		break;
		case FRQ:
			for (i = 1; i < nt; ++i) {
				float r1 = ct[i].r - ct[i-1].r;
				float r2 = ct[i].r + ct[i-1].r;
				float i1 = ct[i].i - ct[i-1].i;
				float i2 = ct[i].i + ct[i-1].i;
				float tmp = r2*r2+r1*r1;
				if(tmp>0) tr.data[i] = odt2 * (i1*r2-i2*r1)/sqrt(tmp);
				else	  tr.data[i] = 0;
			}
			tr.data[0] = tr.data[1];
			tr.trid = INSTFREQ;
		break;
		case ALL:
			for (i = 0; i < nt; ++i) {
				float re = ct[i].r;
				float im = ct[i].i;
				tr.data[i] = sqrt(re*re + im*im);
			}
			tr.trid = ENVELOPE;
			tr.cdpt = 1; 
			puttr(&tr);
			for (i = 0; i < nt; ++i) {
				float re = ct[i].r;
				float im = ct[i].i;
				if (re*re+im*im)  tr.data[i] = atan2(im, re);
				else              tr.data[i] = 0.0;
			}
			tr.trid = INSTPHASE;
			tr.cdpt = 2; 
			puttr(&tr);
			for (i = 1; i < nt; ++i) {
				float r1 = ct[i].r - ct[i-1].r;
				float r2 = ct[i].r + ct[i-1].r;
				float i1 = ct[i].i - ct[i-1].i;
				float i2 = ct[i].i + ct[i-1].i;
				float tmp = r2*r2+r1*r1;
				if(tmp>0) tr.data[i] = odt2 * (i1*r2-i2*r1)/sqrt(tmp);
				else	  tr.data[i] = 0;
			}
			tr.data[0] = tr.data[1];
			tr.cdpt = 3;
			tr.trid = INSTFREQ;
		break;
		default:
			err("%s: mysterious mode=\"%s\"", __LINE__, mode);
		}


		puttr(&tr);

	} while (gettr(&tr));


	return EXIT_SUCCESS;
}

