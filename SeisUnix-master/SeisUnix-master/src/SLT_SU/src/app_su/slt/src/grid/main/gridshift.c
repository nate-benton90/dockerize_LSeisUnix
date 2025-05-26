#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc =
"GRIDSHIFT - 3D grid shift to new datum  \n"
"\n"
"gridshift <infile >outfile [optional parameters]\n"
"\n"
"Required Parameters:\n"
"infile=        name of the input 3D grid file	(velocity)	\n"
"outfile=       name of the output 3D grid file (velocity) \n"
"datum=         name of the 2D grid defining the new datum \n"
"Optional Parameters:\n"
"n1out=         number of samples in the first axis of output \n"
"               (default to the number of samples in the input) \n"
"d1out=         output interval \n"
"               (default to the number of samples in the output) \n"
"gtop=          grid value to be used at top where the output \n"
"               time/depth is beyond the input grid time/depth range \n"
"               (default to the input end value) \n"
"gbot=          grid value to be used at bottom where the output \n"
"               time/depth is beyond the input grid time/depth range \n"
"               (default to the input end value) \n"
"datumscale=1.0 multiple the dataum by datumscale before applying \n"
"Notes:\n"
" 1. Negative datum value will shift grid down, positive up \n "
" 2. Linear interpolation will be used to resample the output within \n "
"    the time/depth range (the 1st axis) of the input. Outside the range \n"
"    of the time/depth the input, grid value(s) at the end(s) will \n"
"    be replicated if gtop or gbot are not specified. \n"
"AUTHOR:  Zhiming Li,         1/17/2000			\n"
"\n";

int main(int argc, char **argv)
{
	usghed usghin, usghdatum;
	FILE *infp,*outfp,*datumfp;
	char *infile,*outfile,*datum;
	int ierr;

	int n1,n2,n3;
	int i1,i2,i3;
	float d1,o1;
	float d1out, o1out;
	int n1out;
	float tmp, z, z0;
	float gmin, gmax;
	float gtop, gbot;
	int igtop=0, igbot=0;
	int ii1;

	float *gdatum, *grid, *grido;
	float datumscale=1.;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("infile",&infile)) {
		infp = efopen(infile,"r");
	} else {
		infp = stdin;
	}
	file2g(infp);
	ierr = fgetusghdr(infp,&usghin);
    if(ierr!=0) err(" input grid header error ");
	if(getparstring("outfile",&outfile)) {
		outfp = efopen(outfile,"w");
	} else {
		outfp = stdout;
	}
	file2g(outfp);

	if (getparstring("datum",&datum)) {
		datumfp = efopen(datum,"r");
		ierr = fgetusghdr(datumfp,&usghdatum);
      		if(ierr!=0) err(" datum grid header error ");
	} else {
		err(" datum missing ");
	}

	if (getparfloat("gtop",&gtop)) igtop = 1;
	if (getparfloat("gbot",&gbot)) igbot = 1;
	if (!getparfloat("datumscale",&datumscale)) datumscale = 1.;

	n1 = usghin.n1;
	n2 = usghin.n2;
	n3 = usghin.n3;
	o1 = usghin.o1;
	d1 = usghin.d1;
	gmin = usghin.gmin;
	gmax = usghin.gmax;

	if(!getparint("n1out",&n1out)) n1out = n1;
	if(!getparfloat("d1out",&d1out)) d1out = d1;

	/* memory allocations */
	grid = (float*) emalloc(n1*sizeof(float));
	grido = (float*) emalloc(n1out*sizeof(float));
	gdatum = (float*) emalloc(n2*n3*sizeof(float));
	
	if(usghin.n2!=usghdatum.n1) err("check datum grid header n1");
	if(usghin.n3!=usghdatum.n2) err("check datum grid header n2");
	if(usghin.o2!=usghdatum.o1) 
		err("check datum grid header o1=%f and grid input o2=%f",
			usghdatum.o1,usghin.o2);
	if(usghin.o3!=usghdatum.o2) err("check datum grid header o2");
	if(usghin.d2!=usghdatum.d1) err("check datum grid header d1");
	if(usghin.d3!=usghdatum.d2) err("check datum grid header d2");

	efseek(datumfp,0,0);
	efread(gdatum,sizeof(float),n2*n3,datumfp);

	for(i1=0;i1<n2*n3;i1++) gdatum[i1] *= datumscale;

	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {

			fread(grid,sizeof(float),n1,infp);

			z0 = gdatum[i3*n2+i2];

			for(i1=0;i1<n1out;i1++) {
				z = z0 + i1*d1out;
				tmp = (z - o1)/d1;
				ii1 = tmp;
				if(tmp<0.) {
					if(igtop==0) {
						grido[i1] = grid[0];
					} else {
						grido[i1] = gtop;
					}
				} else if(ii1>=n1-1) {
					if(igbot==0) {
						grido[i1] = grid[n1-1];
					} else {
						grido[i1] = gbot;
					}
				} else {
					grido[i1] = grid[ii1] + (tmp-ii1)*(grid[ii1+1]-grid[ii1]);
				}
			}
			if(i3==0 && i2==0) {
				gmax = grido[0]; 
				gmin = grido[0];
			}
			for(i1=0;i1<n1out;i1++) {
				if(grido[i1]>gmax) gmax = grido[i1];
				if(grido[i1]<gmin) gmin = grido[i1];
			}

			fwrite(grido,sizeof(float),n1out,outfp);

		}
	}


	usghin.gmin = gmin;
	usghin.gmax = gmax;
	usghin.o1 = 0.;
	usghin.d1 = d1out;
	usghin.n1 = n1out;

	ierr = fputusghdr(outfp,&usghin);
	
	free(gdatum);
	free(grid);
	free(grido);

	exit(0);
}
