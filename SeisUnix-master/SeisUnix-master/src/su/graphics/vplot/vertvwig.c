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

static bool first = true;       /* First entry flag */


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
