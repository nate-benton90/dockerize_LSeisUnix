#include<stdio.h>
#include "su.h"
#include "segy.h"
#include "suhdr.h"

#define LOOKFAC	2	/* Look ahead factor for npfaro	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */

int bp_filter(float f1,float f2,float f3, float f4, 
	      float *tr, float dt, int nt)
/* Band pass filter
   f1,f2,f3,f4 corner frequencies
   tr float array to filter
   dt smaple intervall
   nt number of samples
   Padd the trace with 25% of its length before fft 

*/
{
        float *rt;     /* real trace                           */
        complex *ct;   /* complex transformed trace            */
        float *filter;          /* filter array                         */
        float *f;               /* array of filter frequencies		*/
        int npoly;              /* .... sizes of f and intfr	        */
        float *amps;            /* array of amplitude values		*/
        int namps;              /* .... size of amps                    */
        int iamps;   		/* loop counting variables              */
        int nfft;               /* number of points for fft trace       */
        int nf;                 /* number of frequencies (incl Nyq)     */
	int i;
	int ntp;		/* padded trace length */
	int irts;		/* start element of real trace in paded trace */
	float pad=1.25;

	
	ntp = NINT(nt*pad);
	irts = NINT(0);
	
        /* Set up FFT parameters */
        nfft = npfaro(ntp, LOOKFAC * ntp);
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

        memset((void *)        rt, (int) '\0' ,  irts*FSIZE);
        memcpy((void *) &rt[irts], (const void *) tr, nt*FSIZE);
        memset((void *)   &rt[nt], (int) '\0' , (nfft-nt)*FSIZE);

        /* FFT, filter, inverse FFT */
        pfarc(1, nfft, rt, ct);
        for (i = 0; i < nf; ++i)  ct[i] = crmul(ct[i], filter[i]);
        pfacr(-1, nfft, ct, rt);

       /* Load traces back in, recall filter had nfft factor */
        memcpy((void *) tr, (const void *) &rt[irts], nt*FSIZE);
       
       free1float(f);
       free1float(amps);
       free1float(rt);
       free1float(filter);
       free1complex(ct);
       
       return EXIT_SUCCESS;
}

int bp_filter_padd(float f1,float f2,float f3, float f4, 
	      float *tr, float dt, int nt,float fftpad)
/* Band pass filter
   f1,f2,f3,f4 corner frequencies
   tr float array to filter
   dt smaple intervall
   nt number of samples
   Padd the trace with 25% of its length before fft 

*/
{
        float *rt;     /* real trace                           */
        complex *ct;   /* complex transformed trace            */
        float *filter;          /* filter array                         */
        float *f;               /* array of filter frequencies		*/
        int npoly;              /* .... sizes of f and intfr	        */
        float *amps;            /* array of amplitude values		*/
        int namps;              /* .... size of amps                    */
        int iamps;   		/* loop counting variables              */
        int nfft;               /* number of points for fft trace       */
        int nf;                 /* number of frequencies (incl Nyq)     */
	int i;
	int ntp;		/* padded trace length */
	int irts;		/* start element of real trace in paded trace */
	float pad=1.25;

	
	ntp = nt;
	irts = NINT(0);
	
        /* Set up FFT parameters */
        nfft = npfar(NINT(ntp*(1.0+fftpad/100.0)));
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

        memset((void *)        rt, (int) '\0' ,  irts*FSIZE);
        memcpy((void *) &rt[irts], (const void *) tr, nt*FSIZE);
        memset((void *)   &rt[nt], (int) '\0' , (nfft-nt)*FSIZE);

        /* FFT, filter, inverse FFT */
        pfarc(1, nfft, rt, ct);
        for (i = 0; i < nf; ++i)  ct[i] = crmul(ct[i], filter[i]);
        pfacr(-1, nfft, ct, rt);

       /* Load traces back in, recall filter had nfft factor */
        memcpy((void *) tr, (const void *) &rt[irts], nt*FSIZE);
       
       free1float(f);
       free1float(amps);
       free1float(rt);
       free1float(filter);
       free1complex(ct);
       
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

