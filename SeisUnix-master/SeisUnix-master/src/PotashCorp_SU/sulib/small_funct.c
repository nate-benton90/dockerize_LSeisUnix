#include <stdio.h>
#include <math.h>
#include "suhdr.h"

float distance(float x0, float y0, float x1, float y1)
/* return the distance between two points
*/
{
	return(sqrt(SQR(x1-x0)+SQR(y1-y0)));
}

float slope(float x0, float y0, float x1, float y1)
/* return the slope of the line connecting x,y x1,y1 */
/* if the lines is parallel with the y axis the function returns +/-Inf */
{
	float dx,dy;
	
	dx=x1-x0;
	dy=y1-y0;
	
		return(dy/dx);
}	

void p2line_pr(float x,float y,float m,float b,float *xp,float *yp)
/* project a point on to a line perpendicularly
   x, y point coordinates to project
   m slope of the line
   b intercept of the line
   *xp,*yp - projected coordinates
*/
{

	float bp;
	float mp;
	
	if(!isinf(m)) {
		if(CLOSETO(m,0.0)) {
		/* line is paralell with the x axis */
			*xp=x;
			*yp = b;
			return;
		}
			
		/* projection line */
		mp = -m;
		bp = y - mp*x;
	
		*xp = (bp-b)/(m-mp);
		*yp = mp**xp+bp;
		return;
		
	} else {
	/* the line is paralell with the y axis */
		*yp = y;
		*xp = b;
		return;
	}
}

int isamaxs(int n, float *a,int inc)
{
	int im=0,i;
	for(i=1;i<n;i+=inc)
		if(a[im]< a[i]) im=i; 
	return(im); 
}

int isamins(int n, float *a,int inc)
{
	int im=0,i;
	for(i=1;i<n;i+=inc)
		if(a[im]> a[i]) im=i;
	return(im); 
}

float safe_exp(float a)
{
	float b;
	
	b = (float)exp((double)a);
	if(finite(b))
		return(b);
	else
		return(0.0);
}
   
