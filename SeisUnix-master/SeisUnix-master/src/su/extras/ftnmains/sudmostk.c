/* SUDMOSTK: $Revision: 1.2 $ ; $Date: 90/05/25 19:25:39 $	*/

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
string sdoc = "\
								\n\
SUDMOSTK -- DMO stacking of multi-offset data using 		\n\
	    Kirchhoff (x,t) implementation of common offset DMO	\n\
								\n\
sudmostk xmin= nx= dx= [optional parameters] <stdin >stdout 	\n\
								\n\
Required parameters						\n\
	xmin = 	minimum midpoint location on output data 	\n\
   	nx   =  number of midpoints (traces) on output		\n\
	dx   = 	... midpoint spacing ...			\n\
								\n\
Optional parameters						\n\
	v  = 0 		velocity (for operator truncation)	\n\
	dt = (header) 	time sample rate on input/output	\n\
								\n\
NOTE: 	Header fields tr.sx and tr.offset are used.	        \n\
 	Beware of results if these are incorrect or missing	\n\
	on the input data.					\n\
								\n\
	The input can be in any sort (shots, cdps, ...)		\n\
								\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Chris
 *
 */

segy tr;

main(int argc, char **argv)
{
	float *stkdata;		/* mega-vector to contain output data 	*/
	float *wrk1;		/* work area vector 			*/
	float *wrk2;		/* work area vector 			*/
	float *wrk3;		/* work area vector			*/
	float dtin;		/* time sample rate of input		*/
	float dtout;		/* time sample rate of output		*/
	float dx;		/* trace spacing on output data  	*/
	float h;		/* half offset				*/
	float mid;		/* midpoint				*/
	float iwpow;		/* exponent for sqrt(iw) post processing*/
	float v;		/* velocity				*/
	float xmax;		/* maximum midpoint on stkdata		*/
	float xmin;		/* minimum midpoint on stkdata		*/
	int ix;			/* trace counter			*/ 
	int ntout;		/* length of trace on output		*/
	int ntoutbytes;		/* ... in bytes				*/
	int ntoutpad;		/* samples per trace on padded output	*/
	int ntin;		/* length of input traces		*/
	int nx;			/* traces in output data		*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Read first trace & check that data is time-domain */ 
	if (!gettr(&tr) ) err("can't get first trace\n");
	if (tr.trid != TREAL && tr.trid != 0) {  
		err("input is not t-domain data, trid=%d",tr.trid);
	}

	/* Get header info & calc some constants for input data  */	
	ntin  = tr.ns; 
	if (!igetpar("ntout", &ntout))	ntout = ntin;
	if (!fgetpar("dt", &dtin))	dtin  = tr.dt/1000000.;
	dtout = dtin;

	/*  xmin, nx and dx must be given by user */
	MUSTFGETPAR("xmin",&xmin);
	MUSTIGETPAR("nx",&nx); 
	MUSTFGETPAR("dx",&dx);
	xmax = ( nx -1 ) * dx + xmin; 
	if (!fgetpar("v", &v))		v = 0.;	

	/* Pad output to power of 2 for sqrt(iw) filtering  */	
	for ( ntoutpad = 1 ; ntoutpad < ntout; ntoutpad *= 2 );


	/*  print some info to reassure the user  */
	fprintf(stderr,"\n");
	fprintf(stderr,"  For DMOSTK: 	  v = %g \n",v);
	fprintf(stderr,"      output:    nt = %d      nx = %d\n",
		ntout,nx);
	fprintf(stderr,"               xmin = %g      dx = %g\n",
		xmin,dx);
	fprintf(stderr,"   work area:    nt = %d      nx = %d \n",
		ntoutpad,nx);

	/* Alloc space for stkdata  */	
	ntoutbytes = ntout * FSIZE; 
	stkdata = ealloc1float(ntoutpad*nx);

	/* Main loop over input traces */
	do {

		/* get half offset and midpoint for this trace */
		h = tr.offset / 2.;
		mid = tr.sx + h;

		/* spread this trace along its impulse response(s) */
		spred1(tr.data,stkdata,&ntin,&dtin,&ntoutpad,&dtout,  
		   	&xmin,&nx,&dx,&h,&mid,&v);   

	} while(gettr(&tr));

	/* Alloc memory for iomegasub work areas 	*/
	wrk1 = ealloc1float(ntoutpad*nx);
	wrk2 = ealloc1float(2*ntoutpad*nx);
	wrk3 = ealloc1float(ntoutpad);

	/* Subroutine to do sqrt(i*w) post processing on stkdata */
	iwpow = .5;
      	iomegasub(stkdata,&ntoutpad,&dtout,&nx,&iwpow,wrk1,wrk2,wrk3); 

	/* Set trace headers for output data */
	tr.trid = TREAL;
	tr.ntr = nx;
	tr.ns = ntout;
	tr.dt = dtout*1000000.;
	tr.offset = 0.;

	/* Output the result by pulling traces off stkdata mega-vector */
	for ( ix = 0 ; ix < nx ; ix++ ) {
		bcopy( stkdata + ix*ntoutpad, tr.data, ntoutbytes); 
		tr.tracl = ix + 1;	/* sequential traces    */
		tr.cdp = ix + 1;	/* sequential midpoints */
		puttr(&tr);
	}


	return EXIT_SUCCESS;
}
