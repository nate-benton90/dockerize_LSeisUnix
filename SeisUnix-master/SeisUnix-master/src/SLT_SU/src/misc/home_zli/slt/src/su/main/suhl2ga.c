#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUHL2GA - create a synthetic gather from AVO highlights 	\n\
								\n\
suhl2ga [optional parameters] > out_data_file  			\n\
								\n\
Optional parameters:						\n\
    fpeak=40        peak frequency of Ricker wavelet (Hz)	\n\
                    when fpeak=0, no wavelet		\n\
    dt=4            sampling interval (ms)			\n\
    t0=1900         time of first output sample (ms)	\n\
    nt=50           number of output time samples		\n\
    ofmin=250       minimum output offset			\n\
					=999999 and nof=1 will output h3 trace in time \n\
    dof=100         offset increment			\n\
    nof=30          number of output offsets 		\n\
    cdp=1           output cdp number			\n\
    depth=2000      reflector's depth in m or ft		\n\
    va=2000         average velocity at reflector		\n\
    r0=0.1          zero-offset amplitude of avo highlights 	\n\
    h3=0.1          poisson reflectivity or avo hightlight h3 	\n\
    logs=           name of the files containing multiple rows of \n\
                    depth, va, r0 and h3 in ascii format		\n\
                    when this is given, depth, va, r0 and h3 are 	\n\
                    ignored								\n\
    kb=0            the depth of logs is measured from kb	\n\
    r0.trace=       name of dataset containing traces of r0 in time	\n\
    h3.trace=       name of dataset containing traces of h3 in time	\n\
    va.trace=       name of dataset containing traces of va in time	\n\
            	    when r0.trace, h3.trace and va.trace are given,	\n\
                    the following parameter will be set to:	\n\
                      fpeak=0      ---- no wavelet			\n\
                      dt=tr.dt     ---- same as dt in r0.trace 		\n\
                      t0=tr.delrt  ---- same as t0 in r0.trace 		\n\
                      nt=tr.ns     ---- same as nt in r0.trace 		\n\
                      cdp=tr.cdp   ---- same as cdp in r0.trace \n\
                    \n\
Author: 		Z. Li	      	2-5-96			\n\
";
/**************** end self doc ***********************************/

typedef struct WaveletStruct {
    int lw;         /* length of wavelet */
    int iw;         /* index of first wavelet sample */
    float *wv;      /* wavelet sample values */
} Wavelet;

void makericker (float fpeak, float dt, Wavelet **w);
void addsinc (float time, float amp,int nt, float dt, float ft, float *trace);

segychdr ch;
segybhdr bh;
segy tr, trr, trh, trv;

