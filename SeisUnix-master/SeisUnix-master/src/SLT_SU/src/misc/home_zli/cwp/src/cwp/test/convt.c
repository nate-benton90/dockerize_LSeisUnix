/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define LX 5
#define LY 8
#define LZ LX+LY-1
#define IFX 0
#define IFY 0
#define IFZ 0

float x[LX],y[LY],z[LZ];

main()
{
	int i;
	float one=1.0,mone=-1.0;

	scopy(LX,&one,0,x,1);
	scopy(LY,&mone,0,y,1);
	conv(LX,IFX,x,LY,IFY,y,LZ,IFZ,z);
	pp1d(stdout,"convolution",LZ,IFZ,z);
}
