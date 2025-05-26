/* quality control of velocity 3d cards */

#include "par.h"



char *sdoc = 
"QCVPRINT - quality control print of VS3D cards \n"
"\n"
"qcvprint [parameters] xqc= yqc= vs3d1= vs3d2= >print.file		\n" 
"\n"
"Required parameters:						 	\n"
"xqc=              x coordinate (inline) of VS3D to qc 			\n"
"yqc=              y coordinate (crossline) of VS3D to qc 		\n"
"\n"
"vs3d1=            Name of first VS3D dataset to qc 			\n"
"vs3d2=            Name of second VS3D (checkshot) dataset to qc	\n"
"print.file=       Name of print file 					\n"
"Optional parameters:							\n"
"ft=0              time of first sample (in ms) to print qc results 	\n"
"dt=100            time interval (in ms) to print qc results 		\n"
"nt=100            time interval (in ms) to print qc results 		\n"
"vstype1=0         Velocity type of VS3D card (0=rms 1=avg)		\n"
"vstype2=0         Velocity type of VS3D card (0=rms 1=avg)		\n"
"nvfmax=4096       maximum number of velocity functions each vs3d data	\n"
"                  set may have						\n"
"ntvmax=256        maximum number of t-v pairs per velocity function in \n"
"                  vs3d datasets					\n"
"fxline=0          first xline (cross line number) number		\n"
"fx=0              x at fxline 						\n"  
"dxline=0          xline increment 					\n"
"dx=0              x increment of xline		 			\n"  
"fsline=0          first sline (sail line number) number		\n"
"fy=0              y at fsline 						\n"  
"dsline=0          sline increment 					\n"
"dy=0              y increment of sline					\n"  
"                  (only when the above 8 parameters are given, will 	\n"
"                   xline and sline be printed)				\n"
"\n"
"AUTHOR:	   Zhiming Li,       ,			10/2/94		\n"    
;

void vs3dread(FILE *infp, float *xs, float *ys, float *ts, float *vs, 
	int *nxy, int *nps, int maxp, int maxnxy);

void vrms2vint(float *t, float *v, int ntv, float *time, float *vi, int nt);

void vavg2vint(float *t, float *v, int ntv, float *time, float *vi, int nt);

void vint2vrms(float *t, float *vi, float *vrms, int nt);

void vint2vavg(float *t, float *vi, float *va, int nt);

void findvf(char *in, float xqc, float yqc, float *t, float *v, int *np, 
	float *xx, float *yy, int ntvmax, int nvfmax); 

