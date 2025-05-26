/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NXIN 10
#define NXOUT 20

main()
{
	int ixin,ixout;
	float dxin=2.0,fxin=0.0,dxout=2.0,fxout=-5.0;
	float xin[NXIN],yin[NXIN],xout[NXOUT],yout[NXOUT],x;

	for (ixin=0,x=fxin+(NXIN-1)*dxin; ixin<NXIN; ixin++,x-=dxin) {
		xin[ixin] = x;
		yin[ixin] = 1.0;
	}
	for (ixout=0,x=fxout; ixout<NXOUT; ixout++,x+=dxout)
		xout[ixout] = x;
	intlin(NXIN,xin,yin,yin[0],yin[NXIN-1],NXOUT,xout,yout);
	pp1d(stdout,"constant extrapolation",NXOUT,0,yout);
	getchar();
	intlin(NXIN,xin,yin,0.0,0.0,NXOUT,xout,yout);
	pp1d(stdout,"zero extrapolation",NXOUT,0,yout);
}
