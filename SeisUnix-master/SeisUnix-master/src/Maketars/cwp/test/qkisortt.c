/* test qkisort and qkifind */

#include "cwp.h"

#define N 10001

main()
{
	int i,*k,kmedian;
	long int seed=0L;
	float *a;

	a = (float *)malloc(N*sizeof(*a));
	k = (int *)malloc(N*sizeof(*k));
	for (i=0; i<N; i++) {
		seed = (seed*211+1663)%7875;
		a[i] = 1+(N*seed)/7875;
		k[i] = i;
	}

	printf("qkifind begin\n");
	qkifind(N/2,N,a,k);
	printf("qkifind end\n");

	kmedian = k[N/2];

	printf("qkisort begin\n");
	qkisort(N,a,k);
	printf("qkisort end\n");

	if (kmedian==k[N/2]) 
		printf("qkifind successful!\n");
	else
		printf("qkifind failed!\n");

	for (i=1; i<N; i++)
		if (a[k[i]]<a[k[i-1]]) {
			printf("qkisort failed!\n");
			printf("a[%d] = %f  a[%d] = %f\n",
				i-1,a[k[i-1]],i,a[k[i]]);
			exit(-1);
		}
	printf("qkisort successful!\n");
}
