/* SU2LAYER: $Revision: 1.4 $ ; $Date: 91/10/24 11:59:09 $      */

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

#define FOURPI 4.0*PI
#define LOOKFAC 2       /* Look ahead factor for npfao    */
#define PFA_MAX 720720  /* Largest allowed nfft           */


/*********************** self documentation **********************/
String sdoc = "\
SU2LAYER - Q&D to implement 1D Book problem (sec 2.2.4)         \n\
                                                                \n\
su2layer [optional parameters] > out_data_file                  \n\
                                                                \n\
Optional Parameters:                                            \n\
        c0=1.5          surface speed                           \n\
        c1=2.0          speed in layer                          \n\
        c2=2.2          basement speed                          \n\
        h1=0.5          depth of first layer                    \n\
        h2=1.0          depth of second layer                   \n\
        nm=10           number of multiples	                \n\
        dt=0.004        sampling interval                       \n\
                                                                \n\
";
/**************** end self doc ***********************************/

/* Credits:
 *      CWP:Jack
 */


segy tr;

main(int argc, char **argv)
{
        float *rt;
        complex *ct;
        float c0,c1,c2,cmin;
        float h1,h2,hmax;
        float R1,R2,tautemp,phasetemp;
        float dt;
	int nt,nm,nfft,nf,nfm1,ifreq,i;
        float df,onfft;



        /* Initialize */
        initargs(argc, argv);
        askdoc(0);
        

        /* set parameters and fill header fields */
        if (!getparfloat("c0", &c0))    c0 = 1.5;
        if (!getparfloat("c1", &c1))    c1 = 2.0;
        if (!getparfloat("c2", &c2))    c2 = 2.2;
        if (!getparfloat("h1", &h1))    h1 = 0.5;
        if (!getparfloat("h2", &h2))    h2 = 1.0;
        if (!getparfloat("dt", &dt))    dt = 0.004;  tr.dt = dt*1000000.0;
        if (!getparint("nm", &nm))      nm = 10;
     
	
        /* compute nt */
        hmax = h1 + nm*(h2 - h1)*c0/c1;
        cmin = MIN(c0, MIN(c1,c2)); 
        nt = 2.0*hmax / (cmin*dt);
        if (nt >= SU_NFLTS) err("computed nt=%d -- too big", nt);       
        tr.ns = nt;
        tr.tracl = 1;
        
        
        /* Set up FFT parameters */
        nfft = npfaro(nt, LOOKFAC * nt);
        if (nfft >= MIN(SU_NFLTS, PFA_MAX))
                err("Padded nt=%d -- too big", nfft);

        nf = nfft/2 + 1;
        nfm1 = nf - 1;
        onfft = 1.0 / nfft;
	df = onfft / dt;



        /* Compute u(omega) from (2.2.18,22) */
        ct = ealloc1complex(nf);
        rt = ealloc1float(nfft);

        R1=(c1-c0)/(c1+c0);
        R2=(c2-c1)/(c2+c1);
        tautemp = FOURPI*df*(h2 - h1)/c1;
	phasetemp = -FOURPI*df*h1/c0;
        for (ifreq = 0; ifreq < nf; ++ifreq) {
                float tau = tautemp * ifreq;
		float phase = phasetemp * ifreq;
		complex expiphase = cexp(cmplx(0.0, phase));
                complex expitau = cexp(cmplx(0.0, tau));
                float amptmp1 = 1/(PI*c0);
                complex amptmp2 = crmul(expitau, R2);
                complex amptmp3 = cadd(cmplx(R1, 0.0), amptmp2);
                complex amptmp4 = cadd(cmplx(1.0,0.0), crmul(amptmp2, R1));
		complex amp = crmul(cdiv(amptmp3,amptmp4),amptmp1);

                ct[ifreq] = cmul(amp, expiphase);
        }
                
 
 	/* Compute beta(t)--note that t = 2x/c0 */
	pfacr(-1, nfft, ct, rt);
	for (i = 0; i < nt; ++i)  tr.data[i] = rt[i];
  
        puttr(&tr);
        
        return EXIT_SUCCESS;
}
