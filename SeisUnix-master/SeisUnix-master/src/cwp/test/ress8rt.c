#include "cwp.h"

#define NXIN 10
#define NXOUT 20

main()
{
	int ixin;
	float dxin=2.0,fxin=0.0,dxout=4.0,fxout=-5.0;
	float yin[NXIN],yout[NXOUT];

	for (ixin=0; ixin<NXIN; ixin++)
		yin[ixin] = 1.0;
	ress8r(NXIN,dxin,fxin,yin,yin[0],yin[NXIN-1],NXOUT,dxout,fxout,yout);
	pplot1(stdout,"constant extrapolation",NXOUT,yout);
	getchar();
	ress8r(NXIN,dxin,fxin,yin,0.0,0.0,NXOUT,dxout,fxout,yout);
	pplot1(stdout,"zero extrapolation",NXOUT,yout);
}
