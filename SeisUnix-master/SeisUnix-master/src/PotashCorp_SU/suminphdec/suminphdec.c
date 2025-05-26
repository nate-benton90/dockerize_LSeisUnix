/* suminphdec.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include <sys/file.h>
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
"       fnl=3	        frequency smoother filter half length           ",
"       prw=1.0         pre-withening                                   ",       
"       fa=1	        =0 The gather is converted to minimum phase only",       
"       tdmin=tmin	Desing window start time                        ",
"       tdmax=tmax	Desing window end time                          ",
"	fftpad=20.0	% of trace padding before fft			",
"									",
"	Desing mode:							",
"	d=0		design mode if d=1				",
"			The estimated, datavolume averaged wavelet	",
"			is output to file fn				",
"			The wavelet is stored in Fourier tranformed	",
"			form, as a series of complex numbers		",
"									",
"	fn=wavelet.hsu							",
"									",
"	Filter mode							",
"	f=0		filter mode f=1					",
"	 		The wavelet is read from the file f, and	",
"			the data volume is deconvolved with the 	",
"			inverse of the wavelet				",
"	fn=wavelet.hsu							",
NULL};

#define LOOKFAC 4       /* Look ahead factor for npfaro   */
#define PFA_MAX 720720  /* Largest allowed nfft           */
   
/* Segy data constans */
segy tr;				/* SEGY trace */
segy wavelet;
void do_minphdec(float *tr,int nt, float dt,float *filter,int fnl,int fnr,float prw,int fa,
		int itdn,int itdx,
		int d,int f,float *wavelet,float *d1,unsigned short *nf,
		float fftpad);

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
	int fa;			/* flag to indicate only phase decon */
	float tdmin;		/* operator design window start */
	float tdmax;		/* operator design window end */
	int itdn;		/* the same as above but in samples */
	int itdx;
	int d;			/* design mode switch */
	int f;			/* filter mode switch */
	float fftpad;		/* fft padding in percent */


	
	initargs(argc, argv);
   	requestdoc(1);
	
	if(!getparint ("f", &f)) f=0;
	if(!getparint ("d", &d)) d=0;
	
        /* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
        nt = tr.ns;

        if (!getparfloat("dt", &dt)) dt = ((double) tr.dt)/1000000.0;
        if (!dt) {
                dt = .002;
                warn("dt not set, assumed to be .002");
        }
	
	if(!getparint ("fa", &fa)) fa=1;
	
	if(!getparint ("fnl", &fnl)) fnl=3;
	fnr=fnl;
	if(!getparint ("fnp", &fnp)) fnp=fnr+fnl+fnr/2;
	if(!getparfloat ("prw", &prw)) prw=1.0;
	if(!getparfloat ("tdmin", &tdmin)) tdmin=tr.delrt/1000.0;
	if(!getparfloat ("tdmax", &tdmax)) tdmax=(nt-1)*dt;
	if(!getparfloat ("fftpad", &fftpad)) fftpad=20.0;
	fftpad /=100.0;
		
	itdn=MAX(NINT((tdmin-tr.delrt/1000.0)/dt),0);
	itdx=MIN(NINT(tdmax/dt),nt-1);
	if(fnl!=0) {
		fld=0; fm=0; fnr=fnl;
		filter = ealloc1float(fnp);
		SG_smoothing_filter(fnp,fnl,fnr,fld,fm,filter); 
/*		rwa_smoothing_filter(1,fnl,fnr,filter); */ 
	} else {
		filter= NULL;
	}
	
	wavelet.trid = FUNPACKNYQ;
	
	/* Filter mode */
	if(f) {
		{ FILE *fp;
			fp=efopen("wavelet.hsu","r");
			fcntl((int)fp,F_SETLKW);
			fgettr(fp,&wavelet);
			efclose(fp);
		}
	}
		
	do {
		do_minphdec(tr.data,nt,dt,filter,fnl,fnr,prw,fa,itdn,itdx,
			    d,f,wavelet.data,&wavelet.d1,&wavelet.ns,fftpad);
		ntr++;		
		
		/* Normal and filter mode */
		tr.ns=nt;
		puttr(&tr);

	} while(gettr(&tr));
	
	/* Design mode */
	if(d) {
		{ FILE *fp;
		  int it;
			for(it=0;it<wavelet.ns;it++) wavelet.data[it]/=ntr;
			fp=efopen("wavelet.hsu","w");
			fcntl((int)fp,F_SETLKW);
			fputtr(fp,&wavelet);
			efclose(fp);
		}
	}
	
   return EXIT_SUCCESS;
}

