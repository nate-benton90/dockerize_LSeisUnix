#include "usgrid.h"
#include "par.h"


char *sdoc = 
"VINT2VAVG - convert interval velocity grid to average velocity grid \n"
"\n"
"vavg2vint [parameters] <vavg.data  >vint.data 				\n" 
"\n"
"Required parameters:						 	\n"
"vint.data         Name of interval velocity grid file (standard input)\n"
"vavg.data         Name of average velocity grid file (standard output)	\n"
"\n"
"Optional parameters:							\n"
"tord=0            time or depth of input velocity grid axis 1             \n" 
"                  (0=time 1=depth)                                     \n"
"ghdout=1          grid header output (1=yes 0=no)                      \n" 
"\n"
" Notes:								\n"
" 1. Input and output velocity grids are stored as (nt/nz,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is time/depth					\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 2. Output average velocity is average of time                       \n"
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
	int tord; 
	int ghdout=1;


	usghed usgh;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	/* required parameters */

	/* optional parameters */
	if(!getparint("tord",&tord)) tord=0; 
	if(!getparint("ghdout",&ghdout)) ghdout=1; 

	file2g(infp);
	file2g(outfp);
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
			efread(vint,sizeof(float),n1,infp);
			vavg[0] = vint[0];
			if(gmin>vavg[0]) gmin=vavg[0];
			if(gmax<vavg[0]) gmax=vavg[0];
			for(i1=1;i1<n1;i1++) {
				if(tord==0) {
					tmp = (o1+(i1-1)*d1)*vavg[i1-1] + 
						d1*vint[i1];
					vavg[i1] = tmp/(o1+i1*d1);
				} else {
					t1 = (o1+(i1-1)*d1)/vavg[i1-1];
					t2 = t1 + d1/vint[i1];
					vavg[i1] = (o1+i1*d1)/t2;
				}	
				if(gmin>vavg[i1]) gmin=vavg[i1];
				if(gmax<vavg[i1]) gmax=vavg[i1];
			}
			efwrite(vavg,sizeof(float),n1,outfp);
		}
	}

	usgh.gmin = gmin;
	usgh.gmax = gmax;
	usgh.gtype = 2;

	if(ghdout==1) {
		ierr = fputusghdr(outfp, &usgh);
		if(ierr!=0) err(" output vavg grid file error \n"); 
	}
	
}
