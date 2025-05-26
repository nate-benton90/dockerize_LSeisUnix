#include "gridhd.h"
#include "comva.h"
#include "par.h"
#include "xplot.h"
#include <X11/Xatom.h>
#include <X11/keysym.h>

/* self-documentation */
char *sdoc = 
"VIPICK - Velocity pick of uniformly-sampled VELAN function f(x1,x2)\n"
"\n"
"vipick n1= [optional parameters] <binaryfile\n"
"\n"
"See vpick for Documentation \n"
"\n"
" author: Zhiming Li                           10/3/91                 \n"
"\n";


/* define maximum number of interfaces and maximum number of picks per
	interface in IFILE				*/
#define maxhs 128
#define maxpks 4096


/* functions defined and used internally */
static void zoomBox (int x, int y, int w, int h, 
	int xb, int yb, int wb, int hb,
	int nx, int ix, float x1, float x2,
	int ny, int iy, float y1, float y2,
	int *nxb, int *ixb, float *x1b, float *x2b,
	int *nyb, int *iby, float *y1b, float *y2b);
static unsigned char *newInterpBytes (int n1in, int n2in, unsigned char *bin,
	int n1out, int n2out);
void xMouseLoc(Display *dpy, Window win, XEvent event, int style, Bool show,
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb);
void xMousePrint(Display *dpy, Window win, XEvent event, int style,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	char *pcard, int ppos, float *zp, float *vp, int np, int fex,
	float *ztop, float *zbot, float *dvdz, int ia, int ivof);
void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char *vpickcolor, char *vrefcolor, 
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *zp, float *vp, int *np, float *zr, float *vr, int nr, 
	int pkey, GC gc, int *savebg, int ivmode,
	float *zptop, float *zpbot,
	float *ztop, float *zbot, float *v0, float *dvdz,
	float *vatop, float *vabot, int *ia,
	float *zpscrd, float *vpscrd, int npcrd, char *vpcrdcolor,
	float *zifile, float *vifile, int nzifile, int ivof);
void viplot(Display *dpy, Window win, char *vicolor,  
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *zp, float *vp, int np, float *zr, int nr, int ivmode,
	float *ztop, float *zbot, float *v0, float *dvdz,
	float *vatop, float *vabot, int ia, int ivof);


static Display *dpy;
static Window win;

