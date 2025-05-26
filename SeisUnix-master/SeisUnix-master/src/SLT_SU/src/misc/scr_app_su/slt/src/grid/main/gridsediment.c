#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc =
"GRIDSEDIMENT - 3D attribute grid building within sediment layers \n"
"\n"
"gridsediemnt <infile >outfile [optional parameters]\n"
"\n"
"Required Parameters:\n"
"infile=        name of the input 3D attribute grid file	(velocity)	\n"
"outfile=       name of the output 3D attribute grid file (velocity) \n"
"layertop=      name of the 2D depth grid defining the top of the layer \n"
"layerbot=      name of the 2D depth grid defining the bottom of the layer \n"
"Or layers=     name of the 2D depth grids defining the layer boundaries \n"
"               (specified as layers=layer1 layers=layer2 ... layers=layerN) \n"
"               (order from shallow to deep) \n"
 "Optional Parameters:\n"
"nz=10          number of mini layers between top and bottom layers \n"
"               where grid smoothing along mini layers will be applied \n"
"dzrl=          relative thinkness of the mini layers \n"
"               default: each mini layer will have the same thickness \n"
"                        from top layer to bottom layer \n"
"               otherwise specified as array: \n"
"                 i.e., dzrl=1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10  \n"
"                 10th layer is 10 times the thickness of the 1st one \n"
"                 8th layer is 4 times the thickness of the 2nd one \n"
"                 ... \n"
"               When layers are specified, nz and dzrl will be ignored \n"
"               (nz and dzrl will be determined from number of layers \n"
"               specified) \n"
"sm2top=0       smoothing window length in the x (2nd) dimension along the \n"
"               the layer at the top layer \n" 
"sm2bot=0       smoothing window length in the x (2nd) dimension along the \n"
"               the layer at the bottom layer \n" 
"sm3top=0       smoothing window length in the y (3rd) dimension along the \n"
"               the layer at the top layer \n" 
"sm3bot=0       smoothing window length in the y (3rd) dimension along the \n"
"               the layer at the bottom layer \n" 
"gtopgrid=      grid value file at the top of the layer \n" 
"gbotgrid=      grid value file at the bottom of the layer \n" 
"               if BOTH gtopgrid and gbotgrid are specified, output grid  \n"
"               will be computed from gtopgrid and gbotgrid. \n"
"               otherwise,if g0 and r0 are both specified, constant gradient \n"
"               model within the layer will be used \n"
"               if gtopgrid and gbotgrid, or g0 and r0, are not specified, \n"
"               input grid will be used to compute the output \n"
"ginterp=0      grid interpolation between gtopgrid and gbotgrid \n"
"               =0  interpolate according to mini layer thinkness \n"
"               =1  interpolate evenly then assagn  to layers \n"
"g0=            constant grid value at the top of the layer \n"
"r0=            constant grid gradient from top to bottom of the layer \n"
"               grid(z_from_top) = g0 + r0 * z_from_top \n"
"Or \n"
"glayers=       grid values at the layers \n"
"               (specified as glayers=glayer1 glayers=glayer2 ... glayers=glayerN; \n"
"                when galyers is specified, the number of glayers must match the \n"
"                number of layers, gtopgrid,gbotgrid,ginterp,go,r0 will be ignored) \n"
"gabovetop=     grid value used above layertop \n"
"               if not specified, input grid values will be used above the \n"
"               layertop horizon \n"
"gbelowbot=     grid value used below layerbot \n"
"               if not specified, input grid values will be used below the \n"
"               layerbot horizon \n"
" Notes:						\n"
"    1. The program will compute the grid values along each mini layers \n"
"       within the top and the bottom layer (if not input) and apply smoothing along \n"
"       the mini layers. The output velocity grid within the top and the \n"
"       bottom layers will be linearly interpolated between the smoothed \n"
"       grid values on the mini layers. \n"
"    2. Smoothing window lengths along the x (2nd) and y (3rd) at each \n"
"       each mini layer will be linearly interpolated from sm2top,  sm2bot \n"
"       sm3top and sm3bot. \n"
"    3. Number of data points used to smooth equals smoothing window lengths \n"
"       divided by d2 or d3 in the input grid header \n"
"\n"
"AUTHOR:  Zhiming Li,         2/12/2000			\n"
"\n";

