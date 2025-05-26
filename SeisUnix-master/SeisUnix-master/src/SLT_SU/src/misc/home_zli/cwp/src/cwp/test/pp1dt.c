/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

main()
{
	int lx=21,ifx=-10,ix;
	float x[100];

	for (ix=0; ix<lx; ix++)
		x[ix] = fsinc(0.2*(ix+ifx));

	pp1d(stdout,"sinc function",lx,ifx,x);
}
