#include "cwp.h"

#define NXIN 10
#define NXOUT 40

main()
{
	int ixin,ixout;
	float dxin=2.0,fxin=0.0,dxout=1.0,fxout=-9.0;
	float yin[NXIN],xout[NXOUT],yout[NXOUT],x;

	for (ixin=0; ixin<NXIN; ixin++)
		yin[ixin] = 1.0;
	for (ixout=0,x=fxout; ixout<NXOUT; ixout++,x+=dxout)
		xout[ixout] = x;
	ints8r(NXIN,dxin,fxin,yin,yin[0],yin[NXIN-1],NXOUT,xout,yout);
	pp1d(stdout,"constant extrapolation",NXOUT,0,yout);
	getchar();
	ints8r(NXIN,dxin,fxin,yin,0.0,0.0,NXOUT,xout,yout);
	pp1d(stdout,"zero extrapolation",NXOUT,0,yout);
}
