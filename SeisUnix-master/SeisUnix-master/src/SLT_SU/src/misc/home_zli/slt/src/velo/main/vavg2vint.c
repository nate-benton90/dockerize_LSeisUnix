#include "usgrid.h"
#include "par.h"


char *sdoc = 
"VAVG2VINT - convert average velocity grid to interval velocity grid \n"
"\n"
"vavg2vint [parameters] <vavg.data  >vint.data 				\n" 
"\n"
"Required parameters:						 	\n"
"vavg.data         Name of average velocity grid file (standard input)	\n"
"vint.data         Name of interval velocity grid file (standard output)\n"
"\n"
"Optional parameters:							\n"
"tord=0            time or depth of input velocity grid axis 1             \n" 
"                  (0=time 1=depth)                                     \n"
"ghdout=1          grid header output 					\n"
"                  1=yes 0=no						\n"
"\n"
" Notes:								\n"
" 1. Input and output velocity grids are stored as (nt/nz,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is time/depth					\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 2. Input average velocity is average of time                       \n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	3/20/94   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;
	int ierr;
	int n1,n2,n3,i1,i2,i3;
	float d1,o1,tmp;
	float *vint, *vavg;
	float gmin, gmax;
	float t1, t2;
	int tord, gtype, ghdout; 


	usghed usgh;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	/* required parameters */

	/* optional parameters */
	if(!getparint("tord",&tord)) tord=0; 
	if(!getparint("ghdout",&ghdout)) ghdout=1; 

	/* read velocity grid header */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err(" input vavg grid not standard grid file format \n"); 
	
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3; 
	if(n3==0) n3=1;
	d1 = usgh.d1;
	o1 = usgh.o1;
	
	gmin = usgh.gmin;
	gmax = gmin;
	
    	vint = (float*)malloc(n1*sizeof(float));
    	vavg = (float*)malloc(n1*sizeof(float));
	
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(vavg,sizeof(float),n1,infp);
			vint[0] = vavg[0];
			if(gmin>vint[0]) gmin=vint[0];
			if(gmax<vint[0]) gmax=vint[0];
			for(i1=1;i1<n1;i1++) {
				if(tord==0) {
					tmp = (o1+i1*d1)*vavg[i1] - 
						(o1+(i1-1)*d1)*vavg[i1-1];
					vint[i1] = tmp/d1;
				} else {
					t1 = (o1+(i1-1)*d1)/vavg[i1];
					t2 = (o1+i1*d1)/vavg[i1];
					tmp = t2*vavg[i1] - 
						t1*vavg[i1-1];
					vint[i1] = tmp/(t2-t1);
				}	
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
	
	exit (0);
}
