/* SUSTKVEL: $Revision: 1.5 $ ; $Date: 90/11/27 20:17:58 $		*/

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

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUSTKVEL - convert constant dip layer interval velocity model to the	\n\
	   stacking velocity model required by sunmo			\n\
									\n\
sustkvel v= h= dip=0.0 outpar=/dev/tty					\n\
									\n\
Required parameters:					        	\n\
	v=	interval velocities 					\n\
	h=	layer thicknesses at the cmp	 			\n\
									\n\
Optional parameters:							\n\
	dip=0.0			(constant) dip of the layers (degrees)	\n\
	outpar=/dev/tty		output parameter file in the form	\n\
				required by sunmo:			\n\
				tv=zero incidence time pick vector	\n\
				v=stacking velocities vector		\n\
									\n\
Examples:								\n\
    sustkvel v=5000,6000,8000,10000 h=1000,1200,1300,1500 outpar=stkpar	\n\
    sunmo <data.cdp par=stkpar >data.nmo				\n\
									\n\
    sustkvel par=intpar outpar=stkpar					\n\
    sunmo <data.cdp par=stkpar >data.nmo				\n\
									\n\
If the file, intpar, contains:						\n\
    v=5000,6000,8000,10000						\n\
    h=1000,1200,1300,1500						\n\
then the two examples are equivalent.  The created parameter file,	\n\
stkpar, is in the form of the velocity model required by sunmo.		\n\
									\n\
Note: sustkvel does not have standard su syntax since it does not	\n\
      operate on seismic data.  Hence stdin and stdout are not used.	\n\
									\n\
Caveat: Does not accept a series of interval velocity models to		\n\
	produce a variable velocity file for sunmo.			\n\
									\n\
";
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Jack 
 *
 * Technical Reference:
 *	The Common Depth Point Stack
 *	William A. Schneider
 *	Proc. IEEE, v. 72, n. 10, p. 1238-1254
 *	1984
 *
 * Formulas:
 *    	Note: All sums on i are from 1 to k
 *
 *	From Schneider:
 *	Let h[i] be the ith layer thickness measured at the cmp and
 *	v[i] the ith interval velocity.
 *	Set:
 *		t[i] = h[i]/v[i]
 *		t0[k] = 2 Sum t[i] * cos(dip)
 *		vs[k] = (1.0/cos(dip)) sqrt(Sum v[i]*v[i]*t[i] / Sum t[i])
 *	Define:
 *		t0by2[k] = Sum h[i]/v[i]
 *		vh[k]    = Sum v[i]*h[i]
 *	Then:
 *		t0[k] = 2 * t0by2[k] * cos(dip)
 *		vs[k] = sqrt(vh[k] / t0by2[k]) / cos(dip)
 *
 *
 */


main(int argc, char **argv)
{
	register float *v;	/* interval velocities			*/
	register float *h;	/* layer thicknesses at the cmp		*/
	register float *vs;	/* stacking velocities			*/
	register float *t0;	/* zero incidence times			*/
	register int i;		/* counter				*/
	int n;			/* number of layers			*/
	float t0by2;		/* temporary for t0/2 sum		*/
	float vh;		/* temporary for vh sum			*/
	float dip;		/* (constant) layer dip (degrees)	*/
	float cosdip;		/* cos(dip)				*/
	float ocosdip;		/* 1.0/cos(dip)				*/
	float pi;		/* the usual thing			*/
	string outpar;		/* name of file holding output parfile	*/
	FILE *outparfp;		/* ... its file pointer			*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(0);


	pi = 4.0 * atan(1.0);

	/* Get parameters */
	if (!fgetpar("dip", &dip))	dip = 0.0;
	dip = dip * pi / 180.0;
	cosdip = cos(dip);
	ocosdip = 1.0 / cosdip;

	if (!sgetpar("outpar", &outpar))	outpar = "/dev/tty";
	outparfp = efopen(outpar, "w");


	/* Allocate space for the model */
	if (n = countparval("v")) {
		v  = ealloc1float(n);
		h  = ealloc1float(n);
		vs = ealloc1float(n);
		t0 = ealloc1float(n);
	} else err("no v's specified");

	/* Get the intervals and interval velocities */
	if (n != fgetpar("h", h))
		err("expected %d intervals", n);
	if (n != fgetpar("v", v))
		err("expected %d velocities", n);

	/* Check that v's and h's are positive */
	for (i = 0; i < n; i++) {
		if (v[i] <= 0.0)
			err("v's must be positive: v[%d] = %f", i, v[i]);
		if (h[i] <= 0.0)
			err("h's must be positive: h[%d] = %f", i, h[i]);
	}

	/* Compute T0(i), Vs(i) */
	t0by2 = 0.0;
	vh = 0.0;
	for (i = 0; i < n; i++) {
		t0by2 += h[i]/v[i];
		vh += v[i] * h[i];
		t0[i] = 2.0 * t0by2 * cosdip;
		vs[i] = sqrt(vh/t0by2) * ocosdip;
	}

	/* Make par file */
	fprintf(outparfp, "tnmo=");
	for (i = 0; i < n - 1; i++) {
		fprintf(outparfp, "%g,", t0[i]);
	}
	fprintf(outparfp, "%g\n", t0[n-1]);

	fprintf(outparfp, "vnmo=");
	for (i = 0; i < n - 1; i++) {
		fprintf(outparfp, "%g,", vs[i]);
	}
	fprintf(outparfp, "%g\n", vs[n-1]);


	return EXIT_SUCCESS;
}
