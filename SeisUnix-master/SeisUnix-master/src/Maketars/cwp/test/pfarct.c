#include "cwp.h"

#define NMAX 5040
float z[NMAX],c[NMAX];

main()
{
	int n,nlast,i,j;
	double sumz,sume,scale;
	complex *cz=(complex*)z;

/*
	n = 8;
	for (i=0; i<n; i++)
		z[i] = 0.0;
	z[7] = 1.0;
	pfarc(1,n,z,cz);
	for (i=0; i<n/2; i++)
		printf("cz[%d].r = %f  cz[%d].i = %f\n",i,cz[i].r,i,cz[i].i);
	for (i=0; i<n/2; i++)
		cz[i] = cmplx(0.0,0.0);
	cz[1] = cmplx(1.0,0.0);
	pfacr(-1,n,cz,z);
	for (i=0; i<n; i++)
		printf("z[%d] = %f\n",i,z[i]);
*/
	for (i=1,n=0; i<=NMAX; i++) {
		nlast = n;
		n = 2*npfa((i+1)/2);
		if (n==nlast) continue;

		for (j=0; j<n; j++)
			c[j] = z[j] = franuni();
 
		pfarc(1,n,z,cz);
		pfacr(-1,n,cz,z);
 
		sumz = 0.0;
		sume = 0.0;
		scale = 1.0/n;
		for (j=0; j<n; j++) {
			sumz += fabs(z[j]*scale);
			sume += fabs(z[j]*scale-c[j]);
		}
		printf("n = %d  sume/sumz = %0.10f\n",n,sume/sumz);
		if (sume/sumz>1.0e-6) printf("!!! warning !!!\n");
	}
}
