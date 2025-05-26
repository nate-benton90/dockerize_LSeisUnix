#include "su.h"
#include "suhdr.h"


int
main()
{
	unsigned long a[7]={11,12,13,14,25,26,27};
	unsigned long b[6]={11,12,13,14,15,26};
	unsigned long c[11]={0,0,0,0,0,0,0,0,0,0,0};
	unsigned int ni,i;
	
	set_intersect_lu(a,7,b,6,c,&ni);
	fprintf(stderr," %u\n",ni);
	for(i=0;i<ni;i++) fprintf(stderr," %lu\n",c[i]);
	return(0);
}
