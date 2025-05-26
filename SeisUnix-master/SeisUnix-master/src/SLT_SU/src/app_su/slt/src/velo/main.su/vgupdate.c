#include "usgrid.h"
#include "par.h"

char *sdoc = 
"VGUPDATE - update time rms velocity grid using residual moveout grid \n"
"\n"
"vgupdate [parameters] <vginput.data  >vgoutput.data 			\n" 
"\n"
"Required parameters:						 	\n"
"vginput.data      Name of time rms velocity input grid file 		\n"
"                  (standard input)\n"
"vgoutput.data     Name of time rms velocity output grid file 		\n"
"                  (standard output)					\n"
"rmogrid=          residual moveout grid (ms at maximum offset) \n"
"maxoffset=        maximum offset where the residual moveout is specified \n"
"rmin=0.8          minimum ratio between updated velocity and input velocity\n"
"rmax=1.2          maximum ratio between updated velocity and input velocity\n"
"Optional parameters:							\n"
"None \n"
"Example: \n"
" < v.in.grid vgupdate rmogrid=rmo.grid maxoffset=8200 > v.out.grid \n"
"\n"
" Notes:								\n"
" 1. Input/output grids are stored as (nt,nx,ny) order, \n"
"    i.e., dimensions of the grids are:					\n"
"         1st dimension is time					\n"
"         2nd dimension is in inline direction		\n"
"         3rd dimension is in crossline direction	\n"
" 2. rmogrid can be computed from RNmo program  	\n"
" 3. Time unit in grid header is ms			\n"
" 4. input grid and rmo grid must have the same dimensions \n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	11/20/2000   		\n"    
;


main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;
    	FILE *rfp;
	char *rmogrid;
	int ierr;
	int n1,n2,n3,i1,i2,i3;
	float d1,o1;
	float *vin, *vout, *rmo;
	float gmin, gmax, maxoffset;
	float t, x2, v2, tmp, ratio, rmin, rmax;

	usghed usghv, usghr;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	/* required parameters */
	if(!getparstring("rmogrid",&rmogrid))
		err("rmogrid must be specified");
	if(!getparfloat("maxoffset",&maxoffset))
		err("maxoffset must be specified");

	/* optional parameters */
	if(!getparfloat("rmin",&rmin)) rmin=0.8;
	if(!getparfloat("rmax",&rmax)) rmax=1.2;

	file2g(infp);
	file2g(outfp);

	/* read velocity grid header */
	ierr = fgetusghdr(infp, &usghv);
	if(ierr!=0) err(" input v grid not standard grid file format \n"); 

	rfp = efopen(rmogrid,"r");
	file2g(rfp);
	ierr = fgetusghdr(rfp, &usghr);
	if(ierr!=0) err(" input rmo grid not standard grid file format \n"); 

	/* check dimensions */
	if(usghv.n1!=usghr.n1) err(" check n1 of input and rmo grids");
	if(usghv.n2!=usghr.n2) err(" check n2 of input and rmo grids");
	if(usghv.n3!=usghr.n3) err(" check n3 of input and rmo grids");

	n1 = usghv.n1;
	n2 = usghv.n2;
	n3 = usghv.n3;
	o1 = usghv.o1;
	d1 = usghv.d1;
	if(n3==0) n3=1;
	
    	vin = (float*)malloc(n1*sizeof(float));
    	rmo = (float*)malloc(n1*sizeof(float));
    	vout = (float*)malloc(n1*sizeof(float));

	fseek2g(infp,0,0);
	fseek2g(rfp,0,0);

	x2 = maxoffset*maxoffset;

	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(vin,sizeof(float),n1,infp);
			efread(rmo,sizeof(float),n1,rfp);
			ratio = 1.;
			for(i1=0;i1<n1;i1++) {
				t = i1*d1+o1;
				t = t * 0.001;
				v2 = vin[i1]*vin[i1];

				/*   parabolic approximation */ 
				/*
				t = 2.*sqrt(t*t+x2/v2);
				*/
				/*  hyperbolic approximation */
				t = 2.*t + rmo[i1]*0.001;

				tmp = x2/(t*rmo[i1]*0.001*v2+x2);
				if(tmp>0.) {
					tmp = sqrt(tmp);
					if(tmp>=rmin && tmp<=rmax)
						ratio = tmp;
				}
				vout[i1]=vin[i1]*ratio;
			}
			efwrite(vout,sizeof(float),n1,outfp);
			if(i3==0 && i2==0) {
				gmin = vout[0];
				gmax = vout[0];
			}
			for(i1=1;i1<n1;i1++) {
				if(vout[i1]<gmin) gmin=vout[i1];
				if(vout[i1]>gmax) gmax=vout[i1];
			}
		}
	}

	usghv.gmin = gmin;
	usghv.gmax = gmax;
	ierr = fputusghdr(outfp, &usghv);
	if(ierr!=0) err(" output v grid file error \n"); 

	free(vin);
	free(vout);
	free(rmo);

	return 0;
}
