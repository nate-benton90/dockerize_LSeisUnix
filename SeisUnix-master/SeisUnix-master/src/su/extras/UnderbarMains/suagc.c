/* SUAGC: $Revision: 2.8 $ ; $Date: 89/09/20 19:34:55 $		*/

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

#include "cwp.h"
#include "segy.h"
#include "header.h"
#include "fconst.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUAGC - automatic gain control using a Gaussian weighted window	\n\
								\n\
suagc <stdin >stdout wagc=20					\n\
								\n\
Required parameters:						\n\
	none							\n\
								\n\
Optional parameters:						\n\
	wagc = 20	length of agc window in samples		\n\
								\n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Brian, Jack
 *
 * Notes:
 *	For each input point, we compute a weighted sum of squares of
 *	the data in a symmetric window around the point and divide
 *	the input value by the square root of that value.  This enhances
 *	events which are large relative to the window.  The weight used
 *	is a Gaussian.
 *
 *	Some possible optimizations: unroll the loops; test for a zero
 *	input datum and output a zero output datum (saving the square root
 *	operation) if traces with lots of zeroes are likely.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suagc.c,v $";
static string revid =
	"   $Revision: 2.8 $ ; $Date: 89/09/20 19:34:55 $";


#define EPS	3.8090232	/* exp(-EPS*EPS) = 5e-7, "noise" level	*/
#define WAGC	20		/* default window size			*/

segy intrace, outtrace;

main(argc, argv)
int argc; char **argv;
{
	int wagc;		/* window size				*/
	int nt;			/* number of points on trace		*/
	float u;		/* related to reciprocal of std dev	*/
	float usq;		/* u*u					*/
	register float wtmp;	/* storage for w[i]			*/
	float *w;		/* Gaussian window weights		*/
	register float *d2;	/* square of input data			*/
	register float stmp;	/* storage for s[i]			*/
	register float *s;	/* weighted sum of squares of the data	*/
	register int i;		/* counter 				*/
	float floati;		/* float(i)				*/
	register int j;		/* counter				*/
	register int k;		/* counter				*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Get parameter */
	if (!igetpar("wagc", &wagc))	wagc = WAGC;
	if (wagc <= 0) err("wagc=%d must be positive", wagc);


	/* Gaussian is symmetric, so work with half */
	++wagc;
	wagc >>= 1;


	/* Get nt from first trace */
	if (!gettr(&intrace)) err("can't get first trace");
	nt = intrace.ns;

	/* Allocate and compute Gaussian window weights */
	w = vec(wagc);
	u = EPS / ((float) wagc);
	usq = u*u;
	for (i = 1; i < wagc; i++) {
		floati = (float) i;
		w[i] = exp(-(usq*floati*floati));
	}

	/* Allocate sum of squares and weighted sum of squares */
	d2 = vec(nt);
	s  = vec(nt);

	/* Loop over traces */
	do {
		/* Put sum of squares of data in d2 */
		vsq_(intrace.data, ONE, d2, ONE, &nt);

		/* Initialize s to d2 to get center point set */
		vmov_(d2, ONE, s, ONE, &nt);

		/* Compute weighted sum s; use symmetry of Gaussian */
		for (j = 1; j < wagc; j++) {
			wtmp = w[j];
			for (i = j; i < nt; i++)
				s[i] += wtmp*d2[i-j]; 
			k = nt - j;
			for (i = 0; i < k; i++)
				s[i] += wtmp*d2[i+j]; 
		}

		for (i = 0; i < nt; i++) {
			stmp = s[i];
			outtrace.data[i] =
			   (stmp == 0.0 ? 0.0 : intrace.data[i]/sqrt(stmp));
		}

		bcopy(&intrace, &outtrace, HDRBYTES);
		puttr(&outtrace);

	} while (gettr(&intrace));


	return SUCCEED;
}
