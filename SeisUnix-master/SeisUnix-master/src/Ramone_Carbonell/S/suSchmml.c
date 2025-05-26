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
"                                                                                    ",
" SUSCHMML:                                                                          ",
"                                                                                    ",
" suSschmml <infile >outfile ncyl=3 mode=0                                           ",
"                                                                                    ",
" Required parameters:                                                               ",
"                                                                                    ",
"      ncyl  = number of cycles in the gaussian sigma=ncyl*wavelengt                 ",
"      ntw   = number of traces per window                                           ",
"      isl   = smoothing flag and number of samples                                  ",
"      pwr   = power of the stack scalar gain                                        ",
"      pmn   = minimum rayparameter                                                  ",
"      pmx   = maximum rayparameter                                                  ",
"      np    = number of rayparameters                                               ",
"   verbose  = flag for bug checking                                                 ",
"                                                                                    ",
" Important:                                                                         ",
"                                                                                    ",
" References:                                                                        ",
"   Schimmel1, J. Gallart, (2007) Frequency-dependent phase coherence for noise      ",
"      suppression in seismic array data JGR, 112, B04303, doi:10.1029/2006JB004680  ",
"   Stockwell, R. G., L. Mansinha, and R. P. Lowe (1996), Localization of the        ",
"      complex spectrum: The S-transform, IEEE Trans Signal Process., 44, 998- 1001  ",
"   Simon, C., S. Ventosa, M. Schimmel, J. J. Daobeita, J. Gallart, and              ",
"       A. Manuel (2007), The S-transform and its inverses: Side effects of          ",
"       discretising and filtering, IEEE Trans. Signal Process,                      ",
"                                                                                    ",
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

float rmsvRC(float *r,int n);
void do_smooth(float *data, int nt, int isl);
float rscl(float *r, int n, float rms);
int wrt_cry(char *fvl,complex *v,float *w,int n1,int n2);
int wrt_fry(char *fvl,float *w,int n1,int n2);
void gausian(float *f, int n, float sigma,float a,float dt);
void RdnChpmnPWSC(complex *cdtp, float *x, float dt, float pwr, float *p, 
                 int np, int ns, int ns2, int ntr, int isl, complex *c_tmp0, 
	         complex *c_tmp1, complex *c_tmp2, complex *cps, float *ps);
void strnsfrmC(float *tx, complex *cts, float *gss, float *f_tmp, complex *c_tmp, 
              int n, int ns2, float dt, int ncyl);
void RdnChpmnI(complex *cdtx,float *p, float dt, float *x, int nx, 
               int ns, int ns2, int np, complex  *c_tmp0, complex  *c_tmp1);
void sBtrnsfrm(complex *cdts, float *dtx, complex *c_tmp, int n, int ns2, float dt);

/* Globals (so can trap signal) defining temporary disk files */

char *Sflnm;
char trcfl[BUFSIZ];
char fltfl[BUFSIZ];
char hdrfl[BUFSIZ];
FILE *trcfp,*hdrfp,*fltfp,*fpS;

segy intrr,intri,intrc, outrc;

