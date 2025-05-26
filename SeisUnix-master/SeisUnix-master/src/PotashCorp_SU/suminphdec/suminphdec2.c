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
	
	register float *rt;     /* real trace                           */
        register complex *spc;   /* complex transformed trace            */
        int nfft;               /* number of points on output trace     */
        int nfby2p1;            /* nfft/2 + 1                           */
        float dt;               /* sample interval in secs              */
        float d1;               /* output sample interval in Hz         */
	float *pw;		/* power spectra */
	float *spw;		/* soothed power spectra */
	float m_p=0.0;		/* mean value of spectra */
	float prw;		/* pre-withening */
	float *tmp;		/*  */
	complex *spcinv;	/* inverse filter */
	complex *spcout;	/* inverse filter */
	complex a;		/* complex helper */
	
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
		
	fld=0; fm=1; fnr=fnl;
	filter = ealloc1float(fnp);
	SG_smoothing_filter(fnp,fnl,fnr,fld,fm,filter); 
/*	rwa_smoothing_filter(1,fnl,fnr,filter); */
	
	/* Set up pfa fft */
        nfft = npfaro(nt, LOOKFAC * nt);
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
        nfby2p1 = nfft/2 + 1;
        d1 = 1.0/(nfft*dt);

        rt = ealloc1float(nfft);
        pw = ealloc1float(nfby2p1);
        spw = ealloc1float(nfby2p1);
	tmp = ealloc1float(nfby2p1);
	spcinv = ealloc1complex(nfby2p1);
	spc = ealloc1complex(nfby2p1);
	spcout = ealloc1complex(nfby2p1);
	
	do {
                
		/* Load trace into rt (zero-padded) */
                memcpy( (void *) rt, (const void *) tr.data, nt*FSIZE);
                memset( (void *) (rt + nt), (int) '\0', (nfft - nt)*FSIZE);
                				
                /* FFT */
		pfarc(1, nfft, rt, spc);

                /* Compute amplitude spectrum */
                for (i = 0; i < nfby2p1; ++i)  { pw[i] = rcabs(spc[i]);
		
		/* mean value of power spectra */
		m_p +=pw[i];
		}
		
		/* stabilize */
		m_p *= prw/nfby2p1;
		for (i = 0; i < nfby2p1; ++i)  pw[i] += m_p;

		/* Smooth the power spectrum */
		conv (fnl+fnr+1,-fnl,filter,nfby2p1,0,pw,nfby2p1,0,spw);

		/* Hilbert transform */
		for (i = 0; i < nfby2p1; ++i)  tmp[i] = log(spw[i]);
		hilbert(nfby2p1,tmp,spw);
		
		/* inverse Filter */
		for (i = 0; i < nfby2p1; ++i) {
			a.r = 0.5*tmp[i]; a.i = 0.5*spw[i];
			spcinv[i] = cexp1(a);
		} 
	
		/* separate amplitude and phase terms */
		for (i = 0; i < nfby2p1; ++i) {
			a.r=spcinv[i].r; a.i=spcinv[i].i;
			
			/* inverse of the spectras is the filter */
			spcinv[i].r = 1.0/((float)sqrt(a.r*a.r+a.i*a.i));
			spcinv[i].i = -atan2(a.i,a.r);
		}
		for (i = 0; i < nfby2p1; ++i) {
			a.r=spcinv[i].r; a.i=spcinv[i].i;
			
			/* real and im parts again */
			spcinv[i].r = a.r*cos(a.i);
			spcinv[i].i = a.r*sin(a.i);
		}
		
		/* apply inverse filter */
		for (i = 0; i < nfby2p1; ++i) {
			spcout[i] = cmul (spcinv[i],spc[i]);
		}
			
		/* Time domain */
		pfacr(-1, nfft,spcout,pw);	
		
		/* put back to the trace */
		memcpy( (void *) tr.data, (const void *) pw, tr.ns*FSIZE);
		
		ntr++;		
		puttr(&tr);
	} while(gettr(&tr));
	
   return EXIT_SUCCESS;
}
