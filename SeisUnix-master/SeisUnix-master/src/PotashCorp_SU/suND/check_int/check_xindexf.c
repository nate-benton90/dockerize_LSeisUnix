#include "su.h"
#include "suhdr.h"

int cmp(const void *a, const void *b)
{
        unsigned long i1 = *((unsigned long*) a) ;
        unsigned long i2 = *((unsigned long*) b) ;
        
        if(i1<i2) return(-1);
        if(i1>i2) return(1);
        return(0);
}


int
main()
{
	unsigned long a[12]={1,2,3,4,5,6,11,12,13,14,25,26};
	
	unsigned long ax=7;
	unsigned int indx;
	
	xindexf(12,a,&ax,sizeof(unsigned long),cmp,&indx);
	
	fprintf(stderr," %u\n",indx);
	return(0);
}
