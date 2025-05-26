#include "su.h"
#include "suhdr.h"


void
xindexf (unsigned int nx, void * ax,void *x, size_t size,
       int(*compar)(const void *, const void *),
       unsigned int *index)
/*****************************************************************************
determine index of x with respect to an array of x values
******************************************************************************
Input:
nx		number of x values in array ax
ax		base address of ax
size		size of elements in ax
compar		comparison function
       
       The comparison function must return an  integer  less  than,  equal  to,  or
       greater  than  zero  if  the first argument is considered to be respectively
       less than, equal to, or greater than the second.  If two members compare  as
       equal, their order in the sorted array is undefined.

x		address of the value for which index should be determined 
index		index determined previously (used to begin search)

Output:
index		for monotonically increasing ax values, the largest index
		for which ax[index]<=x, except index=0 if ax[0]>x;
		for monotonically decreasing ax values, the largest index
		for which ax[index]>=x, except index=0 if ax[0]<x
******************************************************************************
Notes:
This function is designed to be particularly efficient when called
repeatedly for slightly changing x values; in such cases, the index 
returned from one call should be used in the next.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 12/25/89
         B.Nemeth, Potashcorp.com
*****************************************************************************/
{
	unsigned int lower,upper,middle,step;

	/* initialize lower and upper indices and step */
	lower = *index;
	if (lower<0) lower = 0;
	if (lower>=nx) lower = nx-1;
	upper = lower+1;
	step = 1;

	/* if x values increasing */
	if (compar(ax+(nx-1)*size,ax)>0) {

		/* find indices such that ax[lower] <= x < ax[upper] */
		while (lower>0 && compar(ax+lower*size,x)>0) {
			upper = lower;
			lower -= MIN(step,lower);/* this prevents lower being negative */
			step += step;
		}
		if (lower<0) lower = 0;
		while (upper<nx && compar(ax+upper*size,x)<=0) {
			lower = upper;
			upper += step;
			step += step;
		}
		if (upper>nx) upper = nx;

		/* find index via bisection */
		while ((middle=(lower+upper)>>1)!=lower) {
			if (compar(x,ax+middle*size)>=0)
				lower = middle;
			else
				upper = middle;
		}

	/* else, if not increasing */
	} else {

		/* find indices such that ax[lower] >= x > ax[upper] */
		while (lower>0 && compar(ax+lower*size,x)<0) {
			upper = lower;
			lower -= MIN(step,lower); /* this prevents lower being negative */
			step += step;
		}
		if (lower<0) lower = 0;
		while (upper<nx && compar(ax+upper*size,x)>=0) {
			lower = upper;
			upper += step;
			step += step;
		}
		if (upper>nx) upper = nx;

		/* find index via bisection */
		while ((middle=(lower+upper)>>1)!=lower) {
			if (compar(x,ax+middle*size)<=0)
				lower = middle;
			else
				upper = middle;
		}
	}

	/* return lower index */
	*index = lower;
}
