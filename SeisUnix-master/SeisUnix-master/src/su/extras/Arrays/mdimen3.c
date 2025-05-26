#include "cwp.h"
#include "dim.h"

/* mdimen3 - test driver for using 3D arrays in subroutines
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/mdimen3.c,v $
 * $Revision: 1.1 $ ; $Date: 89/01/02 18:35:50 $
*/


#define GRIDS1	2
#define ROWS1	2
#define	COLS1	4
#define GRIDS2	2
#define ROWS2	4
#define COLS2	8
main()
{
	float a1[GRIDS1][ROWS1][COLS1];
	float a2[GRIDS2][ROWS2][COLS2];
	register float *p1, *p2;
	float fnum1 = 1.1;
	float fnum2 = 1.2;
	void display();


	/* Fill 2D arrays */
	for (p1 = &a1[0][0][0]; p1 < &a1[GRIDS1][0][0]; ) {
			*p1++ = fnum1++;
	}
	for (p2 = &a2[0][0][0]; p2 < &a2[GRIDS2][0][0]; ) {
			*p2++ = fnum2++;
	}

	/* Call sub to display arrays */
	display(a1, GRIDS1, ROWS1, COLS1);
	putchar('\n');
	display(a2, GRIDS2, ROWS2, COLS2);

	return SUCCEED;
}


void display(arg, n, m, p)
float *arg;
int n, m, p;
{
	register int i, j, k;
	float ***a;		/* the array name */

	/* Dynamically create 3 dim "array" a from arg */
	DIMENSION3(arg, a, n, m, p, float);

	/* Display using 3D array notation */
	for (i = 0; i < n; ++i) {
		printf("Grid %d:\n", i);
		for (j = 0; j < m; ++j) {
			for (k = 0; k < p; ++k) {
				printf("%5g%c",
					a[i][j][k], (k + 1) % p ? ' ' : '\n');
			}
		}
	}

	return;
}
