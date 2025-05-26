/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* test qksort and qkfind */

#include "cwp.h"

#define N 10001

main()
{
	int i;
	long int seed=0L;
	float *a,amedian;

	a = (float *)malloc(N*sizeof(*a));
	for (i=0; i<N; i++) {
		seed = (seed*211+1663)%7875;
		a[i] = 1+(N*seed)/7875;
	}

	printf("qkfind begin\n");
	qkfind(N/2,N,a);
	printf("qkfind end\n");

	amedian = a[N/2];

	printf("qksort begin\n");
	qksort(N,a);
	printf("qksort end\n");

	if (amedian==a[N/2]) 
		printf("qkfind successful!\n");
	else
		printf("qkfind failed!\n");
	
	for (i=1; i<N; i++)
		if (a[i]<a[i-1]) {
			printf("qksort failed!\n");
			printf("a[%d] = %f  a[%d] = %f\n",i-1,a[i-1],i,a[i]);
			exit(-1);
		}
	printf("qksort successful!\n");
}