main(int argc, char **argv)
{
	usghed usghin, usghtop, usghbot, usghlayer;
	usghed usghvtop, usghvbot, usghglayer;
	FILE *infp,*outfp,*topfp,*botfp,*layerfp;
	FILE *vtopfp,*vbotfp,*glayerfp;
	char *infile,*outfile,*layertop, *layerbot;
	char *gtopgrid, *gbotgrid;
	char *layers, *glayers;
	int ibot, itop;
	int ierr;
	int nz, iz;
	float *dzrl, sm2top, sm2bot, sm3top, sm3bot;
	float gabovetop, gbelowbot;
	int igabovetop, igbelowbot;
	float g0, r0;
	int ginterp=0;

	int n1,n2,n3;
	int i1,i2,i3;
	float d1,o1,d2,d3;

	float *grid, *ztop, *zbot, gmin, gmax;
	float *vtop, *vbot;
	float top, bot;
	float tmp;
	int i1top, i1bot, itmp;
	int ivtop, ivbot;

	float *sm2s, *sm3s;
	float *work, *fsmx, *fsmy, *vs, *zs;
	float z, scale, zscale, vscale;
	int ismx, ismy;

	int nlayer, nglayer;


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

	nlayer = 0;
	nglayer = 0;
        nlayer = countparname("layers");
	if(nlayer==1) err(" at least 2 layers are needed \n");
        nglayer = countparname("glayers");

	if(nlayer==0) {
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
	} else {
		if(nlayer!=nglayer && nglayer>0 ) 
		   err(" %d layers not matching %d glayers \n",nlayer,nglayer);
	}

	ivtop = 0;
	if (getparstring("gtopgrid",&gtopgrid)) {
		vtopfp = efopen(gtopgrid,"r");
		ierr = fgetusghdr(vtopfp,&usghvtop);
      		if(ierr!=0) err(" gtopgrid header error ");
		ivtop = 1;
	}
	ivbot = 0;
	if(getparstring("gbotgrid",&gbotgrid) ) {
		vbotfp = efopen(gbotgrid,"r");
		ierr = fgetusghdr(vbotfp,&usghvbot);
      		if(ierr!=0) err(" gbotgrid header error ");
		ivbot = 1;
	}
	if(!getparint("ginterp",&ginterp)) ginterp=0;

	if(ivtop==0 || ivbot==0) {
		if ( getparfloat("g0",&g0) && getparfloat("r0",&r0) ) {
			ivtop = -1; ivbot = -1;
		}
	}

	if( !getparint("nz",&nz) ) nz=10;
	if(nlayer!=0) nz = nlayer - 1;
	dzrl = emalloc(nz*sizeof(float));
	sm2s = emalloc((nz+1)*sizeof(float));
	sm3s = emalloc((nz+1)*sizeof(float));

	if( countparval("dzrl")>0 && nz!=countparval("dzrl") ) {
		err( " number of dzrl elements must match nz=%d \n",nz);
	} else if( countparval("dzrl")==0 ) {
		for(iz=0;iz<nz;iz++) dzrl[iz] = 1.;
	} else if( countparval("dzrl")==nz) {
		getparfloat("dzrl",dzrl);
	}

	if( !getparfloat("sm2top",&sm2top) ) sm2top=0.;
	if( !getparfloat("sm3top",&sm3top) ) sm3top=0.;
	if( !getparfloat("sm2bot",&sm2bot) ) sm2bot=0.;
	if( !getparfloat("sm3bot",&sm3bot) ) sm3bot=0.;

	igabovetop = 1;
	if( !getparfloat("gabovetop",&gabovetop) ) igabovetop=0;
	igbelowbot = 1;
	if( !getparfloat("gbelowbot",&gbelowbot) ) igbelowbot=0;


	n1 = usghin.n1;
	n2 = usghin.n2;
	n3 = usghin.n3;
	o1 = usghin.o1;
	d1 = usghin.d1;
	d2 = usghin.d2;
	d3 = usghin.d3;
	gmin = usghin.gmin;
	gmax = usghin.gmax;

	/* memory allocations */
	ztop = (float*) emalloc(n2*n3*sizeof(float));
	zbot = (float*) emalloc(n2*n3*sizeof(float));
	vtop = (float*) emalloc(n2*n3*sizeof(float));
	vbot = (float*) emalloc(n2*n3*sizeof(float));
	zs = (float*) emalloc(n2*n3*(nz+1)*sizeof(float));
	vs = (float*) emalloc(n2*n3*(nz+1)*sizeof(float));
	work = (float*) emalloc(n2*n3*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));
	
	if(nlayer==0) {
		if(usghin.n2!=usghtop.n1) err("check layertop header n1");
		if(usghin.n3!=usghtop.n2) err("check layertop header n2");
		if(usghin.o2!=usghtop.o1) err("check layertop header o1");
		if(usghin.o3!=usghtop.o2) err("check layertop header o2");
		if(usghin.d2!=usghtop.d1) err("check layertop header d1");
		if(usghin.d3!=usghtop.d2) err("check layertop header d2");
		efseek(topfp,0,0);
		efread(ztop,sizeof(float),n2*n3,topfp);

		if(usghin.n2!=usghbot.n1) err("check layerbot header n1");
		if(usghin.n3!=usghbot.n2) err("check layerbot header n2");
		if(usghin.o2!=usghbot.o1) err("check layerbot header o1");
		if(usghin.o3!=usghbot.o2) err("check layerbot header o2");
		if(usghin.d2!=usghbot.d1) err("check layerbot header d1");
		if(usghin.d3!=usghbot.d2) err("check layerbot header d2");
		efseek(botfp,0,0);
		efread(zbot,sizeof(float),n2*n3,botfp);

		if(ivtop==1) {
		if(usghin.n2!=usghvtop.n1) err("check gtopgrid header n1");
		if(usghin.n3!=usghvtop.n2) err("check gtopgrid header n2");
		if(usghin.o2!=usghvtop.o1) err("check gtopgrid header o1");
		if(usghin.o3!=usghvtop.o2) err("check gtopgrid header o2");
		if(usghin.d2!=usghvtop.d1) err("check gtopgrid header d1");
		if(usghin.d3!=usghvtop.d2) err("check gtopgrid header d2");
		efseek(vtopfp,0,0);
		efread(vtop,sizeof(float),n2*n3,vtopfp);
		} 

		if(ivbot==1) {
		if(usghin.n2!=usghvbot.n1) err("check gbotgrid header n1");
		if(usghin.n3!=usghvbot.n2) err("check gbotgrid header n2");
		if(usghin.o2!=usghvbot.o1) err("check gbotgrid header o1");
		if(usghin.o3!=usghvbot.o2) err("check gbotgrid header o2");
		if(usghin.d2!=usghvbot.d1) err("check gbotgrid header d1");
		if(usghin.d3!=usghvbot.d2) err("check gbotgrid header d2");
		efseek(vbotfp,0,0);
		efread(vbot,sizeof(float),n2*n3,vbotfp);
		}
	} else {
		for(iz=0;iz<nlayer;iz++) {
			getnparstring(iz+1,"layers",&layers);
			layerfp=efopen(layers,"r");
			ierr = fgetusghdr(layerfp,&usghlayer);
			if(ierr!=0) err(" error open layers=%s \n",layers);
			if(usghin.n2!=usghlayer.n1) err("check %s header n1",layers);
			if(usghin.n3!=usghlayer.n2) err("check %s header n2",layers);
			if(usghin.o2!=usghlayer.o1) err("check %s header o1",layers);
			if(usghin.o3!=usghlayer.o2) err("check %s header o2",layers);
			if(usghin.d2!=usghlayer.d1) err("check %s header d1",layers);
			if(usghin.d3!=usghlayer.d2) err("check %s header d2",layers);
			efseek(layerfp,0,0);
			efread(zs+iz*n2*n3,sizeof(float),n2*n3,layerfp);
			efclose(layerfp);
		}
		for(iz=0;iz<nglayer;iz++) {
			getnparstring(iz+1,"glayers",&glayers);
			glayerfp=efopen(glayers,"r");
			ierr = fgetusghdr(glayerfp,&usghglayer);
			if(ierr!=0) err(" error open layers=%s \n",layers);
			if(usghin.n2!=usghglayer.n1) err("check %s header n1",glayers);
			if(usghin.n3!=usghglayer.n2) err("check %s header n2",glayers);
			if(usghin.o2!=usghglayer.o1) err("check %s header o1",glayers);
			if(usghin.o3!=usghglayer.o2) err("check %s header o2",glayers);
			if(usghin.d2!=usghglayer.d1) err("check %s header d1",glayers);
			if(usghin.d3!=usghglayer.d2) err("check %s header d2",glayers);
			efseek(glayerfp,0,0);
			efread(vs+iz*n2*n3,sizeof(float),n2*n3,glayerfp);
			efclose(glayerfp);
		}
		nz = nlayer - 1;
	}

