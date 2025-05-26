char *sdoc =
"REGRID3D - regrid of 3D grid values \n"
"\n"
"regrid3d <in [required parameters] >out \n"
"\n"
"Required Parameters:\n"
"in=        name of the input 3D grid file (default stdin) 	\n"
"out=       name of the output 3D grid file (default stdout)	\n"
"Optional Parameters: \n"
"o1=        output origin  \n"
"d1=        output interval  \n"
"n1=        number of output samples  \n"
"pad1=      pad value beyond the input boundary \n"
"           if not specified, nearest input value will be used \n"
"           beyond the input data boundary \n"
"Notes:			\n"
" 1. user o2,d2,n2,pad2 or o3,d3,n3,pad3 to sample other axis \n"
" 2. linear interpolation used \n"
"\n"
"AUTHOR:  Zhiming Li,         12/9/96			\n"
"\n";
#include "usgrid.h"
#include "subc.h"
#include "par.h"

int main(int argc, char **argv)
{
	usghed ugh;
	FILE *infp,*outfp;
	char *in, *out; 
	float *grids, *grid, *gridi;
	int ierr;
	int n1,n2,n3;
	float d1,d2,d3;
	float o1,o2,o3;
	int n1i,n2i,n3i;
	float d1i,d2i,d3i;
	float o1i,o2i,o3i;
	float pad1,pad2,pad3;
	float x1,x2,x3;
	int ipad1=1,ipad2=1,ipad3=1;
	float e1i, e2i, e3i;
	int i1, i2, i3;

	float tmp;
	int itmp; 
	float gmin, gmax;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("in",&in)) {
		infp = fopen(in,"r");
	} else {
		infp = stdin;
	}
	file2g(infp);
	ierr = fgetusghdr(infp,&ugh);
    if(ierr!=0) err(" input grid header error ");

	if(getparstring("out",&out)) {
		outfp = fopen(out,"w");
	} else {
		outfp = stdout;
	}
	file2g(outfp);

	if( !getparint("n1",&n1) ) n1 = ugh.n1;
	if( !getparint("n2",&n2) ) n2 = ugh.n2;
	if( !getparint("n3",&n3) ) n3 = ugh.n3;
	if( !getparfloat("o1",&o1) ) o1 = ugh.o1;
	if( !getparfloat("o2",&o2) ) o2 = ugh.o2;
	if( !getparfloat("o3",&o3) ) o3 = ugh.o3;
	if( !getparfloat("d1",&d1) ) d1 = ugh.d1;
	if( !getparfloat("d2",&d2) ) d2 = ugh.d2;
	if( !getparfloat("d3",&d3) ) d3 = ugh.d3;
	n1i=ugh.n1; o1i=ugh.o1; d1i=ugh.d1; 
	n2i=ugh.n2; o2i=ugh.o2; d2i=ugh.d2; 
	n3i=ugh.n3; o3i=ugh.o3; d3i=ugh.d3; 
	if(!getparfloat("pad1",&pad1) ) ipad1 = 0;
	if(!getparfloat("pad2",&pad2) ) ipad2 = 0;
	if(!getparfloat("pad3",&pad3) ) ipad3 = 0;

	fprintf(stderr," input grid : \n");
	fprintf(stderr,"   o1=%g d1=%g n1=%d \n", o1i, d1i, n1i);
	fprintf(stderr,"   o2=%g d2=%g n2=%d \n", o2i, d2i, n2i);
	fprintf(stderr,"   o3=%g d3=%g n3=%d \n", o3i, d3i, n3i);
	fprintf(stderr," output grid : \n");
	fprintf(stderr,"   o1=%g d1=%g n1=%d \n", o1, d1, n1);
	fprintf(stderr,"   o2=%g d2=%g n2=%d \n", o2, d2, n2);
	fprintf(stderr,"   o3=%g d3=%g n3=%d \n", o3, d3, n3);
	if(ipad1==1) fprintf(stderr,"   pad1=%g \n",pad1);
	if(ipad2==1) fprintf(stderr,"   pad2=%g \n",pad2);
	if(ipad3==1) fprintf(stderr,"   pad3=%g \n",pad3);

	/* memory allocations */
	grids = (float*) emalloc(n1i*n2i*n3i*sizeof(float));
	gridi = (float*) emalloc(n1i*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));



	e1i = o1i + (n1i-1)*d1i; 
	e2i = o2i + (n2i-1)*d2i; 
	e3i = o3i + (n3i-1)*d3i; 

	fseek(infp,0,0);
	efread(grids,sizeof(float),n1i*n2i*n3i,infp);

	for(i3=0;i3<n3;i3++) {
		x3 = o3 + i3*d3;
		for(i2=0;i2<n2;i2++) {
			x2 = o2 + i2*d2;
			bilint_(&n1i,&n2i,&n3i,&o2i,&o3i,&d2i,&d3i,&x2,&x3,grids,gridi);
			if( (x2<o2i-0.01*d2i || x2>e2i+0.01*d2i) 
			  && x3>=o3i-0.01*d3i && x3<=e3i+0.01*d3i && ipad2==1 ) {
				for(i1=0;i1<n1i;i1++) gridi[i1] = pad2;
			} else if( (x3<o3i-0.01*d3i || x3>e3i+0.01*d3i) && ipad3==1 ) {
				for(i1=0;i1<n1i;i1++) gridi[i1] = pad3;
			} 
			for(i1=0;i1<n1;i1++) {
				x1 = o1 + i1*d1;
				tmp = (x1-o1i)/d1i;
				itmp = tmp;
				tmp = tmp - itmp;
				if( itmp < 0 ) {
					grid[i1] = gridi[0];
					if(ipad1==1) grid[i1] = pad1;
				} else if( itmp>=n1i-1 ) {
					grid[i1] = gridi[n1i-1];
					if(ipad1==1 && x1>e1i) grid[i1] = pad1;
				} else {
					if(n1i==1) { grid[i1] = gridi[0]; } else {
						grid[i1] = gridi[itmp]+
							tmp*(gridi[itmp+1]-gridi[itmp]); 
					}
				}
			}
			if(i3==0 && i2==0) {
				gmin = grid[0]; gmax = grid[0];
			}
			for(i1=0;i1<n1;i1++) {
				if(grid[i1]<gmin) gmin = grid[i1];
				if(grid[i1]>gmax) gmax = grid[i1];
				/*
			fprintf(stderr,"grid=%g i1=%d i2=%d i3=%d \n",grid[i1],i1,i2,i3);
				*/
			}
			fwrite(grid,sizeof(float),n1,outfp);
		}
	}

	ugh.ocdp2 = ugh.ocdp2 + (o2-ugh.o2)/ugh.d2*ugh.dcdp2;
	ugh.dcdp2 = d2/ugh.d2*ugh.dcdp2;
	ugh.oline3 = ugh.oline3 + (o3-ugh.o3)/ugh.d3*ugh.dline3;
	ugh.dline3 = d3/ugh.d3*ugh.dline3;

	ugh.n1 = n1; ugh.o1 = o1; ugh.d1 = d1;
	ugh.n2 = n2; ugh.o2 = o2; ugh.d2 = d2;
	ugh.n3 = n3; ugh.o3 = o3; ugh.d3 = d3;
	ugh.gmin = gmin; ugh.gmax = gmax;

	ierr = fputusghdr(outfp,&ugh);
	if (ierr!=0) err(" error output ");

	free(grids);
	free(gridi);
	free(grid);
	
	exit(0);
}