main (argc,argv)
int argc; char **argv;
{
	int n1,n2,n1tic,n2tic,nfloats,
		i1,i2,grid1,grid2,style,
		n1c,n2c,i1beg,i1end,i2beg,i2end,i1c,i2c,
		nz,iz,i1step,i2step,verbose,
		xbox,ybox,wbox,hbox,
		xb,yb,wb,hb,
		xxb,yyb,wwb,hhb,
		x,y,width,height,
		i,j,nx,ny,nxb,nyb,ixb,iyb,
		imageOutOfDate,winwidth=-1,winheight=-1,
		showloc=0,ppos,np=0,nr=0,pkey,fex, ivgtype,
		nzvg, indxvg, savebg, nxvg, ivmode, ia,
		cdppre,icdppre,ii,jj,ih,ihfile,
		nhs, *npicks, *hnums, *difs,
		cdpxmin,cdpxmax,nzifile,indx,one,i2min,i2max, ivof,
		*cdpscrd, *npscrd, maxp=2048, maxcrds=8192, ncdpscrd=0, *isort;
	float labelsize,titlesize,perc,clip,bperc,wperc,bclip,wclip,
		d1,f1,d2,f2,*z,*temp,zscale,zoffset,zi,
		x1beg,x1end,x2beg,x2end,
		x1min,x1max,x2min,x2max,
		d1num,f1num,d2num,f2num,
		x1begb,x1endb,x2begb,x2endb,
		*zp,*vp,*zr,*vr, fzvg, dzvg, tmp, xcdp, 
		*zptop, *zpbot,
		*ztop, *zbot, *v0, *dvdz, *vatop, *vabot,
	  	*xpicks,*zpicks,*veltops,*velbots,
	  	*dens,*qval,*pois,
                *velavgs,*dvdzs,avplo,avphi,
		xmin,xmax,zmin,zmax,vmin,vmax,dcdp,
		fcdpvg, dcdpvg, 
		*zpscrd, *vpscrd, *zifile, *vifile, *zi2, zi21, zi2n;
	unsigned char *cz,*czp,*czb,*czbp,*czbi=NULL;
	char *label1="depth (or time)",*label2="velocity",
		title[80],*ttl,
		*labelfont="Erg14",*titlefont="Rom22",
		*styles="seismic",*grid1s="none",*grid2s="none",
		*labelcolor="blue",*titlecolor="red",
		*gridcolor="blue",*cmap="rgb",keybuf[256],*mpicks,
		*vrefcolor="green",*vpickcolor="red",*vpcrdcolor="purple",
		*ifile, hnames[maxhs][80],
		vtype[80],dunits[80],zattrib[80],
		*pcard="VELO", *vgfile="NONE", strppos[5];
	FILE *infp=stdin, *mpicksfp, *vgfp, *ifilefp;

	int ierr;
	ghed gh;
/*
	Display *dpy;
	Window win;
*/
	XEvent event;
	KeySym keysym;
	XComposeStatus keystat;
	XImage *image=NULL;
	GC gci;
	int scr;
	unsigned long black,white,pmin,pmax;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters describing 1st dimension sampling */
	if (!getparint("n1",&n1))
		err("Must specify number of samples in 1st dimension!\n");
	d1 = 1.0;  getparfloat("d1",&d1);
	f1 = d1;  getparfloat("f1",&f1);
	x1min = (d1>0.0)?f1:f1+(n1-1)*d1;
	x1max = (d1<0.0)?f1:f1+(n1-1)*d1;

	/* get parameters describing 2nd dimension sampling */
	if (!getparint("n2",&n2)) {
		if (fseek(infp,0L,2)!=0)
			err("must specify n2 if in a pipe!\n");
		nfloats = eftell(infp)/sizeof(float);
		efseek(infp,0L,0);
		n2 = nfloats/n1;
	}
	d2 = 1.0;  getparfloat("d2",&d2);
	f2 = d2;  getparfloat("f2",&f2);
	x2min = (d2>0.0)?f2:f2+(n2-1)*d2;
	x2max = (d2<0.0)?f2:f2+(n2-1)*d2;

	/* set up file to save mouse picks */

	zpscrd = (float*) malloc(maxp*maxcrds*sizeof(float));
	vpscrd = (float*) malloc(maxp*maxcrds*sizeof(float));
	npscrd = (int*) malloc(maxcrds*sizeof(int));
	cdpscrd = (int*) malloc(maxcrds*sizeof(int));
	for(i1=0;i1<maxcrds;i1++) npscrd[i1]=0;

        if(!getparint("ivof",&ivof)) ivof=0;

	if (!getparstring("mpicks", &mpicks)) {
		mpicksfp = stderr;
		fex = 0;
		ncdpscrd=0;
	} else {
		/* if file exist */
		if((mpicksfp = fopen(mpicks,"r"))!=NULL) {
			if(ivof==0) {
				veloread(mpicksfp, cdpscrd, zpscrd, 
				vpscrd, &ncdpscrd, npscrd, maxp, maxcrds);
			} else {
				/* read in velf cards */
				/*
				velfread(mpicksfp, cdpscrd, zpscrd, 
				vpscrd, &ncdpscrd, npscrd, maxp, maxcrds);
				*/
				/* read in hanvel cards */
				hvelread(mpicksfp, cdpscrd, zpscrd, 
				vpscrd, &ncdpscrd, npscrd, maxp, maxcrds);

			}

			fseek(mpicksfp,0,SEEK_END);
			if(ftell(mpicksfp) == 0) {
				fex = 0;
			} else {
				fex = 1;
			}
			fclose(mpicksfp);
			mpicksfp = efopen(mpicks,"a");
		} else {
			mpicksfp = efopen(mpicks,"w");
			fex = 0;
		}
	}

	if (!getparint("ppos", &ppos)) ppos = 1;
	/* maxpks maximum */
	zp = (float*) malloc(maxpks*sizeof(float));
	vp = (float*) malloc(maxpks*sizeof(float));
	zptop = (float*) malloc(maxpks*sizeof(float));
	zpbot = (float*) malloc(maxpks*sizeof(float));
	ztop = (float*) malloc(maxpks*sizeof(float));
	zbot = (float*) malloc(maxpks*sizeof(float));
	v0 = (float*) malloc(maxpks*sizeof(float));
	dvdz = (float*) malloc(maxpks*sizeof(float));
	vatop = (float*) malloc(maxpks*sizeof(float));
	vabot = (float*) malloc(maxpks*sizeof(float));

	zifile = (float*) malloc(maxpks*sizeof(float));
	vifile = (float*) malloc(maxpks*sizeof(float));
	nzifile = 0;

	/*read in ifile if present */
	if (getparstring("ifile", &ifile)) {
		ifilefp = fopen(ifile,"r");
		xpicks = (float *) malloc(maxhs*maxpks*sizeof(float));
        	zpicks = (float *) malloc(maxhs*maxpks*sizeof(float));
        	veltops = (float *) malloc(maxhs*maxpks*sizeof(float));
        	velbots = (float *) malloc(maxhs*maxpks*sizeof(float));
        	npicks = (int *) malloc(maxhs*sizeof(int));
        	hnums = (int *) malloc(maxhs*sizeof(int));
        	difs = (int *) malloc(maxhs*maxpks*sizeof(int));
        	velavgs = (float *) malloc(maxhs*maxpks*sizeof(float));
        	dvdzs = (float *) malloc(maxhs*maxpks*sizeof(float));
        	qval = (float *) malloc(maxhs*maxpks*sizeof(float));
		dens = (float*) malloc(maxhs*maxpks*sizeof(float));
        	pois = (float *) malloc(maxhs*maxpks*sizeof(float));
        	for(ih=0;ih<maxhs;ih++) npicks[ih]=0;
        	for(ih=0;ih<maxhs*maxpks;ih++) {
                	veltops[ih]=0.;
                	velbots[ih]=0.;
                	difs[ih] = 0;
                	velavgs[ih] = 0.;
                	dvdzs[ih] = 0.;
        	}
		nhs = 0;
		ifileread(ifilefp,&nhs,xpicks,zpicks,veltops,velbots,difs,
			  dens,qval,pois,
                          velavgs,dvdzs,
			  (char*)hnames,hnums,npicks,maxhs,maxpks,
			  &xmin,&xmax,&zmin,&zmax,
                          &cdpxmin,&cdpxmax,&vmin,&vmax,
                          vtype,dunits,zattrib,&dcdp,&ihfile);

		/*
		fprintf(stderr,"xmin=%f\n",xmin);
        	fprintf(stderr,"xmax=%f\n",xmax);
        	fprintf(stderr,"zmin=%f\n",zmin);
       	 	fprintf(stderr,"zmax=%f\n",zmax);
        	fprintf(stderr,"cdpxmin=%d\n",cdpxmin);
        	fprintf(stderr,"cdpxmax=%d\n",cdpxmax);
        	fprintf(stderr,"vmin=%f\n",vmin);
        	fprintf(stderr,"vmax=%f\n",vmax);
        	fprintf(stderr,"vtype=%s\n",vtype);
       	 	fprintf(stderr,"dunits=%s\n",dunits);
        	fprintf(stderr,"zattrib=%s\n",zattrib);
        	fprintf(stderr,"dcdp=%f\n",dcdp);
		*/

		/* determine x location of cdp */ 
		if(nhs>0) {
			xcdp=xmin+(ppos-cdpxmin)*(xmax-xmin)/(cdpxmax-cdpxmin);
		

		/* determine z positions of picks from ifile input */
			nzifile = 0;
			one = 1;
			zi2 = (float *) malloc(maxpks*sizeof(float));
			isort = (int *) malloc(maxpks*sizeof(int));

			for(ih=0;ih<nhs;ih++) {

			   if(xpicks[ih*maxpks]<=xcdp && npicks[ih]>1 && 
			      xcdp<=xpicks[ih*maxpks+npicks[ih]-1]) {
			/*
			      if(npicks[ih]==2) {
			*/
				 bisear_(&npicks[ih],&one,
                                       xpicks+ih*maxpks,&xcdp,&indx);
				 zifile[nzifile] = zpicks[ih*maxpks+indx-1] +
                                                (xcdp-
                                                xpicks[ih*maxpks+indx-1]) *
                                                (zpicks[ih*maxpks+indx]-
                                                zpicks[ih*maxpks+indx-1])/
                                                (xpicks[ih*maxpks+indx]-
                                                xpicks[ih*maxpks+indx-1]);
				/* not a good idea to use spline !!!! */
			/*
			      } else {
			         zi21 = 0.;
			         zi2n = 0.;
			         spline_(xpicks+ih*maxpks,zpicks+ih*maxpks,
			  	         &npicks[ih],&zi21,&zi2n,zi2);	
			         splint_(xpicks+ih*maxpks,zpicks+ih*maxpks,zi2,
				         &npicks[ih],&xcdp,&zifile[nzifile]);
		              }
			*/
			      nzifile = nzifile + 1;
			   }
			}
			/* sort zifile to ascending order */
			for(iz=0;iz<nzifile;iz++) {
				isort[iz] = iz;
				zi2[iz] = zifile[iz];
			}
			if(nzifile>0) qkisort(nzifile,zifile,isort);
			for(iz=0;iz<nzifile;iz++) 
				zifile[iz] = zi2[isort[iz]];
			free(isort);
			free(zi2);
		}

        }

	/* read binary data to be plotted */
	nz = n1*n2;
	z = ealloc1float(nz);
	if (fread(z,sizeof(float),nz,infp)!=nz)
		err("error reading input file");


	/* if necessary, determine clips from percentiles */
	if (getparfloat("clip",&clip)) {
		bclip = clip;
		wclip = -clip;
	}
	if ((!getparfloat("bclip",&bclip) || !getparfloat("wclip",&wclip)) &&
		!getparfloat("clip",&clip)) {
		perc = 100.0;  getparfloat("perc",&perc);
		temp = ealloc1float(nz);
		for (iz=0; iz<nz; iz++)
			temp[iz] = z[iz];
		if (!getparfloat("bclip",&bclip)) {
			bperc = perc;	getparfloat("bperc",&bperc);
			iz = (nz*bperc/100.0);
			if (iz<0) iz = 0;
			if (iz>nz-1) iz = nz-1;
			qkfind(iz,nz,temp);
			bclip = temp[iz];
		}
		if (!getparfloat("wclip",&wclip)) {
			wperc = 100.0-perc;  getparfloat("wperc",&wperc);
			iz = (nz*wperc/100.0);
			if (iz<0) iz = 0;
			if (iz>nz-1) iz = nz-1;
			qkfind(iz,nz,temp);
			wclip = temp[iz];
		}
		free1float(temp);
	}
	verbose = 1;  getparint("verbose",&verbose);
	if (verbose) warn("bclip=%g wclip=%g",bclip,wclip);

	/* get colormap specification */
	getparstring("cmap",&cmap);

	/* get axes parameters */
	xbox = 50; getparint("xbox",&xbox);
	ybox = 50; getparint("ybox",&ybox);
	wbox = 700; getparint("wbox",&wbox);
	hbox = 800; getparint("hbox",&hbox);
	x1beg = x1min; getparfloat("x1beg",&x1beg);
	x1end = x1max; getparfloat("x1end",&x1end);
	d1num = 0.0; getparfloat("d1num",&d1num);
	f1num = x1min; getparfloat("f1num",&f1num);
	n1tic = 1; getparint("n1tic",&n1tic);
	getparstring("grid1",&grid1s);
	if (STREQ("dot",grid1s)) grid1 = DOT;
	else if (STREQ("dash",grid1s)) grid1 = DASH;
	else if (STREQ("solid",grid1s)) grid1 = SOLID;
	else grid1 = NONE;
	getparstring("label1",&label1);
	x2beg = x2min; getparfloat("x2beg",&x2beg);
	x2end = x2max; getparfloat("x2end",&x2end);
	d2num = 0.0; getparfloat("d2num",&d2num);
	f2num = 0.0; getparfloat("f2num",&f2num);
	n2tic = 1; getparint("n2tic",&n2tic);
	getparstring("grid2",&grid2s);
	if (STREQ("dot",grid2s)) grid2 = DOT;
	else if (STREQ("dash",grid2s)) grid2 = DASH;
	else if (STREQ("solid",grid2s)) grid2 = SOLID;
	else grid2 = NONE;
	getparstring("label2",&label2);
	getparstring("labelfont",&labelfont);
	labelsize = 18.0; getparfloat("labelsize",&labelsize);

	if(!getparstring("title",&ttl)) {
		ttl = (char*) malloc(30*sizeof(char));
		strcpy(ttl,"Velocity Picking ");
	}
	sprintf(title,"%s at position = %5d \0",ttl,ppos);

	getparstring("titlefont",&titlefont);
	titlesize = 24.0; getparfloat("titlesize",&titlesize);
	getparstring("style",&styles);
	if (STREQ("normal",styles)) style = NORMAL;
	else style = SEISMIC;
	getparstring("titlecolor",&titlecolor);
	getparstring("labelcolor",&labelcolor);
	getparstring("gridcolor",&gridcolor);
	getparstring("vpickcolor",&vpickcolor);
        getparstring("vrefcolor",&vrefcolor);
        getparstring("vpcrdcolor",&vpcrdcolor);
        if(!getparint("ivmode",&ivmode)) ivmode=0;


	/* determine previous pick position*/
        if(!getparint("cdppre",&cdppre)) cdppre=ppos;
	if(cdppre==0) cdppre = ppos;


	icdppre = 0;
	for(ii=0;ii<ncdpscrd;ii++) {
		if( cdpscrd[ii] == cdppre ) {
			icdppre = ii + 1;
			break;
		}
	}
	if(icdppre == 0 && ncdpscrd>0 ) {
		icdppre = 0;
		jj = abs( cdppre - cdpscrd[0] );
		for(ii=1;ii<ncdpscrd;ii++) {
			if( jj>abs(cdppre-cdpscrd[ii]) ) {
				jj = abs(cdppre-cdpscrd[ii]);
				icdppre = ii;
			}
		}
		icdppre = icdppre + 1;
	}
	if(icdppre==0) icdppre = 1;

	/* read in reference velocity curve if any */
        if(getparstring("vgfile",&vgfile)) {
		vgfp = fopen(vgfile,"r");
		ierr = fgetghdr(vgfp,&gh);
		if(!getparfloat("fcdpvg",&fcdpvg)) {
			if(ierr==0) {
				getgval(&gh,"ocdp2",&fcdpvg);
			} else {
				fcdpvg=1;
			}
		}
		if(!getparfloat("dcdpvg",&dcdpvg)) {
			if(ierr==0) {	
				getgval(&gh,"dcdp2",&dcdpvg);
			} else {
				dcdpvg=1;
			}
		}
		if(!getparint("nzvg",&nzvg)) {
			if(ierr==0) {
				nzvg = (int) (float)(gh.n1/gh.scale+0.5);
			} else {
				err(" nzvg must be specified ");
			}
		}
		if(!getparfloat("fzvg",&fzvg)) {
			if(ierr==0) {
				getgval(&gh,"o1",&fzvg);
			} else {
				fzvg=0.;
			}
		}
		if(!getparfloat("dzvg",&dzvg)) {
			if(ierr==0) {
				getgval(&gh,"d1",&dzvg);
			} else {
				err(" dzvg must be specified ");
			}
		}


		if(!getparint("ivgtype",&ivgtype)) ivgtype = 0;
		
		if(ierr==0) {
			nxvg = (int) (float) (gh.n2/gh.scale + 0.5);
		} else {
			fseek(vgfp,0L,2);
			nxvg = ftell(vgfp)/(nzvg*sizeof(float));
		}

	        tmp =  (ppos-fcdpvg)/dcdpvg; 	
		indxvg  = tmp;
		
		if(indxvg<0) {
			indxvg = 0;
		} else if ( indxvg >= nxvg ) {
			indxvg = nxvg-1;
		} 

printf("fcdpvg=%g dcdpvg=%g cdp=%d indxvg=%d \n",fcdpvg,dcdpvg,ppos,indxvg+1);

		zr = (float*) malloc(nzvg*sizeof(float));
		vr = (float*) malloc(nzvg*sizeof(float));

		fseek(vgfp,indxvg*nzvg*sizeof(float),0);
		fread(vr,sizeof(float),nzvg,vgfp);

		/* convert interval velocity to average velocity */
		if(ivgtype==0) {
			zr[0] = vr[0] * fzvg;
			for(iz=1;iz<nzvg;iz++) {
				zr[iz]  = zr[iz-1] + vr[iz] * dzvg;
			}
			for(iz=1;iz<nzvg;iz++) vr[iz] = zr[iz]/(fzvg+iz*dzvg);
		}

		/* compute depth of reference velocity input */
		for(iz=0;iz<nzvg;iz++) zr[iz] = fzvg + iz*dzvg;
		nr = nzvg;
	} else {
		zr = (float*) malloc(1*sizeof(float));
		vr = (float*) malloc(1*sizeof(float));
	        nr=0;	
	}

	/* determine the vifile (automatic picks) from velan */
	if (nzifile>0) {
		if(!getparfloat("avplo",&avplo)) avplo=80.;
		if(!getparfloat("avphi",&avphi)) avphi=120.;

		for(ih=0;ih<nzifile;ih++) {

			if(nr==0) {
				i2min = 0; 
				i2max = n2-1;	
				vifile[ih] = vmin;
			} else {
				
				tmp = (zifile[ih]-fzvg)/dzvg;
				indx = tmp;
				if(indx<0) {
					indx=0;
				} else if (indx>nzvg-1) {
					indx = nzvg-1;
				}
				vifile[ih] = vr[indx]; 

				tmp = vifile[ih] * avplo/100.;
				tmp = (tmp - f2)/d2; 
				i2min = tmp;
				if(i2min<0) {
					i2min=0;
				} else if (i2min>n2-1) {
					i2min = n2-1;
				}
				tmp = vifile[ih] * avphi/100.;
				tmp = (tmp - f2)/d2; 
				i2max = tmp;
				if(i2max<0) {
					i2max=0;
				} else if (i2max>n2-1) {
					i2max = n2-1;
				}
			}

			if(avplo!=avphi) {  
				tmp = (zifile[ih]-f1)/d1;
				indx = tmp;
				if (indx<0) indx=0;
				if (indx>n1) indx=n1-1;
				tmp = 0.;

				for(i2=i2min;i2<=i2max;i2++) {
					if(tmp<z[i2*n1+indx]) {
						tmp = z[i2*n1+indx];
						vifile[ih] = f2 + i2*d2;
					}
				}
			}
			if(vifile[ih]<vmin && vmin>0.) vifile[ih]=vmin;
			if(vifile[ih]>vmax && vmax>0.) vifile[ih]=vmax;
			zp[ih] = zifile[ih];
			vp[ih] = vifile[ih];
		}
		np = nzifile;
	}

	/* adjust x1beg and x1end to fall on sampled values */
	i1beg = NINT((x1beg-f1)/d1);
	i1beg = MAX(0,MIN(n1-1,i1beg));
	x1beg = f1+i1beg*d1;
	i1end = NINT((x1end-f1)/d1);
	i1end = MAX(0,MIN(n1-1,i1end));
	x1end = f1+i1end*d1;

	/* adjust x2beg and x2end to fall on sampled values */
	i2beg = NINT((x2beg-f2)/d2);
	i2beg = MAX(0,MIN(n2-1,i2beg));
	x2beg = f2+i2beg*d2;
	i2end = NINT((x2end-f2)/d2);
	i2end = MAX(0,MIN(n2-1,i2end));
	x2end = f2+i2end*d2;

	/* allocate space for image bytes */
	n1c = 1+abs(i1end-i1beg);
	n2c = 1+abs(i2end-i2beg);
	cz = ealloc1(n1c*n2c,sizeof(unsigned char));

	/* convert data to be imaged into signed characters */
	zscale = (wclip!=bclip)?255.0/(wclip-bclip):1.0e10;
	zoffset = -bclip*zscale;
	i1step = (i1end>i1beg)?1:-1;
	i2step = (i2end>i2beg)?1:-1;
	if (style==NORMAL) {
		for (i2c=0,i2=i2beg; i2c<n2c; i2c++,i2+=i2step) {
			czp = cz+n1c*n2c-(i2c+1)*n1c;
			for (i1c=0,i1=i1beg; i1c<n1c; i1c++,i1+=i1step) {
				zi = zoffset+z[i1+i2*n1]*zscale;
				if (zi<0.0) zi = 0.0;
				if (zi>255.0) zi = 255.0;
				*czp++ = (unsigned char)zi;
			}
		}
	} else {
		czp = cz;
		for (i1c=0,i1=i1beg; i1c<n1c; i1c++,i1+=i1step) {
			for (i2c=0,i2=i2beg; i2c<n2c; i2c++,i2+=i2step) {
				zi = zoffset+z[i1+i2*n1]*zscale;
				if (zi<0.0) zi = 0.0;
				if (zi>255.0) zi = 255.0;
				*czp++ = (unsigned char)zi;
			}
		}
	}
	free1float(z);
	
	/* initialize zoom box parameters */
	nxb = nx = (style==NORMAL ? n1c : n2c);
	nyb = ny = (style==NORMAL ? n2c : n1c);
	ixb = iyb = 0;
	czb = cz;
	x1begb = x1beg;  x1endb = x1end;
	x2begb = x2beg;  x2endb = x2end;

	/* connect to X server */
	if ((dpy=XOpenDisplay(NULL))==NULL)
		err("Cannot connect to display %s!\n",XDisplayName(NULL));
	scr = DefaultScreen(dpy);
	black = BlackPixel(dpy,scr);
	white = WhitePixel(dpy,scr);
	
	/* create window */
	win = xNewWindow(dpy,xbox,ybox,wbox,hbox,black,white,"vpick");

	/* if necessary, create private colormap with gray scale */
	if (STREQ(cmap,"gray")) {
		XSetWindowColormap(dpy,win,xCreateGrayColormap(dpy,win));
	} else if (STREQ(cmap,"hue")) {
		XSetWindowColormap(dpy,win,xCreateHueColormap(dpy,win));
	} else if (STREQ(cmap,"rgb")) {
		XSetWindowColormap(dpy,win,xCreateRGBColormap(dpy,win));
	}
	
	/* determine min and max pixels from standard colormap */
	pmin = xGetFirstPixel(dpy);
	pmax = xGetLastPixel(dpy);
		
	/* make GC for image */
	gci = XCreateGC(dpy,win,0,NULL);
	
	/* set normal event mask */
	XSelectInput(dpy,win,
		StructureNotifyMask |
		ExposureMask |
		KeyPressMask |
		PointerMotionMask |
		ButtonPressMask |
		ButtonReleaseMask |
		Button1MotionMask |
		Button2MotionMask);
	
	/* map window */
	XMapWindow(dpy,win);
					
	/* determine good size for axes box */
	xSizeAxesBox(dpy,win,
		labelfont,titlefont,style,
		&x,&y,&width,&height);
	
	/* clear the window */
	XClearWindow(dpy,win);
	
	/* note that image is out of date */
	imageOutOfDate = 1;
	savebg = 1;
	ia = 0;

	/* main event loop */
	while(True) {
		XNextEvent(dpy,&event);

		/* if window was resized */
		if (event.type==ConfigureNotify &&
			(event.xconfigure.width!=winwidth ||
			 event.xconfigure.height!=winheight)) {
			winwidth = event.xconfigure.width;
			winheight = event.xconfigure.height;
							
			/* determine good size for axes box */
			xSizeAxesBox(dpy,win,
				labelfont,titlefont,style,
				&x,&y,&width,&height);
			
			/* clear background */	
                        if(savebg) XSetWindowBackground(dpy,win,0L);

			/* clear the window */
			XClearWindow(dpy,win);
			
			/* note that image is out of date */
			imageOutOfDate = 1;

		/* else if window exposed */
		} else if (event.type==Expose) {
			
			/* clear all expose events from queue */
			while (XCheckTypedEvent(dpy,Expose,&event));
			
			/* if necessary, make new image */
			if (imageOutOfDate) {
				if (czbi!=NULL) free1(czbi);
				czbi = newInterpBytes(nxb,nyb,czb,
					width,height);
				if (image!=NULL) XDestroyImage(image);
				image = xNewImage(dpy,pmin,pmax,
					width,height,czbi);
				imageOutOfDate = 0;
			}

			/* clear background */	
                        if(savebg) XSetWindowBackground(dpy,win,0L);

			/* draw image (before axes so grid lines visible) */
			XPutImage(dpy,win,gci,image,0,0,x,y,
				image->width,image->height);
			
			/* draw axes on top of image */
			xDrawAxesBox(dpy,win,
				x,y,width,height,
				x1begb,x1endb,0.0,0.0,
				d1num,f1num,n1tic,grid1,label1,
				x2begb,x2endb,0.0,0.0,
				d2num,f2num,n2tic,grid2,label2,
				labelfont,title,titlefont,
				labelcolor,titlecolor,gridcolor,
				style);

		/* else if key down */
		} else if (event.type==KeyPress) {

			XLookupString(&event,keybuf,0,&keysym,
					&keystat);
			/* add, delete or insert picks */
			pkey =99;
			if (keysym==XK_a) {
				pkey = 0; 
			} else if ( keysym==XK_d ) {
				pkey = 1; 
			} else if ( keysym==XK_i ) {
				pkey = 2; 
			} else if (keysym==XK_p) {
				pkey = 3;
			} else if (keysym==XK_r) {
				pkey = 4;
			} else if (keysym==XK_A) {
				pkey = 5;
				/* track pointer and get boundary */
				xxb = xb;
				yyb = yb;
				wwb = wb;
				hhb = hb;
				xRubberBox(dpy,win,event,&xxb,&yyb,&wwb,&hhb);
				zptop[ia] = x1begb+(x1endb-x1begb)*
							(yyb-y)/height;
				zpbot[ia] = x1begb+(x1endb-x1begb)*
							(yyb+hhb-y)/height;
				ia = ia + 1;

			} else if (keysym==XK_D) {
				pkey = 6;
			}
			if (pkey<=6) {
				xMousePicks(dpy,win,event,style,
					vpickcolor,vrefcolor,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					zp,vp,&np,zr,vr,nr,pkey,gci,
					&savebg,ivmode,
					zptop,zpbot,
					ztop,zbot,v0,dvdz,vatop,vabot,&ia,
					zpscrd+(icdppre-1)*maxp,
					vpscrd+(icdppre-1)*maxp,
					npscrd[icdppre-1],vpcrdcolor,
					zifile,vifile,nzifile,ivof);
			} else if (keysym==XK_s) {
				xMousePrint(dpy,win,event,style,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					pcard,ppos,zp,vp,np,fex,
					ztop, zbot, dvdz, ia, ivof);
			} else if (keysym==XK_Q) {
			/* This is the exit from the event loop */
				break;
			} else {
				continue;
			}


		/* else if button down (1 == zoom, 2 == mouse tracking */
		} else if (event.type==ButtonPress) {
			/* if 1st button: zoom */
			if (event.xbutton.button==Button1) {
				savebg = 1;

				/* track pointer and get new box */
				xRubberBox(dpy,win,event,&xb,&yb,&wb,&hb);
			
				/* if new box has tiny width or height */
				if (wb<4 || hb<4) {
				
					/* reset box to initial values */
					x1begb = x1beg;
					x1endb = x1end;
					x2begb = x2beg;
					x2endb = x2end;
					nxb = nx;
					nyb = ny;
					ixb = iyb = 0;
					if (czb!=cz) free1(czb);
					czb = cz;
			
				/* else, if new box has non-zero width
				/* and height */
				} else {
			
					/* calculate new box parameters */
					if (style==NORMAL) {
					    zoomBox(x,y,width,height,
						    xb,yb,wb,hb,
						    nxb,ixb,x1begb,x1endb,
						    nyb,iyb,x2endb,x2begb,
						    &nxb,&ixb,&x1begb,&x1endb,
						    &nyb,&iyb,&x2endb,&x2begb);
					} else {
					    zoomBox(x,y,width,height,
						    xb,yb,wb,hb,
						    nxb,ixb,x2begb,x2endb,
						    nyb,iyb,x1begb,x1endb,
						    &nxb,&ixb,&x2begb,&x2endb,
						    &nyb,&iyb,&x1begb,&x1endb);
					}
			
					/* make new bytes in zoombox */
					if (czb!=cz) free1(czb);
					czb = ealloc1(nxb*nyb,
						sizeof(signed char));
					for (i=0,czbp=czb; i<nyb; i++) {
					    czp = cz+(iyb+i)*nx+ixb;
					    for (j=0; j<nxb; j++)
						    *czbp++ = *czp++; 
					}
				}
			
				/* clear background */	
                        	if(savebg) XSetWindowBackground(dpy,win,0L);

				/* clear area and force an expose event */
				XClearArea(dpy,win,0,0,0,0,True);
			
				/* note that image is out of date */
				imageOutOfDate = 1;
		
			/* else if 2nd button down: display mouse coords */
			} else if (event.xbutton.button==Button2) {

				showloc = 1;
				xMouseLoc(dpy,win,event,style,showloc,
					  x,y,width,height,x1begb,x1endb,
					  x2begb,x2endb);

			/* else if 3rd button down: track and pick */
			} else if (event.xbutton.button==Button3) {
				pkey = 0;
				xMousePicks(dpy,win,event,style,
					vpickcolor,vrefcolor,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					zp,vp,&np,zr,vr,nr,pkey,gci,
					&savebg,ivmode,
					zptop,zpbot,
					ztop,zbot,v0,dvdz,vatop,vabot,&ia,
					zpscrd+(icdppre-1)*maxp,
					vpscrd+(icdppre-1)*maxp,
					npscrd[icdppre-1],vpcrdcolor,
					zifile,vifile,nzifile,ivof);
			} else {
				continue;
			}

		/* else if pointer has moved */
		} else if (event.type==MotionNotify) {
			
			/* if button2 down, show mouse location */
			if (showloc)
				xMouseLoc(dpy,win,event,style,True,
					x,y,width,height,
					x1begb,x1endb,x2begb,x2endb);

		/* else if button2 released, stop tracking */
		} else if (event.type==ButtonRelease &&
			   event.xbutton.button==Button2) {
			showloc = 0;
		}

	} /* end of event loop */

	/* close connection to X server */
	XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}

