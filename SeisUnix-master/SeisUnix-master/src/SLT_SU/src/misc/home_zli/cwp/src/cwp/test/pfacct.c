/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NMAX 5040
complex z[NMAX],c[NMAX];

main()
{
	int n,nlast,i,j;
	double sumz,sume,scale;

	for (i=1,n=0; i<=NMAX; i++) {
		nlast = n;
		n = npfa(i);
		if (n==nlast) continue;

		for (j=0; j<n; j++)
			c[j] = z[j] = cmplx(franuni(),franuni());
 
		pfacc(1,n,z);
		pfacc(-1,n,z);
 
		sumz = 0.0;
		sume = 0.0;
		scale = 1.0/n;
		for (j=0; j<n; j++) {
			sumz += fabs(z[j].r*scale);
			sumz += fabs(z[j].i*scale);
			sume += fabs(z[j].r*scale-c[j].r);
			sume += fabs(z[j].i*scale-c[j].i);
		}
		printf("n = %d  sume/sumz = %0.10f\n",n,sume/sumz);
		if (sume/sumz>1.0e-6) printf("!!! warning !!!\n");
	}
}
