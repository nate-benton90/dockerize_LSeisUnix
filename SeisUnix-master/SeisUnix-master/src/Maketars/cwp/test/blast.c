#include "cwp.h"

void pvec(int n, float *v)
{
	int i;
	printf("v =");
	for (i=0; i<n; i++)
		printf(" %g",v[i]);
	printf("\n");
}

#define N 10
float sx[N] = {0,1,2,3,4,5,6,7,8,9};
float sy[N] = {-1,1,-1,1,-1,1,-1,1,-1,1};

main()
{
	int i,n=N;

	printf("isamax = %d\n",isamax(n,sx,1));
	printf("isamax = %d\n",isamax(n/2,sx,2));
	printf("isamax = %d\n",isamax(n,sy,1));

	printf("sasum = %g\n",sasum(n,sx,1));
	printf("sasum = %g\n",sasum(n/2,sx,2));
	printf("sasum = %g\n",sasum(n,sy,1));

	printf("snrm2 = %g\n",snrm2(n,sx,1));
	printf("snrm2 = %g\n",snrm2(n/2,sx,2));
	printf("snrm2 = %g\n",snrm2(n,sy,1));

	printf("sdot = %g\n",sdot(n,sx,1,sy,1));
	printf("sdot = %g\n",sdot(n/2,sx,2,sy,2));
	printf("sdot = %g\n",sdot(n/2,sx,-2,sy,2));
	printf("sdot = %g\n",sdot(n,sy,1,sy,1));

	printf("sscal\n");
	sscal(n,2.0,sx,1);
	pvec(n,sx);
	sscal(n,0.5,sx,1);
	pvec(n,sx);
	sscal(n/2,2.0,sx,2);
	pvec(n,sx);
	sscal(n/2,0.5,sx,2);
	pvec(n,sx);

	printf("sswap\n");
	sswap(n,sx,1,sy,1);
	pvec(n,sx); pvec(n,sy);
	sswap(n,sy,1,sx,1);
	pvec(n,sx); pvec(n,sy);
	sswap(n/2,sx,1,sx+n/2,-1);
	pvec(n,sx);
	sswap(n/2,sx,1,sx+n/2,-1);
	pvec(n,sx);
	sswap(n/2,sx,2,sy,2);
	pvec(n,sx); pvec(n,sy);
	sswap(n/2,sx,2,sy,2);
	pvec(n,sx); pvec(n,sy);

	printf("saxpy\n");
	saxpy(n,2.0,sx,1,sy,1);
	pvec(n,sx); pvec(n,sy);
	saxpy(n,-2.0,sx,1,sy,1);
	pvec(n,sx); pvec(n,sy);
	saxpy(n/2,2.0,sx,2,sy,2);
	pvec(n,sx); pvec(n,sy);
	saxpy(n/2,-2.0,sx,2,sy,2);
	pvec(n,sx); pvec(n,sy);
	saxpy(n/2,2.0,sx,-2,sy,1);
	pvec(n,sx); pvec(n,sy);
	saxpy(n/2,-2.0,sx,-2,sy,1);
	pvec(n,sx); pvec(n,sy);

	printf("scopy\n");
	scopy(n/2,sx,2,sy,2);
	pvec(n,sx); pvec(n,sy);
	scopy(n/2,sx+1,2,sy+1,2);
	pvec(n,sx); pvec(n,sy);
	scopy(n/2,sx,2,sy,1);
	pvec(n,sx); pvec(n,sy);
	scopy(n/2,sx+1,-2,sy+n/2,-1);
	pvec(n,sx); pvec(n,sy);
}
