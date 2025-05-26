#include <stdio.h>
nyqout(p,n,smoo)
float *p,*smoo; int n;
{
	int i,nm1=n-1;
	fprintf(stderr,"nyqout: ");
	/* filter out nyquist */
	i=0; smoo[i] = (2.*p[i]+p[i+1])/3.;
	for(i=1;i<nm1;i++)
		smoo[i] = (p[i-1]+2.*p[i]+p[i+1])/4.;
	i=nm1; smoo[i] = (p[i-1]+2.*p[i])/3.;
	copy(smoo,p,n);
	fprintf(stderr,"done\n");
}