/* update parameters associated with zoom box */
static void zoomBox (int x, int y, int w, int h, 
	int xb, int yb, int wb, int hb,
	int nx, int ix, float x1, float x2,
	int ny, int iy, float y1, float y2,
	int *nxb, int *ixb, float *x1b, float *x2b,
	int *nyb, int *iyb, float *y1b, float *y2b)
{
	/* if width and/or height of box are zero, just copy values */
	if (wb==0 || hb==0) {
		*nxb = nx; *ixb = ix; *x1b = x1; *x2b = x2;
		*nyb = ny; *iyb = iy; *y1b = y1; *y2b = y2;
		return;		
	} 
	
	/* clip box */
	if (xb<x) {
		wb -= x-xb;
		xb = x;
	}
	if (yb<y) {
		hb -= y-yb;
		yb = y;
	}
	if (xb+wb>x+w) wb = x-xb+w;
	if (yb+hb>y+h) hb = y-yb+h;
	
	/* determine number of samples in rubber box (at least 2) */
	*nxb = MAX(nx*wb/w,2);
	*nyb = MAX(ny*hb/h,2);
	
	/* determine indices of first samples in box */
	*ixb = ix+(xb-x)*(nx-1)/w;
	*ixb = MIN(*ixb,ix+nx-*nxb);
	*iyb = iy+(yb-y)*(ny-1)/h;
	*iyb = MIN(*iyb,iy+ny-*nyb);
	
	
	/* determine box limits to nearest samples */
	*x1b = x1+(*ixb-ix)*(x2-x1)/(nx-1);
	*x2b = x1+(*ixb+*nxb-1-ix)*(x2-x1)/(nx-1);
	*y1b = y1+(*iyb-iy)*(y2-y1)/(ny-1);
	*y2b = y1+(*iyb+*nyb-1-iy)*(y2-y1)/(ny-1);
}

