/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* test LINPACK dge functions */

#include "cwp.h"

void pvecd(int n, double *v)
{
	int i;
	for (i=0; i<n; i++)
		printf("v[%d] = %g\n",i,v[i]);
}

#define N 2

main()
{
	int i,j,*ipvt,info,n=N;
	double **a,*b,*z,rcond;

	a = (double**)alloc2(n,n,sizeof(double));
	ipvt = (int*)alloc1(n,sizeof(int));
	b = (double*)alloc1(n,sizeof(double));
	z = (double*)alloc1(n,sizeof(double));

	/* Examples 3.6 & 3.7 from Kahaner, Moler, and Nash */
	a[0][0] = 9.7;	a[1][0] = 6.6;
	a[0][1] = 4.1;	a[1][1] = 2.8;
	b[0] = 9.70;	b[1] = 4.11;

/*
	dgefa(a,n,ipvt,&info);
	printf("info = %d\n",info);
*/

	dgeco(a,n,ipvt,&rcond,z);
	printf("rcond = %g\n",rcond);
	printf("condition number estimate = %g\n",1.0/rcond);
	printf("number of significant figures = %d\n",
		(int)log10(rcond/DBL_EPSILON));

	dgesl(a,n,ipvt,b,0);
	pvecd(n,b);
}