int
main(int argc, char **argv)
{
   int      j,i,k,ns,np,nf,nt,ntw,ns2,ncyl,itr,isl,ntr=0,NF,v,mode; 
   float    pwr,pmn,pmx,dp,dt,fny,df,F0,F1,DF;                 
   float   *rmstrci,*xff,*dtxi,*x,*p,*ps,*f_tmp,*gss; 
   complex *cdts,*c_tmp,*cdtr,*cdtx,*cdtp,*cps,*c_tmp0,*c_tmp1,*c_tmp2,*c_tmp3; 
   char    *tmpdir;

   initargs(argc,argv);
   requestdoc(1);

   /* Get info from first trace */ 
   if (!gettr(&intrc))  err("can't get first trace");
   if (!getparstring("tmpdir",&tmpdir) && !(tmpdir=getenv("CWP_TMPDIR"))) tmpdir=""; 
   if (!STREQ(tmpdir, "") && access(tmpdir, WRITE_OK)) err("can't write in %s (or it doesn't exist)", tmpdir);

   if (!getparint("mode",&mode)) mode=0; 
   if (!getparint("verbose",&v)) v=0; 
   if (!getparint("ntw",&ntw))   ntw=11; 
   if (!getparint("ncyl",&ncyl)) ncyl=3; 
   if (!getparint("isl",&isl))   isl=11; 
   if (!getparfloat("pwr",&pwr)) pwr=1.0; 
   if (!getparint("np",&np))     np=100; 
   if (!getparfloat("pmn",&pmn)) pmn=-0.2; 
   if (!getparfloat("pmx",&pmx)) pmx=0.2; 
   if (mode) if(!getparstring("Sflnm",&Sflnm)) Sflnm="S-spc.00.su"; 

   itr=0;
   do {                 /* Get a trace count for memory allocation*/
     itr++;
   } while (gettr(&intrc));
   ntr=itr;

   rewind(stdin);
   ns  = intrc.ns;
   dt  = ((double) intrc.dt)/1000000.0;
   ns2 = npfaro(ns, LOOKFAC * ns);
   fny = 1.0/(2.0*dt);
   nf  = ns2/2+1;
   df  = fny/(nf-1);
   nt  = (ntw-1)/2;
   dp  = (pmx-pmn)/(np-1);

   F0 = 4/(ns*dt); 
   F1 = 1/(4*dt); 
   NF  = (int) ns2/4+1;
   DF  = (F1-F0)/NF;
   
   warn("Dimensions: ntr in emsamble -> %i n_samples -> %i dt-> %f ns2 -> %i",ntr,ns,dt,ns2);
   warn("            nf -> %i df -> %f",nf,df);
   warn("            nf -> %i df -> %f",NF,DF);

   if (!(rmstrci  =   (float*)malloc(ntr*sizeof(float))))      err(" *malloc* dtx failed\n");
/* ----------------  
 *   dtxi[ns]         cdts[ns*NF]       f_tmp[ns2*ns]        c_tmp[nf*ns2]
 *   cdts[ns*ntw]     cdtr[ns*ntw]      c_tmp[NF]	       cdts[NF*ns2]       dtxo[ns2] 
 * ----------*/
   if (!(xff     =   (float*)malloc(ntr*sizeof(float))))        err(" *malloc* dtx failed\n");
   if (!(dtxi    =   (float*)malloc(ns2*sizeof(float))))        err(" *malloc* dtx failed\n");
   if (!(gss     =   (float*)malloc(ns*sizeof(float))))         err(" *malloc* tmp failed\n");
   if (!(cdts    = (complex*)malloc(ns2*NF*sizeof(complex))))   err(" *malloc* cs failed\n");
   if (!(f_tmp   =   (float*)malloc(ns2*ns*sizeof(float))))     err(" *malloc* tmp failed\n");
   if (!(c_tmp   = (complex*)malloc(ns*nf*sizeof(complex))))    err(" *malloc* cs failed\n");
   if (!(cdtr    = (complex*)malloc(ns*ntw*sizeof(complex))))   err(" *malloc* cs failed\n");

/* ----------------  
*  cdtx[ntw*ns2]   cdtp[np*ns2]     x[ntw]          c_tmp0[ntw*ns2]  c_tmp1[ntw*ns2], 
*  c_tmp2[ntw*ns2] c_tmp3[ntw*ns2]  cps[np*ns]      ps[np*ns]        cdtx[ntw,ns2]   
*  cdtp[np,ns2]    p[np]            c_tmp0[np*ns2]  c_tmp1[np*ns2]
----------*/ 
   if (!(cdtx    = (complex*)malloc(ntw*ns2*sizeof(complex))))  err(" *malloc* cs failed\n");
   if (!(cdtp    = (complex*)malloc(np*ns2*sizeof(complex))))   err(" *malloc* cs failed\n");
   if (!(x       =   (float*)malloc(ntw*sizeof(float))))      err(" *malloc* cs failed\n");
   if (!(p       =   (float*)malloc(np*sizeof(float))))       err(" *malloc* cs failed\n");
   if (!(c_tmp0  = (complex*)malloc(np*ns2*sizeof(complex))))   err(" *malloc* cs failed\n");
   if (!(c_tmp1  = (complex*)malloc(np*ns2*sizeof(complex))))   err(" *malloc* cs failed\n");
   if (!(c_tmp2  = (complex*)malloc(ntw*ns2*sizeof(complex))))  err(" *malloc* cs failed\n");
   if (!(cps     = (complex*)malloc(np*ns2*sizeof(complex))))   err(" *malloc* cs failed\n");
   if (!(ps      =   (float*)malloc(np*ns2*sizeof(float))))   err(" *malloc* cs failed\n");

/* ---------------- -----------------------*/
   for (i=0; i<np; i++) p[i]=pmn+i*dp;

   if (STREQ(tmpdir,"")) {
      fltfp = etmpfile();
      trcfp = etmpfile();
      hdrfp = etmpfile();
   } else { 
      char directory[BUFSIZ];
      strcpy(directory, tmpdir);
      strcpy(trcfl, temporary_filename(directory));
      strcpy(fltfl, temporary_filename(directory));
      strcpy(hdrfl, temporary_filename(directory));
      trcfp = efopen(trcfl, "w+");
      fltfp = efopen(fltfl, "w+");
      hdrfp = efopen(hdrfl, "w+");
      if (v) warn("putting temporary files in Dir: %s", directory);
   }
   for (itr=0;itr<ntr;itr++) {
       gettr(&intrc);
       xff[itr]=(float)intrc.offset/1000;
       efwrite(&intrc,1,HDRBYTES,hdrfp);
       bcopy(intrc.data,dtxi,ns*sizeof(float));
       rmstrci[itr]=rmsvRC(dtxi,ns);
       if (!mode) {
          strnsfrmC(dtxi,cdts,gss,f_tmp,c_tmp,ns,ns2,dt,ncyl);
          efwrite(cdts,sizeof(complex),ns*NF,trcfp);
          efwrite(cdts,sizeof(complex),ns*NF,fltfp);
       }
       if (v) warn(" S-Transform: Trace # -> %i %i",itr,ntr);
   } 
   if (v) warn(" Done S-transform: file %s ns -> %i NF -> %i ntr-> %i",trcfl,ns,NF,ntr);
   erewind(trcfp);                            /* nt=(ntw-1)/2  */
   erewind(fltfp);                            /* nt=(ntw-1)/2  */
   if (mode) {
      if (v) warn(" Reading S-spectra from  File %s",Sflnm);
      fpS = efopen(Sflnm, "r");
      for (itr=0;itr<ntr;itr++) for (j=0;j<NF; j++) {   /* Get a trace count for memory allocation*/
	  fgettr(fpS,&intrr);
	  fgettr(fpS,&intri);
	  for (k=0;k<ns;k++) {
              cdts[k].r = intrr.data[k];
              cdts[k].i = intri.data[k];
          }
          efwrite(cdts,sizeof(complex),ns,trcfp);
          efwrite(cdts,sizeof(complex),ns,fltfp);
      } 
      efclose(fpS);
   }

   for  (i=0; i<np; i++)  p[i]=pmn+i*dp;                                                   /*  Ray Parameters, slowness etc...   */
   for (itr=nt; itr<ntr-nt; itr++){                                                        /* ---------         Loop over traces */
        if (v) warn("Processing window centeref at Trace: %i",itr);
        for (j=0; j<NF; j++) {                                                             /* Loop over frequencies slant staks  */
            erewind(trcfp); 
            erewind(fltfp);  
            for (i=0; i<ntw; i++){
	        k=itr-nt+i;
                fseek(trcfp,((k*NF+j)*ns)*sizeof(complex),SEEK_SET);
                fread(&cdtr[i*ns2],ns,sizeof(complex),trcfp);
		x[i]=xff[itr]-xff[k];
	        bzero(&cdtr[i*ns2+ns],(ns2-ns)*sizeof(complex));
             }
             bcopy(cdtr,cdts,ns2*ntw*sizeof(complex));
             RdnChpmnPWSC(cdtp,x,dt,pwr,p,np,ns,ns2,ntw,isl,cdts,c_tmp0,c_tmp1,cps,ps);
             RdnChpmnI(cdtx,p,dt,x,ntw,ns,ns2,np,cdtp,c_tmp0);
             if (itr==nt) {
                for (i=0; i<ntw; i++){
                    fseek(fltfp,((i*NF+j)*ns)*sizeof(complex),SEEK_SET);
                    fwrite(&cdtx[i*ns2],ns,sizeof(complex),fltfp);
                }
             }
             else if (itr==(ntr-nt-1)) {
                for (i=0; i<nt; i++){
                    fseek(fltfp,(((i+itr)*NF+j)*ns)*sizeof(complex),SEEK_SET);
                    fwrite(&cdtx[(i+nt)*ns2],ns,sizeof(complex),fltfp);
                }
             }
             else {
                fseek(fltfp,((itr*NF+j)*ns)*sizeof(complex),SEEK_SET);
                fwrite(&cdtx[nt*ns2],ns,sizeof(complex),fltfp);
             }
        }
    }
    if (v) warn(" Done Tp forward and Back Result in file Fltp -> %s n1-> %i n2-> %i n3-> %i",fltfl,ns,NF,ntr);
    erewind(hdrfp);
    erewind(fltfp);
    for (itr=0;itr<ntr;itr++) {
        efread(cdts,sizeof(complex),ns*NF,fltfp);
        sBtrnsfrm(cdts,dtxi,c_tmp,ns,ns2,dt);
        efread(&outrc,1,HDRBYTES,hdrfp);
        rscl(dtxi,ns,rmstrci[itr]);
        bcopy(dtxi,outrc.data,ns*sizeof(float));
        puttr(&outrc);
   } 
   return EXIT_SUCCESS;
}
/*******************************************************************************************/
void strnsfrmC(float *tx, complex *cts, float *gss, float *f_tmp, complex *c_tmp, 
              int n, int ns2, float dt, int ncyl)
