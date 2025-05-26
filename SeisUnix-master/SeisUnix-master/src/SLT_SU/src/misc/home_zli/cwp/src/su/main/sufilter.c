/* SUFILTER: $Revision: 1.2 $ ; $Date: 92/10/22 14:15:45 $        */

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

void buildpolyfilt(float *f, float *amps, int npoly, int nfft, 
					float dt, float *filter);

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"									",
" SUFILTER - applies a zero-phase, sine-squared tapered filter		",
"									",
" sufilter <stdin >stdout [optional parameters]         		",
"									",
" Required parameters:                                         		",
"       if dt is not set in header, then dt is mandatory        	",
"									",
" Optional parameters:							",
"       f=f1,f2,...             array of filter frequencies(HZ) 	",
"       amps=a1,a2,...          array of filter amplitudes		",
"       dt = (from header)      time sampling rate (sec)        	",
"									",
" Defaults:f=.10*(nyquist),.15*(nyquist),.45*(nyquist),.50*(nyquist)	",
"                        (nyquist calculated internally)		",
"          amps=0.,1.,...,1.,0.  trapezoid-like bandpass filter		",
"									",
" Examples of filters:							",
" Bandpass:   sufilter <data f=10,20,40,50 | ...			",
" Bandreject: sufilter <data f=10,20,30,40 amps=1.,0.,0.,1. | ..	",
" Lowpass:    sufilter <data f=10,20,40,50 amps=1.,1.,0.,0. | ...	",
" Highpass:   sufilter <data f=10,20,40,50 amps=0.,0.,1.,1. | ...	",
" Notch:      sufilter <data f=10,12.5,35,50,60 amps=1.,.5,0.,.5,1. |..	",
NULL};
/**************** end self doc ***********************************/

/* Credits:
 *      CWP: John Stockwell, Jack Cohen
 *
 * Possible optimization: Do assignments instead of crmuls where
 * filter is 0.0.
 */


#define PIBY2   1.57079632679490
#define FRAC0   0.10    /* Ratio of default f1 to Nyquist */
#define FRAC1   0.15    /* Ratio of default f2 to Nyquist */
#define FRAC2   0.45    /* Ratio of default f3 to Nyquist */
#define FRAC3   0.50    /* Ratio of default f4 to Nyquist */
#define LOOKFAC 2       /* Look ahead factor for npfao    */
#define PFA_MAX 720720  /* Largest allowed nfft           */


segy tr;

main(int argc, char **argv)
{
        register float *rt;     /* real trace                           */
        register complex *ct;   /* complex transformed trace            */
        float *filter;          /* filter array                         */
        float *f;               /* array of filter frequencies		*/
        int *intfr;             /* .... integerizations of f		*/
        int npoly;              /* .... sizes of f and intfr	        */
        float *amps;            /* array of amplitude values		*/
        int namps;              /* .... size of amps                    */
        int icount,ifs,iamps;   /* loop counting variables              */
        float dt;               /* sample spacing                       */
        float nyq;              /* nyquist frequency                    */
        int nt;                 /* number of points on input trace      */
        int nfft;               /* number of points for fft trace       */
	int taper=0;		/* flag counter				*/
        int nf;                 /* number of frequencies (incl Nyq)     */
        float onfft;            /* reciprocal of nfft                   */
        float df;               /* frequency spacing (from dt)          */

        
        /* Initialize */
        initargs(argc, argv);
        requestdoc(1);


        /* Get info from first trace */ 
        if (!gettr(&tr))  err("can't get first trace");
        if (tr.trid && tr.trid != TREAL)
                err("input is not seismic data, trid=%d", tr.trid);
        nt = tr.ns;
        if (!getparfloat("dt", &dt))    dt = (float) tr.dt/1000000.0;
        if (!dt) err("dt field is zero and not getparred");
        nyq = 0.5/dt;


        /* Set up FFT parameters */
        nfft = npfaro(nt, LOOKFAC * nt);
        if (nfft >= MIN(SU_NFLTS, PFA_MAX))
                err("Padded nt=%d -- too big", nfft);

        nf = nfft/2 + 1;
        onfft = 1.0 / nfft;

        /* Get frequencies that define the filter */
        if ((npoly = countparval("f"))!=0) {
                f = ealloc1float(npoly);
                intfr = ealloc1int(npoly);
                getparfloat("f",f);
        } else {
                npoly = 4;
                f = ealloc1float(npoly);
                intfr = ealloc1int(npoly);

                f[0] = FRAC0 * nyq;
                f[1] = FRAC1 * nyq;
                f[2] = FRAC2 * nyq;
                f[3] = FRAC3 * nyq;
        }

	/* Check f values */
	if(npoly < 3) warn("Only %d value(s) defining filter",npoly);
        for(ifs=0; ifs < npoly-1; ifs++)
		if(f[ifs] < 0.0 || f[ifs] > f[ifs+1])
                                err("Bad filter parameters");
	
	/* Get filter amplitude values*/
        if ((namps = countparval("amps"))!=0) {
                amps = ealloc1float(namps);
                getparfloat("amps",amps);
        } else {
                namps = npoly;
                amps = ealloc1float(namps);

		/* default is a trapezoidal bandpass filter */
		for(iamps=0; iamps<namps; iamps++)
               		amps[iamps]=1.;
		
		amps[0]=0.; amps[namps-1]=0.;
        }
	if (!(namps==npoly)) 
		err("number of f values must = number of amps values");
        
        /* Check amps values */
        for(iamps = 0, icount=0; iamps < namps ; iamps++) {
		icount+=amps[iamps];
                if( amps[iamps] < 0.) err("amp values must be positive");
        }
        if (icount==0) err("All amps values are zero");
        for(iamps = 0, icount=0; iamps < namps-1 ; iamps++) {
			if(!(amps[iamps]==amps[iamps+1])) icount++;
	}
        if (icount==0) warn("All amps values are the same");


        /* Allocate fft arrays */
        rt   = ealloc1float(nfft);
        ct   = ealloc1complex(nf);
        filter = ealloc1float(nf);

	/* Build the polygonal filter filter[]*/
	buildpolyfilt(f,amps,npoly,nfft,dt,filter);

        /* Main loop over traces */
        do {
                register int i;

                /* Load trace into rt (zero-padded) */
                memcpy((char*)rt, (char*)tr.data, nt*FSIZE);
                bzero(rt + nt, (nfft-nt)*FSIZE);

                /* FFT, filter, inverse FFT */
                pfarc(1, nfft, rt, ct);
                for (i = 0; i < nf; ++i)  ct[i] = crmul(ct[i], filter[i]);
                pfacr(-1, nfft, ct, rt);

                /* Load traces back in, recall filter had nfft factor */
                for (i = 0; i < nt; ++i)  tr.data[i] = rt[i];

                puttr(&tr);
        } while (gettr(&tr));

        return EXIT_SUCCESS;
}

