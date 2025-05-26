
#include "comva.h"
#include "su.h"
#include "segy.h"
#include "header.h"
#include "par.h"
#include "cwp.h"

/*********************** self documentation *****************************/
string sdoc =
" CSMODLE - Common-Shot Modeling using CWP's Cshot Raytracing	\n"
"                                                               \n"
" csmodel hfile= [parameters]   		      		\n"
"                                                               \n"
" Required parameters:                                          \n"
"  hfile=              name of Hfile (horizon/velocity file)    \n"
" Optional parameters:                                          \n"
"  sx=                 source lateral locations			\n"
"                      (sx can be specified as an array:        \n"
"                       sx=0.,50.,2000.,6000)                   \n"
"  os=xmin             first source lateral location            \n"           
"                      If not specified, default to the minimum \n"
"                      x location in the input hfile (xmin)	\n"
"                      (ignored when sx is specified)		\n"
"  ds=0.               source spacing                           \n"
"                      (ignored when sx is specified)		\n"
"  ns=1                number of sources                        \n"
"                      (if sx is given, the actual locations of \n"
"                       source are given by sx array; otherwise \n"
"                       the single number os, ds and ns will be \n"
"                       used to compute the source locations:   \n"
"                       Si = os + (i-1)*ds, i=1,2,...,ns )      \n"
"  sz=0.               source depth                  		\n"           
"  r1=0.               starting lateral location of first half spread 	\n" 
"                      (relative to source Si position. + right; - left)\n"
"  r2=25.              ending lateral location of first half spread 	\n" 
"                      (relative to source Si position)			\n"
"  r3=50.              starting lateral location of second half spread 	\n" 
"                      (relative to source Si position)			\n"
"  r4=2975.            ending lateral location of second half spread 	\n" 
"                      (relative to source Si position)			\n"
"  dr=25.              receiver spacing                                 \n"
"  rz=0.               receiver depth                                   \n"
"  amin=-90.           minimum takeoff angle                            \n"
"  amax=90.            maximum takeoff angle                            \n"
"  da=1.               takeoff angle increment                          \n"
"  dt=4                sampling rate in ms                              \n"
"  tmax=4000           record length in ms                             	\n"
"  wl=150              wavelet length in ms                             \n"
"  f1=10.              starting frequency at lower slope of spectrum	\n"  
"  f2=25.              ending frequency at lower slope of spectrum	\n"  
"  f3=35.              starting frequency at higher slope of spectrum	\n"  
"  f4=50.              ending frequency at higher slope of spectrum	\n"  
"  hn=2,3,4,...        horizon numbers to compute primary reflections   \n"
"                      (default to all horizons except z=0 in hfile)    \n"
"                      (hn=0 no reflection will be computed)		\n"
"  direct=n            direct wave computed (n=no y=yes)		\n" 
"  headhn=             head wave horizon numbers (for example, headhn=2,5)\n"
"                      (default to no head waves)			\n"
"  mfile=              name of file specifying multiple reflection 	\n"
"                      interface indices				\n"
"                      (default to no multiples)			\n"
"                      the mfile should be specified as, e.g.:		\n"
"                      3 1 3						\n"
"                      2 1 2 4 3 4					\n"
"                      ...						\n"
"                      where the number indicates the reflection interface \n"
"                      number of a multiple reflection ray.		\n"
"                      The maximum number of multiples is 32 		\n" 
"  rfsave=0            ray tracing files save mode (0=no 1=yes)		\n"
"                      when rfsave=1, geometry-file, parameter files, 	\n"
"                      ray path xt file (csm_os=OSlisting), etc, will be \n"
"                      saved.\n"
"  plot=0              graphics mode (0=x-window 1=postscript hard copy \n" 
"                                     -1=no graphics)			\n"
"  is=1                the shot to display ray diagram			\n" 
"  comp=1              computation mode 				\n" 
"                      (=0 	--- compute only rays of the is-th shot	\n" 
"                                   and display 			\n"
"                       =1 	--- compute both rays and traces of 	\n"
"                                   the is-th shot (and display)	\n" 
"                       =2 	--- compute only traces of all shots 	\n"
"                       =-1 	--- compute only rays of all shots and 	\n"
"                                   will not plot rays) 	\n"
"  gathers=            name of dataset of computed shot gathers \n"
"                      (required when comp=1 or 2) 			\n"  
"  icolor=7             color for interfaces when x-window graphics used\n"
"  rcolor=2             color for normal rays when x-window graphics used\n"
"  ccolor=3             color for caustic rays when x-window graphics used\n"
"                       0      black					\n"
"                       1      white					\n"
"                       2      red					\n"
"                       3      green					\n"
"                       4      dark blue				\n"
"                       5      light blue				\n"
"                       6      violet 					\n"
"                       7      yellow 					\n"
"  vcid=0               velocity contrast interface display 		\n"
"                       (0=display interface regardless velocity contrast\n"
"                        1=display only when there is velocity contrast)\n"
" Notes:								\n"
"    1. maximum number of interfaces in hfile is currently set to 50.	\n" 
"    2. maximum number of points per interface in hfile is currently 	\n"
"       set to 100.							\n" 
"    3. the current version of csmodel requires all the horizons extended \n"
"       to both left and right sides of the model.			\n"
"\n"
" author: Zhiming Li		      		8/10/92			\n"
;
/**************** end self doc *******************************************/

