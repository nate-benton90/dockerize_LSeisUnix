#include <stdio.h>
unfrac(p,n)
float *p;
{
	extern dtmax;
	int i;
	fprintf(stderr,"unfrac ");
	for(i=0;i<n;i++)
		p[i] = (int)(p[i]+dtmax+0.5) - dtmax;
	fprintf(stderr,"done\n");
}
