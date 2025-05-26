h62122
s 00000/00000/00032
d D 1.2 88/11/15 14:04:05 shuki 2 1
c 
e
s 00032/00000/00000
d D 1.1 88/04/14 13:49:54 shuki 1 0
c date and time created 88/04/14 13:49:54 by shuki
e
u
U
f e 0
t
T
I 1
#include <math.h>
#define EPSILON 1e-6
dopbal(pbal,n,p)
float pbal,*p;
{
	register nn;
	register float *pp,*pa,oclip;
	static first=1,k;
	static float *a;
	float clip;

	err(__FILE__,__LINE__,"dopbal not ready\n");

	if(first) {
 		a = (float*) malloc(n*sizeof(float));
 		k = pbal/100.*n - 0.5; /*round, don't truncate*/
 		if( (k<=0) || (k>n-1) ) err(__FILE__,__LINE__,"bad pbal\n");
		first = 0;
	}
	pp = p; pa = a; nn = n;
	do *(pa++) = fabs(*(pp++)); while(--nn);
 	quant(k,a,n); clip = a[k];

	if(clip>EPSILON) oclip = 1.0/clip;
	else oclip = 0.0;

/* 	fprintf(stderr,"dopbal: clip=%d oclip=%f\n",clip,oclip); */
	pp = p; nn = n;
	do {
		*(pp++) *= oclip;
	} while (--nn);
}
E 1