/* define maximum number of horizons and maximum number of picks per horizon */
#define maxhs 128
#define maxpks 256
#define maxspl 101
#define maxint 50

segychdr ch;
segybhdr bh;
segytrace tr;

main (int argc, char **argv) 
{

	/* variables for hfile input */
	int *npicks,*hnums,ih,nhs,*difs,
		cdpxmini,cdpxmaxi;
	float *xpicks,*zpicks,*veltops,*velbots,*velavgs,*dvdzs,
		*dens,*qval,*pois,
		xmini,xmaxi,zmini,zmaxi,dcdpi,vmini,vmaxi;
	char hnames[maxhs][80], dunitsi[80], zattribi[80], vtypei[80];
	char *gathers;
	char *hfile;

	/* variable for this program */
	FILE *infp, *outfp, *paramfp, *modelfp, *geofp;
	FILE *datafp;
	int *hn, nh, jh, i, j, imax, ir1, ir2, ir3, ir4, ip, nr;
	int j1, j2, one=1, nn, ihfile;
	int rfsave, plot, comp, is, ns, js;
	int it,nt;
	float *sx,sz,r1,r2,r3,r4,dr,rz,amin,amax,da,dt,tmax,wl,f1,f2,f3,f4;
	float os;
	float ds,ssx,tmp, pp, rr1, rr2, rr3, rr4, dcdp, vint;
	float tmp1, tmp2;
	char cmd[1024];
	string direct="n";
	int *headhn, nhead;
	float dstn, xstn0;
	char param1[80], param2[80];
	char param1_1[80], param1_2[80];
	char fname[80];
	char **mn;
	char *mfile, *cbuf; 
	FILE *mfp;
	int maxnm=32, nm;

	float xpre, xnow;
	int icolor, ccolor, rcolor;
	int vcid;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* read in hfile */ 
	/* memory allocations for reading hfile */
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
	for(ih=0;ih<maxhs*maxpks;ih++) {
		veltops[ih]=0.;
		velbots[ih]=0.;
		difs[ih] = 0;
		velavgs[ih] = 0.;
		dvdzs[ih] = 0.;
	}

	/* read in hfile*/
	nhs = 0;
	dcdpi = 0.;
	if (!getparstring("hfile",&hfile)) err("hfile missing");
	infp = efopen(hfile,"r");
	ifileread(infp,&nhs,xpicks,zpicks,veltops,velbots,difs,
		  dens,qval,pois,velavgs,dvdzs,
              	  (char *)hnames,hnums,npicks,maxhs,maxpks,
		  &xmini,&xmaxi,&zmini,&zmaxi,
		  &cdpxmini,&cdpxmaxi,&vmini,&vmaxi,
		  vtypei,dunitsi,zattribi,&dcdpi,&ihfile);
	efclose(infp);

	free(dens);
	free(qval);
	free(pois);
	free(velavgs);
	free(dvdzs);

	hn = (int *) malloc(nhs*sizeof(int));
	mn = (char **) malloc(maxnm*sizeof(char *));
	headhn = (int *) malloc(nhs*sizeof(int));

	/* get parameters */
	if (!getparint("ns",&ns)) ns = 1;
	js = countparval("sx");
	if(js==0) js = 1;
	if(js>1) ns = js;
	sx = (float *) malloc(ns*sizeof(float));

	if (!getparfloat("sx",sx)) {
		if (!getparfloat("os",&os)) os = xmini;
		if (!getparfloat("ds",&ds)) ds = 0.;
		for(is=0;is<ns;is++) sx[is] = os + is*ds;
	} else {
		os = sx[0];
	}

	if (!getparint("is",&is)) is = 1;
	if (!getparfloat("sz",&sz)) sz = 0.;
	if (!getparfloat("r1",&r1)) r1 = 0.;
	if (!getparfloat("r2",&r2)) r2 = 25.;
	if (!getparfloat("r3",&r3)) r3 = 50.;
	if (!getparfloat("r4",&r4)) r4 = 2975.;
	if (!getparfloat("dr",&dr)) dr = 25.;
	if (!getparfloat("rz",&rz)) rz = 0.;
	if (!getparfloat("amin",&amin)) amin = -90.;
	if (!getparfloat("amax",&amax)) amax = 90.;
	if (!getparfloat("da",&da)) da = 1.;
	if (!getparfloat("dt",&dt)) dt = 4.;
	if (!getparfloat("tmax",&tmax)) tmax = 4000.;
	if (!getparfloat("wl",&wl)) wl = 150.;
	if (!getparfloat("f1",&f1)) f1 = 10.;
	if (!getparfloat("f2",&f2)) f2 = 25.;
	if (!getparfloat("f3",&f3)) f3 = 35.;
	if (!getparfloat("f4",&f4)) f4 = 50.;
	nh = countparval("hn");
	if (!getparint("hn",hn)) { 
		nh = nhs-1;
        	for (ih=0; ih<nh; ih++) hn[ih] = ih + 1;
	} else {
        	for (ih=0; ih<nh; ih++) hn[ih] = hn[ih] - 1;
	}
	if(nh==1 && hn[0]==-1) nh = 0;
	if(nh==1 && hn[0]==0) nh = 0;

	nhead = countparval("headhn");
	if (!getparint("headhn",headhn)) { 
		nhead = 0;
	} else {
        	for (ih=0; ih<nhead; ih++) headhn[ih] = headhn[ih] - 1;
	}
	if(nh>49) err("maximum 50 horizons allowed ");

	nm = 0;
	nm = getparstring("mfile",&mfile);
	if(nm>0) {
		mfp = efopen(mfile,"r");
		cbuf = (char *) malloc(81*sizeof(char));
		nm = 0;
		for(i=0;i<maxnm;i++) {
			if (feof(mfp) !=0 ) break;
			bzero(cbuf,81);
                	fgets(cbuf,81,mfp);
			if(strncmp(cbuf, "0", 1)==0 || 
			   strncmp(cbuf, "1", 1)==0 || 
			   strncmp(cbuf, "2", 1)==0 || 
			   strncmp(cbuf, "3", 1)==0 || 
			   strncmp(cbuf, "4", 1)==0 || 
			   strncmp(cbuf, "5", 1)==0 || 
			   strncmp(cbuf, "6", 1)==0 || 
			   strncmp(cbuf, "7", 1)==0 || 
			   strncmp(cbuf, "8", 1)==0 || 
			   strncmp(cbuf, "9", 1)==0 ) { 
              			mn[nm] = (char*) malloc(81);
				strncpy(mn[nm],cbuf,81);
			        nm = nm + 1;
			}
		}
		/*
		for(i=0;i<nm;i++) {
			fprintf(stderr,"mn=%s i=%d \n",mn[i],i);
		}
		*/
	}

	if (!getparint("rfsave",&rfsave)) rfsave = 0;
	if (!getparint("plot",&plot)) plot = 0;
	if (!getparint("comp",&comp)) comp = 1;
	if (!getparint("icolor",&icolor)) icolor = 7;
	if (!getparint("rcolor",&rcolor)) rcolor = 2;
	if (!getparint("ccolor",&ccolor)) ccolor = 3;
	if (!getparint("vcid",&vcid)) vcid = 0;
	getparstring("direct",&direct);
	if ( comp == 0 || comp==1 ) {
		ns = 1;
		sx[0] = sx[is-1];
	}
	if( comp == 2 || comp == -1) plot = -1;

	dstn = dr;
	xstn0 = sx[0] + r1;

	/* open shot gather */
	if(comp==1 || comp==2) {
		if (!getparstring("gathers",&gathers)) {
			err(" must specify name of gathers !");
		} else {
			outfp = efopen(gathers,"w");
		}
	}
	
	/* output model file */	
	bzero(fname,80);
	sprintf(fname,"model-file_os=%g\0",os);
	modelfp = fopen(fname,"w");
	if(nhs>maxint) err("too many horizons (max 50 allowed)");
	for(ih=0;ih<nhs;ih++) {
		imax = npicks[ih];
		if(imax>maxspl) {
			imax = maxspl;
			tmp = npicks[ih];
			tmp = tmp / (imax-1);
		} else {
			tmp = 1.0;
		}
		for(i=0;i<imax;i++) { 
			pp = i*tmp + 0.5;
			ip = pp;
			if ( ip<npicks[ih] ) {
				vint = 0.5*(veltops[ih*maxpks+ip]
					+velbots[ih*maxpks+ip]);
				fprintf(modelfp,"%9.2f   %9.2f   %9.2f\n",
				xpicks[ih*maxpks+ip],zpicks[ih*maxpks+ip],vint);
			}

			xpre = xnow;
			xnow = xpicks[ih*maxpks+ip];
			if(i>0) {
				if(xnow<=xpre) 
err(" x positions must be increasing in horizon %d xpre=%g xnow=%g of hfile",
	ih+1,xpre,xnow);
			} 
		}
		fprintf(modelfp," 1.000000   -99999.00   %9.2f\n",vint);
	}
	fprintf(modelfp,"%d\n",vcid);
	efclose(modelfp);


	/* output geometry file */
	bzero(fname,80);
	sprintf(fname,"geometry-file_os=%g\0",os);
	geofp = fopen(fname,"w");
	
	fprintf(geofp,
"1         %-9.2f                 :reference station number and x-coord\n",
	xstn0);
	fprintf(geofp,
"%-9.2f     %-9.2f                :station spacing and receiver depth\n",
	dstn,rz);

	for(js=0;js<ns;js++) {
		/* actual receiver positions */
		ssx = sx[js] - xstn0;
		rr1 = ssx + r1; 
		rr2 = ssx + r2; 
		rr3 = ssx + r3; 
		rr4 = ssx + r4; 
		tmp = rr1/dstn + 1.5;
		ir1 = tmp;	
		tmp = rr2/dstn + 1.5;
		ir2 = tmp;	
		tmp = rr3/dstn + 1.5;
		ir3 = tmp;	
		tmp = rr4/dstn + 1.5;
		ir4 = tmp;	
		ssx = ssx/dstn + 1;

		fprintf(geofp,
"%d  %d  %d   %d      %-6.2f %-5.1f        :shot %d - r1 r2 r3 r4 s sdepth\n",
			ir1,ir2,ir3,ir4,ssx,sz,js+1);
	}

	efclose(geofp);
	
	/* name param1_os=XXX */
	sprintf(param1,"param1_os=%g\0",os);
	sprintf(param1_1,"param1_1_os=%g\0",os);
	sprintf(param1_2,"param1_2_os=%g\0",os);
	/* name param2_os=XXX */
	sprintf(param2,"param2_os=%g\0",os);

	/* output param1_1 file */
	paramfp = fopen(param1_1,"w");
	fprintf(paramfp,
"model-file_os=%g                          :model file\n",os);
	fprintf(paramfp,
"%-2d                                  :#interfaces in model\n",nhs-1);
	fprintf(paramfp,
"plotcolors_os=%g                          :model colors file\n",os);
	if(plot==-1) {
	fprintf(paramfp,
"                                    :first plot descriptor (mwq)\n");
	} else {
	fprintf(paramfp,
"m                                   :first plot descriptor (mwq)\n");
	}
	fprintf(paramfp,
"don't care                          :well coordinates\n");
	fprintf(paramfp,
"s                                   :shooting mode (sd)\n");
	fprintf(paramfp,
"geometry-file_os=%g                       :receiver geometry\n",os);  
	if(plot==-1) {
	fprintf(paramfp,
"                                    :second plot descriptor (sgq)\n");
	fprintf(paramfp,
"l                                   :job descriptor (rlt)\n");
	} else {
	fprintf(paramfp,
"sg                                  :second plot descriptor (sgq)\n");
	fprintf(paramfp,
"rl                                  :job descriptor (rlt)\n");
	}
	fprintf(paramfp,
"csm_os=%g                                 :output filename(s)\n",os);
	fprintf(paramfp,
"%-5.1f  %-5.1f                        :range of takeoff angles\n",amin,amax);
	fprintf(paramfp,
"%-5.1f                               :increment in takeoff angle\n",da);
	for(ih=0;ih<nhs;ih=ih+3) {
		imax = 3;
		if(ih+imax >nhs) imax = nhs - ih;
		for(i=0;i<imax;i++) {
			jh = ih + i;
			tmp = 0.;
			j1 = 1;
			j2 = npicks[jh];
			nn = npicks[jh];
			if(ns==1) {
				tmp1 = sx[0]+r1;
				if(r1>0.) tmp1 = sx[0]; 
				tmp2 = sx[0]+r4;
				if(r4<0.) tmp2 = sx[0];
				if(tmp1>tmp2) {
					tmp = tmp2;
					tmp1 = tmp2;
					tmp2 = tmp;
				}
				if(tmp1<xpicks[jh*maxpks]) {
					j1 = 1;
				} else if(tmp1>xpicks[jh*maxpks+nn-1]) {
					j1 = nn;
				} else {
					bisear_(&nn,&one,
						xpicks+jh*maxpks,&tmp1,&j1);
				}
				if(tmp2<xpicks[jh*maxpks]) {
					j2 = 1;
				} else if(tmp2>xpicks[jh*maxpks+nn-1]) {
					j2 = nn;
				} else {
					bisear_(&nn,&one,
						xpicks+jh*maxpks,&tmp2,&j2);
				}
			}
		 	for(j=j1-1;j<j2;j++) {
				tmp = tmp + veltops[jh*maxpks+j] 
				    + velbots[jh*maxpks+j];
			}	
			tmp = tmp /(j2-j1+1);
			fprintf(paramfp,"%-7.1f ",0.5*tmp); 
		}
		if(ih+imax!=nhs) {
			fprintf(paramfp,"\n");
		} else {
			fprintf(paramfp,"                       :velocities\n");
		}
	}
	fprintf(paramfp,
"%1s                                   :direct wave? (y or n)\n",direct);
	if(nhead==0) {
		fprintf(paramfp,
"                                    :headwave interface numbers (1,2,...)\n");
	} else {
		for(ih=0;ih<nhead;ih++) {
			fprintf(paramfp,"%d ",headhn[ih]);
		}
		fprintf(paramfp,
"                                    :headwave interface numbers (1,2,...)\n");
	}
	fprintf(paramfp,
"n                                   :all primaries? (y or n)\n");
	for(ih=0;ih<nh;ih++) {
		fprintf(paramfp,
"%-2d                                  :reflection from interface %d\n",
			hn[ih],hn[ih]);
	}
	for(i=0;i<nm;i++) fprintf(paramfp, "%s",mn[i]);
	
	fclose(paramfp);

	/* output param1_2 file */
	paramfp = fopen(param1_2,"w");
	fprintf(paramfp,
"model-file_os=%g                          :model file\n",os);
	fprintf(paramfp,
"%-2d                                  :#interfaces in model\n",nhs-1);
	fprintf(paramfp,
"plotcolors_os=%g                          :model colors file\n",os);
	fprintf(paramfp,
"                                    :first plot descriptor (mwq)\n");
	fprintf(paramfp,
"don't care                          :well coordinates\n");
	fprintf(paramfp,
"s                                   :shooting mode (sd)\n");
	fprintf(paramfp,
"geometry-file_os=%g                       :receiver geometry\n",os);  
	fprintf(paramfp,
"                                    :second plot descriptor (sgq)\n");
	fprintf(paramfp,
"t                                   :job descriptor (rlt)\n");
	fprintf(paramfp,
"csm_os=%g                                 :output filename(s)\n",os);
	fprintf(paramfp,
"%-5.1f  %-5.1f                        :range of takeoff angles\n",amin,amax);
	fprintf(paramfp,
"%-5.1f                               :increment in takeoff angle\n",da);
	for(ih=0;ih<nhs;ih=ih+3) {
		imax = 3;
		if(ih+imax >nhs) imax = nhs - ih;
		for(i=0;i<imax;i++) {
			jh = ih + i;
                        tmp = 0.;
                        j1 = 1;
                        j2 = npicks[jh];
                        nn = npicks[jh];
                        if(ns==1) {
                                tmp1 = sx[0]+r1;
                                if(r1>0.) tmp1 = sx[0];
                                tmp2 = sx[0]+r4;
                                if(r4<0.) tmp2 = sx[0];
                                if(tmp1>tmp2) {
                                        tmp = tmp2;
                                        tmp1 = tmp2;
                                        tmp2 = tmp;
                                }
                                if(tmp1<xpicks[jh*maxpks]) {
                                        j1 = 1;
                                } else if(tmp1>xpicks[jh*maxpks+nn-1]) {
                                        j1 = nn;
                                } else {
                                        bisear_(&nn,&one,
                                                xpicks+jh*maxpks,&tmp1,&j1);
                                }
                                if(tmp2<xpicks[jh*maxpks]) {
                                        j2 = 1;
                                } else if(tmp2>xpicks[jh*maxpks+nn-1]) {
                                        j2 = nn;
                                } else {
                                        bisear_(&nn,&one,
                                                xpicks+jh*maxpks,&tmp2,&j2);
                                }
                        }
                        for(j=j1-1;j<j2;j++) {
                                tmp = tmp + veltops[jh*maxpks+j]
                                    + velbots[jh*maxpks+j];
                        }
                        tmp = tmp /(j2-j1+1);
                        fprintf(paramfp,"%-7.1f ",0.5*tmp);
			/*
			jh = ih + i;
			tmp = 0.;
		 	for(j=0;j<npicks[jh];j++) {
				tmp = tmp + veltops[jh*maxpks+j] 
				    + velbots[jh*maxpks+j];
			}	
			tmp = tmp /(npicks[jh]*2);
			fprintf(paramfp,"%-7.1f ",tmp); 
			*/
		}
		if(ih+imax!=nhs) {
			fprintf(paramfp,"\n");
		} else {
			fprintf(paramfp,"                       :velocities\n");
		}
	}
	fprintf(paramfp,
"%1s                                   :direct wave? (y or n)\n",direct);
	if(nhead==0) {
		fprintf(paramfp,
"                                    :headwave interface numbers (1,2,...)\n");
	} else {
		for(ih=0;ih<nhead;ih++) {
			fprintf(paramfp,"%d ",headhn[ih]);
		}
		fprintf(paramfp,
"                                    :headwave interface numbers (1,2,...)\n");
	}
	fprintf(paramfp,
"n                                   :primaries? (y or n)\n");
	for(ih=0;ih<nh;ih++) {
		fprintf(paramfp,
"%-2d                                  :reflection from interface %d\n",
			hn[ih],hn[ih]);
	}

	for(i=0;i<nm;i++) fprintf(paramfp, "%s",mn[i]);
		
	fclose(paramfp);

	/* output param2 file */
	tmp = (r2-r1)/dr + (r4-r3)/dr + 1 + 1 + 0.5;
	nr  = tmp;
	
	paramfp = fopen(param2,"w");
	fprintf(paramfp,
"s                                :job option (s,r)\n");
	fprintf(paramfp,
"1  %d                             :first, last shot for sort\n",ns);
	fprintf(paramfp,
"1  %d                           :first, last trace OR first last receiver\n",
		nr);
	fprintf(paramfp,
"%-4.1f %-4.1f %-4.1f %-4.1f              :frequency spectrum of wavelet\n",
				f1,f2,f3,f4);
	fprintf(paramfp,
"%-4.3f                            :wavelet length (secs)\n",wl*0.001);
	fprintf(paramfp,
"%-4.3f                            :sample rate (secs)\n",dt*0.001);
	fprintf(paramfp,
"%-6.3f                            :record length (secs)\n",tmax*0.001);
	fprintf(paramfp,
"csm_os=%gshot                          :input filename\n",os);
	fprintf(paramfp,
"csm_os=%gtraces                        :output filename\n",os);
	fclose(paramfp);

	/* output plotcolors file */
	sprintf(fname,"plotcolors_os=%g\0",os);
	paramfp = fopen(fname,"w");

	fprintf(paramfp, "0          receivers\n");
	fprintf(paramfp, "4          sources\n");
	fprintf(paramfp, "6          well color\n");
	fprintf(paramfp, "%d          caustic rays\n",ccolor);
	fprintf(paramfp, "%d          rays\n",rcolor);
	fprintf(paramfp, "%d          interfaces\n",icolor);
	fprintf(paramfp,"\n");
	fprintf(paramfp,"\n");
	fprintf(paramfp,"\n");
	fprintf(paramfp,"key:  (CWP's xgraph colors)\n");
	fprintf(paramfp,"0      black\n");
	fprintf(paramfp,"1      white\n");
	fprintf(paramfp,"2      red\n");
	fprintf(paramfp,"3      green\n");
	fprintf(paramfp,"4      dark blue\n");
	fprintf(paramfp,"5      light blue\n");
	fprintf(paramfp,"6      violet\n");
	fprintf(paramfp,"7      yellow\n");
	fclose(paramfp);	


	/* plot rays */
	bzero(cmd,1024);
	sprintf(cmd,"cp %s %s",param1_1,param1);
	system(cmd);
	if(comp==0 || comp==1) {
		bzero(cmd,1024);
		sprintf(cmd,
	"cshot1 param1=%s | cshotplot >csmplot_os=%g outpar=csmpar_os=%g",
			param1,os,os);
		system(cmd);
	}else if(comp==-1) {
		bzero(cmd,1024);
		sprintf(cmd,"cshot1 param1=%s >/dev/null",param1);
		system(cmd);
	}

	if(plot==0) {
		bzero(cmd,1024);
		if(rfsave==0) {
			sprintf(cmd,
"( xgraph <csmplot_os=%g par=csmpar_os=%g style=seismic title='Csmodel raypaths' label1=Depth label2=Distance x1beg=%g x1end=%g x2beg=%g x2end=%g ; /bin/rm -f csmpar_os=%g csmplot_os=%g ) &",
			os,os,zmini,zmaxi,xmini,xmaxi,os,os); 
		} else {
			sprintf(cmd,
"xgraph <csmplot_os=%g par=csmpar_os=%g style=seismic title='Csmodel raypaths' label1=Depth label2=Distance x1beg=%g x1end=%g x2beg=%g x2end=%g &",
			os,os,zmini,zmaxi,xmini,xmaxi); 
		}
	} else if(plot==1) {
		sprintf(cmd,"psgraph <csmplot_os=%g par=csmpar_os=%g title=Csmodel label1=Depth label2=Distance x1beg=%g x1end=%g x2beg=%g x2end=%g | lpr &",
			os,os,zmini,zmaxi,xmini,xmaxi); 
	}

	if(comp!=2 && comp!=-1 && plot!=-1) {
		system(cmd);
		if(plot==1 && rfsave==0) {
			bzero(cmd,1024);
			sprintf(cmd,"/bin/rm -f csmpar_os=%g csmplot_os=%g");
			system(cmd);
		}
	}

	if(comp==0 || comp==-1) goto notrace;

	bzero(cmd,1024);
	sprintf(cmd,"cp %s %s",param1_2,param1);
	system(cmd);
	bzero(cmd,1024);
	sprintf(cmd,"cshot1 param1=%s >/dev/null",param1);
	system(cmd);
	bzero(cmd,1024);
	sprintf(cmd,"cshot2 param2=%s",param2);
	system(cmd);
	nt = 1 + ( tmax + dt / 2. ) / dt;
	j = dt*1000;
	bzero(cmd,1024);
	sprintf(cmd,"suaddhead <csm_os=%gtraces ftn=1 ns=%d | sushw key=dt a=%d >csmtraces.segy_os=%g", os,nt,j,os);
	system(cmd);

	bzero(fname,80);
	sprintf(fname,"csm_os=%gtraces\0",os);
	if(rfsave==0) unlink(fname); 

	bzero(fname,80);
	sprintf(fname,"csmtraces.segy_os=%g\0",os);
	datafp = fopen(fname,"r");
	i = 0;
	dcdp = dr * 0.5;
	if (ds!=0. && ds<dr) dcdp = ds * 0.5; 

	fgethdr(datafp,&ch,&bh);
	nt = nt - 1;
	bh.hns = nt;
	bh.ntrpr = nr;
	bh.tsort = 1;
	bh.hdt = dt * 1000;
	bh.format = 1;
	fputhdr(outfp,&ch,&bh);

	while( fgettr(datafp,&tr) ) {
		i = i + 1;
		js = (i-1)/nr;
		tr.tracl = i;
		tr.sx = (int) sx[js];
		tmp = r1 + (i-js*nr-1)*dr + 0.0001; 
		if(tmp<=r2) {
			tr.gx = (int)tmp + tr.sx;
			j = i;
		} else {
			tmp = r3 + (i-j-1)*dr;
			tr.gx = (int) tmp + tr.sx;
		}
		tr.ep = js + 1;
		tr.fldr = js + 1;
		tr.tracf = i-js*nr;
		tr.offset = tr.gx - tr.sx;
		tmp = tr.gx + tr.sx;
		tmp = tmp * 0.5;
		tmp = tmp/dcdp + 1.5; 
		js  = tmp;
		tr.cdp = js;
		tr.scalco = 1;
		tr.trid = 1;
		tr.delrt = (int) dt;
		tr.ns = nt;
		for(it=0;it<nt;it++) tr.data[it] = tr.data[it+1]; 
		fputtr(outfp,&tr);
	}
	
	if(plot==0) {
		bzero(cmd,1024);
		if(rfsave==0) {
			sprintf(cmd,
	"( <csmtraces.segy_os=%g suxwigb title='Modeled gather' label1=Time label2=Trace xcur=2 grid1=solid perc=99 ; /bin/rm -f csmtraces.segy_os=%g ) & ",os,os,os); 
		} else {
			sprintf(cmd,
	"<csmtraces.segy_os=%g suxwigb title='Modeled gather' label1=Time label2=Trace xcur=2 grid1=solid perc=99 & ",os); 
		}
		system(cmd);
	} else {
		bzero(cmd,1024);
		sprintf(cmd,"<csmtraces.segy_os=%g supswigb title='Modeled gather' label1=Time label2=Trace xcur=2 grid1=solid perc=99 | lpr ",os);
		system(cmd);
		if(rfsave==0) {
			bzero(cmd,1024);
			sprintf(cmd,"/bin/rm -f csmtraces.segy_os=%g ",os);
			system(cmd);
		}
	}


	notrace:

	if(rfsave==0) {
		bzero(fname,80);
		sprintf(fname,"model-file_os=%g\0",os);
		unlink(fname);
		bzero(fname,80);
		sprintf(fname,"geometry-file_os=%g\0",os); 
		unlink(fname);
		unlink(param1_1);
		unlink(param1_2);
		unlink(param1);
		unlink(param2);
		bzero(fname,80);
		sprintf(fname,"plotcolors_os=%g\0",os); 
		unlink(fname);
		if(comp!=0) {
			bzero(fname,80);
			sprintf(fname,"csm_os=%gshot\0",os); 
			unlink(fname);
		}
		bzero(fname,80);
		sprintf(fname,"csm_os=%gdata\0",os); 
		unlink(fname);
		bzero(fname,80);
		sprintf(fname,"csm_os=%glisting\0",os); 
		unlink(fname);

		system("sleep 5");
		if(comp==1 || comp==2) {
			bzero(fname,80);
			sprintf(fname,"csmtraces.segy_os=%g\0",os); 
			unlink(fname);
		}
	}

	return(0);
}
