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
" SURADON-I: Chapman's 1980 Generalized radon transform (Inverse) ",
"                                                                 ",
" suRADON-I <infile >outfile [optional parameters]                ",
"                                                                 ",
" Required parameters:                                            ",
"                                                                 ",
"      key  = Sorted order of the data                            ",
"      xmn  = Minimum offset (m)                                  ",
"      xmx  = Maximum offset (m) absolute value of offset,        ",
"             include a negative sign if offsets are to the       ",
"             left. If offsets are to the right of shot point     ",
"             xmx is positive.                                    ",
"      np   = Number of offset                                    ",
"      ntrs = Max Number of traces per ensamble                   ",
"             ntrs is needed for dimensioning the arrays          ",
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
void RdnChpmnI(float *dtx, float *dtp, float *x, float dt, 
		   float pmn, float pmx, int np, int ns, int ns2, int ntr,
		   float *r, register complex *s, register complex *tmp);
int
main(int argc, char **argv)
{
   int j,ns,ntr,ntrs,ns0,ns2;          /* numbers of samples,traces   */
   int ip,np,iky=0,itmp;             /* sample indices         */
   float *dtx,*dtp,dt,pmn,pmx,*x,dp; /* Working Arrays        */
   cwp_String key[SU_NKEYS];         /* array of keywords */
   cwp_String typ[SU_NKEYS]; 
   int indx[SU_NKEYS],fneg=1;               /* name of type of getparred key */
   Value val;                        /* value of key field */
   float *r,xmn,xmx,dtrmsi=0,dtrmso=0;         /* Working Arrays        */
   register complex *s,*tmp;              /* Working Arrays        */

   initargs(argc,argv);
   requestdoc(1);

   /* Get info from first trace */ 
   if (!gettr(&intrace))  err("can't get first trace");
   ns  = intrace.ns;
   dt  = ((double) intrace.dt)/ 1000000.0;
   if (!getparstringarray("key",key)) key[0]="fldr";
   if (!getparfloat("xmn",&xmn)) xmn=0.0;
   if (!getparfloat("xmx",&xmx)) xmx=0.5; 
   if (!getparint("np", &np)) np=100;
   if (!getparint("ntrs",&ntrs)) ntrs=100;
   typ[0]=hdtype(key[0]);
   indx[0]=getindex(key[0]);
   gethval(&intrace, indx[0], &val);
   iky  = (int) vtof(hdtype(key[0]), val);
   if (xmx < 0) fneg=-1;
   pmn=xmn/1000;
   pmx=xmx/1000;
   dp  =(pmx-pmn)/(np-1);
   ntr = 0;
   ns0 = ns*2;
   ns2 = npfaro(ns0, LOOKFAC * ns0);
   dtx  = ealloc1float(ns2*ntrs);
   dtp  = ealloc1float(ns*np);
   x    = ealloc1float(ntrs);
   r    = ealloc1float(ns2*ntrs);
   s    = ealloc1complex(ns2*ntrs);
   tmp  = ealloc1complex(ns2*ns2);
   do {
      gethval(&intrace, indx[0], &val);
      itmp = ((int) vtof(hdtype(key[0]), val));
      if (iky == itmp) {
          x[ntr]=(float)intrace.offset/100000;
          bcopy(intrace.data, &dtx[ntr*ns2], ns*sizeof(float));
          ntr++; 
      } 
   } while (gettr(&intrace));

   for (j=0; j<ns2*ntr; j++) dtrmsi += dtx[j]*dtx[j];
   dtrmsi=sqrt(dtrmsi/(ntr*ns2));

   RdnChpmnI( dtp, dtx, x, dt, pmn, pmx, np, ns, ns2, ntr, r, s, tmp);

   for (j=0; j<ns*np; j++) dtrmso += dtp[j]*dtp[j];
   dtrmso=sqrt(dtrmso/(np*ns));
   for (j=0; j<ns*np; j++) dtp[j]=(dtp[j]*dtrmsi)/dtrmso;

   for (ip=0; ip<np; ip++) { 
      bcopy(&dtp[ip*ns], outtrace.data, ns*sizeof(float));
/* 
 set header values 
*/
      outtrace.tracl = ip+1;
      outtrace.tracr = ip+1;
      outtrace.tracf = ip+1;
      outtrace.ns = ns;
      outtrace.dt = intrace.dt;  /* d1 is now the relevant step size */
      outtrace.trid = TREAL;
      outtrace.d1 = dt;
      outtrace.fldr = intrace.fldr;
      outtrace.f1 = 0;
      outtrace.d2 = dp;
      outtrace.f2 = pmn;
      outtrace.offset = (int)((pmn+ip*dp)*1000);
      puttr(&outtrace);
 }

 return EXIT_SUCCESS;
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
