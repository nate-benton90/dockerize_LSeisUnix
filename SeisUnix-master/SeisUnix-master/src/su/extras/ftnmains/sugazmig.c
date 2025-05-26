/* SUGAZMIG: $Revision: 1.3 $ ; $Date: 90/05/23 14:02:57 $	*/

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
SUGAZMIG -- constant vel migration of zero-offset data		\n\
								\n\
sugazmig dx= v= [optional parameters] < stdin > stdout 		\n\
								\n\
Required parameters						\n\
	dx= trace spacing on input data 			\n\
	v=  migration velocity (constant)			\n\
								\n\
Optional parameters						\n\
	dt= (from header) 	time sample rate (in seconds)	\n\
	ntau= (nt from header)	number of out samples		\n\
								\n\
Note:  	Execution time is proportional to ntau; ntau < nt will  \n\
	be faster, but resolution will be degraded 		\n\
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
	float *data;		/* mega-vector of input data		*/
	float *outdata;		/* mega-vector of migrated data		*/
	float dt;		/* time sample rate			*/
	float dx;		/* trace spacing on input data  	*/
	float v;		/* migration velocity		  	*/
	float *wrk1;		/* work area vector 			*/
	float *wrk2;		/* work area vector 			*/
	float *wrk4;		/* work area vector			*/
	float *wrk5;		/* work area vector 			*/
	float *wrk6;		/* work area vector 			*/
	int ix;			/* counter			 	*/
	int nalloc;		/* allocation parameter			*/
	int ntau;		/* output 'time' samples		*/
	int ntaubytes;		/* ... in bytes				*/
	int nt;			/* time samples per trace in input data	*/
	int ntpad;		/* time samples to gazmigsub (pow of 2) */
	int ntpadbytes;		/* ... in bytes				*/
	int nx;			/* traces in input data			*/
	int nxpad;		/* traces in output data (power of 2) 	*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/*  read first trace & check that indata is time-domain */ 
	if (!gettr(&tr)) err("can't get first trace\n");
	if (tr.trid != TREAL && tr.trid != FPACK && tr.trid != 0) {  
		err("input is not x-domain data, trid=%d",tr.trid);
	}
	if (tr.trid == FPACK) {
		err("not ready for fpack, trid=%d",tr.trid);
	}

	/*  get nt from header, pad to a power of 2 */
	nt = tr.ns;	
	for (ntpad = 1; ntpad < nt; ntpad *= 2);
	ntpadbytes = ntpad * FSIZE;

	/* get dt from header or user */
	if (!fgetpar("dt", &dt))	dt = tr.dt/1000000.;

	/* set number of out samples or get from user */
	if (!igetpar("ntau", &ntau))	ntau = nt;
	if (ntau < 0) err("ntau=%d :should be positive integer", ntau);
	ntaubytes = ntau * FSIZE;

	/*  dx and v must be given by user */ 
	MUSTFGETPAR("dx", &dx);
	MUSTFGETPAR("v", &v);
	if (dx <= 0) err("dx=%d :should be > 0", dx);
	if (v < 0) err("v=%d :should be positive or zero", v);

	/* Alloc block of memory for data 	*/
	nalloc = MAX(NFALLOC, ntpad); 
	data = ealloc1float(nalloc);


	/* Loop over input traces & put them into data mega-vector */
	nx = 0;
	do {
		++nx;
		if (nx*ntpad > nalloc) { /* need more memory */	
			nalloc += NFALLOC;
			data = erealloc1float(data, nalloc);
		}
		bcopy(tr.data, data + (nx - 1)*ntpad, ntpadbytes); 
	} while (gettr(&tr));


	/* FFTs to come later, so pad number of traces 
	   to power of 2 and allocate appropriate space  */
	for (nxpad = 1; nxpad < nx; nxpad *= 2);
	if (nxpad*ntpad > nalloc) { 
	    data = erealloc1float(data, ntpad*nxpad); 
	}

	/* Print some info for the user */
	warn(" SUGAZMIG: 	dx   = %g     v  = %g", dx, v);
	warn("    input: 	nt   = %d     nx = %d", nt, nx);
	warn("   output: 	ntau = %d     nx = %d", ntau,nx);
	warn("work area: 	ntpad = %d    nxpad = %d", ntpad, nxpad);

	/* Alloc memory for outdata and work areas 	*/
	outdata = ealloc1float(ntau*nxpad);
	wrk1 	= ealloc1float(ntpad*nxpad);
	wrk2 	= ealloc1float(2*ntpad*nxpad);
	wrk4 	= ealloc1float(2*ntau*nxpad);
	wrk5 	= ealloc1float(nxpad);
	wrk6 	= ealloc1float(ntpad);

	/*  zero-out the padded traces  */
	if (nxpad > nx) bzero(data + nx*ntpad, (nxpad-nx)*ntpadbytes); 

	/* gazdag migration (const. vel) */
	gazmigsub(data, outdata, &ntpad, &dt, &nxpad, &dx, &v, &ntau,      
		   wrk1, wrk2, wrk4, wrk5, wrk6);      

	/* set constant header words */
	tr.trid = TREAL;
	tr.ntr = nx;
	tr.ns = ntau;
	tr.dt = dt*1000000.0;

	/* output the result by pulling traces
	 * off data mega-vector  */
	for (ix = 0; ix < nx; ix++) {
		bcopy(outdata + ix*ntau, tr.data, ntaubytes); 
		tr.tracl = ix + 1;
		tr.cdp = ix;
		puttr(&tr);
	}

	
	return EXIT_SUCCESS;
}