void do_minphdec(float *tr,int nt, float dt,float *filter,int fnl,int fnr,float prw,int fa,
		int itdn,int itdx,
		int d,int ff,float *wavelet,float *d1,unsigned short *nwf,float fftpad)
{
/* Inputs:
 tr 		data trace 
 nt		number of samples
 dt		sampling intervall 
 filter 	arrayf of smoothig filter
 nfl 		number of elements in filter array
 prw		Prewightewning
 fa		1 full decon; 0 only convert phase to minimum
 itdn		start autocorr from this sample
 itdx		end atocorr at this sample
 d		design mode flag
 f		filter mode flag
 wavelet 	pointer to the Fourier transformed wavelet array
 d1		frequency spacing
 nf		number of fequencies 
 */
	float *rtr;
	float *rtx;     
	complex *f;
	complex *w;
	complex a;
	float *am;
	float *ph;	
	float mean=0.0;

	int nfftc; 
        int nf;    
	int i,j;			/* counter */
	float snfftc;
	float atmp=0.0;
	

	/* Set up pfa fft */
	nfftc = npfa(NINT(nt*(1.0+fftpad))); 
        if (nfftc >= SU_NFLTS || nfftc >= PFA_MAX)
                 err("Padded nt=%d--too big", nfftc);
        nf = nfftc/2 + 1;
	snfftc=1.0/nfftc;
 	*d1 = 1.0/(nfftc*dt);
	
        rtr = ealloc1float(nfftc);
        rtx = ealloc1float(nf);
	f = ealloc1complex(nfftc);
	w = ealloc1complex(nfftc);
	am = ealloc1float(nf);
	ph = ealloc1float(nf);
        
	/* clean the arrays */
	memset( (void *) w, (int) '\0', nfftc*sizeof(complex));
        memset( (void *) rtr, (int) '\0', nfftc*FSIZE);
	
	/* If not filter mode than to the next chunk */
	if(!ff) {
		/* Auto correlation */
		xcor(itdx-itdn,0,&tr[itdn],itdx-itdn,0,&tr[itdn],nf,-(nf-1)/2,rtr);
	
		/* If no autocorr, leave the trace alone */
		atmp = rtr[(nf-1)/2];
		if(atmp==0.0) return;
		
		/* Normalize autocorr to preserve amplitudes*/
		for(i=0;i<nf;i++) rtr[i] /=atmp;
		 
		/* FFT */
		pfarc(1, nfftc,rtr,w);
	
		/* Compute amplitude and phase spectra */
		/* Amplitude spectra of autocorr = square of wavelet spectra */
		for(i=0;i<nf;i++) {
			a=w[i];
			am[i]= sqrt(rcabs(a));
			ph[i]=0.0;
		}
	
		mean=0.0;
		for(i=0;i<nf;i++) mean+=am[i];
		mean/=nf;
		mean*=prw;
		for(i=0;i<nf;i++) if(am[i] < mean) am[i]=mean;
	
		/* Smooth the apmlitude spectra  */
		if(fnl!=0) conv (fnl+fnr+1,-fnl,filter,nf,0,am,nf,0,am);

		/* Recompute the real and imag parts  */
		for(i=0;i<nf;i++) {
			w[i].r = am[i]*cos(ph[i]);
			w[i].i = 0.0; 
		}
		for(i=nf,j=nf-1;i<nfftc;i++,j--) {
			w[i].r = am[j]*cos(ph[j]);
			w[i].i = 0.0;
		}
		
		/* log spectra */
		for (i = 0; i < nfftc; ++i)  w[i] =clog(w[i]);

		/* Kolmogoroff spectral factorization to get a minimum phase signal*/
		pfacc(-1,nfftc,w);
        	for (i=0; i<nfftc; ++i) {
			w[i].r *=snfftc;
			w[i].i *=snfftc;
		}
		w[0] = crmul(w[0],0.5);
		w[nf-1] = crmul(w[nf-1],0.5);
		for(i=nf;i<nfftc;i++) w[i] = cmplx(0.0,0.0);
		pfacc(1,nfftc,w);
	
		/* exponentiate */
		for(i=0;i<nfftc;i++) w[i] = cexp(w[i]);
	
		/* Make an option to chage only the phase 
	   	and not the amplitudes */
		/* Compute amplitude and phase spectra */
		if(fa==0) {
			for(i=0;i<nf;i++) {
				a=w[i];
				am[i]= rcabs(a);
				if(am[i]!=0) ph[i] = -atan2(a.i,a.r);
				else ph[i]=0.0;
			}
			for(i=0;i<nf;i++) am[i]=1.0;
			/* Recompute the real and imag parts  */
			for(i=0;i<nf;i++) {
				w[i].r = am[i]*cos(ph[i]);
				w[i].i = am[i]*sin(ph[i]);
			}
		}
	}
	
	/* We do this and return in design mode */
	if(d==1) {
		/* store the wavelet */
		for(i=0;i<nf;i++) {
                        wavelet[2*i]   += w[i].r;
                        wavelet[2*i+1] += w[i].i;
		}
		*nwf=2*nf;
		return;
	}	
	
	/* Filter mode starts here */
	if(ff==1) {
		/* get the wavelet */
		for(i=0;i<nf;i++) {
                        w[i].r = wavelet[2*i];
                        w[i].i = wavelet[2*i+1];
		}
	}
		
	/* We only get here in normal and filter mode */
	/* inverse filter */
	for(i=0;i<nf;i++) f[i] = w[i];
		
	/* Load trace into tr (zero-padded) */
        memset( (void *) rtr, (int) '\0',nfftc*FSIZE);
	memcpy( (void *) rtr, (const void *) tr, nt*FSIZE);

      /* Trace to frequency domain */
	pfarc(1,nfftc,rtr,w);
      
      	/* apply filter */
        for(i=0;i<nf;i++) w[i] = cdiv(w[i],f[i]);
            
        /* Time domain */
        pfacr(-1, nfftc,w,rtr);
	for(i=0;i<nt;i++) rtr[i] *=snfftc;
	memcpy( (void *) tr, (const void *) rtr, nt*FSIZE);				
	
	free1float(rtr);
	free1float(rtx);
	free1float(am);
	free1float(ph);
	free1complex(f);
	free1complex(w);
}	
	
