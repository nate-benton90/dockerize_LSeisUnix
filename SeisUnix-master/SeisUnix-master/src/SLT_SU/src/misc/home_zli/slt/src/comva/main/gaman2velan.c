#include "gridhd.h"
#include "comva.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"GAMAN2VELAN - convert gamma semblance to velocity semblance \n"
"\n"
"gaman2velan <gaman.data >velan.data [optional parameters]\n"
"\n"
"Required Parameters:\n"
"vgfile= 		name of interval velocity grid file  \n"
"\n"
"Optional Parameters:\n"
"fzvgrid=from-vgfile	first depth in interval velocity grid file \n"
"dzvgrid=from-vgfile	depth interval in interval velocity grid file \n"
"nzvgrid=from-vgfile	number of depths per cdp in interval velocity \n"
"				grid file \n"
"fcdpvgrid=from-vgfile	first cdp number of interval velocity grid file \n"
"dcdpvgrid=from-vgfile	cdp number increment in interval velocity grid file \n"
"nv=41			number of velocity values to output  \n"
"dv=50.  		velocity sampling interval	 \n"
"fv=1500. 		first velocity value 		\n"
"\n"
"NOTE:							\n"
"\n"
"    1. gamma*10000 is stored as offset in the input trace header \n"
"    2. dzvgrid is usually NOT the same as the depth interval in input \n"
"          gamma spectrum		\n"	
"    3. Trace header fields accessed:  ns, dt, delrt, offset, cdp, fz, dz. \n"
"    4. Trace header fields modified:  offset (store velocity). \n"
"    5. If vgfile is standard grid file, i.e. with grid header, \n"
"       fzvgrid, dzvgrid, nzvgrid, fcdpvgrid, dcdpvgrid will 	\n"
"       default to those in the grid header. If, however, vgfile is 	\n"
"       nonstandard, these five parameters are required 	\n"
"    6. gamma is defined as Vnew/Vold				\n"
" \n"
" Author: Zhiming Li             10/3/91			\n"
"\n";
/**************** end self doc *******************************************/

void gam2vel(float cdpn, float *gaman, float *velan, int nz, int ng, int nv,  
	     float fz, float dz, float fg, float dg, float fv, float dv, 
	     FILE *vgfp, float fcdpvgrid, float dcdpvgrid, int ncdpvgrid, 
	     float fzvgrid, float dzvgrid, int nzvgrid);

segytrace trin, trout;
segybhdr bh;
segychdr ch;
ghed gh;

