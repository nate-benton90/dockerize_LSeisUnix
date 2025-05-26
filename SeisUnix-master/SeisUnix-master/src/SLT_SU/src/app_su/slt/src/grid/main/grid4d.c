/* GRID4D grid 4d scale program */
#include "usgrid.h"
#include "par.h"

char *sdoc = 
"GRID4D - 4D (t,offset,x,y) constant-offset interpolation program	\n"
"\n"
"grid4d [parameters] >grid.output		\n" 
"\n"
"Required parameters:							\n"
"grid=             name(s) of 2D input grid(s) (t,offset) at location x,y \n"
"x=                x location of the grid \n"
"y=                y location of the grid \n"
"ngrid=            number of input 2D grids		\n"
"                  grid=grid1 x=x1 y=y1 		\n"
"                  grid=grid2 x=x2 y=y2 		\n"
"                  grid=grid3 x=x3 y=y3 		\n"
"                  ...							\n"
"grid.out=         name of the output 4D grid (t,x,y,offset)	\n"
"fx=               minimum x of output grid 			\n"
"fy=               minimum y of output grid 			\n"
"dx=               x increment of output grid 			\n"
"dy=               y increment of output grid 			\n"
"nx=               number of x points of output grid 			\n"
"ny=               number of y points of output grid 			\n"
"Optional parameters:							\n"
"nf=0              number of closest grid functions used to interpolate \n"
"                  output grid					\n"
"                  0=use all the functions whose distance from the output \n"
"                  is less than dismax			\n"
"dismax=999999     maximum distance to use input grid functions for \n"
"                  interpolating output 		\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	12/20/96   \n"
;

int main(int argc, char **argv)
{
	FILE *infp, *outfp=stdout;
	char *grid;
	int ngrid;
	float *xs, *ys, *gridin, *gridout;
	float x, y;
	int nf;
	float fx, fy, dx, dy;
	int nx, ny;
	float *work, dismax;
	int *indx, n12, i1, i2, ig, lpos, ix, iy; 

	float o1, o2;
	float d1, d2;
	int n1, n2;
	float gmin, gmax;

	int ierr;

	usghed usgh;

   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	/* get input parameters */
	if(!getparint("ngrid",&ngrid)) err("must specify ngrid");
	xs = (float*) emalloc(ngrid*sizeof(float));
	ys = (float*) emalloc(ngrid*sizeof(float));

	if(!getnparstring(1,"grid",&grid)) err("must specify grid");
	if(!getnparfloat(1,"x",&x)) err("must specify x");
	if(!getnparfloat(1,"y",&y)) err("must specify y");
	xs[0] = x;
	ys[0] = y;

	if(!getparfloat("fx",&fx)) err( " fx missing "); 
	if(!getparfloat("fy",&fy)) err( " fy missing "); 
	if(!getparfloat("dx",&dx)) err( " dx missing "); 
	if(!getparfloat("dy",&dy)) err( " dy missing "); 
	if(!getparint("nx",&nx)) err( " nx missing "); 
	if(!getparint("ny",&ny)) err( " ny missing "); 
	if(!getparint("nf",&nf)) nf = 0;
	if(!getparfloat("dismax",&dismax)) dismax = 999999;


	/* get input grid parameters */
	infp = efopen(grid,"r");
 	ierr = fgetusghdr(infp,&usgh);
	if(ierr!=0) err(" input grid header error "); 
	n1 = usgh.n1; n2 = usgh.n2; 
	o1 = usgh.o1; o2 = usgh.o2; 
	d1 = usgh.d1; d2 = usgh.d2; 

	/* memory allocation */
	n12 = n1 * n2; 
	gridin = (float*) emalloc(n12*ngrid*sizeof(float));
	work = (float*) malloc(ngrid*sizeof(float));
	indx = (int*) malloc(ngrid*sizeof(int));
	gridout = (float*) emalloc(n12*sizeof(float));

	efseek(infp,0,0);
	efread(gridin,sizeof(float),n1*n2,infp);
	efclose(infp);

	/* read in input grids */
	for(ig=1;ig<ngrid;ig++) {
		if(!getnparstring(ig+1,"grid",&grid)) 
			err("must specify  %d-th grid \n",ig+1);
		infp = efopen(grid,"r");
 		ierr = fgetusghdr(infp,&usgh);
		if(ierr!=0) err(" input grid header error "); 
		if(o1!=usgh.o1) err (" check %d-th grid header o1 \n",ig+1);
		if(o2!=usgh.o2) err (" check %d-th grid header o2 \n",ig+1);
		if(n1!=usgh.n1) err (" check %d-th grid header n1 \n",ig+1);
		if(n2!=usgh.n2) err (" check %d-th grid header n2 \n",ig+1);
		if(d1!=usgh.d1) err (" check %d-th grid header d1 \n",ig+1);
		if(d2!=usgh.d2) err (" check %d-th grid header d2 \n",ig+1);
		efseek(infp,0,0);
		efread(gridin+ig*n1*n2,sizeof(float),n1*n2,infp);
		efclose(infp);
		if(!getnparfloat(ig+1,"x",&x)) 
			err("must specify %d-th x",ig+1);
		if(!getnparfloat(ig+1,"y",&y)) 
			err("must specify %d-th y",ig+1);
		xs[ig] = x;
		ys[ig] = y;
	}

/*
	for(ig=0;ig<ngrid;ig++) {
		fprintf(stderr,"input grid %d: x=%g y=%g \n",
			ig+1,xs[ig],ys[ig]); 
		dump2xplot(gridin+ig*n1*n2,n1,n2,0,"input grid");
	}
*/

	bzero(gridout,n12*sizeof(float));
	for(iy=0;iy<ny;iy++) {
	for(ix=0;ix<nx;ix++) {
		efwrite(gridout,sizeof(float),n12,outfp);
	}
	}

	/* grid interpolation */
	for(iy=0;iy<ny;iy++) {
		y = fy + iy*dy;
		for(ix=0;ix<nx;ix++) {
			x = fx + ix*dx;
			if(nf==0) {
				plint_(xs,ys,gridin,&n12,&ngrid,&x,&y,
						gridout,work,&dismax,indx);
			} else {
				intp2d_(xs,ys,gridin,&n12,&ngrid,&x,&y,
						gridout,&nf,indx,work);
			}
			if(ix==0 && iy==0) {gmin=gridout[0]; gmax=gridout[0];} 

			for(i2=0;i2<n2;i2++) {
				for(i1=0;i1<n1;i1++) {
					if(gmin>gridout[i1+i2*n1]) 
						gmin = gridout[i1+i2*n1];
					if(gmax<gridout[i1+i2*n1]) 
						gmax = gridout[i1+i2*n1];
				}
				lpos = (i2*nx*ny*n1+iy*nx*n1+ix*n1)
					*sizeof(float);
				efseek(outfp,lpos,0);
				efwrite(gridout+i2*n1,sizeof(float),n1,outfp);
			}

		}
	}

	usgh.n1 = n1; 
	usgh.n2 = nx; usgh.o2 = fx; usgh.d2 = dx;
	usgh.n3 = ny; usgh.o3 = fy; usgh.d3 = dy;
	usgh.n4 = n2; usgh.o4 = o2; usgh.d4 = d2;
	usgh.gmin = gmin;
	usgh.gmax = gmax;

 	ierr = fputusghdr(outfp,&usgh);
	if(ierr!=0) err(" output grid header error "); 


	free(xs);
	free(ys);
	free(work);
	free(indx);
	free(gridin);
	free(gridout);
	exit(0);
}
