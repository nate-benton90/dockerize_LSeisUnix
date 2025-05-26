/* SURESAMP: $Revision: 1.3 $ ; $Date: 92/10/22 16:50:49 $	*/

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
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"								",
" SURESAMP - Resample in time					",
"								",
" suresamp <stdin >stdout  [optional parameters]		",
"								",
" Required parameters:						",
"	none							",
"								",
" Optional Parameters:						",
"	nt=tr.ns	    number of time samples on output	",
"	dt=tr.dt/10^6	    time sampling interval on output	",
"	tmin=tr.delrt/10^3  first time sample on output		",
"								",
" Example 1: (assume original data had dt=.004 nt=256)		",
" 	sufilter <data f=40,50 amps=1.,0. | 			",
" 	suresamp nt=128 dt=.008 | ...		 		",
" Note the typical anti-alias filtering before sub-sampling.	",
" Example 2: (assume original data had dt=.004 nt=256)		",
" 	suresamp <data nt=512 dt=.002 | ...	 		",
"								",
" Trace header fields accessed:  ns, dt, delrt			",
" Trace header fields modified:  ns, dt, delrt			",
NULL};
/************************ end self doc ***************************/

/* Credits:
 *	CWP: Dave (resamp algorithm), Jack (SU adaptation)
 */


segy intrace, outtrace;
segychdr ch;
segybhdr bh;


main(int argc, char **argv)
{
	int nt;		/* number of samples on output trace	*/
	int nt_in;	/* ... on input trace			*/
	float dt;	/* sample rate on output trace		*/
	int idt;	/* ... as integer			*/
	float dt_in;	/* ... on input trace			*/
	float tmin;	/* first time sample on output trace	*/
	int itmin;	/* ... as integer			*/
	float tmin_in;	/* ... on input trace			*/
	float *t;	/* array of output times		*/
	
	/* Hook up getpar */
	initargs(argc, argv);
	requestdoc(1);

	/* Get header */
	gethdr(&ch,&bh); 

	/* Get information from first trace */
	if (!gettr(&intrace)) err("can't get first trace");
	nt_in   = intrace.ns;
	tmin_in = intrace.delrt/1000.0;
	dt_in   = (float) intrace.dt/1000000.0;
	
	/* Get parameters */
	if (!getparint("nt", &nt))		nt = nt_in;
	if (!getparfloat("tmin", &tmin))	tmin = tmin_in;
	if (!getparfloat("dt", &dt))		dt = dt_in;

	idt = dt * 1000000.0;
	itmin = tmin * 1000.0;

	/* modified header if needed */
	if(bh.hns!=nt) bh.hns = nt;
	if(bh.hdt!=idt) bh.hdt = idt;
	puthdr(&ch,&bh);

	
	/* Allocate vector of output times */
	t = ealloc1float(nt);

	/* Compute output times */
	{ register int itime;
	  register float tvalue;
	  for (itime=0,tvalue=tmin; itime<nt; itime++,tvalue+=dt)
		t[itime] = tvalue;
	}


	/* Loop on traces */	
	do {
		
		/* copy and adjust header */
		memcpy((char*)&outtrace, (char*)&intrace, HDRBYTES);
		outtrace.ns    = nt;
		outtrace.dt    = idt;
		outtrace.delrt = itmin;
		
		/* sinc interpolate new data */
		ints8r(nt_in, dt_in, tmin_in, intrace.data, 
				0.0, 0.0, nt, t, outtrace.data);
		
		puttr(&outtrace);
	} while (gettr(&intrace));


	return EXIT_SUCCESS;
}
