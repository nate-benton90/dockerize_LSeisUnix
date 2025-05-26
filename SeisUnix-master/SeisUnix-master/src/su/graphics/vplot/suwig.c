/* SUWIG: $Revision: 1.5 $ ; $Date: 90/11/10 13:21:40 $	 */

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
#include "vplot.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" SUWIG - no-frills wiggle trace program for data plotting		",
" 									",
" suwig <stdin [optional parameters] | pen				",
" 									",
" Optional Parameters:							",
" 	fill    = 0	     flag for positive fill (=1 for fill)	",
" 	sizet   = 6.0	   length of t-axis (inches)	     		",
" 	sizex   = 4.7	   length of x-axis (inches)			",
" 	zerot   = 1.0	   base of plot to bottom of screen		",
" 	zerox   = 0.6	   left of plot to left of screen		",
" 	plotfat = 0	     line thickness of traces			",
" 	overlap = 2.0	   max deflection (in traces) is		",
" 				overlap*scale				",
" 									",
NULL};

/* Credits:
 *      SEP: Shuki
 *      CWP: Chris, Jack
 *
 */
/**************** end self doc *******************************************/


/* Set plotting defaults */
#define  FILL	   0
#define  OVERLAP	2.0
#define  ZEROT	  1.0
#define  ZEROX	  0.6
#define  SIZET	  6.0
#define  SIZEX	  4.7
#define  PLOTFAT	0

/* prototypes */
void wigplot(float *dataptr, int nt, int ntr); 
void vertvwig(float *data, int n, int fill);

segy tr;

main(int argc, char **argv)
{
	float *dataptr;	 /* mega-vector of data from the segys   */
	float dt;	       /* sample rate			  */
	float tmin;	     /* first time on trace		  */
	int nt;		 /* time samples per trace (from tr.ns)  */
	int ntsize;	     /* number of data bytes on a trace      */
	int ntr;		/* traces in input data (from gettr)    */
	int ndata;	      /* floats allocated for mega-vector     */



	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Prevent bytes from spilling onto screen */
	if (isatty(STDOUT)) {
		err("must redirect or pipe byte code output");
	}


	/* Get info from first trace    */ 
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	ntsize = nt * FSIZE;


	/* Allocate block of memory for data float mega-vector */
	ndata = MAX(NFALLOC, nt); /* alloc at least one trace */
	dataptr = ealloc1float(ndata);


	/* Loop over input traces & put them into data mega-vector */
	ntr = 0;
	do {
		++ntr;
		if (ntr*nt > ndata) {   /* need more memory */
			ndata += NFALLOC;
			dataptr = erealloc1float(dataptr, ndata);
		}
		bcopy(tr.data, dataptr + (ntr - 1)*nt, ntsize); 
	} while (gettr(&tr));


	/* Gain */
	tmin = tr.delrt/1000.0;
	dt = tr.dt/1000000.0;   if (!dt) dt=0.004;

	/* Plot */
	warn("nt = %d  ntr = %d", nt, ntr);     
	wigplot(dataptr, nt, ntr);

	vp_endplot ();


	return EXIT_SUCCESS;
}


/* Wiggle plot subroutine for vertical plotting */
void wigplot(float *dataptr, int nt, int ntr)
{
	float scalet;	   /* time axis scale		      */
	float scalex;	   /* trace axis scale		     */
	float sizet;	    /* length of t-axis (inches)	    */
	float sizex;	    /* length of x-axis (inches)	    */
	float zerot;	    /* base of plot to bot. of screen       */
	float zerox;	    /* left of plot to left of screen       */
	int fill;	       /* fill flag			    */
	float overlap;	  /* maximum trace overlap		*/
	int plotfat;	    /* line thickness of traces	     */
	register int i;	 /* counter			      */


	/* Get parameters */
	if (!getparint("fill", &fill))	    fill = FILL;

	if (!getparfloat("overlap", &overlap))      overlap = OVERLAP;

	if (!getparfloat("zerot", &zerot))	  zerot = ZEROT;
	if (!getparfloat("zerox", &zerox))	  zerox = ZEROX;

	if (!getparfloat("sizet", &sizet))	  sizet = SIZET;
	if (!getparfloat("sizex", &sizex))	  sizex = SIZEX;

	scalet = -sizet/nt;
	scalex = sizex/MAX(ntr, 8);

	if (!getparint("plotfat", &plotfat))      plotfat = PLOTFAT;

	vp_scale (scalex, scalet);
	vp_orig (zerox, zerot + sizet);

	/* Draw wiggle traces */
	vp_color (RED);  
	vp_fat (plotfat);
	vp_scale (scalex*overlap, scalet);
	for (i = 0; i < ntr; i++) {
		vp_uorig (-(float) i / overlap, 0.0);
		vertvwig(dataptr + nt*i, nt, fill);
	}
}
/* VERTVWIG: $Revision: 1.2 $ ; $Date: 90/05/25 19:41:47 $       */

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
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
#include "vplot.h"

