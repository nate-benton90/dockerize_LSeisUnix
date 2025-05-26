#include "cwp.h"
#include "dim.h"

/* mdim3 - test driver for 3D array macros
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/mdim3.c,v $
 * $Revision: 1.1 $ ; $Date: 89/01/02 09:29:42 $
*/


main(argc, argv)
int argc; char *argv[];
{
	int ***a;			/* 3D array of ints	  */
	double ***b;			/* 3D array of doubles	  */
	int grids, rows, cols;
	register int i, j, k;
	int inum = 1;
	double dnum = 1.1;

	/* Initialize SU */
	initargs(argc, argv);

	/* Get grid, row and col lengths */
	MUSTIGETPAR("grids", &grids);
	MUSTIGETPAR("rows", &rows);
	MUSTIGETPAR("cols", &cols);

	/* Create and fill 3D arrays */
	DIM3(a, grids, rows, cols, int);
	for (i = 0; i < grids; ++i) {
		for (j = 0; j < rows; ++j) {
			for (k = 0; k < cols; ++k) {
				a[i][j][k] = inum++;
			}
		}
	}

	DIM3(b, grids, rows, cols, double);
	for (i = 0; i < grids; ++i) {
		for (j = 0; j < rows; ++j) {
			for (k = 0; k < cols; ++k) {
				b[i][j][k] = dnum++;
			}
		}
	}


	/* Display arrays */
	for (i = 0; i < grids; ++i) {
		printf("Grid %d:\n", i);
		for (j = 0; j < rows; ++j) {
			for (k = 0; k < cols; ++k) {
				printf("%5d%c",
					a[i][j][k], (k+1) % cols ? ' ' : '\n');
			}
		}
	}

	putchar('\n');
	for (i = 0; i < grids; ++i) {
		printf("Grid %d:\n", i);
		for (j = 0; j < rows; ++j) {
			for (k = 0; k < cols; ++k) {
				printf("%5g%c",
					b[i][j][k], (k+1) % cols ? ' ' : '\n');
			}
		}
	}
	
	FREE3((char ***) a);
	FREE3((char ***) b);

	return SUCCEED;
}
