char *sdoc =
"TTHORIZON - window travel time along a horizon \n"
"\n"
"tthorizon <in [required parameters] >out \n"
"\n"
"Required Parameters:\n"
"in=        name of the input travel time grid file (default stdin) 	\n"
"out=       name of the output travel time grid file (default stdout)	\n"
"hzgrid=    horizon grid name to specify the starting depth \n"
"           of the window \n"
"nz=        number of output depth samples  \n"
"Optional Parameters: \n"
"v0=1500    reference velocity at the surface \n" 
"dvz=0.2    reference vertical velocity gradient (default m/s/m \n"
"Notes:			\n"
" 1. hzgrid is the horizon going to be used in the kzmig program  \n"
"    the o1,d1,n1 and o2,d2,n2 of hzgrid define the inline and \n"  
"    crossline coordinates. \n"
"    they must be in the same unit and coordinate system \n"
"    as the o2,d2,n2 and o3,d3,n3 of the travel time table \n"
"    (the o1,d1,n1 of travel time define the depth coordinates) \n"
" 2. the output depth sampling of the travel time is the same as input \n"
" 3. nz should be large enough to make sure the travel time depth range \n"
"    cover the output migration depth range:  \n"
"       nz*dz > nzo*dzo \n"
"    where dz is the depth interval of travel time table \n"
"          nzo is the number of output depth samples in kzmig \n"
"          dzo is the output depth interval in kzmig \n"
"\n"
"AUTHOR:  Zhiming Li,         9/12/99			\n"
"\n";
#include "usgrid.h"
#include "subc.h"
#include "par.h"

main(int argc, char **argv)
{
	usghed ugh;
	usghed hzgh;
	FILE *infp,*outfp,*hzfp;
	char *in, *out, *hzgrid; 
	float *gridi, *grido, *hz;
	int ierr;
	int n1,n2,n3,n4,n5;
	float d1,d2,d3,d4,d5;
	float o1,o2,o3,o4,o5;
	int i1,i2,i3,i4,i5;

	float v0, dvz; 

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("in",&in)) {
		infp = efopen(in,"r");
	} else {
		infp = stdin;
	}
	file2g(infp);
	ierr = fgetusghdr(infp,&ugh);
    if(ierr!=0) err(" input grid header error ");

	if(getparstring("out",&out)) {
		outfp = efopen(out,"w");
	} else {
		outfp = stdout;
	}
	file2g(outfp);

	if(!getparstring("hzgrid",&hzgrid)) err(" hzgrid missing");
	if(!getparint("nz",&nz)) err(" nz missing");
	if(!getparfloat("v0",&v0)) v0 = 1500.;
	if(!getparfloat("dvz",&dvz)) dvz = 0.2;

	hzfp = efopen(hzgrid,"r");
	ierr = fgetusghdr(hzfp,&hzgh);
	if (ierr!=0) err("Grid parameters of %s required!\n",hzgrid);
	o1hz = hzgh.o1;
	d1hz = hzgh.d1;
	n1hz = hzgh.n1;
	o2hz = hzgh.o2;
	d2hz = hzgh.d2;
	n2hz = hzgh.n2;
	hz = (float*) malloc(n1hz*n2hz*sizeof(float));
	fseek(hzfp,0,0);
	efread(hz,sizeof(float),n1hz*n2hz,hzfp);
	fclose(hzfp);




	n1 = ugh.n1;
	n2 = ugh.n2;
	n3 = ugh.n3;
	n4 = ugh.n4;
	n5 = ugh.n5;
	o1 = ugh.o1;
	o2 = ugh.o2;
	o3 = ugh.o3;
	o4 = ugh.o4;
	o5 = ugh.o5;
	d1 = ugh.d1;
	d2 = ugh.d2;
	d3 = ugh.d3;
	d4 = ugh.d4;
	d5 = ugh.d5;

	fprintf(stderr," input travel time grid : \n");
	fprintf(stderr,"   o1=%g d1=%g n1=%d \n", o1, d1, n1);
	fprintf(stderr,"   o2=%g d2=%g n2=%d \n", o2, d2, n2);
	fprintf(stderr,"   o3=%g d3=%g n3=%d \n", o3, d3, n3);
	fprintf(stderr,"   o4=%g d4=%g n4=%d \n", o4, d4, n4);
	fprintf(stderr,"   o5=%g d5=%g n5=%d \n", o5, d5, n5);

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
			efwrite(grid,sizeof(float),n1,outfp);
		}
	}

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
