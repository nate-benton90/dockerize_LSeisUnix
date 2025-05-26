#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"VRSCAN - Velocity Ratio (Vscan/Vmig) Scan of depth migrated cdp gathers\n"
"\n"
"vrscan <stdin >stdout [optional parameters]\n"
"\n"
"Optional Parameters:\n"
"nvr=11                  number of velocity scans  \n"
"dvr=0.02                velocity ratio sampling interval	 \n"
"fvr=.90                 first velocity ratio value to scan 		\n"
"smute=1.5               samples with stretch exceeding smute are zeroed \n"
"zpow=0.0                input trace will be gained by factor of depth**zpow\n"
"			 before scan 				\n"
"\n"
"Note:	\n"
"1. Trace header fields accessed:  ns, offset, cdp, fz, dz. \n"
"2. Trace header fields modified:  offset  \n"
"   after scaning offset will be sent to vr*10000,  		\n" 
"   i.e., if offset=9500, it means the trace uses velocity ratio of .95	\n"
" \n"
" Author: Zhiming Li             10/19/93			\n"
"\n";
/**************** end self doc *******************************************/



void gmo(float *data, float *dgmo, float fz, float dz, int nz, float og,
	float offset, float *fold, float smute, float mute);


segy tri, tro;
SU_bhed bh;
SU_ched ch;

main(int argc, char **argv)
{
	int ng;		/* number of gamma values */
	float dg;	/* gamma sampling interval */
	float fg;	/* first gamma */
	int ig;		/* gamma index */
	int nz;		/* number of depth samples per input trace */
	float dz;	/* depth sampling interval for input traces */
	float fz;	/* depth of first sample input and output */
	int iz;		/* input depth sample index */
	int verbose;	/* =1 for diagnostic print */
	long cdp;	/* cdp from current input trace header */
	long cdpprev;	/* cdp from previous input trace header */
	float smute;	/* GMO stretch mute factor */
	float offset;	/* offset from input trace header */
	float g;	/* gamma */
	float *data;	/* array[nz] of input trace */
	int tracl;	/* trace number of the line */
	float *gain, zpow;	/* depth gain */
	float *fold, *gfold, *gout, *dgmo;
	float temp;
	float mute, mutesave;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(1);

	/* read id headers */
	gethdr(&ch,&bh);
	/* get parameters from the first trace */
	if (!gettr(&tri)) err("can't get first trace");
	nz = tri.ns;

	if ( tri.dz == 0. ) err(" input trace header dz is zero");
	if ( tri.fz != 0. ) 
		warn(" input trace header fz is not zero fz=%g \n",tri.fz);

	dz = tri.dz;
	fz = tri.fz;

	cdp = tri.cdp;
	offset = tri.offset;

	/* get optional parameters */
	if (!getparint("nvr",&ng)) ng = 11;
	if (!getparfloat("dvr",&dg)) dg = 0.02;
	if (!getparfloat("fvr",&fg)) fg = .90;
	if (!getparfloat("smute",&smute)) smute = 1.5;
	if (smute<=1.0) err("smute must be greater than 1.0");
	if (!getparint("verbose",&verbose)) verbose = 1;
	if (!getparfloat("zpow",&zpow)) zpow = 0.;


	/* update id headers and output them */
	bh.fold = ng;
	bh.tsort = 2;
	puthdr(&ch,&bh);

	if (verbose) {
		fprintf(stderr,
			"\t === VRSCAN PROGRAM PRINTOUT === \n");
		fprintf(stderr,"   first vr to scan = %g \n", fg);
		fprintf(stderr,"   number of vr to scan = %d \n", ng);
		fprintf(stderr,"   vr interval to scan = %g \n", dg);
	}

	/* memory allocations */
	data = (float *) malloc(nz*sizeof(float));
	fold = (float *) malloc(nz*sizeof(float));
	dgmo = (float *) malloc(nz*sizeof(float));
	gout = (float *) malloc(ng*nz*sizeof(float));
	gfold = (float *) malloc(ng*nz*sizeof(float));

	/* compute zpow factors */
	if(zpow!=0.0) {
		gain = ealloc1float(nz);
		for(iz=0;iz<nz;iz++) {
			 temp = (fz + iz * dz)/(fz+nz*.5*dz);
			 if (temp > 0. ) {  
			 	gain[iz]=pow(temp,zpow);
			 } else {
			 	gain[iz]=0.;
				
			 }
		}
	}

	
	/* remember previous cdp */
	cdpprev = cdp ;
	tracl = 1;
	mutesave = tri.mute;
	
	bzero(fold,nz*sizeof(float));
	bzero(gfold,ng*nz*sizeof(float));
	bzero(gout,ng*nz*sizeof(float));
	bzero(dgmo,nz*sizeof(float));
	bzero(data,nz*sizeof(float));

	/* loop over input traces */
	do {

		/* determine offset and cdp */
		offset = tri.offset;
		cdp = tri.cdp;
		mute = tri.mute; 

		/* same cdp */
		if(cdp==cdpprev) {
			bcopy(&tri,&tro,240);
			bcopy(tri.data,data,nz*sizeof(float));
			if(zpow!=0.0) 
				for(iz=0;iz<nz;iz++) data[iz] *= gain[iz];
			for(ig=0;ig<ng;ig++) {
				g = fg + ig * dg;
 				gmo(data,dgmo,fz,dz,nz,g,offset,fold,
					smute,mute);
				for(iz=0;iz<nz;iz++) {
					gout[iz+ig*nz] += dgmo[iz];
					gfold[iz+ig*nz] += fold[iz];
				}
			}
			if(mute<mutesave) mutesave = mute;
		/* change of cdp */
		} else {
			/* set output trace header fields */
			tro.n2 = ng;

			/* normalize */
			for (ig=0; ig<ng; ++ig) {
				for(iz=0;iz<nz;iz++) {
					if(gfold[iz+ig*nz]>1.) {
					  	gout[iz+ig*nz] = 
						gout[iz+ig*nz]/gfold[iz+ig*nz];
					} 
				}
			}

			for (ig=0; ig<ng; ++ig) {
				bcopy(gout+ig*nz,tro.data,nz*sizeof(float));
				/* update offset */
				g = fg + ig * dg;
				g = g * 10000. + 0.5;
				tro.offset = (long) g;  
				tro.tracl = tracl;  
				tro.mute = mutesave; 
				tro.cdpt = ig + 1;  
				puttr(&tro);
				tracl = tracl + 1;
			}

			/* zero arrays */
			bzero(gout,nz*ng*sizeof(float));
			bzero(gfold,nz*ng*sizeof(float));

			cdpprev = cdp;
			mutesave = mute;

			bcopy(&tri,&tro,240);
			bcopy(tri.data,data,nz*sizeof(float));
			if(zpow!=0.0) 
				for(iz=0;iz<nz;iz++) data[iz] *= gain[iz];
			for(ig=0;ig<ng;ig++) {
				g = fg + ig * dg;
 				gmo(data,dgmo,fz,dz,nz,g,offset,fold,
					smute,mute);
				for(iz=0;iz<nz;iz++) {
					gout[iz+ig*nz] += dgmo[iz];
					gfold[iz+ig*nz] += fold[iz];
				}
			}
		}

	} while (gettr(&tri));

	/* last cdp */
	/* set output trace header fields */
	tro.n2 = ng;
	/* normalize */
	for (ig=0; ig<ng; ig++) {
		for(iz=0;iz<nz;iz++) {
			if(gfold[iz+ig*nz]>1.) {
			  	gout[iz+ig*nz] = 
				gout[iz+ig*nz]/gfold[iz+ig*nz];
			} 
		}
	}

	for (ig=0; ig<ng; ig++) {
		bcopy(gout+ig*nz,tro.data,nz*sizeof(float));
		/* update offset */
		g = fg + ig * dg;
		g = g * 10000. + 0.5;
		tro.offset = (long) g;  
		tro.tracl = tracl;  
		tro.mute = mutesave;  
		tro.cdpt = ig + 1;  
		puttr(&tro);
		tracl = tracl + 1;
	}

	free(gout);
	free(gfold);
	free(dgmo);
	free(data);
	free(fold);

	return EXIT_SUCCESS;
}


