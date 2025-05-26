char *sdoc =
"GRIDVELO - 3D velocity (attribute) grid building within a layer \n"
"\n"
"gridvelo <infile >outfile [optional parameters]\n"
"\n"
"Required Parameters:\n"
"infile=        name of the input 3D attribute grid file	(velocity)	\n"
"outfile=       name of the output 3D attribute grid file (velocity) \n"
"velotop=       velocity (attribute) value at top of layer		\n"
"velobot=       velocity (attribute) value at bottom of layer		\n"
"or the spatial variant velotop and velobot given by \n"
"vgridtop=      velocity 2D grid at top of layer        \n"
"vgridbot=      velocity 2D grid at bottom of layer     \n"
"               if vgridtop is specified, velotop is ignored \n" 
"               if vgridbot is specified, velobot is ignored \n" 
"depthnull=     depth value to be used where there is no presence of the layer \n"
"               in the depth grid files when the grid value is greater or equal \n"
"               to depthnull 					\n"
"toplayer=      name of the 2D grid defining the top of the layer \n"
 "Optional Parameters:\n"
"botlayer=      name of the 2D grid defining the bottom of the layer \n"
"               (if not given,  default to below the bottom of the model)	\n"
"\n"
"AUTHOR:  Zhiming Li,         12/05/95			\n"
"\n";
#include "velo.h"
#include "usgrid.h"
#include "par.h"

