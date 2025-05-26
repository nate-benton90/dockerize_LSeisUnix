/*********************** self documentation **********************/
/*****************************************************************************

NNW2DI - Nearest neighbour weigthed 2D interpolation

******************************************************************************
Function Prototype:
float nnw2di (int n, float x[], float y[], float z[],
             int nx, int ny, float fx, float fy, float dx,float dy,float **data,
	     float p);

******************************************************************************
Input:
n		number of values in x[],y[],and z[]
x		array of n x values
y		array of n y values
z		array of n z values
nx		output data matrix x dimension
ny		outpur data matrix y dimension
fx		x value of first element of data matrix
fy		y value of first element of data matrix
dx		increment in x direction
dy		increment in y direction
p		distance weigthing factor ; typically p (1-10)
r		search radius

Output:
data		nx x ny data matrix with the interpolated values

returns the RMS misfit
******************************************************************************
*/

#include "suhdr.h"

float nnw2di (int n, float x[], float y[], float z[],
             int nx, int ny, float fx, float fy, float dx,float dy,float **data,
	     float p,float r)
{
float dist(float x0, float y0,float x, float y);
	int ix;
	int iy;
	int iv;
	float d=0.0;
	float xi;
	float yi;
	float num;
	float denm;
	float wd;
	float errf=0.0;
	float z_true=0.0;
	float arg;
	int flag=0;
	/*float cnst=1.0/(p*sqrt(2.0*PI)); */
	
	for(ix=0;ix<nx;ix++) {
		xi = ix*dx+fx;	
		for(iy=0;iy<ny;iy++) {
			yi = iy*dy+fy;
			num=0.0;
			denm=0.0;
			flag=0;
			for(iv=0;iv<n;iv++) {
				d=dist(xi,yi,x[iv],y[iv]);
				arg=(d*d)/(p*p);
				if (arg<5.0) {
					wd=safe_exp(-arg);
					num+=z[iv]*wd;
					denm+=wd;
				/* save z for error computing */
					if(d==0.0) {
						z_true=z[iv];
						flag=1;
					}
				}
				/*if(d<r) {
					if(!CLOSETO(d,0.0)) {
						wd=(float)pow((double)d,(double)p);
						if(wd>0.0) {
							num+=z[iv]/wd;
							denm+=1.0/wd;
						}
					} else {
						zi=z[iv];
						f=1;
						break;
					} 
				} 
			}	
			 if(f) {
				data[ix][iy]=zi;
			} else {
				if(denm >0.0) {
					data[ix][iy]=num/denm;
				} else {
					data[ix][iy]=0.0;
				}
			} */
			}
			if(denm >0.0) {
				data[ix][iy]=num/denm;
			} else {
				data[ix][iy]=0.0;
			}
			
			if (flag==1) {
				errf+=SQR(data[ix][iy]-z_true);
			}
				
		}
	}
	errf=sqrt(errf/n);
	return(errf);		
}

float dist(float x0, float y0,float x,float y)
{
	float dx;
	float dy;
	
	dx=x0-x;
	dy=y0-y;
	
	return(sqrt(dx*dx+dy*dy));
}		