/* gamma moveout */

void gmo(float *data, float *dgmo, float fz, float dz, int nz, float og,
	float offset, float *fold, float smute, float mute) {


	float offovs, znmute, temp, frac, zi; 
	int izmute, iz, izi, imute; 
	float zn;

	float g, g2; 

	g = 1./og;
	g2 = g * g;
	

	/* compute (offset/2)^2 * (gamma^2-1) */
	offovs = (offset*offset*0.25)*(g*g-1.);

	/* determine mute depth after gmo */
	if ( g >= 1.0 ) {
		znmute=sqrt(offovs/(smute*smute-1.0));
	} else {
		znmute=sqrt(offovs/(1./(smute*smute)-1.0));
	}
	izmute = (znmute-fz)/dz;

			
	bzero(fold,nz*sizeof(float));
	bzero(dgmo,nz*sizeof(float));

	temp = (mute-fz)/dz;
	imute = temp;

	if(izmute<0) izmute=0;
	if(imute<0) imute=0;

	/* do gmo via quick and dirty linear interpolation */
	for (iz=izmute,zn=fz+izmute*dz; iz<nz; ++iz,zn+=dz) {

		temp = zn*zn + offovs;
        	if(temp >=0.) {
			zi = (sqrt(temp)-fz)/dz;
		} else {
			zi = -100000;
		}
		izi = zi;
		if (izi<nz-1 && izi >= imute ) {
			frac = zi-izi;
			dgmo[iz] = (1.0-frac)*data[izi]+ frac*data[izi+1];
			fold[iz] = 1.;
		}
	}
}
