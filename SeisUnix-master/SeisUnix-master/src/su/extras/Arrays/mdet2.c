#include "cwp.h"
#include "dim.h"

/* mdimension2 - test driver using 2D arrays in subroutines
 *
 * Credits:
 *	Anderson and Anderson: Advanced C Tips and Techniques, Hayden, 1988
 *	CWP: Jack
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/mdet2.c,v $
 * $Revision: 1.2 $ ; $Date: 89/01/02 18:31:07 $
*/


/* find determinant of a 2-D array of doubles */
main()
{
	double det();

	static double f[4][4] = {
		1,   3,   2,   1,
		4,   6,   1,   2,
		2,   1,   2,   3,
		1,   2,   4,   1,
	};
	static double g[5][5] = {
		1,   3,   2,   1,  7,
		4,   6,   1,   2,  6,
		2,   1,   2,   3,  5,
		1,   2,   4,   1,  4,
		8,   5,   4,   1,  3,
	};


	printf("determinant of f = %g\n", det(f, 4));
	printf("determinant of g = %g\n", det(g, 5));

	return SUCCEED;
}



/* calculate determinant for n by n matrix */
double det(arg, n)
double *arg;
int n;
{
	register int i, j, k;
	double **a;		/* this is the array name */
	double d;		/* determinant		  */
	double x;		/* temp			  */

	/* Dynamically create 2 dim "array" from arg */
	DIMENSION2(arg, a, n, n, double);


	/* Determinant algorithm using array indices */
	for (k = 0; k < n - 1; ++k) {
		for (i = k + 1; i < n; ++i) {
			x = a[i][k] / a[k][k];
			for (j = k; j < n; ++j) {
				a[i][j] -= x * a[k][j];
			}
		}
	}

	for (d = 1.0, i = 0; i < n; ++i) {
		d *= a[i][i];
	}

	free((char *) a);

	return d;
}