int main(int argc, char **argv)
{
	usghed usghin, usghtop, usghbot;
	usghed usghvgt, usghvgb;
	FILE *infp,*outfp,*topfp,*botfp;
	char *infile,*outfile,*toplayer,*botlayer;
	FILE *vgtfp,*vgbfp;
	char *vgtfile,*vgbfile;
	float velotop, velobot, depthnull, zmax;
	int ibot, itop;
	int ierr;

	int n1,n2,n3;
	int i1,i2,i3;
	float d1,o1;

	float *grid, *ztop, *zbot, gmin, gmax;
	float top, bot;
	float tmp, grad;
	int i1top, i1bot;

	float *vgt, *vgb;

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

	if(!getparfloat("depthnull",&depthnull)) err(" depthnull missing ");


	if (getparstring("toplayer",&toplayer)) {
		topfp = efopen(toplayer,"r");
		ierr = fgetusghdr(topfp,&usghtop);
      		if(ierr!=0) err(" toplayer grid header error ");
	} else {
		err(" toplayer missing ");
	}

	if (getparstring("botlayer",&botlayer)) {
		botfp = efopen(botlayer,"r");
		ierr = fgetusghdr(botfp,&usghbot);
      	if(ierr!=0) err(" botlayer grid header error ");
		ibot = 1;	
	} else {
		ibot = 0;
	}

	n1 = usghin.n1;
	n2 = usghin.n2;
	n3 = usghin.n3;
	o1 = usghin.o1;
	d1 = usghin.d1;
	gmin = usghin.gmin;
	gmax = usghin.gmax;

	zmax = o1 + n1*d1;

	/* memory allocations */
	ztop = (float*) emalloc(n2*n3*sizeof(float));
	zbot = (float*) emalloc(n2*n3*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));
	vgt = (float*) emalloc(n2*n3*sizeof(float));
	vgb = (float*) emalloc(n2*n3*sizeof(float));

	if (getparstring("vgridtop",&vgtfile)) {
		vgtfp = efopen(vgtfile,"r");
		ierr = fgetusghdr(vgtfp,&usghvgt);
      	if(ierr!=0) err(" vgridtop grid header error ");
		if(usghin.n2!=usghvgt.n1) err("check vgridtop grid header n1");
		if(usghin.n3!=usghvgt.n2) err("check vgridtop grid header n2");
		if(usghin.o2!=usghvgt.o1) err("check vgridtop grid header o1");
		if(usghin.o3!=usghvgt.o2) err("check vgridtop grid header o2");
		if(usghin.d2!=usghvgt.d1) err("check vgridtop grid header d1");
		if(usghin.d3!=usghvgt.d2) err("check vgridtop grid header d2");
		efseek(vgtfp,0,0);
		efread(vgt,sizeof(float),n2*n3,vgtfp);
	} else {
		if(!getparfloat("velotop",&velotop)) err(" velotop missing ");
		for(i2=0;i2<n2*n3;i2++) vgt[i2] = velotop;
	}

	if (getparstring("vgridbot",&vgbfile)) {
		vgbfp = efopen(vgbfile,"r");
		ierr = fgetusghdr(vgbfp,&usghvgb);
      	if(ierr!=0) err(" vgridbot grid header error ");
		if(usghin.n2!=usghvgb.n1) err("check vgridbot grid header n1");
		if(usghin.n3!=usghvgb.n2) err("check vgridbot grid header n2");
		if(usghin.o2!=usghvgb.o1) err("check vgridbot grid header o1");
		if(usghin.o3!=usghvgb.o2) err("check vgridbot grid header o2");
		if(usghin.d2!=usghvgb.d1) err("check vgridbot grid header d1");
		if(usghin.d3!=usghvgb.d2) err("check vgridbot grid header d2");
		efseek(vgbfp,0,0);
		efread(vgb,sizeof(float),n2*n3,vgbfp);
	} else {
		if(!getparfloat("velobot",&velobot)) err(" velobot missing ");
		for(i2=0;i2<n2*n3;i2++) vgb[i2] = velobot;
	}
	
	if(usghin.n2!=usghtop.n1) err("check top grid header n1");
	if(usghin.n3!=usghtop.n2) err("check top grid header n2");
	if(usghin.o2!=usghtop.o1) err("check top grid header o1");
	if(usghin.o3!=usghtop.o2) err("check top grid header o2");
	if(usghin.d2!=usghtop.d1) err("check top grid header d1");
	if(usghin.d3!=usghtop.d2) err("check top grid header d2");
	efseek(topfp,0,0);
	efread(ztop,sizeof(float),n2*n3,topfp);


	if(ibot==1) {
		if(usghin.n2!=usghbot.n1) err("check bot grid header n1");
		if(usghin.n3!=usghbot.n2) err("check bot grid header n2");
		if(usghin.o2!=usghbot.o1) err("check bot grid header o1");
		if(usghin.o3!=usghbot.o2) err("check bot grid header o2");
		if(usghin.d2!=usghbot.d1) err("check bot grid header d1");
		if(usghin.d3!=usghbot.d2) err("check bot grid header d2");
		efseek(botfp,0,0);
		efread(zbot,sizeof(float),n2*n3,botfp);
	} else {
		for(i1=0;i1<n2*n3;i1++) zbot[i1] = o1+(n1-1)*d1;
	}

	efseek(infp,0,0);
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {

			efread(grid,sizeof(float),n1,infp);

			top = ztop[i3*n2+i2];
			if(ibot==1) bot = zbot[i3*n2+i2];

			tmp = (top - o1)/d1 + 0.5; 
			i1top = tmp;
			tmp = (bot - o1)/d1 + 0.5; 
			i1bot = tmp;

			if(top==depthnull) i1top = n1;
			if(ibot==1 && bot==depthnull) i1bot = 0;
			if(ibot==0) i1bot=n1;

			velotop = vgt[i2+i3*n2];
			velobot = vgb[i2+i3*n2];

			/* grid between top and bot */
			if(i1bot>i1top) {
				grad = (velobot - velotop)/(i1bot-i1top);
			} else {
				grad = 0.;
			}

			for(i1=i1top;i1<i1bot;i1++) {
				if(i1>=0 && i1<=n1-1) {
					grid[i1] = velotop + (i1-i1top)*grad;
				}
			}

			for(i1=0;i1<n1;i1++) {
				if(gmin>grid[i1]) gmin = grid[i1]; 
				if(gmax<grid[i1]) gmax = grid[i1]; 
			}
			efwrite(grid,sizeof(float),n1,outfp);
		}
	}


	usghin.gmin = gmin;
	usghin.gmax = gmax;

	ierr = fputusghdr(outfp,&usghin);
	
	free(ztop);
	free(zbot);
	free(grid);
	free(vgt);
	free(vgb);
	exit(0);
}
