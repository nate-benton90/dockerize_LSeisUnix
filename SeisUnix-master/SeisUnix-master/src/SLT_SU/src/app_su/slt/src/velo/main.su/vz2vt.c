#include "usgrid.h"
#include "par.h"


char *sdoc = 
"VZ2VT - convert depth velocity grid to time velocity grid \n"
"\n"
"vz2vt [parameters] <vz.data  >vt.data 			\n" 
"\n"
"Required parameters:						 	\n"
"vz.data           Name of interval velocity depth grid file 		\n"
"                  (standard input)					\n"
"vt.data           Name of interval velocity time grid file 		\n"
"                  (standard output)					\n"
"dt=               time interval (ms) to output interval velocity 	\n"
"nt=               number of times to output interval velocity        	\n"
"\n"
"Optional parameters:							\n"
"ft=0              minimum time (ms) to output interval velocity       	\n"
"\n"
" Notes:								\n"
" 1. Input grid is stored as (nz,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is depth					\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 2. Output grid is stored as (nt,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is time					\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 3. Time unit in output velocity grid header is ms			\n"
" 4. Time is two-way time						\n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	7/12/94   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;
	int ierr;
	int n1,n2,n3,i1,i2,i3;
	float d1,o1;
	float *vin, *vout;
	float *tin, *tout;
	int *indx;
	float gmin, gmax;
	float vmin, vmax;

	int nt;
	float dt, ft;

	usghed usgh;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	/* required parameters */

	/* optional parameters */

        file2g(infp);
        file2g(outfp);


	/* read velocity grid header */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err(" input v grid not standard grid file format \n"); 

	if(!getparfloat("dt",&dt)) err(" dt missing");
	if(!getparint("nt",&nt)) err(" nt missing");
	if(!getparfloat("ft",&ft)) ft = 0.;
	
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3; 
	if(n3==0) n3=1;
	d1 = usgh.d1;
	o1 = usgh.o1;
	
    	vin = (float*)malloc(n1*sizeof(float));
    	tin = (float*)malloc(n1*sizeof(float));
    	tout = (float*)malloc(nt*sizeof(float));
    	vout = (float*)malloc(nt*sizeof(float));
    	indx = (int*)malloc(nt*sizeof(int));

	for(i1=0;i1<nt;i1++) tout[i1] = (ft + i1*dt); 
	bzero(vout,nt*sizeof(float));
	bzero(indx,nt*sizeof(int));

	vmin = 9999999999.; 
	vmax = 0.;

	
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(vin,sizeof(float),n1,infp);
			tin[0] = o1/vin[0]*2000.;
			for(i1=1;i1<n1;i1++)
			tin[i1] = tin[i1-1] + d1/(vin[i1-1]+vin[i1])*4000.;
			lin1d2_(tin,vin,&n1,tout,vout,&nt,indx);
			efwrite(vout,sizeof(float),nt,outfp);
			fminmax(vout, nt, &gmin, &gmax);
			if(gmin<vmin) vmin = gmin;
			if(gmax>vmax) vmax = gmax;
		}
	}

	usgh.gmin = vmin;
	usgh.gmax = vmax;
	usgh.o1 = ft;
	usgh.d1 = dt;
	usgh.n1 = nt;

	ierr = fputusghdr(outfp, &usgh);
	if(ierr!=0) err(" output depth grid file error \n"); 
	
}
