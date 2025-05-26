/* Modified as a callable c function */
/* B. Nemeth, Saskatoon */


/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */

/* SUFILTER: $Revision: 1.15 $ ; $Date: 1998/08/24 20:10:26 $        */

/* Prototype of function used internally */
void polygonalFilter(float *f, float *amps,
			int npoly, int nfft, float dt, float *filter);

#define PIBY2   1.57079632679490
#define LOOKFAC 2       /* Look ahead factor for npfao    */
#define PFA_MAX 720720  /* Largest allowed nfft           */

#include "su.h"
#include "segy.h"


int bp_filter(float f1,float f2,float f3, float f4, 
	      float *tr, float dt, int nt)
{
        register float *rt;     /* real trace                           */
        register complex *ct;   /* complex transformed trace            */
        float *filter;          /* filter array                         */
        float *f;               /* array of filter frequencies		*/
        int npoly;              /* .... sizes of f and intfr	        */
        float *amps;            /* array of amplitude values		*/
        int namps;              /* .... size of amps                    */
        int icount,ifs,iamps;   /* loop counting variables              */
        float nyq;              /* nyquist frequency                    */
        int nfft;               /* number of points for fft trace       */
        int nf;                 /* number of frequencies (incl Nyq)     */
	cwp_Bool seismic;	/* is this seismic data?		*/


	
        /* Set up FFT parameters */
        nfft = npfaro(nt, LOOKFAC * nt);
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                err("Padded nt=%d -- too big", nfft);

        nf = nfft/2 + 1;

        npoly = 4;
        f = ealloc1float(npoly);

        f[0] = f1;
        f[1] = f2;
        f[2] = f3;
        f[3] = f4;

	
        namps = npoly;
        amps = ealloc1float(namps);

	/* default is a trapezoidal bandpass filter */
	for(iamps=0; iamps<namps; ++iamps)
        amps[iamps]=1.;
		
	amps[0]=0.; amps[namps-1]=0.;
	


        /* Allocate fft arrays */
        rt   = ealloc1float(nfft);
        ct   = ealloc1complex(nf);
        filter = ealloc1float(nf);

	/* Build the polygonal filter filter[]*/
	polygonalFilter(f,amps,npoly,nfft,dt,filter);

        memcpy((void *) rt, (const void *) tr, nt*FSIZE);
        memset((void *) (rt + nt), (int) '\0' , (nfft-nt)*FSIZE);

        /* FFT, filter, inverse FFT */
        pfarc(1, nfft, rt, ct);
        for (i = 0; i < nf; ++i)  ct[i] = crmul(ct[i], filter[i]);
        pfacr(-1, nfft, ct, rt);

       /* Load traces back in, recall filter had nfft factor */
        memcpy((void *) tr, (const void *) rt, nt*FSIZE);
       
       return EXIT_SUCCESS;
}


void polygonalFilter(float *f, float *amps, int npoly,
				int nfft, float dt, float *filter)
/*************************************************************************
polygonalFilter -- polygonal filter with sin^2 tapering
**************************************************************************
Input:
f		array[npoly] of frequencies defining the filter
amps		array[npoly] of amplitude values
npoly		size of input f and amps arrays
dt		time sampling interval
nfft		number of points in the fft

Output:
filter		array[nfft] filter values
**************************************************************************
Notes: Filter is to be applied in the frequency domain
**************************************************************************
Author:  CWP: John Stockwell   1992
*************************************************************************/
#define PIBY2   1.57079632679490
{
        int *intfr;             /* .... integerizations of f		*/
        int icount,ifs;		/* loop counting variables              */
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
        for(ifs=0; ifs < npoly ; ++ifs) {
                intfr[ifs] = NINT(f[ifs]/df);
                if (intfr[ifs] > nfm1) intfr[ifs] = nfm1;
        }

	/* Build filter, with scale, and taper specified by amps[] values*/
	/* Do low frequency end first*/
	for(icount=0; icount < intfr[0] ; ++icount) 
		filter[icount] = amps[0] * onfft;

	/* now do the middle frequencies */
	for(ifs=0 ; ifs<npoly-1 ; ++ifs){
	   if(amps[ifs] < amps[ifs+1]) {	
		++taper;
		for(icount=intfr[ifs]; icount<=intfr[ifs+1]; ++icount) {
		    float c = PIBY2 / (intfr[ifs+1] - intfr[ifs] + 2);
		    float s = sin(c*(icount - intfr[ifs] + 1));
		    float adiff = amps[ifs+1] - amps[ifs];
		    filter[icount] = (amps[ifs] + adiff*s*s) * onfft;
		}
	   } else if (amps[ifs] > amps[ifs+1]) {	
		++taper;
		for(icount=intfr[ifs]; icount<=intfr[ifs+1]; ++icount) {
			   float c = PIBY2 / (intfr[ifs+1] - intfr[ifs] + 2);
                	   float s = sin(c*(intfr[ifs+1] - icount + 1));
			   float adiff = amps[ifs] - amps[ifs+1];
                	   filter[icount] = (amps[ifs+1] + adiff*s*s) * onfft;
		  }
	   } else 
		if(!(taper)){
		for(icount=intfr[ifs]; icount <= intfr[ifs+1]; ++icount)
		   	   filter[icount] = amps[ifs] * onfft;
		} else {
		for(icount=intfr[ifs]+1; icount <= intfr[ifs+1]; ++icount)
		   	   filter[icount] = amps[ifs] * onfft;
		}
	}

	/* finally do the high frequency end */
	for(icount=intfr[npoly-1]+1; icount<nf; ++icount){
		filter[icount] = amps[npoly-1] * onfft;
	}

}
