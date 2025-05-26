/* velocity grid to VELF card conversion */

#include "usgrid.h"
#include "comva.h"
#include "par.h"


char *sdoc = 
"VGRID2HFILE - convert interval velocity grid to HFILE  			\n"
"\n"
"vgrid2hfile oldhfile= [parameters] < vgrid  >hfile 				\n" 
"\n"
"Required parameters:						 	\n"
"vgrid=                Name of interval velocity grid 				\n"
"oldhfile=             Name of old hfile where (x,z) are specified \n"
"                      for all picked horizons							\n"
"hfile=                Name of output hfile 	\n"
"Optional parameters:					\n"
"xtol=0.                horizons extend, at both ends, a distance of xtol\n"
"                       to avoid discontinuity.                 \n"
"ism=1                  smoothing option; to laterally smooth interval  \n"
"                         velocities at top and bottom, along interface,\n"
"                         before output  (applied to updated horizons   \n"
"                         (only)                                        \n"
"                         0=average all values at picks along interface and \n"
"                           this single value is assigned to all output \n"
"                           points along the interface \n"
"                         1=no smoothing \n"
"                         n=smooting values at picks along interface using \n"
"                           n data points \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	12/1/95   		\n"
;

#define maxhs 128
#define maxpks 8192

void findbot(float xt, float *xpicks, float *zpicks, int *npicks, 
	float zmax, int maxpk, int nhs, float xtol, int ihtop, float *zb); 

