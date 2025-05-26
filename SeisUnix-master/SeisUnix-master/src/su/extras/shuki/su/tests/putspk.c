#include <stdio.h>
putspk(f,t,nt)
float *f,t;
{
	int i,i0;

/* 	fprintf(stderr,"putspk: nt=%d t=%f\n",nt,t); */

	i0 = t;

	if(i0<0) return;
	if(i0>nt-2) return;
	f[i0+1] += t - i0;
	f[i0]   += 1.0 - f[i0+1];
}
