/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define N 100
#define NFFT 500

float p[N],q[N];
complex z[NFFT];
float zamp[NFFT];

main()
{
	int npoles,nfft,i;
	float f3db,zero=0.0,fpass,apass,fstop,astop;

/*
	printf("Enter npoles f3db:\n");
	scanf("%d %f",&npoles,&f3db);
*/
	printf("Enter fpass apass fstop astop:\n");
	scanf("%f %f %f %f",&fpass,&apass,&fstop,&astop);
	bfdesign(fpass,apass,fstop,astop,&npoles,&f3db);
	printf("npoles = %d  f3db = %f\n",npoles,f3db);

	/* impulse response */
	scopy(N,&zero,0,p,1);
	p[0] = 1.0;
	bflowpass(npoles,f3db,N,p,q);
	pp1d(stdout,"impulse response",N,0,q);

	/* amplitude spectrum */
	nfft = npfa(N);
	for (i=0; i<N; i++)
		z[i] = cmplx(q[i],0.0);
	for (i=N; i<nfft; i++)
		z[i] = cmplx(0.0,0.0);
	pfacc(1,nfft,z);
	for (i=0; i<nfft; i++)
		zamp[i] = fcabs(z[i]);
	pp1d(stdout,"amplitude spectrum",nfft/2+1,0,zamp);
}
