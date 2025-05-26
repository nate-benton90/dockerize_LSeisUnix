/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */

/* SUNORMALIZE: $Revision: 1.1 $ ; $Date: 1998/08/07 22:39:31 $	*/

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 	   							",
" SUNNRNORM - Limit surface noise by trace dependent scaling	", 
" 	   							",
"   susnnorm    <stdin >stdout     t1=0.2 s tr=0.1 v=2000	",
" 								",
" Required parameters:						",
"	dt=tr.dt	if not set in header, dt is mandatory	",
"	ns=tr.ns	if not set in header, ns is mandatory	",
"								",
" Optional parameters:						",
"	tr=0.1		Length of window			",
"	t1=0.2		Ending time for Window at zero offset	",	
"	v=2000          Velocity of the window			",
"								",
NULL};

/*
 * Trace header fields accessed: ns, dt
 * Trace header fields modified: none
 */
/**************** end self doc ***********************************/

segy tr;

int
main(int argc, char **argv)
{
	int ns;			/* number of samples		*/
	int it0;		/* first sample of time window	*/
	int it1;		/* last sample of time window	*/

	int n;			/* size of temporary arrays 	*/

	float dt;		/* time sampling interval	*/
	float ttr;		/* first time of time window	*/
	float t1;		/* ending time of time window	*/
	float v;


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

	/* Get parameters */
	if (!fgettr(stdin, &tr)) err("can't read first trace");
	if (!tr.dt) MUSTGETPARFLOAT("dt", &dt);
	if (!tr.ns) MUSTGETPARINT("ns", &ns);
	ns = (int) tr.ns;
	dt = ((double) tr.dt)/1000000.0;	
	if (!getparfloat("tr", &ttr))	 ttr=0.1;
	if (!getparfloat("t1", &t1))	 t1=0.2;
	if (!getparfloat("v", &v))	 v=2000;


	/* Loop over traces */
	do {
		/* Define integerized times */
        	n=NINT(ttr/dt); 
        	it1=MIN(NINT(t1/dt+tr.offset/v/dt),tr.ns);
		it0=MAX(it1-n,0);

		/* scale in window */
		{ int it,in;
			for(it=0;it<it0;it++) tr.data[it]=0.0;
			for(it=it0,in=0;it<it1;it++,in++) {
				tr.data[it] *=((float)in/(float)n);
			}
		}
		
		
		puttr(&tr);
	} while (gettr(&tr));
        	
	return EXIT_SUCCESS;
}