void hfileout(FILE *hfilefp, float *xpicks, float *zpicks,
	float *veltop,float *velbot, int *dif,
	float *dens,float *qval, float *pois,
	int *npicks, int nhs, int ism, int maxpk,
	float xmin, float xmax, float zmin, float zmax,
	int cdpxmin, int cdpxmax, float dcdp, 
	int *hnums, char hnames[maxhs][80],
	char *dunits, char *zattrib);

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout,*hfoldfp;
		usghed usgh;

		/* hfile variables */
		int *npicks,*hnums,nhs,*difs,
			cdpxmini,cdpxmaxi,ihfile;
		float *xpicks,*zpicks,*veltops,*velbots,*velavgs,*dvdzs,
			*dens,*qval,*pois,
			xmini,xmaxi,zmini,zmaxi,dcdpi,vmini,vmaxi;
		char hnames[maxhs][80], dunitsi[80], zattribi[80], vtypei[80],
			*oldhfile;


		/* vgrid variables */
		float x0,z0,dx,dz,*vgrid;
		int nx,nz,ierr,ih,ip,jp;
		float xtol,xt,zt,zb,x,sx1,sx2,sz1,sz2,z;

	
		/* other variables */
		int ism, ix, iz;
		float vt, vt1, vt2, vb, vb1, vb2;


    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

		/* read in hfile format */
		if(!getparstring("oldhfile",&oldhfile)) err(" oldhfile missing");
		hfoldfp = efopen(oldhfile,"r");
    	nhs = 0;
		dcdpi = 0.;
		/* memory allocations */
		xpicks = (float *) emalloc(maxhs*maxpks*sizeof(float));
		zpicks = (float *) emalloc(maxhs*maxpks*sizeof(float));
		veltops = (float *) emalloc(maxhs*maxpks*sizeof(float));
		velbots = (float *) emalloc(maxhs*maxpks*sizeof(float));
		npicks = (int *) emalloc(maxhs*sizeof(int));
		hnums = (int *) emalloc(maxhs*sizeof(int));
		difs = (int *) emalloc(maxhs*maxpks*sizeof(int));
		velavgs = (float *) emalloc(maxhs*maxpks*sizeof(float));
		dvdzs = (float *) emalloc(maxhs*maxpks*sizeof(float));
		dens = (float *) emalloc(maxhs*maxpks*sizeof(float));
		qval = (float *) emalloc(maxhs*maxpks*sizeof(float));
		pois = (float *) emalloc(maxhs*maxpks*sizeof(float));

		fprintf(stderr,"before ifileread \n");

		ifileread(hfoldfp,&nhs,xpicks,zpicks,veltops,velbots,difs,
				  dens,qval,pois,velavgs,dvdzs,
				  (char*)hnames,hnums,npicks,maxhs,maxpks,
				  &xmini,&xmaxi,&zmini,&zmaxi,
				  &cdpxmini,&cdpxmaxi,&vmini,&vmaxi,
				  vtypei,dunitsi,zattribi,&dcdpi,&ihfile);

		fprintf(stderr,"after ifileread nhs=%d \n",nhs);

		/* get optional parameters */
		if(!getparint("ism",&ism)) ism = 1;
		if(!getparfloat("xtol",&xtol)) xtol = 0.;

		/* read in velocity grid */
		ierr = fgetusghdr(infp,&usgh);
		if(ierr!=0) err(" nonstandard grid file ");
		z0 = usgh.o1;
		dz = usgh.d1;
		nz = usgh.n1;
		x0 = usgh.o2;
		dx = usgh.d2;
		nx = usgh.n2;
		vgrid = (float*) emalloc(nx*nz*sizeof(float));
		efseek(infp,0,0);
		efread(vgrid,sizeof(float),nz*nx,infp);

		fprintf(stderr,"after vgrid read \n");

		/* update velocity in hfile */

		for (ih=0;ih<nhs;ih++) {
			fprintf(stderr," ih=%d npicks=%d \n",ih,npicks[ih]);
			for(ip=0;ip<npicks[ih];ip++) {
				jp = ip + ih * maxpks;
				xt = xpicks[jp];
				zt = zpicks[jp];
				findbot(xt,xpicks,zpicks,npicks,zmaxi,maxpks,nhs,xtol,ih,&zb);
				x = (xt-x0)/dx;
				ix = x;
				if(ix<0) {
					ix = 0;
					sx1 = 1.;
					sx2 = 0.;
				} else if(ix>=nx-1) {
					ix = nx-2;
					sx1 = 0.;
					sx2 = 1.;
				} else {
					sx2 = x - ix;
					sx1 = 1. - sx2;
				}

				z = (zt-z0)/dz;
				iz = z;
				if(iz<0) {
					iz = 0;
					sz1 = 1.;
					sz2 = 0.;
				} else if(iz>=nz-1) {
					iz = nz-2;
					sz1 = 0.;
					sz2 = 1.;
				} else {
					sz2 = z - iz;
					sz1 = 1. - sz2;
				}
			
				vt1 = vgrid[iz+ix*nz]*sx1 + vgrid[iz+(ix+1)*nz]*sx2;
				vt2 = vgrid[iz+1+ix*nz]*sx1 + vgrid[iz+1+(ix+1)*nz]*sx2;
				vt = vt1*sz1 + vt2*sz2;

				z = (zb-z0)/dz;
				iz = z;
				if(iz<0) {
					iz = 0;
					sz1 = 1.;
					sz2 = 0.;
				} else if(iz>=nz-1) {
					iz = nz-2;
					sz1 = 0.;
					sz2 = 1.;
				} else {
					sz2 = z - iz;
					sz1 = 1. - sz2;
				}
			
				vb1 = vgrid[iz+ix*nz]*sx1 + vgrid[iz+(ix+1)*nz]*sx2;
				vb2 = vgrid[iz+1+ix*nz]*sx1 + vgrid[iz+1+(ix+1)*nz]*sx2;
				vb = vb1*sz1 + vb2*sz2;

				veltops[jp] = vt;
				velbots[jp] = vb;

			}
		}

		fprintf(stderr, "before hfileout \n");


		/* output hfile */
		hfileout(outfp, xpicks, zpicks,
			veltops, velbots, difs,
			dens,qval,pois,
			npicks, nhs, ism, maxpks,
			xmini, xmaxi, zmini, zmaxi,
			cdpxmini, cdpxmaxi, dcdpi, 
			hnums, hnames,
			dunitsi, zattribi);

		return EXIT_SUCCESS;
}

