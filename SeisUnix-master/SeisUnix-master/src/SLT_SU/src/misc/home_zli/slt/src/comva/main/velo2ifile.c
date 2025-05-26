
#include "comva.h"
#include "par.h"

/*********************** self documentation *****************************/
string sdoc =
" 									\n"
" VELO2IFILE - VELO CARDS to INTERFACE FILE (IFILE) CONVERSION 		\n"
" 									\n"
" velo2ifile <velocards oldifile= [optional parameters] >newifile	\n"
"\n"
"Required parameters:							\n"
"velocards              file name of VELO/DVDZ cards			\n"
"oldifile=              file name of ifile in which x-z \n"
"                       picks are specified for each horizon     	\n"
"newifile=              file name of new ifile 				\n"
"Optional parameters: 							\n"
"vtopupdate=1,2,3,...   horizon numbers to update top velocities        \n"
"                       (default is all horizons)                       \n"
"vbotupdate=1,2,3,...   horizon numbers to update bottom velocities     \n"
"                       (default is all horizons)                       \n"
"ism=1                  smoothing option; to laterally smooth interval  \n"
"                         velocities at top and bottom, and the interval \n"
"                         velocity gradient, along interface,		\n" 
"                         before output (applied to updated horizons only)\n"
"                         0=average all values at picks along interface and \n"
"                           this single value is assigned to all output \n"
"                           points along the interface \n"
"                         1=no smoothing \n"
"                         n=smooting values at picks along interface using \n"
"                           n data points \n"
"dcdp=from oldifile     cdp spacing.  If not given, it will be read from\n"
"                         oldifile. When oldifile is Hfile input, dcdp \n"
"                         must be given if Hfile does not have this value\n"
"xtol=0.                horizons extend, at both ends, a distance of xtol\n"
"                       to avoid discontinuity.               	\n"
"\n"
" author: Zhiming Li		      		5/11/92			\n"
;
/**************** end self doc *******************************************/

/* define maximum number of horizons and maximum number of picks per horizon */
#define maxhs 128
#define maxpks 256

/* functions defined and used internally */

void vtopbot(int maxp,int maxcdp, 
     	int ncdp_velo,float *x_velo,float *z_velo,float *v_velo,int *nps_velo,
	int ncdp_dvdz,float *x_dvdz,float *zt_dvdz,float *zb_dvdz, 
	float *dvdz_dvdz,int *nps_dvdz,
	int nhs, float *xpicks, float *zpicks, int *npicks, float xtol, 
	float *veltops, float *velbots, float *velavgs, float *dvdzs,
	int *vtopupdate, int *vbotupdate, int *hnums);

void ifileout(FILE *ifilefp, float *xpicks, float *zpicks,
	float *veltop,float *velbot, int *dif,
	float *dens,float *qval,float *pois,
	float *velavg, float *dvdz,
	int *npicks, int nhs, int ism, 
	float xmin, float xmax, float zmin, float zmax,
	int cdpxmin, int cdpxmax, float dcdp, 
	int *hnums, char hnames[maxhs][80],
	char *dunits, char *zattrib,
	int *vtopupdate, int *vbotupdate);

void ilimit(int *i,int i1,int i2);


