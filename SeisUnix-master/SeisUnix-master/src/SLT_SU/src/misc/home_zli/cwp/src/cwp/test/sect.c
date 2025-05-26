/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

main()
{
	int i,n=10000000;
	float cputime,walltime,a=0.0,b=1.0;

	cputime = cpusec();
	for (i=0,a=0.0; i<n; i++)
		a = a+b;
	cputime = cpusec()-cputime;
	printf("a = %f  cpu time = %f\n",a,cputime);
	walltime = wallsec();
	for (i=0,a=0.0; i<n; i++)
		a = a+b;
	walltime = wallsec()-walltime;
	printf("a = %f  wall time = %f\n",a,walltime);
}
