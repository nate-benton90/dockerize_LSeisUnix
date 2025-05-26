#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc =
"GRIDTHICKNESS - Computer thickness between two layes \n"
"\n"
"gridthickness <infile >outfile [optional parameters]\n"
"\n"
"Required Parameters:\n"
"infile=          name of the input 3D attribute grid file	\n"
"outfile=         name of the output 2D thickness file \n"
"layertop=        name of the 2D grid defining the depth of the layer top \n"
"layerbot=        name of the 2D grid defining the depth of the layer bottom \n"
"Optional Parameters:\n"
"gridnull=-99999  grid value in infile to be exclused from thickness \n"
"                 calculation \n"
"tol=0            tolerance value (when abs(grid-gridnull)<=tol \n"
"                 the grid is treated as gridnull)  	\n"
"topmask=         name of the 2D grid defining the mask (0 or 1) of the layer top \n"
"                 if not specified, layertop exists everywhere.		\n"
"                 (if specified, layertop exists where topmask value=1 \n"
"                                layertop does not exists where topmask value=0) \n"
"botmask=         name of the 2D grid defining the mask (0 or 1) of the layer bottom \n"
"                 if not specified, layerbot exists everywhere.		\n"
"                 (if specified, layerbot exists where botmask value=1 \n"
"                                layerbot does not exists where botmask value=0) \n"
"thicknull=-77777 thickness value to indicate thickness computation is invalid \n" 
"                 (when either topmask value or botmask value is 0, and there \n"
"                 are no gridnull values between layertop and layerbot; \n"
"                 or when both topmask value and botmask value are 0) \n"
" Notes:						\n"
"    1. The program will compute the thickness of the zone between \n"
"       layertop and layerbot where input grid value is not gridnull. \n"
"\n"
"AUTHOR:  Zhiming Li,         6/20/2002			\n"
"\n";

