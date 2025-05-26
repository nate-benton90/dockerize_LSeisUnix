/*********************** self documentation **********************/
char *sdoc = "\
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

/* suagc - automatic gain control using a Gaussian weighted window
 *
 * Credits:
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
 *----------------------------------------------------------------------
 * Permission to use the SU programs is granted.  Courteous users will
 * retain this acknowlegment of origins at the Stanford Exploration
 * Project, Stanford University and the Center for Wave Phenomena, 
 * Colorado School of Mines.   Inquiries/bug reports should be sent to:
 * Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 * Golden, CO 80014
 *----------------------------------------------------------------------
 *
 * $Author: jkc $
 * $Source: /src/su/src/RCS/suagc.c,v $
 * $Revision: 2.1 $ ; $Date: 88/11/20 22:31:56 $
*/

/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/src/RCS/suagc.c,v $";
static char revid[] =
	"   $Revision: 2.1 $ ; $Date: 88/11/20 22:31:56 $";


#include "../include/cwp.h"
#include "../include/segy.h"
#include "../include/hdr.h"
#include "../include/fconst.h"

segy intrace, outtrace;

#define EPS	3.8090232	/* exp(-EPS*EPS) = 5e-7, "noise" level	*/
#define WAGC	20		/* default window size			*/

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


	initgetpar(argc, argv); askdoc(1);


	/* Get parameter */
	wagc = WAGC;	igetpar("wagc", &wagc);
	if (wagc <= 0) {
		err("wagc=%d must be positive", wagc);
	}
	wagc >>= 1;	/* Gaussian is symmetric, so work with half */

	/* Get nt from first trace */
	if (!gettr(&intrace)) {
		err("first trace is null");
	}
	nt = intrace.ns;

	/* Allocate and compute Gaussian window weights */
	w = vector(wagc);
	u = EPS / ((float) wagc);
	usq = u*u;
	for (i = 1; i <= wagc; i++) {
		floati = (float) i;
		w[i] = exp(-(usq*floati*floati));
	}

	/* Allocate sum of squares and weighted sum of squares */
	d2 = vector(nt);
	s  = vector(nt);

	/* Loop over traces */
	do {
		/* Put sum of squares of data in d2 */
		vsq_(intrace.data, ONE, d2, ONE, &nt);

		/* Initialize s to d2 to get center point set */
		vmov_(d2, ONE, s, ONE, &nt);

		/* Compute weighted sum s; use symmetry of Gaussian */
		for (j = 1; j <= wagc; j++) {
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
	exit(0);
}