main(int argc, char **argv)
{
	int ng,nv;	/* number of gamma values, velocity values */
	float dg,dv;	/* gamma and velocity sampling interval */
	float fg,fv;	/* first gamma and first velocity*/
	int ig,iv;	/* gamma and velocity index */
	int nz;		/* number of depth samples per input trace */
	float dz;	/* depth sampling interval for input traces */
	float fz;	/* depth of first sample input and output */
	int iz;		/* input depth sample index */
	float *g,*v;	/* gamma and velocity*/
	float temp;	/* temporary scalar */
	float *velan;	/* array[nz,nv] */
	float *gaman;	/* array[nz,ng] */
	float fcdpvgrid;/* first cdp number in interval velocity grid file */
	FILE *vgfp, *infp;
	string vgfile;
	float fzvgrid, dzvgrid;
	int nzvgrid, ncdpvgrid;
	float dcdpvgrid;
	int cdpprev, cdp, tracl;

	int n1,n2,n3,n4,n5;
	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
	float scale, ocdp2, oline3, dcdp2, dline3, gmin, gmax;
	int dtype,ierr,ighcdp;
	float xm,xcdp;
	int orient,gtype;
	

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get required parameters */
	if (!getparstring("vgfile",&vgfile))
        err("name of velocity grid file (vgfile) must be given \n");

	/* open velocity grid file and obtain header info */ 
	vgfp = fopen(vgfile,"r");
	ierr = fgetghdr(vgfp,&gh);
	if(ierr==0) fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
				&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
				&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
				&orient,&gtype);

	/* get parameters from the id headers */
	gethdr(&ch,&bh);

	/* get parameters from the first trace */
	if (!gettr(&trin)) err("can't get first trace");
	nz = trin.ns;
	if ( trin.dt < 1000 ) {
		dz = trin.dt;
		fz = trin.delrt;
	}
	else {
		dz = (float)trin.dt/1000;
		fz = trin.delrt/1000;
	}

	if ( trin.dz!=0.) {
		dz = trin.dz;
		fz = trin.fz;
	}

	/* get optional parameters */
	if (!getparint("nv",&nv)) nv = 41;
	if (!getparfloat("dv",&dv)) dv = 50.;
	if (!getparfloat("fv",&fv)) fv = 1500.;

	if (!getparint("nzvgrid",&nzvgrid)) {
		if(ierr==0) {
			nzvgrid = n1;
		} else {
			err("nzvgrid must be specified for nonstandard vgfile");
		}
	}
	if (!getparfloat("dzvgrid",&dzvgrid)) {
		if(ierr==0) {
			dzvgrid = d1;
		} else {
			err("dzvgrid must be specified for nonstandard vgfile");
		}
	}
	if (!getparfloat("fzvgrid",&fzvgrid)) {
		if(ierr==0) {
			fzvgrid = o1;
		} else {
			err("fzvgrid must be specified for nonstandard vgfile");
		}
	}

	if (!getparfloat("fcdpvgrid",&fcdpvgrid)) {
		if(ierr==0) {
			fcdpvgrid = ocdp2;
		} else {
		      err("fcdpvgrid must be specified for nonstandard vgfile");
		}
	}
	if (!getparfloat("dcdpvgrid",&dcdpvgrid)) {
		if(ierr==0) {
			dcdpvgrid = dcdp2;
		} else {
		      err("fcdpvgrid must be specified for nonstandard vgfile");
		}
	}
	/* error checking */
	if(ierr==0) {
		if(nzvgrid!=n1) err("nzvgrid not consistant with grid header");
		if(fzvgrid!=o1) err("fzvgrid not consistant with grid header");
		if(dzvgrid!=d1) err("dzvgrid not consistant with grid header");
		if(fcdpvgrid!=ocdp2) 
			warn("fcdpvgrid not consistant with grid header");
		if(dcdpvgrid!=dcdp2) 
			warn("dcdpvgrid not consistant with grid header");
	}
	ighcdp = 1;
	if(dcdpvgrid==0. && ierr==0) {
		/* switch to x coordinates */
		ighcdp = 0;
		fcdpvgrid = o2;
		dcdpvgrid = d2;
	}


	/* open vgfile and estimate number of cdp's in vgfile */
    	fseek(vgfp,0L,2);
	if(ierr==0) {
		ncdpvgrid = n2;
	} else {
		ncdpvgrid = ftell(vgfp)/sizeof(float)/nzvgrid;
	}
    	fseek(vgfp,0L,0);

	/* update id headers and output them */
	ng = bh.fold;
	bh.fold = nv;
	bh.tsort = 2;
	puthdr(&ch,&bh);

	/* allocate memory */
	velan = (float *) malloc(nz*nv*sizeof(float));
	gaman = (float *) malloc(nz*ng*sizeof(float));
	g = (float *) malloc(ng*sizeof(float));
	v = (float *) malloc(nv*sizeof(float));

	/* remember all output velocities */
	for(iv=0;iv<nv;iv++) v[iv] = fv + iv * dv;

	/* remember previous cdp */
	cdpprev = trin.cdp;
	tracl = 1;
	ig = 0;
	xm = xmcor(trin.sx,trin.gx,trin.scalco);

	/* save trace header */
	memcpy((char*)&trout,(char*)&trin,240);


	/* loop over input traces */
	do {
		/* determine offset and cdp */
		cdp = trin.cdp;

		if(cdp==cdpprev) {
			/* get gamma spectrum */
			for(iz=0;iz<nz;iz++) gaman[iz+ig*nz]=trin.data[iz];
			g[ig] = trin.offset/10000.;		      	
			ig = ig + 1;
		/* cdp changed */
		} else {
			if(ig!=ng) 
			    err("number of traces per cdp error at cdp=%d !\n",
				cdpprev);

			/* conversion */
			fg = g[0];		      	
			if(ng>0) dg = g[1]-g[0];

			if(ighcdp==0) {
				xcdp = xm;
			} else {
				xcdp = cdpprev;
			}
			gam2vel(xcdp, gaman, velan, nz, ng, nv,  
	     			fz, dz, fg, dg, fv, dv, 
	     			vgfp, fcdpvgrid, dcdpvgrid, ncdpvgrid, 
	     			fzvgrid, dzvgrid, nzvgrid); 

			/* output */

			for(iv=0;iv<nv;iv++) {
				for(iz=0;iz<nz;iz++) 
					trout.data[iz]=velan[iz+iv*nz];
				trout.offset = v[iv];
				trout.tracl = tracl;
				trout.cdpt = iv+1;

				puttr(&trout);
				tracl = tracl + 1;
			}

			fprintf(stderr,
				"gaman to velan conversion done at cdp=%d \n",
				cdpprev);

			/* save trace header */
			memcpy((char*)&trout,(char*)&trin,240);
			ig = 0;
			/* save gamma spectrum */
			for(iz=0;iz<nz;iz++) gaman[iz+ig*nz]=trin.data[iz];
			g[ig] = trin.offset/10000.;		      	

			ig = ig + 1;
			cdpprev = cdp;
			xm = xmcor(trin.sx,trin.gx,trin.scalco);
		}
	} while (gettr(&trin));

	/* last cdp */
	if(ig!=ng) err("number of traces per cdp error at cdp=%d !\n",cdpprev);


	/* conversion */
	fg = g[0];		      	
	if(ng>0) dg = g[1]-g[0];

	if(ighcdp==0) {
		xcdp = xm;
	} else {
		xcdp = cdpprev;
	}
	gam2vel(xcdp, gaman, velan, nz, ng, nv,  
		fz, dz, fg, dg, fv, dv, 
	     	vgfp, fcdpvgrid, dcdpvgrid, ncdpvgrid, 
	     	fzvgrid, dzvgrid, nzvgrid); 

	/* output */


	for(iv=0;iv<nv;iv++) {
		for(iz=0;iz<nz;iz++) 
			trout.data[iz]=velan[iz+iv*nz];
		trout.offset = v[iv];
		trout.tracl = tracl;
		trout.cdpt = iv+1;

		puttr(&trout);
		tracl = tracl + 1;
	}

	fprintf(stderr,
		"gaman to velan conversion done at cdp=%d \n",
		cdpprev);


	free(v);
	free(g);
	free(gaman);
	free(velan);

	
	return EXIT_SUCCESS;
}

