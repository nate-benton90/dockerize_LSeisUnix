/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

/* SUFKF: $Revision: 0. $ ; $Date: 1998/12/3:36:46 $      */

#include "su.h"
#include "segy.h"
#include <signal.h>
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                 ",
" SURADON-F: Chapman's 1980 Generalized radon transform (Forward) ",
"                                                                 ",
" suRADON-F <infile >outfile [optional parameters]                ",
"                                                                 ",
" Required parameters:                                            ",
"                                                                 ",
"      key  = Sorted order of the data                            ",
"      pmn  = Minimum ray parameter (1/Vmax- Vmax in km/s)        ",
"      pmx  = Maximum ray parameter (1/Vmin- Vmin in km/s)        ",
"      np   = Number of ray parameters                            ",
"      ntrs = Max Number of traces per ensamble                   ",
"      perc = Precentage bellow which Enveloped is zeroed out     ",
"      off  = Offset of the transformation ususally 0     ",
"      mode = 0 tauP                                              ",
"      mode = 1 tauP + Envelope                                   ",
"      mode = 2 tauP + Semblance                                  ",
"                                                                 ",
" Optional parameters:                                            ",
"                                                                 ",
" verbose=0   verbose = 1 echoes information                      ",
"                         ",
" Important !!!                                                   ",
"                                                                 ",
" Trace Header ntr must be set before this module is used         ",
"                         ",
NULL};

/* Credits:
 *
 *   IJA: Ramon Carbonell (algorithm), Jack (reformatting for SU)
 *   Adapted from suspecfk
 *
 * Trace header fields accessed: ns, dt, d2
 * Trace header fields modified: tracl, ns, dt, trid, d1, f1, d2, f2
 */
/**************** end self doc ***********************************/

#define LOOKFAC 2  
#define PFA_MAX   720720   /* Largest allowed ns2             */

/* Prototype */

/* Globals (so can trap signal) defining temporary disk files */

segy intrace, outtrace;
void RadonChapmanFS0(float *dtx, float *dtp, float *x, float dt, float off, 
		   float pmn, float pmx, int np, int ns, int ns2, int ntr,
		   float *r, register complex *s,  register complex *t, register complex *s0);
void RadonChapmanFS1(float *dtx, float *dtp, float *x, float dt, float off,
		   float pmn, float pmx, int np, int ns, int ns2, int ntr,
		   float *r, register complex *s,  register complex *t);
