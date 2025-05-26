/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

/* SUFKF: $Revision: 0. $ ; $Date: 1998/12/3:36:46 $      */

#include "su.h"
#include "segy.h"
#include <signal.h>
#include "header.h"
#include "bheader.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                                   ",
" SUSTRANSFORM:                                                                     ",
"                                                                                   ",
" suStransform <infile >outfile ncyl=3 mode=0                                       ",
"                                                                                   ",
" Required parameters:                                                              ",
"                                                                                   ",
"      ncyl  = number of cycles in the gaussian sigma=ncyl*wavelengt                ",
"      mode  = controls the output 0 outputs amplitude spectra trid=AMPLITUDE (118) ",
"                                                                                   ",
"               1 outputs Real and imaginary parts first trace real second imag     ",
"               Heades, trid are marked as REALPART=116 and IMAGPART=117            ",
"                                                                                   ",
" Important:                                                                        ",
"    Input one trace: the output consists of a matrix ensable of traces             ",
"    consisting of nf traces of length = ns, and d1=df                              ",
"     When mode = 0 One ensamble of nf tarces with trid=AMPLITUDE (118)             ",
"     When mode = 0 One ensamble of 2*nf tarces with trids=REALPART (116) and       ",
"                   trid=IMAGPART (117)                                             ",
"                                                                                   ",
" References:                                                                       ",
"    Stockwell, R. G., L. Mansinha, and R. P. Lowe (1996), Localization of the      ",
"       complex spectrum: The S-transform, IEEE Trans. Signal Process., 44,         ",
"       998- 1001.                                                                  ",
"    Simon, C., S. Ventosa, M. Schimmel, J. J. Daobeita, J. Gallart, and            ",
"       A. Manuel (2007), The S-transform and its inverses: Side effects of         ",
"       discretising and filtering, IEEE Trans. Signal Process,                     ",
"                                                                                   ",
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
float rmsvRC(float *r,int n);
int writeatrace(char *fvl,float *v, int n1);
void gausian(float *f, int n, float sigma,float a,float dt);
void sspctrmR(float *tx, float *rc, float *s, float *gss, int n, int ns2, float dt, int ncyl);
void strnsfrm(float *tx, complex *cs, float *s, float *ts, float *gss, float *tmp, int n, int ns2, float dt, int ncyl);

/* Globals (so can trap signal) defining temporary disk files */

segy intrc, outrc;