main (int argc, char **argv) 
{

	int *npicks,*hnums,ih,nhs,*difs,
		cdpxmini,cdpxmaxi,
		ism,ncdps_velo,ncdps_dvdz,maxp,maxcdp,
		*nps_velo,*nps_dvdz,*sortindex,*isort,
		*cdps_velo,*cdps_dvdz,jh,i,ihfile,
		vtopupdate[maxhs], vbotupdate[maxhs];
	float *xpicks,*zpicks,*veltops,*velbots,*velavgs,*dvdzs, 
		*dens,*qval,*pois,
		xmini,xmaxi,zmini,zmaxi,dcdpi,vmini,vmaxi,xtol,
		*z_velo,*x_velo,*v_velo,*sorts,*cdpsort,dcdp,
		*zt_dvdz,*zb_dvdz,*dvdz_dvdz,*x_dvdz;
	char hnames[maxhs][80], dunitsi[80], zattribi[80], vtypei[80],
		*oldifile;
	FILE *infp=stdin, *ifoldfp, *outfp=stdout;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get old.ifile name */
	if (!getparstring("oldifile", &oldifile)) 
		err("must specify oldifile !");
	ifoldfp = efopen(oldifile,"r"); 
	
	/* get horizon numbers to update velocities */
	for(ih=0;ih<maxhs;ih++) {
		vtopupdate[ih] = ih + 1;
		vbotupdate[ih] = ih + 1;
	}
	if (jh=getparint("vtopupdate",vtopupdate))
                for (ih=jh; ih<maxhs; ih++) vtopupdate[ih] = 0;

	if (jh=getparint("vbotupdate",vbotupdate))
                for (ih=jh; ih<maxhs; ih++) vbotupdate[ih] = 0;

	/* get length of smoothing operator */
	if(!getparint("ism",&ism)) ism=1;
	if(!getparfloat("xtol",&xtol)) xtol=0.;

	/* memory allocations */
	xpicks = (float *) malloc(maxhs*maxpks*sizeof(float));
	zpicks = (float *) malloc(maxhs*maxpks*sizeof(float));
	veltops = (float *) malloc(maxhs*maxpks*sizeof(float));
	velbots = (float *) malloc(maxhs*maxpks*sizeof(float));
	npicks = (int *) malloc(maxhs*sizeof(int));
	hnums = (int *) malloc(maxhs*sizeof(int));
	difs = (int *) malloc(maxhs*maxpks*sizeof(int));
	velavgs = (float *) malloc(maxhs*maxpks*sizeof(float));
	dvdzs = (float *) malloc(maxhs*maxpks*sizeof(float));
	dens = (float *) malloc(maxhs*maxpks*sizeof(float));
	qval = (float *) malloc(maxhs*maxpks*sizeof(float));
	pois = (float *) malloc(maxhs*maxpks*sizeof(float));

	for(ih=0;ih<maxhs;ih++) npicks[ih]=0;
	for(ih=0;ih<maxhs*maxpks;ih++) {
		veltops[ih]=0.;
		velbots[ih]=0.;
		difs[ih] = 0;
		velavgs[ih] = 0.;
		dvdzs[ih] = 0.;
		qval[ih] = 0.;
		dens[ih] = 0.;
		pois[ih] = 0.;
	}

	/* read in old ifile*/
	nhs = 0;
	dcdpi = 0.;
	ifileread(ifoldfp,&nhs,xpicks,zpicks,veltops,velbots,difs,
		  dens,qval,pois,velavgs,dvdzs,
              	  (char *)hnames,hnums,npicks,maxhs,maxpks,
		  &xmini,&xmaxi,&zmini,&zmaxi,
		  &cdpxmini,&cdpxmaxi,&vmini,&vmaxi,
		  vtypei,dunitsi,zattribi,&dcdpi,&ihfile);
	if(!getparfloat("dcdp",&dcdp)) dcdp = 0.;
	if(dcdp!=0.) {
		if(dcdpi!=0. && dcdpi!=dcdp )
	warn("input dcdp not the same as dcdp from oldifile, input dcdp used");
		dcdpi = dcdp;
	} 
	if(dcdpi==0.) err("dcdp must be specified");

	/* read in VELO cards and DVDZ cards dataset */
	ncdps_velo = 0;
	ncdps_dvdz = 0;
	maxp = 256;
	maxcdp= 128;

	cdps_velo = (int*) malloc(maxcdp*sizeof(int));
	x_velo = (float*) malloc(maxcdp*sizeof(float));
	z_velo = (float*) malloc(maxp*maxcdp*sizeof(float));
 	v_velo = (float*) malloc(maxp*maxcdp*sizeof(float));
	nps_velo = (int*) malloc(maxcdp*sizeof(int));
	sorts = (float*) malloc(maxp*maxcdp*sizeof(float));
	sortindex = (int*) malloc(maxcdp*sizeof(int));
	isort = (int*) malloc(maxcdp*sizeof(int));
	cdpsort = (float*) malloc(maxcdp*sizeof(float));

	veloread(infp,cdps_velo,z_velo,v_velo,
		  &ncdps_velo,nps_velo, maxp, maxcdp);
	/* sort cdps of VELO cards into ascending order */
	for(i=0;i<ncdps_velo;i++) {
		sortindex[i] = i;
		cdpsort[i] = cdps_velo[i];
	}
        if(ncdps_velo>0) qkisort(ncdps_velo,cdpsort,sortindex);
        for(i=0;i<ncdps_velo;i++) {
            	cdps_velo[i] = cdpsort[sortindex[i]];
                isort[i] = nps_velo[sortindex[i]];
        }
        for(i=0;i<ncdps_velo;i++) {
                nps_velo[i] = isort[i];
        }
        for(i=0;i<ncdps_velo;i++) {
                bcopy((char*)(v_velo+sortindex[i]*maxp),
                     (char*)(sorts+i*maxp),maxp*4);
        }
	for(i=0;i<ncdps_velo;i++) {
                bcopy((char*)(sorts+i*maxp),
                      (char*)(v_velo+i*maxp),maxp*4);
        }
        for(i=0;i<ncdps_velo;i++) {
                bcopy((char*)(z_velo+sortindex[i]*maxp),
                      (char*)(sorts+i*maxp),maxp*4);
        }
	for(i=0;i<ncdps_velo;i++) {
                bcopy((char*)(sorts+i*maxp),
                      (char*)(z_velo+i*maxp),maxp*4);
        }
	/* DVDZ cards */
	cdps_dvdz = (int*) malloc(maxcdp*sizeof(int));
	x_dvdz = (float*) malloc(maxcdp*sizeof(float));
	zt_dvdz = (float*) malloc(maxp*maxcdp*sizeof(float));
	zb_dvdz = (float*) malloc(maxp*maxcdp*sizeof(float));
	dvdz_dvdz= (float*) malloc(maxp*maxcdp*sizeof(float));
        nps_dvdz = (int*) malloc(maxcdp*sizeof(int));
	dvdzread(infp,cdps_dvdz,zt_dvdz,zb_dvdz,dvdz_dvdz,
		 &ncdps_dvdz,nps_dvdz,maxp,maxcdp);
	/* sort cdps of DVDZ cards into ascending order */
	for(i=0;i<ncdps_dvdz;i++) {
		sortindex[i] = i;
		cdpsort[i] = cdps_dvdz[i];
	}
    	if(ncdps_dvdz>0) qkisort(ncdps_dvdz,cdpsort,sortindex);
    	for(i=0;i<ncdps_dvdz;i++) {
        	cdps_dvdz[i] = cdpsort[sortindex[i]];
        	isort[i] = nps_dvdz[sortindex[i]];
    	}
    	for(i=0;i<ncdps_dvdz;i++) {
        	nps_dvdz[i] = isort[i];
	}
    	for(i=0;i<ncdps_dvdz;i++) {
        	bcopy((char*)(dvdz_dvdz+sortindex[i]*maxp),
		      (char*)(sorts+i*maxp),maxp*4);
	}
    	for(i=0;i<ncdps_dvdz;i++) {
       		bcopy((char*)(sorts+i*maxp),
		      (char*)(dvdz_dvdz+i*maxp),maxp*4);
	}
    	for(i=0;i<ncdps_dvdz;i++) {
        	bcopy((char*)(zt_dvdz+sortindex[i]*maxp),
		      (char*)(sorts+i*maxp),maxp*4);
	}
    	for(i=0;i<ncdps_dvdz;i++) {
        	bcopy((char*)(sorts+i*maxp),
		      (char*)(zt_dvdz+i*maxp),maxp*4);
	}
    	for(i=0;i<ncdps_dvdz;i++) {
       		bcopy((char*)(zb_dvdz+sortindex[i]*maxp),
		      (char*)(sorts+i*maxp),maxp*4);
	}
    	for(i=0;i<ncdps_dvdz;i++) {
       		bcopy((char*)(sorts+i*maxp),
		      (char*)(zb_dvdz+i*maxp),maxp*4);
	}
	free(isort);
	free(sorts);
	free(sortindex);
	free(cdpsort);

	/* convert cdp locations of VELO into x locations */
	for(i=0;i<ncdps_velo;i++) { 
		x_velo[i] = xmini + (cdps_velo[i]-cdpxmini)*dcdpi;
	}
	/* convert cdp locations of DVDZ into x locations */
	/* still store in cdps_dvdz */
	for(i=0;i<ncdps_dvdz;i++) 
		x_dvdz[i] = xmini + (cdps_dvdz[i]-cdpxmini)*dcdpi;
	free(cdps_velo);
	free(cdps_dvdz);
	

        vtopbot(maxp,maxcdp,ncdps_velo,x_velo,z_velo,
		v_velo,nps_velo,
		ncdps_dvdz,x_dvdz,zt_dvdz,zb_dvdz, 
		dvdz_dvdz,nps_dvdz,
		nhs,xpicks,zpicks,npicks,xtol, 
		veltops,velbots,velavgs,dvdzs,vtopupdate,vbotupdate,
		hnums);

	ifileout(outfp, xpicks, zpicks,
		veltops, velbots, difs,
		dens,qval,pois,
		velavgs, dvdzs,
		npicks, nhs, ism, 
		xmini, xmaxi, zmini, zmaxi,
		cdpxmini, cdpxmaxi, dcdpi, 
		hnums, hnames,
		dunitsi, zattribi,
		vtopupdate,vbotupdate);

	return EXIT_SUCCESS;
}

