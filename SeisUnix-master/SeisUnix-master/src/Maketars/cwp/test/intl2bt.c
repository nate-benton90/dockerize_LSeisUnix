/* test intl2b function */

#include "cwp.h"

main()
{
	int nxin,nyin,nxout,nyout,ixout,iyout;
	float dxin,fxin,dyin,fyin,fxout,dxout,fyout,dyout;
	signed char zin[2][2],zout[3][3];
	
	zin[0][0] = -128;	zin[0][1] = 0;
	zin[1][0] = 0;		zin[1][1] = 127;
	nxin=2;  dxin=1.0;  fxin=0.0;
	nyin=2;  dyin=1.0;  fyin=0.0;
	nxout=3;  dxout=dxin*(nxin-1)/(nxout-1);  fxout=0.0;
	nyout=3;  dyout=dyin*(nyin-1)/(nyout-1);  fyout=0.0;
	intl2b(nxin,dxin,fxin,nyin,dyin,fyin,&zin[0][0],
		nxout,dxout,fxout,nyout,dyout,fyout,&zout[0][0]);
	for (iyout=0; iyout<nyout; iyout++)
		for (ixout=0; ixout<nxout; ixout++)
			printf("zout[%d][%d] = %d\n",
				iyout,ixout,zout[iyout][ixout]);
}