int
main(int argc, char **argv)
{
   int i,ns,nf,nt,ns0,ns2,ncyl,ntr=0,NF,mode=0,v;   /* numbers of samples,traces   */
   float dt,fny,df,F0,F1,DF,rms;                      /* Working Arrays        */
   float *dtx,*dts,*gss,*tmp,*ts;                    
   complex *cs;                                 

   initargs(argc,argv);
   requestdoc(1);

   /* Get info from first trace */ 
   if (!gettr(&intrc))  err("can't get first trace");
   if (!getparint("ncyl",&ncyl)) ncyl=3; 
   if (!getparint("verbose",&v)) v=0; 
   if (!getparint("mode",&mode)) mode=0; 

   ns  = intrc.ns;
   dt  = ((double) intrc.dt)/1000000.0;
   ns0 = ns*2;
   ns2 = npfaro(ns, LOOKFAC * ns);
   fny = 1.0/(2.0*dt);
   nf  = ns2/2+1;
   df  = fny/(nf-1);
   warn("Dimensions:                 Trace ns -> %i ",ns);

   F0 = 4/(ns*dt); 
   F1 = 1/(4*dt); 
   NF  = (int) ns2/4+1;
   DF  = (F1-F0)/NF;

   if (!(dtx =   (float*)malloc(ns2*sizeof(float))))        err(" *malloc* dtx failed\n");
   if (!(tmp =   (float*)malloc(ns2*ns*sizeof(float))))      err(" *malloc* tmp failed\n");
   if (!(gss =   (float*)malloc(ns2*sizeof(float))))        err(" *malloc* gss failed\n");
   if (!(dts =   (float*)malloc(nf*ns2*sizeof(float))))     err(" *malloc* dts failed\n");
   if (!(ts =    (float*)malloc(2*nf*ns2*sizeof(float))))   err(" *malloc* dts failed\n");
   if (!(cs  = (complex*)malloc(ns2*ns*sizeof(complex))))    err(" *malloc* cs failed\n");

   do {
       bcopy(&intrc,&outrc,SEGY_HDRBYTES);
       bcopy(intrc.data, dtx, ns*sizeof(float));
       ntr++; 
       rms=rmsvRC(dtx,ns);
       if (v) warn(" Trace # -> %i %f",ntr,rms);
       strnsfrm(dtx, cs, dts, ts, gss, tmp, ns, ns2, dt, ncyl);

       for (i=0; i<NF; i++) { 
/* 
   Output S-spectra transformed data frequency as a function of time
   set header values 
*/
            outrc.d1 = dt;
            outrc.swdep = (int)(rms*1000);
            outrc.ep = intrc.tracf;
            outrc.f1 = 0;
            outrc.d2 = DF;
            outrc.f2 = F0;
            outrc.offset = (int)((F0+i*DF)*1000);
            if ((mode==0) || (mode==3)){
               outrc.duse = 3;
               outrc.ntr = NF;
               bcopy(&dts[i*ns], outrc.data, ns*sizeof(float));
               outrc.trid = AMPLITUDE;
           }
           if ((mode==1) || (mode==2)){
               outrc.duse = 4;
               outrc.ntr = 2*NF;
	       bcopy(&ts[2*i*ns], outrc.data, ns*sizeof(float));
               outrc.trid = REALPART;
               puttr(&outrc);
               bcopy(&ts[(2*i+1)*ns], outrc.data, ns*sizeof(float));
               outrc.trid = IMAGPART;
	   }
           puttr(&outrc);
       }
   } while (gettr(&intrc));

   return EXIT_SUCCESS;
}
/*******************************************************************************************/

void strnsfrm(float *tx, complex *cs, float *s, float *ts, float *gss, float *tmp, 
              int n, int ns2, float dt, int ncyl)
