/* SUTVBAND: $Revision: 1.10 $ ; $Date: 92/03/22 18:48:28 $      */

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
String sdoc =
"                                                               \n"
" SUTVBAND - time-variant bandpass filter (sine-squared taper)  \n"
"                                                               \n"
" sutvband <stdin >stdout tf= f=			        \n"
"                                                               \n"
" Required parameters:                                          \n"
"       dt = (from header)      time sampling rate (sec)        \n"
"       tf=             times for which f-vector is specified   \n"
"       f=f1,f2,f3,f4   Corner frequencies corresponding to the \n"
"                       times in tf. Specify as many f= as      \n"
"                       there are entries in tf.                \n"
"                                                               \n"
" The filters are applied in frequency domain.                  \n"
"                                                               \n"
" Example:                                                      \n"
" sutvband <data tf=.2,1.5 f=10,12.5,40,50 f=10,12.5,30,40 | ...\n"
"                                                               \n"
" Trace header fields accessed:  ns, dt, delrt.			\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *      CWP: Jack, Ken
 */


#define PIBY2   1.57079632679490
#define LOOKFAC 2       /* Look ahead factor for npfaro   */
#define PFA_MAX 720720  /* Largest allowed nfft           */

/* Prototypes */
void makefilter(float *f, int nfft, int nfreq, float dt, float *filter);
void bandpass(float *data, int ntime, int nfft, int nfreq, 
		float *filterj, float *ftracej);

segy tr;

main(int argc, char **argv)
{
        float **filter;         /* filter arrays                        */
        float *tf;              /* times at which filters are centered  */
        int *itf;               /* ... as integers                      */
	int jmin;		/* index of first filter itf value	*/
	int jmax;		/* index of last filter itf value	*/
        int nfft;     	        /* fft sizes in each time gate          */
        int nfreq;     	        /* number of frequencies  	        */
        float **ftrace;         /* filtered sub-traces                  */
        int nfilter;            /* number of filters specified          */
      	float dt;               /* sample spacing                       */
        float tmin;             /* first time on traces                 */
        int nt;                 /* number of points on input trace      */

        
        /* Initialize */
        initargs(argc, argv);
        askdoc(1);


        /* Get info from first trace */ 
        if (!gettr(&tr))  err("can't get first trace");
        if (tr.trid && tr.trid != TREAL)
                err("input is not seismic data, trid=%d", tr.trid);
        nt = tr.ns;
        if (!getparfloat("dt", &dt))    dt = tr.dt/1000000.0;
        if (!dt) err("dt field is zero and not getparred");
	tmin = tr.delrt/1000.0;


        /* Get number of filters and center times */
        if (!(nfilter = countparval("tf")))  MUSTGETPARFLOAT("tf", tf);
	if (countparname("f") != nfilter)
		err("must give one f 4-tuple for each"
		    " (%d) tf value", nfilter);
		
	/* Leave room for possibly missing filters at endpoints */
	tf = ealloc1float(nfilter+4);  /* never use ist2 or last 2 */
	itf = ealloc1int(nfilter+4);
        getparfloat("tf", tf+2); jmin = 2; jmax = nfilter + 1;
	{ register int j;
          for (j = jmin; j <= jmax; ++j)  itf[j] = NINT((tf[j] - tmin)/dt);
        }
	

        /* Make filters with scale for inverse transform */
	nfft = npfaro(nt, LOOKFAC * nt);
	if (nfft >= MIN(SU_NFLTS, PFA_MAX))
		err("Padded nt=%d -- too big", nfft);
	nfreq = nfft/2 + 1;
	filter = ealloc2float(nfreq, nfilter+4); /* never use 1st & last */
        { register int j;
          for (j = jmin; j <= jmax; ++j) {
                float *f = ealloc1float(4);

                if (getnparfloat(j-jmin+1, "f", f) != 4)
                        err("must give 4 corner frequencies in f=");
        	if (f[0] < 0.0 || f[0] > f[1] ||
				  f[1] >= f[2] || f[2] > f[3])
               		err("Filter #%d has bad frequencies", j - jmin + 1);
                makefilter(f, nfft, nfreq, dt, filter[j]);
          }
        }
	

	/* User may not have given a filter for tmin and/or tmax--	*/
	/* Extend array so can always assume these filters are present.	*/
	/* Note don't really use any of the extra storage in **filter!	*/
	if (itf[jmin] > 0) {
		filter[jmin-1] = filter[jmin]; 
		itf[jmin-1] = 0;
		--jmin;
	}
	if (itf[jmax] < nt - 1) {
		filter[jmax+1] = filter[jmax];
		itf[jmax+1] = nt - 1;
		++jmax;
	}
	
	
	/* Extend array so can always consider time points to be interior */
	itf[jmin-1] = 0;      /* now jmin - 1 is a valid index */
	itf[jmax+1] = nt - 1; /* now jmax + 1 is a valid index */


        /* Main loop over traces */
        ftrace = ealloc2float(nt, nfilter+4); /* never use 1st & last */
        do {
		float *data = ealloc1float(nt);
                register int i, j;
		
		/* Construct filtered sub-traces */
		for (j = jmin; j <= jmax; ++j) {			
			bzero(data, nt*FSIZE);
			for (i = itf[j-1]; i <= itf[j+1]; ++i)
				data[i] = tr.data[i];
                        bandpass(data,nt,nfft,nfreq,filter[j],ftrace[j]);
                }
		               
               /* Compose filtered trace from sub-traces */
               for (j = jmin; j < jmax; ++j) {
	       		float fitfj;
                        for (fitfj = i = itf[j]; i <= itf[j+1]; ++i) {
                                float a = (i - fitfj)/(itf[j+1] - fitfj);
                                tr.data[i] = (1-a)*ftrace[j][i] +
                                                 a*ftrace[j+1][i];
			}
                }
                
                puttr(&tr);
        } while (gettr(&tr));

        return EXIT_SUCCESS;
}

