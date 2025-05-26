/* SUSTATIC: $Revision: 1.2 $ ; $Date: 92/10/22 16:27:10 $	*/

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

/*********************** self documentation ******************************/
char *sdoc[] = {
"									",
" SUSTATIC - Elevation static corrections				",
"									",
" sustatic <stdin >stdout  [optional parameters]	 		",
"									",
" Required parameters:							",
"	none								",
"									",
" Optional Parameters:							",
"	v0=from header 	   weathering velocity				",
"	v1=from header 	   subweathering velocity			",
"									",
" input field sut is assumed measured in ms.				",
" output field sstat = scalel(selev - sdel - sdepth)/swevel.		",
" output field gstat = sstat + sut/1000.				",
" output field tstat = sstat + gstat + scalel(gelev - selev)/wevel.	",
"									",
" Trace header fields accessed:  ns, dt, delrt, gelev, selev, 		",
"	sdepth, gdel, sdel, wevel, swevel, sut, scalel 			",
" Trace header fields modified:  sstat, gstat, tstat			",
NULL};
/************************ end self doc ***********************************/

/* Credits:
 *	CWP: Jamie Burns
 */


segy intrace, outtrace;

main(int argc, char **argv)
{
	int nt;		/* number of samples on output trace	*/
	float dt;	/* sample rate on output trace		*/
	int idt;	/* ... as integer			*/
	float tmin;	/* first time sample on output trace	*/
	float tsd;	/* time to move source to datum         */
	float trd;	/* time to move 0 offset receiver       */
	float tstatic;	/* time to move both 0 offset & source  */
	float v0;	/* weathering velocity			*/
	float v1;	/* subweathering velocity		*/
	float *t;	/* array of output times		*/
	
	/* Hook up getpar */
	initargs(argc, argv);
	requestdoc(1);

	/* Get information from first trace */
	if (!gettr(&intrace)) err("can't get first trace");
	nt   = intrace.ns;
	tmin = intrace.delrt/1000.0;
	dt   = (float)intrace.dt/1000000.0;
	
	/* Get parameters */
	if (!getparfloat("v0", &v0))		v0 = (float) intrace.wevel;
	if (!getparfloat("v1", &v1))            v1 = (float) intrace.swevel;

	/* Allocate vector of output times */
	t = ealloc1float(nt);


	/* Loop on traces */	
	do {
		int temp = intrace.scalel;
		int scale;
		if (temp > 0)      scale = temp;
		else if (temp < 0) scale = -1/temp;
		else               scale = 1;
		
		/* copy and adjust header */
		memcpy((char*)&outtrace, (char*)&intrace, HDRBYTES);
	
		/* compute static correction */
		tsd = scale *
			(-intrace.selev + intrace.sdel + intrace.sdepth)/v1;
		trd = tsd - intrace.sut/1000.0;
		tstatic = tsd + trd +
			scale * (intrace.gelev - intrace.selev)/v0;
		
		/* Compute output times */
		{ int itime;
			for (itime=0; itime<nt; ++itime)
			t[itime] = tmin + itime*dt + tstatic;
		}

		/* sinc interpolate new data */
		ints8r(nt, dt, tmin, intrace.data, 
				0.0, 0.0, nt, t, outtrace.data);
		
		/* set header field for output trace */
		outtrace.sstat = 1000.0 * tsd;
		outtrace.gstat = 1000.0 * trd;
		outtrace.tstat = 1000.0 * tstatic;
		
		puttr(&outtrace);
	} while (gettr(&intrace));


	return EXIT_SUCCESS;
}
