/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NMIN 500
#define NMAX 550
#define NMAX2 (NMAX+2)*(NMAX+2)
complex cz[NMAX2];

main()
{
	int n,nc,nr,i;
	float *rz=(float*)cz;
	float cpucc,cpurc;

	for (nc=npfa(NMIN),nr=npfar(nc);
		nc<NMAX && nr<NMAX;
		nc=npfa(nc+1),nr=npfar(nc)) {

		for (i=0; i<nc*nc; i++)
			cz[i] = cmplx(0.0,0.0);
		cpucc = cpusec();
		pfa2cc(1,1,nc,nc,cz);
		cpucc = cpusec()-cpucc;

		for (i=0; i<nr*nr; i++)
			rz[i] = 0.0;
		cpurc = cpusec();
		pfa2rc(1,1,nr,nr,rz,cz);
		cpurc = cpusec()-cpurc;

		printf("nc,nr,cc,rc,cc/rc = %d %d %f %f %f\n",
			nc,nr,cpucc,cpurc,cpucc/cpurc);
	}
}
