/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NX 10
#define NY 10

main()
{
	int ix,iy;
	float dx=1.0,fx=0.0,dy=1.0,fy=0.0,xylo=0.0,xyhi=0.0;
	float y[NX],x[NY];

	for (ix=0; ix<NX; ix++)
		y[ix] = ix;
	pp1d(stdout,"y(x)",NX,0,y);

	yxtoxy(NX,dx,fx,y,NY,dy,fy,xylo,xyhi,x);
	pp1d(stdout,"x(y)",NY,0,x);
}
