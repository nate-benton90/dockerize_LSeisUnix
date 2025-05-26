/* test LINPACK sge functions */

#include "cwp.h"

void pvecf(int n, float *v)
{
	int i;
	for (i=0; i<n; i++)
		printf("v[%d] = %g\n",i,v[i]);
}

#define N 2

main()
{
	int i,j,*ipvt,info,n=N;
	float **a,*b,*z,rcond;

	a = (float**)alloc2(n,n,sizeof(float));
	ipvt = (int*)alloc1(n,sizeof(int));
	b = (float*)alloc1(n,sizeof(float));
	z = (float*)alloc1(n,sizeof(float));

	/* Examples 3.6 & 3.7 from Kahaner, Moler, and Nash */
	a[0][0] = 9.7;	a[1][0] = 6.6;
	a[0][1] = 4.1;	a[1][1] = 2.8;
	b[0] = 9.70;	b[1] = 4.11;

/*
	sgefa(a,n,ipvt,&info);
	printf("info = %d\n",info);
*/
	
	sgeco(a,n,ipvt,&rcond,z);
	printf("rcond = %g\n",rcond);
	printf("condition number estimate = %g\n",1.0/rcond);
	printf("number of significant figures = %d\n",
		(int)log10(rcond/FLT_EPSILON));

	sgesl(a,n,ipvt,b,0);
	pvecf(n,b);
}
