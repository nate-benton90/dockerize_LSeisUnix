#include "cwp.h"

/* dim.h - header file for run time arrays
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
 * $Source: /usr/local/src/su/extras/Arrays/RCS/dim.h,v $
 * $Revision: 1.5 $ ; $Date: 89/01/04 22:32:34 $
*/


/* Allocate 1D array */
#define DIM1(p, n, type)						\
{									\
	/* Allocate room for data */					\
	if (NULL == (p = (type *)					\
		malloc((uint) n * sizeof(type))))			\
		err("%s: No heap space for data", __FILE__);		\
}


/* Free 1D storage */
#define FREE1(p)							\
{									\
	free(p);			/* free the data	  */	\
}


/* Allocate 2D array */
#define DIM2(prow, row, col, type)					\
{									\
	register type *pdata;						\
	register int i_i;						\
									\
									\
	/* Allocate room for data and row pointers */			\
	if (NULL == (pdata = (type *)					\
		malloc((uint) row * col * sizeof(type))))		\
		err("%s: No heap space for data", __FILE__);		\
	if (NULL == (prow = (type **)					\
		malloc((uint) row * sizeof(type *))))			\
		err("%s: No heap space for row pointers", __FILE__);	\
									\
									\
	/* Set row pointers */						\
	for (i_i = 0; i_i < row; ++i_i) {				\
		prow[i_i] = pdata;	/* store pointers to rows */	\
		pdata += col;		/* move to next row	  */	\
	}								\
}


/* Free 2D storage */
#define	FREE2(prow)							\
{									\
	free((char *) *prow);		/* free the data	  */	\
	free((char *) prow);		/* free the row pointers  */	\
}


/* Allocate 3D array */
#define DIM3(pgrid, grid, row, col, type)				\
{									\
	register type **prow, *pdata;					\
	register int i_i;						\
									\
									\
	/* Allocate room for data, row and grid pointers */		\
	if (NULL == (pdata = (type *)					\
		malloc((uint) grid * row * col * sizeof(type))))	\
		err("%s: No heap space for data", __FILE__);		\
	if (NULL == (prow = (type **)					\
		malloc((uint) grid * row * sizeof(type *))))		\
		err("%s: No heap space for row pointers", __FILE__);	\
	if (NULL == (pgrid = (type ***)					\
		malloc((uint) grid * sizeof(type **))))			\
		err("%s: No heap space for grid pointers", __FILE__);	\
									\
									\
	/* Set row pointers */						\
	for (i_i = 0; i_i < grid * row; ++i_i) {			\
		prow[i_i] = pdata;	/* store pointers to rows  */	\
		pdata += col;		/* move to next row	   */	\
	}								\
									\
									\
	/* Set grid pointers */						\
	for (i_i = 0; i_i < grid; ++i_i) {				\
		pgrid[i_i] = prow;	/* store pointers to grids */	\
		prow += row;		/* move to next grid	   */	\
	}								\
}


/* Free 3D storage */
#define FREE3(pgrid)							\
{									\
	free((char *) **pgrid);		/* free the data	   */	\
	free((char *) *pgrid);		/* free the row pointers   */	\
	free((char *) pgrid);		/* free the grid pointers  */	\
}


/* Dynamic creation of 2D array in subroutines */
#define DIMENSION2(arga usera, row, col, type)				\
{									\
	register int i_i;						\
									\
									\
	/* Allocate room for row pointers */				\
	if (NULL == (usera = (type **)					\
		malloc((uint) row * sizeof(type *))))			\
		err("%s: No heap space for row pointers", __FILE__);	\
									\
									\
	/* Set row pointers */						\
	for (i_i = 0; i_i < row; ++i_i) {				\
		usera[i_i] = (type *) ((char *) arga +			\
				(sizeof(type) * i_i * col));		\
	}								\
}
