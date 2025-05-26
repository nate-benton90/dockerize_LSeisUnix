/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

/* SUFKF: $Revision: 0. $ ; $Date: 1998/12/3:36:46 $      */

#include "su.h"
#include "segy.h"
#include <signal.h>
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                   ",
" SURADON-PWS: Chapman's 1980 Generalized radon transform (Forward) ",
"              Includes a phase stacking coherence filter in the    ",
"              stacking procedure (tau-p Slant-stack)               ",
"                                                                   ",
" suRADON-PWS <infile >outfile [optional parameters]                ",
"                                                                   ",
" Required parameters:                                              ",
"                                                                   ",
"      key  = Sorted order of the data                              ",
"      pmn  = Minimum ray parameter (1/Vmax- Vmax in km/s)          ",
"      pmx  = Maximum ray parameter (1/Vmin- Vmin in km/s)          ",
"      np   = Number of ray parameters                              ",
"      ntrs = Max Number of traces per ensamble                     ",
"      perc = Precentage bellow which Enveloped is zeroed out       ",
"      off  = Offset of the transformation ususally 0               ",
"      isl  = Length of the smoothing window                        ",
"      pwr  = Power of the gain for Phase Stacking                  ",
"      mode = 0 Output the taupe                                    ",
"      mode = 1 Output in same file the coherence factors           ",
"                                                                   ",
" Optional parameters:                                              ",
"                                                                   ",
" verbose=0   verbose = 1 echoes information                        ",
"                                                                   ",
" Important !!!                                                     ",
"                                                                   ",
" References:                                                       ",
"    Kennett, B.L.N. 2000: Stacking three-component seismograms.    ",
"      Geophysical Journal International, vol. 141, p. 263-269.     ",
"    Schimmel, M., and H. Paulssen, 1997: Noise reduction and       ",
"      detection of weak, coherent signals through                  ", 
"      phase-weighted stacks, Geophysical Journal International,    ",
"      vol. 130, p. 497-505.                                        ",
"    Taner, M.T., A. F. Koehler, and R. E. Sheriff, 1979: Complex   ",
"      seismic trace analysis. Geophysics, vol. 44, p. 1041-1063.   ",
"                                                                   ",
NULL};

/* Credits:
 *
 *   IJA: Ramon Carbonell (algorithm), 
 *
 * Trace header fields accessed: ns, dt, d2
 * Trace header fields modified: tracl, ns, dt, trid, d1, f1, d2, f2
 */
/**************** end self doc ***********************************/

#define LOOKFAC 2  
#define PFA_MAX   720720   /* Largest allowed ns2             */

/* Function prototypes */

float rscl(float *r, int n, float rms);
int writeatrace(char *fvl,float *v, int n1);
void do_smooth(float *data, int nt, int isl);
void RadonChapmanPS(float *dtx, float *dtp, float *x, float dt, float off, float pwr, float pmn, 
                    float pmx, int np, int ns, int ns2, int ntr, int isl, float *r, complex  *s, 
		    complex *s0, complex *cr, complex *h, float *hr, complex *cps, float *ps);

/* Globals (so can trap signal) defining temporary disk files */

segy intrace, outtrace;

