/* suminphdec.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
char *sdoc[] = {
" SUMINPHDEC - perform minimum phase deconvolution                      ",
"                                                                       ",
" suminphdec < stdin > stdout                                           ",
"                                                                       ",
" Required parameters:                                                  ",
"        none                                                           ",
"                                                                       ",
" Optional parameters:                                                  ",
"       fnl=15	        frequency smoother filter half length           ",
"       pwr=0.1         pre-withening                                   ",       
"       ph=1         	minimum phase; ph=0 zero phase                  ",       
"       amp=1         	amp=0 modify only the phase spectrum            ",       
NULL};

#define LOOKFAC 2       /* Look ahead factor for npfaro   */
#define PFA_MAX 720720  /* Largest allowed nfft           */
   
/* Segy data constans */
segy tr;				/* SEGY trace */
void do_minphdec(float *tr,int nt, float *filter,int fnl,int fnr,
		int amp,int ph,float prw);

int main( int argc, char *argv[] )
{
	/* Segy data constans */
	int nt;                 /* number of time samples               */
        int ntr=0;              /* number of traces                     */
	
	float *filter;
	int fnl,fnr;
	int fnp;
	int fld;
	int fm;
	int ph;			/* minimuphase or zero phase switch     */
        float dt;               /* sample interval in secs              */
	float prw;		/* pre-withening */
	int amp;		/* amplitude flag */
	
	initargs(argc, argv);
   	requestdoc(1);
	
        /* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
        nt = tr.ns;

        if (!getparfloat("dt", &dt)) dt = ((double) tr.dt)/1000000.0;
        if (!dt) {
                dt = .002;
                warn("dt not set, assumed to be .002");
        }
	
	if(!getparint ("fnl", &fnl)) fnl=15;
	fnr=fnl;
	if(!getparint ("fnp", &fnp)) fnp=fnr+fnl+fnr/2;
	if(!getparfloat ("prw", &prw)) prw=0.1;
	if(!getparint ("ph", &ph)) ph=1;
	if(!getparint ("amp", &amp)) amp=1;
		
	if(fnl!=0) {
		fld=0; fm=0; fnr=fnl;
		filter = ealloc1float(fnp);
		SG_smoothing_filter(fnp,fnl,fnr,fld,fm,filter); 
/*		rwa_smoothing_filter(1,fnl,fnr,filter); */ 
	} else {
		filter= NULL;
	}
	
	do {
		do_minphdec(tr.data,nt,filter,fnl,fnr,amp,ph,prw);
		
		tr.ns=nt;
		ntr++;		
		puttr(&tr);
	} while(gettr(&tr));
	
   return EXIT_SUCCESS;
}

void do_minphdec(float *tr,int nt, float *filter,int fnl,int fnr,
		int amp,int ph,float prw)
{
	float *rt;
	float *rtx;     
        complex *spc;   
        float *pw;
	float *rw;
	float *uw;
	float *fw;
	complex *f;
	complex *w;
	complex *spcout;
	complex a;	
	complex b;
	float snfft;
	float mean=0;

	float maxam;
	int imax;
	int nfft;       
        int nfby2p1;    
	int i;			/* counter */

	/* Set up pfa fft */
        nfft = npfaro(nt, LOOKFAC * nt);
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
        nfby2p1 = nfft/2 + 1;
	snfft=1.0/nfft;

        rt = ealloc1float(nfft);
        rtx = ealloc1float(nt);
	pw = ealloc1float(nfby2p1);
	rw = ealloc1float(nfby2p1);
	uw = ealloc1float(nfby2p1);
	fw = ealloc1float(nfby2p1);
	f = ealloc1complex(nfby2p1);
	w = ealloc1complex(nfby2p1);
	spc = ealloc1complex(nfby2p1);
	spcout = ealloc1complex(nfby2p1);
	
	/* Cross correlation */
	xcor(nt,0,tr,nt,0,tr,nt,0,rtx);
	
	
	/* Load trace into rt (zero-padded) */
        memcpy( (void *) rt, (const void *) rtx, nt*FSIZE);
        memset( (void *) (rt + nt), (int) '\0', (nfft - nt)*FSIZE);
	
        /* FFT */
	pfarc(1, nfft, rt, spc);

        /* Save the real part of the fft*/
	a.r = spc[0].r; a.i=spc[0].i;
	rw[0]= (float)sqrt(a.r*a.r+a.i*a.i)/2.0;
        for (i = 1; i < nfby2p1; ++i)  {
		a.r=spc[i].r; a.i=spc[i].i;
		rw[i] = sqrt(a.r*a.r+a.i*a.i);
		mean += rw[i];
	}			
	mean /=	nfby2p1;
	mean *=	prw;	

	for (i = 0; i < nfby2p1; ++i) rw[i] +=mean;

	/* Smooth the spectra */
	if(fnl!=0) conv (fnl+fnr+1,-fnl,filter,nfby2p1,0,rw,nfby2p1,0,rw);

	for (i = 0; i < nfby2p1; ++i)  uw[i] = log(rw[i]);
	
	/* Hilbert transform to estimate fw */
	hilbert(nfby2p1,uw,fw);

	for (i = 0; i < nfby2p1; ++i) {
		a.r=0.5*uw[i]; a.i=0.5*fw[i];
		w[i] = cexp(a);
	} 
	
	/* inverse Filter design */
	for (i = 0; i < nfby2p1; ++i) {
		a.r = w[i].r; a.i=w[i].i;
		pw[i] = (a.r*a.r+a.i*a.i);

		if(ph==0) {
			f[i].i=0;
		} else {
			f[i].i = atan2(a.i,a.r);
		}
		
	}

	/* normalize the filter */
	imax = isamax(nfby2p1,pw,1);
	maxam = pw[imax];
	for(i=0;i<nfby2p1;i++) pw[i] /= maxam;
		
	for (i = 0; i < nfby2p1; ++i) {
                a.r=1.0/pw[i]; a.i=-f[i].i;
                
                f[i].r = a.r*cos(a.i);
                f[i].i = a.r*sin(a.i);
	}


	/* apply inverse filter */
        memcpy( (void *) rt, (const void *) tr, nt*FSIZE);
        memset( (void *) (rt + nt), (int) '\0', (nfft - nt)*FSIZE);
	/* trace to frequency domain */
	pfarc(1, nfft,rt,spc);
	
	for (i = 0; i < nfby2p1; ++i) {
		spcout[i] = cmul (f[i],spc[i]);
	}
		
	/* Time domain */
	pfacr(-1, nfft,spcout,rt);	
	for(i=0;i<nt;i++) rt[i] *=snfft;
	
	memcpy( (void *) tr, (const void *) rt, nt*FSIZE);				
	
	free1float(rt);
	free1float(rtx);
	free1float(pw);
	free1float(rw);
	free1float(uw);
	free1float(fw);
	free1complex(f);
	free1complex(w);
	free1complex(spc);
	free1complex(spcout);
	
}	
	
	
	
