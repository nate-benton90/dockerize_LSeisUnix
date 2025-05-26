#include "usgrid.h"
#include "par.h"


char *sdoc = 
"VAVG2DEPTH - convert average velocity grid to depth grid \n"
"\n"
"vavg2vint [parameters] <vavg.data  >depth.data 			\n" 
"\n"
"Required parameters:						 	\n"
"vavg.data         Name of average velocity grid file (standard input)	\n"
"depth.data        Name of depth (function of time) grid file 		\n"
"                  (standard output)					\n"
"\n"
"Optional parameters:							\n"
"None	\n"
"\n"
" Notes:								\n"
" 1. Input and output grids are stored as (nt,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is time					\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 2. Input average velocity is average of time                       \n"
" 3. Input average velocity sampling interval d1 is in ms (in header)	\n"
" 4. time is two way time, depth is in m or ft				\n"
" 5. Output grid is depth at evenly sampled times			\n"  
"\n"
"AUTHOR:	   Zhiming Li,       ,	6/30/94   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;
	int ierr;
	int n1,n2,n3,i1,i2,i3;
	float d1,o1;
	float *depth, *vavg;
	float gmin, gmax;


	usghed usgh;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	/* required parameters */

	/* optional parameters */

	/* read velocity grid header */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err(" input vavg grid not standard grid file format \n"); 
	
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3; 
	if(n3==0) n3=1;
	d1 = usgh.d1;
	o1 = usgh.o1;
	
	gmin = 9999999999.;
	gmax = 0.;
	
    	depth = (float*)malloc(n1*sizeof(float));
    	vavg = (float*)malloc(n1*sizeof(float));
	
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(vavg,sizeof(float),n1,infp);
			for(i1=0;i1<n1;i1++) {
				depth[i1] = vavg[i1]*(o1+d1*i1)*0.0005;
				if(gmin>depth[i1])gmin=depth[i1];
				if(gmax<depth[i1])gmax=depth[i1];
			}
			efwrite(depth,sizeof(float),n1,outfp);
		}
	}

	usgh.gmin = gmin;
	usgh.gmax = gmax;
	usgh.gtype = 4;

	ierr = fputusghdr(outfp, &usgh);
	if(ierr!=0) err(" output depth grid file error \n"); 
	
}
