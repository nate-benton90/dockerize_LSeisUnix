/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NXMAX 100

main()
{
	int i,j,n,d;
	float x,a[NXMAX];

	while(1) {
		printf("Enter n,d,x,i:\n");
		scanf("%d %d %f %d",&n,&d,&x,&i);
		printf("\n");
		for (j=0; j<n; j++) {
			a[j] = (d>0) ? j : n-1-j;
			printf("a[%d] = %f\n",j,a[j]);
		}
		xindex(n,a,x,&i);
		printf("i = %d\n",i);
	}
}