main(int argc, char **argv)
{
    	FILE *vfp1, *vfp2, *outfp=stdout;

	char *in1, *in2;
	int it, i1, i2;
	float xqc, yqc, dt, ft; 
	int vstype1, vstype2, nt;
    	float *time;
	float *vi1, *vi2;
	float *vr1, *vr2;
	float *va1, *va2;
	int n1s, n2s;

	float *t1, *v1, *t2, *v2;
	float x1, y1, x2, y2;
	int np1=0, np2=0;
	float tmp;
	int ntvmax,nvfmax;

	float fx,fy,dx,dy;
	int ix, iy, ixy=0;
	float fxline,fsline,dxline,dsline;
	
	
	initargs(argc,argv);
        askdoc(1);

	/* required parameters */
	if (!getparfloat("xqc",&xqc)) err(" xqc missing ");
	if (!getparfloat("yqc",&yqc)) err(" yqc missing ");

	/* optional parameters */

	if (!getparstring("vs3d1",&in1)) err(" vs3d1 missing");
	if (!getparstring("vs3d2",&in2)) err(" vs3d2 missing");
	vfp1 = efopen(in1,"r");
	vfp2 = efopen(in2,"r");

	if (!getparint("vstype1",&vstype1)) vstype1 = 0;
	if (!getparint("vstype2",&vstype2)) vstype2 = 0;
	if (!getparint("nvfmax",&nvfmax)) nvfmax = 4096;
	if (!getparint("ntvmax",&ntvmax)) ntvmax = 256;
	if (!getparfloat("dt",&dt)) dt = 100.;
	if (!getparfloat("ft",&ft)) ft = 0.;
	if (!getparint("nt",&nt)) nt = 100;

	if( getparfloat("fx",&fx) &&
	    getparfloat("fy",&fy) &&
	    getparfloat("dx",&dx) &&
	    getparfloat("dy",&dy) &&
	    getparfloat("dxline",&dxline) &&
	    getparfloat("dsline",&dsline) &&
	    getparfloat("fxline",&fxline) &&
	    getparfloat("fsline",&fsline) ) ixy=1;

	time = (float*) malloc(nt*sizeof(float));
	for(it=0;it<nt;it++) time[it] = ft + it*dt;

	t1 = (float*) malloc(ntvmax*sizeof(float));
	t2 = (float*) malloc(ntvmax*sizeof(float));
	v1 = (float*) malloc(ntvmax*sizeof(float));
	v2 = (float*) malloc(ntvmax*sizeof(float));

	/* read in t, v at closest location of data in1 */
	findvf(in1, xqc, yqc, t1, v1, &np1, &x1, &y1, ntvmax, nvfmax); 
	vi1 = (float*) malloc(nt*sizeof(float));
	vr1 = (float*) malloc(nt*sizeof(float));
	va1 = (float*) malloc(nt*sizeof(float));
	if(vstype1==0) {
		vrms2vint(t1, v1, np1, time, vi1, nt);
	} else if(vstype1==1) {
		vavg2vint(t1, v1, np1, time, vi1, nt);
	} 
	n1s = nt;
	vint2vrms(time, vi1, vr1, n1s);
	vint2vavg(time, vi1, va1, n1s);

	/* read in t, v at closest location of data in2 */
	findvf(in2, xqc, yqc, t2, v2, &np2, &x2, &y2, ntvmax, nvfmax); 
	vi2 = (float*) malloc(nt*sizeof(float));
	vr2 = (float*) malloc(nt*sizeof(float));
	va2 = (float*) malloc(nt*sizeof(float));
	if(vstype2==0) {
		vrms2vint(t2, v2, np2, time, vi2, nt);
	} else if (vstype2==1) {
		vavg2vint(t2, v2, np2, time, vi2, nt);
	}
	n2s = nt;
	vint2vrms(time, vi2, vr2, n2s);
	vint2vavg(time, vi2, va2, n2s);

	if(ixy==0) {
	fprintf(outfp," \n");
	fprintf(outfp," Velocity Q.C. Print at xqc=%g yqc=%g \n",xqc,yqc); 
	fprintf(outfp," \n");
	fprintf(outfp," Closest Location in vs3d1=%s : \n",in1);
	fprintf(outfp,"         x1=%g y1=%g \n",x1,y1);
	fprintf(outfp," Closest Location in vs3d2=%s : \n",in2);
	fprintf(outfp,"         x2=%g y2=%g \n",x2,y2);
	fprintf(outfp," \n");
	} else {
	fprintf(outfp," \n");
	tmp = (xqc - fx)/dx*dxline + fxline + 0.5;
	ix = tmp;
	tmp = (yqc - fy)/dy*dsline + fsline + 0.5;
	iy = tmp;
	fprintf(outfp,
	" Velocity Q.C. Print at xqc=%g (xline=%d) yqc=%g (sline=%d) \n",
		xqc,ix,yqc,iy); 
	fprintf(outfp," \n");
	tmp = (x1 - fx)/dx*dxline + fxline + 0.5;
	ix = tmp;
	tmp = (y1 - fy)/dy*dsline + fsline + 0.5;
	iy = tmp;
	fprintf(outfp, " Closest Location in vs3d1=%s : \n",in1);
	fprintf(outfp, "         x1=%g (xline=%d) y1=%g (sline=%d) \n",
			x1,ix,y1,iy);
	tmp = (x2 - fx)/dx*dxline + fxline + 0.5;
	ix = tmp;
	tmp = (y2 - fy)/dy*dsline + fsline + 0.5;
	iy = tmp;
	fprintf(outfp, " Closest Location in vs3d2=%s : \n",in2);
	fprintf(outfp, "         x2=%g (xline=%d) y2=%g (sline=%d) \n",
			x2,ix,y2,iy);
	fprintf(outfp," \n");
	}
fprintf(outfp,
"                        VS3D1                         VS3D2             \n");
fprintf(outfp,
"------------------------------------------------------------------------\n");
fprintf(outfp,
"    Time      Vrms      Vavg      Vint      Vrms      Vavg      Vint    \n");
fprintf(outfp,
"  --------  --------  --------  --------  --------  --------  --------  \n");

	for(it=0;it<nt;it++) {
		fprintf(outfp,
		"  %8.3g  %8.3g  %8.3g  %8.3g  %8.3g  %8.3g %8.3g  \n",
		time[it],vr1[it],va1[it],vi1[it],vr2[it],va2[it],vi2[it]);
	}

	exit(0);

}
	

void vint2vrms(float *t, float *vi, float *vrms, int nt) {

	int i1;
	
	vrms[0] = vi[0]*vi[0]*t[0];
	for(i1=1;i1<nt;i1++) {
		vrms[i1] = vrms[i1-1] + vi[i1]*vi[i1]*(t[i1]-t[i1-1]);
	}
	for(i1=1;i1<nt;i1++) {
		vrms[i1] = sqrt(vrms[i1]/t[i1]);
	}
	vrms[0] = vi[0];
}

