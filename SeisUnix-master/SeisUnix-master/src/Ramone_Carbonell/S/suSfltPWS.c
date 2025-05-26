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
" SUSFLTPWS:                                                                      ",
"                                                                                 ",
" suSfltPWS <infile >outfile (parameters)                                         ",
"         ncyl=3 prc=0.3 pmn=-0.15 pmx=0.20 np=600 ntwn=200 pwr=1 isl=0           ",
"                                                                                 ",
" Required parameters:                                                            ",
"                                                                                 ",
"      ncyl  = number of cycles in the gaussian sigma=ncyl*wavelengt ncyl=3       ",
"      prc   = factor between 0 and 1 form Filter(f,t)               prc=0.3      ",
"      pmn  = Minimum ray parameter (1/Vmax- Vmax in km/s)           pmn=-0.25    ",
"      pmx  = Maximum ray parameter (1/Vmin- Vmin in km/s)           pm=0.25      ",
"      np   = Number of ray parameters                               np=200       ",
"      ntwn = Max Number of traces per iwindow                       ntwn=11      ",
"      dx   = For constant trace spacing (use intead of offset)      dx=0.25      ",
"      isl  = Length of the smoothing window                         isl=3        ",
"      pwr  = Power of the gain for Phase Stacking                   pwr=1        ",
"  verbose  = Eror Checking debug flag                           verbose=1        ",
"                                                                                 ",
" Frequency-dependent phase coherence for noise suppression in seismic array data ",
" Applies a filter in the time frequency domain. Then does a local slant stack    ",
" using phase coherence stack (Schimmel & Gallart, 2009).                         ",
" The steps are: S-transform, filter, tau-p phase stack, & transform back to time ",
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

void do_smooth(float *data, int nt, int isl);
float rscl(float *r, int n, float rms);
int writeatrace(char *fvl,float *v, int n1);
void gausian(float *f, int n, float sigma, float a,float dt);
void sSflt(float *tx, complex *cs, float *s, float *gss, 
           float *tmp, complex *st, int n, int ns2, float dt, int ncyl, float prc);
void RdnChpmnPS(float *dtp, float *x, float dt, float off, float pwr, float pmn, 
                float pmx, int np, int ns, int ns2, int ntr, int isl, float *r, complex  *s, 
		complex *s0, complex *cr, complex *h, float *hr, complex *cps, float *ps);
void RdnChpmnI(float *dtx, float *dtp, float *x, float dt, 
	     float pmn, float pmx, int np, int ns, int ns2, int ntr,
	     float *r, register complex  *s, register complex  *tmp);

/* Globals (so can trap signal) defining temporary disk files */

segy intrc, outrc;

