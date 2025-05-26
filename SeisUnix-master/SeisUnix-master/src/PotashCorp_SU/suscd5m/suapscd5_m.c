/* sapscd5_m.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#define MS	5000
#define MG	10000
#define MH	3000
#define MY	100000
#define MN      900000
#define MZ	720
#define BLANK	-9999
#define AMPSP(c) rcabs(c)
#define PHSSP(c) atan2(c.i,c.r)
#define TINY log(FLT_EPSILON)

/*********************** self documentation *****************************/
char *sdoc[] = {
" SUAPSCD5_m - apply surface consistent deconvolution solutions         ",
"                                                                       ",
" suscd < stdin	> stdout                                                ",
"                                                                       ",
" Required parameters:                                                  ",
"        none                                                           ",
"                                                                       ",
" Optional parameters:                                                  ",
"	fnl=0     	Smoothing filter length			        ",
"	fmin=		Minimum frequency to restore			",
"	fmax=		Maximum frequency to restore			",
"	fftpad=25.0	% of the trace for fft padding			",
"       prw=0.01	Pre withening                                   ",
"                                                                       ",
"       ltpr=7.0 	Low end frequency taper in Hz                   ",
"       htpr=10.0       High end frequency taper in Hz                  ",
"                                                                       ",
"                       Default range of log spectra files              ",
"	fns=s.lgs	Name of file with shot residual log spectra	",
"	fng=g.lgs     	Name of file with receiver residual log spectra ",
"	fnh=h.lgs    	Name of file with offset residual log spectra   ",
"	fny=y.lgs     	Name of file with CDP residual log spectra      ",
"	fnz=z.lgs     	Name of file with azimuth residual log spectra  ",
"	fna=a.lgs     	Name of file with average amplitude spectra     ",
"	s=1		=0 Not to apply shot componenet		        ",
"	g=1		=0 Not to apply receiver componenet	        ",
"	h=1		=0 Not to apply offset componenet		",
"	y=0		=0 Not to apply cdp componenet		        ",
"	z=1		=0 Not to apply azimuth componenet		",
"	sp=0		=1 Minimum phase shot component                 ",
"	gp=0		=0 Minimum phase receiver component             ",
"	hp=0		=0 Minimum phase offset componenet              ",
"	yp=0		=0 Minimum phase CDP componenet                 ",
"	zp=0		=0 Minimum phase azimuth component              ",
"	ap=0		=0 Minimum phase average component              ",
"       hb=15		Size of offset bins (should be same as in suscd5_m1)",
"       zb=5		Size of azimuth bins (should be same as in suscd5_m1)",
"	                                                         	",
"       SMOOTHING                                                       ",
"       smt=4           Degree of smoothing of input trace spectra in Hz",
"	                This only effects the residual computation	",
"	res=0		1 Remove the residual between the model and a 	",
"	                real trace as a zero phase component     	",
"                                                                       ",
"       time=0		If zero the deconvolution is done in frequency  ",
"                       domain. To do a predcitive decon time has to    ",
"                       be set to 1                                     ",
"       lag=dt                                                          ",
"       oplen=(tmax-tmin)/20                                            ",
"                                                                       ",
"                                                                       ",
"                                                                       ",
NULL};

#define LOOKFAC 2       /* Look ahead factor for npfaro   */
#define PFA_MAX 720720  /* Largest allowed nfft           */

/* Functions */
void scf(FILE *fp,int *NT,cwp_String key,float bin);
int fval(int val,int *NT);
void taper(float *data, int n, float *ltaper, int lntr,
           float *htaper, int hntr, int st, int en);
void spcfk(complex *w,int nfftc);
void pre_w(float *a,int n,float p);

/* Segy data constans */
static segy tr,trtmp;				/* SEGY trace */