/* BUILDPOLYFILT  -- Build a POLYgonal FILTer with sin^2 tapering
 *			--filter to be applied in the frequency domain
 *Credits:   CWP: John Stockwell
 *	float *f		array of frequencies defining the filter
 *      float *amps;            array of amplitude values
 *      int npoly;              size of f/amps
 *      float dt;               sampling rate
 *      int nfft;               number of points in the fft
 *      float *filter;          array filter values
 */

void buildpolyfilt(float *f, float *amps, int npoly, int nfft, 
					float dt, float *filter)
#define PIBY2   1.57079632679490


{
        int *intfr;             /* .... integerizations of f		*/
        int icount,ifs,iamps;   /* loop counting variables              */
        float nyq;              /* nyquist frequency                    */
        int nt;                 /* number of points on input trace      */
	int taper=0;		/* flag counter				*/
        int nf;                 /* number of frequencies (incl Nyq)     */
        int nfm1;               /* nf-1                                 */
        float onfft;            /* reciprocal of nfft                   */
        float df;               /* frequency spacing (from dt)          */

        
	intfr=alloc1int(npoly);

        nf = nfft/2 + 1;
        nfm1 = nf - 1;
        onfft = 1.0 / nfft;

        /* Compute array of integerized frequencies that define the filter*/
        df = onfft / dt;
        for(ifs=0; ifs < npoly ; ifs++) {
                intfr[ifs] = NINT(f[ifs]/df);
                if (intfr[ifs] > nfm1) intfr[ifs] = nfm1;
        }

	/* Build filter, with scale, and taper specified by amps[] values*/
	/* Do low frequency end first*/
	for(icount=0; icount < intfr[0] ; icount++) 
		filter[icount] = amps[0] * onfft;

	/* now do the middle frequencies */
	for(ifs=0 ; ifs<npoly-1 ; ifs++){
	   if(amps[ifs] < amps[ifs+1]) {	
		taper++;
		for(icount=intfr[ifs]; icount<=intfr[ifs+1]; icount++) {
		    float c = PIBY2 / (intfr[ifs+1] - intfr[ifs] + 2);
		    float s = sin(c*(icount - intfr[ifs] + 1));
		    float adiff = amps[ifs+1] - amps[ifs];
		    filter[icount] = (amps[ifs] + adiff*s*s) * onfft;
		}
	   } else if (amps[ifs] > amps[ifs+1]) {	
		taper++;
		for(icount=intfr[ifs]; icount<=intfr[ifs+1]; icount++) {
			   float c = PIBY2 / (intfr[ifs+1] - intfr[ifs] + 2);
                	   float s = sin(c*(intfr[ifs+1] - icount + 1));
			   float adiff = amps[ifs] - amps[ifs+1];
                	   filter[icount] = (amps[ifs+1] + adiff*s*s) * onfft;
		  }
	   } else 
		if(!(taper)){
		for(icount=intfr[ifs]; icount <= intfr[ifs+1]; icount++)
		   	   filter[icount] = amps[ifs] * onfft;
		} else {
		for(icount=intfr[ifs]+1; icount <= intfr[ifs+1]; icount++)
		   	   filter[icount] = amps[ifs] * onfft;
		}
	}

	/* finally do the high frequency end */
	for(icount=intfr[npoly-1]+1; icount<nf; icount++){
		filter[icount] = amps[npoly-1] * onfft;
	}

}
