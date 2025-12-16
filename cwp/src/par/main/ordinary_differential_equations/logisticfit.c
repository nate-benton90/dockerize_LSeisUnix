/* Copyright (c) Colorado School of Mines, 2011.*/

/* All rights reserved.		       */

/* LOGISTICFIT: $Revision: 1.3 $ ; $Date: 2020/05/23 21:11:42 $	*/

#include "par.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 									",
" LOGISTICFIT - least squares fit of a LOGISTIC function to input data 	",
" 									",
"  logisticfit <infile >outfile [optional parameters]			",
" 									",
" Required Parameter:							",
" n=			the number of values of t and P(t)		",
" Optional Parameters:							",
" nstart=0		start of input data window			",
" nend=n		end of input data window			",
" 									",
" outpar=/dev/tty 	output parameter file				",
" 									",
" Notes:								",
" If the data are a population P(t) as a function of time t, then the 	",
" input consists of unformatted C-style floats in the form of a vector of",
" length 2n consisting of the vector of the values of 			",
" [t0,t1,t2,...tn-1)] followed by [P(t0),P(t1),P(t2),...,P(tn-1)].	",
"  									",
" Notes about logistic curve fitting:					",
" The logistic function is a nontrivial solution to the Verhulst equation",
" 									",
"		 dP/dt = r P(1-P/K)					",
" 									",
" which describes the growth of a population P(t), with growth rate r   ",
" and carrying capacity K. This equation has analytic solution:		",
" 									",
"	       P(t)= K/(1 + (K-P0)/P0 exp(-r(t-t0))			",
" 									",
" Here P0=P(t0). 							",
" 									",
" The goal of this program is, that given population data that is assumed",
" to be governed by the logistic equation, to estimate K and r. These   ",
" may then be used as input to the program 'verhulst'. In that program  ",
" a1 corresponds to r and a2 corresponds to K here. 			",
" 									",
" Caveat: this code will give results that get you close, but you may   ",
" have to hand adjust r and K to suit your data. 			",
" Related program: verhulst 						",
NULL};

/*
 *   AUTHOR: 
 *      John Stockwell, Adjunct Faculty, Colorado School of Mines,  April 2020
 *
 * Technical reference:
 * https://www.maa.org/press/periodicals/loci/joma/logistic-growth-model-fitting-a-logistic-model-to-data-i
 * Algorithm:
 *
 * Part I:
 * We note
 *	dP/dt = r P(1-P/K)
 * implies
 *      dP/dt
 *    --------- = d/dt ln(P) =  -(r/K) P + r 
 *       P 
 * Hence, plotting d/dt ln(P)  versus  P  will have a linear trend of slope -(r/K) and
 * intercept r. Thus, for y = mx + b    K=(-intercept/slope)
 *
 * The logarithmic derivative is implemented in this code as (dP/dt)/P
 * 
 *
 * Part II:
 * Second we note that the solution is the logistic (sigmoid function) curve given by
 *
 *     P(t) =		 K
 *		 -------------------------- 
 *	       1 + ([K-P0]/P0) exp[-r(t-t0)]
 *
 * Here P0 is the value of P(t) at t0
 *
 * P(t)( 1 + ([K-P0]/P0) exp[-r(t-t0)] ) = K
 *       
 *	     ([K-P0]/P0) exp[-r(t-t0)] = [K-P(t)]/P(t)   
 *       
 *	  -r(t-t0) = ln([K-P(t)]/P(t)) +  ln(P0/[K-P0]) = ln([K-P(t)]/P(t)) + C
 *       
 * Given an estimate of K from the Part I, a line through a plot of 
 *	   ln([K-P(t)]/P(t))  versus time t will have a slope of -r
 *       
 *	  r(t-t0) = ln(P(t)/[K-P(t)]) + C
 *       
 * Here the ln() argument is flipped around to remove the minus sign.
 *       
 */

/**************** end self doc ********************************/