int
main(int argc, char **argv)
{
   int i,j,ns,nf,nt,ns0,ns2,ncyl,intr=0,NF,v;
   float dt,fny,df,F0,F1,DF,prc;                  /* Working Arrays        */
   float *dtx,*dts,*gss,*tmp;                    /* Working Arrays        */
   float *r,*hr,*ps,xmn=0,xmx=0;
   complex *s0,*h,*cps;
   int   isl,np,ntr,nmx,ntwn,verbose;
   float pmn,pmx,pwr,dx,dp;
   float *x,*dtrmsi,*dtrmso;
   complex *cs,*st,*cr;                                  /* Working Arrays        */
   char *hdrs;

   initargs(argc,argv);
   requestdoc(1);

   /* Get info from first trace */ 
   if (!gettr(&intrc))  err("can't get first trace");
   if (!getparint("ncyl",&ncyl)) ncyl=3; 
   if (!getparfloat("prc",&prc)) prc=0.3; 
   if (!getparfloat("pmn",&pmn)) pmn=-0.2;
   if (!getparfloat("pmx",&pmx)) pmx=0.2;
   if (!getparfloat("pwr",&pwr)) pwr=1;
   if (!getparfloat("dx",&dx)) dx=0;
   if (!getparint("isl", &isl)) isl=3;
   if (!getparint("np", &np)) np=101;
   if (!getparint("ntwn",&ntwn)) ntwn=11;
   if (!getparint("verbose",&v)) v=0;

   if (!getparint("verbose",&v)) v=0; 

   ns  = intrc.ns;
   dt  = ((double) intrc.dt)/1000000.0;
   ns0 = ns*2;
   ns2 = npfaro(ns0, LOOKFAC * ns0);
   fny = 1.0/(2.0*dt);
   nf  = ns2/2+1;
   df  = fny/(nf-1);
   dp  = (pmx-pmn)/(np-1);
   warn("Dimensions:                 Trace ns -> %i ",ns);

   F0 = 4/(ns*dt); 
   F1 = 1/(4*dt); 
   NF  = (int) ns2/4+1;
   DF  = (F1-F0)/NF;

   intr = 0;
   do {
       intr++;
   } while (gettr(&intrc));
   ntr=intr;

   nmx=ntr;
   if (np >  ntr) nmx=np;

   if (!(hdrs   =  (char*)malloc(SEGY_HDRBYTES*ntr*sizeof(char)))) err(" *malloc* hdrs failed \n");
   if (!(x      = (float*)malloc(ntr*sizeof(float))))          err(" *malloc* dtrmsi failed\n");
   if (!(dtrmsi = (float*)malloc(ntr*sizeof(float))))          err(" *malloc* dtrmsi failed\n");
   if (!(dtrmso = (float*)malloc(ntr*sizeof(float))))          err(" *malloc* dtrmso failed\n");

   if (!(dtx =   (float*)malloc(ntr*ns2*sizeof(float))))       err(" *malloc* dtx failed\n");
   if (!(gss =   (float*)malloc(ns2*sizeof(float))))           err(" *malloc* gss failed\n");
   if (!(dts =   (float*)malloc(ntr*nf*ns2*sizeof(float))))    err(" *malloc* dts failed\n");
   if (!(tmp =   (float*)malloc(ns2*ns2*sizeof(float))))       err(" *malloc* tmp failed\n");
   if (!(cs  = (complex*)malloc(ntr*ns2*nf*sizeof(complex))))  err(" *malloc* cs failed\n");
   if (!(st  = (complex*)malloc(ns2*nf*sizeof(complex))))      err(" *malloc* st failed\n");

   if (!(r   =   (float*)malloc(nmx*ns2*sizeof(float))))       err(" *malloc* r failed\n");
   if (!(hr  =   (float*)malloc(ntr*ns2*sizeof(float))))       err(" *malloc* hr failed\n");
   if (!(cr  = (complex*)malloc(nmx*ns2*sizeof(complex))))     err(" *malloc* cr failed\n");
   if (!(h   = (complex*)malloc(ntr*ns2*sizeof(complex))))     err(" *malloc* h failed\n");
   if (!(s0  = (complex*)malloc(nmx*ns2*sizeof(complex))))     err(" *malloc* s0 failed\n");
   if (!(ps  =   (float*)malloc(np*ns2*sizeof(float))))        err(" *malloc* ps failed\n");
   if (!(cps = (complex*)malloc(np*ns2*sizeof(complex))))      err(" *malloc* cps failed\n");

   rewind(stdin);
   for (i=0; i<ntr; i++) {
       gettr(&intrc);
       dtrmsi[i]=0;
       x[i]=(float)intrc.offset/1000;
       bcopy(&intrc,&hdrs[i*SEGY_HDRBYTES],SEGY_HDRBYTES);
       bcopy(intrc.data, &dtx[i*ns2], ns*sizeof(float));
       for (j=0; j<ns; j++) dtrmsi[i] += dtx[i*ns2+j]*dtx[i*ns2+j];
       dtrmsi[i]=sqrt(dtrmsi[i]/ns);
   }
   xmn=0.0;
   xmx=0.0;
   for (i=0;i<ntr;i++){ 
       if (xmn > x[i]) xmn=x[i];
       if (xmx < x[i]) xmx=x[i];
   }
   for  (i=0; i<ntr; i++) {
       if (v) warn(" Trace # -> %i %f",i,dtrmsi[i]);
       sSflt(&dtx[i*ns2],&cs[i*nf],&dts[i*ns2],gss,tmp,st,ns,ns2,dt,ncyl,prc);
   }
   if (v) {
      warn(" After S-transform filter, input to the Slant Stacking loop");
      writeamatrix("CHK-Sflt",dts,ns2,ntr);
   }
   RdnChpmnPS(dts,x,dt,x[0],pwr,pmn,pmx,np,ns,ns2,ntr,isl,r,cs,s0,cr,h,hr,cps,ps);
   if (v) {
      warn(" After Slant Stacking Phase weigthing loop");
      writeamatrix("CHK-TP",dts,ns2,np);
   }
   for  (i=0; i<np; i++) hr[i]=pmn+i*dp;
   RdnChpmnI(dtx,dts,hr,dt,xmn,xmx,ntr,ns,ns2,np,r,s0,cr); 

   for  (i=0; i<ntr; i++) {
       dtrmso[i]=0;
       for (j=0; j<ns; j++) dtrmso[i] += dtx[i*ns+j]*dtx[i*ns+j];
       dtrmso[i]=sqrt(dtrmso[i]/ns);
       for (j=0; j<ns; j++) dtx[i*ns+j] = (dtx[i*ns+j]*dtrmsi[i])/dtrmso[i];
       if (v) warn(" Done Trace # -> %i %f %i",i,dtrmsi[i],dtrmso[i]);
       bcopy(&hdrs[i*SEGY_HDRBYTES],&outrc,SEGY_HDRBYTES);
       bcopy(&dtx[i*ns],outrc.data, ns*sizeof(float));
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
   bzero(tmp,nf*ns2*sizeof(float));
   bzero(s,nf*ns2*sizeof(float));
   bzero(st,nf*ns2*sizeof(complex));

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
   for (i=0; i<nf; i++) for (j=0; j<n; j++) if (s[i*n+j] > rmx) rmx=s[i*n+j];
#pragma omp parallel default(none) shared(s,tmp,rmx,prc,nf,n) private(i,j)
   for (i=0; i<nf; i++) for (j=0; j<n; j++){ 
       tmp[i*n+j]=1.0;
       if (s[i*n+j] < rmx*prc) tmp[i*n+j]=0.0;
   }
   warn ("Back transform");
   bzero(cs,nf*ns2*sizeof(complex));
   for (j=0; j<n ; j++) {
#pragma omp parallel default(none) shared(st,tmp,df,n,nf,j) private(i)
        for (i=1; i<nf; i++) { 
              st[i*n+j].r*= tmp[i*n+j]/(i*df); 
              st[i*n+j].i*= tmp[i*n+j]/(i*df); 
        }
#pragma omp parallel default(none) shared(cs,st,n,nf,j) private(i)
        for (i=0; i<nf; i++) { 
             cs[i].r+= st[i*n+j].r; 
             cs[i].i+= st[i*n+j].i; 
        }
	cs[0].r=0;
	cs[0].i=0;
#pragma omp parallel default(none) shared(cs,ns2,nf,j) private(i)
        for (i=nf; i<ns2; i++) { 
             cs[i].r=0; 
             cs[i].i=0; 
        }
   }
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

int writeamatrix(char *fvl,float *v, int n1, int n2)

{
   FILE *f0;
   int  icnt,n;
   char ftmp[80];
   n=n1*n2;
      	   
   sprintf(ftmp,"%s.bin",fvl);
   if ((f0=fopen(ftmp,"w"))==NULL) err("Cannot open Velocity model file =%s",ftmp);
   icnt=fwrite(v,n,sizeof(float),f0);
   fclose(f0);
   warn("CHECK file %s size n1-> %i n2-> %i",ftmp,n1,n2);
   warn("xwigb < %s title=%s n1=%i n2=%i",ftmp,ftmp,n1,n2);
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

void RdnChpmnPS(float *dtp, float *x, float dt, float off, float pwr, float pmn, 
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
 * float   dtp[*np*ns] output data ntp*ns (number slowness * number samples) 
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
   bzero(h,nf*ntr*sizeof(complex));
   bzero(r,ntr*ns2*sizeof(float));
   bzero(hr,ntr*ns2*sizeof(float));
   bzero(cr,ntr*ns*sizeof(complex));
   bzero(cps,np*ns*sizeof(complex));
   bzero(ps,np*ns*sizeof(float));
   bzero(dtp,np*ns*sizeof(float));

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
	          dtp[ip*ns2+j]   += r[ix*ns2+j];
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
        for (j=0; j<ns; j++) dtp[ip*ns2+j] *= ps[ip*ns+j];
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

void RdnChpmnI(float *dtx, float *dtp, float *x, float dt, 
	     float pmn, float pmx, int np, int ns, int ns2, int ntr,
	     float *r, register complex  *s, register complex  *tmp)
/*******************************************************************************************
* Generalized Radon (Tau-P) transfrom basedd in Chapmand 1989
* *****************************************************************************************
* Function parameters:
*
* float dtx[ntr*ns] output data ntr*ns (number traces * number samples) 
* float dtp[np*ns]  input data ntp*ns (number slowness * number samples) 
* float x           array with offsets
* float dt          sample rate
* float pmn         min offset 
* float pmx         max offset 
*
* *****************************************************************************************
* Author: Ramon Carbonell  CSIC-Inst. Earth  Sciences 
* *****************************************************************************************/
{   
 int ix,ip,j;
 int nf,nshft;
 float pi,fny,dp,df,p,f,f0;
 float cs,sn,a,b,rshft;           

 pi   = 8.0*atan(1.0e0);
 fny  = 1.0/(2.0*dt);
 nf   = ns2/2+1;
 df   = fny/(nf-1);
 dp=(pmx-pmn)/(np-1);

 bzero(r,ns2*ntr*sizeof(float));
 bzero(s,nf*ntr*sizeof(complex));
 bzero(tmp,nf*ntr*sizeof(complex));

 pfa2rc(1,1,ns2,ntr,dtp,s);
 for (ip=0; ip<np; ip++) {
      p=-(pmn+ip*dp);
      for (ix=0;ix<ntr; ix++) {
	   rshft=p*x[ix];
	   nshft=rshft/dt;
	   if (nshft < ns){
#pragma omp parallel default(none) shared(nf,pi,df,rshft,s,tmp,ix) private(j,f0,f,cs,sn,a,b)
	      for (j=0;j<nf;j++) {
		   f0=pi*j*df;
		   f=f0*rshft;
		   cs=cos(f);
		   sn=sin(f);
		   a=f0*s[ix*nf+j].r;
		   b=f0*s[ix*nf+j].i;
		   tmp[ix*nf+j].r = a*cs + b*sn;
		   tmp[ix*nf+j].i = b*cs - a*sn;
	      }
	   }
      }
      pfa2cr(-1,1,ns2,ntr,tmp,r);
      for (j=0; j<ns; j++) {
           dtx[ip*ns+j]=0;
           for (ix=0; ix<ntr; ix++) dtx[ip*ns+j]+=(r[ix*ns2+j]/pi);
      }
   }
}
