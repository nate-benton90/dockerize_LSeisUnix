#include "su.h"
#include "segy.h"          
#include "suhdr.h"
/*********************** self documentation **********************/
char *sdoc[] = {
" 	   							",
NULL};


int
main(int argc, char **argv)
{
	char *b,*c;
	
	/* Initialize */
        initargs(argc, argv);
        requestdoc(1);
	
	
	b=ealloc1bit(1000);
	c=ealloc1bit(1000);
	
	zerobit(b,1000);
	zerobit(c,1000);
	setbit(b,999);
	setbit(c,0);
	
	fprintf(stderr," %u\n",readbit(b,999));
	fprintf(stderr," %u\n",readbit(c,0));
	ORbit(b,c,c,1000);
	fprintf(stderr," %u\n",readbit(c,999));
	fprintf(stderr," %u\n",readbit(c,0));
	fprintf(stderr," %u\n",readbit(b,0));
	fprintf(stderr," %u\n",readbit(b,999));
	
	free1bit(b);

	return(1);
}