int
main(int argc, char **argv)
{
   int j,i,ns,ntr,ntrs,ns0,ns2,islw;                /* numbers of samples,traces   */
   int ip,np,iky=0,itmp,mode=0,isl;               /* sample indices         */
   float *dtx,*dtp,dt,pmn,pmx,*x,dp,perc,off,pwr; /* Working Arrays        */
   cwp_String key[SU_NKEYS];                      /* array of keywords */
   cwp_String typ[SU_NKEYS]; 
   int indx[SU_NKEYS];                             /* name of type of getparred key */
   Value val;                                      /* value of key field */
   float *r,*hr,*ps,*rmstr,slw,dtrmsi=0,dtrmso=0;               /* Working Arrays        */
   register complex *s,*s0,*h,*cps,*cr;                /* Working Arrays        */

   initargs(argc,argv);
   requestdoc(1);

   /* Get info from first trace */ 
   if (!gettr(&intrace))  err("can't get first trace");
   ns  = intrace.ns;
   dt  = ((double) intrace.dt)/1000000.0;
   if (!getparstringarray("key",key)) key[0]="fldr";
   if (!getparfloat("pmn",&pmn)) pmn=0.0;
   if (!getparfloat("pmx",&pmx)) pmx=0.5; 
   if (!getparfloat("off",&off)) off=0.0; 
   if (!getparint("isl",&isl)) isl=0; 
   if (!getparfloat("pwr",&pwr)) pwr=1.0; 
   if (!getparfloat("perc",&perc)) perc=0.0; 
   if (!getparint("np", &np)) np=100;
   if (!getparint("ntrs",&ntrs)) ntrs=100;
   if (!getparint("mode", &mode)) mode=0;
   typ[0]=hdtype(key[0]);
   indx[0]=getindex(key[0]);
   gethval(&intrace, indx[0], &val);
   iky  = (int) vtof(hdtype(key[0]), val);
   dp   =(pmx-pmn)/(np-1);
   ntr = 0;
   ns0 = ns*2;
   ns2 = npfaro(ns0, LOOKFAC * ns0);
   warn("Dimensions:                 Trace ns -> %i ",ns);
   warn("         Power of 2 used for the FFT -> %i",ns2);
   warn("         Max. Power PFA_MAX -> 720720");
   dtx   = ealloc1float(ns2*ntrs);

   dtp   = ealloc1float(2*np*ns);
   hr    = ealloc1float(ns2*ntrs);
   r     = ealloc1float(ns2*ntrs);
   x     = ealloc1float(ntrs);
   s0    = ealloc1complex(ns2*ntrs);
   h     = ealloc1complex(ns2*ntrs);

   if (!(ps=    (float*)malloc(np*ns*sizeof(float)))) err(" *malloc* ps failed\n");
   if (!(cps= (complex*)malloc(np*ns*sizeof(complex)))) err(" *malloc* cps failed\n");
   if (!(cr=  (complex*)malloc(ns2*ntrs*sizeof(complex)))) err(" *malloc* cr failed\n");
   if (!(s=   (complex*)malloc(ns2*ntrs*sizeof(complex)))) err(" *malloc* s failed\n");

   do {
       gethval(&intrace, indx[0], &val);
       itmp = ((int) vtof(hdtype(key[0]), val));
       if (iky == itmp) {
           x[ntr]=(float)intrace.offset/1000;
           bcopy(intrace.data, &dtx[ntr*ns2], ns*sizeof(float));
           ntr++; 
       } 
   } while (gettr(&intrace));
   for (j=0; j<ns2*ntr; j++) dtrmsi += dtx[j]*dtx[j];
   dtrmsi=sqrt(dtrmsi/(ntr*ns2));

   RadonChapmanPS(dtx,dtp,x,dt,off,pwr,pmn,pmx,np,ns,ns2,ntr,isl,r,s,s0,cr,h,hr,cps,ps);

   for (j=0; j<ns*np; j++) dtrmso += dtp[j]*dtp[j];
   dtrmso=sqrt(dtrmso/(np*ns));
   for (j=0; j<ns*np; j++) dtp[j]=(dtp[j]*dtrmsi)/dtrmso;

   for (ip=0; ip<np; ip++) { 
        bcopy(&dtp[ip*ns], outtrace.data, ns*sizeof(float));
/* 
   Output Tau P transformed data
   set header values 
*/
	slw=pmn+ip*dp;
	islw=(int)(10/slw);
        outtrace.tracl = ip+1;
        outtrace.tracr = ip+1;
        outtrace.tracf = ip+1;
        outtrace.ns = ns;
        outtrace.dt = intrace.dt;  /* d1 is now the relevant step size */
        outtrace.trid = AMPLITUDE;
        outtrace.d1 = dt;
        outtrace.fldr = intrace.fldr;
        outtrace.f1 = 0;
        outtrace.d2 = dp;
        outtrace.f2 = pmn;
        outtrace.offset = (int)(slw*100000);
	if (slw > 0.0000000001) outtrace.sx = islw;
        puttr(&outtrace);
   }
   if (mode) {
      for (ip=0; ip<np; ip++) {  
           bcopy(&dtp[(np+ip)*ns], outtrace.data, ns*sizeof(float));
/* 
   Output Semblance  
      if (perc) for (i=0;i<np*ns;i++) if (dtp[np*ns+i] < (1./perc)) dtp[np*ns+i]=0.0;
   set header values 
*/
	   slw=pmn+ip*dp;
	   islw=(int)(10/slw);
           outtrace.tracl = ip+1;
           outtrace.tracr = ip+1;
           outtrace.tracf = ip+1;
           outtrace.ns = ns;
           outtrace.dt = intrace.dt;  /* d1 is now the relevant step size */
           outtrace.trid = ENVELOPE;
           outtrace.d1 = dt;
           outtrace.fldr = intrace.fldr;
           outtrace.f1 = 0;
           outtrace.d2 = dp;
           outtrace.f2 = pmn;
           outtrace.offset = (int)(slw*100000);
	   if (slw > 0.0000000001) outtrace.sx = islw;
           puttr(&outtrace);
      }
   }
   return EXIT_SUCCESS;
}