/*-------------------------------------------------------------------------------------------
   int      ns2          Power of 2 of the length of the trace array for the FFT
   int      n            length of the trace array
   int      ncyl         number of cycles in the standard deviation sigma of the gausian function 
   float    dt           time sample rate in seconds
   float    txi(n)        intput float data array input
   float    gss(n)       float Gaussian function same length os the time array
   float    f_tmp(ns2*n) float tempora array input to fft
   complex  c_tmp(nnf*n) complex temporal storage out of the FFT 
   complex  cts(nf*ns2)  complex Output Real and Imaginary parts of the S-transform
-------------------------------------------------------------------------------------------*/
{
   int i, j, k, nf, nnf;
   float t, fny,  df, wl,sigma,F0,F1;

   F0 = 4/(n*dt); 
   F1 = 1/(4*dt); 
   nf  = (int) ns2/4+1;
   nnf = (int) ns2/2+1;
   df  = (F1-F0)/nf;
   
   bzero(gss,ns2*sizeof(float)); 
   bzero(f_tmp,ns2*n*sizeof(float)); 
   bzero(c_tmp,nf*ns2*sizeof(complex)); 
   bzero(cts,nf*ns2*sizeof(complex)); 

/* ------------------------------------------------------------*/
/*   warn ("S-trnsfrm -> %f %f",dt,df); */

   for (i=0; i<nf; i++) {     /* Fix the frequency */
       sigma=ncyl/(F0+i*df);
       for (j=0; j<n; j++) {  /* Fix the time along the trace */
           t=j*dt;	    
           gausian(gss,n,sigma,t,dt);
#pragma omp parallel default(none) shared(ns2,j,n,f_tmp,gss,tx) private(k)
           for (k=0; k<n; k++) f_tmp[j*ns2+k]=gss[k]*tx[k];
       }
       bzero(c_tmp,ns2*n*sizeof(complex));
       pfa2rc (1,1,ns2,n,f_tmp,c_tmp);
       for (j=0; j<n; j++) {  /* Fix the time along the trace */
#pragma omp parallel default(none) shared(c_tmp,cts,n,ns2,nf,nnf,j) private(k)
           for (k=0; k<nf; k++) {
               cts[k*n+j].r +=c_tmp[j*nnf+k].r;
               cts[k*n+j].i +=c_tmp[j*nnf+k].i;
           }
        }
   }
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

#pragma omp parallel default(none) \
            shared(n,a,dt,sgm,pi,f) private(j,t,b,c)
  for (j=0; j<n; j++) {
      t = j*dt;
      b = ((t-a)*(t-a))/(2*sgm);
      c = sqrt(2*pi*sgm);
      f[j]=c*exp(-b);
  }
  for (j=0; j<n; j++) tmp=tmp+f[j];
  for (j=0; j<n; j++) f[j]=f[j]/tmp;
}

