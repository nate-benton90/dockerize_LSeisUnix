#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc =
"GRIDDEFORM - 3D grid deform between two horizons \n"
"\n"
"griddeform <infile >outfile [parameters]\n"
"\n"
"Required Parameters:\n"
"infile=        name of the input 3D grid file	(velocity)	\n"
"outfile=       name of the output 3D grid file (velocity) \n"
"topgridi=      name of the 2D grid defining the top of layer in input \n"
"botgridi=      name of the 2D grid defining the bottom of layer in input \n"
"topgrido=      name of the 2D grid defining the top of layer in output \n"
"botgrido=      name of the 2D grid defining the bottom of layer in output \n"
"Optional Parameters:\n"
"None \n"
"Notes:\n"
" 1.            input grids between topgridi and botgridi will be \n"
"               uniformly deformed and output between topgrido and botgrido \n" 
" 2.            re-assign values above topgrido or below botgrido may be \n"
"               necessary (using gridsalt or gridsediment)	\n"
" 3.            1st and 2nd dimensions of topgridi, botgridi, topgrido \n"
"               and botgrido must be the same as 2nd and 3rd \n"
"               dimensions of the input grid \n"
"AUTHOR:  Zhiming Li,         5/1/2002			\n"
"\n";

main(int argc, char **argv)
{
	usghed usghin, usghtopi, usghboti, usghtopo, usghboto;
	FILE *infp,*outfp,*topfpi,*botfpi,*topfpo,*botfpo;
	char *infile,*outfile,*topgridi,*botgridi,*topgrido,*botgrido;
	int ierr;

	int n1,n2,n3;
	int i1,i2,i3;
	float d1,o1;
	float gmin, gmax;
	float z, zi, zo, dzo, dzi, tmp, r;
	int itopi, iboti, itmp;

	float *topi,*boti,*topo,*boto, *grid, *grido;

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

	if (getparstring("topgridi",&topgridi)) {
		topfpi = efopen(topgridi,"r");
		ierr = fgetusghdr(topfpi,&usghtopi);
      		if(ierr!=0) err(" topgridi header error ");
	} else {
		err(" topgridi missing ");
	}
	if (getparstring("botgridi",&botgridi)) {
		botfpi = efopen(botgridi,"r");
		ierr = fgetusghdr(botfpi,&usghboti);
      		if(ierr!=0) err(" botgridi header error ");
	} else {
		err(" botgridi missing ");
	}
	if (getparstring("topgrido",&topgrido)) {
		topfpo = efopen(topgrido,"r");
		ierr = fgetusghdr(topfpo,&usghtopo);
      		if(ierr!=0) err(" topgrido header error ");
	} else {
		err(" topgrido missing ");
	}
	if (getparstring("botgrido",&botgrido)) {
		botfpo = efopen(botgrido,"r");
		ierr = fgetusghdr(botfpo,&usghboto);
      		if(ierr!=0) err(" botgrido header error ");
	} else {
		err(" botgrido missing ");
	}


	n1 = usghin.n1;
	n2 = usghin.n2;
	n3 = usghin.n3;
	o1 = usghin.o1;
	d1 = usghin.d1;
	gmin = usghin.gmin;
	gmax = usghin.gmax;

	/* memory allocations */
	grid = (float*) emalloc(n1*sizeof(float));
	grido = (float*) emalloc(n1*sizeof(float));
	topi = (float*) emalloc(n2*n3*sizeof(float));
	topo = (float*) emalloc(n2*n3*sizeof(float));
	boti = (float*) emalloc(n2*n3*sizeof(float));
	boto = (float*) emalloc(n2*n3*sizeof(float));
	
	if(usghin.n2!=usghtopi.n1) err("check topgridi header n1");
	if(usghin.n3!=usghtopi.n2) err("check topgridi header n2");
	if(usghin.o2!=usghtopi.o1) 
		err("check topgridi header o1=%f and grid input o2=%f",
			usghtopi.o1,usghin.o2);
	if(usghin.o3!=usghtopi.o2) err("check topgridi header o2");
	if(usghin.d2!=usghtopi.d1) err("check topgridi header d1");
	if(usghin.d3!=usghtopi.d2) err("check topgridi header d2");

	if(usghin.n2!=usghboti.n1) err("check botgridi header n1");
	if(usghin.n3!=usghboti.n2) err("check botgridi header n2");
	if(usghin.o2!=usghboti.o1) 
		err("check botgridi header o1=%f and grid input o2=%f",
			usghboti.o1,usghin.o2);
	if(usghin.o3!=usghboti.o2) err("check botgridi header o2");
	if(usghin.d2!=usghboti.d1) err("check botgridi header d1");
	if(usghin.d3!=usghboti.d2) err("check botgridi header d2");

	if(usghin.n2!=usghtopo.n1) err("check topgrido header n1");
	if(usghin.n3!=usghtopo.n2) err("check topgrido header n2");
	if(usghin.o2!=usghtopo.o1) 
		err("check topgrido header o1=%f and grid input o2=%f",
			usghtopo.o1,usghin.o2);
	if(usghin.o3!=usghtopo.o2) err("check topgrido header o2");
	if(usghin.d2!=usghtopo.d1) err("check topgrido header d1");
	if(usghin.d3!=usghtopo.d2) err("check topgrido header d2");

	if(usghin.n2!=usghboto.n1) err("check botgrido header n1");
	if(usghin.n3!=usghboto.n2) err("check botgrido header n2");
	if(usghin.o2!=usghboto.o1) 
		err("check botgrido header o1=%f and grid input o2=%f",
			usghboto.o1,usghin.o2);
	if(usghin.o3!=usghboto.o2) err("check botgrido header o2");
	if(usghin.d2!=usghboto.d1) err("check botgrido header d1");
	if(usghin.d3!=usghboto.d2) err("check botgrido header d2");

	efseek(topfpi,0,0);
	efread(topi,sizeof(float),n2*n3,topfpi);
	efseek(botfpi,0,0);
	efread(boti,sizeof(float),n2*n3,botfpi);
	efseek(topfpo,0,0);
	efread(topo,sizeof(float),n2*n3,topfpo);
	efseek(botfpo,0,0);
	efread(boto,sizeof(float),n2*n3,botfpo);

	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {

			fread(grid,sizeof(float),n1,infp);

			dzi = boti[i3*n2+i2] - topi[i3*n2+i2];
			if(dzi<0.) 
			err(" topgridi and botgridi same at i1=%d i2=%d \n",i2+1,i3+1);

			dzo = boto[i3*n2+i2] - topo[i3*n2+i2];
			if(dzo<0.) 
			err(" topgrido and botgrido same at i1=%d i2=%d \n",i2+1,i3+1);

			tmp = (topi[i3*n2+i2]-o1)/d1;
			itopi = tmp;

			tmp = (boti[i3*n2+i2]-o1)/d1;
			iboti = tmp;

			if(itopi<0) itopi=0;
			if(iboti>(n1-1)) iboti=n1-1;

			for(i1=0;i1<n1;i1++) {
				z = o1 + i1 * d1;
				if(z>topo[i3*n2+i2] && z<boto[i3*n2+i2]) {
					zo = z - topo[i3*n2+i2];
					r = zo/dzo;
					zi = r * dzi;
					tmp = zi + topi[i3*n2+i2];
					if(tmp<topi[i3*n2+i2]) tmp = topi[i3*n2+i2];
					if(tmp>boti[i3*n2+i2]) tmp = boti[i3*n2+i2];
					tmp = (tmp - o1)/d1;
					itmp = tmp;
					/*
					fprintf(stderr,"zi=%f zo=%f topi=%f topo=%f itmp=%d\n",
						zi,zo,topi[i3*n2+i2],topo[i3*n2+i2],itmp);
						*/
					if(itmp<itopi){
						grido[i1] = grid[itopi];
					} else if (itmp>=iboti) {
						grido[i1] = grid[iboti];
					} else {
						grido[i1] = grid[itmp]+(tmp-itmp)*(grid[itmp+1]-grid[itmp]);
					}
				} else {
					grido[i1] = grid[i1];
				}
			}

			if(i3==0 && i2==0) {
				gmax = grido[0]; 
				gmin = grido[0];
			}
			for(i1=0;i1<n1;i1++) {
				if(grido[i1]>gmax) gmax = grido[i1];
				if(grido[i1]<gmin) gmin = grido[i1];
			}

			fwrite(grido,sizeof(float),n1,outfp);

		}
	}


	usghin.gmin = gmin;
	usghin.gmax = gmax;

	ierr = fputusghdr(outfp,&usghin);
	
	free(topi);
	free(boti);
	free(topo);
	free(boto);
	free(grid);
	free(grido);

	exit(0);
}