void ilimit(int *i, int i1, int i2) {
	if (*i<i1) {
 		*i = i1;
	}else if(*i>i2) {
		*i = i2;
	}
}

void ifileout(FILE *ifilefp, float *xpicks, float *zpicks,
	float *veltop,float *velbot, int *dif,
	float *dens, float *qval, float *pois,
	float *velavg, float *dvdz,
	int *npicks, int nhs, int ism, 
	float xmin, float xmax, float zmin, float zmax,
	int cdpxmin, int cdpxmax, float dcdp, 
	int *hnums, char hnames[maxhs][80],
	char *dunits, char *zattrib,
	int *vtopupdate, int *vbotupdate) {



	int ic,ip,np,ih,jp,nhout,h0; 
	int p1,p2,i,ismx,ismxh;
	float vmax, vmin, s;
	float *w, *f;
	float vt, vb, g;

	int jh, ig;



	/* lateral smooth velocities */
	if(ism==0) {
	/* take average */ 
		for(ih=0;ih<nhs;ih++) {
			vt = 0.;
			vb = 0.;
			g = 0.;
			np = npicks[ih];
			ig = 0;
			for(jh=0;jh<maxhs;jh++) {
                                if(ih+1 == vtopupdate[jh]) {
					ig = 1;	
                                        for(ip=0;ip<np;ip++) {
                                                vt += veltop[ip+ih*maxpks];
						g += dvdz[ip+ih*maxpks];
                                        }
                                        if(np>0) vt = vt/np;
					if(np>0) g = g/np;
                                        for(ip=0;ip<np;ip++) {
                                                veltop[ip+ih*maxpks] = vt;
						dvdz[ip+ih*maxpks] = g;
					}
                                        break;
                                }
                        }
			for(jh=0;jh<maxhs;jh++) {
                                if(ih+1 == vbotupdate[jh]) {
                                        for(ip=0;ip<np;ip++) {
						vb += velbot[ip+ih*maxpks];
						if(ig==0) 
						   g += dvdz[ip+ih*maxpks];
                                        }
                                        if(np>0) vb = vb/np;
					if(np>0) g = g/np;
                                        for(ip=0;ip<np;ip++) {
						velbot[ip+ih*maxpks] = vb;
						if(ig==0) 
						   dvdz[ip+ih*maxpks] = g;
					}
                                        break;
                                }
                        }
		}
	} else if (ism>1) {
	/* smoothing */
		for(ih=0;ih<nhs;ih++) {
			h0 = ih * maxpks;
			np = npicks[ih];
			if(np<2) continue;
			ismx = ism;
			ismx = (ismx/2)*2+1;
			if(ismx>np) ismx=(np/2)*2+1;
			f = (float *) malloc(ismx*sizeof(float));
			w = (float *) malloc(np*sizeof(float));
			/* triangular smoothing coefficients */
			ismxh = (ismx+1)/2;
			s = 0.;
			for(i=0;i<ismx;i++) {
				f[i] = ismxh - abs(i+1-ismxh);
				s += f[i];
			}
			s = 1./s;
			for(i=0;i<ismx;i++) {
				f[i]=f[i]*s;
			}
			/* apply smoothing to veltop, velbot and dvdz */
			ic = 1;
			ig = 0;
			for(jh=0;jh<maxhs;jh++) {
                                if(ih+1 == vtopupdate[jh]) {
					ig = 1;		
					smth1d_(veltop+h0,f,w,&np,&ismx,&ic);
					for(ip=0;ip<np;ip++) 
						veltop[ip+h0] = w[ip];
					smth1d_(dvdz+h0,f,w,&np,&ismx,&ic);
					for(ip=0;ip<np;ip++) 
						dvdz[ip+h0] = w[ip];
					break;
				}
			}
			for(jh=0;jh<maxhs;jh++) {
                                if(ih+1 == vbotupdate[jh]) {
					smth1d_(velbot+h0,f,w,&np,&ismx,&ic);
					for(ip=0;ip<np;ip++) 
						velbot[ip+h0] = w[ip];
					if(ig==0) {	
						smth1d_(dvdz+h0,f,w,
							&np,&ismx,&ic);
						for(ip=0;ip<np;ip++) 
							dvdz[ip+h0] = w[ip];
					}
					break;
				}
			}
			

			free(w);
			free(f);
		}
	}

	/* find maximum and minimum velocities */
	vmin = veltop[0];
	vmax = veltop[0];
	for (ih=0;ih<nhs;ih++) {
		for(ip=0;ip<npicks[ih];ip++) {
			jp = ip + ih * maxpks;
			if(vmin>veltop[jp]) vmin = veltop[jp];
			if(vmin>velbot[jp]) vmin = velbot[jp];
			if(vmax<veltop[jp]) vmax = veltop[jp];
			if(vmax<velbot[jp]) vmax = velbot[jp];
		}
	}

	nhout = 0;
	for(ih=0;ih<nhs;ih++) {
		if(npicks[ih]>1) nhout = nhout + 1;
	}


	/* output picks */

	fprintf(ifilefp, 
	"IFILE   <--- FILE FORMAT DESCRIPTOR (INTERFACE FILE) \n");
	fprintf(ifilefp,"\n");
	fprintf(ifilefp, "MODEL.X-MIN         %8.1f\n",xmin);
	fprintf(ifilefp, "MODEL.X-MAX         %8.1f\n",xmax);
	fprintf(ifilefp, "MODEL.Z-MIN         %8.1f \n",zmin);
	fprintf(ifilefp, "MODEL.Z-MAX         %8.1f \n",zmax);
	fprintf(ifilefp, "MODEL.CDP#-AT-X-MIN %6d\n",cdpxmin);
	fprintf(ifilefp, "MODEL.CDP#-AT-X-MAX %6d\n",cdpxmax);
	fprintf(ifilefp, "MODEL.MIN-VELOCITY  %8.1f\n",vmin); 
	fprintf(ifilefp, "MODEL.MAX-VELOCITY  %8.1f\n",vmax); 
	fprintf(ifilefp, "MODEL.#HORIZONS     %6d\n",nhout); 
	fprintf(ifilefp, "MODEL.VELOCITY-TYPE  INTERVAL\n"); 
	fprintf(ifilefp, "MODEL.DIST-UNITS     %s\n",dunits); 
	fprintf(ifilefp, "MODEL.Z-ATTRIBUTE    %s\n",zattrib); 
	fprintf(ifilefp, "MODEL.CDP-SPACING   %8.1f\n",dcdp); 
	fprintf(ifilefp, "\n"); 

	
	for(ih=0;ih<nhs;ih++) {

		np = npicks[ih];
		if(np<=1) continue;

		fprintf(ifilefp, "\n"); 
		fprintf(ifilefp, "\n"); 
		fprintf(ifilefp, "HORIZON-NUMBER %3d\n", hnums[ih]); 
		fprintf(ifilefp, "HORIZON-NAME     %s\n", hnames[ih]); 
		fprintf(ifilefp, "* \n"); 
		fprintf(ifilefp, 
"* X-VALUE   Z-VALUE   VELTOP   VELBOT  DIF  DENS  QVAL  POIS   VELAVG   DVDZ \n");
		fprintf(ifilefp, 
"* =======  ========  =======  =======  ===  ====  ====  ====  =======  ======= \n");

		h0 = ih*maxpks;
		
		for (ip=0;ip<npicks[ih];ip++) {

   			fprintf(ifilefp, 
"%9.1f %9.1f %8.0f %8.0f   %1d   %4.2f  %4.0f  %4.2f %8.0f %8.3f \n",
				xpicks[ip+h0],zpicks[ip+h0],
				veltop[ip+h0],velbot[ip+h0],dif[ip+h0],
				dens[ip+h0],qval[ip+h0],pois[ip+h0],
				velavg[ip+h0],dvdz[ip+h0]);
		}
	}
}


