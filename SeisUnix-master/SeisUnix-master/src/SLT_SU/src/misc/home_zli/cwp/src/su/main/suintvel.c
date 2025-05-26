/* SUINTVEL: $Revision: 1.3 $ ; $Date: 90/05/29 20:44:44 $		*/

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

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUINTVEL - convert stacking velocity model to interval velocity model	\n\
									\n\
suintvel vs= t0= outpar=/dev/tty					\n\
									\n\
Required parameters:					        	\n\
	vs=	stacking velocities 					\n\
	t0=	normal incidence times		 			\n\
									\n\
Optional parameters:							\n\
	outpar=/dev/tty		output parameter file in the form:	\n\
				h=layer thicknesses vector		\n\
				v=interval velocities vector		\n\
									\n\
Examples:								\n\
    suintvel vs=5000,5523,6339,7264 t0=.4,.8,1.125,1.425 outpar=intpar	\n\
									\n\
    suintvel par=stkpar outpar=intpar					\n\
									\n\
If the file, stkpar, contains:						\n\
    vs=5000,5523,6339,7264						\n\
    t0=.4,.8,1.125,1.425						\n\
then the two examples are equivalent.					\n\
									\n\
Note: suintvel does not have standard su syntax since it does not	\n\
      operate on seismic data.  Hence stdin and stdout are not used.	\n\
									\n\
Note: may go away in favor of par program, velconv, by Dave		\n\
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
 *	Define:
 *		t0by2[k] = 0.5 * t0[k] = Sum h[i]/v[i]
 *		vh[k] = vs[k]*vs[k]*t0by2[k] = Sum v[i]*h[i]
 *	Then:
 *		dt[i] = h[i]/v[i] = t0by2[i] - t0by2[i-1]
 *		dvh[i] = h[i]*v[i] = vh[i] - vh[i-1]
 *		h[i] = sqrt(dvh[i] * dt[i])
 *		v[i] = sqrt(dvh[i] / dt[i])
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
	float t1, t2;		/* temporaries for one-way times	*/
	float v1, v2;		/* temporaries for stacking v's		*/
	float dt;		/* temporary for t0/2 difference 	*/
	float dvh;		/* temporary for v*h difference		*/
	string outpar;		/* name of file holding output parfile	*/
	FILE *outparfp;		/* ... its file pointer			*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(0);


	outpar = "/dev/tty" ;	sgetpar("outpar", &outpar);
	outparfp = efopen(outpar, "w");


	/* Allocate space for the model */
	if (n = countparval("t0")) {
		v  = ealloc1float(n);
		h  = ealloc1float(n);
		vs = ealloc1float(n);
		t0 = ealloc1float(n);
	} else err("no t0's specified");

	/* Get the normal incidence times and stacking velocities */
	if (n != fgetpar("t0", t0))
		err("expected %d intervals", n);
	if (n != fgetpar("vs", vs))
		err("expected %d velocities", n);

	/* Check that vs's and t0's are positive */
	for (i = 0; i < n; i++) {
		if (vs[i] <= 0.0)
			err("vs's must be positive: vs[%d] = %f", i, vs[i]);
		if (t0[i] <= 0.0)
			err("t0's must be positive: t0[%d] = %f", i, t0[i]);
	}

	/* Compute h(i), v(i) */
	h[0] = 0.5 * vs[0] * t0[0];
	v[0] = vs[0];
	for (i = 1; i < n; i++) {
		t2 = 0.5 * t0[i]; t1 = 0.5 * t0[i-1];
		v2 = vs[i]; v1 = vs[i-1];
		dt = t2 - t1;
		dvh = v2*v2*t2 - v1*v1*t1;
		h[i] = sqrt(dvh * dt);
		v[i] = sqrt(dvh / dt);
	}

	/* Make par file */
	fprintf(outparfp, "h=");
	for (i = 0; i < n - 1; i++) {
		fprintf(outparfp, "%g,", h[i]);
	}
	fprintf(outparfp, "%g\n", h[n-1]);

	fprintf(outparfp, "v=");
	for (i = 0; i < n - 1; i++) {
		fprintf(outparfp, "%g,", v[i]);
	}
	fprintf(outparfp, "%g\n", v[n-1]);


	return EXIT_SUCCESS;
}
