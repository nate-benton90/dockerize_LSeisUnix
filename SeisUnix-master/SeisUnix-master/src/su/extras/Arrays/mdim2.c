#include "cwp.h"
#include "dim.h"

/* mdim2 - test driver for 2D arrays at run time
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/mdim2.c,v $
 * $Revision: 1.1 $ ; $Date: 89/01/02 08:31:54 $
*/


main(argc, argv)
int argc; char *argv[];
{
	int **a;			/* 2D array of ints	  */
	double **b;			/* 2D array of doubles	  */
	int rows, cols;
	register int i, j;
	int inum = 1;
	double dnum = 1.1;

	/* Initialize SU */
	initargs(argc, argv);


	/* Get row and col lengths */
	MUSTIGETPAR("rows", &rows);
	MUSTIGETPAR("cols", &cols);

	/* Create and fill 2D arrays */
	DIM2(a, rows, cols, int);
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			a[i][j] = inum++;
		}
	}

	DIM2(b, rows, cols, double);
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			b[i][j] = dnum++;
		}
	}

	/* Display arrays */
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			printf("%5d%c", a[i][j], (j+1) % cols ? ' ' : '\n');
		}
	}
	putchar('\n');

	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			printf("%5g%c", b[i][j], (j+1) % cols ? ' ' : '\n');
		}
	}
	
	FREE2((char **) a);
	FREE2((char **) b);

	return SUCCEED;
}
