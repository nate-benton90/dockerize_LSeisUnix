/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NXIN 10
#define NXOUT 40

main()
{
	int ixin,ixout;
	float dxin=2.0,fxin=0.0,dxout=1.0,fxout=-9.0;
	float xout[NXOUT],x,youtr[NXOUT],youti[NXOUT];
	complex yin[NXIN],yout[NXOUT],czero=cmplx(0.0,0.0);

	for (ixin=0; ixin<NXIN; ixin++)
		yin[ixin] = cmplx(1.0,-1.0);
	for (ixout=0,x=fxout; ixout<NXOUT; ixout++,x+=dxout)
		xout[ixout] = x;
	ints8c(NXIN,dxin,fxin,yin,yin[0],yin[NXIN-1],NXOUT,xout,yout);
	for (ixout=0; ixout<NXOUT; ixout++) {
		youtr[ixout] = yout[ixout].r;
		youti[ixout] = yout[ixout].i;
	}
	pp1d(stdout,"constant extrapolation real",NXOUT,0,youtr);
	pp1d(stdout,"constant extrapolation imag",NXOUT,0,youti);
	getchar();
	ints8c(NXIN,dxin,fxin,yin,czero,czero,NXOUT,xout,yout);
	for (ixout=0; ixout<NXOUT; ixout++) {
		youtr[ixout] = yout[ixout].r;
		youti[ixout] = yout[ixout].i;
	}
	pp1d(stdout,"zero extrapolation real",NXOUT,0,youtr);
	pp1d(stdout,"zero extrapolation imag",NXOUT,0,youti);
}
