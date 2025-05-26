#include "cwp.h"

#define N 100000
#define NLOOP 1

main()
{
	int i,ip,iloop;
	float p,q1,q2,q3,*x,*y,cputime;

	x = (float *)malloc((sizeof *x)*N);
	y = (float *)malloc((sizeof *y)*N);

	p = 0.50;
	ip = p*N;

	for (i=0; i<N; i++)
		x[i] = franuni();

	cputime = cpusec();
	for (iloop=0; iloop<NLOOP; iloop++)
		q1 = quest(p,N,x);
	cputime = cpusec()-cputime;
	printf("q1 = %g in %g seconds\n",q1,cputime);
	
	cputime = cpusec();
	for (iloop=0; iloop<NLOOP; iloop++) {
		for (i=0; i<N; i++)
			y[i] = x[i];
		qkfind(ip,N,y);
		q2 = y[ip];
	}
	cputime = cpusec()-cputime;
	printf("q2 = %g in %g seconds\n",q2,cputime);
	
	cputime = cpusec();
	for (iloop=0; iloop<NLOOP; iloop++) {
		for (i=0; i<N; i++)
			y[i] = x[i];
		qksort(N,y);
		q3 = y[ip];
	}
	cputime = cpusec()-cputime;
	printf("q3 = %g in %g seconds\n",q3,cputime);
}
