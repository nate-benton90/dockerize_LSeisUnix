#include <stdio.h>
#include "suhdr.h"




void set_intersect_lu(unsigned long *a, unsigned int na,
               unsigned long *b, unsigned int nb,
	       unsigned long *i, unsigned int *ni)
/* Compute the intersectioon of set a with set b and return 
   the common elements in set c and the number of elements in ni
   array i has to be declared outside with an lenght of MIN(na,nb)
   
   a and b has to be sorted in ascending order
   
*/
{
	unsigned long *s;
	unsigned long *l;
	unsigned int ns;
	unsigned int nl;
	
	
	/* check size of the arrays */
	if(ns==0 || nb==0) {
		*ni=0;
		return;
	}
	
	/* check if the two intersect at all */
	if((a[na-1] < b[0]) || (a[0] > b[nb-1])) {
		*ni=0;
		return;
	}
	
	
	if(na>nb) {
		l=a;
		nl=na;
		s=b;
		ns=nb;
	} else {
		s=a;
		ns=na;
		l=b;
		nl=nb;
	}
	
	memcpy((void *) i,(const void *) s, ns*(sizeof(unsigned long)));
	
	*ni=0;
	{ unsigned int ne=ns; /* number of elements that not has been checked */
	  unsigned int sp=0;
	  unsigned long *res;
	  
	  while(ne>0){
	   
		res=(unsigned long *)bsearch(&i[sp],l,nl,sizeof(unsigned long),LU_cmp);
		
		
		if(res==NULL) {
			/* No element matching i[sp] was found */
			/* remove i[sp] from the array */
			
			if(ne!=1) 
				/* middle elements has to be removed */
				memmove(&i[sp],&i[sp+1],(ne)*sizeof(unsigned long));
		} else {
			/* Found one leave it alone */
			/* Increment the pointer by one */
			sp++;
			*ni+=1;
		}
	   
		ne--;
	  }
	}

}
