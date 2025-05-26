#include <math.h>
#define EPSILON 1e-6
doqbal(qbal,n,p)
float qbal,*p;
{
	register nn;
	register float *pp,*pa,oclip;
	static first=1,k;
	static float *a;
	float clip;

	if(first) {
 		a = (float*) malloc(n*sizeof(float));
 		k = qbal/100.*n - 0.5; /*round, don't truncate*/
 		if( (k<=0) || (k>n-1) ) err(__FILE__,__LINE__,"bad qbal\n");
		first = 0;
	}
	pp = p; pa = a; nn = n;
	do *(pa++) = fabs(*(pp++)); while(--nn);
 	quant(k,a,n); clip = a[k];

	if(clip>EPSILON) oclip = 1.0/clip;
	else oclip = 0.0;

/* 	fprintf(stderr,"doqbal: clip=%d oclip=%f\n",clip,oclip); */
	pp = p; nn = n;
	do {
		*(pp++) *= oclip;
	} while (--nn);
}
