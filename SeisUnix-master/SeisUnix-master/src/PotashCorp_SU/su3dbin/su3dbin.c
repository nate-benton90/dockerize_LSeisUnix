/* su3dbin.c */
/* B.Nemeth */


#include "suhdr.h"

/*********************** self documentation *****************************/
char *sdoc[] = {"su3dbin < infile > outfile                             ",
" SU3DBIN - 3D binning program 						",
" 			 						",
" su3dbin <stdin >stdout [parameters]					",
" 			 						",
" Required  parameters:	 						",
" 	xc=1			xcoordinate of the corner bin centre	",
" 	yc=1                    ycoordinate of the corner bin centre	",
" 	dbx=20			bin size in x direction			",
" 	dby=20			bin size in y direction			",
" 	dirx=1			direction of bin numbering in x  dir.   ",
" 			 	-1=right to left; other=left to right	",
" 	diry=1	                direction of bin numbering in Y dir.	",
" 			 	-1=up to down; other= down to up     	",
" 			 						",
" 	deg=0		 	rotation of the bin-grid relative to the",
" 			 	coordinate system in degrees		",
" 									",
" 									",
" 	cc=1								",
" 									",
" 	if cc=1								",
" 									",
" 	Header words accessed:	tr.sx shot x coord.			",
" 				tr.sy shot y coord.			",
" 				tr.swdep receiver x ccordinate !!!!!	",
" 				tr.gwdep receiver y coordinate !!!!!	",
" 	Header words modified:						",
" 				tr.cdp cdp composite number(binx*1000+biny) ",
" 				tr.gx cdpbin centre x coordinate	",
" 				tr.gy cdpbin centre y coordinate	",
" 									",
" 	if cc=0								",
" 									",
" 	Header words accessed:	tr.gx cdp  x coord.			",
" 				tr.gy cdp  y coord.			",
" 									",
" 									",
" 	Header words modified:						",
" 									",
" 	If ix=0	(Default)    						",
" 									",
" 				tr.cdp cdp composite number(binx*1000+biny) ",
" 	if ix=1								",
" 				tr.cdp=binx				",
" 				tr.ep=biny				",
" 									",
NULL};
   
/* Segy data constans */
segy tr;				/* SEGY trace */
float *data;				/* trace data */

int main( int argc, char *argv[] )
{
	/* Segy data constans */
	segy tr;		/* SEGY trace */
        int ntr=0;              /* number of traces           */
	int flag;		/* counter */
	float xc, yc;
	double xe, ye;
	float dbx, dby;
	int dirx,diry;
	float deg,degr;
	double binxc,binyc;
	int binx,biny;
	int verbose;
	
	double sx,sy,gx,gy;
	double cmpx,cmpy;
	float dummy1,dummy2;
	
	int cc,ix;
	
	
	
	initargs(argc, argv);
   	requestdoc(1);
	

	if(!getparfloat("xc",&xc)) xc=1;
	if(!getparfloat("yc",&yc)) yc=1;
	if(!getparfloat("dbx",&dbx)) dbx=20;
	if(!getparfloat("dby",&dby)) dby=20;
	if(!getparfloat("deg",&deg)) deg=0;
	degr = 3.141592653/180 * deg;
	
	if(!getparint("dirx",&dirx)) dirx=1;
	if(!getparint("diry",&diry)) diry=1;
	if(!getparint("cc",&cc)) cc=1;
	if(!getparint("verbose",&verbose)) verbose=0;
	if(!getparint("ix",&ix)) ix=0;

	/* centre of bin# 1,1 */
	
/*	xe = xc - dirx*dbx/2.0*cos(degr) - diry*dby/2.0*sin(degr);
	ye = yc - diry*dby/2.0*cos(degr) + dirx*dbx/2.0*sin(degr); */
	xe = xc;
	ye = yc;

	/* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");	
	
	do {
		ntr++;
		flag=0;
		
		if (cc==1) {
			/* receiver and source coordinates */
			sx = (double)tr.sx*pow((double)10.0,(double)tr.scalco);
			sy = (double)tr.sy*pow((double)10.0,(double)tr.scalco);
			gx = (double)tr.swdep*pow((double)10.0,(double)tr.scalco);
			gy = (double)tr.gwdep*pow((double)10.0,(double)tr.scalco);
			/* cmp coordinate, halfway between s and g */
			cmpx = sx + (gx-sx)/2.0;
			cmpy = sy + (gy-sy)/2.0;
		} else {			
			cmpx =(double)tr.gx*pow((double)10.0,(double)tr.scalco); 
			cmpy =(double)tr.gy*pow((double)10.0,(double)tr.scalco);
		} 
		bin(xe,ye,cmpx,cmpy,degr,dbx,dby,dirx,diry,
			&binx,&biny,&binxc,&binyc,&dummy1,&dummy2);
			
		if(verbose) fprintf(stderr," %d %10.0f %10.0f \n", binx*1000+biny,
		        binxc,binyc);
		if(!ix) {
			tr.cdp = binx*1000+biny;
		} else {
			tr.cdp=binx;
			tr.ep=biny;
		}
		tr.gx = NINT(binxc/pow((double)10.0,(double)tr.scalco));
		tr.gy = NINT(binyc/pow((double)10.0,(double)tr.scalco));
		puttr(&tr);
	} while(gettr(&tr));
	
   return EXIT_SUCCESS;
}