/* gamma spectrum to velocity spectrum mapping */
void gam2vel(float cdpn, float *gaman, float *velan, int nz, int ng, int nv,  
	     float fz, float dz, float fg, float dg, float fv, float dv, 
	     FILE *vgfp, float fcdpvgrid, float dcdpvgrid, int ncdpvgrid, 
	     float fzvgrid, float dzvgrid, int nzvgrid) {

	float temp, z, vi;
	int indx, iz, izz, iv;
	float *v, *v_resample, *odvg, *fvg;

	/* allocate some arrays */
	v = (float *) malloc(nzvgrid*sizeof(float));
	v_resample = (float *) malloc(nz*sizeof(float));
	fvg = (float *) malloc(nz*sizeof(float));
	odvg = (float *) malloc(nz*sizeof(float));

	/* read in interval velocity from vgfile */
	temp = (cdpn-fcdpvgrid)/dcdpvgrid;
	indx = temp;
	if ( indx < 0 ) indx = 0;
	if ( indx >= ncdpvgrid ) indx = ncdpvgrid - 1;
	fseek(vgfp,indx*nzvgrid*sizeof(float),0);
	fread(v,sizeof(float),nzvgrid,vgfp);

	/* compute average velocity */
	temp = v[0]*fzvgrid;
	for(iz=1;iz<nzvgrid;iz++) {
		temp = temp + v[iz]*dzvgrid;
		v[iz] = temp;
	}
	for(iz=1;iz<nzvgrid;iz++) v[iz] = v[iz]/(fzvgrid+iz*dzvgrid);

	/* compute resampled average velocity */
	for(iz=0;iz<nz;iz++) {
		z = (fz+iz*dz-fzvgrid)/dzvgrid;
		izz=z;
		if(izz<0) {
			v_resample[iz] = v[0];
	     	} else if (izz>=nzvgrid-1) {
			v_resample[iz] = v[nzvgrid-1];
	     	} else {
			v_resample[iz] = v[izz]+(z-izz)*(v[izz+1]-v[izz]); 
	     	}
		fvg[iz] = fg * v_resample[iz];
		odvg[iz] = 1/(v_resample[iz] * dg);
	}
		
	/* gaman to velan mapping */
	for(iv=0;iv<nv;iv++) {
		vi = fv + iv*dv;
		for(iz=0;iz<nz;iz++) {
			temp=(vi-fvg[iz])*odvg[iz];
			indx = temp;
			if(indx>=0 && indx<ng-1) {
				velan[iv*nz+iz] = gaman[indx*nz+iz] +
				(temp-indx)*(gaman[(indx+1)*nz+iz]-
					     gaman[indx*nz+iz]);
			} else {
			       	velan[iv*nz+iz]=0.;
		   	}
		}
	}
	free(v);
	free(v_resample);
	free(fvg);
	free(odvg);
}