main(int argc, char **argv)
{
	float dt,offset;
	float fpeak, ofmin, dof, t0;
	float *depth, *r0, *h3, *va, *data;
	int nof, nt, cdp, iz;
	float kb;

	float a, t, tmp, *trace;
	int i, it, iw, lw, jt, nz, nzmax;
	int itrace;
	char *logs;
	FILE *fp;
	int ilogs;
	char *cbuf;
	char *r0trace, *h3trace, *vatrace;
	FILE *rfp, *hfp, *vfp;
	float *time, *vrms, *vi, *v;
	int ntv, itmp, one=1;

	Wavelet *w;

	/* Initialize */
	initargs(argc, argv);
	askdoc(0); /* stdin not used */


	ofmin = 250.;	getparfloat("ofmin", &ofmin);
	dof = 100.;	getparfloat("dof", &dof);
	nof = 30;	getparint("nof", &nof);

	if(getparstring("r0.trace",&r0trace) &&
	   getparstring("h3.trace",&h3trace) &&
	   getparstring("va.trace",&vatrace) ) {

		itrace = 1;
		rfp = efopen(r0trace,"r");
		hfp = efopen(h3trace,"r");
		vfp = efopen(vatrace,"r");
		fgethdr(rfp,&ch,&bh);
		bh.fold = nof;
		bh.tsort = 2;
		fseek2g(rfp,3600,0);
		fseek2g(hfp,3600,0);
		fseek2g(vfp,3600,0);
		fgettr(rfp,&trr);
		fgettr(hfp,&trh);
		fgettr(vfp,&trv);
		if(trr.delrt!=trh.delrt || trr.ns!=trh.ns || trr.dt!=trh.dt ||
		   trr.delrt!=trv.delrt || trr.ns!=trv.ns || trr.dt!=trv.dt )
			err("check t0, dt, nt in r0.trace, h3.trace and va.trace ");
		puthdr(&ch,&bh);

		t0 = trr.delrt;
		dt = (float)trr.dt/1000.;
		nt = trr.ns;

		do {
			bcopy(&trr,&tr,240);
			for (i=0;i<nof;i++) {
				offset = ofmin + i * dof;
				tr.offset = offset;
				tr.cdpt = i + 1;
				if(ofmin==999999. && nof==1) {
					for(it=0;it<nt;it++)
						tr.data[it] = trh.data[it];
				} else {
					for(it=0;it<nt;it++) {
						tmp = trv.data[it]*(t0+it*dt)*0.001*0.5;
						if(tmp==0.) tmp = 0.000001*offset;
						tmp = atan(offset/tmp/2.);
						tr.data[it] = trr.data[it]*cos(tmp)*cos(tmp)+
							trh.data[it]*sin(tmp)*sin(tmp);
					} 
				}
				puttr(&tr);
			}
		} while(fgettr(rfp,&trr) && fgettr(hfp,&trh) && fgettr(vfp,&trv));
	} else {
		itrace = 0;
		fpeak = 40.;	getparfloat("fpeak", &fpeak);
		dt = 4;		getparfloat("dt", &dt);	tr.dt = dt*1000;
		dt = dt * 0.001;
		cdp = 1;	getparint("cdp", &cdp); tr.cdp  = 1;
		t0 = 1900;	getparfloat("t0", &t0);	tr.delrt = t0;
		t0 = t0 * 0.001;
		nt = 50;	getparint("nt", &nt);	tr.ns = nt;

		if(getparstring("logs",&logs)) {
			ilogs = 1;
			fp  = efopen(logs,"r");
			cbuf = (char*) malloc(81*sizeof(char));
			if(!getparfloat("kb",&kb)) kb=0.;
			fgets(cbuf,80,fp);
			i = 0;
			nzmax = 4096*128;
			depth = (float*) malloc(nzmax*sizeof(float));
			va = (float*) malloc(nzmax*sizeof(float));
			r0 = (float*) malloc(nzmax*sizeof(float));
			h3 = (float*) malloc(nzmax*sizeof(float));
			do {
				sscanf(&cbuf[0],"%g %g %g %g",&depth[i],&va[i],&r0[i],&h3[i]);
				depth[i] -= kb;
				i = i + 1;
				if(i>nzmax) err(" number of depth levels exceeds %d \n",nzmax);
			} while (fgets(cbuf,80,fp));
			nz = i;
		} else {
			nz = 1;
			depth = (float*) malloc(nz*sizeof(float));
			va = (float*) malloc(nz*sizeof(float));
			r0 = (float*) malloc(nz*sizeof(float));
			h3 = (float*) malloc(nz*sizeof(float));
			depth[0] = 2000.;	getparfloat("depth", &depth[0]);
			va[0] = 2000.;	getparfloat("va", &va[0]);
			r0[0] = 0.1;	getparfloat("r0", &r0[0]);
			h3[0] = 0.1;	getparfloat("h3", &h3[0]);
		}

		if( (2.*depth[nz-1]/va[nz-1]) < t0 ) err(" t0 too large ");
		t = ( depth[nz-1] * 2. / va[nz-1] - t0) / dt;
		it = t;
		if(it<0 || it>nt-1) 
		err(" check t0, dt and nt for reflection t=%g \n",(t*dt+t0)*1000.);
		/* create id headers and output */
   		idhdrs(&ch,&bh,nt);
		bh.hns = nt;
		bh.hdt = dt*1000000;
		bh.fold = nof;
		bh.tsort = 2;
		puthdr(&ch,&bh);
		if(fpeak>0.) makericker(fpeak,dt,&w);

		trace = (float*) emalloc(nt*sizeof(float));
		data = (float*) emalloc(nof*nt*sizeof(float));


		for(i=0;i<nof;i++) {
			offset = ofmin + i * dof;
			for(it=0;it<nt;it++) data[it+i*nt] = 0.;
			for(iz=0;iz<nz;iz++) {
				for(it=0;it<nt;it++) trace[it] = 0.;
				t = depth[iz]*2./va[iz];
				tmp = atan(offset/depth[iz]/2.);
				if(ofmin==999999. & nof==1) {
					a = h3[iz];
				} else { 
					a = r0[iz]*cos(tmp)*cos(tmp)+h3[iz]*sin(tmp)*sin(tmp);
				}
				/* add sinc wavelet to trace */
				addsinc(t,a,nt,dt,t0,trace);
				for(it=0;it<nt;it++) {
					data[it+i*nt] += trace[it]; 
				}
			}
		}

		for(i=0;i<nof;i++) {
			offset = ofmin + i * dof;
			tr.offset = offset;
			tr.cdpt = i + 1;
			/* convolve wavelet with trace */
			for(it=0;it<nt;it++) tr.data[it] = 0.;
			if(fpeak>0.) { 
				conv(w->lw,w->iw,w->wv,nt,0,data+i*nt,nt,0,tr.data);
			} else {
				for(it=0;it<nt;it++) tr.data[it] = data[it+i*nt];
			}
			puttr(&tr);	
		}
		free(data);
		free(trace);
	}

	return EXIT_SUCCESS;
}

