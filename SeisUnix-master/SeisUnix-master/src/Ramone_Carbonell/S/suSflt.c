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
"                                                                                 ",
" SUSFLT:                                                                         ",
"                                                                                 ",
" suSflt <infile >outfile  ncyl=3 prc=0.3                                         ",
"                                                                                 ",
" Required parameters:                                                            ",
"                                                                                 ",
"      ncyl  = number of cycles in the gaussian sigma=ncyl*wavelengt              ",
"      prc   = factor between 0 and 1 form Filter(f,t)                            ",
"                                                                                 ",
" Applies a filter in the time frequency domain. The steps are: Transforms the    ",
" traces into frequency time domain (S-transform), them applies a filter as a     ",
" Funtion of the amplitude of the S transform. The filter is:                     ",
"          Trace(t,x) -> Trace(t,f)                                               ",
"          Filter(t,f)=0 if Amp of Trace(t,f) < prc*max(Trace(t,f))               ",
"          Filter(t,f)=1 if Amp of Trace(t,f) > prc*max(Trace(t,f))               ",
"  then:                                                                          ",
"        RT(t,) <- Filter(t,f)*Trace(t,f)                                         ", 
"         ->  denotes Froward S-Transform                                         ",
"         <-  denotes Inverse S-Transform                                         ",
"          * denotes a product in this case)                                      ",
"                                                                                 ",
" References:                                                                     ",
"    Schimmel, M., and H. Paulssen, 1997: Noise reduction and                     ",
"      detection of weak, coherent signals through                                ", 
"      phase-weighted stacks, Geophysical Journal International,                  ",
"      vol. 130, p. 497-505.                                                      ",
"    Simon, C., S. Ventosa, M. Schimmel, J. J. Daobeita, J. Gallart, and          ",
"       A. Manuel (2007), The S-transform and its inverses: Side effects of       ",
"       discretising and filtering, IEEE Trans. Signal Process,                   ",
"    Stockwell, R. G., L. Mansinha, and R. P. Lowe (1996), Localization of the    ",
"       complex spectrum: The S-transform, IEEE Trans. Signal Process., 44,       ",
"       998- 1001.                                                                ",
"                                                                                 ",
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
void gausian(float *f, int n, float sigma, float a,float dt);
void sSflt(float *tx, complex *cs, float *s, float *gss, 
           float *tmp, complex *st, int n, int ns2, float dt, int ncyl, float prc);

/* Globals (so can trap signal) defining temporary disk files */

segy intrc, outrc;

int
main(int argc, char **argv)
{
   int i,j,ns,nf,nt,ns0,ns2,ncyl,ntr=0,NF,v;
   float dtrmsi=0,dtrmso=0;                         /* numbers of samples,traces   */
   float dt,fny,df,F0,F1,DF,prc;                  /* Working Arrays        */
   float *dtx,*dts,*gss,*tmp;                    /* Working Arrays        */
   complex *cs,*st;                                  /* Working Arrays        */

   initargs(argc,argv);
   requestdoc(1);

   /* Get info from first trace */ 
   if (!gettr(&intrc))  err("can't get first trace");
   if (!getparint("ncyl",&ncyl)) ncyl=3; 
   if (!getparfloat("prc",&prc)) prc=0.3; 
   if (!getparint("verbose",&v)) v=0; 

   ns  = intrc.ns;
   dt  = ((double) intrc.dt)/1000000.0;
   ns0 = ns*2;
   ns2 = npfaro(ns0, LOOKFAC * ns);
   fny = 1.0/(2.0*dt);
   nf  = ns2/2+1;
   df  = fny/(nf-1);
   warn("Dimensions:                 Trace ns -> %i ",ns);

   F0 = 4/(ns*dt); 
   F1 = 1/(4*dt); 
   NF  = (int) ns2/4+1;
   DF  = (F1-F0)/NF;

   if (!(dtx =   (float*)malloc(ns2*sizeof(float))))       err(" *malloc* dtx failed\n");
   if (!(gss =   (float*)malloc(ns2*sizeof(float))))       err(" *malloc* gss failed\n");
   if (!(dts =   (float*)malloc(nf*ns2*sizeof(float))))    err(" *malloc* dts failed\n");
   if (!(tmp =   (float*)malloc(ns2*ns2*sizeof(float))))    err(" *malloc* tmp failed\n");
   if (!(cs  = (complex*)malloc(ns2*nf*sizeof(complex))))  err(" *malloc* cs failed\n");
   if (!(st  = (complex*)malloc(ns2*nf*sizeof(complex))))  err(" *malloc* st failed\n");

   do {
       bcopy(&intrc,&outrc,SEGY_HDRBYTES);     
       bcopy(intrc.data, dtx, ns*sizeof(float));
       ntr++; 
       dtrmsi=0;
       for (j=0; j<ns; j++) dtrmsi += dtx[j]*dtx[j];
       dtrmsi=sqrt(dtrmsi/ns);

       if (v) warn(" Trace # -> %i %f",ntr,dtrmsi);
       sSflt(dtx, cs, dts, gss, tmp, st, ns, ns2, dt, ncyl, prc);

       dtrmso=0;
       for (j=0; j<ns; j++) dtrmso += dts[j]*dts[j];
       dtrmso=sqrt(dtrmso/ns);
       for (j=0; j<ns; j++) dts[j] = (dts[j]*dtrmsi)/dtrmso;
       if (v) warn(" Done Trace # -> %i %f",ntr,dtrmso);

       bcopy(dts, outrc.data, ns*sizeof(float));
       puttr(&outrc);
   } while (gettr(&intrc));

   /* Clean up */
   /* free1float(dtx); free1float(dtp); free1float(x);
   */
   return EXIT_SUCCESS;
}
/*******************************************************************************************/

