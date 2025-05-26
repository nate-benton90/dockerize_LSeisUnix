/* SUDIVCOR: $Revision: 1.2 $ ; $Date: 91/04/25 16:35:25 $		*/

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

/*********************** self documentation ******************************/
string sdoc =
" 									\n"
" SUDIVCOR - Divergence (spreading) correction				\n"
" 									\n"
" sudivcor <stdin >stdout  [optional parms]				\n"
" 									\n"
" Required Parameters:							\n"
" none									\n"
" 									\n"
" Optional Parameters:							\n"
" trms=0.0	times corresponding to rms velocities in vrms		\n"
" vrms=1500.0	interval velocities corresponding to times in trms	\n"
" vfile=	binary (non-ascii) file containing velocities vrms(t)	\n"
" 									\n"
" Notes:								\n"
" The trms, vrms arrays specify an rms velocity function of time.	\n"
" Linear interpolation and constant extrapolation is used to determine	\n"
" interval velocities at times not specified.  Values specified in trms	\n"
" must increase monotonically.						\n"
" 									\n"
" Alternatively, rms velocities may be stored in a binary file		\n"
" containing one velocity for every time sample.  If vfile is specified,\n"
" then the trms and vrms arrays are ignored.				\n"
" 									\n"
" The time of the first sample is assumed to be constant, and is taken	\n"
" as the value of the first trace header field delrt. 			\n"
" 									\n"
" Trace header fields accessed:  ns, dt, delrt				\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Jack
 */


segy tr;

main(int argc, char **argv)
{
	int nt;			/* number of points on input trace	*/
	float dt;		/* sample spacing			*/
	float tmin;		/* time of first sample			*/
	float *vt;		/* velocity function			*/
	float *vrms;		/* rms velocity picks			*/
	float *trms;		/* times corresponding to vrms picks	*/
	char *vfile="";		/* binary file giving vrms(t)		*/
	float *divcor;		/* divergence correction function	*/


	/* Hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* Get information from the first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000000.0;
	tmin = tr.delrt/1000.0;


	/* Determine velocity function v(t) */
	vt = ealloc1float(nt);
	if (!getparstring("vfile", &vfile)) {
		int ntrms = countparval("trms");
		int nvrms = countparval("vrms");
		int npar;
		register int it, itrms;

		if (nvrms != ntrms)
			err("number of trms and vrms must be equal");

		if (!ntrms)  ntrms = 2;  /* default case */
		trms = ealloc1float(ntrms);
		if (!(npar = getparfloat("trms", trms))) {
			trms[0] = 0.0;
			trms[1] = dt;
		}
		if (npar == 1) trms[1] = trms[0] + dt; /* const vel case */
		
		if (!nvrms)  nvrms = 2;  /* default case */
		vrms = ealloc1float(nvrms);
		if (!(npar = getparfloat("vrms", vrms))) {
			vrms[0] = 1500.0;
			vrms[1] = 1500.0;
		}
		if (npar == 1) vrms[1] = vrms[0];  /* const vel case */

		for (itrms = 1; itrms < ntrms; ++itrms)
			if (trms[itrms] <= trms[itrms-1])
				err("trms must increase monotonically");

		for (it = 0; it < nt; ++it) {
			float t = tmin + it*dt;
			intlin(ntrms,trms,vrms,vrms[0],vrms[ntrms-1],
				1,&t,&vt[it]);
		}
	} else {  /* user gave a vfile */
		FILE *fp = efopen(vfile, "r");
		
		if (nt != efread(vt, FSIZE, nt, fp)) {
			err("cannot read %d velocities from file %s",
				nt, vfile);
		}
	}
	
	
	/* Form divergence correction vector */
	{ float denom = trms[1]*vrms[1]*vrms[1];
	  register int it;
	  
	  divcor = ealloc1float(nt);
	  for (it = 0; it < nt; ++it) {
	  	float t = tmin + it*dt;
		divcor[it] = t*vt[it]*vt[it] / denom;
	  }
	}
   
	  
	/* Main loop over traces */
	do {
		register int it;

	  	for (it = 0; it < nt; ++it)  tr.data[it] *=  divcor[it];
		puttr(&tr);
	} while (gettr(&tr));

	return EXIT_SUCCESS;
}