/*-------------------------------------------------------------------------------------------
   int      ns2          Power of 2 of the length of the trace array for the FFT
   int      n            length of the trace array
   int      ncyl         number of cycles in the standard deviation sigma of the gausian function 
   float    dt           time sample rate in seconds
   float    tx(n)        float data array input
   float    gss(n)       float Gaussian function same length os the time array
   float    tmp(ns2*n)     float Result of product of gaussian and the data array
   float    s(nf*ns2)    float Output Matrix Spectra as a fuction of time 
   float    ts(2*nf*ns2) float Output Real and Imaginary parts of the S-transform
   complex  cs(nf*n)      complex temporal storage out of the FFT 
-------------------------------------------------------------------------------------------*/
{
   int i, j, k, nf, nnf,i0,i1,icnt=0;
   float t, fny,  df, wl,sigma,F0,F1;

   F0 = 4/(n*dt); 
   F1 = 1/(4*dt); 
   nf  = (int) ns2/4+1;
   nnf = (int) ns2/2+1;
   df  = (F1-F0)/nf;
   
   bzero(gss,ns2*sizeof(float)); 
   bzero(tmp,ns2*n*sizeof(float)); 
   bzero(s,nf*ns2*sizeof(float)); 
   bzero(ts,2*nf*ns2*sizeof(float)); 

/* ------------------------------------------------------------*/
   for (i=0; i<nf; i++) {     /* Fix the frequency */
       sigma=ncyl/(F0+i*df);
       for (j=0; j<n; j++) {  /* Fix the time along the trace */
           t=j*dt;	    
           gausian(gss,n,sigma,t,dt);
#pragma omp parallel default(none) shared(ns2,j,n,tmp,gss,tx) private(k)
           for (k=0; k<n; k++) tmp[j*ns2+k]=gss[k]*tx[k];
       }
       bzero(cs,ns2*n*sizeof(complex)); 
       pfa2rc (1,1,ns2,n,tmp,cs);
       for (j=0; j<n; j++) {  /* Fix the time along the trace */
#pragma omp parallel default(none) shared(s,ts,cs,n,ns2,nf,nnf,j) private(k)
           for (k=0; k<nf; k++) {
               ts[2*k*n+j]     +=cs[j*nnf+k].r;
               ts[(2*k+1)*n+j] +=cs[j*nnf+k].i;
           }
        }
   }
   for (j=0; j<n; j++) for (k=0; k<nf; k++) {
       s[k*n+j] = sqrt(ts[2*k*n+j]*ts[2*k*n+j] + ts[(2*k+1)*n+j]*ts[(2*k+1)*n+j]);
   }
}
/*-------------------------------------------------------------------------------------------*/
void sspctrmR(float *tx, float *rc, float *s, float *gss, int n, int ns2, float dt, int ncyl)
/*-------------------------------------------------------------------------------------------
 
      ns2     Power of 2 of the length of the trace array for the FFT
      n       length of the trace array
      ncyl    number of cycles in the standard deviation sigma of the gausian function 
      dt      time sample rate
      f0 and f1 limit of the frecuency analysis
      t(n)    float data array
      rc(ns) complex temporal storage out of the FFT 
      gss(n)  float  Gaussian function same length os the time array
      s(nf*n) float  Output Matrix Spectra as a fuction of time 
 -------------------------------------------------------------------------------------------*/
{
   int i, j, k,nf;
   float t, fny,  df, wl,sigma,F0,F1;
 
   F0 = 4/(n*dt);
   F1 = 1/(4*dt);
   nf  = (int) ns2/4+1;
   df  = (F1-F0)/nf;
   bzero(s,nf*n*sizeof(float)); 

/* ------------------------------------------------------------*/
   for (i=0; i<nf; i++) {     /* Fix the frequency */
       sigma=ncyl/(F0+i*df);
       for (j=0; j<n; j++) {  /* Fix the time along the trace */
           t=j*dt;
           gausian(gss,n,sigma,t,dt);
           bzero(rc,nf*sizeof(float)); 
           conv (n,0,tx,n,0,gss,nf,0,rc);
           for (k=0; k<nf; k++) s[k*n+j] +=rc[k];
       }
    }
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
/*******************************************************************************************
 * Gausian Function
 *****************************************************************************************
   Subroutine: Gausian 
   Function: Generates a gausian function
   Author: R.C.
   Date: 2011
   Variables: r     --- time series
              n     --- dimesion of time series 
              rms   --- New rms value

	      expo
  sgm=sigma*sigma;
*/
void gausian(float *f, int n, float sigma,float a,float dt)
{
  float tmp=0,t,b,c,sgm,pi;
  int j;

  pi  = 8.0*atan(1.0e0);
  sgm=sigma;

#pragma omp parallel default(none) shared(n,a,dt,sgm,pi,f) private(j,t,b,c)
  for (j=0; j<n; j++) {
      t = j*dt;
      b = ((t-a)*(t-a))/(2*sgm);
      c = sqrt(2*pi*sgm);
      f[j]=c*exp(-b);
  }
  for (j=0; j<n; j++) tmp=tmp+f[j];
  for (j=0; j<n; j++) f[j]=f[j]/tmp;
}

/*****************************************************************************************/
float rmsvRC(float *r,int n)
/* compute RMS */
{
  int  j; 
  float rms=0;

  for (j=0;j<n;j++) rms +=r[j]*r[j];
  rms=sqrt(rms/n);
  return rms;
}
