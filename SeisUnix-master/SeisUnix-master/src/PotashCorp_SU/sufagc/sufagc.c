/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */

/* SUNORMALIZE: $Revision: 1.1 $ ; $Date: 1998/08/07 22:39:31 $	*/

#include "su.h"
#include "segy.h"          
#include "suhdr.h"


/*********************** self documentation **********************/
char *sdoc[] = {
" 	   							",
" SUFAGC - Gain trace by the filtered trace derived agc scaler	", 
" 	   							",
"   sunormalize <stdin >stdout t0=0 t1=TMAX norm=rms		",
" 								",
" Required parameters:						",
"	dt=tr.dt	if not set in header, dt is mandatory	",
"	ns=tr.ns	if not set in header, ns is mandatory	",
"								",
" agc=0.1		length of agc window in s              	",
"								",
" f=f1,f2,f3,f4		bandpass filter frequencies		",
"			If these are specified, the scaling	",
"			factor is computed on the filtered 	",
"			trace but applied to the unfiltered one.",
"								",
NULL};

/*
 *
 * Trace header fields accessed: ns, dt
 * Trace header fields modified: none
 */
/**************** end self doc ***********************************/

segy tr;

int
main(int argc, char **argv)
{
	/* Filter */
	float *f;
	
	int iagc;
	float agc;
	float dt;
	int ns;
	
	float *scale;
	float *z;

	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

	/* Get parameters */
	if (!getparfloat("agc", &agc))	 agc=0.1;
	if (!fgettr(stdin, &tr)) err("can't read first trace");
	if (!tr.dt) MUSTGETPARFLOAT("dt", &dt);
	if (!tr.ns) MUSTGETPARINT("ns", &ns);
	ns = (int) tr.ns;
	dt = ((double) tr.dt)/1000000.0;	

	/* Define integerized times */
	iagc = NINT(agc/dt);

	/* Allocate space for temporary arrays */
        scale=ealloc1float(ns);
        z= ealloc1float(ns);

        
	/* Get frequencies that define the filter */
        { int np=4;
		if ((np = countparval("f"))!=0) {
                	f = ealloc1float(np);
                	getparfloat("f",f);
        	} else {
			f=NULL;
        	}
	}


	/* Loop over traces */
	do {
                memcpy((void *) z,(const void *) &tr.data[0],ns*sizeof(float));
		
		/* Filter if requested */
		if(f!=NULL) {
			bp_filter_padd(f[0],f[1],f[2],f[3],z,((double) tr.dt)/1000000.0,ns,1.5);
		} 
		
		do_agc_scale(z,iagc,scale,ns);
		
		{ int i;
			for(i=0;i<ns;i++) tr.data[i] /=scale[i];
		}
		
		puttr(&tr);
	} while (gettr(&tr));
        	
	return EXIT_SUCCESS;
}


