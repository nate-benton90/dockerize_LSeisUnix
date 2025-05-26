/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NMAX 100
#define NMAX2 (NMAX+2)*(NMAX+2)
float z[NMAX2],c[NMAX2];
/**/ complex cz[NMAX2]; /**/

main()
{
	int n1,n2,i1,i2,j;
	float scale,sumz,sume;
	/** / complex *cz=(complex*)z; /**/
 
	for (n1=npfar(1),n2=npfar(n1+1); n2<NMAX; n1=n2,n2=npfar(n1+1)) {
		for (j=0; j<n1*n2; j++)
			c[j] = z[j] = franuni();
		pfa2rc(-1,2,n1,n2,z,cz);
		pfa2cr(1,2,n1,n2,cz,z);
		pfa2rc(1,1,n1,n2,z,cz);
		pfa2cr(-1,1,n1,n2,cz,z);
		scale = 1.0/(float)(n1*n2);
		for (j=0; j<n1*n2; j++)
			z[j] *= scale;

		for (j=0,sumz=sume=0.0; j<n1*n2; j++) {
			sumz += fabs(z[j]);
			sume += fabs(z[j]-c[j]);
		}
		printf("n1 = %d  n2 = %d  sume/sumz = %0.10f\n",
			n1,n2,sume/sumz);
		if (sume/sumz>1.0e-6) {printf("!!! warning !!!\n"); exit(-1);}
	}
}
