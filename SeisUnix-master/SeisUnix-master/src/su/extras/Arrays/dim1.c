#include "cwp.h"

/* dim1 - allocate 1D array of arbitrary type
 *
 * Credits:
 *	Anderson and Anderson: Advanced C Tips and Techniques, Hayden, 1988
 *	CWP: Jack
 *
 * Synopsis:
 * char *dim1(n, size)
 * int n;	# number of elements in array
 * int size;	# sizeof(datatype)
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/dim1.c,v $
 * $Revision: 1.3 $ ; $Date: 89/01/02 00:37:44 $
*/


static bool first = true;	/* to check if first entry	*/

/* Allocate 1D array */
char *dim1(n, size)
int n, size;
{
	register char *pdata;


	/* Allocate room for data */
	if (NULL == (pdata = malloc((uint) n * size)))
		err("%s: No heap space for data", __FILE__);


	return pdata;			/* pointer to 1D array    */
}


/* Free 1D storage */
void free1(pa)
char *pa;
{
	free(pa);			/* free the data	  */
}


#ifdef TEST

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
	a = (int *) dim1(n, sizeof(int));
	for (i = 0; i < n; ++i) {
		a[i] = inum++;
	}

	b = (double *) dim1(n, sizeof(double));
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
	
	free1((char *) a);
	free1((char *) b);
	return SUCCEED;
}
	
	
#endif
