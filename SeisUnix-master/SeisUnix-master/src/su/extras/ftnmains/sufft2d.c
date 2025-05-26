/* SUFFT2D: $Revision: 1.3 $ ; $Date: 90/05/23 10:43:54 $	*/

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
SUFFT2D -- transform a (t,x) panel of data to (freq,wavenumber)	\n\
								\n\
sufft2d [optional parameters] < stdin > stdout 			\n\
								\n\
Optional parameters:						\n\
	dx = if needed	trace spacing on input data 		\n\
	mode=amp	output flag		 		\n\
	       		amp   = output amplitude traces		\n\
	       		phase = output phase traces		\n\
	       		real  = output real parts		\n\
	       		imag  = output imag parts 		\n\
								\n\
	out=0 	only output FREQUENCIES		 		\n\
			from 0 to +nyquist			\n\
			= 1 output all FREQUENCIES from		\n\
			    -nyquist --> + nyquist		\n\
								\n\
NOTES:								\n\
       	1. nt and nx padded to power of 2 for ffts		\n\
	2. output is ordered for human viewing: wavenumbers 	\n\
	   and frequencies from -nyq --> +nyq (see out parameter)\n\
								\n\
Example processing sequence:					\n\
	sufft2d <DATA | sucontour | tube			\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Chris
 *
 * Caveats:
 *	1. Currently set up to output data for human display only
 *	   later will want to output in form for fk filter, etc.
 *	2. trid set to REALPART, etc. in conflict with definitions in
 *	   su.h that specifies packed nyquist, etc.
 *	3. dx will need to be a required parameter when code
 *	   is modified to allow output to filter progs.  dx will
 *	   be used to calc min wavenumber and increment for outpar.
 */


#define	REAL	1
#define	IMAG	2
#define	AMP	3
#define	ARG	4


segy tr;

main(int argc, char **argv)
{
	float *cdata;		/* work area vector 			*/
	float *ctemp;		/* work area vector 			*/
	float df;		/* frequency increment			*/
	float dk;		/* wavenumber increment			*/
	float dt;		/* time sample rate			*/
	float dx;		/* trace spacing on input data  	*/
	float fmin;		/* minimum frequency			*/
	float *idata;		/* imaginary part of output		*/
	float im;		/* temp variable for imaginary parts	*/
	float kmin;		/* minimum wavenumber			*/
	float *rdata;		/* input data AND real part of output	*/
	float re;		/* temp variable for real parts		*/
	int i;			/* counter			 	*/
	int ix;			/* counter			 	*/
	int mode;		/* flag for type of output	 	*/
	int nt;			/* time samples per trace in input rdata*/
	int ntpad;		/* time samples padded to power of 2 	*/
	int ntpbytes;  		/* bytes per padded trace		*/
	int nx;			/* traces in input rdata		*/
	int nxpad;		/* output traces (power of 2) 		*/
	int out;		/* flag to output all or half of freqs	*/
	int outns;		/* output samples per trace		*/
	int nalloc;		/* allocation parameter			*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/*  read first trace & check than indata is time-domain */
	if ( !gettr(&tr) ) err("can't get first trace\n");
	if ( tr.trid != TREAL && tr.trid != 0 ) {
		err("input is not (t,x)-domain data, trid=%d", tr.trid);
	}

	/*  get nt from header, pad to a power of 2
	    then calc some constants  */	
	nt = tr.ns;	
	for (ntpad = 1; ntpad < nt; ntpad *= 2);	
	ntpbytes = ntpad * FSIZE;

	/* get dt from header or user */
	dt = tr.dt/1000000.0;

	/* Get mode and out parameters */
	if (!igetpar("mode", &mode))	mode = AMP;
	if (!igetpar("out",  &out))	out = 0;
	if (out != 0 && out != 1) err("out=%d: should be 0 or 1", out);

	/* Alloc block of memory for rdata 	*/
	nalloc = MAX(NFALLOC, ntpad);
	rdata = ealloc1float(nalloc);


	/* Loop over input traces & put them into rdata mega-vector */
	nx = 0;
	do {
		++nx;
		if (nx*ntpad > nalloc) { /* need more memory */	
			nalloc += NFALLOC;
			rdata = erealloc1float(rdata, nalloc);
		}
		bcopy(tr.data, rdata + (nx - 1)*ntpad, ntpbytes); 
	} while (gettr(&tr));


	/* FFTs to come later, so pad number of traces
	   to power of 2 and allocate appropriate space  */
	if (!igetpar("nxpad", &nxpad)) {
		for (nxpad = 1; nxpad < nx; nxpad *= 2);
	}
	if (nxpad*ntpad > nalloc) {
	    rdata = erealloc1float(rdata, ntpad*nxpad);
	}

	/* check if output is to be half or all of freqs */
	outns = ntpad/2;
	if (out == 1) outns = ntpad;

	/*  print some info for the user */
	warn("input: 	nt = %d     nx = %d",nt,nx);
	warn("output: 	nf = %d     nk = %d",outns,nxpad);
	if (out == 0) {
		fmin = 0;
	} else {
		fmin = - 1.0 / (2.0 * dt);
	}
	df = 1.0 / (nt * dt);
	warn("min frequency  = %g     df = %g --  in Hertz", fmin, df );
	if (fgetpar("dx", &dx)) {
	  kmin = - 1.0 / (2.0 * dx);
	  dk = 1.0 / (nx * dx);
	  warn("min k = %g     dk = %g --  in cycles/unit dist", kmin, dk);
	}

	/*  alloc memory for outdata and work areas 	*/
	idata = ealloc1float(ntpad*nxpad);
	cdata = ealloc1float(2*ntpad*nxpad);
	ctemp = ealloc1float(2*ntpad*nxpad);

	/*  zero-out the padded traces  */
	if (nxpad > nx)
		bzero(rdata + nx*ntpad, (nxpad - nx)*ntpbytes);

	/* 2dfft of rdata .. returned rdata is real part, etc. */
	twodfftsub(rdata, idata, cdata, ctemp, &ntpad, &nxpad);

	/* Compute the desired type of output */
	switch(mode) {
	case REAL:
		for ( i = 0; i <= ntpad*nxpad; i++ ) {
			ctemp[i] = rdata[i];
		}
		tr.trid = REALPART;
	break;
	case IMAG:
		for ( i = 0; i <= ntpad*nxpad; i++ ) {
			ctemp[i] = idata[i];
		}
		tr.trid = IMAGPART;
	break;
	case AMP:
		for ( i = 0; i < ntpad*nxpad; i++ ) {
			re = rdata[i];
			im = idata[i];
			ctemp[i] = sqrt( re*re + im*im );
		}
		tr.trid = AMPLITUDE;
	break;
	case ARG:
		for ( i = 0; i <= ntpad*nxpad; i++ ) {
			ctemp[i] = atan2( idata[i], rdata[i] );
		}
		tr.trid = PHASE;
	break;
	default:
		err("%d: mysterious mode=\"%d\"", __LINE__, mode);
	}

	/* set trace headers for output data */
	tr.trid = KOMEGA;
	tr.ntr = nxpad;
	tr.ns = outns;
	tr.dt = dt*1000000.0;

	/* output the result from ctemp buffer */
	for (ix = 0; ix < nxpad; ix++) {
		if (out == 0) {
			bcopy(ctemp + ix*ntpad + outns, tr.data,
						(int) (outns*FSIZE));
		} else {
			bcopy(ctemp + ix*ntpad, tr.data, ntpbytes);
		}
		tr.tracl = ix;
		puttr(&tr);
	}


	return EXIT_SUCCESS;
}
