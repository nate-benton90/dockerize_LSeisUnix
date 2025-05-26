#include "usgrid.h"
#include "grid.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"RMOSCAN - Residual Moveout Scan (Vscan/Vmig) of Migrated/Nmo cdp Gathers\n"
"\n"
"vrscan <stdin velfile= cdp1= dcdpx= dcdpy= >stdout [optional parameters]\n"
"\n"
"velfile=                RMS velocity file name (velocity is stored as	\n" 
"                        v(t,inline,crossline)				\n"
"cdp1=                   cdp number at first trace of velfile		\n"
"dcdpx=                  cdp number increment of trace along the inline \n" 
"                        direction of velfile				\n" 
"dcdpy=                  cdp number increment of trace along the cross	\n" 
"                         line direction of velfile			\n"
"\n"
"Optional Parameters:\n"
"nvr=11                  number of velocity ratio scans  \n"
"dvr=0.02                velocity ratio sampling interval	 \n"
"fvr=.90                 first velocity ratio value to scan 		\n"
"smute=1.5               samples with stretch exceeding smute are zeroed \n"
"tpow=0.0                input trace will be gained by factor of time**tpow\n"
"                        before scan 				\n"
"\n"
"Note:	\n"
"1. Trace header fields accessed:  ns, offset, cdp, fz, dz. \n"
"2. Trace header fields modified:  offset  \n"
"   after scaning offset will be sent to vr*10000,  		\n" 
"   i.e., if offset=9500, it means the trace uses velocity ratio of .95	\n"
"3. dcdpy is typically larger than the number of traces per line in	\n"
"   velfile, since velfile often defines only a subregion of the 3d 	\n"
"   master grid. Therefore, dcdpy should be the number of cdp per line	\n"
"   in the master 3d grid, to match the cdp number in the input seismic \n"
"   trace.								\n"
" \n"
" Author: Zhiming Li             11/8/93			\n"
"\n";
/**************** end self doc *******************************************/



void gmo(float *data, float *dgmo, float ft, float dt, int nt, float og,
	float offset, float *fold, float smute, float mute, float *ovvt);

void readovv(FILE *vgfp, int ntvgrid, int ncdpvgrid,
        int cdp1, float dcdpx, float dcdpy,
        int nx, int ny,
        float dtvgrid, float ftvgrid,
        int cdp, int nt, float dt, float ft, float *ovvt);

segy tri, tro;
SU_bhed bh;
SU_ched ch;

