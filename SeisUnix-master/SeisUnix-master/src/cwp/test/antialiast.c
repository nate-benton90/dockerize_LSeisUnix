#include "cwp.h"

#define N 50

main()
{
	int n=N,i;
	float frac=0.5;
	float p[N],q[N];

	for (i=0; i<N; i++)
		p[i] = 0.0;
	p[N/2] = 1.0;
	antialias(frac,0,n,p,q);
	pplot1(stdout,"zero-phase",n,q);
	getchar();
	antialias(frac,1,n,p,q);
	pplot1(stdout,"minimum-phase",n,q);
}
