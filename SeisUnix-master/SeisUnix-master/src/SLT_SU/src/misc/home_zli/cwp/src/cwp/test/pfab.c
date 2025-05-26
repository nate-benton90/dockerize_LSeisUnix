/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NMIN 100
#define NMAX 5000
#define NSTEP 100
#define NT 5
complex z[NMAX];

main()
{
	int n=NMIN,nfft,i,it;
	float cpucc;

	for (i=0; i<NMAX; i++)
		z[i] = cmplx(0.0,0.0);

	for (n=NMIN; n<=NMAX; n+=NSTEP) {
		nfft = npfa(n);
		cpucc = cpusec();
		for (it=0; it<NT; it++)
			pfacc(1,nfft,z);
		cpucc = cpusec()-cpucc;
		cpucc /= NT;
		printf("n = %d  nfft = %d  sec = %f\n",n,nfft,cpucc);
	}
}