/* compute mini layer depth and grid values */
	if(nlayer==0) {
		for(i2=0;i2<n2*n3;i2++) {
			zs[i2] = ztop[i2];
			vs[i2] = vtop[i2];
		}
		tmp = 0.;
		for(iz=0;iz<nz;iz++) tmp = tmp + dzrl[iz];
		vscale = 0.;
		zscale = 0.;
		for(iz=1;iz<nz;iz++) {
			zscale += dzrl[iz-1]/tmp;
			if(ginterp==0) {
				vscale += dzrl[iz-1]/tmp;
			} else {
				vscale = (float)iz / nz;
			}

			for(i2=0;i2<n2*n3;i2++) {
				zs[i2+iz*n2*n3] = ztop[i2] +
					zscale*(zbot[i2]-ztop[i2]);
				vs[i2+iz*n2*n3] = vtop[i2] + 
					vscale*(vbot[i2]-vtop[i2]);
			}
		}
		for(i2=0;i2<n2*n3;i2++) {
			zs[i2+nz*n2*n3] = zbot[i2];
			vs[i2+nz*n2*n3] = vbot[i2];
		}
	}

/* compute smoothing window for mini layers */
	tmp = 0.;
	for(iz=0;iz<nz;iz++) tmp = tmp + dzrl[iz];
	scale = 0.;
	sm2s[0] = sm2top;
	sm3s[0] = sm3top;
	for(iz=1;iz<nz+1;iz++) {
		scale += dzrl[iz-1]/tmp;
		sm2s[iz] = sm2top + scale*(sm2bot-sm2top);
		sm3s[iz] = sm3top + scale*(sm3bot-sm3top);
	} 