/* vertvwig - convert a trace to a vertical wiggle plot
 *
 * Returns:
 *      void
 *
 * Synopsis:
 *      void vertvwig(data, n, fill)
 *      float *data;
 *      int n, fill;
 *
 * Credits:
 *      CWP: Shuki, Jack
 *
 */


#define WEPS    0.001           /* For efficiency, don't fill 0 < datum <WEPS */
#define FILLCOLOR	RED

static cwp_Bool first = true;       /* First entry flag */


void vertvwig(float *data, int n, int fill)
{
	int lpoly = 0;          /* Number of points in fill arrays */
	static float *xp, *yp;  /* Fill corner arrays              */
	register int i;         /* Counter for data points         */


	/* Allocate fill corner arrays if first entry.                */
	/* We keep the x, y values of the polygon to be filled in two */
	/* separate arrays.  The index lpoly points to the last corner.  */

	if (first) {
		xp = ealloc1float(n + 2);
		yp = ealloc1float(n + 2);
		vp_color(FILLCOLOR);
		first = false;
	}
	

	/* Initialize pen to top of trace */
	vp_umove(data[0], 0.0);


	/* Start fill arrays if first two x's are significantly positive. */
	/* This needs to be handled specially since we cannot detect the  */
	/* begin of fill by a previous non-fill point in this case.       */

	if (data[0] > WEPS) {

		/* Phoney first corner point to tie down the polygon */
		yp[0] = 0.0;    xp[0] = WEPS;

		/* Record the actual value */
		yp[1] = 0.0;    xp[1] = data[0];
		lpoly = 2;
	}
	

	if (fill) {

		/* Loop over interior data points */
		for (i = 1; i < n - 1; ++i) {
			vp_udraw(data[i], (float) i);


			/* If significantly > 0, add to the fill arrays  */
			if (data[i] > WEPS) {

				/* If previous point was not a fill corner, */
				/* space up proportionately to start fill   */
				/* and initialize fill arrays               */
				if (!lpoly) {
					yp[0] = i-data[i]/(data[i]-data[i-1]);
					xp[0] = WEPS;
					lpoly = 1;
				}

				/* Add this fill corner to the previous ones */
				yp[lpoly] = i;
				xp[lpoly] = data[i];
				++lpoly;
			}

			/* At end of fill region call uarea to do the fill */
			if (data[i] < WEPS && lpoly) {
				yp[lpoly] = i - data[i]/(data[i]-data[i-1]);
				xp[lpoly] = WEPS;
				++lpoly;
				if (lpoly > 2) vp_uarea(xp, yp, lpoly, 1, 1, 1);
				lpoly = 0;
			}


		} /* End of loop over interior points on trace */
		
	} else {

		/* Not filling, just connect the dots */
		for (i = 1; i < n - 1; ++i) {
			vp_udraw(data[i], (float) i);
		}
		
	} /* End if fill */


	/* Draw to last x, y on trace */
	vp_udraw(data[n-1], (float) n-1);


	/* Force out fill arrays at end of trace. */
	/* Tie down end of polygon.               */
	if (fill && lpoly) {
		yp[lpoly] = n - 1;
		xp[lpoly] = WEPS;
		++lpoly;
		if (lpoly > 2) vp_uarea(xp, yp, lpoly, 1, 1, 1);
	}
}
