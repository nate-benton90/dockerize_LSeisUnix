/* SUHALEKDMO: $Revision: 1.2 $ ; $Date: 90/05/24 21:56:26 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
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

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUHALEKDMO -- DMO of Commo Offset data using a Kirchhoff (x,t)	\n\
	      implementation of Hale's DMO			\n\
								\n\
suhalekdmo dx= [optional parameters] <stdin >stdout 		\n\
								\n\
Required parameter						\n\
	dx = 	trace spacing on input data 			\n\
								\n\
Optional parameters						\n\
	v=0 		      velocity (for operator truncation)\n\
	dt= (header) 		time sample rate on input data	\n\
	offset= (1st header) 	common offset of input data	\n\
	nxout= (nx of input)	number of traces on output	\n\
	dxout= (dx of input)	trace spacing on output	data	\n\
	ntout= (nt of input) 	samp/trace on output		\n\
	dtout= (dt of input)	trace spacing on output	data	\n\
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
	float *indata;		/* mega-vector to contain input data 	*/
	float *outdata;		/* mega-vector to contain output data 	*/
	float *wrk1;		/* work area vector 			*/
	float *wrk2;		/* work area vector 			*/
	float *wrk3;		/* work area vector			*/
	float dtin;		/* time sample rate of input		*/
	float dtout;		/* time sample rate of output		*/
	float dxin;		/* trace spacing on input data  	*/
	float dxout;		/* trace spacing on output data  	*/
	float h;		/* offset/2 				*/
	float v;		/* velocity				*/
	float offset;		/* offset in common offset section	*/
	float power;		/* 1/2 					*/
	int ix;			/* trace counter		 	*/
	int nalloc;		/* allocation parameter			*/
	int ntout;		/* length of trace on output		*/
	int ntoutbytes;		/* ... in bytes				*/
	int ntoutpad;		/* samples per trace on padded output	*/
	int ntin;		/* length of input traces		*/
	int ntinbytes;		/* ... in bytes				*/
	int nxin;		/* traces in input data			*/
	int nxout;		/* traces in output data		*/

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Read first trace & check that indata is time-domain */ 
	if ( !gettr(&tr) ) err("can't get first trace\n");
	if ( tr.trid != TREAL && tr.trid != 0 ) {  
		err("input is not t-domain data, trid=%d",tr.trid);
	}

	/* Get header info & calc some constants for input data  */	
	ntin 	= tr.ns; 
	ntinbytes = ntin * FSIZE;
	if (!fgetpar("dt", &dtin))		dtin = tr.dt/1000000.0;
	dtout 	= dtin;

	/*  dx must be given by user; offset from header for forward
	 *  dmo, and must be given by user for inverse dmo  */	
	MUSTFGETPAR("dx", &dxin);
	if (!fgetpar("dxout", &dxout))		dxout = dxin;
	if (!fgetpar("offset", &offset))	offset = tr.offset;
	h = offset/2.0;
	if (!fgetpar("v", &v))			v = 0.0;

	/* Alloc block of memory for indata 	*/
	nalloc = MAX(NFALLOC, ntin); 
	indata = ealloc1float(nalloc);


	/* Loop over input traces & put them into data mega-vector */
	nxin = 0;
	do {
		++nxin;
		if (nxin*ntin > nalloc) { /* need more memory */	
			nalloc += NFALLOC;
			indata = erealloc1float(indata, nalloc);
		}
		bcopy(tr.data, indata + (nxin - 1)*ntin, ntinbytes); 
	} while (gettr(&tr));


	/* Set dimensions for outdata  */	
	if (!igetpar("ntout", &ntout))		 ntout = ntin;
	if (!igetpar("nxout", &nxout))		 nxout = nxin;

	/* Pad output to power of 2 for sqrt(iw) filtering  */	
	for ( ntoutpad = 1; ntoutpad < ntout; ntoutpad *= 2 );

	/* Print some info to reassure the user  */
	warn("For HALEKDMO:    offset = %g     v = %g", offset, v);
	warn("       input:    nt = %d    nx = %d    dx = %g",
							ntin, nxin, dxin);
	warn("      output:    nt = %d    nx = %d    dx = %g",
							ntout, nxout, dxout);
	warn("   work area:    nt = %d    nx = %d", ntoutpad, nxin);

	/* Alloc space for outdata  */	
	ntoutbytes = ntout * sizeof (float); 
	outdata = ealloc1float(ntoutpad*nxout);

	/* (x,t) form of Hale's common offset DMO */
	fhaleksub(indata,outdata,&ntin,&dtin,&ntoutpad,&dtout,  
		   &nxin,&dxin,&nxout,&dxout,&h,&v);   

	/* Alloc memory for iomegasub work areas 	*/
	wrk1 = ealloc1float(ntoutpad*nxout);
	wrk2 = ealloc1float(2*ntoutpad*nxout);
	wrk3 = ealloc1float(ntoutpad);

	/* Subroutine to do sqrt(i*w) post processing on outdata */
	power = .5;
      	iomegasub(outdata,&ntoutpad,&dtout,&nxout,&power,wrk1,wrk2,wrk3); 

	/* Set trace headers for output data */
	tr.trid = TREAL;
	tr.ntr = nxout;
	tr.ns = ntout;
	tr.dt = dtout*1000000.;
	tr.offset = 0.;

	/* Output the result by pulling traces off data mega-vector  */
	for ( ix = 0 ; ix < nxout ; ix++ ) {
		bcopy( outdata + ix*ntoutpad, tr.data, ntoutbytes); 
		tr.tracl = ix + 1;	/* sequential traces    */
		tr.cdp = ix + 1;	/* sequential midpoints */
		puttr(&tr);
	}

	
	return EXIT_SUCCESS;
}
