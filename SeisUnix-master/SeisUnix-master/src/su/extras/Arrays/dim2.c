#include "cwp.h"

/* dim2 - allocate 2D array of arbitrary type
 *
 * Credits:
 *	Anderson and Anderson: Advanced C Tips and Techniques, Hayden, 1988
 *	CWP: Jack
 *
 * Synopsis:
 * char **dim2(row, col, size)
 * int row;	# number of elements in each row
 * int col;	# number of elements in each col
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/dim2.c,v $
 * $Revision: 1.8 $ ; $Date: 89/01/02 00:37:51 $
*/


static bool first = true;	/* to check if first entry	*/

/* Allocate 2D array */
char **dim2(row, col, size)
int row, col, size;
{
	register char **prow, *pdata;
	register int i;


	/* Allocate room for data and row pointers */
	if (NULL == (pdata = malloc((uint) row * col * size)))
		err("%s: No heap space for data", __FILE__);
	if (NULL == (prow = (char **) malloc((uint) row * sizeof(char *))))
		err("%s: No heap space for row pointers", __FILE__);


	/* Set row pointers */
	for (i = 0; i < row; ++i) {
		prow[i] = pdata;	/* store pointers to rows */
		pdata += col * size;	/* move to next row	  */
	}

	return prow;			/* pointer to 2D array    */
}


/* Free 2D storage */
void free2(pa)
char **pa;
{
	free((char *) *pa);		/* free the data	  */
	free((char *) pa);		/* free the row pointers  */
}


#ifdef TEST

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
	a = (int **) dim2(rows, cols, sizeof(int));
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			a[i][j] = inum++;
		}
	}

	b = (double **) dim2(rows, cols, sizeof(double));
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
	
	free2((char **) a);
	free2((char **) b);
	return SUCCEED;
}
	
	
#endif
