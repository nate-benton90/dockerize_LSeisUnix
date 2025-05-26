#include "cwp.h"

#define NXIN 10
#define NXOUT 20

main()
{
	int ixin;
	float dx=2.0,fxin=0.0,fxout=-5.0;
	float yin[NXIN],yout[NXOUT],x;

	for (ixin=0; ixin<NXIN; ixin++)
		yin[ixin] = 1.0;
	shfs8r(dx,NXIN,fxin,yin,yin[0],yin[NXIN-1],NXOUT,fxout,yout);
	pp1d(stdout,"constant extrapolation",NXOUT,0,yout);
	getchar();
	shfs8r(dx,NXIN,fxin,yin,0.0,0.0,NXOUT,fxout,yout);
	pp1d(stdout,"zero extrapolation",NXOUT,0,yout);
}