void RdnChpmnPWSC(complex *cdtp, float *x, float dt, float pwr, float *p, 
                  int np, int ns, int ns2, int ntr, int isl, complex *c_tmp0, 
		  complex *c_tmp1, complex *c_tmp2, complex *cps, float *ps)
/*******************************************************************************************
 * Generalized Radon (Tau-P) transfrom based in Chapmand 1989 
 * Includes in the stack of the shifted traces the PWS Phase Weigthing Stack of Schimmel and
 * Paulsen, 1997.
 * *****************************************************************************************
 * Function parameters:
 *
 * float   cdtp[ns*np] output data ntp*ns (number slowness * number samples) 
 * float   x[ntr]       array with offsets
 * float   dt           sample rate
 * float   pwr          Power of the coherence factors
 * float   pmn          min ray parameter slowness
 * float   pmx          max ray parmater slowness
 * int     isl          length of the smoothing window for the coherence factors of the PWS
 * int     np           number of ray parameters (slowness)                                  
 * complex  c_tmp0[ntr*ns2] input data ntr*ns (number traces * number samples) 
 * complex  c_tmp1[ntr*ns2]   
 * complex  c_tmp2[ntr*ns2]   
 * complex  c_tmp3[ntr*ns2]  
 * For phase stacking  analytical processing
 * complex cps[np*ns]   complex gain function derived from the phase stack  
 * float   ps[np*ns]      real gain function derived from phase stack
 *
 * *****************************************************************************************
 * Author: Ramon Carbonell  CSIC-Inst. Earth  Sciences 
 * *****************************************************************************************/
{   
   int ix,ip,j,i;
   int nf,nshft;
   float pi,fny,dp,df,f;
   float cs,sn,a,b,rshft;           

   pi  = 8.0*atan(1.0e0);
   fny = 1.0/(2.0*dt);
   nf  = ns2/2+1;
   df  = fny/(nf-1);

   bzero(c_tmp1,ns2*ntr*sizeof(complex));
   bzero(c_tmp2,ns2*ntr*sizeof(complex));
   bzero(cps,np*ns*sizeof(complex));
   bzero(cdtp,np*ns2*sizeof(complex));

   pfa2cc (1,1,ns2,ntr,c_tmp0);
   for (ip=0; ip<np; ip++) {
        /* Clean arrays */
        for (ix=0; ix<ntr; ix++) {
             rshft=p[ip]*x[ix];
             nshft=rint(rshft/dt);
             if (nshft < ns){
#pragma omp parallel for private(j,f,cs,sn,a,b) shared(nf,df,rshft,ns2,ix,c_tmp0,c_tmp1)
                 for (j=0; j<ns2; j++) {
	              f=pi*j*df*rshft;
	              cs=cos(f);
	              sn=sin(f);
                      a=c_tmp0[ix*ns2+j].r;
                      b=c_tmp0[ix*ns2+j].i;
                      c_tmp1[ix*ns2+j].r = a*cs + b*sn;
                      c_tmp1[ix*ns2+j].i = b*cs - a*sn;
	         }
             }
             if (nshft > ns) bzero(&c_tmp1[ix*ns2],ns2*sizeof(float));
        }
        pfa2cc(-1,1,ns2,ntr,c_tmp1);
        for (ix=0; ix<ntr; ix++) {
  /* build the complex trace, analytical function and get rid of the amplitude, normalize */
#pragma omp parallel for private(j,a) shared(c_tmp1,c_tmp2,ns,ns2,ip)
	     for (j=0; j<ns; j++) {  
		 a=sqrt(c_tmp1[ix*ns2+j].r*c_tmp1[ix*ns2+j].r+c_tmp1[ix*ns2+j].i*c_tmp1[ix*ns2+j].i);
		 c_tmp2[ix*ns2+j].r=c_tmp1[ix*ns2+j].r/a;
		 c_tmp2[ix*ns2+j].i=c_tmp1[ix*ns2+j].i/a;
             }
        } 
	/* Stack */
#pragma omp parallel for private(j,ix) shared(cdtp,ntr,cps,c_tmp1,c_tmp2,ns,ns2,ip)
        for (ix=0; ix<ntr; ix++)  for (j=0; j<ns; j++) {  
	          cdtp[ip*ns2+j].r += c_tmp1[ix*ns2+j].r;
	          cdtp[ip*ns2+j].i += c_tmp1[ix*ns2+j].i;
		  cps[ip*ns+j].r   += c_tmp2[ix*ns2+j].r;
		  cps[ip*ns+j].i   += c_tmp2[ix*ns2+j].i;
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
#pragma omp parallel for private(j) shared(cdtp,ps,np,ns,ip)
        for (j=0; j<ns; j++) {
	     cdtp[ip*ns2+j].r *= ps[ip*ns+j];
	     cdtp[ip*ns2+j].i *= ps[ip*ns+j];
        }
	bzero(&cdtp[ip*ns2+ns],(ns2-ns)*sizeof(complex));
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

/******************************************************************************************/
void RdnChpmnI(complex *cdtx, float *p, float dt, float *x, int nx, 
               int ns, int ns2, int np, complex  *c_tmp0, complex  *c_tmp1)
/*******************************************************************************************
* Generalized Radon (Tau-P) transfrom basedd in Chapmand 1989
* *****************************************************************************************
* Function parameters:
*
* complex cdtx[ntr*ns2] output data ntr*ns (number traces * number samples) 
* complex cdtp[np*ns2]  input data ntp*ns (number slowness * number samples) 
* complex c_tmp0[np*ns2] output data ntr*ns (number traces * number samples) 
* complex c_tmp1[np*ns2]  input data ntp*ns (number slowness * number samples) 
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
 float pi,fny,dx,df,f,f0;
 float cs,sn,a,b,rshft;           

 pi   = 8.0*atan(1.0e0);
 fny  = 1.0/(2.0*dt);
 nf   = ns2/2+1;
 df   = fny/(nf-1);

 bzero(c_tmp1,ns2*np*sizeof(float));
 bzero(cdtx,nx*ns2*sizeof(float));

 pfa2cc(1,1,ns2,np,c_tmp0);
 for (ix=0; ix<nx; ix++) {
      for (ip=0;ip<np; ip++) {
	   rshft=x[ix]*p[ip];
	   nshft=rshft/dt;
	   if (nshft < ns){
#pragma omp parallel default(none) shared(nf,pi,df,rshft,c_tmp0,c_tmp1,ip,ns2) private(j,f0,f,cs,sn,a,b)
	      for (j=0;j<ns2;j++) {
		   f0=pi*j*df;
		   f=f0*rshft;
		   cs=cos(f);
		   sn=sin(f);
		   a=f0*c_tmp0[ip*ns2+j].r;
		   b=f0*c_tmp0[ip*ns2+j].i;
		   c_tmp1[ip*ns2+j].r = a*cs + b*sn;
		   c_tmp1[ip*ns2+j].i = b*cs - a*sn;
	      }
	   }
      }
      pfa2cc(-1,1,ns2,np,c_tmp1);
      for (j=0; j<ns; j++) {
          cdtx[ix*ns2+j].r=0;
          cdtx[ix*ns2+j].i=0;
#pragma omp parallel default(none) shared(ns2,ix,j,cdtx,c_tmp1,np,pi) private(ip)
          for (ip=0; ip<np; ip++) {
              cdtx[ix*ns2+j].r +=(c_tmp1[ip*ns2+j].r/pi);
              cdtx[ix*ns2+j].i +=(c_tmp1[ip*ns2+j].i/pi);
          }
      }
      bzero(&cdtx[ix*ns],(ns2-ns)*sizeof(float));
   }
}

/*******************************************************************************************/
void sBtrnsfrm(complex *cdts, float *dtx, complex *c_tmp, int n, int ns2, float dt)
/*-------------------------------------------------------------------------------------------
   int      ns2          Power of 2 of the length of the trace array for the FFT
   int      n            length of the trace array
   float    dt           time sample rate in seconds
   float    dtx(ns2)    float Output Matrix Spectra as a fuction of time 
   complex  c_tmp(nf*ns2) float Output Real and Imaginary parts of the S-transform
   complex  cdts(nf*n)      complex temporal storage out of the FFT 
-------------------------------------------------------------------------------------------*/
{
   int i, j, k, nf, nnf;
   float t, fny,  df, wl,sigma,F0,F1;
 
   F0 = 4/(n*dt);
   F1 = 1/(4*dt);
   nf  = (int) ns2/4+1;
   nnf = (int) ns2/2+1;
   df  = (F1-F0)/nf;
 
   bzero(c_tmp,nf*ns2*sizeof(complex));
 
/* ------------------------------------------------------------*/
/*   warn ("Back transform");
 *   */
   bzero(c_tmp,nf*ns2*sizeof(complex));
   for (j=0; j<n ; j++) {
#pragma omp parallel default(none) shared(cdts,c_tmp,ns2,nf,j,df) private(i)
      for (i=1; i<nf; i++) {
             c_tmp[i].r += cdts[i*ns2+j].r/(i*df);
             c_tmp[i].i += cdts[i*ns2+j].i/(i*df);
        }
    }
    c_tmp[0].r=0;
    c_tmp[0].i=0;
    bzero(&c_tmp[nf],(ns2-nf)*sizeof(complex));
    pfacr(-1, ns2, c_tmp, dtx);
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

/*****************************************************************************************/
int wrt_fry(char *fvl,float *v,int n1,int n2)
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

/*****************************************************************************************/
int wrt_cry(char *fvl,complex *v,float *w,int n1,int n2)
{
    FILE *f0;
    int  i,icnt,n;
    char ftmp[80];
    float rr,ri;
    n=n1*n2;
 
    sprintf(ftmp,"%s.bin",fvl);
    if ((f0=fopen(ftmp,"w"))==NULL) err("Cannot open Velocity model file =%s",ftmp);
    for (i=0;i<n;i++){ 
        rr=v[i].r;
        ri=v[i].i;
        w[i]=sqrt(rr*rr + ri*ri);
    }
    icnt=fwrite(w,n,sizeof(float),f0);
    fclose(f0);
    warn("CHECK file %s size n1-> %i n2-> %i",ftmp,n1,n2);
    warn("xwigb < %s title=%s n1=%i n2=%i",ftmp,ftmp,n1,n2);
    return icnt;
}
