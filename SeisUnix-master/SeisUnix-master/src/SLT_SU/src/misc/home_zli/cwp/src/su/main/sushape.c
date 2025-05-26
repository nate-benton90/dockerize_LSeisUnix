/* SUSHAPE: $Revision: 1.2 $ ; $Date: 91/03/20 07:18:51 $		*/

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
#include "header.h"

/*********************** self documentation ******************************/
string sdoc =
" 									\n"
" SUSHAPE - Wiener shaping filter					\n"
" 									\n"
" sushape <stdin >stdout  d= [optional parameters]			\n"
" 									\n"
" Required parameters:							\n"
"	wtrace= trace of input wavelet to be shaped				\n"
" 									\n"
"	dtrace= trace of desired output wavelet				\n"
" 									\n"
" 	dt is a mandatory getpar if not set in header	 		\n"
" 									\n"
" Optional parameters:							\n"
" 									\n"
"      nshape=trace	length of shaping filter			\n"
" 									\n"
"      pnoise=0.001	relative additive noise level			\n"
" 									\n"
"      showshaper=0	=1 to show shaping filter 			\n"
"      freq=0 to design/apply filter in time domain	\n" 
"           1 to design/apply filter in frequency domain	\n" 
"      onlyphase=0 to apply amplity and phase filter in frequency domain \n"
"                1 to apply phase-only filter in frequency domain \n"
"                  ignored when freq=0				\n"
" 									\n"
" Trace header fields accessed: ns, dt					\n"
" Trace header fields modified: none					\n"
" 									\n"
" 	To get the shaping filters into an ascii file:			\n"
" 	... | sushape ... showwshaper=1 2>file | ...   (sh or ksh)	\n"
" 	(... | sushape ... showshaper=1 | ...) >&file  (csh)		\n"
" 									\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Jack
 *        : Z. Li    change w and d arrays to be wtrace and dtrace
 *                   (seismic trace input)
 *
 */


#define PNOISE	0.001


segy intrace, outtrace, tr;