/* find zb */ 
void findbot(float xt, float *xpicks, float *zpicks, int *npicks, 
float zmax, int maxpk, int nhs, float xtol, int ihtop, float *zb) {

	int ih, n1, n2, one=1;
	float xmin, xmax, z, zt;
	float *zxs, *zsort;
	int *ixs, *ixsave, *isort;
	int i1, i2, nxs, iz, ihbot;

	zxs = (float*) malloc(nhs*sizeof(float));
	ixs = (int*) malloc(nhs*sizeof(int));
	ixsave = (int*) malloc(nhs*sizeof(int));
	isort = (int*) malloc(nhs*sizeof(int));
	zsort = (float*) malloc(nhs*sizeof(float));

	/* find cross point at horizons */
	nxs = 0;
	for(ih=0;ih<nhs;ih++) {
		xmin = xpicks[ih*maxpk]-xtol;
		xmax = xpicks[ih*maxpk+npicks[ih]-1]+xtol;
		n1 = npicks[ih];
		n2 = ih * maxpk;
		if(xt>=xmin && xt<=xmax) {
			ixs[nxs] = ih;
			/* find z */
			bisear_(&n1,&one,xpicks+n2,&xt,&i1);
			if(xt<=xpicks[n2]) {
				z = zpicks[n2];
			} else if(xt>=xpicks[n1+n2-1]) {
				z = zpicks[n1+n2-1];
		   	} else {
			/* linear interpolation */
				z = zpicks[n2+i1-1] +
					(xt-xpicks[n2+i1-1])*(zpicks[n2+i1]-
					zpicks[n2+i1-1])/(xpicks[n2+i1]-
					xpicks[n2+i1-1]);
			}
			zxs[nxs] = z;
			nxs = nxs + 1;
		}
	}
	/* sort zxs into ascending order */
	for (iz=0;iz<nxs;iz++) {
		isort[iz] = iz;
		zsort[iz] = zxs[iz];
	}

	if(nxs>0) qkisort(nxs,zsort,isort);
	for(iz=0;iz<nxs;iz++) zxs[iz] = zsort[isort[iz]];
	for(iz=0;iz<nxs;iz++) ixsave[iz] = ixs[iz];
	for(iz=0;iz<nxs;iz++) ixs[iz] = ixsave[isort[iz]];

	ihbot = -1;
	for(iz=0;iz<nxs;iz++) {
		if(ihtop==ixs[iz]) {
		/*
			zt = zxs[iz]; 
		*/
			if(iz+1 < nxs) {
				ihbot = ixs[iz+1];
				*zb = zxs[iz+1];
				break;
			}		
		}
	}
	if(ihbot==-1) {
			*zb = zmax;
	}

	free(isort);
	free(ixs);
	free(ixsave);
	free(zxs);
	free(zsort);

/*
	fprintf(stderr,"ihtop=%d xt=%g zt=%g zb=%g ihbot=%d \n",
			ihtop+1,xt,zt,*zb,ihbot+1);
*/

}