/* return pointer to new interpolated array of bytes */
static unsigned char *newInterpBytes (int n1in, int n2in, unsigned char *bin,
	int n1out, int n2out)
{
	unsigned char *bout;
	float d1in,d2in,d1out,d2out,f1in,f2in,f1out,f2out;
	
	f1in = f2in = f1out = f2out = 0.0;
	d1in = d2in = 1.0;
	d1out = d1in*(float)(n1in-1)/(float)(n1out-1);
	d2out = d2in*(float)(n2in-1)/(float)(n2out-1);
	bout = ealloc1(n1out*n2out,sizeof(unsigned char));
	intl2b(n1in,d1in,f1in,n2in,d2in,f2in,bin,
		n1out,d1out,f1out,n2out,d2out,f2out,bout);
	return bout;
}
	
void xMouseLoc(Display *dpy, Window win, XEvent event, int style, Bool show,
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb)
{
	XGCValues values;
	static XFontStruct *fs=NULL;
	static XCharStruct overall;
	static GC gc;
	int dummy,xoffset=10,yoffset=10;
	float x1,x2;
	char string[256];

	/* if first time, get font attributes and make gc */
	if (fs==NULL) {
		fs = XLoadQueryFont(dpy,"fixed");
		gc = XCreateGC(dpy,win,0,&values);
		XSetFont(dpy,gc,fs->fid);
		overall.width = 1;
		overall.ascent = 1;
		overall.descent = 1;
	}

	/* erase previous string */
	XClearArea(dpy,win,xoffset,yoffset,
		overall.width,overall.ascent+overall.descent,False);

	/* if not showing, then return */
	if (!show) return;

	/* convert mouse location to (x1,x2) coordinates */
	if (style==NORMAL) {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.x-x)/width;
		x2 = x2endb+(x2begb-x2endb)*(event.xmotion.y-y)/height;
	} else {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.y-y)/height;
		x2 = x2begb+(x2endb-x2begb)*(event.xmotion.x-x)/width;
	}

	/* draw string indicating mouse location */
	sprintf(string,"(%0.4g,%0.4g)",x1,x2);
	XTextExtents(fs,string,strlen(string),&dummy,&dummy,&dummy,&overall);
	XDrawImageString(dpy,win,gc,xoffset,yoffset+overall.ascent,
		string,strlen(string));
}