int main(int argc, char **argv)
{
	usghed usghin, usghtop, usghbot, usghtopmask, usghbotmask;
	FILE *infp,*outfp,*topfp,*botfp;
	FILE *topmaskfp,*botmaskfp;
	char *infile,*outfile,*layertop, *layerbot;
	char *topmask, *botmask;
	int ibot, itop;
	int ierr;
	int nz, iz;
	int n1,n2,n3;
	int i1,i2,i3;
	float d1,o1,d2,o2,d3,o3;
	float saltdz;
	float gridnull, tol;
	float gmin, gmax, tmp;
	float *ztop, *zbot, *grid, *dz, *masktop, *maskbot;
	int it, ib;
	float thicknull=-77777;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("infile",&infile)) {
		infp = efopen(infile,"r");
	} else {
		infp = stdin;
	}
	ierr = fgetusghdr(infp,&usghin);
    	if(ierr!=0) err(" input grid header error ");
	if(getparstring("outfile",&outfile)) {
		outfp = efopen(outfile,"w");
	} else {
		outfp = stdout;
	}
	file2g(infp);
	file2g(outfp);

	if (getparstring("layertop",&layertop)) {
		topfp = efopen(layertop,"r");
		ierr = fgetusghdr(topfp,&usghtop);
      		if(ierr!=0) err(" layertop grid header error ");
	} else {
		err(" layertop missing ");
	}

	if (getparstring("layerbot",&layerbot)) {
		botfp = efopen(layerbot,"r");
		ierr = fgetusghdr(botfp,&usghbot);
      		if(ierr!=0) err(" layerbot grid header error ");
	} else {
		err(" layerbot missing ");
	}

	if(!getparfloat("gridnull",&gridnull)) gridnull=-99999;
	if(!getparfloat("tol",&tol)) tol=0.;


	n1 = usghin.n1;
	n2 = usghin.n2;
	n3 = usghin.n3;
	o1 = usghin.o1;
	d1 = usghin.d1;
	o2 = usghin.o2;
	d2 = usghin.d2;
	o3 = usghin.o3;
	d3 = usghin.d3;

	/* memory allocations */
	ztop = (float*) emalloc(n2*n3*sizeof(float));
	zbot = (float*) emalloc(n2*n3*sizeof(float));
	masktop = (float*) emalloc(n2*n3*sizeof(float));
	maskbot = (float*) emalloc(n2*n3*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));
	dz = (float*) emalloc(n2*n3*sizeof(float));
	
	/* check grid header of layertop */
	if(usghin.n2!=usghtop.n1) err("check layertop header n1");
	if(usghin.n3!=usghtop.n2) err("check layertop header n2");
	if(usghin.o2!=usghtop.o1) err("check layertop header o1");
	if(usghin.o3!=usghtop.o2) err("check layertop header o2");
	if(usghin.d2!=usghtop.d1) err("check layertop header d1");
	if(usghin.d3!=usghtop.d2) err("check layertop header d2");
	efseek(topfp,0,0);
	efread(ztop,sizeof(float),n2*n3,topfp);

	/* check grid header of layerbot */
	if(usghin.n2!=usghbot.n1) err("check layerbot header n1");
	if(usghin.n3!=usghbot.n2) err("check layerbot header n2");
	if(usghin.o2!=usghbot.o1) err("check layerbot header o1");
	if(usghin.o3!=usghbot.o2) err("check layerbot header o2");
	if(usghin.d2!=usghbot.d1) err("check layerbot header d1");
	if(usghin.d3!=usghbot.d2) err("check layerbot header d2");
	efseek(botfp,0,0);
	efread(zbot,sizeof(float),n2*n3,botfp);

	/* read topmask grid if specified */
	if (getparstring("topmask",&topmask)) {
		topmaskfp = efopen(topmask,"r");
		ierr = fgetusghdr(topmaskfp,&usghtopmask);
      		if(ierr!=0) err(" topmask grid header error ");
		if(usghin.n2!=usghtopmask.n1) err("check topmask header n1");
		if(usghin.n3!=usghtopmask.n2) err("check topmask header n2");
		if(usghin.o2!=usghtopmask.o1) err("check topmask header o1");
		if(usghin.o3!=usghtopmask.o2) err("check topmask header o2");
		if(usghin.d2!=usghtopmask.d1) err("check topmask header d1");
		if(usghin.d3!=usghtopmask.d2) err("check topmask header d2");
		efseek(topmaskfp,0,0);
		efread(masktop,sizeof(float),n2*n3,topmaskfp);
	} else {
		for(i1=0;i1<n2*n3;i1++) masktop[i1] = 1.;
	}
	/* read botmask grid if specified */
	if (getparstring("botmask",&botmask)) {
		botmaskfp = efopen(botmask,"r");
		ierr = fgetusghdr(botmaskfp,&usghbotmask);
      		if(ierr!=0) err(" botmask grid header error ");
		if(usghin.n2!=usghbotmask.n1) err("check botmask header n1");
		if(usghin.n3!=usghbotmask.n2) err("check botmask header n2");
		if(usghin.o2!=usghbotmask.o1) err("check botmask header o1");
		if(usghin.o3!=usghbotmask.o2) err("check botmask header o2");
		if(usghin.d2!=usghbotmask.d1) err("check botmask header d1");
		if(usghin.d3!=usghbotmask.d2) err("check botmask header d2");
		efseek(botmaskfp,0,0);
		efread(maskbot,sizeof(float),n2*n3,botmaskfp);
	} else {
		for(i1=0;i1<n2*n3;i1++) maskbot[i1] = 1.;
	}
	if (!getparfloat("thicknull",&thicknull)) thicknull=-77777;

	for(i3=0;i3<n3;i3++) { 
		for(i2=0;i2<n2;i2++) {
			efread(grid,sizeof(float),n1,infp);
			/*
			fprintf(stderr," read input at i2=%d i3=%d \n",i2,i3);
			*/
			dz[i2+i3*n2] = zbot[i2+i3*n2] - ztop[i2+i3*n2];
			tmp = (ztop[i2+i3*n2]-o1)/d1;
			it = tmp;
			tmp = (zbot[i2+i3*n2]-o1)/d1;
			ib = tmp;
			saltdz = 0;
			if(it<0) it=0; if(it>=n1) it=n1-1;
			if(ib<0) ib=0; if(ib>=n1) ib=n1-1;

			for(i1=it;i1<ib;i1++) {
				if(i1>=0 && i1<n1) {
					if(fabs(grid[i1]-gridnull)<=tol) {
						dz[i2+i3*n2] -= d1;
						saltdz += d1;
					}
				}
			}
			if((masktop[i2+i3*n2]+maskbot[i2+i3*n2])==1) {
				if(saltdz==0.) dz[i2+i3*n2] = thicknull;
			} else if((masktop[i2+i3*n2]+maskbot[i2+i3*n2])==0) {
				dz[i2+i3*n2] = thicknull;
			}
		}
	}
	gmin = dz[0];
	gmax = dz[0];

	for(i1=0;i1<n2*n3;i1++) {
		if(gmin>dz[i1] && dz[i1]!=thicknull) gmin = dz[i1];
		if(gmax<dz[i1] && dz[i1]!=thicknull) gmax = dz[i1];
	}


	efwrite(dz,sizeof(float),n2*n3,outfp);
	usghin.n1 = n2;
	usghin.n2 = n3;
	usghin.n3 = 1;
	usghin.d1 = d2;
	usghin.d2 = d3;
	usghin.d3 = 1;
	usghin.o1 = o2;
	usghin.o2 = o3;
	usghin.o3 = 0;
	ierr = fputusghdr(outfp,&usghin);
    	if(ierr!=0) err(" output grid header error ");


	free(ztop);
	free(zbot);
	free(grid);
	free(dz);

	exit(0);
}
