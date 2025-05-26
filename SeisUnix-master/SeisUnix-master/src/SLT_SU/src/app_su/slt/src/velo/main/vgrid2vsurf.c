char *sdoc =
"VGRID2VSURF - obtain velocity values along a landmark horizon \n"
"\n"
"vgrid2vsurf <infile [required parameters] \n"
"\n"
"Required Parameters:\n"
"infile=stdin   name of the input 3D velocity grid file	\n"
"outfile=       name of the output landmark velocity file(s)	\n"
"lmkfile=       name of the input landmark file defining the horizon \n"
"topgrid=       name of the grid defined top surface \n"
"Optional Parameters: \n"
"xlpos=2        column position of landmark file defining output x  \n"
"ylpos=1        column position of landmark file defining output y  \n"
"tzpos=5        column position of landmark file defining output t or z \n"
"               also the column position of output grid value	\n" 
"nlayer=1       number of layers to output between topgrid and lmkfile \n"
"               =1 only output at lmkfile \n"
"               =n output at top+del,top+2*del,..,Zlmkfile \n"
"                  where del=(Zlmkfile-top)/n \n"
"                  output files are outfile_1, outfile_2,...,outfile_n \n"
"Note:			\n"
"1. \n"
" o1, d1 and n1 in input grid (infile) define grid's t or z coordinates \n"
" o2, d2 and n2 in input grid (infile) define grid's x coordinates \n"
" o3, d3 and n3 in input grid (infile) define grid's y coordinates \n"
" o1, d1 and n1 in topgrid define topgrid's x coordinates \n"
" o2, d2 and n2 in topgrid define topgrid's y coordinates \n"
"2. velocity value along the landmark horizon is estimated \n"
"   by linearly fit the velocity grid values between the topgrid \n"
"   and the landmark horizon \n"
"\n"
"AUTHOR:  Zhiming Li,         12/9/96			\n"
"\n";
#include "usgrid.h"
#include "par.h"

