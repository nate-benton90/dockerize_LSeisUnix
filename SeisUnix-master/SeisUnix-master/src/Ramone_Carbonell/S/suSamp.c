/* Copyright (c) Colorado School of Mines, 2010.*/
/* All rights reserved.                       */

/* SUAMP: $Revision: 1.16 $ ; $Date: 2010/01/27 18:45:01 $		*/

#include "su.h"
#include "segy.h"
#include "header.h"
#include "bheader.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 								",
" SUSAMP - output amp, phase, real or imag trace from		",
" 	(frequency, x) domain data				",
" 								",
" suSamp <stdin >stdout mode=amp				",
" 								",
" Required parameters:						",
" none								",
" Optional parameter:						",
" mode=amp	output flag		 			",
" 		=amp	output amplitude traces			",
" 	       	=phase	output phase traces			",
" Notes:							",
" 	The trace returned is half length from 0 to Nyquist. 	",
" 								",
" Example:							",
" 	sufft <data | suamp >amp_traces				",
" Example: 							",
"	suStrnsfrm < data > complex_traces			",
" 	suSamp < s.transform    mode=amp > Spectra		",
"  								",
" Note: the inverse of the above operation is: 			",
"       suop2 real_traces imag_traces op=zipper > complex_traces",
"  								",
" Note: Explanation of jack=1 					",
" The amplitude spectrum is the modulus of the complex output of",
" the fft. f(0) is thus the average over the range of integration",
" of the transform. For causal functions, or equivalently, half",
" transforms, f(0) is 1/2 of the average over the full range.	",
" Most oscillatory functions encountered in wave applications are",
" zero mean, so this is usually not an issue.			",
NULL};

/* Credits:
 *	CWP: Shuki Ronen, Jack K. Cohen c.1986
 *
 * Notes:
 *	If efficiency becomes important consider inverting main loop
 *      and repeating extraction code within the branches of the switch.
 *
 * Trace header fields accessed: ns, trid
 * Trace header fields modified: ns, trid
 */
/**************** end self doc ***********************************/

#define	AMP	1
#define	PHS	2

segy intrc, outrc;

int
main(int argc, char **argv)
{
   cwp_String mode;	/* display: real, imag, amp, arg	*/
   int imode=AMP;	/* integer abbrev. for mode in switch	*/
   int i,nf,ns;		/* number of samples on input trace	*/
   float *xr,*xi,*r;	/* real part of trace			*/

   /* Initialize */
   initargs(argc, argv);
   requestdoc(1);

   /* Get information from first trace */
   if (!gettr(&intrc)) err("can't get first trace");
   if (intrc.duse != 4) err(" input data not S transform ");

   nf = (int) intrc.ntr/2; 
   ns = (int) intrc.ns;   

   /* Get mode; note that imode is initialized to AMP */
   if (!getparstring("mode", &mode))	mode = "amp";
   if      (STREQ(mode, "phase")) imode = PHS;
   else if (STREQ(mode, "amp")) imode = AMP;
   else if (!STREQ(mode, "amp")) err("unknown mode=\"%s\", see self-doc", mode);

   /* Allocate arrays for real and imaginary parts */

   if (!(xr = (float*)malloc(ns*sizeof(float)))) err(" *malloc* xr failed\n");
   if (!(xi = (float*)malloc(ns*sizeof(float)))) err(" *malloc* xi failed\n");
   if (!(r  = (float*)malloc(ns*sizeof(float)))) err(" *malloc* xi failed\n");

   /* Main loop over traces */
   do {
      bcopy(&intrc,&outrc,SEGY_HDRBYTES);
      if (intrc.trid == REALPART) bcopy(intrc.data, xr, ns*sizeof(float));
      gettr(&intrc);
      if (intrc.trid == IMAGPART) bcopy(intrc.data, xi, ns*sizeof(float));
      if (imode==AMP) {
#pragma omp parallel default(none) shared(xr,xi,r,ns) private(i)
         for (i=0; i<ns; i++) r[i]=sqrt(xr[i]*xr[i]+xi[i]*xi[i]); 
         outrc.trid=AMPLITUDE;
      }
      if (imode==PHS) {
#pragma omp parallel default(none) shared(xr,xi,r,ns) private(i)
         for (i=0; i<ns; i++) r[i]=atan2(xi[i],xr[i]);
         outrc.trid = PHASE;
      }
      bcopy(r, outrc.data, ns*sizeof(float));
      outrc.ntr=nf;
      puttr(&outrc);
   } while (gettr(&intrc));
   return(CWP_Exit());
}
