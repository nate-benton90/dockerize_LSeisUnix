/*
FUNCTION:  compute cubic spline interpolation coefficients with
continuous second derivatives

PARAMETERS:
n		i number of samples (n>=2 is required and assumed)
x  		i array of monotonically increasing or decreasing abscissae
y		i array of ordinates
yd		o array of cubic interpolation coefficients (see notes below)

NOTES:
The computed cubic spline coefficients are as follows:
yd[i][0] = y(x[i])    (the value of y at x = x[i])
yd[i][1] = y'(x[i])   (the 1st derivative of y at x = x[i])
yd[i][2] = y''(x[i])  (the 2nd derivative of y at x = x[i])
yd[i][3] = y'''(x[i]) (the 3rd derivative of y at x = x[i])

To evaluate y(x) for x between x[i] and x[i+1] and h = x-x[i],
use the computed coefficients as follows:
y(x) = yd[i][0]+h*(yd[i][1]+h*(yd[i][2]/2.0+h*yd[i][3]/6.0))


AUTHOR:  Dave Hale, Colorado School of Mines, 10/03/89
*/

void csplin (int n, float x[], float y[], float yd[][4])
{
	int i;
	float h1,h2,del1,del2,dmax,hsum,w1,w2,divdf3,sleft,sright,alpha,t;

	/* if n=2, then use linear interpolation */
	if (n==2) {
		yd[0][0] = y[0];  yd[1][0] = y[1];
		yd[0][1] = yd[1][1] = (y[1]-y[0])/(x[1]-x[0]);
		yd[0][2] = yd[1][2] = 0.0;
		yd[0][3] = yd[1][3] = 0.0;
		return;
	}
	
	/* set left end derivative via shape-preserving 3-point formula */
	h1 = x[1]-x[0];
	h2 = x[2]-x[1];
	hsum = h1+h2;
	del1 = (y[1]-y[0])/h1;
	del2 = (y[2]-y[1])/h2;
	w1 = (h1+hsum)/hsum;
	w2 = -h1/hsum;
	sleft = w1*del1+w2*del2;
	if (sleft*del1<=0.0)
		sleft = del1;
	else if (del1*del2<0.0) {
		dmax = 3.0*del1;
		if (ABS(sleft)>ABS(dmax)) sleft = dmax;
	}

	/* set right end derivative via shape-preserving 3-point formula */
	h1 = x[n-2]-x[n-3];
	h2 = x[n-1]-x[n-2];
	hsum = h1+h2;
	del1 = (y[n-2]-y[n-3])/h1;
	del2 = (y[n-1]-y[n-2])/h2;
	w1 = -h2/hsum;
	w2 = (h2+hsum)/hsum;
	sright = w1*del1+w2*del2;
	if (sright*del2<=0.0)
		sright = 0.0;
	else if (del1*del2<0.0) {
		dmax = 3.0*del2;
		if (ABS(sright)>ABS(dmax)) sright = dmax;
	}
	
	/* compute tridiagonal system coefficients and right-hand-side */	
	yd[0][0] = 1.0;
	yd[0][2] = 2.0*sleft;
	for (i=1; i<n-1; i++) {
		h1 = x[i]-x[i-1];
		h2 = x[i+1]-x[i];
		del1 = (y[i]-y[i-1])/h1;
		del2 = (y[i+1]-y[i])/h2;
		alpha = h2/(h1+h2);
		yd[i][0] = alpha;
		yd[i][2] = 3.0*(alpha*del1+(1.0-alpha)*del2);
	}
	yd[n-1][0] = 0.0;
	yd[n-1][2] = 2.0*sright;
	
	/* solve tridiagonal system for slopes */
	t = 2.0;
	yd[0][1] = yd[0][2]/t;
	for (i=1; i<n; i++) {
		yd[i][3] = (1.0-yd[i-1][0])/t;
		t = 2.0-yd[i][0]*yd[i][3];
		yd[i][1] = (yd[i][2]-yd[i][0]*yd[i-1][1])/t;
	}
	for (i=n-2; i>=0; i--)
		yd[i][1] -= yd[i+1][3]*yd[i+1][1];

	/* copy ordinates into output array */
	for (i=0; i<n; i++)
		yd[i][0] = y[i];

	/* compute 2nd and 3rd derivatives of cubic polynomials */
	for (i=0; i<n-1; i++) {
		h2 = x[i+1]-x[i];
		del2 = (y[i+1]-y[i])/h2;
		divdf3 = yd[i][1]+yd[i+1][1]-2.0*del2;
		yd[i][2] = 2.0*(del2-yd[i][1]-divdf3)/h2;
		yd[i][3] = (divdf3/h2)*(6.0/h2);
	}
	yd[n-1][2] = yd[n-2][2]+(x[n-1]-x[n-2])*yd[n-2][3];
	yd[n-1][3] = yd[n-2][3];
}

#include "cwp.h"

/* symmetric oscillating
*/
#define NXIN 5
#define NXOUT 101
float xin[NXIN] = {0.0, 1.0, 2.0, 3.0, 4.0};
float yin[NXIN] = {0.0, 1.0, -1.0, 1.0, 0.0};
float dxout=0.06;
float fxout=-1.0;

/* RPN 14 
#define NXIN 9
#define NXOUT 101
float xin[NXIN] = {7.99, 8.09, 8.19, 8.7, 9.2, 
	10.0, 12.0, 15.0, 20.0};
float yin[NXIN] = {0, 2.76429e-5, 4.37498e-2, 0.169183, 0.469428,
	0.943740, 0.998636, 0.999919, 0.999994};
float dxout=0.12;
float fxout=8.0;
*/

/* Akima 3
#define NXIN 11
#define NXOUT 101
float xin[NXIN] = {15,14,12,11,9,8,6,5,3,2,0};
float yin[NXIN] = {85,60,50,15,10.5,10,10,10,10,10,10};
float dxout=0.15;
float fxout=0.0;
*/

main()
{
	int ixin,ixout;
	float ydin[NXIN][4],xout[NXOUT],yout[NXOUT],x;

	for (ixout=0,x=fxout; ixout<NXOUT; ixout++,x+=dxout)
		xout[ixout] = x;
	
	csplin(NXIN,xin,yin,ydin);
	intcub(0,NXIN,xin,ydin,NXOUT,xout,yout);
	fwrite(yout,sizeof(float),NXOUT,stdout);
	/* pp1d(stdout,"Cubic spline",NXOUT,0,yout); */

	cmonot(NXIN,xin,yin,ydin);
	intcub(0,NXIN,xin,ydin,NXOUT,xout,yout);
	fwrite(yout,sizeof(float),NXOUT,stdout);
	/* pp1d(stdout,"Fritsch-Carlson method",NXOUT,0,yout); */

	cakima(NXIN,xin,yin,ydin);
	intcub(0,NXIN,xin,ydin,NXOUT,xout,yout);
	fwrite(yout,sizeof(float),NXOUT,stdout);
	/* pp1d(stdout,"Akima's method",NXOUT,0,yout); */
}
