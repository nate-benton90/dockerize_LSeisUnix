#include "usgrid.h"
#include "par.h"


char *sdoc = 
"VINT2VRMS - convert interval velocity grid to rms velocity grid \n"
"\n"
"vint2vrms [parameters] <vint.data  >vrms.data 				\n" 
"\n"
"Required parameters:						 	\n"
"vint.data         Name of interval velocity grid file (standard input)	\n"
"vrms.data         Name of rms velocity grid file (standard output)	\n"
"\n"
"Optional parameters:							\n"
"ghdout=1          grid header output (1=yes 0=no)			\n"
"\n"
" Notes:								\n"
" 1. Input and output velocity grids are stored as (nt/nz,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is time       				\n"
"         2nd dimension is lateral distance in inline direction		\n"
"         3rd dimension is lateral distance in crossline direction	\n"
" 2. No time/depth conversion performed. The input and output must      \n"
"    be sampled in time.                                 		\n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	3/30/93   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;
	int ierr;
	int n1,n2,n3,i1,i2,i3;
	float d1,o1,tmp;
	float *vint, *vrms;
	float gmin, gmax;
	int ghdout=1;


	usghed usgh;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	file2g(infp);
	file2g(outfp);

	/* required parameters */

	/* optional parameters */
	if(!getparint("ghdout",&ghdout)) ghdout=1;

	/* read velocity grid header */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err(" input vint grid not standard grid file format \n"); 
	
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3; 
	if(n3==0) n3=1;
	d1 = usgh.d1;
	o1 = usgh.o1;
	
	gmin = usgh.gmin;
	gmax = gmin;
	
    	vint = (float*)malloc(n1*sizeof(float));
    	vrms = (float*)malloc(n1*sizeof(float));
	
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(vint,sizeof(float),n1,infp);
			vrms[0] = vint[0];
			tmp = vint[0]*vint[0]*o1;
			if(gmin>vrms[0])gmin=vrms[0];
			if(gmax<vrms[0])gmax=vrms[0];
			for(i1=1;i1<n1;i1++) {
				tmp = tmp +  vint[i1]*vint[i1]*d1;
				vrms[i1] = sqrt(tmp/(o1+i1*d1));
				if(gmin>vrms[i1])gmin=vrms[i1];
				if(gmax<vrms[i1])gmax=vrms[i1];
			}
			efwrite(vrms,sizeof(float),n1,outfp);
		}
	}

	usgh.gmin = gmin;
	usgh.gmax = gmax;
	usgh.gtype = 1;

	if(ghdout==1) {
		ierr = fputusghdr(outfp, &usgh);
		if(ierr!=0) err(" output vrms grid file error \n"); 
	}
	
}