int
main(int argc, char **argv)
{
	/* hook up getpar */
	initargs(argc,argv);
	requestdoc(0);

	char *outpar=NULL;	/* name of file holding output parfile	*/
	FILE *outparfp=NULL;	/* ... its file pointer			*/

	int n=0;		/* number of (t, P(t))			*/
	int nstart=0;		/* start of input data window		*/
	int nend=0;		/* ..end of input data window		*/
	int nwindow=0;		/* number of values in window		*/
	int ncount=0;		/* number of values read		*/

	size_t nread;		/* number of items read			*/
	float tp[1]={0.0};	/* full input data vector t followed by P */
	float *p=NULL;		/* P(t) values binary floats		*/
	float *t=NULL;		/* t binary floats			*/
	float K=0.0;		/* carrying capacity */
	float r=0.0;		/* growth rate */

	float Kfit=0.0;		/* fit coefficient of K */
	float Kerr=0.0;		/* error in K */
	float rfit=0.0;		/* fit coefficient of r */
	float rerr=0.0;		/* error in r */

	

	/* Hook up getpar */
	initargs(argc, argv);
	requestdoc(1);

	/* Get parameters and do set up */
	if (!getparstring("outpar", &outpar))	outpar = "/dev/tty" ;
	outparfp = efopen(outpar, "w");


	MUSTGETPARINT("n",&n); /* the number of t,P(t) values */
	if (!getparint("nstart",&nstart))		nstart=0;
	if (!getparint("nend",&nend))			nend=n;
	if (nend < nstart)
		err("nend cannot be greater than nstart!");
	if (nend > n)
		err("nend cannot be greater than n!");
	if (nstart < 0)
		err("nstart cannot be less than 0!");
	if (nend < 0)
		err("nstart cannot be less than 0!");

	nwindow=(nend-nstart);


	t = ealloc1float(nwindow);
	p = ealloc1float(nwindow);

	/* Loop over data reading time and p data */
	while ((nread = efread(tp, FSIZE, 1, stdin))) {
		if ( ncount >= nstart && ncount < nend )
			t[ncount-nstart] = tp[0];
		if (ncount >= n+nstart)
			p[ncount-(n+nstart)] = tp[0];

		++ncount;
	}

	/* Part I - linear fit of (P,d(ln(P))/dt) */
	{	
		register int i=0;
		float *logpprime=NULL;	/* logpprime_e(P) */
		float coeff[4]={0.0,0.0,0.0,0.0}; /* coefficients from linear_regression */

		logpprime=ealloc1float(nwindow);

		
		/**** differentiate p[i] and divide by p[i] */
		/* do first as a leading difference */
		logpprime[0] = (float) ((p[1] - p[0])/(t[1]-t[0]))/p[0];
		
		/* do the middle values as a centered difference */
		for (i=1; i<nwindow-1; ++i) 
			logpprime[i] = (float) ((p[i+1] - p[i-1])/(2*(t[i+1]-t[i-1])))/p[i];

		/* do last value as a lagging difference */
		logpprime[nwindow-1] = (float) ((p[nwindow-1] - p[nwindow-2])/(t[nwindow-1] - t[nwindow-2]))/p[nwindow-1];
		
		/***** end differentiate **/
		
		/* perform linear regression on logpprime as a function p */
		linear_regression(logpprime, p, nwindow, coeff);

		K = (float) NINT(-coeff[1]/coeff[0]);
		if (K < p[nwindow-1])
			warn("K = %f is less than largest value p[%d]=%f!",
					K,nwindow-1,p[nwindow-1]);

		Kfit = coeff[2];
		Kerr = coeff[3];
		
		free1float(logpprime);
	}
	
	/* Part II - linear fit of (log(p[i]/K-p[i],t) */
	{	
		float coeff[4]={0.0,0.0,0.0,0.0}; /* coefficients from linear_regression */
		register int i=0;
		float *logpp=NULL;

		logpp = ealloc1float(nwindow);
		for (i=0; i<nwindow; ++i)
			logpp[i] = (float) log(p[i]/(K-p[i]));

		/* perform linear regression on (log(p[i]/(K-p[i]),t) */
		linear_regression(logpp, t, nwindow, coeff);
	
		r = coeff[0];
		rfit = coeff[2];
		rerr = coeff[3];
	}	

	/* Make par file */
	fprintf(outparfp, "\n");
	fprintf(outparfp, "Carrying capacity estimate: K = %0.0f\n",K);
	fprintf(outparfp, "Growth rate estimate: r = %f\n",r );
	fprintf(outparfp, "\n");
	fprintf(outparfp, "Goodness of fit: K-fit = %f, percent error: K-err = %f\n",Kfit,Kerr);
	fprintf(outparfp, "Goodness of fit: r-fit = %f, percent error: r-err = %f\n",rfit,rerr);


	return(CWP_Exit());
}
