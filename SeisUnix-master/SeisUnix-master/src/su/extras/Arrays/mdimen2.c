#include "cwp.h"
#include "dim.h"

/* mdimen2 - test driver for using 2D arrays in subroutines
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/mdimen2.c,v $
 * $Revision: 1.1 $ ; $Date: 89/01/02 18:34:13 $
*/


#define ROWS1	2
#define	COLS1	4
#define ROWS2	4
#define COLS2	8
main()
{
	float a1[ROWS1][COLS1];
	float a2[ROWS2][COLS2];
	register float *p1, *p2;
	float fnum1 = 1.1;
	float fnum2 = 1.2;
	void display();


	/* Fill 2D arrays */
	for (p1 = &a1[0][0]; p1 < &a1[ROWS1][0]; ) {
			*p1++ = fnum1++;
	}
	for (p2 = &a2[0][0]; p2 < &a2[ROWS2][0]; ) {
			*p2++ = fnum2++;
	}

	/* Call sub to display arrays */
	display(a1, ROWS1, COLS1);
	putchar('\n');
	display(a2, ROWS2, COLS2);

	return SUCCEED;
}


void display(arg, n, m)
float *arg;
int n, m;
{
	register int i, j;
	float **a;		/* the array name */

	/* Dynamically create 2 dim "array" a from arg */
	DIMENSION2(arg, a, n, m, float);

	/* Display using 2D array notation */
	for (i = 0; i < n; ++i) {
		for (j = 0; j < m; ++j) {
			printf("%5g%c", a[i][j], (j + 1) % m ? ' ' : '\n');
		}
	}

	return;
}
