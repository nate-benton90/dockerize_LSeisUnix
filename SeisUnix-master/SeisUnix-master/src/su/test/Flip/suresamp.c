/* SURESAMP: $Revision: 1.3 $ ; $Date: 90/11/15 10:43:44 $	*/

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
String sdoc = "\
								\n\
SURESAMP - Resample in time					\n\
								\n\
suresamp <stdin >stdout  [optional parameters]		 	\n\
								\n\
Required parameters:						\n\
	none							\n\
								\n\
Optional Parameters:						\n\
	nt=tr.ns		number of time samples on output\n\
	dt=tr.dt/10^6		time sampling interval on output\n\
	tmin=tr.delrt/10^3	first time sample on output	\n\
								\n\
NOTE:	Any anti-alias filtering required must be done before	\n\
	resampling.						\n\
								\n\
";
/************************ end self doc ***************************/

/* Credits:
 *	CWP: Dave (resamp algorithm), Jack (SU adaptation)
 */


segy intrace, outtrace;

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
	askdoc(1);

	/* Get information from first trace */
	if (!gettr(&intrace)) err("can't get first trace");
	nt_in = intrace.ns;
	tmin_in =intrace.delrt/1000.0;
	dt_in =intrace.dt/1000000.0;
	
	/* Get parameters */
	if (!getparint("nt", &nt))		nt = nt_in;
	if (!getparfloat("tmin", &tmin))	tmin = tmin_in;
	if (!getparfloat("dt", &dt))		dt = dt_in;

	idt = (int) dt * 1000000.0;
	itmin = (int) tmin * 1000.0;

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
		memcpy(&outtrace, &intrace, HDRBYTES);
		outtrace.ns = nt;
		outtrace.dt = idt;
		outtrace.delrt = itmin;
		
		/* sinc interpolate new data */
		ints8r(nt_in,dt_in,tmin_in,intrace.data,
				0.0,0.0,nt,t,outtrace.data);
		
		puttr(&outtrace);
	} while (gettr(&intrace));


	return EXIT_SUCCESS;
}
