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
	int i,j,k,*ipvt,n=N,p=P;
	float rcond,**a,**ata,*work,*b,*x;

	a = (float**)alloc2(n,p,sizeof(float));
	b = (float*)alloc1(n,sizeof(float));
	ata = (float**)alloc2(p,p,sizeof(float));
	ipvt = (int*)alloc1(p,sizeof(int));
	work = (float*)alloc1(p,sizeof(float));
	x = (float*)alloc1(p,sizeof(float));
	
	for (i=0; i<n; i++)
		for (j=0; j<p; j++)
			a[j][i] = pow((float)(i+1),(float)j);
	b[0] = 1.0;
	b[1] = 2.3;
	b[2] = 4.6;
	b[3] = 3.1;
	b[4] = 1.2;

	for (i=0; i<p; i++)
		for (j=0; j<p; j++)
			for (k=0,ata[j][i]=0.0; k<n; k++)
				ata[j][i] += a[j][k]*a[i][k];
	for (i=0; i<p; i++)
		for (k=0,x[i]=0.0; k<n; k++)
			x[i] += a[i][k]*b[k];
				
	sgeco(ata,p,ipvt,&rcond,work);
	printf("rcond = %g\n",rcond);
	sgesl(ata,p,ipvt,x,0);
	printf("\nx\n");
	pvecf(p,x);
}