void xMousePrint(Display *dpy, Window win, XEvent event, int style,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	char *pcard, int ppos, 
	float *zp, float *vp, int np, int fex,
	float *ztop, float *zbot, float *dvdz, int ia, int ivof)
{
	int ic,ip,npout; 
	int p1,p2;
	float p3;
	static int first=1;
	/* output picks */
	if(first==1 && fex==0 ) {
		first = 999;
		if(ivof==0) {
			fprintf(mpicksfp,
"1---5----11--15----21----27----33----39----45----51----57----63----69----75\n");
			fprintf(mpicksfp,
"NAME    cdp        z1    v1    z2    v2    z3    v3    z4    v4    z5    v5\n");
		} else {
			fprintf(mpicksfp,
"1---5----11--15---20---25---30---35---40---45---50---55---60---65---70---75\n");
			fprintf(mpicksfp,
"NAME        cdp        t1   v1   t2   v2   t3   v3   t4   v4   t5   v5     \n");
		}
		if(ia>0) {
			fprintf(mpicksfp,
"NAME    cdp       zt1   zb1 dvdz1   zt2   zb2 dvdz2   zt3   zb3 dvdz3    \n");

		}
	
		fprintf(mpicksfp,"\n");
	}

	if(ivof==0) {
		for (ic=0;ic<(np+4)/5;ic++) {
			if( (ic+1)*5 < np ) {
	      			npout = 5;  
	   		} else { 
	      			npout = np - ic*5;
	   		}
   			fprintf(mpicksfp, "%-5s%6d    ",pcard,ppos);
   			for(ip=0;ip<npout;ip++) {
         			p1 = zp[ic*5+ip];	
         			p2 = vp[ic*5+ip];	
         			fprintf(mpicksfp,"%6d%6d",p1,p2);
   			}
			/* output VELF cards */
			/*
	   		fprintf(mpicksfp, "VELF %10d     ",ppos);
	   		for(ip=0;ip<npout;ip++) {
	         		p1 = zp[ic*5+ip];	
	         		p2 = vp[ic*5+ip];	
	         		fprintf(mpicksfp,"%5d%5d",p1,p2);
	   		}
			*/
	   		fprintf(mpicksfp,"\n");
		}
	} else {
		/* output HANVEL cards */ 
			printhvel(ppos,np,zp,vp,mpicksfp);
	}
	for (ic=0;ic<(ia+2)/3;ic++) {
	   if( (ic+1)*3 < ia ) {
	      npout = 3;  
	   }
	   else { 
	      npout = ia - ic*3;
	   }
	   fprintf(mpicksfp, "DVDZ %6d    ",ppos);
	   for(ip=0;ip<npout;ip++) {
	         p1 = ztop[ic*3+ip];	
	         p2 = zbot[ic*3+ip];	
	         p3 = dvdz[ic*3+ip];	
	         fprintf(mpicksfp,"%6d%6d%6.3f",p1,p2,p3);
	   }
	   fprintf(mpicksfp,"\n");
	}
	fflush(mpicksfp);
}

