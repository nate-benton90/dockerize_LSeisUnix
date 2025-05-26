#include "cwp.h"

/* dim3 - allocate 3D array of arbitrary type
 *
 * Credits:
 *	Anderson and Anderson: Advanced C Tips and Techniques, Hayden, 1988
 *	CWP: Jack
 *
 * Synopsis:
 * char ***dim3(grid, row, col, size)
 * int grid;	# number of elements in each grid
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/dim3.c,v $
 * $Revision: 1.3 $ ; $Date: 89/01/02 00:37:54 $
*/


static bool first = true;	/* to check if first entry	*/

/* Allocate 3D array */
char ***dim3(grid, row, col, size)
int grid, row, col, size;
{
	register char ***pgrid, **prow, *pdata;
	register int i;


	/* Allocate room for data, row  and grid pointers */
	if (NULL == (pdata =
		malloc((uint) grid * row * col * size)))
		err("%s: No heap space for data", __FILE__);
	if (NULL == (prow =
		(char **) malloc((uint) grid * row * sizeof(char *))))
		err("%s: No heap space for row pointers", __FILE__);
	if (NULL == (pgrid =
		(char ***) malloc((uint) grid * sizeof(char **))))
		err("%s: No heap space for grid pointers", __FILE__);


	/* Set row pointers */
	for (i = 0; i < grid * row; ++i) {
		prow[i] = pdata;	/* store pointers to rows  */
		pdata += col * size;	/* move to next row	   */
	}


	/* Set grid pointers */
	for (i = 0; i < grid; ++i) {
		pgrid[i] = prow;	/* store pointers to grids */
		prow += row;	/* move to next grid	   */

	}
	return pgrid;			/* pointer to 3D array     */
}


/* Free 3D storage */
void free3(pa)
char ***pa;
{
	free((char *) **pa);		/* free the data	   */
	free((char *) *pa);		/* free the row pointers   */
	free((char *) pa);		/* free the grid pointers  */
}


#ifdef TEST

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
	a = (int ***) dim3(grids, rows, cols, sizeof(int));
	for (i = 0; i < grids; ++i) {
		for (j = 0; j < rows; ++j) {
			for (k = 0; k < cols; ++k) {
				a[i][j][k] = inum++;
			}
		}
	}

	b = (double ***) dim3(grids, rows, cols, sizeof(double));
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
	
	free3((char ***) a);
	free3((char ***) b);
	return SUCCEED;
}
	
	
#endif
