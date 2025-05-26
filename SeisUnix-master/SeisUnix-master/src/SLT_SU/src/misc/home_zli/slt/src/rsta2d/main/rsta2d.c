char *sdoc =
"rsta2d - Ray shooting travel time and amplitude computation \n"
"\n"
"rsta2d hfile= tfile= [optional parameters]\n"
"\n"
"Required Parameters:\n"
"hfile=stdin            file containing Hfile (if not			\n"
"				specified, standard input is assumed ) 	\n"
"tfile=stdout           file containing times t(nzo,nxo,ns) (if not 	\n"
"				specified, standard output is assumed ) \n"
"fxo=                   first output x coordinate of travel/amplitude grids\n"
"fzo=                   first output z coordinate \n"
"dxo=                   output x interval \n"
"dzo=                   output z interval \n"
"nxo=                   number of output x samples \n"
"nzo=                   number of output z samples \n"
"\n"
"Optional Parameters:\n"
"ns=1                   number of source locations to compute travel time \n"
"os=fxo                 x coordinate of first source \n"
"ds=dxo                 x coordinate increment of sources \n"
"afile=null             file to contain amplitudes a(nzo,nxo,ns) \n"
"                       (if not given, no amplitudes are computed)	\n" 
"pfile=null             file to contain propagation angles p(nzo,nxo,ns) \n"
"                       (if not given, no propagation angles are computed)\n" 
"kmheader=par.kirmig    header file for travel time and amplitude for KIRMIG \n"
"angmin=-75             minimum propagation angle allowed in amplitude table\n"
"angmax=75              maximum propagation angle allowed in amplitude table\n"
"nang=151               number of rays to shoot				\n"
"amptype=0              type of amplitude computation			\n"
"                        0=simplified amplitude factor cos(angle)/sqrt(t) \n"
"                        1=Bleistein: WKBJ theory for common-offset mig\n"
"                        2=Vidale: amplitude by ray-tube theory)	\n"
"restart=n              job is restarted (=y yes; =n no)		\n"
"rayplot=0              plot ray path diagrams (0=no 1=x-window -1=hardcopy)\n"
"treplace=10            replacement time at zones outside the ray coverage \n"
"                       (it should be equal or greater than the maximum \n"
"                       reflection time in the input data, if timefile is \n"
"                       goint to be used in kirmig)			\n"
"maxfac=2.0             maximum factor used to determine whether a ray's \n"
"                       arrival time at a constant depth will be used to \n"
"                       compute the travel time grid at that depth.	\n"
"                       This is the maximum ratio allowed between |dt/dx| \n"
"                       of a ray and the average |dt/dx| of all the rays \n"
"                       at that depth.	(when maxfac=0., no checking)	\n" 
"fillsd=0               fill the shadow zones with extrapolated travel  \n"
"                       times (0=no 1=yes)				\n"
"zminfl=0               minimum depth to fill the shadow zones when fillsd=1\n" 
"zmaxfl=fzo+(nzo-1)*dzo maximum depth to fill the shadow zones when fillsd=1\n" 
"Notes:									\n"
"    1. The current version of rsta2d requires the horizons in input hfile \n"
"       to extend from the left of the model to the right of model.	\n" 
 
"         \n"
"author:                Zhiming Li,        , 8/1/91 \n"
"\n";

#define maxhs 128
#define maxpks 256

#include "cshoot.h"
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "xplot.h"
#include "psplot.h"
#include "par.h"

