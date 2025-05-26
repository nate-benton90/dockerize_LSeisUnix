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

int main( int argc, char *argv[] )
{
	/* Segy data constans */
	int nt;                 /* number of time samples               */
        int ntr=0;              /* number of traces                     */
	int i;			/* counter */
	
	float *filter;
	int fnl,fnr;
	int fnp;
	int fld;
	int fm;
	int ph;			/* minimuphase or zero phase switch     */
	register float *rt;     /* real trace                           */
	register float *rtx;     /* real trace                           */
        register complex *spc;   /* complex transformed trace            */
        float *pw;
	float *rw;
	float *uw;
	float *fw;
	complex *f;
	complex *w;
	complex *spcout;	/* inverse filter */
	complex a;		/* complex helper */
	float snfft;

	int nfft;               /* number of points on output trace     */
        int nfby2p1;            /* nfft/2 + 1                           */
        float dt;               /* sample interval in secs              */
        float d1;               /* output sample interval in Hz         */
	float m_p=0.0;		/* mean value of spectra */
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
		fld=0; fm=1; fnr=fnl;
		filter = ealloc1float(fnp);
/*		SG_smoothing_filter(fnp,fnl,fnr,fld,fm,filter); */
		rwa_smoothing_filter(1,fnl,fnr,filter); 
	} else {
		filter= NULL;
	}
	
	/* Set up pfa fft */
        nfft = npfaro(nt, LOOKFAC * nt);
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
        nfby2p1 = nfft/2 + 1;
	snfft=1.0/nfft;
        d1 = 1.0/(nfft*dt);

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
	prw +=1.0;
	
	do {
                m_p=0;
		
		/* Cross correlation */
		xcor(nt,0,tr.data,nt,0,tr.data,nt,0,rtx);
		
		
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
			rw[i] = (float)sqrt(a.r*a.r+a.i*a.i);
		}			
		
		for (i = 0; i < nfby2p1; ++i)  uw[i] = log(rw[i]);
		
		/* Hilbert transform to estimate fw */
		hilbert(nfby2p1,uw,fw);

		for (i = 0; i < nfby2p1; ++i) {
			a.r=uw[i]; a.i=fw[i];
			a = crmul(a,0.5);
			w[i] = cexp1(a);
		} 
	
		/* inverse Filter design */
		for (i = 0; i < nfby2p1; ++i) {
			a.r = w[i].r; a.i=w[i].i;
			pw[i] = (float)sqrt(a.r*a.r+a.i*a.i);
			pw[i] *= prw;

			if(ph==0) {
				f[i].i=0;
			} else {
				f[i].i = atan((double)(a.i/a.r));
			}
			
		}
				
		/* Smooth the spectra */
		if(fnl!=0) conv (fnl+fnr+1,-fnl,filter,nfby2p1,0,pw,nfby2p1,0,pw);
		
		for (i = 0; i < nfby2p1; ++i) {
                        a.r=1.0/pw[i]; a.i=-f[i].i;
                        
                        f[i].r = a.r*cos(a.i);
                        f[i].i = a.r*sin(a.i);
		}

/*		for (i = 0; i < nfby2p1; ++i) {
 			f[i] = cipow(w[i],-1);
		}
*/
		/* apply inverse filter */
                memcpy( (void *) rt, (const void *) tr.data, nt*FSIZE);
                memset( (void *) (rt + nt), (int) '\0', (nfft - nt)*FSIZE);
		/* trace to frequency domain */
		pfarc(1, nfft,rt,spc);
		
		for (i = 0; i < nfby2p1; ++i) {
			spcout[i] = cmul (f[i],spc[i]);
		}
			
		/* Time domain */
		pfacr(-1, nfft,spcout,rt);	
		for(i=0;i<nt;i++) rt[i] *=snfft;
		
		/* put back to the trace */
		tr.ns=nt;
		memcpy( (void *) tr.data, (const void *) rt, nt*FSIZE);
		
		ntr++;		
		puttr(&tr);
	} while(gettr(&tr));
	
   return EXIT_SUCCESS;
}
