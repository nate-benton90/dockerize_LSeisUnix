#include "cwp.h"

/* symmetric oscillating * /
#define NXIN 5
#define NXOUT 101
float xin[NXIN] = {0.0, 1.0, 2.0, 3.0, 4.0};
float yin[NXIN] = {0.0, 1.0, -1.0, 1.0, 0.0};
float dxout=0.06;
float fxout=-1.0;
/**/

/* RPN 14 * /
#define NXIN 9
#define NXOUT 101
float xin[NXIN] = {7.99, 8.09, 8.19, 8.7, 9.2, 
	10.0, 12.0, 15.0, 20.0};
float yin[NXIN] = {0, 2.76429e-5, 4.37498e-2, 0.169183, 0.469428,
	0.943740, 0.998636, 0.999919, 0.999994};
float dxout=0.12;
float fxout=8.0;
/**/

/* Akima 3 */
#define NXIN 11
#define NXOUT 101
float xin[NXIN] = {15,14,12,11,9,8,6,5,3,2,0};
float yin[NXIN] = {85,60,50,15,10.5,10,10,10,10,10,10};
float dxout=0.15;
float fxout=0.0;
/**/

main()
{
	int ixin,ixout;
	float ydin[NXIN][4],xout[NXOUT],yout[NXOUT],x;

	for (ixout=0,x=fxout; ixout<NXOUT; ixout++,x+=dxout)
		xout[ixout] = x;
	
	csplin(NXIN,xin,yin,ydin);
	intcub(0,NXIN,xin,ydin,NXOUT,xout,yout);
	/* fwrite(yout,sizeof(float),NXOUT,stdout); */
	pp1d(stdout,"Cubic spline",NXOUT,0,yout);

	cmonot(NXIN,xin,yin,ydin);
	intcub(0,NXIN,xin,ydin,NXOUT,xout,yout);
	/* fwrite(yout,sizeof(float),NXOUT,stdout); */
	pp1d(stdout,"Fritsch-Carlson method",NXOUT,0,yout);

	cakima(NXIN,xin,yin,ydin);
	intcub(0,NXIN,xin,ydin,NXOUT,xout,yout);
	/* fwrite(yout,sizeof(float),NXOUT,stdout); */
	pp1d(stdout,"Akima's method",NXOUT,0,yout);
}