main(int argc, char **argv)
{
	int nt;			/* number of points on trace		*/
	float dt;		/* time sample interval (sec)		*/
	float *shaper;		/* shaping filter coefficients		*/
	float *spiker;		/* spiking decon filter (not used)	*/
	float *w;		/* input wavelet			*/
	int nw;			/* length of input wavelet in samples	*/
	float *d;		/* desired output wavelet		*/
	int nd;			/* length of desired wavelet in samples	*/
	int nshape;		/* length of shaping filter in samples	*/
	float pnoise;		/* pef additive noise level		*/
	float *crosscorr;	/* right hand side of Wiener eqs	*/
	float *autocorr;	/* vector of autocorrelations		*/
	int showshaper;		/* flag to display shaping filter	*/
	int	freq;		/* design/apply filter in frequency domain */
	complex *a, *b;
	float *c, amax, atmp, tmp;
	complex *e, *f;
	float *wa, *wf;
	int na, nf, ntmp;
	int onlyphase;

	char *wtrace, *dtrace;
	FILE *trfp;
	int it;



	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&intrace)) err("can't get first trace");
	nt = intrace.ns;
	dt = (float)intrace.dt/1000000.0; if (!dt) MUSTGETPARFLOAT ("dt", &dt);

	/* Get parameters */
	if (!getparint("showshaper",  &showshaper))	showshaper = 0;

	if (!getparint("nshape",  &nshape))		nshape = nt;
	
	if (!getparfloat("pnoise",  &pnoise))	pnoise = PNOISE;

	if (!getparstring("wtrace",&wtrace))  
		err("must specify wtrace= input wavelet");
	trfp = efopen(wtrace,"r");
	efseek(trfp,3600,0);
	if (!auxgettr(trfp,&tr)) err(" can't read wtrace"); 
	nw = tr.ns;
	w = ealloc1float(nw);	
	for(it=0;it<nw;it++) w[it] = tr.data[it];
	efclose(trfp);

	if (!getparstring("dtrace",&dtrace))  
		err("must specify dtrace= desired wavelet");
	trfp = efopen(dtrace,"r");
	efseek(trfp,3600,0);
	if (!auxgettr(trfp,&tr)) err(" can't read dtrace"); 
	nd = tr.ns;
	d = ealloc1float(nd);	
	for(it=0;it<nd;it++) d[it] = tr.data[it];
	efclose(trfp);

	if (!getparint("freq",  &freq))	freq = 0;
	if (!getparint("onlyphase",  &onlyphase))	onlyphase = 0;

	if(freq==0) {
	/* Get shaping filter by Wiener-Levinson */
	shaper	  = ealloc1float(nshape);
	spiker 	  = ealloc1float(nshape);	/* not used */
	crosscorr = ealloc1float(nshape);
	autocorr  = ealloc1float(nshape);
	autocorr[0] *= 1.0 + pnoise;			/* whiten */
	xcor(nw, 0, w, nw, 0, w, nshape, 0, autocorr);  /* for matrix */
	xcor(nw, 0, w, nd, 0, d, nshape, 0, crosscorr); /* right hand side */
	if (CLOSETO(autocorr[0], 0.0))  err("can't shape with zero wavelet");
	stoepf(nshape, autocorr, crosscorr, shaper, spiker);
		
	/* Show shaper on request */
	if (showshaper) {
		register int i;
		warn("Shaping filter:");
		for (i = 0; i < nshape; ++i)
			fprintf(stderr, "%10g%c", shaper[i],
				(i%6==5 || i==nshape-1) ? '\n' : ' ');
	}

	} else {
		nf = nt * 3 / 2;
		ntmp = (nf+1)/2*2;
		radix_(&ntmp,&nf);

		na = nd;
		if(nd<nw) na = nw;
		na = na * 3 / 2;
		ntmp = (na+1)/2*2;
		radix_(&ntmp,&na);

		a = (complex*) emalloc(nf*sizeof(complex));
		b = (complex*) emalloc(nf*sizeof(complex));
		c = (float*) emalloc(nf*sizeof(float));
		e = (complex*) emalloc(nf*sizeof(complex));
		f = (complex*) emalloc(nf*sizeof(complex));
		wa = (float*) emalloc((4*nf+60)*sizeof(float));
		wf = (float*) emalloc((4*nf+60)*sizeof(float));
		cffti_(&nf,wa);
		cffti_(&nf,wf);

		for(it=0;it<nw;it++)
				a[it] = cmplx(w[it],0.); 
		for(it=nw;it<nf;it++)
				a[it] = cmplx(0.,0.);
		cfftf_(&nf,a,wa);

		for(it=0;it<nd;it++)
				b[it] = cmplx(d[it],0.); 
		for(it=nd;it<nf;it++)
				b[it] = cmplx(0.,0.);
		cfftf_(&nf,b,wa);

		amax = 0.;
		for(it=0;it<nf;it++) {
			c[it] = fcabs(a[it]);
			if(c[it]>amax) amax = c[it];
		}
		amax = sqrt(amax);
		atmp = amax*pnoise; 
		for(it=0;it<nf;it++) {
			tmp = (c[it]+atmp)/(c[it]+atmp*0.000001); 
			a[it] = crmul(a[it],tmp);
		}
		for(it=0;it<nf;it++)
			f[it] = cdiv(b[it],a[it]);
		tmp = 1./nf;
		for(it=0;it<nf;it++)
			f[it] = crmul(f[it],tmp);
		if(onlyphase==1) {
			for(it=0;it<nf;it++)
				c[it] = 1./fcabs(f[it]);
			for(it=0;it<nf;it++)
				f[it] = crmul(f[it],c[it]);
		}
	}



	/* Main loop over traces */
	do {
		if(freq==0) {
			/* Center and convolve shaping filter with trace */
			conv(nshape, (nw-nd)/2, shaper, nt, 0, intrace.data, 
                     nt, 0, outtrace.data);        
		} else {
			for(it=0;it<nt;it++)
				e[it] = cmplx(intrace.data[it],0.);
			for(it=nt;it<nf;it++)
				e[it] = cmplx(0.,0.);
			cfftf_(&nf,e,wf);
			for(it=0;it<nf;it++) { 
				a[it] = cmul(e[it],f[it]);
			}
			cfftb_(&nf,a,wf);
			for(it=0;it<nt;it++) outtrace.data[it] = a[it].r;
		}


		/* Output filtered trace */
		memcpy((char*)&outtrace, (char*)&intrace, HDRBYTES);
		puttr(&outtrace);

	} while (gettr(&intrace));


	return EXIT_SUCCESS;
}
