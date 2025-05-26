/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define N 100000

main()
{
	int i;
	long int seed=0L;
	float *a;

	a = (float *)malloc(N*sizeof(*a));
	for (i=0; i<N; i++) {
		seed = (seed*211+1663)%7875;
		a[i] = 1+(N*seed)/7875;
	}

	hpsort(N,a);

	for (i=1; i<N; i++)
		if (a[i]<a[i-1]) {
			printf("a[%d] = %f  a[%d] = %f\n",i-1,a[i-1],i,a[i]);
			exit(0);
		}
	printf("hpsort successful!\n");
}
