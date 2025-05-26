/* SULOGDMO: $Revision: 1.3 $ ; $Date: 90/05/24 21:59:09 $	*/

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
SULOGDMO -- dmo/inv.dmo of log-stretched common offset data	\n\
								\n\
sulogdmo dx= [optional parameters] <stdin >stdout 		\n\
								\n\
Required parameters:						\n\
	dx= 		trace spacing on input data		\n\
			(always required)			\n\
	offset= 	offset in common offset section		\n\
		  	(required if dmosgn=-1)			\n\
								\n\
Optional parameters:						\n\
	dmosgn=1		forward dmo [-1 for inv.dmo]	\n\
	offset=(from header)	offset in common offset section	\n\
				(from header if dmosgn=1)	\n\
	dt= (from header) 	time sample rate		\n\
								\n\
NOTE:  dx and offset should be in consistent length units	\n\
								\n\
Example Processing Sequence:					\n\
								\n\
sulog outpar=logpar < data1 > data2 	'Log stretch'		\n\
sulogdmo dx=25 < data2 > data3		'Log stretch dmo'	\n\
suilog par=logpar < data3 > data4 	'Inverse Log stretch'	\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Chris
 *
 */


segy tr;

main(int argc, char **argv)
{
	float *data;		/* mega-vector to contain data set	*/
	float dt;		/* time sample rate			*/
	float dx;		/* trace spacing on input data  	*/
	float h;		/* offset/2 				*/
	float offset;		/* offset in common offset section	*/
	float *wrk1;		/* work vector for cdata in sub		*/
	float *wrk2;		/* work vector for idata in sub		*/
	float *wrk3;		/* work vector for wavenumbers in sub	*/
	float *wrk4;		/* work vector for frequencies in sub	*/
	int dmosgn;		/* +1=forward dmo; -1=inv.dmo		*/
	int ix;			/* trace counter		 	*/
	int nalloc;		/* allocation parameter			*/
	int nt;			/* time samples per trace		*/
	int ntbytes;		/* bytes per trace			*/
	int ntpad;		/* time samples padded to pow of 2	*/
	int ntpbytes;		/* bytes per padded trace		*/
	int nx;			/* traces in input data			*/
	int nxpad;		/* traces in output data (power of 2) 	*/

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	/* Read first trace & check than indata is time-domain */ 
	if ( !gettr(&tr) ) err("can't get first trace\n");
	if ( tr.trid != TREAL && tr.trid != 0 ) {  
		err("input is not t,x-domain data, trid=%d",tr.trid);
	}

	/*  get nt from header, pad to a power of 2 
	    then calc some constants  */	
	nt = tr.ns;	
	ntbytes = nt * FSIZE; 
	for ( ntpad = 1; ntpad < nt; ntpad *= 2 );	
	ntpbytes = ntpad * FSIZE; 


	/*  get dt and dmosgn  */	
	if (!fgetpar( "dt", &dt ))	 	dt = tr.dt;
	dt = dt/1000000.0;
	if (!igetpar( "dmosgn", &dmosgn ))	dmosgn = 1;

	/*  dx must be given by user; offset from header for forward
	 *  dmo, and must be given by user for inverse dmo  */	
	MUSTFGETPAR( "dx", &dx );
	if (dmosgn == 1) {
		if (!fgetpar( "offset", &offset ))	offset = tr.offset;
	} else {
		MUSTFGETPAR( "offset", &offset );
	}
	h = offset / 2.0;

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
		bcopy(tr.data, data + (nx - 1)*ntpad, ntpbytes); 
	} while (gettr(&tr));


	/* FFT to come later, so pad number of traces to power of 2 	
	   and allocate appropriate space  */
	if (!igetpar("nxpad", &nxpad)) { /* compute as next power of 2 */
		for (nxpad = 1; nxpad < nx; nxpad *= 2);
	}
	if ( nxpad*ntpad > nalloc ) { 
		data = erealloc1float(data, ntpad*nxpad); 
	}

	/* Print some info to reassure the user  */
	warn("    SULOGDMO:    dx    = %g      offset = %g", dx, offset);
	warn("input/output:    nt    = %d      nx     = %d", nt, nx);
	warn("   work area:    ntpad = %d      nxpad  = %d", ntpad, nxpad);

	/* Alloc memory for work areas 	*/
	wrk1 = ealloc1float(ntpad*nxpad);
	wrk2 = ealloc1float(2*ntpad*nxpad);
	wrk3 = ealloc1float(nxpad);
	wrk4 = ealloc1float(ntpad);

	/* Zero-out the padded traces  */
	if ( nxpad > nx ) bzero( data + nx*ntpad, (nxpad - nx)*ntpbytes ); 

	/* log-stretch dmo */
	lsdmosub(data,&ntpad,&dt,&nxpad,&dx,
			&h,&dmosgn,wrk1,wrk2,wrk3,wrk4); 

	/* set trace headers for output data */
	tr.trid = TREAL;
	tr.ntr = nx;
	tr.ns = nt;
	tr.dt = dt*1000000.;
	if ( dmosgn == 1 ) {
		tr.offset = 0.;
	} else {
		tr.offset = h ;
	}

	/* output the result by pulling traces off data mega-vector  */
	for( ix = 0; ix < nx; ix++ ) {
		bcopy( data + ix*ntpad, tr.data, ntbytes ); 
		tr.tracl = ix;
		tr.cdp = ix;
		puttr(&tr);
	}

	
	return EXIT_SUCCESS;
}