/* compute velocities from VELO and DVDZ cards */
void vtopbot(int maxp,int maxcdp, 
     int ncdp_velo,float *x_velo,float *z_velo,float *v_velo,int *nps_velo,
     int ncdp_dvdz,float *x_dvdz,float *zt_dvdz,float *zb_dvdz, 
     float *dvdz_dvdz, int *nps_dvdz,
     int nhs,float *xpicks,float *zpicks,int *npicks,float xtol, 
     float *veltops,float *velbots,float *velavgs,float *dvdzs,
     int *vtopupdate, int *vbotupdate, int *hnums) {

	float x, z, va, g, vat, gt, vab, gb;
	int ih, ip, jh;
	float zmax, xmin, xmax, zt, zb, tmp;
	float *vth, *vbh, *vah, *dvdzh, *xvh, *zvs, *zvh;
	float *zgrid, *vagrid, *dvdzgrid, dz, *g1, *g2, *z2, z21, z2n;
	int i1,i2,n1,n2,one=1,nzgrid;
	int *ivs, *nvh;
	int iz, n, j2, j1, nvs;
	float *xnew, *znew, *vanew, *vtnew, *vbnew, *gnew, xpre, xnow;
	int *indx, inow;

	float *zsort;
	int *isort;
	char vtopyes, vbotyes;

	int it1, it2, ib1, ib2, ii;
	
	
	if(ncdp_velo==0) {
		/* if no velo card input, simply return with warning message */
		warn("no VELO card input ! \n");
	} else {
		/* find maximum depth */
		zmax = 0.;
		for(ih=0;ih<nhs;ih++) {
			for(iz=0;iz<npicks[ih];iz++) {
				if(zmax<zpicks[ih*maxpks+iz])
				  	zmax=zpicks[ih*maxpks+iz];
			}
		}


		for(i2=0;i2<ncdp_velo;i2++) {
			for(iz=0;iz<nps_velo[i2];iz++) {
				if(zmax<z_velo[i2*maxp+iz])
				  	zmax=z_velo[i2*maxp+iz];
			}
		}




		nzgrid = 1024;
		zgrid = (float *) malloc(nzgrid*sizeof(float));
		vagrid = (float *) malloc(nzgrid*ncdp_velo*sizeof(float));
		dvdzgrid = (float *) malloc(nzgrid*ncdp_velo*sizeof(float));
		g1 = (float *) malloc(nzgrid*sizeof(float));
		g2 = (float *) malloc(nzgrid*sizeof(float));

		vth = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		vbh = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		vah = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		dvdzh = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		xvh = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		zvh = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		nvh = (int *) malloc(nhs*sizeof(int));

		zvs = (float *) malloc(nhs*sizeof(float));
		ivs = (int *) malloc(nhs*sizeof(int));
		zsort = (float *) malloc(nhs*sizeof(float));
		isort = (int *) malloc(nhs*sizeof(int));

		vtnew = (float *) malloc(maxpks*sizeof(float));
		vbnew = (float *) malloc(maxpks*sizeof(float));
		indx = (int *) malloc(maxpks*sizeof(int));
		vanew = (float *) malloc(maxpks*sizeof(float));
		gnew = (float *) malloc(maxpks*sizeof(float));
		xnew = (float *) malloc(maxpks*sizeof(float));
		znew = (float *) malloc(maxpks*sizeof(float));

		z2 = (float *) malloc(nhs*sizeof(float));
		/* spline interpolation of velocities --- not a good idea 
		vt2 = (float *) malloc(maxpks*sizeof(float));
		vb2 = (float *) malloc(maxpks*sizeof(float));
		va2 = (float *) malloc(maxpks*sizeof(float));
		dvdz2 = (float *) malloc(maxpks*sizeof(float));
		*/

	

		dz = zmax/(nzgrid-2);
		for(iz=0;iz<nzgrid;iz++) zgrid[iz] = iz * dz;
		/* compute average velocity and gradient at velo location and
		   at depth zgrid */


		for(i2=0;i2<ncdp_velo;i2++) {
			n2 = i2*nzgrid;
			/* find gradient */
			x = x_velo[i2];

			if(ncdp_dvdz==0) {
				for(iz=0;iz<nzgrid;iz++) 
					dvdzgrid[iz+n2] = 0.;
			} else if (ncdp_dvdz==1) {
				n = nps_dvdz[0];
				dvdzint(zt_dvdz,zb_dvdz,dvdz_dvdz,n,
					zgrid,dvdzgrid+n2,nzgrid);
			} else {
				bisear_(&ncdp_dvdz,&one,x_dvdz,&x,&j2);
				if(x<=x_dvdz[0]) {
					n = nps_dvdz[0];
					dvdzint(zt_dvdz,zb_dvdz,dvdz_dvdz,n,
					     zgrid,dvdzgrid+n2,nzgrid);
				} else if (x>=x_dvdz[ncdp_dvdz-1]) {
					n = nps_dvdz[ncdp_dvdz-1];
					i1 = (ncdp_dvdz-1)*maxp;
					dvdzint(zt_dvdz+i1,zb_dvdz+i1,
						dvdz_dvdz+i1,n,zgrid,
						dvdzgrid+n2,nzgrid);
				} else {
					j1 = j2 - 1;
					n = nps_dvdz[j1];
					i1 = j1*maxp;
					dvdzint(zt_dvdz+i1,zb_dvdz+i1,
						dvdz_dvdz+i1,n,
						zgrid,g1,nzgrid);
					n = nps_dvdz[j2];
					i1 = j2*maxp;
					dvdzint(zt_dvdz+i1,zb_dvdz+i1,
						dvdz_dvdz+i1,n,
						zgrid,g2,nzgrid);
					tmp = (x-x_dvdz[j1]) /
					      (x_dvdz[j2]-x_dvdz[j1]);
				
					for(iz=0;iz<nzgrid;iz++) {
						dvdzgrid[iz+n2] = g1[iz]+
						tmp * (g2[iz]-g1[iz]);
					}
				}
			}
			/* find average velocity at zgrid */
			n = nps_velo[i2];
			n1 = i2*maxp;
			j1 = 0;
			j2 = 0;
			vconint(z_velo+n1,v_velo+n1,n,zgrid,vagrid+n2,nzgrid,
				j1,j2,dvdzgrid+n2);

		}
			


		/* compute vt and vb along horizons at velan locations */

		for(ih=0;ih<nhs;ih++) nvh[ih] = 0;
		for(i2=0;i2<ncdp_velo;i2++) {
			x = x_velo[i2];	
			jh = 0;
			/* find cross points of horizons at velan locations */

			for(ih=0;ih<nhs;ih++) {
				xmin = xpicks[ih*maxpks]-xtol;
				xmax = xpicks[ih*maxpks+npicks[ih]-1]+xtol;
				n1 = npicks[ih]; 
				n2 = ih * maxpks;
				if(x>=xmin && x<=xmax) {
					ivs[jh] = ih;
					/* find z */
					bisear_(&n1,&one,xpicks+n2,&x,&i1);
					if(x<=xpicks[n2]) {
						z = zpicks[n2];
					} else if(x>=xpicks[n1+n2-1]) {
						z = zpicks[n1+n2-1];
					} else {
						/*
						if(n1==2) {
						*/
						/* linear interpolation */
						   z = zpicks[n2+i1-1] +
						    (x-xpicks[n2+i1-1])*
						    (zpicks[n2+i1]-
						     zpicks[n2+i1-1])/
						    (xpicks[n2+i1]-
						     xpicks[n2+i1-1]);
						/*
						} else {
						*/
						/* spline interpolation */
						/* not a good idea */
						/*
						   z21 = 0.;
                                 		   z2n = 0.;
                                                   spline_(xpicks+n2,zpicks+n2,
                                         		   &n1,&z21,&z2n,z2);
                                 		   splint_(xpicks+n2,zpicks+n2,
							   z2,&n1,&x,&z);
						}
						*/
					}
					zvs[jh] = z; 
					jh = jh + 1;
				}
			}

			/* sort zvs into ascending order */
			for (iz=0;iz<jh;iz++) {
				isort[iz] = iz;
				zsort[iz] = zvs[iz];
			}

			qkisort(jh,zsort,isort);
			for(iz=0;iz<jh;iz++) zvs[iz] = zsort[isort[iz]];

			/* find vt and vb */
			for(iz=0;iz<jh;iz++) {

				z = zvs[iz];
				tmp = z/dz;
				i1 = tmp;
				it1 = i1 - 2;
				it2 = i1 - 1; 
				ib1 = i1;
				ib2 = i1 + 1;

				ilimit(&it1,0,nzgrid-1);
				ilimit(&it2,0,nzgrid-1);
				ilimit(&ib1,0,nzgrid-1);
				ilimit(&ib1,0,nzgrid-1);
				ilimit(&i1,0,nzgrid-1);
				
				ii = ivs[isort[iz]]*ncdp_velo
					+nvh[ivs[isort[iz]]];

				
				z = zgrid[it2];
				zt = zgrid[it1];
				va = vagrid[i2*nzgrid+it2];
				vat = vagrid[i2*nzgrid+it1];
				gt = dvdzgrid[i2*nzgrid+it2];

				vth[ii] = (z*va-zt*vat)/(z-zt) + 0.5*gt*(z-zt);

				z = zgrid[ib1];
				zb = zgrid[ib2];
				va = vagrid[i2*nzgrid+ib1];
				vab = vagrid[i2*nzgrid+ib2];
				gb = dvdzgrid[i2*nzgrid+ib2];

				vbh[ii] = (zb*vab-z*va)/(zb-z) - 0.5*gb*(zb-z);


				va = vagrid[i2*nzgrid+i1];
				g = dvdzgrid[i2*nzgrid+i1];


				vah[ii] = va;
				dvdzh[ii] = g;
				xvh[ii] = x;
				zvh[ii] = z;
				nvh[ivs[isort[iz]]] += 1;
				if(iz==0) {
					vth[ii] = va;
				} else if (iz==jh-1) {
					vbh[ii] = vth[ii];
				}

				/*
				fprintf(stderr,
			"vt=%6.0f vb=%6.0f va=%6.0f g=%6.2f x=%6.0f z=%6.0f \n",
					vth[ii],vbh[ii],vah[ii],dvdzh[ii],x,z);
				*/
			}
		}


		/* fing vt, bt, dvdz and va at pick positions */
		for(ih=0;ih<nhs;ih++) {
                       	vtopyes = 'n';
			for(jh=0;jh<maxhs;jh++) {
				if(hnums[ih]==vtopupdate[jh]) {
                        		vtopyes = 'y';
					break;
				}
			}
                       	vbotyes = 'n';
			for(jh=0;jh<maxhs;jh++) {
				if(hnums[ih]==vbotupdate[jh]) {
                        		vbotyes = 'y';
					break;
				}
			}

                        if(vtopyes=='n' && vbotyes=='n') continue;

			n2 = ih*maxpks;
			n1 = ih*ncdp_velo;
			nvs = nvh[ih];
			if(nvs==0) {
				fprintf(stderr, 
				"warning: no velan at horizon %d !\n",
				hnums[ih]); 
			} else if (nvs==1) {
				for(ip=0;ip<npicks[ih];ip++) {
					if(vtopyes=='y') veltops[ip+n2]=vth[n1];
                                	if(vbotyes=='y') velbots[ip+n2]=vbh[n1];
                                	velavgs[ip+n2] = vah[n1];
                                	dvdzs[ip+n2] = dvdzh[n1];
				}
			} else {
				for(ip=0;ip<npicks[ih];ip++) {
					x = xpicks[ip+ih*maxpks];
					bisear_(&nvs,&one,xvh+n1,&x,&i2);
					if(x<=xvh[n1]) {
						if(vtopyes=='y') 
						veltops[ip+n2] = vth[n1];
						if(vbotyes=='y')
                                		velbots[ip+n2] = vbh[n1];
                                		velavgs[ip+n2] = vah[n1];
                                		dvdzs[ip+n2] = dvdzh[n1];
					} else if(x>=xvh[n1+nvs-1]) {
						if(vtopyes=='y') 
						veltops[ip+n2] = vth[n1+nvs-1];
						if(vbotyes=='y')
                                		velbots[ip+n2] = vbh[n1+nvs-1];
                                		velavgs[ip+n2] = vah[n1+nvs-1];
                                		dvdzs[ip+n2] = dvdzh[n1+nvs-1];
					} else { 
						tmp = (x-xvh[n1+i2-1])/
						      (xvh[n1+i2]-xvh[n1+i2-1]);
						if(vtopyes=='y') 
						veltops[ip+n2] = vth[n1+i2-1] +
						  tmp*(vth[n1+i2]-vth[n1+i2-1]);
						if(vbotyes=='y')
						velbots[ip+n2] = vbh[n1+i2-1] +
						  tmp*(vbh[n1+i2]-vbh[n1+i2-1]);
                                		velavgs[ip+n2] = vah[n1+i2-1] +
						  tmp*(vah[n1+i2]-vah[n1+i2-1]);
                                		dvdzs[ip+n2] = dvdzh[n1+i2-1] +
						  tmp*
						  (dvdzh[n1+i2]-dvdzh[n1+i2-1]);
					}
				}
			}
		}

		free(indx);
		free(xnew);
		free(znew);
		free(vtnew);
		free(vbnew);
		free(vanew);
		free(gnew);

		free(zvs);
		free(ivs);
		free(isort);
		free(zsort);
		free(nvh);
		free(xvh);
		free(zvh);
		free(vbh);
		free(vth);
		free(vah);
		free(dvdzh);
		free(zgrid);
		free(vagrid);
		free(dvdzgrid);
		free(g1);
		free(g2);

		free(z2);
	}
}
