#include "su.h"
#include "segy.h"
/*********************** self documentation **********************/
char *sdoc[] = {" SUBINS - print bin coordinates to stdout  ",
"							   ",
"	subins 						   ",
"									",
"	Required parameters:						",
"	nx=			grid x dimension			",
"	ny=			grid y dimension			",
"									",
"	Optional  parameters:                                           ",
"	xc=1		       xcoordinate of the corner bin centre     ",
"	yc=1			ycoordinate of the corner bin centre	",
"	dbx=20  		bin size in x direction 		",
"	dby=20  		bin size in y direction 		",
"	dirx=1  		direction of bin numbering in x  dir.	",
"	 			-1=right to left; other=left to right	",
"	diry=1  		direction of bin numbering in Y dir.	",
"	 			-1=up to down; other= down to up	",
"	 								",
"	deg=0			rotation of the bin-grid relative to the",
"				       coordinate system in degrees	",
"									",
NULL};
int main( int argc, char *argv[] )
{
	/* binning */
	float xc;
	float yc;
	double cdpcx;
	double cdpcy;
	float dbx;
	float dby;
	float deg;
	float degr;
	int dirx;
	int diry;
	int nx;
	int ny;
	float xe;
	float ye;
	int ix;
	int iy;

	initargs(argc, argv);
   	requestdoc(1);

	/* binning stuff */
	if(!getparfloat("xc",&xc)) xc=1;
	if(!getparfloat("yc",&yc)) yc=1;
	if(!getparfloat("dbx",&dbx)) dbx=20;
	if(!getparfloat("dby",&dby)) dby=20;
	if(!getparfloat("deg",&deg)) deg=0;
	degr = 3.141592653/180 * deg;
	if(!getparint("dirx",&dirx)) dirx=1;
	if(!getparint("diry",&diry)) diry=1;
	MUSTGETPARINT("nx", &nx);
	MUSTGETPARINT("ny", &ny);

	/* edge of bin# 1,1 */
	xe = xc - dirx*dbx/2.0*cos(degr) - diry*dby/2.0*sin(degr);
	ye = yc - diry*dby/2.0*cos(degr) + dirx*dbx/2.0*sin(degr);

	/* compute bin centre coordinates */
	for(ix=1;ix<=nx; ix++) {
		for(iy=1;iy<=ny;iy++) { 
			cdpcx= xc + dirx*(ix-1)*dbx*cos(degr) +
                        	diry*(iy-1)*dby*sin(degr);
			cdpcy= yc + diry*(iy-1)*dby*cos(degr) -
                        	dirx*(ix-1)*dbx*sin(degr);
			fprintf(stdout," %d %f %f\n",
				ix*1000+iy,cdpcx,cdpcy);
			}
	}
	return EXIT_SUCCESS;
}
