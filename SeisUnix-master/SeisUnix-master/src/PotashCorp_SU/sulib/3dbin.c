#include<stdio.h>
#include"su.h"


void bin(double x0, double y0,double  xp, double yp, float degr, float dbx, 
	float dby, int dirx, int diry, 
	int *binx, int *biny, double *binxc, double *binyc,float *fbinx, float *fbiny)
/* Input
	grid[0][0] centre coordinate x0,y0
	coordinate of the point to grid xp,yp
	rotation of the grid relative to the coordinat system in radian degr
	bin x width
	bin y width
	direction of numbering in x directin and y direction of bins, dirx,diry
	
   return
   	bin number in x direction and y direction, binx, biny
	coordinate of bin centre binxc binyc
*/

{
	double xr, yr, xt , yt;
	
	/* locationof the point in bin coordinate system */	
	xr = (xp - x0)*cos(-degr) + (yp - y0)*sin(-degr);
	yr = (yp - y0)*cos(-degr) - (xp - x0)*sin(-degr);
		
	/* edge of bin 1,1 */
	xt = -((double)dbx/2.0); yt = -((double)dby/2.0);
	*binx = 1; *biny = 1;
	
	
	if(dirx*xr <= xt) err(" Error in bin# 1,1 x coordinate dirx=%d xr=%f xt=%f\n",dirx,xr,xt);
	while(dirx*xr > xt+(double)dbx){ 
		xt +=(double)dbx;
		(*binx)++;
	} 

	if(diry*yr <= yt) err(" Error in bin# 1,1 y coordinate diry=%d yr=%f yt=%f\n",diry,yr,yt);
	while(diry*yr > yt+(double)dby){ 
		yt +=(double)dby;
		(*biny)++;
	} 

	/* centre coordinate of bin 1,1 is 1.5 1.5 in floating point bin coordinte system */	
	*fbinx=1.5+(float)(xr/(double)dbx);
	*fbiny=1.5+(float)(yr/(double)dby);
	
	*binxc = x0 + dirx*(*binx-1)*dbx*cos(degr) +
			diry*(*biny-1)*dby*sin(degr);
	*binyc = y0 + diry*(*biny-1)*dby*cos(degr) -
			dirx*(*binx-1)*dbx*sin(degr);
}			