void vint2vavg(float *t, float *vi, float *vavg, int nt) {

	int i1;
	
	vavg[0] = vi[0]*t[0];
	for(i1=1;i1<nt;i1++) {
		vavg[i1] = vavg[i1-1] + vi[i1]*(t[i1]-t[i1-1]);
	}
	for(i1=1;i1<nt;i1++) {
		vavg[i1] = vavg[i1]/t[i1];
	}
	vavg[0] = vi[0];
}


void vrms2vint(float *t, float *v, int ntv, float *time, float *vi, int nt) {

	float *work, tmax;
	int *indx;
	int i1, j;


	work = (float*) emalloc(nt*sizeof(float));
	indx = (int*) emalloc(nt*sizeof(int));


	/* linearly interpolate v to nt times */
	lin1d_(t,v,&ntv,time,vi,&nt,indx);

	/* compute interval velocities via dix formula */
	work[0] = vi[0]*vi[0];
	tmax = t[ntv-1];

	for(j=1;j<nt;j++) {
		if(time[j]<=tmax) { 
			work[j] = (vi[j]*vi[j]*time[j] -  	
		   		vi[j-1]*vi[j-1]*time[j-1]) / 
		  		(time[j]-time[j-1]);
		} else {
			work[j] = work[j-1];
		}

	}
	for(j=0;j<nt;j++) {
		if(work[j]<0.) err(" Negative Interval Velocity "); 
		vi[j] = sqrt(work[j]);
	}

	free(indx);
	free(work);

}

void vavg2vint(float *t, float *v, int ntv, float *time, float *vi, int nt) {

	float *work, tmax;
	int *indx;
	int i1, j;


	work = (float*) emalloc(nt*sizeof(float));
	indx = (int*) emalloc(nt*sizeof(int));


	/* linearly interpolate v to nt times */
	lin1d_(t,v,&ntv,time,vi,&nt,indx);

	/* compute interval velocities via dix formula */
	work[0] = vi[0];
	tmax = t[ntv-1];

	for(j=1;j<nt;j++) {
		if(time[j]<=tmax) { 
			work[j] = (vi[j]*time[j] -  
		   		vi[j-1]*time[j-1]) / 
		  		(time[j]-time[j-1]);
		} else {
			work[j] = work[j-1];
		}

	}
	for(j=0;j<nt;j++) {
		if(work[j]<0.) err(" Negative Interval Velocity "); 
		vi[j] = work[j];
	}

	free(indx);
	free(work);

}

/* read in velocity picks from input file pointer infp (VS3D card dataset) */
/* input:								*/
/*	infp			file pointer for VELF card dataset 	*/
/*	maxp			maximum number of t-v pairs per (x,y) 	*/
/*	maxnxy			maximum number of (x,y)'s of VS3D cards	*/
/* ouput:								*/
/*	xs[maxnxy]		x location of VS3D card 		*/	
/*	ys[maxnxy]		y location of VS3D card 		*/	
/*	ts[maxnxy][maxp]	time of picks (ms)			*/
/*	vs[maxnxy][maxp]	velocity of picks			*/
/*	nxy			total number of (x,y)'s found in VS3D cards*/
/*	nps[maxnxy]		number t-v pairs per (x,y)		*/
/*									*/	
/* author:	Zhiming Li		6/92	      			*/