main(int argc, char **argv)
{
	int ng;		/* number of gamma values */
	float dg;	/* gamma sampling interval */
	float fg;	/* first gamma */
	int ig;		/* gamma index */
	int nt;		/* number of time samples per input trace */
	float dt;	/* time sampling interval for input traces */
	float ft;	/* time of first sample input and output */
	int it;		/* input time sample index */
	int verbose;	/* =1 for diagnostic print */
	long cdp;	/* cdp from current input trace header */
	long cdpprev;	/* cdp from previous input trace header */
	float smute;	/* RMO stretch mute factor */
	float offset;	/* offset from input trace header */
	float g;	/* gamma */
	float *data;	/* array[nt] of input trace */
	int tracl;	/* trace number of the line */
	float *gain, tpow;
	float *fold, *gfold, *gout, *dgmo, *ovvt;
	float temp;
	float mute, mutesave;
	char *velfile;
	FILE *vfp;
	int cdp1;
        float dcdpx, dcdpy;
	
	int n1,n2,n3,n4,n5;
        float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
        float scale, ocdp2, oline3, dcdp2, dline3;
        float vmin, vmax;
        int dtype,ierr,orient,gtype;
        ghed gh;



	/* hook up getpar */
	initargs(argc,argv);
	askdoc(1);

	/* check velocity file */ 
        if (!getparstring("velfile",&velfile))
                err(" File velfile must be specified ");
        if((vfp = fopen(velfile,"r"))==NULL)
                err("Input rms velocity grid file %s not found \n",velfile);
        /* obtain grid header info */
        ierr = fgetghdr(vfp, &gh);
        if(ierr==0) fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
                                &d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                                &dcdp2,&dline3,&ocdp2,&oline3,
                                &vmin,&vmax,&orient,&gtype);
        if (!getparint("cdp1",&cdp1))
                err(" cdp1 must be specified ");
        if (!getparfloat("dcdpx",&dcdpx))
                err(" dcdpx must be specified ");
        if (!getparfloat("dcdpy",&dcdpy))
                err(" dcdpy must be specified ");

	/* read id headers */
	gethdr(&ch,&bh);
	/* get parameters from the first trace */
	if (!gettr(&tri)) err("can't get first trace");
	nt = tri.ns;
	dt = (float)tri.dt*1.0e-6;
	ft = tri.delrt * 0.001;

	cdp = tri.cdp;
	offset = tri.offset;

	/* get optional parameters */
	if (!getparint("nvr",&ng)) ng = 11;
	if (!getparfloat("dvr",&dg)) dg = 0.02;
	if (!getparfloat("fvr",&fg)) fg = .90;
	if (!getparfloat("smute",&smute)) smute = 1.5;
	if (smute<=0.) err("smute must be greater than 0.");
	if (!getparint("verbose",&verbose)) verbose = 1;
	if (!getparfloat("tpow",&tpow)) tpow = 0.;


	/* update id headers and output them */
	bh.fold = ng;
	bh.tsort = 2;
	puthdr(&ch,&bh);

	if (verbose) {
		fprintf(stderr,
			"\t === RMOSCAN PROGRAM PRINTOUT === \n");
		fprintf(stderr,"   first vr to scan = %g \n", fg);
		fprintf(stderr,"   number of vr to scan = %d \n", ng);
		fprintf(stderr,"   vr interval to scan = %g \n", dg);
	}

	/* memory allocations */
	data = (float *) malloc(nt*sizeof(float));
	fold = (float *) malloc(nt*sizeof(float));
	dgmo = (float *) malloc(nt*sizeof(float));
	gout = (float *) malloc(ng*nt*sizeof(float));
	gfold = (float *) malloc(ng*nt*sizeof(float));
	ovvt = (float *) malloc(nt*sizeof(float));


	/* compute tpow factors */
	if(tpow!=0.0) {
		gain = ealloc1float(nt);
		for(it=0;it<nt;it++) {
			 temp = (ft + it * dt)/(ft+nt*.5*dt);
			 if (temp > 0. ) {  
			 	gain[it]=pow(temp,tpow);
			 } else {
			 	gain[it]=0.;
				
			 }
		}
	}

	
	/* remember previous cdp */
	cdpprev = cdp ;
	tracl = 1;
	mutesave = tri.mute;
	
	bzero(fold,nt*sizeof(float));
	bzero(gfold,ng*nt*sizeof(float));
	bzero(gout,ng*nt*sizeof(float));
	bzero(dgmo,nt*sizeof(float));
	bzero(data,nt*sizeof(float));
	bzero(ovvt,nt*sizeof(float));

	o1 = o1 * 0.001;
	d1 = d1 * 0.001;

	/* read in velocity function for the first cdp */
	readovv(vfp,n1,n2*n3,cdp1,dcdpx,dcdpy,n2,n3,d1,o1,cdp,nt,dt,ft,ovvt);


	/* loop over input traces */
	do {

		/* determine offset and cdp */
		offset = tri.offset;
		cdp = tri.cdp;
		mute = tri.mute; 

		/* same cdp */
		if(cdp==cdpprev) {
			bcopy(&tri,&tro,240);
			bcopy(tri.data,data,nt*sizeof(float));
			if(tpow!=0.0) 
				for(it=0;it<nt;it++) data[it] *= gain[it];
			for(ig=0;ig<ng;ig++) {
				g = fg + ig * dg;
 				gmo(data,dgmo,ft,dt,nt,g,offset,fold,
					smute,mute*0.001,ovvt);
				for(it=0;it<nt;it++) {
					gout[it+ig*nt] += dgmo[it];
					gfold[it+ig*nt] += fold[it];
				}
			}
			if(mute<mutesave) mutesave = mute;
		/* change of cdp */
		} else {
			/* set output trace header fields */
			tro.n2 = ng;


			/* normalize */
			for (ig=0; ig<ng; ++ig) {
				for(it=0;it<nt;it++) {
					if(gfold[it+ig*nt]>1.) {
					  	gout[it+ig*nt] = 
						gout[it+ig*nt]/gfold[it+ig*nt];
					} 
				}
			}

			for (ig=0; ig<ng; ++ig) {
				bcopy(gout+ig*nt,tro.data,nt*sizeof(float));
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
			bzero(gout,nt*ng*sizeof(float));
			bzero(gfold,nt*ng*sizeof(float));

			cdpprev = cdp;
			mutesave = mute;

			bcopy(&tri,&tro,240);
			bcopy(tri.data,data,nt*sizeof(float));
			if(tpow!=0.0) 
				for(it=0;it<nt;it++) data[it] *= gain[it];
			readovv(vfp,n1,n2*n3,cdp1,dcdpx,dcdpy,n2,n3,d1,o1,
				cdp,nt,dt,ft,ovvt);
			for(ig=0;ig<ng;ig++) {
				g = fg + ig * dg;
 				gmo(data,dgmo,ft,dt,nt,g,offset,fold,
					smute,mute*0.001,ovvt);
				for(it=0;it<nt;it++) {
					gout[it+ig*nt] += dgmo[it];
					gfold[it+ig*nt] += fold[it];
				}
			}
		}

	} while (gettr(&tri));

	/* last cdp */
	/* set output trace header fields */
	tro.n2 = ng;
	/* normalize */
	for (ig=0; ig<ng; ig++) {
		for(it=0;it<nt;it++) {
			if(gfold[it+ig*nt]>1.) {
			  	gout[it+ig*nt] = 
				gout[it+ig*nt]/gfold[it+ig*nt];
			} 
		}
	}

	for (ig=0; ig<ng; ig++) {
		bcopy(gout+ig*nt,tro.data,nt*sizeof(float));
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
	free(ovvt);

	return EXIT_SUCCESS;
}


/* gamma moveout for time data */

void gmo(float *data, float *dgmo, float ft, float dt, int nt, float og,
	float offset, float *fold, float smute, float mute, float *ovvt) {


	float offovs, tnmute, temp, frac, ti; 
	int itmute, it, iti, imute; 
	float tn;
	float g; 

	g = 1./og;
	
	/* compute (offset)^2 * (gamma^2-1) */
	offovs = offset*offset*(g*g-1.);

	/* determine mute depth after gmo */
	if ( g >= 1.0 ) {
		tnmute=sqrt(offovs*ovvt[0]/(smute*smute-1.0));
	} else {
		tnmute=sqrt(offovs*ovvt[0]/(1./(smute*smute)-1.0));
	}
	itmute = (tnmute-ft)/dt;

			
	bzero(fold,nt*sizeof(float));
	bzero(dgmo,nt*sizeof(float));

	temp = (mute-ft)/dt;
	imute = temp;

	if(itmute<0) itmute=0;
	if(imute<0) imute=0;

	/* do gmo via quick and dirty linear interpolation */
	for (it=itmute,tn=ft+itmute*dt; it<nt; ++it,tn+=dt) {

		temp = tn*tn + offovs*ovvt[it];
        	if(temp >=0.) {
			ti = (sqrt(temp)-ft)/dt;
		} else {
			ti = -100000;
		}
		iti = ti;
		if (iti<nt-1 && iti >= imute ) {
			frac = ti-iti;
			dgmo[it] = (1.0-frac)*data[iti]+ frac*data[iti+1];
			fold[it] = 1.;
		}
	}
}


void readovv(FILE *vgfp, int ntvgrid, int ncdpvgrid,
        int cdp1, float dcdpx, float dcdpy,
        int nx, int ny,
        float dtvgrid, float ftvgrid,
        int cdp, int nt, float dt, float ft, float *ovvt) {

        float *vread, ratio, t, ftr;
        int icdp, it, itread;
        float tmp;

        int ix, iy;


        if(dcdpy>dcdpx) {
                tmp = (cdp - cdp1)/dcdpy;
                iy = tmp;
                tmp = ( (cdp - cdp1) - iy*dcdpy )/dcdpx;
                ix = tmp;
        } else {
                tmp = (cdp - cdp1)/dcdpx;
                ix = tmp;
                tmp = ( (cdp - cdp1) - ix*dcdpx )/dcdpy;
                iy = tmp;
        }
        if(iy<0) iy=0;
        if(iy>ny-1) iy=ny-1;
        if(ix<0) ix=0;
        if(ix>nx-1) ix=nx-1;
        vread = (float *) malloc(ntvgrid*sizeof(float));

        icdp = ix + iy*nx;

        efseek(vgfp,icdp*ntvgrid*sizeof(float),0);
        efread(vread,sizeof(float),ntvgrid,vgfp);
        ratio = dt/dtvgrid;
        ftr = (ft - ftvgrid)/dtvgrid;

        for(it=0;it<nt;it++) {
                t = it*ratio + ftr;
                itread = t;
                if(itread < 0) {
                        tmp = vread[0];
                } else if(itread >= ntvgrid-1) {
                        tmp = vread[ntvgrid-1];
                } else {
                        tmp = vread[itread] + (t-itread)*
                                (vread[itread+1]-vread[itread]);
                }
                ovvt[it] = 1./(tmp*tmp);
        }

        free(vread);
}