void makericker (float fpeak, float dt, Wavelet **w)
/*****************************************************************************
Make Ricker wavelet
******************************************************************************
Input:
fpeak       peak frequency of wavelet (in Hz)
dt          time sampling interval (in sec)

Output:
w       Ricker wavelet
*****************************************************************************/
{
    int iw,lw,it,jt;
    float t,x,*wv;
   
    iw = -(1+1.0/(fpeak*dt));
    lw = 1-2*iw;
    wv = ealloc1float(lw);
    for (it=iw,jt=0,t=it*dt; jt<lw; ++it,++jt,t+=dt) {
        x = PI*fpeak*t;
        x = x*x;
        wv[jt] = exp(-x)*(1.0-2.0*x);
    }
    *w = ealloc1(1,sizeof(Wavelet));
    (*w)->lw = lw;
    (*w)->iw = iw;
    (*w)->wv = wv;
}

void addsinc (float time, float amp,
	int nt, float dt, float ft, float *trace)
/*****************************************************************************
	Add sinc wavelet to trace at specified time and with specified amplitude
******************************************************************************
	Input:
	time        time at which to center sinc wavelet
	amp     peak amplitude of sinc wavelet
	nt      number of time samples
	dt      time sampling interval
	ft      first time sample
	trace       array[nt] containing sample values

	Output:
	trace       array[nt] with sinc added to sample values
	**********************************************************************/
{
    static float sinc[101][8];
	static int nsinc=101,madesinc=0;
	int jsinc;
	float frac;
	int itlo,ithi,it,jt;
	float tn,*psinc;

	/* if not made sinc coefficients, make them */
	if (!madesinc) {
			for (jsinc=1; jsinc<nsinc-1; ++jsinc) {
				frac = (float)jsinc/(float)(nsinc-1);
				mksinc(frac,8,sinc[jsinc]);
			}
			for (jsinc=0; jsinc<8; ++jsinc)
				sinc[0][jsinc] = sinc[nsinc-1][jsinc] = 0.0;
			sinc[0][3] = 1.0;
			sinc[nsinc-1][4] = 1.0;
			madesinc = 1;
	}
	if(time>=ft) {
		tn = (time-ft)/dt;
		jt = tn;
		jsinc = (tn-jt)*(nsinc-1);
		itlo = jt-3;
		ithi = jt+4;
		if (itlo>=0 && ithi<nt) {
			psinc = sinc[jsinc];
			trace[itlo] += amp*psinc[0];
			trace[itlo+1] += amp*psinc[1];
			trace[itlo+2] += amp*psinc[2];
        	trace[itlo+3] += amp*psinc[3];
			trace[itlo+4] += amp*psinc[4];
			trace[itlo+5] += amp*psinc[5];
			trace[itlo+6] += amp*psinc[6];
			trace[itlo+7] += amp*psinc[7];
		} else if (ithi>=0 && itlo<nt) {
        	if (itlo<0) itlo = 0;
			if (ithi>=nt) ithi = nt-1;
			psinc = sinc[jsinc]+itlo-jt+3;
        	for (it=itlo; it<=ithi; ++it)
				trace[it] += amp*(*psinc++);
		}
	}
}
