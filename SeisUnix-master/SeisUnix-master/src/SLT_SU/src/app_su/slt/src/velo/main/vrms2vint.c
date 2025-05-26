#include "usgrid.h"
#include "par.h"


char *sdoc = 
"VRMS2VINT - convert rms veloclity grid to interval velocity grid \n"
"\n"
"vrms2int [parameters] <vrms.data  >vint.data 				\n" 
"\n"
"Required parameters:						 	\n"
"vrms.data         Name of rms velocity grid file (standard input)	\n"
"vint.data         Name of interval velocity grid file (standard output)\n"
"\n"
"Optional parameters:							\n"
"vinvmax=99999     maximum value of interval velocity inversion allowed \n" 
"                  (in ft/s or m/s) between 2 adjacent time grid points \n" 
"vmax=99999        maximum interval velocity		\n"
"vmin=999          minimum interval velocity		\n"
"ghdout=1          grid header output 	(1=yes 0=no)			\n"
"tvc=              start time of contant interval velocity at each trace \n"
"                  default to the maximum time of input grid, \n"
"                  i.e., (o1+(n1-1)*d1); \n" 
"                  Velocity after tvc will be same as velocity at tvc \n"
"                  at that trace location \n" 
"\n"
" Notes:								\n"
" 1. Input and output velocity grids are stored as (nt/nz,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is time      					\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 2. No time/depth conversion performed. The input and output must      \n"
"    be sampled in time.                          			\n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	3/30/93   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;
	int ierr;
	int n1,n2,n3,i1,i2,i3;
	float d1,o1,tmp;
	float *vint, *vrms, vinvmax;
	float gmin, gmax;
	int ghdout=1;
	float vmin, vmax, tvc;


	usghed usgh;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	file2g(infp);
	file2g(outfp);

	/* required parameters */

	/* optional parameters */
	if(!getparint("ghdout",&ghdout)) ghdout=1;
	
	if(!getparfloat("vinvmax",&vinvmax)) vinvmax=99999;
	if(!getparfloat("vmax",&vmax)) vmax=99999;
	if(!getparfloat("vmin",&vmin)) vmin=999;
	if(!getparfloat("tvc",&tvc)) tvc=-999;

	/* read velocity grid header */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err(" input vint grid not standard grid file format \n"); 
	
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3; 
	if(n3==0) n3=1;
	d1 = usgh.d1;
	o1 = usgh.o1;
	if(tvc==-999.) tvc = o1 + d1*(n1-1);

	
	gmin = usgh.gmin;
	gmax = gmin;
	
   	vint = (float*)malloc(n1*sizeof(float));
   	vrms = (float*)malloc(n1*sizeof(float));
	
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(vrms,sizeof(float),n1,infp);
			vint[0] = vrms[0];
			if(vint[0]>vmax) vint[0] = vmax;
			if(vint[0]<vmin) vint[0] = vmin;
			if(gmin>vint[0])gmin=vint[0];
			if(gmax<vint[0])gmax=vint[0];
			for(i1=1;i1<n1;i1++) {
				if(o1+i1*d1>tvc) {
					vint[i1] = vint[i1-1];
				} else {
					tmp = (i1*d1+o1)*vrms[i1]*vrms[i1] - 
				      	((i1-1)*d1+o1)*vrms[i1-1]*vrms[i1-1];
					if(tmp>0.) {
						vint[i1] = sqrt(tmp/d1);
					} else {
						vint[i1] = vint[i1-1];
					}
					if(vint[i1-1]-vint[i1]>vinvmax) 
						vint[i1] = vint[i1-1]-vinvmax; 
				}
				if(vint[i1]>vmax) vint[i1] = vmax;
				if(vint[i1]<vmin) vint[i1] = vmin;

				if(gmin>vint[i1])gmin=vint[i1];
				if(gmax<vint[i1])gmax=vint[i1];
			}
			efwrite(vint,sizeof(float),n1,outfp);
		}
	}

	usgh.gmin = gmin;
	usgh.gmax = gmax;
	usgh.gtype = 3;

	if(ghdout==1) {
		ierr = fputusghdr(outfp, &usgh);
		if(ierr!=0) err(" output vint grid file error \n"); 
	}

	exit(0);
	
}
