/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NMAX 100
#define NMAX2 NMAX*NMAX
complex z[NMAX2],c[NMAX2];

main()
{
	int n1,n2,j;
	float scale,sumz,sume;
 
	for (n1=1,n2=npfa(n1+1); n2<NMAX; n1=n2,n2=npfa(n1+1)) {
		for (j=0; j<n1*n2; j++)
			c[j] = z[j] = cmplx(franuni(),franuni());
		pfamcc(1,n1,n2,1,n1,z);
		pfamcc(1,n2,n1,n1,1,z);
		pfamcc(-1,n1,n2,1,n1,z);
		pfamcc(-1,n2,n1,n1,1,z);
		scale = 1.0/(float)(n1*n2);
		for (j=0; j<n1*n2; j++)
			z[j] = crmul(z[j],scale);

		for (j=0,sumz=sume=0.0; j<n1*n2; j++) {
			sumz += fcabs(z[j]);
			sume += fcabs(csub(z[j],c[j]));
		}
		printf("n1 = %d  n2 = %d  sume/sumz = %0.10f\n",
			n1,n2,sume/sumz);
		if (sume/sumz>1.0e-6) printf("!!! warning !!!\n");
	}
}