void vs3dread(FILE *infp, float *xs, float *ys, float *ts, float *vs, 
	int *nxy, int *nps, int maxp, int maxnxy) {

     	int icmax, ic, cdpnow=0, cdppre=-1, cdpchange=0, jc=0;
	int i1, i2, i3, i;
	char *cbuf, x[8], y[8], velo[8], time[8];
	int cardfound=0;
	float xnow, ynow, xpre, ypre;

	icmax = maxnxy * maxp;

	cbuf = (char *) malloc(81*sizeof(char));

	/* rewind infp */
	efseek(infp,0,0);


	for (ic=0;ic<icmax;ic++) {
		if(feof(infp) !=0) break;
		bzero(cbuf,81);
                fgets(cbuf,81,infp);

                if(strncmp(cbuf, "VS3D",4)==0) {
			cardfound = 1;

			strncpy(x,&cbuf[8],8);
                	if(strncmp(x, "        ",8)!=0) {
                		xnow = atof(x);
			} else {
				xnow = xpre;
			}
			strncpy(y,&cbuf[16],8);
                	if(strncmp(y, "        ",8)!=0) {
                		ynow = atof(y);
			} else {
				ynow = ypre;
			}

			if(cdppre == -1) {
				xpre = xnow;
				ypre = ynow;
				cdppre = 0;
			}

		 	if( (xpre==xnow && ypre==ynow) ) {
				cdpnow = cdppre;
			} else {
				cdpnow = cdppre + 1;
			}
		
			/* if cdp changes */
          		if (cdpnow != cdppre ) {
				if(cdpchange>=maxnxy)
                                err(" maximum number of functions exceeded ");
                                if(jc>maxp)
                                err(" maximum number of t-v pairs exceeded ");
             			nps[cdpchange] = jc;
				xs[cdpchange] = xpre;
				ys[cdpchange] = ypre;
				cdpchange += 1;
				jc = 0;
             			cdppre = cdpnow;
				xpre = xnow;
				ypre = ynow;
          		}

			/* store read values in tpicks and vpicks arrays */
          		for(i=0;i<3;i++) {

				strncpy(time,&cbuf[24+i*16],8);
                        	i2 = atoi(time);
				strncpy(velo,&cbuf[32+i*16],8);
                        	i3 = atoi(velo);

				if(i3==0) break;

             			ts[jc+cdpchange*maxp] = i2;
             			vs[jc+cdpchange*maxp] = i3;
             			jc = jc + 1;
          		}
		}
	}
/* last input cdp location */
	if(cardfound==1) {
    		xs[cdpchange] = xpre;
    		ys[cdpchange] = ypre;
    		nps[cdpchange] = jc;
		*nxy = cdpchange + 1;
	} else {
		*nxy = 0;
	}
	if(*nxy>maxnxy) 
		err("number of (x,y) of VS3D cards exceeds %d \n",maxnxy);
	free(cbuf);
}

/* find closest VS3D card to location (xqc,yqc) */ 
/* t and v are pre-allocated arrays of length longer than 256 */
/*  
    input:
	in= 	file name of vs3d cards
	xqc=    x coordinate of qc position
	yqc=    y coordinate of qc position
	ntvmax= maximum number of t-v pairs per velocity function
	nvfmax= maximum number of velocity functions in the input
    output:
	t=      times of closest vs3d card
	v=      velocities of closest vs3d card
	np=     number of t-v pairs at this locations
	xx=     x coorinate of the closest vs3d card
	yy=     y coorinate of the closest vs3d card

    author:		z. li		      		9/14/93 
*/
	

void findvf(char *in, float xqc, float yqc, float *t, float *v, int *np, 
	float *xx, float *yy, int ntvmax, int nvfmax) { 


	int n1, n2, nxy;
    	float *xs, *ys, *tpicks, *vpicks;
    	int *nps;
	FILE *infp;
	float dis, x, y, tmp;
	int ip, i1;

	infp = efopen(in,"r");

    	/* at most 4096 input (x,y) VS3D cards with at most 256 time-vel 
		pairs each */

    	n1 = ntvmax;
    	n2 = nvfmax;

    	/* arrays used to store all VELF card's cdp, time and velocity */
    	xs = (float*)malloc(n2*sizeof(int));
    	ys = (float*)malloc(n2*sizeof(int));
    	tpicks = (float*)malloc(n1*n2*sizeof(float));
    	vpicks = (float*)malloc(n1*n2*sizeof(float));
    	nps = (int*)malloc(n2*sizeof(int));
	bzero(nps,n2*sizeof(int));

    	/* read in VS3D cards */
    	nxy = 0;
	vs3dread(infp,xs,ys,tpicks,vpicks,&nxy,nps,n1,n2);
   	if (nxy==0) err("No VS3D card input for in=%s ! Job aborted",in);
	x = xs[0];
	y = ys[0];
	dis = sqrt((xqc-x)*(xqc-x)+(yqc-y)*(yqc-y));
	*np = nps[0];
	*xx = x;
	*yy = y;
	for(i1=0;i1<*np;i1++) {
		t[i1] = tpicks[i1];
		v[i1] = vpicks[i1];
	}

	for(ip=1;ip<nxy;ip++) {
		x = xs[ip];
		y = ys[ip];
		tmp = sqrt((xqc-x)*(xqc-x)+(yqc-y)*(yqc-y));
		if(tmp<dis) {
			dis = tmp;
			*np = nps[ip];
			for(i1=0;i1<*np;i1++) {
				t[i1] = tpicks[ip*n1+i1];
				v[i1] = vpicks[ip*n1+i1];
			}
			*xx = x;
			*yy = y;
		}
	}
			
	fprintf(stderr,
	"VS3D card found for %s at x=%g y=%g location for %d t-v pairs \n",
			in,*xx,*yy,*np); 

	free(nps);
	free(tpicks);
	free(vpicks);	
	free(xs);
	free(ys);
	fclose(infp);

}
