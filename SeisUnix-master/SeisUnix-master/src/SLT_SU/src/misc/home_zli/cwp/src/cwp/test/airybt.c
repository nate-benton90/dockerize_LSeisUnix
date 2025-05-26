/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NMAX 1000
float bi[NMAX];

main()
{
	int nx,i;
	float xmin,xmax,x,dx;

	while(1) {
		/*
		printf("Enter nx xmin xmax: \n");
		*/
		scanf("%d %f %f",&nx,&xmin,&xmax);

		for (i=0,x=xmin,dx=(xmax-xmin)/(nx-1); i<nx; i++,x+=dx)
			bi[i] = airyb(x);
		/*
		pp1d(stdout,"",nx,0,bi);
		*/
		fwrite(bi,sizeof(float),nx,stdout);
		fflush(stdout);
		break;
	}
}