void hfileout(FILE *hfilefp, float *xpicks, float *zpicks,
	float *veltop,float *velbot, int *dif,
	float *dens,float *qval, float *pois,
	int *npicks, int nhs, int ism, int maxpk, 
	float xmin, float xmax, float zmin, float zmax,
	int cdpxmin, int cdpxmax, float dcdp, 
	int *hnums, char hnames[maxhs][80],
	char *dunits, char *zattrib) {


	int ic,ip,np,ih,jp,nhout,h0; 
	int p1,p2,i,ismx,ismxh;
	float vmax, vmin, s;
	float *w, *f;
	float vt, vb, g;


	/* lateral smooth velocities */
	if(ism==0) {
	/* take average */ 
		for(ih=0;ih<nhs;ih++) {
			vt = 0.;
			vb = 0.;
			g = 0.;
			np = npicks[ih];
			for(ip=0;ip<np;ip++) {
				vt += veltop[ip+ih*maxpk];
			}
			if(np>0) vt = vt/np; 
			for(ip=0;ip<np;ip++) veltop[ip+ih*maxpk] = vt;

			for(ip=0;ip<np;ip++) {
				vb += velbot[ip+ih*maxpk];
			}
			if(np>0) vb = vb/np; 
			for(ip=0;ip<np;ip++) velbot[ip+ih*maxpk] = vb;
		}
	} else if (ism>1) {
	/* smoothing */
		for(ih=0;ih<nhs;ih++) {
			h0 = ih * maxpk;
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
			/* apply smoothing to veltop and velbot */
			ic = 1;
			smth1d_(veltop+h0,f,w,&np,&ismx,&ic);
			for(ip=0;ip<np;ip++) 
				veltop[ip+h0] = w[ip];
			smth1d_(velbot+h0,f,w,&np,&ismx,&ic);
			for(ip=0;ip<np;ip++) 
				velbot[ip+h0] = w[ip];

			free(w);
			free(f);
		}
	}

	fprintf(stderr," before min max velo \n");

	/* find maximum and minimum velocities */
	vmin = veltop[0];
	vmax = veltop[0];


	for (ih=0;ih<nhs;ih++) {
	fprintf(stderr," ih=%d  npicks=%d \n", ih, npicks[ih]);
		for(ip=0;ip<npicks[ih];ip++) {
			jp = ip + ih * maxpk;
			veltop[jp] += 0.5;
			velbot[jp] += 0.5;
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

	fprintf(stderr," before output picks \n");

	/* output picks */

	fprintf(hfilefp, 
	"HFILE   <--- FILE FORMAT DESCRIPTOR \n");
	fprintf(hfilefp,"\n");
	fprintf(hfilefp, "MODEL.X-MIN         %8.1f\n",xmin);
	fprintf(hfilefp, "MODEL.X-MAX         %8.1f\n",xmax);
	fprintf(hfilefp, "MODEL.Z-MIN         %8.1f \n",zmin);
	fprintf(hfilefp, "MODEL.Z-MAX         %8.1f \n",zmax);
	fprintf(hfilefp, "MODEL.CDP#-AT-X-MIN %6d\n",cdpxmin);
	fprintf(hfilefp, "MODEL.CDP#-AT-X-MAX %6d\n",cdpxmax);
	fprintf(hfilefp, "MODEL.MIN-VELOCITY  %8.1f\n",vmin); 
	fprintf(hfilefp, "MODEL.MAX-VELOCITY  %8.1f\n",vmax); 
	fprintf(hfilefp, "MODEL.#HORIZONS     %6d\n",nhout); 
	fprintf(hfilefp, "MODEL.VELOCITY-TYPE  INTERVAL\n"); 
	fprintf(hfilefp, "MODEL.DIST-UNITS     %s\n",dunits); 
	fprintf(hfilefp, "MODEL.Z-ATTRIBUTE    %s\n",zattrib); 
	fprintf(hfilefp, "MODEL.CDP-SPACING   %8.1f\n",dcdp); 
	fprintf(hfilefp, "\n"); 

	
	for(ih=0;ih<nhs;ih++) {

	fprintf(stderr," ih=%d  npicks=%d \n", ih, npicks[ih]);

		np = npicks[ih];
		if(np<=1) continue;

		fprintf(hfilefp, "\n"); 
		fprintf(hfilefp, "\n"); 
		fprintf(hfilefp, "HORIZON-NUMBER %3d\n", hnums[ih]); 
		fprintf(hfilefp, "HORIZON-NAME     %s\n", hnames[ih]); 
		fprintf(hfilefp, "* \n"); 
		fprintf(hfilefp, 
"* X-VALUE   Z-VALUE   VELTOP   VELBOT  DIF  DENS  QVAL  POIS \n");
		fprintf(hfilefp, 
"* =======  ========  =======  =======  ===  ====  ====  ==== \n");

		h0 = ih*maxpk;
		
		for (ip=0;ip<npicks[ih];ip++) {

   			fprintf(hfilefp, 
"%9.1f %9.1f %8.0f %8.0f   %1d   %4.2f  %4.0f  %4.2f \n",
				xpicks[ip+h0],zpicks[ip+h0],
				veltop[ip+h0],velbot[ip+h0],dif[ip+h0],
				dens[ip+h0],qval[ip+h0],pois[ip+h0]);
		}
	}
}