int
main(int argc, char **argv)
{
   int i,ns,ntr,ntrs,ns0,ns2,islw,j;   /* numbers of samples,traces   */
   int ip,np,iky=0,itmp,mode=0;      /* sample indices         */
   float *dtx,*dtp,dt,pmn,pmx,*x,dp,perc,off; /* Working Arrays        */
   cwp_String key[SU_NKEYS];         /* array of keywords */
   cwp_String typ[SU_NKEYS]; 
   int indx[SU_NKEYS];               /* name of type of getparred key */
   Value val;                        /* value of key field */
   float *r,slw,dtrmso=0,dtrmsi=0;       /* Working Arrays        */
   register complex *s,*tmp,*s0;     /* Working Arrays        */

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
   /*
 * float   dtx[ntr*ns] input data ntr*ns (number traces * number samples) 
 * float   dtp[np*ns]  output data ntp*ns (number slowness * number samples) 
 * complex s[ntr*nf]    output data ntp*ns (number slowness * number samples) 
 * complex tmp[ntr*nf]  output data ntp*ns (number slowness * number samples) 
 * float   r[ntr*ns2]  output data ntp*ns (number slowness * number samples) 
 * float   x[ntr]      array with offsets
 */
   dtx  = ealloc1float(ns2*ntrs);
   dtp  = ealloc1float(ns2*np*2);
   x    = ealloc1float(ntrs);
   r    = ealloc1float(ns2*ntrs);
   s    = ealloc1complex(ns2*ntrs);
   s0   = ealloc1complex(ns2*ntrs);
   tmp  = ealloc1complex(ns2*ns2);
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

   if (mode <= 1) RadonChapmanFS0(dtx, dtp, x, dt, off, pmn, pmx, np, ns, ns2, ntr, r, s, tmp, s0);
   if (mode == 2) RadonChapmanFS1(dtx, dtp, x, dt, off, pmn, pmx, np, ns, ns2, ntr, r, s, tmp);

   for (j=0; j<ns*np; j++) dtrmso += dtp[j]*dtp[j];
   dtrmso=sqrt(dtrmso/(np*ns));
   for (j=0; j<ns*np; j++) dtp[j]=(dtp[j]*dtrmsi)/dtrmso;

   for (ip=0; ip<np; ip++) { 
       bcopy(&dtp[ip*ns], outtrace.data, ns*sizeof(float));
/* 
 Output Tau P transformed data
 set header values 
*/
      slw =pmn+ip*dp;
      islw=(int) (10/slw);
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
    if (perc) for (i=0;i<np*ns;i++) if (dtp[np*ns+i] < (1./perc)) dtp[np*ns+i]=0.0;
    for (ip=0; ip<np; ip++) {  
	 bcopy(&dtp[np*ns+ip*ns], outtrace.data, ns*sizeof(float));
/* 
 Output Semblance  
 set header values 
*/
	 slw =pmn+ip*dp;
	 islw=(int) (10/slw);
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
 /* Clean up */
 /*
 free1float(dtx);
 free1float(dtp);
 free1float(x);
 */
 return EXIT_SUCCESS;
}


void RadonChapmanFS0( float *dtx, float *dtp, float *x, float dt, float off, 
		 float pmn, float pmx, int np, int ns, int ns2, int ntr,
		 float *r, register complex  *s, register complex  *tmp, register complex *s0)
/*******************************************************************************************
* Generalized Radon (Tau-P) transfrom basedd in Chapmand 1989
* *****************************************************************************************
* Function parameters:
*
* float dtx[ntr*ns] input data ntr*ns (number traces * number samples) 
* float dtp[np*ns]  output data ntp*ns (number slowness * number samples) 
* float x           array with offsets
* float dt          sample rate
* float pmn         min ray parameter slowness
* float pmx         max ray parmater slowness
*
* *****************************************************************************************
* Author: Ramon Carbonell  CSIC-Inst. Earth  Sciences 
* *****************************************************************************************/
{   
 int ix,ip,j,i;
 int nf,nshft;
 float pi,fny,dp,df,p,f;
 float cs,sn,a,b,rshft,rmn,rms;           

 pi  = 8.0*atan(1.0e0);
 fny = 1.0/(2.0*dt);
 nf  = ns2/2+1;
 df  = fny/(nf-1);
 dp=(pmx-pmn)/(np-1);

 pfa2rc (1,1,ns2,ntr,dtx,s);
 for (ip=0; ip<np; ip++) {
      p=pmn+ip*dp;
      for (ix=0;ix<ntr; ix++) {
	   rshft=p*(x[ix]-off);
	   nshft=rint(rshft/dt);
	   if (nshft < ns){
#pragma omp parallel default(none) shared(nf,df,rshft,s,s0,ns2,ix,pi) private(f,j,cs,sn,a,b)
	       for (j=0;j<nf;j++) {
		    f=pi*j*df*rshft;
		    cs=cos(f);
		    sn=sin(f);
		    a=s[ix*nf+j].r;
		    b=s[ix*nf+j].i;
		    s0[ix*ns2+j].r = a*cs + b*sn;
		    s0[ix*ns2+j].i = b*cs - a*sn;
	       }
	   }
	   if (nshft > ns) bzero(&s0[ix*ns2],ns2*sizeof(float));
      }
      bzero(&tmp[0],ns2*sizeof(float));
      bzero(&r[0],ns2*sizeof(float));
      for (j=0; j<nf; j++) {
#pragma omp parallel default(none) shared(ntr,ns2,j,tmp,s0) private(ix)
	   for (ix=0; ix<ntr; ix++) {
		tmp[j].r+=s0[ix*ns2+j].r;
		tmp[j].i+=s0[ix*ns2+j].i;
	   }
      }
      pfacr(-1, ns2, &tmp[0], &r[0]);
      bcopy(&r[0], &dtp[ip*ns], ns*sizeof(float));
/*
Computation of Hilbert Transform for estimation of Envelope of the Tau P trace
*/
#pragma omp parallel default(none) shared(nf,tmp) private(j,a,b)
      for (j=0;j<nf;j++) {
	   a=tmp[j].r;
	   b=tmp[j].i;
	   tmp[j].r = b;
	   tmp[j].i = -a;
      }
      pfacr(-1, ns2, &tmp[0], &r[0]);
#pragma omp parallel default(none) shared(ns,dtp,np,ip,r) private(j)
      for (j=0;j<ns;j++){
	   dtp[(np+ip)*ns+j]=sqrt(dtp[ip*ns+j]*dtp[ip*ns+j]+r[j]*r[j]);
      }
 }
}

void RadonChapmanFS1( float *dtx, float *dtp, float *x, float dt, float off,
		 float pmn, float pmx, int np, int ns, int ns2, int ntr,
		 float *r, register complex  *s, register complex  *tmp)
/*******************************************************************************************
* Generalized Radon (Tau-P) transfrom basedd in Chapmand 1989
* *****************************************************************************************
* Function parameters:
*
* float   dtx[ntr*ns] input data ntr*ns (number traces * number samples) 
* float   dtp[np*ns]  output data ntp*ns (number slowness * number samples) 
* complex s[ntr*nf]    output data ntp*ns (number slowness * number samples) 
* complex tmp[ntr*nf]  output data ntp*ns (number slowness * number samples) 
* float   r[ntr*ns2]  output data ntp*ns (number slowness * number samples) 
* float   x[ntr]      array with offsets
* float   dt          sample rate
* float   pmn         min ray parameter slowness
* float   pmx         max ray parmater slowness
*
* *****************************************************************************************
* Author: Ramon Carbonell  CSIC-Inst. Earth  Sciences 
* *****************************************************************************************/
{   
 int ix,ip,j;
 int nf,nshft;
 float pi,fny,dp,df,p,f;
 float cs,sn,a,b,rshft,rtmp;           

 pi  = 8.0*atan(1.0e0);
 fny = 1.0/(2.0*dt);
 nf  = ns2/2+1;
 df  = fny/(nf-1);
 dp=(pmx-pmn)/(np-1);

 pfa2rc (1,1,ns2,ntr,dtx,s);
 for (ip=0; ip<np; ip++) {
     p=pmn+ip*dp;
     for (ix=0;ix<ntr; ix++) {
	 rshft=p*(x[ix]-off);
	 nshft=rint(rshft/dt);
	 if (nshft < ns){
#pragma omp parallel for shared(nf,pi,df,rshft,tmp) private(j,f,cs,sn,a,b)
	     for (j=0;j<nf;j++) {
		    f=pi*j*df*rshft;
		    cs=cos(f);
		    sn=sin(f);
		    a=s[ix*nf+j].r;
		    b=s[ix*nf+j].i;
		    tmp[ix*nf+j].r = a*cs + b*sn;
		    tmp[ix*nf+j].i = b*cs - a*sn;
	       }
	   }
      }
      pfa2cr(-1,1,ns2,ntr,tmp,r);
#pragma omp parallel for shared(dtp,ns) private(j)
      for (j=0; j<ns; j++) {
          dtp[ip*ns+j]=0;
          dtp[(np+ip)*ns+j]=0;
          for (ix=0; ix<ntr; ix++) {
               rtmp=r[ix*ns2+j]/(pi*pi);
               dtp[ip*ns+j]+=rtmp;
               dtp[(np+ip)*ns+j]+=(rtmp*rtmp);
          }
          if (!dtp[(np+ip)*ns+j]) dtp[(np+ip)*ns+j]=1.0;
          dtp[(np+ip)*ns+j]=(dtp[ip*ns+j]*dtp[ip*ns+j])/(ntr*dtp[(np+ip)*ns+j]);
      }
   }
}
