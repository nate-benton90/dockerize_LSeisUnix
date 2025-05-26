#include "su.h"
#include "suhdr.h"

typedef struct dataPindex{
	unsigned long d;
	unsigned long i;
} dpi;

int cmp(const void *a, const void *b)
{
        dpi i1 = *((dpi*) a) ;
        dpi i2 = *((dpi*) b) ;
        
        if(i1.i<i2.i) return(-1);
        if(i1.i>i2.i) return(1);
        return(0);
}


int
main()
{
	unsigned long a[6]={11,12,13,14,25,26};
	unsigned long b[6]={1,2,3,4,5,6};
	unsigned long c[11]={0,0,0,0,0,0,0,0,0,0,0};
	unsigned int ni,i,un=0;
	
	dpi *di_a;
	
	di_a = malloc(6*sizeof(dpi*));
		
	for(i=0;i<6;i++) {
		di_a[i].d=a[i];
		di_a[i].i=b[i];
	}
	
	qsort(&di_a[0], 6,sizeof(dpi),cmp);	
	unqsort(&di_a[0], 6,sizeof(dpi),cmp,&un);
	for(i=0;i<un;i++) fprintf(stderr," %lu %lu \n",di_a[i].d,di_a[i].i);
	fprintf(stderr," %u\n",un);
	
	return(0);
}
