/* SULOG: $Revision: 1.3 $ ; $Date: 90/11/15 10:43:34 $		*/

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

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SULOG -- time axis log-stretch of seismic traces		\n\
								\n\
sulog [optional parameters] <stdin >stdout 			\n\
								\n\
Required parameters:						\n\
	none				 			\n\
								\n\
Optional parameters:						\n\
	ntmin= .1*nt		minimum time sample of interest	\n\
	outpar=/dev/tty		output parameter file, contains:\n\
				number of samples (nt=)		\n\
				minimum time sample (ntmin=)	\n\
				output number of samples (ntau=)\n\
	m=3			length of stretched data	\n\
				is set according to		\n\
					ntau = nextpow(m*nt)	\n\
	ntau= pow of 2		override for length of stretched\n\
				data (useful for padding zeros	\n\
				to avoid aliasing)		\n\
								\n\
NOTES:								\n\
	ntmin is required to avoid taking log of zero and to 	\n\
	keep number of outsamples (ntau) from becoming enormous.\n\
        Data above ntmin is zeroed out.				\n\
								\n\
	The output parameters will be needed by suilog to 	\n\
	reconstruct the original data. 				\n\
								\n\
EXAMPLE PROCESSING SEQUENCE:					\n\
		sulog outpar=logpar <data1 >data2		\n\
		suilog par=logpar <data2 >data3			\n\
								\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Shuki, Chris
 *
 * Caveats:
 * 	Amplitudes are not well preserved.
 */


segy tr;

main(int argc, char **argv)
{
	float *buf;	/* temporary repository of log stretched data 	*/
	float dt;	/*  			*/
	float dtau;	/*  			*/
	float *t;	/* fractional sample number on input data	*/
	float *w;	/* Interpolation weights 			*/
	int *it;	/*	??????					*/
	int itau;	/* tau sample counter				*/
	int m;		/* m*nt=ntau 					*/
	int nt;		/* time samples on input data			*/
	int ntau;	/* pow of 2 >= ntautemp  ==  samples in outdata	*/
	uint ntausize;	/*  ... in bytes				*/
	int ntautemp;	/* tau sample corresponding to nt	 	*/
	int ntmin;	/* minimum input time of interest		*/
	int nw;		/* Number of interpolation weights (2=linear)	*/
	string outpar;	/* name of file holding output parfile		*/
	FILE *outparfp;	/* ... its file pointer				*/
	int nextpower();/* function for padding to power of 2		*/
	void stretch();
	void lintrp();


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get information from the first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000000.0;

	/* Minimum sample of interest (data above tmin is lost) */
	if (!igetpar("ntmin", &ntmin))		 ntmin = 0.1 * nt;
	if (!sgetpar("outpar", &outpar))	 outpar = "/dev/tty";

	/* Open file to save parameters */
	outparfp = efopen(outpar, "w");

	/* 2 weights for linear interpolation */
	nw = 2;
	
	/* fix length of stretched data (but will be padded) */
	if (!igetpar("m", &m))		 m = 3;
	ntautemp = m*nt;  

	/* find dtau, dt in log argument cancels */ 
	dtau = log((float) nt/ntmin) / ntautemp;
	if (!igetpar("ntau", &ntau))	 ntau = nextpower(2, ntautemp);
	ntausize = ntau * FSIZE;
	
	/* Put out enough parameters to reconstruct original data */
	/* (ntau for reassurance to user, also in tr.ns of output) */
	(void) fprintf(outparfp, "nt=%d ntmin=%d dt=%g ntau=%d\n", 
					nt, ntmin, dt*1000000.0, ntau);

	/* Allocate space for stretching operation */
	t   = ealloc1float(ntau);
	it  = ealloc1int(ntau);
	w   = ealloc1float(nw * ntau);
	buf = ealloc1float(ntau);


/* 	The log-stretch from 't' to 'tau' is given mathematically by  	
 *  								
 * 		tau = log( t / tmin ) + taumin           
 *  							
 * 	taumin is arbitrary and taken to be taumin=0	
 */
  					
	/* Calculate fractional t-sample that each tau-sample maps to;  */
	/* [ itau=0 --> (float)itmin ..&.. itau=ntau --> (float)itmax ] */
	for (itau = 0; itau < ntau; itau++) {
		t[itau] = (float) ntmin * exp((float) itau*dtau);
	}

	/* Calculate the linear interpolation coefficients */
	lintrp (t, w, it, nt, ntau);

	/* Main loop over traces */
	do {
		/* Perform the stretch; put new data into buf */
		stretch (buf, tr.data, w, it, ntau, nw);

		/* Overwrite the segy data */
		memcpy((char*)tr.data, (char*)buf, ntausize);

		tr.ns = ntau;
		tr.dt = dtau*1000000.;

		puttr(&tr);

	} while (gettr(&tr));

	
	return EXIT_SUCCESS;
}

void stretch(float *q, float *p, float *w, int *it, int lq, int nw)
/*
 *  General coordinate stretch with predetermined coefficients
 *
 *         NW-1
 * Q(T) =  SUM W(T,J)*P(IT(T)), FOR T=0,LQ-1
 *         J=0
 */
{
	int j, i;

	for (i = 0; i < lq; i++) {
		q[i] = 0.0;
		for (j=0; j<nw; j++) {
			q[i] += w[i*nw+j] * p[it[i]+j];
		}
	}
	return;
}

void lintrp (float *q, float *w, int *it, int lp, int lq)
{
	int i;
	float delta;

	for (i = 0; i < lq; i++) {
		if (q[i] >= 0.0 && q[i] < lp - 1) {
			it[i] = q[i]; 
			delta = q[i] - it[i];
			w[i*2] = 1.0 - delta;
			w[i*2+1] = delta;
		} else {
			it[i] = 0;
			w[i*2] = 0.0;
			w[i*2+1] = 0.0;
		}
	}
	return;
}

int nextpower(p, n)
int p, n;
{
	int nn;
	if (!n) return 0;
	for (nn = 1; nn < n; nn *= p);
	return nn;
}