void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char *vpickcolor, char *vrefcolor,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *zp, float *vp, int *np, float *zr, float *vr, int nr, 
	int pkey, GC gc, int *savebg, int ivmode,
	float *zptop, float *zpbot,  
	float *ztop, float *zbot, float *v0, float *dvdz, 
	float *vatop, float *vabot, int *ia,
	float *zpcrd, float *vpcrd, int npcrd, char *vpcrdcolor,
	float *zifile, float *vifile, int nzifile, int ivof)
{
	float x1,x2;
	int ipicks, ppkey, ibfit=0, nn;
	int xx1,yy1,xx2,yy2,ip,ni;
	float dismin,dis,temp;
	int ipmin, ipins;
	static int fs=1;
	float *vaa, *zz, v00, aa;
	float v1, z1, v2, z2;
	
	
	GC gcvp, gcvr, gcvir, gcvip, gcvpcrd;
        XGCValues *values;
        XColor scolor,ecolor;
        XWindowAttributes wa;
        Colormap cmap;
        int scr;
	float *vi, *va, *zi;
	int *indx;
	int xw, xh;

	

	/* first time, save bitmap of window to background for retrival */ 
	if(*savebg) {
		fg2bg(dpy,win);
		*savebg = 0;
	}

	/* convert mouse location to (x1,x2) coordinates */
	if (style==NORMAL) {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.x-x)/width;
		x2 = x2endb+(x2begb-x2endb)*(event.xmotion.y-y)/height;
	} else {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.y-y)/height;
		x2 = x2begb+(x2endb-x2begb)*(event.xmotion.x-x)/width;
	}

	ppkey = pkey;
	if ( pkey == 2 && *np == 0 ) ppkey=0;

	/* save x1 and x2 */
	if(ppkey==0) {
		ipicks = *np;
		zp[ipicks] = x1;
		vp[ipicks] = x2;
		*np = *np + 1;
	/* delete nearest picks from x1 and x2 picks*/
	} else if ( ppkey==1 ) {
		ipicks = *np;	
		dismin = (x1-zp[0])*(x1-zp[0])+
		       	    (x2-vp[0])*(x2-vp[0]);
		ipmin = 0;
		for(ip=1;ip<ipicks;ip++) {
			dis=(x1-zp[ip])*(x1-zp[ip])+
		       	    (x2-vp[ip])*(x2-vp[ip]);
			if(dis<dismin) {
				dismin = dis;
				ipmin = ip;
			}
		}
		ipicks = ipicks - 1;
		for(ip=ipmin;ip<ipicks;ip++) {
			zp[ip] = zp[ip+1];
			vp[ip] = vp[ip+1];
		}
		*np = ipicks;
	/* insert the pick into x1 and x2 picks */
	} else if ( ppkey==2 ) {
		ipicks = *np;	
		dismin=(x1-zp[0])*(x1-zp[0]);
		ipmin = 0;
		for(ip=1;ip<ipicks;ip++) {
			dis=(x1-zp[ip])*(x1-zp[ip]);
			if(dis<dismin) {
				dismin = dis;
				ipmin = ip;
			}
		}
		if(x1>zp[ipmin]) { 
			ipins = ipmin+1;
			ipicks +=1;
			for(ip=ipicks;ip>ipins;ip--) {
				zp[ip] = zp[ip-1];
				vp[ip] = vp[ip-1];
			}
			zp[ipins] = x1;
			vp[ipins] = x2;
		} else if(x1<zp[ipmin]) {
			ipins = ipmin;
			ipicks +=1;
			for(ip=ipicks;ip>ipins;ip--) {
				zp[ip] = zp[ip-1];
				vp[ip] = vp[ip-1];
			}	
			zp[ipins] = x1;
			vp[ipins] = x2;
		}
		*np = ipicks;
	}
	/* draw lines between picks */
       	XClearWindow(dpy, win);

	if (*np > 1 || nr > 1 || npcrd > 1) {
                /* get screen */
                scr = DefaultScreen(dpy);
                /* determine window's current colormap */
                XGetWindowAttributes(dpy,win,&wa);
                cmap = wa.colormap;
        }

	if(*np>1) {
		/* create graphics contexts */
                gcvp = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,vpickcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcvp,ecolor.pixel);
                else
                        XSetForeground(dpy,gcvp,1L);
                XSetLineAttributes(dpy,gcvp,2,LineSolid,CapButt,JoinMiter);

                gcvip = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,vpickcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcvip,ecolor.pixel);
                else
                        XSetForeground(dpy,gcvip,1L);
                XSetLineAttributes(dpy,gcvip,1,LineSolid,CapButt,JoinMiter);
		xw = width /25;
		xh = height /25;

		/* draw zifile from ifile input */
		if(nzifile>0) {
			yy1=(zifile[0]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=x;
			yy2=yy1;
			xx2=xx1+width;
			XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);
			for(ip=1;ip<nzifile;ip++) {
			       yy1=(zifile[ip]-x1begb)/(x1endb-x1begb)*height+y;
			       xx1=x;
			       yy2=yy1;
			       xx2=xx1+width;
			       XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);
			}
		}

		
		yy1=(zp[0]-x1begb)/(x1endb-x1begb)*height+y-xh/2;
		xx1=(vp[0]-x2begb)/(x2endb-x2begb)*width+x;
		yy2=yy1+xh;
		xx2=xx1;
		XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);
		yy1=(zp[0]-x1begb)/(x1endb-x1begb)*height+y;
		xx1=(vp[0]-x2begb)/(x2endb-x2begb)*width+x-xw/2;
		yy2=yy1;
		xx2=xx1+xw;
		XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);

		for(ip=1;ip<*np;ip++) {
			yy1=(zp[ip]-x1begb)/(x1endb-x1begb)*height+y-xh/2;
			xx1=(vp[ip]-x2begb)/(x2endb-x2begb)*width+x;
			yy2=yy1+xh;
			xx2=xx1;
			XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);
			yy1=(zp[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(vp[ip]-x2begb)/(x2endb-x2begb)*width+x-xw/2;
			yy2=yy1;
			xx2=xx1+xw;
			XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);

			yy1=(zp[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(vp[ip-1]-x2begb)/(x2endb-x2begb)*width+x;
			yy2=(zp[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(vp[ip]-x2begb)/(x2endb-x2begb)*width+x;
			XDrawLine(dpy,win,gcvp,xx1,yy1,xx2,yy2);
		}
		/* free resources before returning */
                XFreeGC(dpy,gcvp);
                XFreeGC(dpy,gcvip);

	}

	/* reference velocity curve */
	if(nr>1) {
		/* create graphics contexts */
                gcvr = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,vrefcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcvr,ecolor.pixel);
                else
                        XSetForeground(dpy,gcvr,1L);
                XSetLineAttributes(dpy,gcvr,2,LineSolid,CapButt,JoinMiter);

		for(ip=1;ip<nr;ip++) {
			yy1=(zr[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(vr[ip-1]-x2begb)/(x2endb-x2begb)*width+x;
			yy2=(zr[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(vr[ip]-x2begb)/(x2endb-x2begb)*width+x;
			XDrawLine(dpy,win,gcvr,xx1,yy1,xx2,yy2);
		}
		/* free resources before returning */
                XFreeGC(dpy,gcvr);

	}

	/* previous picks read from mpicks */
	if(npcrd>1) {
		/* create graphics contexts */
                gcvpcrd = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,vpcrdcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcvpcrd,ecolor.pixel);
                else
                        XSetForeground(dpy,gcvpcrd,1L);
                XSetLineAttributes(dpy,gcvpcrd,2,LineSolid,CapButt,JoinMiter);

		gcvip = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,vpcrdcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcvip,ecolor.pixel);
                else
                        XSetForeground(dpy,gcvip,1L);
                XSetLineAttributes(dpy,gcvip,1,LineSolid,CapButt,JoinMiter);
                xw = width /25;
                xh = height /25;

                yy1=(zpcrd[0]-x1begb)/(x1endb-x1begb)*height+y-xh/2;
                xx1=(vpcrd[0]-x2begb)/(x2endb-x2begb)*width+x;
                yy2=yy1+xh;
                xx2=xx1;
                XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);
                yy1=(zpcrd[0]-x1begb)/(x1endb-x1begb)*height+y;
                xx1=(vpcrd[0]-x2begb)/(x2endb-x2begb)*width+x-xw/2;
                yy2=yy1;
                xx2=xx1+xw;
                XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);


		for(ip=1;ip<npcrd;ip++) {
			yy1=(zpcrd[ip]-x1begb)/(x1endb-x1begb)*height+y-xh/2;
                        xx1=(vpcrd[ip]-x2begb)/(x2endb-x2begb)*width+x;
                        yy2=yy1+xh;
                        xx2=xx1;
                        XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);
                        yy1=(zpcrd[ip]-x1begb)/(x1endb-x1begb)*height+y;
                        xx1=(vpcrd[ip]-x2begb)/(x2endb-x2begb)*width+x-xw/2;
                        yy2=yy1;
                        xx2=xx1+xw;
                        XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);


			yy1=(zpcrd[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(vpcrd[ip-1]-x2begb)/(x2endb-x2begb)*width+x;
			yy2=(zpcrd[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(vpcrd[ip]-x2begb)/(x2endb-x2begb)*width+x;
			XDrawLine(dpy,win,gcvpcrd,xx1,yy1,xx2,yy2);
		}
		/* free resources before returning */
                XFreeGC(dpy,gcvpcrd);
                XFreeGC(dpy,gcvip);

	}

	/* interval velocity curve of picked result */
	if(ppkey==4 && *np > 1 ) viplot(dpy, win, vpickcolor,  
					x, y, width, height,
					x1begb, x1endb, x2begb, x2endb,
					zp, vp, *np, zr, nr, ivmode, 
					ztop, zbot, v0, dvdz,
					vatop, vabot, *ia, ivof);

	/* interval velocity curve of previous picked result */
	if(ppkey==4 && npcrd > 1 ) viplot(dpy, win, vpcrdcolor,  
					x, y, width, height,
					x1begb, x1endb, x2begb, x2endb,
					zpcrd, vpcrd, npcrd, zr, nr, ivmode, 
					ztop, zbot, v0, dvdz,
					vatop, vabot, *ia, ivof);

	/* interval velocity curve of reference velocity input */
	if(ppkey==4 && nr > 1 ) {
		/* create graphics contexts */
                gcvir = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,vrefcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcvir,ecolor.pixel);
                else
                        XSetForeground(dpy,gcvir,1L);
                XSetLineAttributes(dpy,gcvir,2,LineOnOffDash,CapButt,JoinMiter);

		vi = (float *) malloc(nr*sizeof(float));

		vi[0] = vr[0];
		for(ip=1;ip<nr;ip++) {
		   	vi[ip] = (zr[ip]*vr[ip]-zr[ip-1]*vr[ip-1])
					/(zr[ip]-zr[ip-1]);
	   	} 
		for(ip=1;ip<nr;ip++) {
			yy1=(zr[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(vi[ip-1]-x2begb)/(x2endb-x2begb)*width+x;
			yy2=(zr[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(vi[ip]-x2begb)/(x2endb-x2begb)*width+x;
			XDrawLine(dpy,win,gcvir,xx1,yy1,xx2,yy2);
		}
		/* free resources before returning */
                XFreeGC(dpy,gcvir);
		free(vi);
	}
	/* delete dvdz analysis point */
	if( ppkey==6 && *ia > 0 ) {
		ipmin = 0;
		for(ip=0;ip<*ia;ip++) {
			if( zptop[ip] <= x1 && x1 <= zpbot[ip] ) {
				ipmin = ip;
				break;
			}
		}
		*ia= *ia - 1;
		for(ip=ipmin;ip<*ia;ip++) {
			zptop[ip] = zptop[ip+1];
			zpbot[ip] = zpbot[ip+1];
			vatop[ip] = vatop[ip+1];
			vabot[ip] = vabot[ip+1];
			dvdz[ip] = dvdz[ip+1];
			v0[ip] = v0[ip+1];
		}
	}
	/* velocity gradient picking plot */
	if( (ppkey==5 || ppkey==3 || ppkey==6 ) && *ia > 0 ) {
		zz = (float*) malloc(maxpks*sizeof(float));
		vaa = (float*) malloc(maxpks*sizeof(float));
		for(ip=0;ip<*ia;ip++) {
			zandva(zptop[ip],zpbot[ip],zp,vp,*np,zz,vaa,&nn);
			if(nn>1) {
				ztop[ip] = zz[0];
				zbot[ip] = zz[nn-1];
				vatop[ip] = vaa[0];
				vabot[ip] = vaa[nn-1];
				estvi_(zz,vaa,&nn,&ibfit,&v00,&aa);
				dvdz[ip] = aa;
				v0[ip] = v00;
			} else {
				ztop[ip] = x1begb - 1000. ;
				zbot[ip] = x1begb - 1000.;
				vatop[ip] = vp[0];
				vabot[ip] = vp[0];
				dvdz[ip] = 0.;
				v0[ip] = vp[0];
			}
		}
		free(zz);
		free(vaa);
		/* create graphics contexts */
                gcvip = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,vpickcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcvip,ecolor.pixel);
                else
                        XSetForeground(dpy,gcvip,1L);
                XSetLineAttributes(dpy,gcvip,2,LineOnOffDash,CapButt,JoinMiter);

                gcvir = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,vpickcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcvir,ecolor.pixel);
                else
                        XSetForeground(dpy,gcvir,1L);
                XSetLineAttributes(dpy,gcvir,1,LineSolid,CapButt,JoinMiter);

		for(ip=0;ip<*ia;ip++) {
			if(ztop[ip] < zbot[ip]) {
				yy1=(ztop[ip]-x1begb)/(x1endb-x1begb)*height+y;
				xx1=(vatop[ip]-x2begb)/(x2endb-x2begb)*width+x;
				yy2=yy1;
				xx2=width+x;
				XDrawLine(dpy,win,gcvir,xx1,yy1,xx2,yy2);

				yy1=(zbot[ip]-x1begb)/(x1endb-x1begb)*height+y;
				xx1=(vabot[ip]-x2begb)/(x2endb-x2begb)*width+x;
				yy2=yy1;
				xx2=width+x;
				XDrawLine(dpy,win,gcvir,xx1,yy1,xx2,yy2);

				yy1=(ztop[ip]-x1begb)/(x1endb-x1begb)*height+y;
				xx1=(v0[ip]-x2begb)/(x2endb-x2begb)*width+x;
				yy2=(zbot[ip]-x1begb)/(x1endb-x1begb)*height+y;
				xx2=(v0[ip]+dvdz[ip]*(zbot[ip]-ztop[ip])-x2begb)
						/(x2endb-x2begb)*width+x;
				XDrawLine(dpy,win,gcvip,xx1,yy1,xx2,yy2);
			}
		}
		/* free resources before returning */
                XFreeGC(dpy,gcvip);
                XFreeGC(dpy,gcvir);
	}
}

/* plot interval velocity curve of picked result */
void viplot(Display *dpy, Window win, char *vicolor,  
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *zp, float *vp, int np, float *zr, int nr, int ivmode,
	float *ztop, float *zbot, float *v0, float *dvdz,
	float *vatop, float *vabot, int ia, int ivof) {
	
	int ni, *indx, xx1, xx2, yy1, yy2;
	float *va, *vi, *zi;
	int ip; 
	float z1,z2,v1,v2,aa;

	GC gci;
        XGCValues *values;
        XColor scolor,ecolor;
        XWindowAttributes wa;
        Colormap cmap;
	int scr;

        /* get screen */
        scr = DefaultScreen(dpy);
        /* determine window's current colormap */
        XGetWindowAttributes(dpy,win,&wa);
        cmap = wa.colormap;

	/* create graphics contexts */
       	gci = XCreateGC(dpy,win,0,values);
       	if (XAllocNamedColor(dpy,cmap,vicolor,&scolor,&ecolor))
       		XSetForeground(dpy,gci,ecolor.pixel);
       	else
        	XSetForeground(dpy,gci,1L);
        XSetLineAttributes(dpy,gci,2,LineOnOffDash,CapButt,JoinMiter);


	if (ivmode==1 && ivof==0) {
		if( nr > 1 && ia == 0 ) {
			ni = nr;
			va = (float *) malloc(ni*sizeof(float));
			vi = (float *) malloc(ni*sizeof(float));
			zi = (float *) malloc(ni*sizeof(float));
			indx = (int *) malloc(ni*sizeof(int));

			for(ip=0;ip<ni;ip++) zi[ip]=zr[ip];

			lin1d_(zp,vp,&np,zi,va,&ni,indx);

			vi[0] = va[0];
			for(ip=1;ip<ni;ip++) {
				if(zi[ip]<=zp[np-1]) {  
			   		vi[ip] = (zi[ip]*va[ip]
						-zi[ip-1]*va[ip-1])
							/(zi[ip]-zi[ip-1]);
		   		} else {
					vi[ip] = vi[ip-1];
		   		}
			}
			for(ip=0;ip<ni;ip++) {
				va[ip] = vi[ip];
			}
		} else {
			ni = np;
			va = (float *) malloc(ni*sizeof(float));
			vi = (float *) malloc(ni*sizeof(float));
			zi = (float *) malloc(ni*sizeof(float));
			for(ip=0;ip<ni;ip++) {
				zi[ip] = zp[ip];
				xx1 = -1;
				aa = 0.;
				for(yy1=0;yy1<ia;yy1++) {
					if(zi[ip]>ztop[yy1] &&	
					   zi[ip]<=zbot[yy1]) {
						xx1 = yy1;
						break;
					} 	
				}
				if (xx1 != -1) aa = dvdz[xx1];
				if(v0[xx1]>0.) {
					vi[ip] = v0[xx1]+(zp[ip]-ztop[xx1])*aa;
					xx2 = ip - 1;
					if(xx2<0) xx2 = 0;
					if(zi[xx2]>ztop[xx1] &&	
					   zi[xx2]<=zbot[xx1]) {
						va[ip] = v0[xx1]+
							(zp[xx2]-ztop[xx1])*aa;
					} else {
						va[ip] = v0[xx1];
					}
				} else {
					z2 = zp[ip];
					v2 = vp[ip];
					if(ip==0) {
						v1 = v2;
						z1 = 0.;
					} else {
						v1 = vp[ip-1];	
						z1 = zp[ip-1];
					}
					if(z2>z1) {
						va[ip]=(z2*v2-z1*v1)/(z2-z1) - 
							0.5*aa*(z2-z1);
					} else {
						va[ip] = v2;
					}
					vi[ip] = va[ip] + aa * (z2-z1); 
				}
			}
			
		}
	
	} else if(ivof==0 && ivmode!=1) {
		ni = np;
		va = (float *) malloc(ni*sizeof(float));
		vi = (float *) malloc(ni*sizeof(float));
		zi = (float *) malloc(ni*sizeof(float));

		vi[0] = vp[0];
		va[0] = vi[0];
		zi[0] = zp[0];
		for(ip=1;ip<ni;ip++) {
			zi[ip] = zp[ip];	
		   	vi[ip] = (zp[ip]*vp[ip]-zp[ip-1]*vp[ip-1])
					/(zp[ip]-zp[ip-1]);
			va[ip] = vi[ip];
		}
	} else if(ivof==1) {
		ni = np;
		va = (float *) malloc(ni*sizeof(float));
		vi = (float *) malloc(ni*sizeof(float));
		zi = (float *) malloc(ni*sizeof(float));

		vi[0] = vp[0];
		va[0] = vi[0];
		zi[0] = zp[0];
		for(ip=1;ip<ni;ip++) {
			zi[ip] = zp[ip];	
		   	vi[ip] = (zp[ip]*vp[ip]*vp[ip]
					-zp[ip-1]*vp[ip-1]*vp[ip-1])
					/(zp[ip]-zp[ip-1]);
			if(vi[ip]>0.) {
				vi[ip] = sqrt(vi[ip]);
			} else {
				vi[ip] = 0.;
			}
			va[ip] = vi[ip];
		}
	}

	
	for(ip=1;ip<ni-1;ip++) {
	/*
		fprintf(stderr,"draw point ip=%d \n",ip);
	*/

		yy1=(zi[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
		xx1=(va[ip]-x2begb)/(x2endb-x2begb)*width+x;
		yy2=(zi[ip]-x1begb)/(x1endb-x1begb)*height+y;
		xx2=(vi[ip]-x2begb)/(x2endb-x2begb)*width+x;
		XDrawLine(dpy,win,gci,xx1,yy1,xx2,yy2);

		yy1=(zi[ip]-x1begb)/(x1endb-x1begb)*height+y;
		xx1=(vi[ip]-x2begb)/(x2endb-x2begb)*width+x;
		yy2=yy1;
		xx2=(va[ip+1]-x2begb)/(x2endb-x2begb)*width+x;
		XDrawLine(dpy,win,gci,xx1,yy1,xx2,yy2);
	}

	yy1=(zi[ni-2]-x1begb)/(x1endb-x1begb)*height+y;
	xx1=(va[ni-1]-x2begb)/(x2endb-x2begb)*width+x;
	yy2=(zi[ni-1]-x1begb)/(x1endb-x1begb)*height+y;
	xx2=(vi[ni-1]-x2begb)/(x2endb-x2begb)*width+x;
	XDrawLine(dpy,win,gci,xx1,yy1,xx2,yy2);

	if(zi[0]>x1begb) {
		yy1=y;
		xx1=(va[0]-x2begb)/(x2endb-x2begb)*width+x;
		yy2=(zi[0]-x1begb)/(x1endb-x1begb)*height+y;
		xx2=xx1;
		XDrawLine(dpy,win,gci,xx1,yy1,xx2,yy2);

		yy1=(zi[0]-x1begb)/(x1endb-x1begb)*height+y;
		xx1=(vi[0]-x2begb)/(x2endb-x2begb)*width+x;
		yy2=yy1;
		xx2=(va[1]-x2begb)/(x2endb-x2begb)*width+x;
		XDrawLine(dpy,win,gci,xx1,yy1,xx2,yy2);
	}
	if(zi[ni-1]<x1endb) {
		yy1=(zi[ni-1]-x1begb)/(x1endb-x1begb)*height+y;
		xx1=(vi[ni-1]-x2begb)/(x2endb-x2begb)*width+x;
		yy2=height+y;
		xx2=xx1;
		XDrawLine(dpy,win,gci,xx1,yy1,xx2,yy2);
	}
		

	/* free resources before returning */
        XFreeGC(dpy,gci);
	free(indx);
	free(vi);
	free(va);
	free(zi);

}