main(int argc, char **argv)
{
	int ns,nxo,nzo,amptype;
	int nang;
	float fxs,dxs,dxo,dzo,fxo,fzo,xsi,zsi;
	float dxo1,dzo1,fxo1,fzo1,xsi1,zsi1;
	float *xs;
	float *t,*a,*p,tmp,*work;
	int iamp;
	char *afile="", *hisfile, *tfile, *hfile;
	char *pfile="";
	char *restart; 
	FILE *hfilefp, *tfp, *afp, *hfp, *pfp;
	int is,ix,iz,i,j,one,nn;
	int jj, ii;
	float disx, disz;
	float angmin, angmax, dang;
	float amin, amax;
	int is0, isize, rayplot;
	float dx, zoutmax;
	float zmax, treplace;
	float xmin, xmax;
	int fillsd;
	float zminfl, zmaxfl, zz;


	float os0, smax0, maxfac; 
	int ns0;

	float *plotdata;
	int *nplots, ncurve, nc, k;
	char title[80];
	char *label1="depth", *label2="distance", *style="seismic";

	/* variables for grid headers */ 
	float scale;
	int dtype, n1, n2, n3, n4, n5;
	float d1,d2,d3,d4,d5,o1,o2,o3,o4,o5,dcdp2,dline3,ocdp2,oline3;
	float gmin,gmax;
	float tgmin,tgmax,agmin,agmax,pgmin,pgmax;
	int itg=0, iag=0, ipg=0;
	ghed gh;

	/* variables for hfile input */
        int *npicks,*hnums,ih,nhs,*difs,
                cdpxmini,cdpxmaxi,ihfile;
        float *xpicks,*zpicks,*veltops,*velbots,*velavgs,*dvdzs,
                *dens,*qval,*pois,
                xmini,xmaxi,zmini,zmaxi,dcdpi,vmini,vmaxi;
        char hnames[maxhs][80], dunitsi[80], zattribi[80], vtypei[80];

	/* variables for cshoot */
	float *xint, *zint, *vint;
	int maxspl, nint;
	double *a0, *a1, *a2, *a3;
	float *sign; 
	int *norder, flag, *npts;
	float *v, xstart, xend, *xray, *zray, *tray;
	float vs;

	
	/* hook up getpar to handle the parameters */
	initargs(argc,argv);
	askdoc(1);
	
	/* get required parameters */
	if (!getparint("nxo",&nxo)) err("nxo missing");
	if (!getparint("nzo",&nzo)) err("nzo missing");
	if (!getparfloat("fxo",&fxo)) err("fxo missing");
	if (!getparfloat("fzo",&fzo)) err("fzo missing");
	if (!getparfloat("dxo",&dxo)) err("dxo missing");
	if (!getparfloat("dzo",&dzo)) err("dzo missing");
	
	/* get optional parameters */
	if (!getparint("ns",&ns)) ns = 1;
	if (!getparfloat("os",&fxs)) fxs = fxo;
	if (!getparfloat("ds",&dxs)) dxs = dxo;
	if (!getparstring("restart",&restart)) restart = "n"; 
	if (!getparfloat("maxfac",&maxfac)) maxfac = 2.0;

	if( !getparstring("hfile",&hfile) ) {
		hfilefp = stdin;
	} else {
		hfilefp = efopen(hfile,"r"); 
	} 

	if (!getparfloat("os0",&os0)) os0 = fxs;
	if (!getparint("ns0",&ns0)) ns0 = ns;
	if (!getparint("rayplot",&rayplot)) rayplot = 0;
	if (!getparfloat("treplace",&treplace)) treplace = 10.;
	smax0 = os0 + (ns0-1)*dxs;
	if (!getparint("fillsd",&fillsd)) fillsd = 0;
	if (!getparfloat("zminfl",&zminfl)) zminfl = 0.;
	if (!getparfloat("zmaxfl",&zmaxfl)) zmaxfl = fzo+(nzo-1)*dzo;

	/* read in hfile */
	xpicks = (float *) malloc(maxhs*maxpks*sizeof(float));
        zpicks = (float *) malloc(maxhs*maxpks*sizeof(float));
        veltops = (float *) malloc(maxhs*maxpks*sizeof(float));
        velbots = (float *) malloc(maxhs*maxpks*sizeof(float));
        dens = (float *) malloc(maxhs*maxpks*sizeof(float));
        pois = (float *) malloc(maxhs*maxpks*sizeof(float));
        qval = (float *) malloc(maxhs*maxpks*sizeof(float));
        npicks = (int *) malloc(maxhs*sizeof(int));
        hnums = (int *) malloc(maxhs*sizeof(int));
        difs = (int *) malloc(maxhs*maxpks*sizeof(int));
        velavgs = (float *) malloc(maxhs*maxpks*sizeof(float));
        dvdzs = (float *) malloc(maxhs*maxpks*sizeof(float));
	for(ih=0;ih<maxhs;ih++) npicks[ih]=0;
	nhs = 0;

	ifileread(hfilefp,&nhs,xpicks,zpicks,veltops,velbots,difs,
                  dens,qval,pois,velavgs,dvdzs,
                  (char *)hnames,hnums,npicks,maxhs,maxpks,
                  &xmini,&xmaxi,&zmini,&zmaxi,
                  &cdpxmini,&cdpxmaxi,&vmini,&vmaxi,
                  vtypei,dunitsi,zattribi,&dcdpi,&ihfile);
        efclose(hfilefp);

	zoutmax = fzo + (nzo-1)*dzo;
	nint = nhs;
	zmax = zpicks[(nhs-1)*maxpks];
	for(i=0;i<npicks[nhs-1];i++) {
		if(zmax<zpicks[(nhs-1)*maxpks+i]) zmax=zpicks[(nhs-1)*maxpks+i];
		if(zpicks[(nhs-1)*maxpks+i]>zoutmax) {
		} else {
			nint = nhs + 1;
		} 
	}

	if(zmax <zoutmax) zmax = zoutmax;

	if(nint>maxhs) err("Too many horizons in input hfile nhs=%d ",nhs);
	if(nhs == nint-1) {
		nhs = nint;
		npicks[nint-1] = npicks[nint-2];
		for(i=0;i<npicks[nint-1];i++) {
			xpicks[(nint-1)*maxpks+i] = xpicks[(nint-2)*maxpks+i];
			zpicks[(nint-1)*maxpks+i] = zmax;
			veltops[(nint-1)*maxpks+i] = velbots[(nint-2)*maxpks+i];
			velbots[(nint-1)*maxpks+i] = velbots[(nint-2)*maxpks+i];
		}
	}

	maxspl = 0;
	xstart = xmini;
	xend = xmaxi;
	nint = nhs;

	for(i=0;i<nhs;i++) {
		if(npicks[i]>maxspl) maxspl=npicks[i];
	}
	maxspl = maxspl + 1;
	xint = (float*) malloc(maxspl*nint*sizeof(float));
	zint = (float*) malloc(maxspl*nint*sizeof(float));
	vint = (float*) malloc(maxspl*nint*sizeof(float));
	for(i=0;i<nint;i++) {
		for(j=0;j<npicks[i];j++) {
			xint[i+j*nint] = xpicks[i*maxpks+j];
			zint[i+j*nint] = zpicks[i*maxpks+j];
			vint[i+j*nint] = 0.5*(veltops[i*maxpks+j]
					+ velbots[i*maxpks+j]);	
		}
	}
	if(rayplot==0) free(zpicks);
        free(veltops);
        free(velbots);
        free(dens);
        free(pois);
        free(qval);
        free(hnums);
        free(difs);
        free(velavgs);
        free(dvdzs);

	is0 = 0;
	isize = 0;
	if( !getparstring("tfile",&tfile) ) {
		tfp = stdout;
	} else {
		if((tfp = fopen(tfile,"r"))!=NULL) {
			fclose(tfp);
			tfp = fopen(tfile,"r+");	
		} else {
	   		tfp = fopen(tfile,"w");
		}
		if(restart[0]=='y') { 
			fseek(tfp,0,SEEK_END);
			isize = ftell(tfp)/sizeof(float)/nxo/nzo;
			is0 = (isize>0)?isize-1:0;
			/* if(isize==ns) return 0; */
		} else {
			fclose(tfp);
			tfp = fopen(tfile,"w");	
		}
		fseek(tfp,is0*nxo*nzo*sizeof(float),SEEK_SET);
	}

	getparstring("afile",&afile);
	getparstring("pfile",&pfile);
	if( !getparstring("kmheader",&hisfile) ) hisfile = "par.kirmig";

	if (!getparfloat("angmin",&angmin)) angmin = -75.;
	if (!getparfloat("angmax",&angmax)) angmax = 75.;
	if (!getparint("nang",&nang)) nang = 151;
	if(nang>1) dang = (angmax-angmin)/(nang-1);

	if (!getparint("amptype",&amptype)) amptype=0; 

/* compute locations of sources */
	xs = (float*)malloc(ns*sizeof(float));

	for (is=0;is<ns;is++) {
	   xs[is] = fxs + is*dxs;
	}

	hfp = fopen(hisfile,"w");
		

/* amplitude computation flag */	
	iamp = 0;
	if (afile[0]!='\0') {
		if((afp = fopen(afile,"r"))!=NULL) {
			fclose(afp);
			afp = fopen(afile,"r+");	
		} else {
	   		afp = fopen(afile,"w");
		}
	   	iamp = 1;
		if(restart[0]=='n') { 
			fclose(afp);
			afp = fopen(afile,"w");	
		}
	   	fseek(afp,is0*nxo*nzo*sizeof(float),SEEK_SET);
	}
	if (pfile[0]!='\0') {
		if((pfp = fopen(pfile,"r"))!=NULL) {
			fclose(pfp);
			pfp = fopen(pfile,"r+");	
		} else {
	   		pfp = fopen(pfile,"w");
		}
	   	iamp = 1;
		if(restart[0]=='n') { 
			fclose(pfp);
			pfp = fopen(pfile,"w");	
		}
	   	fseek(pfp,is0*nxo*nzo*sizeof(float),SEEK_SET);
		if(amptype==0) amptype = 3;
	}


	/* allocate space */
	v = (float*)malloc((nint+1)*sizeof(float));
	xray = (float*)malloc(nint*nang*sizeof(float));
	zray = (float*)malloc(nint*nang*sizeof(float));
	tray = (float*)malloc(nint*nang*sizeof(float));
	npts = (int*)malloc(nang*sizeof(int));
	a0 = (double*) malloc(nint*maxspl*sizeof(double));
	a1 = (double*) malloc(nint*maxspl*sizeof(double));
	a2 = (double*) malloc(nint*maxspl*sizeof(double));
	a3 = (double*) malloc(nint*maxspl*sizeof(double));
	sign = (float*)malloc(nint*sizeof(float));
	norder = (int*)malloc(nint*sizeof(int));

 	t = (float*)malloc(nzo*nxo*sizeof(float));
	work = (float*)malloc(nzo*nxo*sizeof(float));

	/* amplitude array and angle array */
	if(iamp==1) { 
 		a = (float*)malloc(nzo*nxo*sizeof(float));
    		p = (float*)malloc(nzo*nxo*sizeof(float));
	}
	  
	flag = 1;
	one = 1;
	dx = dxo / 2.;
	amin = angmin * 3.141592654/180.;
	amax = angmax * 3.141592654/180.;

	xmin = xint[0];
	xmax = xint[npicks[0]-1];

	/* loop over sources */
	for (is=is0;is<ns;is++) {
		xsi = xs[is];
		zsi = 0.;

		if(fabs(xsi-xmin<0.001) &&  xsi<=xmin) xsi = xsi + 0.001;
		if(fabs(xsi-xmax<0.001) &&  xsi>=xmax) xsi = xsi - 0.001;

		/* velocities at this shot */
		for(i=0;i<nint;i++) {
			nn = npicks[i];
			bisear_(&nn,&one,xpicks+i*maxpks,&xsi,&j);
			v[i] = vint[i+(j-1)*nint];
		} 

	   	/* compute times */
		cshoot(xint, zint, vint, maxspl, nint, npicks,
			a0, a1, a2, a3,
			sign, norder, flag, 
			xsi, angmin, dang, nang, v,
			xstart, xend, dx,
			xray, zray, tray, npts);
		flag = 0;
	
		/* ray checking */
		for(i=0;i<nang;i++) {
			k = 0;
			nc = i * nint;
			for(j=0;j<npts[i];j++) {
				if(xray[nc+j]>=xstart && xray[nc+j]<=xend &&
				   zray[nc+j]<=zmax) {
					xray[nc+k] = xray[nc+j];
					zray[nc+k] = zray[nc+j];
					tray[nc+k] = tray[nc+j];
					k = k + 1;
				}
			}
			npts[i] = k;
		}

		/* plot ray paths */
		if(rayplot!=0) {
			j = 0;
			ncurve = 0;
			for(i=0;i<nhs;i++) {
				j = j + npicks[i];
				ncurve = ncurve + 1;
			} 
			for(i=0;i<nang;i++) {
				j = j + npts[i];
				ncurve = ncurve + 1;
			}
			plotdata = (float*) malloc(j*2*sizeof(float));
			nplots = (int*) malloc(ncurve*sizeof(int));
			k = 0;
			nc = 0; 
			for(i=0;i<nhs;i++) {
				for(j=0;j<npicks[i];j++) {
					plotdata[k] = zpicks[j+i*maxpks];
					plotdata[k+1] = xpicks[j+i*maxpks];
					k = k + 2;
				}
				if(npicks[i]>0) {
					nplots[nc] = npicks[i];
					nc = nc + 1;
				}
			}
			for(i=0;i<nang;i++) {
				for(j=0;j<npts[i];j++) {
					plotdata[k] = zray[j+i*nint];
					plotdata[k+1] = xray[j+i*nint];
					k = k + 2;
				}
				if(npts[i]>0) {
					nplots[nc] = npts[i];
					nc = nc + 1;
				}
			}

			bzero(title,80);
			sprintf(title,"ray paths at source=%d xs=%g \n",
				is+1,xs[is]);
			if(rayplot==1) {
				dump2xgraph(plotdata,nplots,nc,title,
					label1,label2,style);
			}else if(rayplot==-1) {
				dump2psgraph(plotdata,nplots,nc,title,
					label1,label2,style);
			}
			
			free(nplots);
			free(plotdata);
			
		}
		/*	
		for(i=0;i<nang;i++) {
			for(j=0;j<npts[i];j++) {
	fprintf(stderr,"xray=%g zray=%g tray=%g point=%d ray=%d \n",
					xray[i*nint+j],
					zray[i*nint+j],
					tray[i*nint+j],
					j, i);
			}
		}
		*/

	   	/* interpolation time to grid */
		vs = v[0];
		ray2grd(xray, zray, tray, npts,
			nang, nint, 
			xsi, 0, treplace, 
			fxo, fzo, dxo, dzo, nxo, nzo, 
			t, maxfac); 
	
		/* extrapolation travel time if needed in shadow zones */
		if(fillsd==1) {
			for(i=0;i<nxo;i++) {
				for(j=0;j<nzo;j++) {
					zz = fzo + j*dzo;
					if(t[i*nzo+j]==treplace &&
						zz>=zminfl && zz<=zmaxfl) {
						disz = nxo; 
						disx = nzo;
						ix = nxo + 1;
						iz = nzo + 1;
						for(jj=0;jj<nzo;jj++) {
							tmp = jj - j;
							if(t[i*nzo+jj]!=
								treplace &&
								fabs(tmp)<disz){
								iz = jj;
								disz=fabs(tmp);
							}
						}
						for(ii=0;ii<nxo;ii++) {
							tmp = ii - i;
							if(t[ii*nzo+j]!=
								treplace &&
						   		fabs(tmp)<disx){
								ix = ii;
								disx=fabs(tmp);
							}
						}
						if(disx<nxo && disz<nzo) {
							tmp = disx*dxo+disz*dzo;
							work[i*nzo+j] =  
						   	(t[i*nzo+iz]*disx*dxo + 
						   	t[ix*nzo+j]*disz*dzo)
							/tmp;
						} else {
							work[i*nzo+j] =
								t[i*nzo+j];
						}
					} else {
						work[i*nzo+j] = t[i*nzo+j];
					}		
				}
			}
			bcopy(work,t,nxo*nzo*sizeof(float));
		} 
		/* output grid */
	        fwrite(t,sizeof(float),nxo*nzo,tfp);
		fminmax(t,nxo*nzo,&gmin,&gmax);
	   	if(itg==0) {
			tgmin = gmin;
			tgmax = gmax;
			itg = 1;
	   	} else {
			if(tgmin>gmin) tgmin = gmin;
			if(tgmax<gmax) tgmax = gmax;
	   	}

	   	/* compute ampitudes and angles */
	   	if ( iamp == 1 ) {

			/* normalize distance */
			xsi1 = xsi / dzo;
			zsi1 = zsi / dzo;
			fxo1 = fxo / dzo;
			fzo1 = fzo / dzo;
			dzo1 = dzo / dzo;
			dxo1 = dxo / dzo;

	        	amp2d_ (t,a,p,&nzo,&nxo,&amin,&amax,work,
				&amptype,&xsi1,&zsi1,&fxo1,&fzo1,&dxo1,&dzo1);

	        	/* output amplitude */
	        	if (afile[0]!='\0') { 
	         		fwrite(a,sizeof(float),nxo*nzo,afp);
				fminmax(a,nxo*nzo,&gmin,&gmax);
	             		if(iag==0) {
					agmin = gmin;
					agmax = gmax;
					iag = 1;
	   			} else {
					if(agmin>gmin) agmin = gmin;
					if(agmax<gmax) agmax = gmax;
				}
	   		}

	        	/* output propagation angle */
	        	if (pfile[0]!='\0') { 
	         		fwrite(p,sizeof(float),nxo*nzo,pfp);
				fminmax(p,nxo*nzo,&gmin,&gmax);
	             		if(ipg==0) {
					pgmin = gmin;
					pgmax = gmax;
					ipg = 1;
	   			} else {
					if(pgmin>gmin) pgmin = gmin;
					if(pgmax<gmax) pgmax = gmax;
				}
	   		}
	   	}

   fprintf(stderr,"travel time computed at source=%d xs=%g \n",is+1,xs[is]);

	   	if( fabs(smax0-xs[is]) < 0.1*dxs ) {
			scale = 1.e-6;
			dtype = 4;
			n1 = nzo;
			n2 = nxo;
			n3 = ns0;
			n4 = 1;
			n5 = 1;
			d1 = dzo;
			d2 = dxo;
			d3 = dxs;
			d4 = 0.;
			d5 = 0.;
			o1 = fzo; 
			o2 = fxo; 
			o3 = os0; 
			o4 = 0.;
			o5 = 0.;
			ocdp2 = 0.;
			oline3 = 0.;
			dcdp2 = 0.;
			dline3 = 0.;
			gmin = 0.;
			gmax = 0.;
		
			fflush(tfp);
			toghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
                		&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                		&dcdp2,&dline3,&ocdp2,&oline3,&tgmin,&tgmax);
			fputghdr(tfp,&gh);

			if (afile[0]!='\0') { 
				fflush(afp);
				putgval(&gh,"gmin",agmin);
				putgval(&gh,"gmax",agmax);
				fputghdr(afp,&gh);
			}
			if (pfile[0]!='\0') {
				fflush(pfp);
				putgval(&gh,"gmin",pgmin);
				putgval(&gh,"gmax",pgmax);
				fputghdr(pfp,&gh);
			}

			/* output hisfile */
			fprintf(hfp,
			"time/amplitude table parameter for KIRMIG \n"); 
			fprintf(hfp,
			"xmint=%f zmint=%f smint=%f \n",fxo,fzo,os0); 
			fprintf(hfp,"dxt=%f dzt=%f dst=%f \n",dxo,dzo,dxs); 
			fprintf(hfp,"nxt=%d nzt=%d nst=%d \n",nxo,nzo,ns0);
			if(amptype==3) {
				fprintf(hfp,"amptype=%d \n",0);
			} else {
				fprintf(hfp,"amptype=%d \n",amptype);
			}

	   	}
	}

	fclose(hfp);
	fclose(tfp);
	if (afile[0]!='\0') fclose(afp);
	if (pfile[0]!='\0') fclose(pfp);
	/* free space */
	free(xpicks);
	if(rayplot!=0) free(zpicks);
	free(npicks);
        free(t);
	free(work);
	if(iamp==1) {
		free(a);
		free(p);
	}
	free(a0);
	free(a1);
	free(a2);
	free(a3);
	free(sign);
	free(norder);
	free(xray);
	free(zray);
	free(tray);
	free(npts);
	
	return 0;
}