void RadonChapmanPS(float *dtx, float *dtp, float *x, float dt, float off, float pwr, float pmn, 
                    float pmx, int np, int ns, int ns2, int ntr, int isl, float *r, complex  *s, 
		    complex *s0, complex *cr, complex *h, float *hr, complex *cps, float *ps)
/*******************************************************************************************
 * Generalized Radon (Tau-P) transfrom based in Chapmand 1989 
 * Includes in the stack of the shifted traces the PWS Phase Weigthing Stack of Schimmel and
 * Paulsen, 1997.
 * *****************************************************************************************
 * Function parameters:
 *
 * float   dtx[ntr*ns2] input data ntr*ns (number traces * number samples) 
 * float   dtp[2*np*ns] output data ntp*ns (number slowness * number samples) 
 * float   x[ntr]       array with offsets
 * float   dt           sample rate
 * float   pwr          Power of the coherence factors
 * float   pmn          min ray parameter slowness
 * float   pmx          max ray parmater slowness
 * int     isl          length of the smoothing window for the coherence factors of the PWS
 * int     np           number of ray parameters (slowness)                                  
 * complex cr[ntr*ns]   analytical function of the shifted trace dimensions ntr*ns
 * complex h[nf*ntr]    analytical function of the shifted trace dimensions ns2
 * complex s[ntr*nf]    storage array required dimensions ntr*ns2
 * complex s0[ntr*nf]   storage array 
 * float   r[ntr*ns2]   real part of hilbert transform of r (shifted trace) dimesions ntr*ns2
 * float   hr[ntr*ns2]  real part of hilbert transform of r (shifted trace) dimesions ntr*ns2
 * Fomr phase stacking  analytical processing
 * complex cps[np*ns]   complex gain function derived from the phase stack  
 * float ps[np*ns]      real gain function derived from phase stack
 *
 * *****************************************************************************************
 * Author: Ramon Carbonell  CSIC-Inst. Earth  Sciences 
 * *****************************************************************************************/
{   
   int ix,ip,j,i;
   int nf,nshft;
   float pi,fny,dp,df,p,f;
   float cs,sn,a,b,rshft;           

   pi  = 8.0*atan(1.0e0);
   fny = 1.0/(2.0*dt);
   nf  = ns2/2+1;
   df  = fny/(nf-1);
   dp=(pmx-pmn)/(np-1);

   bzero(s0,nf*ntr*sizeof(complex));
   bzero(s,nf*ntr*sizeof(complex));
   bzero(h,nf*ntr*sizeof(complex));
   bzero(r,ntr*ns2*sizeof(float));
   bzero(hr,ntr*ns2*sizeof(float));
   bzero(cr,ntr*ns*sizeof(complex));
   bzero(cps,np*ns*sizeof(complex));
   bzero(dtp,2*np*ns*sizeof(float));

   pfa2rc (1,1,ns2,ntr,dtx,s);
   for (ip=0; ip<np; ip++) {
        /* Clean arrays */
        p=pmn+ip*dp;
        for (ix=0; ix<ntr; ix++) {
             rshft=p*(x[ix]-off);
             nshft=rint(rshft/dt);
             if (nshft < ns){
#pragma omp parallel for private(j,f,cs,sn,a,b) shared(nf,df,rshft,ns2,ix,s,s0,h)
                 for (j=0; j<nf; j++) {
	              f=pi*j*df*rshft;
	              cs=cos(f);
	              sn=sin(f);
                      a=s[ix*nf+j].r;
                      b=s[ix*nf+j].i;
                      s0[ix*nf+j].r = a*cs + b*sn;
                      s0[ix*nf+j].i = b*cs - a*sn;
		      /* Start Hilbert transform */
                      h[ix*nf+j].r =  s0[ix*nf+j].i;
                      h[ix*nf+j].i = -s0[ix*nf+j].r;
		      /* End Hilbert transform */
	         }
             }
             if (nshft > ns) bzero(&s0[ix*ns2],ns2*sizeof(float));
        }
        pfa2cr(-1,1,ns2,ntr,s0,r);
        pfa2cr(-1,1,ns2,ntr,h, hr);
        for (ix=0; ix<ntr; ix++) {
  /* build the complex trace, analytical function and get rid of the amplitude, normalize */
#pragma omp parallel for private(j,a) shared(cr,r,hr,ns,ns2,ip)
	     for (j=0; j<ns; j++) {  
		 a=sqrt(r[ix*ns2+j]*r[ix*ns2+j]+hr[ix*ns2+j]*hr[ix*ns2+j]);
		 cr[ix*ns+j].r=r[ix*ns2+j]/a;
		 cr[ix*ns+j].i=hr[ix*ns2+j]/a;
             }
        } 
	/* Stack */
#pragma omp parallel for private(j,ix) shared(dtp,ntr,cr,r,ns,ns2,ip)
        for (ix=0; ix<ntr; ix++)  for (j=0; j<ns; j++) {  
	          dtp[ip*ns+j]   += r[ix*ns2+j];
		  cps[ip*ns+j].r += cr[ix*ns+j].r;
		  cps[ip*ns+j].i += cr[ix*ns+j].i;
        }
        /* convert complex phase stack to real weights */
#pragma omp parallel for private(j) shared(ps,cps,ns,ip,pwr)
        for (j=0;j<ns;j++) {  
             ps[ip*ns+j]=sqrt(cps[ip*ns+j].r*cps[ip*ns+j].r+cps[ip*ns+j].i*cps[ip*ns+j].i);
             if (pwr) ps[ip*ns+j] = pow(ps[ip*ns+j],pwr);
        }
        /* smooth phase-stack (weights) */
        if (isl) do_smooth(&ps[ip*ns],ns,isl);
        /* apply weights to "ordinary" stack (do PWS) */
#pragma omp parallel for private(j) shared(dtp,ps,np,ns,ip)
        for (j=0; j<ns; j++) {
	     dtp[ip*ns+j]     *= ps[ip*ns+j];
	     dtp[(np+ip)*ns+j] = ps[ip*ns+j];
        }
   }
}

