#include <stdio.h>
dcout(p,n)			/* filter out dc */
float *p; int n;
{
	extern dtmax;
	int is; float mean;

	fprintf(stderr,"dcout: ");
	for(is=0,mean=0.;is<n;is++)
		mean += p[is] - dtmax;
	mean /= n;
	for(is=0;is<n;is++)
		p[is] -= mean;
	fprintf(stderr,"done\n");
}

rlinout(p,n)
float *p;
{
	int i;
	float a,b,r1,r2,inv11,inv12,inv22;
	extern dtmax;

	fprintf(stderr,"rlinout: ");
	for(i=0,r1=r2=0.0;i<n;i++) {
		r1 += (p[i] - dtmax)*i;
		r2 += p[i]  - dtmax;
		}

	inv11 = 12./n/(n+1)/(n-1);
	inv12 = -6./n/(n+1);
	inv22 = 2.*(2*n-1)/n/(n+1);

	a = inv11*r1 + inv12*r2;
	b = inv12*r1 + inv22*r2;

	for(i=0;i<n;i++)
		p[i] -= a*i + b;
	fprintf(stderr,"done\n");
}