int main( int argc, char *argv[] )
{

	int *ST;	        /* shots file table*/
	int *GT;	        /* receivers file table*/
	int *HT;	        /* offset file table */
	int *YT;	        /* cdps file table */
	int *ZT;	        /* cdps file table */
	
	int s;			/* Flags which component apply */
	int g;			
	int h;
	int y;
	int z;
	int a;
	int sp;			/* Flags phase of  component */
	int gp;			
	int hp;
	int yp;
	int zp;
	int ap;
	
	int res;		/* residual removal flag */
	
	FILE *fps;		/* shot spectra file pointer */		
	FILE *fpg;		/* receiver spectra file pointer */
	FILE *fph;		/* offset spectra file pointer */
	FILE *fpy;		/* CDP spectra file pointer */
	FILE *fpz;		/* Azimuth spectra file pointer */
	FILE *fpa;		/* average spectra file pointer */
	char *fns;		/* file names */
	char *fng;
	char *fnh;
	char *fny;
	char *fnz;
	char *fna;
	
	
	FILE *efp=NULL;		/* Diagnostic file */
	

	float *rt;     /* real trace                           */
        complex *spc;  /* complex transformed trace            */
        complex *spcinv;  /* complex transformed trace            */
        complex *spcav;  /* complex transformed trace of the average wavelet */
        complex *spcrs;  /* complex transformed trace of the residuals */
        int nfft;
	float snfft;
	int nf;            /* nfft/2 + 1                           */

	float fmin;
	float fmax;
	float fftpad;
	float ltpr;		/* Low end taper in Hz */
	float htpr;		/* High end taper in Hz */
	int iltpr;		/* Low end taper in freq samples */
	int ihtpr;		/* High end taper in freq samples */
	float ffmin;		/*mimimum frequency value stored in spc files*/
	float ffmax;		/*maximum frequency values stored in spc files*/
	float ffd1;		/* frequency smapling in files */
	int ffn;		/* Number of frequency samples in files*/
	float *ffv;		/* Frequency values */
	int  flimi;
	int  flima;
	int fn;		/* Number of frequency smaples */
	float *fv;		/* Frequency values */
	float *ltpra;		/* taper values */
	float *htpra;		/* taper values */
	float st;

	/* bin sizes */
	float hb;		/* offset bin size */
	float zb;		/* azimuth bin size */

	float *pwt;		/* data residuals spectra */
	float *pwat;		/* data residuals spectra accumulated */
	float *aw;		/* averaged spectra */
	float *amp;		/* amplitude spectra */
	float *ph;		/* phase spectra */
	float *ar;		/* amplitude spectra */
	float *phr;		/* phase spectra */
	float d1;
	float dt;
	int nt;
	
	float *aspt;		/* amplitude spectra of the trace */
	
	
	float *aspa;
	float *aspg;
	float *asps;
	float *aspy;
	float *asph;
	float *aspz;
	float *aspe;

	int i;			/* counter */
	int ti;			/* trace index */
	float prw;		/* pre whithening */
	float chi_err;		/* rms error */
	
	float smt;		/* trace spectra smoother in Hz */
	int  ismt;		/* trace spectra smoother in smaples */
	float *filter;	        /* smoothing filter */
	
	
	
	/* Time domain solution parameters */
	int time;
	float lag;
	float oplen;
	
	int verbose=0;

	initargs(argc, argv);
   	requestdoc(1);

	if(!getparstring("fns",&fns)) fns="s.lgs";
 	if(!getparstring("fng",&fng)) fng="g.lgs";
 	if(!getparstring("fnh",&fnh)) fnh="h.lgs";
 	if(!getparstring("fny",&fny)) fny="y.lgs";
 	if(!getparstring("fnz",&fnz)) fnz="z.lgs";
 	if(!getparstring("fna",&fna)) fna="a.lgs";
     	if(!getparfloat("prw",&prw)) prw=0.01;
    	if(!getparfloat("hb",&hb)) hb=15;
    	if(!getparfloat("zb",&zb)) zb=5;
    	if(!getparfloat("fftpad",&fftpad)) fftpad=25.0;
	fftpad /=100.0;
  	
	if(!getparint("s",&s)) s=1;
	if(!getparint("g",&g)) g=1;
	if(!getparint("h",&h)) h=1;
	if(!getparint("y",&y)) y=0;
	if(!getparint("z",&z)) z=1;
	if(!getparint("a",&a)) a=1;
	if(!getparint("sp",&sp)) sp=0;
	if(!getparint("gp",&gp)) gp=0;
	if(!getparint("hp",&hp)) hp=0;
	if(!getparint("yp",&yp)) yp=0;
	if(!getparint("zp",&zp)) zp=0;
	if(!getparint("ap",&ap)) ap=0;
	if(!getparint("res",&res)) res=0;
	if(!getparfloat("smt",&smt)) smt=4.0;
       
	if(!getparint("time",&time)) time=0;
	if(!getparfloat("lag",&lag)) lag=-1.00;
	if(!getparfloat("oplen",&oplen)) oplen=-1.00;
	
	if(!getparint("verbose",&verbose)) verbose=0;
	
	/* allocate space for tables */
	ST = ealloc1int(MS);
	GT = ealloc1int(MG);
	HT = ealloc1int(MH);
	YT = ealloc1int(MY);
	ZT = ealloc1int(MZ);
	for(i=0;i<MS;i++) ST[i]=BLANK;
	for(i=0;i<MG;i++) GT[i]=BLANK;
	for(i=0;i<MH;i++) HT[i]=BLANK;
	for(i=0;i<MY;i++) YT[i]=BLANK;
	for(i=0;i<MZ;i++) ZT[i]=BLANK;
	
	
	/* open the input files */
	fps = efopen(fns,"r");
        fpg = efopen(fng,"r");
        fph = efopen(fnh,"r");
        fpy = efopen(fny,"r");
        fpz = efopen(fnz,"r");
        fpa = efopen(fna,"r");
	
	/* scan the files and make the tables */
	scf(fps,ST,"ep",1);
	scf(fpg,GT,"sdepth",1);
	scf(fph,HT,"offset",hb);
	scf(fpy,YT,"cdp",1);
	scf(fpz,ZT,"otrav",zb);
	
	/* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
        nt = tr.ns;
        if (!getparfloat("dt", &dt)) dt = ((double) tr.dt)/1000000.0;
        if (!dt) {
                dt = .002;
                warn("dt not set, assumed to be .002");
        }

	if(lag<0) lag=dt;


	/* Set up pfa fft */
        nfft = npfar(NINT(nt*(1.0+fftpad)));
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
        nf = nfft/2 + 1;
	snfft = 1.0/nfft;
        d1 = 1.0/(nfft*dt);
	
	/* Get the frequency range */
     	if(!getparfloat("fmin",&fmin)) fmin=-1;
     	if(!getparfloat("fmax",&fmax)) fmax=0.5/dt;

     	if(!getparfloat("ltpr",&ltpr)) ltpr=7.0;
     	if(!getparfloat("htpr",&htpr))  htpr=10.0;
	
	/* intergers smoother length */
	ismt = NINT((smt/0.5)/d1);
	if(!ISODD(ismt)) ismt++;
	fprintf(stderr," Smoothing filter integer length %d\n",ismt);
	
	/* smoothing filter */
	filter = ealloc1float(ismt);
	rwa_smoothing_filter (2,ismt/2,ismt/2,filter);
	
	/* get the average operator get the default freq range from */
	/* its header values */
	fgettr(fpa,&trtmp);
	
	ffmin=trtmp.f1;
	ffmax=(trtmp.ns*trtmp.d1+trtmp.f1);
	
	fprintf(stderr," Minimum and maximum frequency in log spectra files %f %f \n",ffmin,ffmax);
	
	ffd1=trtmp.d1;
	ffn=trtmp.ns;
	
	fprintf(stderr," Frequency step of solution %f number of frequencies %d\n",ffd1,ffn);
	

	iltpr=NINT(ltpr/d1);
	ihtpr=NINT(htpr/d1);
	
	fmin=MAX(ffmin,fmin);
	fmax=MIN(fmax,ffmax);
	flimi=fmin/d1; 
	flima=fmax/d1;
	fn=flima-flimi;
	
	fprintf(stderr," Minimum and maximum frequency to deconvolve %f %f \n",fmin,fmax);
	fprintf(stderr," Frequency step of trace %f number of frequencies %d\n",d1,fn);
        
	/* Freq values from files */
	ffv=ealloc1float(ffn);
	for(i=0;i<ffn;i++) ffv[i]=ffmin+i*ffd1;
	
	/* output freq values */
	fv=ealloc1float(nf);
	for(i=0;i<nf;i++) fv[i]=i*d1;
	
	/* allocate storage space for FFT*/
	rt = ealloc1float(nfft);
        pwt = ealloc1float(nf);
        pwat = ealloc1float(nf);
        aw = ealloc1float(nf);
        amp = ealloc1float(nf);
        ph = ealloc1float(nf);
        ar = ealloc1float(nf);
        phr = ealloc1float(nf);
        spc = ealloc1complex(nf);
        spcinv = ealloc1complex(nf);
        spcav = ealloc1complex(nf);
        spcrs = ealloc1complex(nf);
        
	aspt = ealloc1float(nf);
	
	aspa = ealloc1float(nf);
	aspg = ealloc1float(nf);
	asps = ealloc1float(nf);
	aspy = ealloc1float(nf);
	asph = ealloc1float(nf);
	aspz = ealloc1float(nf);
	
	aspe = ealloc1float(nf);
	
        
	/* set up the taper */
	ltpra = ealloc1float(iltpr);
        for (i = 0; i < iltpr; ++i) {
		st = sin(i*PI/(2*(iltpr-1)));
		ltpra[i] = st*st;
	}
	htpra = ealloc1float(ihtpr);
        for (i = 0; i < ihtpr; ++i) {
		st = sin(((ihtpr-1)-i)*PI/(2*(ihtpr-1)));
		htpra[i] = st*st;
	}
	
	/* get the average spectra into the array */
	memcpy( (void *) aspa, (const void *) trtmp.data, nf*FSIZE);
	
	/* tests */
	if(verbose>1)
		efp = efopen("avespr.su","w");
		
	
	do {

		/* Load trace into rt (zero-padded) */
                memcpy( (void *) rt, (const void *) tr.data, nt*FSIZE);
                memset( (void *) (rt + nt), (int) '\0', (nfft - nt)*FSIZE);
		
		 /* FFT data trace to be deconvolved */
                sscal(nfft,snfft,rt,1);
		pfarc(1, nfft, rt, spc);
		
		/* Remove the band that is larger than fmax */
		for(i=flima;i<nf;i++) spc[i] = cmplx(0.0,0.0);  
		
		/* Compute the log amplitude spectra of the trace */
		/* This is for checking the solution */
		/* aspe is used as temorary storage */
		for(i=0;i<nf;i++) aspe[i]=AMPSP(spc[i]);
		
		/* Smooth it so we have the same input as in the scd5 module */
		if(ismt) 
			conv(ismt,-ismt/2,filter,nf,0,aspe,nf,0,aspt);
		
		for(i=0;i<nf;i++) aspt[i]=log(aspt[i]+FLT_EPSILON);

		 
		/* get the residuals for this trace */
		/* Shot */
		ti=fval(tr.ep,ST);
		if(ti!=-1) { 
			fgettra(fps,&trtmp,ti);
			
			/* Interpolate it to the input trace frequency steps */
			intlin(ffn,ffv,trtmp.data,0.0,0.0,nf,fv,asps);
		} else {
				warn("Shot %d not in spectra file. Skipping.\n",tr.ep);
		} 
		
		/* Receiver */
		ti=fval(tr.sdepth,GT);
		if(ti!=-1) { 
			fgettra(fpg,&trtmp,ti);
			
			/* Interpolate it to the input trace frequency steps */
			intlin(ffn,ffv,trtmp.data,0.0,0.0,nf,fv,aspg);
		} else {
				warn("Receiver %d not in spectra file. Skipping.\n",
				tr.sdepth);
		}
		 
		
		/* Offset */
		ti=fval(NINT(tr.offset/hb),HT);
		if(ti!=-1) { 
			fgettra(fph,&trtmp,ti);
			
			/* Interpolate it to the input trace frequency steps */
			intlin(ffn,ffv,trtmp.data,0.0,0.0,nf,fv,asph);
		} else {
			warn("Offset %d not in spectra file. Skipping.\n",
				tr.offset);
		} 
		
		/* CDP */
		ti=fval(tr.cdp,YT);
		if(ti!=-1) { 
			fgettra(fpy,&trtmp,ti);
			
			/* Interpolate it to the input trace frequency steps */
			intlin(ffn,ffv,trtmp.data,0.0,0.0,nf,fv,aspy);
		} else {
				warn("Offset %d not in spectra file. Skipping.\n",
				tr.cdp);
		} 
		
		
		/* Azimuth */
		ti=fval(NINT(tr.otrav/zb),ZT);
		if(ti!=-1) { 
			fgettra(fpz,&trtmp,ti);
		
			/* Interpolate it to the input trace frequency steps */
			intlin(ffn,ffv,trtmp.data,0.0,0.0,nf,fv,aspz);
		} else {
				warn("Azimuth %d not in spectra file. Skipping.\n",
				tr.otrav);
		}
		
		 
		
		/* Error should be close to zero if the solution was correct */
		/* This contains the error tha does not fit the model */
		/* for this trace */
		
		for(i=0;i<nf;i++) aspe[i] = aspt[i]-aspa[i]-aspy[i]-asps[i]
		                            -aspg[i]-aspz[i]-asph[i];
		if (verbose>1) {
			{segy etr;
				memcpy( (void *) etr.data, (const void *) aspe, nf*FSIZE);
				memcpy( (void *) &etr, (const void *) &tr, HDRBYTES);
				etr.ns=nf;
				etr.dt=ffd1*1000;
				etr.d1=ffd1;
				fputtr(efp,&etr);
			}
		}
					   
		
		
		/* error between the model and solution */ 
		chi_err=0.0;
		for(i=0;i<nf;i++) chi_err +=aspe[i];
		chi_err = sqrt((chi_err*chi_err)/nf);
		
		/* store it in header word ungpow */
		tr.ungpow=chi_err;
		
		/* APPLY SOLUTION */
		
		if(!time) {
		
			/* FREQUENCY DOMAIN SOLUTION */
			/* Build the inverse filter */
			/* Decide weather to remove the componenet
			 and if it is minimum phase or not */
		
			for(i=0;i<nf;i++) ar[i]=1.0;
			memset( (void *) phr, (int) '\0', nf*sizeof(float));
		
			if(s==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(asps[i]);
				if(sp==1)
					/* Minimum phase */
					spcfk(spcrs,nf);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
					if(ar[i]) 
						phr[i] += PHSSP(spcrs[i]);
				}
			}
			if(g==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspg[i]);
				if(gp==1)
					/* Minimum phase */
					spcfk(spcrs,nf);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
					if(ar[i]) 
						phr[i] += PHSSP(spcrs[i]);
				}
			}
			if(y==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspy[i]);
				if(yp==1)
					/* Minimum phase */
					spcfk(spcrs,nf);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
					if(ar[i]) 
						phr[i] += PHSSP(spcrs[i]);
				}
			}
			if(h==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(asph[i]);
				if(hp==1)
					/* Minimum phase */
					spcfk(spcrs,nf);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
					if(ar[i]) 
						phr[i] += PHSSP(spcrs[i]);
				}
			}
			if(z==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspz[i]);
				if(zp==1)
					/* Minimum phase */
					spcfk(spcrs,nf);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
					if(ar[i]) 
						phr[i] += PHSSP(spcrs[i]);
				}
			}
		
			if(a==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspa[i]);
				if(ap==1)
					/* Minimum phase */
					spcfk(spcrs,nf);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
					if(ar[i]) 
						phr[i] += PHSSP(spcrs[i]);
				}
			}
		
			if(res==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspe[i]);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
					if(ar[i]) 
						phr[i] += PHSSP(spcrs[i]);
				}
			}
				
			/* Pre whithen */
			pre_w(ar,nf,prw);
		
			/* compute inverse filter */
			memset( (void *) spcinv, (int) '\0', nf*sizeof(complex));
			for(i=0;i<nf;i++) {
				spcinv[i].r = ar[i]*cos(phr[i]);
				spcinv[i].i = ar[i]*sin(phr[i]);
			}

		
			/* Do the filtering */
			for(i=0;i<nf;i++) 
				spc[i] = cdiv(spc[i],spcinv[i]); 

			/* Inverse fft */
 			pfacr(-1, nfft,spc,rt);
					
			/* for(i=0;i<nf;i++) rt[i] = spcinv[i].r;   
			for(i=nf;i<tr.ns;i++) rt[i] = 0.0;  */
		} else {
		
			/* TIME DOMAIN SOLUTION */
			/* PREDICTIVE FILTERING */
		
			for(i=0;i<nf;i++) ar[i]=1.0;
			memset( (void *) phr, (int) '\0', nf*sizeof(float));
		
			if(s==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(asps[i]);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
				}
			}
			if(g==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspg[i]);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
				}
			}
			if(y==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspy[i]);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
				}
			}
			if(h==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(asph[i]);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
				}
			}
			if(z==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspz[i]);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
				}
			}
		
			if(a==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspa[i]);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
				}
			}
		
			if(res==1) {
				memset( (void *) spcrs,  (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) 
					spcrs[i].r=exp(aspe[i]);
				for(i=0;i<nf;i++) {
					ar[i] *= AMPSP(spcrs[i]);
				}
			}
			
			{ unsigned int ioplen,iprd;
			  int ilag;
			
			  float *wavelet,*xcorr,*wiener,*spiker;
			
			
				/* Prediction distance and operator length */
				ilag = NINT(lag/dt);
				if(oplen<0) {
					ioplen = NINT(nt/20.0);
				} else { 
					ioplen = MAX(MIN(NINT(oplen/dt),tr.ns),0);
				}
				
				wiener = ealloc1float(ioplen);
        			spiker = ealloc1float(ioplen);

				/* Create wavelet; inverse transform amplitude spectra */
				memset( (void *) spcinv, (int) '\0', nf*sizeof(complex));
				for(i=0;i<nf;i++) {
					spcinv[i].r = ar[i]*cos(phr[i]);
				}
				
				/* Inverse fft */
 				pfacr(-1, nfft,spcinv,rt);
				
				wavelet = rt;
				xcorr =  rt + ilag;
				
                		/* Whiten */
                		wavelet[0] *= 1.0 + prw;
               			                
				/* Get inverse filter by Wiener-Levinson */
				stoepf(ioplen, wavelet, xcorr, wiener, spiker);
 				
				/* Convolve pefilter with trace - don't do zero multiplies */
                		for (iprd = 0; iprd < tr.ns; iprd++) {
                        		int jprd;
                        		int nprd = MIN(iprd, ioplen); 
                        		float sumprd = tr.data[iprd];

                        		for (jprd = ilag; jprd <= nprd; jprd++)
                                		sumprd -= wiener[jprd-ilag] * tr.data[iprd-jprd];

                        		rt[iprd] = sumprd;
                		}


				free1float(wiener);
				free1float(spiker);
			}
		}
		

	        /* put back to the trace */
                memcpy( (void *) tr.data, (const void *) rt, tr.ns*FSIZE);
		

	     	puttr(&tr);
	}while(gettr(&tr));

	free1int(ST);
	free1int(GT);
	free1int(HT);
	free1int(YT);
	free1int(ZT);
        free1float(pwt);
        free1float(pwat);
        free1float(aw);
        free1float(amp);
        free1float(ph);
        free1float(ar);
        free1float(phr);
        free1float(rt);
        free1float(htpra);
        free1float(ltpra);
        free1complex(spc);
        free1complex(spcav);
        free1complex(spcrs);
        free1complex(spcinv);
	free1float(ffv);
	efclose(fps);
	efclose(fpg);
	efclose(fph);
	efclose(fpy);
	efclose(fpz);
 	efclose(fpa);
	
    return EXIT_SUCCESS;
}