void sSflt(float *tx, complex *cs, float *s, float *gss, 
           float *tmp, complex *st, int n, int ns2, float dt, int ncyl, float prc)
/*-------------------------------------------------------------------------------------------
  int     ns2          Power of 2 of the length of the trace array for the FFT
  int     n            length of the trace array
  int     ncyl         number of cycles in the standard deviation sigma of the gausian function 
  float   dt           time sample rate in seconds
  float   tx(ns2)      float data array
  float   gss(n)       float  Gaussian function same length os the time array
  float   tmp(ns2*nf)  float Result of product of gaussian and the data array
  float   s(ns2*nf)    float  Output Matrix Spectra as a fuction of time 
  complex cs(ns2*n)   complex temporal storage out of the FFT 
  complex st(n*nf)   complex temporal storage out of the FFT 
-------------------------------------------------------------------------------------------*/
{
   int i, j, k, nf, nnf;
   float t, fny, pi, df, wl,sigma,F0,F1,rmx=0;

   pi  = 8.0*atan(1.0e0);
   F0 = 4/(n*dt); 
   F1 = 1/(4*dt); 
   nf  = (int) ns2/4+1;
   nnf = (int) ns2/2+1;
   df  = (F1-F0)/nf;

   bzero(gss,ns2*sizeof(float));
   bzero(tmp,nnf*ns2*sizeof(float));
   bzero(s,nnf*ns2*sizeof(float));
   bzero(st,nnf*ns2*sizeof(complex));

/* ------------------------------------------------------------*/
   warn ("S-transform Start");
   for (i=0; i<nf; i++) {     /* Fix the frequency */
       sigma=ncyl/(F0+i*df);
       for (j=0; j<n; j++) {  /* Fix the time along the trace */
           t=j*dt;	    
           gausian(gss,n,sigma,t,dt);
#pragma omp parallel default(none) shared(n,tmp,gss,ns2,tx,j) private(k)
           for (k=0; k<n; k++) tmp[j*ns2+k]=gss[k]*tx[k];
       }
       bzero(cs,ns2*n*sizeof(complex));
       pfa2rc (1,1,ns2,n,tmp,cs);
       for (j=0; j<n; j++) {  /* Fix the time along the trace */
#pragma omp parallel default(none) shared(s,cs,st,n,nf,nnf,j) private(k) 
           for (k=0; k<nf; k++) {
	        s[k*n+j]    += sqrt(cs[j*nnf+k].r*cs[j*nnf+k].r + cs[j*nnf+k].i*cs[j*nnf+k].i);
                st[k*n+j].r += cs[j*nnf+k].r; 
                st[k*n+j].i += cs[j*nnf+k].i; 
           }
        }
   }
   warn ("Filter");
   for (i=0; i<nnf; i++) for (j=0; j<n; j++) if (s[i*n+j] > rmx) rmx=s[i*n+j];
#pragma omp parallel default(none) shared(s,tmp,rmx,prc,nnf,n) private(i,j)
   for (i=0; i<nnf; i++) for (j=0; j<n; j++){ 
       tmp[i*n+j]=1.0;
       if (s[i*n+j] < rmx*prc) tmp[i*n+j]=0.0;
   }
   warn ("Back transform");
   bzero(cs,nnf*ns2*sizeof(complex));
   for (j=0; j<n ; j++) {
#pragma omp parallel default(none) shared(st,tmp,df,n,nnf,j) private(i)
        for (i=1; i<nnf; i++) { 
              st[i*n+j].r*= tmp[i*n+j]; 
              st[i*n+j].i*= tmp[i*n+j]; 
        }
#pragma omp parallel default(none) shared(cs,st,n,nnf,j,df) private(i)
        for (i=0; i<nnf; i++) { 
             cs[i].r+= st[i*n+j].r/(i*df); 
             cs[i].i+= st[i*n+j].i/(i*df); 
        }
   }
   cs[0].r=0;
   cs[0].i=0;
#pragma omp parallel default(none) shared(cs,ns2,nnf,j) private(i)
   bzero(&cs[nnf],(ns2-nnf)*sizeof(complex));
   pfacr(-1, ns2, cs, s);
}
/*******************************************************************************************/
/*******************************************************************************************
 * Write a trace                       
 ***************************************************************************************** if (ip==10) writeatrace("trc3-cps",&ps[ip*ns],ns);
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

  for (j=0; j<n; j++) tmp += r[j];
  tmp=tmp/n;
  for (j=0; j<n; j++) r[j] -= tmp;
  tmp=0;
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
  float tmp=0,t,b,c,sgm,pi,ncnt=0;
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
  tmp=0.0;
  for (j=0; j<n; j++) tmp=tmp+f[j];
  for (j=0; j<n; j++) f[j]=f[j]/tmp;
}
