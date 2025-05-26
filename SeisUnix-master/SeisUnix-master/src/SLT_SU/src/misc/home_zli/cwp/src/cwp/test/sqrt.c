/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* test LINPACK sqr functions */

#include "cwp.h"

void pveci(int n, int *v)
{
	int i;
	for (i=0; i<n; i++)
		printf("v[%d] = %d\n",i,v[i]);
}

void pvecf(int n, float *v)
{
	int i;
	for (i=0; i<n; i++)
		printf("v[%d] = %g\n",i,v[i]);
}

#define N 5
#define P 3

main()
{
	int i,j,k,*jpvt,info,n=N,p=P;
	float **a,*b,*x,*qraux,*work,*rsd,sum,sumj;

	a = (float**)alloc2(n,p,sizeof(float));
	b = (float*)alloc1(n,sizeof(float));
	qraux = (float*)alloc1(p,sizeof(int));
	jpvt = (int*)alloc1(p,sizeof(int));
	work = (float*)alloc1(p,sizeof(int));
	x = (float*)alloc1(p,sizeof(float));
	rsd = (float*)alloc1(n,sizeof(float));

	/* Example from Kahaner, Moler, and Nash, p. 211 */
	for (i=0; i<n; i++)
		for (j=0; j<p; j++)
			a[j][i] = pow((double)(i+1),(double)j);
	b[0] = 1.0;
	b[1] = 2.3;
	b[2] = 4.6;
	b[3] = 3.1;
	b[4] = 1.2;
	
	/*
	sqrdc(a,n,p,qraux,jpvt,work,0);
	
	sqrsl(a,n,p,qraux,b,NULL,x,x,rsd,NULL,110,&info);
	printf("info = %d\n",info);
	*/
	
	sqrst(a,n,p,b,1.0e-4,x,rsd,&k,jpvt,qraux,work);
	printf("k = %d\n",k);
	
	printf("\npivots\n");
	pveci(p,jpvt);
	printf("\nparameters\n");
	pvecf(p,x);
	printf("\nresiduals\n");
	pvecf(n,rsd);
	for (i=0,sum=0.0; i<n; i++)
		sum += rsd[i]*rsd[i];
	printf("norm residuals = %g\n",sqrt(sum));
	
	for (i=0; i<n; i++)
		for (j=0; j<p; j++)
			a[j][i] = pow((double)(i+1),(double)j);
	for (i=0,sum=0.0; i<n; i++) {
		for (j=0,sumj=0.0; j<p; j++)
			sumj += a[j][i]*x[j];
		sumj -= b[i];
		sum += sumj*sumj;
	}
	printf("norm residuals = %g\n",sqrt(sum));
}
