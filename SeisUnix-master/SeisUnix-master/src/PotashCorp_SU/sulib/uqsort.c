#include <stdio.h>
#include "su.h"

void uqsort(int n, int *un, float *a)
/* Unique sort of float array
   only outputs the first element of a equal sub-sequence
   the unique element are returned as the first un elmenst of array a
*/
{
	int i,in;
	
	if(n==1) {
		*un=1;
		return;
	}
	/* do a quick sort */
	qksort(n,a);
	
	/* check for equal sub-sequence */
	i=1;
	in=1;
	while(in<n) {	
		while(a[in]==a[i-1] && in!=n) {
			in++;
		}
		a[i]=a[in];
		i++;
	}
	*un=i-1;
}
	
