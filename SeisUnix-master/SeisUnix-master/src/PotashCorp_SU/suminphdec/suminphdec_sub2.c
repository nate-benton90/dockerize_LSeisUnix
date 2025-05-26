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
"       prw=1.0         pre-withening                                   ",       
NULL};

#define LOOKFAC 4       /* Look ahead factor for npfaro   */
#define PFA_MAX 720720  /* Largest allowed nfft           */
   
/* Segy data constans */
segy tr;				/* SEGY trace */
void do_minphdec(float *tr,int nt, float *filter,int fnl,int fnr,float prw);

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
        float dt;               /* sample interval in secs              */
	float prw;		/* pre-withening */
	
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
	if(!getparfloat ("prw", &prw)) prw=1.0;
		
	if(fnl!=0) {
		fld=0; fm=0; fnr=fnl;
		filter = ealloc1float(fnp);
		SG_smoothing_filter(fnp,fnl,fnr,fld,fm,filter); 
/*		rwa_smoothing_filter(1,fnl,fnr,filter); */ 
	} else {
		filter= NULL;
	}
	
	do {
		do_minphdec(tr.data,nt,filter,fnl,fnr,prw);
		
		tr.ns=nt;
		ntr++;		
		puttr(&tr);
	} while(gettr(&tr));
	
   return EXIT_SUCCESS;
}

void do_minphdec(float *tr,int nt, float *filter,int fnl,int fnr,float prw)
{

	float *rtr;
	float *rtx;     
	complex *f;
	complex *w;
	complex a;
	int iamp;
	float amp;
	float ampm=-1.0e+20;
	float amps;
	float *am;
	float *ph;	
	float mean=0.0;
	float sum=0.0;

	int nfftc; 
        int nf;    
	int i,j;			/* counter */
	float snfftc;
	

	/* Set up pfa fft */
	nfftc = npfao(nt,LOOKFAC*nt); 
        if (nfftc >= SU_NFLTS || nfftc >= PFA_MAX)
                 err("Padded nt=%d--too big", nfftc);
        nf = nfftc/2 + 1;
	snfftc=1.0/nfftc;

        rtr = ealloc1float(nfftc);
        rtx = ealloc1float(nf);
	f = ealloc1complex(nfftc);
	w = ealloc1complex(nfftc);
	am = ealloc1float(nf);
	ph = ealloc1float(nf);
        
	/* clean the arrays */
	memset( (void *) w, (int) '\0', nfftc*sizeof(complex));
        memset( (void *) rtr, (int) '\0', nfftc*FSIZE);
	
	/* Cross correlation */
	xcor(nt,0,tr,nt,0,tr,nf,0,rtr);

        /* FFT */
	pfarc(1, nfftc,rtr,w);

	/* stabilize */
	for(i=0;i<nf;i++) {
		am[i] += am[i]*prw;
	}
	
	/* Normalize */
	for(i=0;i<nf;i++) {
		a=w[i];
		am[i]= sqrt(a.r*a.r+a.i*a.i);
		sum += am[i];
		if(am[i]!=0) ph[i] = atan2(a.i,a.r);
		else ph[i]=0;
	}
	sum *=	1.0/nf;
	sum = 1.0/sum;
	sscal(nf,sum,am,1);
	
	/* Smooth the apmlitude spectra  */
	if(fnl!=0) conv (fnl+fnr+1,-fnl,filter,nf,0,am,nf,0,am);

	fprintf(stderr," %f\n",sum);	
	
	for(i=0;i<nf;i++) {
		w[i].r = am[i]*cos(ph[i]);
		w[i].i = am[i]*sin(ph[i]);
	}
	for(i=nf,j=nf-1;i<nfftc;i++,j--) {
		w[i].r = am[j]*cos(ph[j]);
		w[i].i = am[j]*sin(ph[j]);
	}
		
	/* log spectra */
	for (i = 0; i < nfftc; ++i)  w[i] =
		crmul(clog(cmul(w[i],conjg(w[i]))),0.5);

	/* Hilbert transform */
	pfacc(-1,nfftc,w);
        for (i=0; i<nfftc; ++i) {
		w[i].r *=snfftc;
		w[i].i *=snfftc;
	}
	for(i=1;i<nfftc/2;i++) w[i] = cadd(w[i],w[i]);
	for(i=nfftc/2;i<nfftc;i++) w[i] = cmplx(0,0);
	pfacc(1,nfftc,w);
	/* end of Hilbert transform */
	
	/* exponentiate */
	for(i=0;i<nfftc;i++) w[i] = cexp(w[i]);
	
	/* inverse filter */
	for(i=0;i<nfftc;i++) f[i] = cdiv(cmplx(1.0,0),w[i]);
	
	/* Load trace into tr (zero-padded) */
        memset( (void *) w, (int) '\0',nfftc*sizeof(complex));
	for(i=0;i<nt;i++) w[i].r = tr[i];

	/* Trace to frequency domain */
	pfacc(1,nfftc,w);
      
      	/* apply filter */
        for(i=0;i<nfftc;i++) w[i] = cmul(w[i],f[i]);
             
        /* Time domain */
        pfacr(-1, nfftc,w,rtr);
	for(i=0;i<nt;i++) rtr[i] *=snfftc;
	
	memcpy( (void *) tr, (const void *) rtr, nt*FSIZE);				
	
	free1float(rtr);
	free1float(am);
	free1float(ph);
	free1complex(f);
	free1complex(w);
}	
	
