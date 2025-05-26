/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include <stdio.h>

main()
{
	int i;
	float rscale=1.0/2147483648.0,r;

	for (i=0; i<4000; i++)
	{
		r = (float)(rand()*rscale);
		fwrite(&r,sizeof(float),1,stdout);
	}
}
