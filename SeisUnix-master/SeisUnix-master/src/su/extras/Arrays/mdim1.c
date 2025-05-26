#include "cwp.h"
#include "dim.h"

/* mdim1 - test driver for 1D array allocation
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/mdim1.c,v $
 * $Revision: 1.1 $ ; $Date: 89/01/02 09:09:19 $
*/


main(argc, argv)
int argc; char *argv[];
{
	int *a;				/* 1D array of ints	  */
	double *b;			/* 1D array of doubles	  */
	int n;
	register int i;
	int inum = 1;
	double dnum = 1.1;

	/* Initialize SU */
	initargs(argc, argv);

	/* Get array length parameter */
	MUSTIGETPAR("n", &n);

	/* Create and fill 1D arrays */
	DIM1(a, n, int);
	for (i = 0; i < n; ++i) {
		a[i] = inum++;
	}

	DIM1(b, n, double);
	for (i = 0; i < n; ++i) {
		b[i] = dnum++;
	}

	/* Display arrays */
	for (i = 0; i < n; ++i) {
		printf("%5d", a[i]);
	}
	putchar('\n'); putchar('\n');

	for (i = 0; i < n; ++i) {
		printf("%5g", b[i]);
	}
	putchar('\n');
	
	FREE1((char *) a);
	FREE1((char *) b);

	return SUCCEED;
}