void do_smooth(float *data, int nt, int isl)
/**********************************************************************
do_smooth - smooth data in a window of length isl samples
**********************************************************************
Input:
data[]          array of floats of size nt
nt              size of array
isl             integerized window length
Output:
returns smoothed data.

**********************************************************************
Author: Nils Maercklin,
         GeoForschungsZentrum (GFZ) Potsdam, Germany, 2001.
         E-mail: nils@gfz-potsdam.de
**********************************************************************/
{
    register int it,jt;
    float *tmpdata, sval;

    tmpdata=ealloc1float(nt);
    for (it=0;it<nt;it++) {
        sval=0.0;
        if ( (it >= isl/2) && (it < nt-isl/2) ) {
        for (jt=it-isl/2;jt<it+isl/2;jt++) {
             sval += data[jt];
        }
        tmpdata[it] = sval / (float) isl;
        } else {
          tmpdata[it] = data[it];
        }
    }
    memcpy((void *) data, (const void *) tmpdata, nt*FSIZE);
    free1float(tmpdata);
}

/*******************************************************************************************
 * Write a trace                       
 *****************************************************************************************
        if (ip==10) writeatrace("trc3-cps",&ps[ip*ns],ns);
   Subroutine: readVEL.c
   Function:  Read velocity from a file
   Author: R.C.
   Date: 8-2005
   Variables: fl  --- File name of the velocity file
              n   --- size of the velocity array
              v   --- output velocity model (double precission array)
*/

int writeatrace(char *fvl,float *v, int n1)

{
   FILE *f0;
   int  icnt,n;
   char ftmp[80];
   n=n1;
      	   
   sprintf(ftmp,"%s.bin",fvl);
   if ((f0=fopen(ftmp,"w"))==NULL) err("Cannot open Velocity model file =%s",ftmp);
   icnt=fwrite(v,n,sizeof(float),f0);
   fclose(f0);
   warn("CHECK file %s size n1-> %i",ftmp,n1);
   return icnt; 
}
/*******************************************************************************************
 * ReScale to rms
 *****************************************************************************************
   Subroutine: ReSacel.c
   Function:  Scale the array time series to the rms amplitude suplide as input
   Author: R.C.
   Date: 8-2005
   Variables: r     --- time series
              n     --- dimesion of time series 
              rms   --- New rms value
*/
float rscl(float *r, int n, float rms)
{
  float tmp=0,tmr;
  int j;

  for (j=0; j<n; j++) tmp += r[j]*r[j];
  tmp=sqrt(tmp/n);
  tmr=rms/tmp;
  for (j=0; j<n; j++) r[j] *=tmr;
  return tmp;
}
