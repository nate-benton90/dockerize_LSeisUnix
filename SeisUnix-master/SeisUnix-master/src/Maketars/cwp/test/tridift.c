#include "cwp.h"

#define N 10
int ipvt[N];
float x[N],r[N],u[N],a[N],b[N],c[N];

main()
{
	int i,j,n=N,info;
	float **aa; 
	
	aa = alloc2float(n,n);
	for (i=0; i<n; i++)
		for (j=0; j<n; j++)
			aa[i][j] = 0.0;
	
	for (i=0; i<n; i++) {
		aa[i][i] = b[i] = 10*(i+1);
		if (i>0) aa[i][i-1] = c[i-1] = i-1;
		if (i<n-1) aa[i][i+1] = a[i+1] = i+1;
		x[i] = r[i] = i;
	}

	sgefa(aa,n,ipvt,&info);
	sgesl(aa,n,ipvt,x,0);
	
	tridif(n,a,b,c,r,u);
	
	for (i=0; i<n; i++)
		printf("x[%d] = %g  u[%d] = %g\n",i,x[i],i,u[i]);
}
