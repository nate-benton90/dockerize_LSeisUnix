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

void sBtrnsfrm(complex *cs, float *s, complex *st, int n, int ns2, float dt);
float rscl(float *r, int n, float rms);

/* Globals (so can trap signal) defining temporary disk files */

segy intrc,outrc;

int
main(int argc, char **argv)
{
   int i,k,itr=0,j,ns,nf,nt,ns0,ns2,ntr=0,NF,mode=0,v;   /* numbers of samples,traces   */
   float dt,fny,df,F0,F1,DF,rmstrc;                      /* Working Arrays        */
   float *dts;                    
   complex *cs,*st;                                 

   initargs(argc,argv);
   requestdoc(1);

   /* Get info from first trace */ 
   if (!gettr(&intrc))  err("can't get first trace");
   rewind(stdin);
   itr=0;
   do {                 /* Get a trace count for memory allocation*/
     itr++;
   } while (gettr(&intrc));
   ntr=itr-1;
   rewind(stdin);

   if (!getparint("verbose",&v)) v=0; 

   ns  = intrc.ns;
   dt  = ((double) intrc.dt)/1000000.0;
   ns0 = ns*2;
   ns2 = npfaro(ns, LOOKFAC * ns);
   fny = 1.0/(2.0*dt);
   nf  = ns2/2+1;
   df  = fny/(nf-1);

   F0  = 4/(ns*dt); 
   F1  = 1/(4*dt); 
   NF  = (int) ns2/4+1;
   DF  = (F1-F0)/NF;
   ntr = (int)(ntr/(2*NF));
   if (v) warn("Dimensions:  Trace ns -> %i ntr-> %i %i",ns,ntr,itr,ntr);

   if (!(dts =   (float*)malloc(ns2*sizeof(float))))     err(" *malloc* dts failed\n");
   if (!(cs  = (complex*)malloc(NF*ns2*sizeof(complex))))   err(" *malloc* cs failed\n");
   if (!(st  = (complex*)malloc(NF*ns2*sizeof(complex))))   err(" *malloc* st failed\n");

   if (v) warn(" Num traces-> %i Num. Freq -> %i %i df -> %f",ntr,nf,NF,df);
   for (j=0;j<ntr;j++){
       bzero(st,NF*ns2*sizeof(complex));
       bzero(cs,NF*ns2*sizeof(complex));
       for (i=0; i<NF; i++){
           gettr(&intrc); 
	   rmstrc=intrc.swdep/1000.;
	   if (!i) bcopy(&intrc,&outrc,SEGY_HDRBYTES);
           if (v) warn(" Real Trace # -> %i trid %i %f",i,intrc.trid,rmstrc);
           for (k=0;k<ns;k++) st[i*ns+k].r=intrc.data[k]; 
           gettr(&intrc); 
           if (v) warn(" Imag Trace # -> %i trid %i",i,intrc.trid);
           for (k=0;k<ns;k++) st[i*ns+k].i=intrc.data[k]; 
       }
       sBtrnsfrm(cs,dts,st,ns,ns2,dt);
       rscl(dts,ns,rmstrc);
       bcopy(dts,outrc.data,ns*sizeof(float));
       outrc.trid=1;
       puttr(&outrc);
   }
   return EXIT_SUCCESS;
}
/*******************************************************************************************/
void sBtrnsfrm(complex *cs, float *s, complex *st, int n, int ns2, float dt)
/*-------------------------------------------------------------------------------------------
   int      ns2          Power of 2 of the length of the trace array for the FFT
   int      n            length of the trace array
   float    dt           time sample rate in seconds
   float    s(ns2)    float Output Matrix Spectra as a fuction of time 
   complex  st(nf*ns2) float Output Real and Imaginary parts of the S-transform
   complex  cs(nf*n)      complex temporal storage out of the FFT 
-------------------------------------------------------------------------------------------*/
{
   int i, j, k, nf, nnf;
   float t, fny,  df, wl,sigma,F0,F1;

   F0 = 4/(n*dt); 
   F1 = 1/(4*dt); 
   nf  = (int) ns2/4+1;
   nnf = (int) ns2/2+1;
   df  = (F1-F0)/nf;
   
   bzero(s,ns2*sizeof(float)); 
   bzero(cs,nf*ns2*sizeof(complex)); 

/* ------------------------------------------------------------*/
   warn ("Back transform");
   bzero(cs,nf*ns2*sizeof(complex));
   for (j=0; j<n ; j++) {
#pragma omp parallel default(none) shared(cs,st,n,nf,j,df) private(i)
        for (i=1; i<nf; i++) {
             cs[i].r+= st[i*n+j].r/(i*df);
             cs[i].i+= st[i*n+j].i/(i*df);
        }
    }
    cs[0].r=0;
    cs[0].i=0;
    bzero(&cs[nf],(ns2-nf)*sizeof(complex)); 
    writeatrace("SPEC",cs,s,nf);
    pfacr(-1, ns2, cs, s);
}
/*******************************************************************************************
 * Write a trace                       
 *****************************************************************************************
   Subroutine: readVEL.c
   Function:  Read velocity from a file
   Author: R.C.
   Date: 8-2005
   Variables: fl  --- File name of the velocity file
              n   --- size of the velocity array
              v   --- output velocity model (double precission array)
*/

int writeatrace(char *fvl,complex *cs,float *s, int n1)

{
   FILE *f0;
   int  i,icnt,n;
   char ftmp[80];
   n=n1;
      	   
   sprintf(ftmp,"%s.bin",fvl);
   if ((f0=fopen(ftmp,"w"))==NULL) err("Cannot open Velocity model file =%s",ftmp);
   for (i=0;i<n1;i++) s[i]=sqrt(cs[i].r*cs[i].r+cs[i].i*cs[i].i);
   icnt=fwrite(s,n,sizeof(float),f0);
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
