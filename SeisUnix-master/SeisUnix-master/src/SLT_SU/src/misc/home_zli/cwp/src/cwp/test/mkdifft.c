/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

main()
{
	int n,l,m,nfft,i;
	float a,h,d[100],zamp[100];
	complex z[200];

	while(1) {
		printf("Enter n a h l m:\n");
		scanf("%d %f %f %d %d",&n,&a,&h,&l,&m);
		mkdiff(n,a,h,l,m,d);
		pp1d(stdout,"differentiation filter",m-l+1,l,d);
		getchar();
		getchar();
		nfft = npfa(100);
		for (i=0; i<=m-l; i++)
			z[i] = cmplx(d[i],0.0);
		for (i=m-l+1; i<nfft; i++)
			z[i] = cmplx(0.0,0.0);
		pfacc(1,nfft,z);
		for (i=0; i<nfft/2+1; i++)
			zamp[i] = fcabs(z[i]);
		pp1d(stdout,"amplitude spectrum",nfft/2+1,0,zamp);
	}
}
