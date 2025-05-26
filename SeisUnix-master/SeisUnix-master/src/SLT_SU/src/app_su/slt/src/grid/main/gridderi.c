/* GRIDDERI grid derivative program */
#include "grid.h"
#include "gridhd.h"
#include "par.h"

char *sdoc = 
"GRIDDERI - grid derivative program					\n"
"\n"
"gridderi [parameters] <grid.input >grid.output			\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"order=1        order of the derivative (1 or 2)      \n"
"\n"
"Note: \n"
"   output is the n-th order (given by parameter order) derivative of \n"
"   input with respect to first axis of the grid \n" 
"\n"
"AUTHOR:		Zhiming Li,       ,	1/14/98   		\n"    
;

int main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

   	int n1,n2,n3,n4,n5;
   	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
	float scale; 
	int dtype;
	float ocdp2,dcdp2,oline3,dline3,gmin,gmax;
	int orient, gtype=0;
	
	int i1,i2;

	int ierr, order;
	float *grid, *gd, s;

	ghed gh;

   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	/* get input grid parameters */
   	ierr = fgetghdr(infp,&gh);
	if(ierr==0) {
		fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
               	 		&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                		&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
				&orient,&gtype);
	} else {
		err(" use program transp for nonstandard grid file"); 
	}

	/* get parameters */
	if (!getparint("order", &order)) order = 1;

	if(n2==0) n2=1;
	if(n3==0) n3=1;
	if(n4==0) n4=1;
	if(n5==0) n5=1;

	if(order==1) {
		s = 0.5;
		if(d1!=0.) s = 0.5/d1;
	} else if(order==2) {
		s = 1.;
		if(d1!=0.) s = 1./d1/d1;
	} else {
		err(" invalid order %d \n",order); 
	}

	grid = (float*) malloc(n1*sizeof(float));
	gd = (float*) malloc(n1*sizeof(float));

	for(i2=0;i2<n2*n3*n4*n5;i2++) {
		efread(grid,sizeof(float),n1,infp);
		if(order==1) {
			for(i1=1;i1<n1-1;i1++) {
				gd[i1] = (grid[i1+1] - grid[i1-1])*s;
			}
			gd[0] = gd[1];
			gd[n1-1] = gd[n1-2];
		} else {
			for(i1=1;i1<n1-1;i1++) {
				gd[i1] = (grid[i1+1] - 2. * grid[i1] + grid[i1-1])*s;
			}
			gd[0] = gd[1];
			gd[n1-1] = gd[n1-2];
		}
		if(i2==0) { gmin = gd[0]; gmax = gd[0]; }
		for(i1=0;i1<n1;i1++) {
			if(gmin>gd[i1]) gmin = gd[i1];
			if(gmax<gd[i1]) gmax = gd[i1];
		}
		efwrite(gd,sizeof(float),n1,outfp);
	}

	fflush(outfp);
	toghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
    	&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
       	&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
		&orient,&gtype);
	ierr = fputghdr(outfp,&gh);
	if(ierr!=0) warn(" fputghdr error \n");

	free(grid);
	free(gd);

	exit(0);
}