void scf(FILE *fp,int *NT,cwp_String key,float bin)
{

	segy t;
	int i=0;
	cwp_String type;
	int indx;
        Value val;
		
	erewind(fp);        
	type = hdtype(key);
        indx = getindex(key);
		
	while(fgettr(fp,&t)) {
		gethval(&t, indx, &val);
		NT[i]=NINT(vtoi(type,val)/bin);
		i++;
	}
	
	erewind(fp);
}

int fval(int val,int *NT)
{
	int i=0;
	
	while(NT[i]!=BLANK) {
		if(NT[i]==val) return(i);
		i++;
	}
	return(-1);
}	

void taper(float *data, int n, float *ltaper, int lntr,float *htaper, int hntr, int st, int en)
{

	int i,j;
	
	for(i=0;i<st;i++) data[i] = FLT_EPSILON;
	for(i=st,j=0;i<st+lntr;i++,j++) data[i] *= ltaper[j];
	for(i=en-hntr,j=0;i<en;i++,j++) data[i] *= htaper[j];
	for(i=en;i<n;i++) data[i] = FLT_EPSILON;
}

void spcfk(complex *w,int nf)
/* Kolmogoroff spectral factorization */
/* input W spectrum of the wavelet */
/* output is its minimum phase equivalent */ 
{
	int i;
	int nfft;
	float snfftc;
	complex *w2;
	complex tmpdc;
	
        
	nfft = 2*nf-2;
	snfftc = 1.0/nfft;
	
	w2 = alloc1complex(nfft);
	for(i=0;i<nf;i++) {
		w2[i] = clog(cadd(w[i],cmplx(FLT_EPSILON,0.0)));
	}
        for(i=0;i<nf-2;i++) {
        	w2[nf+i]=w2[nf-2-i];
		 
	}

	pfacc(-1,nfft,w2);

        for (i=0; i<nfft; ++i) {
		w2[i].r *=snfftc;
		w2[i].i *=snfftc;
	}
	

	w2[0] = crmul(w2[0],0.5);
	w2[nf-1] = crmul(w2[nf-1],0.5);
	for(i=nf;i<nfft;i++) w2[i] = cmplx(0.0,0.0);
	
	pfacc(1,nfft,w2);
	
	for(i=0;i<nf;i++) {
		tmpdc = cexp(w2[i]);
		w[i].r = (float)tmpdc.r;
		w[i].i = (float)tmpdc.i;
	}
		 
	free1complex(w2);
}

void pre_w(float *a,int n,float p)
{
	/* Pre-withening */
	int i;
	float mean,sum=0.0;
	for(i=0;i<n;i++)
		sum+=a[i];
	mean =sum/n;
	mean*=p/100.0;
	for(i=0;i<n;i++)
		if(a[i] < mean) a[i] = mean ;
}
	
	 


 
