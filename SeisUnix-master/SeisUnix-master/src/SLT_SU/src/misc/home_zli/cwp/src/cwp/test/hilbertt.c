/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define N 100

float x[N],y[N],z[N];

main()
{
	int i;
	float zero=0.0,one=1.0,mone=-1.0;

	scopy(N,&zero,0,x,1);
	for (i=0; i<N; i++)
		x[i] = sin(0.9*PI*i);
	hilbert(N,x,y);
	for (i=0; i<N; i++)
		z[i] = sqrt(x[i]*x[i]+y[i]*y[i]);
	pp1d(stdout,"should be 1.0",N,0,z);
}
