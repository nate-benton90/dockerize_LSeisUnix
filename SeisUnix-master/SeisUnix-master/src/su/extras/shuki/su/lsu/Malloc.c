#include <stdio.h>
char *Malloc(n)
unsigned n;
{
	char *r;
	r = (char*)malloc(n);
	if(r==NULL) err(__FILE__,__LINE__,"Malloc(%d) failed\n",n);
	return(r);
}