void bandpass(float *data, int nt, int nfft, int nfreq,
		float *filterj, float *ftracej)
{
	float *rt = ealloc1float(nt);
	complex *ct = ealloc1complex(nfreq);
	register int i;

        /* Load trace into rt (zero-padded) */
        memcpy(rt, data, nt*FSIZE);
        bzero(rt + nt, (nfft-nt)*FSIZE);

        /* FFT, filter, inverse FFT */
        pfarc(1, nfft, rt, ct);
        for (i = 0; i < nfreq; ++i)  ct[i] = crmul(ct[i], filterj[i]);
        pfacr(-1, nfft, ct, rt);

        /* Load traces back in, recall filter had nfft factor */
        for (i = 0; i < nt; ++i)  ftracej[i] = rt[i]; /* ftracej = rt ?? */
}


void makefilter(float *f, int nfft, int nfreq, float dt, float *filter)
{
        float onfft = 1.0 / nfft;
        float df = onfft / dt;
        int nfreqm1 = nfreq - 1;
        int if1 = NINT(f[0]/df);
        int if2 = NINT(f[1]/df);
        int if3 = MIN(NINT(f[2]/df), nfreqm1);
        int if4 = MIN(NINT(f[3]/df), nfreqm1);


        { register int i;
	  register float c = PIBY2 / (if2 - if1 + 2);
	  for (i = if1; i <= if2; ++i) {
		register float s = sin(c*(i - if1 + 1));
		filter[i] = s * s * onfft;
          }
	 }

        { register int i;
	  register float c = PIBY2 / (if4 - if3 + 2);
	  for (i = if3; i <= if4; ++i) {
		register float s = sin(c*(if4 - i + 1));
		filter[i] = s * s * onfft;
	  }
        }

        { register int i;
          for (i = if2 + 1; i < if3;   ++i)  filter[i] = onfft; 
          for (i = 0;       i < if1;   ++i)  filter[i] = 0.0; 
          for (i = if4 + 1; i < nfreq; ++i)  filter[i] = 0.0; 
        }
}