main(int argc, char **argv)
{
	usghed ugh,ughtop;
	FILE *infp,*lmkfp,*topfp;
	char *infile, *outfile, *lmkfile, *topgrid; 
	string *outfilei;
	FILE **outfpi;
	float *grids, *grid, *top;
	int ierr, nf, nc;
	char *cbuf; 
	float *fbuf;
	int xlpos=2,ylpos=1,tzpos=5;
	int nlayer;

	int n1,n2,n3;
	float d1,d2,d3;
	float o1,o2,o3;

	float xl, yl, tz, tmp, sc, val;
	int one=1, itmp; 
	float ztop;
	float *x, *y, a, b;
	float z0, zn, del;
	int iz0, izn, iz, nz; 
	int ilayer; 

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("infile",&infile)) {
		infp = efopen(infile,"r");
	} else {
		infp = stdin;
	}
	ierr = fgetusghdr(infp,&ugh);
    if(ierr!=0) err(" input grid header error ");
	if(!getparstring("outfile",&outfile)) {
		err(" outfile missing");
	}
	if(!getparstring("lmkfile",&lmkfile)) err(" lmkfile missing ");
	lmkfp = efopen(lmkfile,"r");
	if(!getparstring("topgrid",&topgrid)) err(" topgrid missing ");
	topfp = efopen(topgrid,"r");
	ierr = fgetusghdr(topfp,&ughtop);
    if(ierr!=0) err(" topgrid header error ");

	if( !getparint("xlpos",&xlpos) ) xlpos=2; xlpos -= 1;
	if( !getparint("ylpos",&ylpos) ) ylpos=1; ylpos -= 1;
	if( !getparint("tzpos",&tzpos) ) tzpos=5; tzpos -= 1;
	if( !getparint("nlayer",&nlayer) ) nlayer=1;

	outfilei = (string *)malloc(nlayer*sizeof(string));
	outfpi = (FILE**) malloc(nlayer*sizeof(FILE *));
	if(nlayer==1) {
		outfpi[0] = efopen(outfile,"w");
	} else {
		for(ilayer=0;ilayer<nlayer;ilayer++) {
			sprintf(outfilei[ilayer],"%s_%d\0",outfile,ilayer+1);
			outfpi[ilayer] = efopen(outfilei[ilayer],"w");
		}
	}

	n1 = ugh.n1;
	n2 = ugh.n2;
	n3 = ugh.n3;
	o1 = ugh.o1;
	o2 = ugh.o2;
	o3 = ugh.o3;
	d1 = ugh.d1;
	d2 = ugh.d2;
	d3 = ugh.d3;

	if(ughtop.o1!=o2) err(" check o1 of topgrid \n");
	if(ughtop.d1!=d2) err(" check d1 of topgrid \n");
	if(ughtop.n1!=n2) err(" check n1 of topgrid \n");
	if(ughtop.o2!=o3) err(" check o2 of topgrid \n");
	if(ughtop.d2!=d3) err(" check d2 of topgrid \n");
	if(ughtop.n2!=n3) err(" check n2 of topgrid \n");

	
	nf = 10;
	nc = 200;
	/* memory allocations */
	grids = (float*) emalloc(n1*n2*n3*sizeof(float));
	x = (float*) emalloc(n1*sizeof(float));
	y = (float*) emalloc(n1*sizeof(float));
	top = (float*) emalloc(n2*n3*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));
	fbuf = (float *) malloc(nf*sizeof(float));
	cbuf = (char *) emalloc(nc*sizeof(char));

	efread(grids,sizeof(float),n1*n2*n3,infp);
	efread(top,sizeof(float),n2*n3,topfp);

	fgets(cbuf,nc,lmkfp);
	do {
		sscanf(cbuf,"%f %f %f %f %f",
					&fbuf[0],&fbuf[1],&fbuf[2],&fbuf[3],&fbuf[4]);
		xl = fbuf[xlpos];
		yl = fbuf[ylpos];
		tz = fbuf[tzpos];
		bilint_(&n1,&n2,&n3,&o2,&o3,&d2,&d3,&xl,&yl,grids,grid);

		bilint_(&one,&n2,&n3,&o2,&o3,&d2,&d3,&xl,&yl,top,&ztop);

		del = (tz - ztop)/nlayer; 
		
		for(ilayer=0;ilayer<nlayer;ilayer++) {

			z0 = ztop + ilayer * del; 
			zn = ztop + (ilayer+1) * del; 

			tmp = (z0-o1)/d1;
			iz0 = tmp;
			if(iz0<0) iz0 = 0;
			if(iz0>=n1) iz0 = n1-1;

			tmp = (zn-o1)/d1;
			izn = tmp;
			if(izn<0) izn = 0;
			if(izn>=n1) izn = n1-1;

			if(izn<=iz0) {
				val = grid[izn];
			} else {
				for(iz=iz0;iz<izn;iz++) {
					x[iz-iz0] = o1+iz*d1;
					y[iz-iz0] = grid[iz];
				}
				nz = izn - iz0 + 1;
				linefit(x,y,nz,&a,&b);
				val = a + (izn-iz0)*d1*b;

				fprintf(stderr,"a=%g b=%g xl=%g yl=%g tz=%g val=%g viz0=%g vizn=%g\n",
					a,b,xl,yl,tz,val,grid[iz0],grid[izn]);
			}

			fbuf[tzpos] = val;
			fprintf(outfpi[ilayer],
				"                    %10.2f%10.2f%12.2f%12.2f%12.4f \n",
				fbuf[0],fbuf[1],fbuf[2],fbuf[3],fbuf[4]); 
		}
		bzero(cbuf,nc);
	}while(fgets(cbuf,nc,lmkfp));

	free(outfpi);
	free(outfilei);
	free(top);
	free(x);
	free(y);
	free(grids);
	free(grid);
	free(cbuf);
	free(fbuf);
	
	exit(0);
}
