/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.                       */

#include <stdio.h>
#include <math.h>

#define N 4000

main()
{
	int i;
	float r,a=1.0/(2*N);

	for (i=0; i<N; i++)
	{
		r = sin(a*i*i);
		fwrite(&r,sizeof(float),1,stdout);
	}
}