/* compute grid values at the mini layers if not specified */
	fseek2g(infp,0,0);
	if( ( (ivtop==0 || ivbot==0) && nlayer==0 ) || (nglayer==0 && nlayer>0) ) {
		for(i3=0;i3<n3;i3++) {
			for(i2=0;i2<n2;i2++) {
				efread(grid,sizeof(float),n1,infp);
				for(iz=0;iz<nz+1;iz++) {
					tmp = (zs[i2+i3*n2+iz*n2*n3] - o1)/d1 + 0.5; 
					i1 = tmp;
					if(i1<0) {
						vs[i2+i3*n2+iz*n2*n3] = grid[0];
					} else if(i1>=n1-1) {
						vs[i2+i3*n2+iz*n2*n3] = grid[n1-1];
					} else {
						vs[i2+i3*n2+iz*n2*n3] = grid[i1]+
								(tmp-i1)*(grid[i1+1]-grid[i1]);
					}
				}
			}
		}
	} else if(ivtop==-1 && ivbot==-1) {
		for(i3=0;i3<n3;i3++) {
			for(i2=0;i2<n2;i2++) {
				for(iz=0;iz<nz+1;iz++) {
					tmp = (zs[i2+i3*n2+iz*n2*n3] - ztop[i3*n2+i2]);
					vs[i2+i3*n2+iz*n2*n3] = g0 + r0 * tmp;
				}	
			}
		}
	}
/* smooth mini layer grids */
	for(iz=0;iz<nz+1;iz++) {
		tmp = sm2s[iz]/d2;
		ismx = tmp + 1.5;
		tmp = sm3s[iz]/d3;
		ismy = tmp + 1.5;
		fsmx = (float*) emalloc(ismx*sizeof(float));
		fsmy = (float*) emalloc(ismy*sizeof(float));
		/* 2d smoothing */
		smth2d_(vs+iz*n2*n3,work,fsmx,fsmy,&n2,&n3,&ismx,&ismy);
		/*
		dump2xplot(vs+iz*n2*n3,n2,n3,0,"vsmoth");
		*/
		free(fsmx);
		free(fsmy);
	}


/* output grid */
	fseek2g(infp,0,0);
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {

			efread(grid,sizeof(float),n1,infp);
			itmp = i2+i3*n2;
			if(nlayer==0) {
				top = ztop[itmp];
				bot = zbot[itmp];
			} else {
				top = zs[itmp];
				bot = zs[itmp+(nlayer-1)*n2*n3];
			}
			tmp = (top - o1)/d1 + 0.5; 
			i1top = tmp;
			tmp = (bot - o1)/d1 + 0.5; 
			i1bot = tmp;
			if(i1top<0) i1top = 0;
			if(i1bot>n1-1) i1bot = n1-1;
			for(i1=i1top;i1<=i1bot;i1++) {
				z = o1 + i1*d1;
				for(iz=0;iz<nz;iz++) {
					if(z>=zs[itmp+iz*n2*n3] && z<=zs[itmp+(iz+1)*n2*n3]) {
						tmp = (z-zs[itmp+iz*n2*n3]) /
					  		(zs[itmp+(iz+1)*n2*n3]-zs[itmp+iz*n2*n3]);
						grid[i1] = vs[itmp+iz*n2*n3] + 
							tmp*(vs[itmp+(iz+1)*n2*n3]-vs[itmp+iz*n2*n3]);
						break;
					} else if(z<zs[itmp]) {
						break;
					} else if(z>zs[itmp+nz*n2*n3]) {
						break;
					}
				}
			}

			if(igabovetop==1) {
				for(i1=0;i1<i1top;i1++) {
					grid[i1] = gabovetop;
				}
			}
			if(igbelowbot==1) {
				for(i1=i1bot;i1<n1;i1++) {
					grid[i1] = gbelowbot;
				}
			}

			if(i2==0 && i3==0) {
				gmin = grid[0];
				gmax = grid[0];
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
	free(vtop);
	free(vbot);
	free(zs);
	free(vs);
	free(work);
	free(grid);
	exit(0);
}
