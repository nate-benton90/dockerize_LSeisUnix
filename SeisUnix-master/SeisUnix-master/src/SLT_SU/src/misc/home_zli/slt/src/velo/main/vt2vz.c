#include "usgrid.h"
#include "par.h"


char *sdoc = 
"VT2VZ - convert time velocity grid to depth velocity grid \n"
"\n"
"vt2vz [parameters] <vt.data  >vz.data 			\n" 
"\n"
"Required parameters:						 	\n"
"vt.data           Name of interval velocity time grid file 		\n"
"                  (standard input)\n"
"vz.data           Name of interval velocity depth grid file 		\n"
"                  (standard output)					\n"
"dz=               depth interval (m or ft) to output interval velocity \n"
"nz=               number of depths to output interval velocity        	\n"
"\n"
"Optional parameters:							\n"
"fz=0              minimum depth to output interval velocity          	\n"
"\n"
" Notes:								\n"
" 1. Input grid is stored as (nt,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is time					\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 2. Output grid is stored as (nz,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is depth					\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 3. Time unit in input velocity grid header is ms			\n"
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
	float *zin, *zout;
	int *indx;
	float gmin, gmax;
	float vmin, vmax;

	int nz;
	float dz, fz;

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

	if(!getparfloat("dz",&dz)) err(" dz missing");
	if(!getparint("nz",&nz)) err(" nz missing");
	if(!getparfloat("fz",&fz)) fz = 0;
	
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3; 
	if(n3==0) n3=1;
	d1 = usgh.d1;
	o1 = usgh.o1;
	
    	vin = (float*)malloc(n1*sizeof(float));
    	zin = (float*)malloc(n1*sizeof(float));
    	zout = (float*)malloc(nz*sizeof(float));
    	vout = (float*)malloc(nz*sizeof(float));
    	indx = (int*)malloc(nz*sizeof(int));

	for(i1=0;i1<nz;i1++) zout[i1] = fz + i1*dz; 

	vmin = 9999999999.; 
	vmax = 0.;

	
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(vin,sizeof(float),n1,infp);
			zin[0] = o1*vin[0]*0.0005;
			for(i1=1;i1<n1;i1++)
			zin[i1] = zin[i1-1] + (vin[i1-1]+vin[i1])*d1*0.00025;

			 lin1d2_(zin,vin,&n1,zout,vout,&nz,indx); 
/*			 lin1d_(zin,vin,&n1,zout,vout,&nz,indx); */
			 /*
			 for(i1=0;i1<n1;i1++)
				fprintf(stderr,"z=%g tin=%g vin=%g vout=%g \n",
						i1*dz+fz, zin[i1], vin[indx[i1]],vout[i1]);
			*/
			efwrite(vout,sizeof(float),nz,outfp);
			fminmax(vout, nz, &gmin, &gmax);
			if(gmin<vmin) vmin = gmin;
			if(gmax>vmax) vmax = gmax;
		}
	}

	usgh.gmin = vmin;
	usgh.gmax = vmax;
	usgh.o1 = fz;
	usgh.d1 = dz;
	usgh.n1 = nz;

	ierr = fputusghdr(outfp, &usgh);
	if(ierr!=0) err(" output depth grid file error \n"); 
	
}
