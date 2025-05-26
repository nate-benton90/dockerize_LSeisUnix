/* Copyright (c) Colorado School of Mines, 2023.*/
/* All rights reserved.			*/

/* SURESCSV: $Revision: 1.0 $ ; $Date: 2023/08/25 11:00:01 $	*/

#include "su.h"
#include "segy.h"
#include "header.h"
#include <stdbool.h>
#include "headcase.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                       ",
" SURESCSV - Surface Consistent Residual Statics Determination.         ",
"                                                                       ",
" Notes:                                                                ",
" Derives surface-consistent residual statics for sources and receivers.",
" There is one static value estimated for each identified source and    ",
" one static value estimated for each identified receiver.              ",
" The method here is based on stack-power maximization as described by: ",
" Ronen, J. and Claerbout, J., 1985, Geophysics, vol. 50, 2759-2767.    ",
"                                                                       ",
" ----------------------------------------------------------------------",
" Conceptually, the basic methodology herein is:                        ",
"    - store all (nmo-corrected) input traces.                          ",
"    - stack those traces into cdps.                                    ",
"    - cross-correlate each trace of a source with the stacked trace    ",
"      that has the same cdp number.                                    ",
"    - sum those cross-correlations for each source.                    ",
"    - find time of maximum of sum of cross-correlation for each source.",
"    - perform a 3-point interpolation around that time using the       ",
"      neighbouring values (yielding a fractional sample shift).        ",
"                                                                       ",
"    - (by default) reduce long spatial wavelengths in source shifts.   ",
"                                                                       ",
"    - restack the cdps with the derived source time shifts applied by  ",
"      the ints8r routine (which is able to perform fractional shifts). ",
"                                                                       ",
"    - repeat the procedure above, but for receivers. That is,          ",
"      cross-correlate each trace of a receiver with the stacked trace  ",
"      that has the same cdp number, and so on...                       ",
"                                                                       ",
"    - repeat everything above for the amount of iterations you choose. ",
"                                                                       ",
"    - output the source and receiver statics in csv files formatted    ",
"      for use by multiple SU programs (and spreadsheet programs).      ",
" ----------------------------------------------------------------------",
"                                                                       ",
" surescsv  <stdin                                                      ",
"                                                                       ",
" skeyloc=fldr    List of keys that identify source locations.          ",
" 									",
" rkeyloc=gaps    List of keys that identify receiver locations.        ",
" 									",
" ckeyloc=cdp     List of keys that identify cdp locations.             ",
"           Note: For 3D surveys, you may wish to use igi,igc keys here ",
"                 rather than cdp key. See next parameter (cdivider).   ",  
" 									",
" cdivider=4.0    List of dividers for ckeyloc values. This parameter   ",
"                 makes multiple input cdps into 1 larger cdp.          ",
"                 For complicated reasons, typical survey geometry may  ",
"                 cause this program to produce statics with repeating  ",
"                 larger-smaller values (a sawtooth or zigzag pattern). ",
"                 This is known as decoupling and is more likely to     ",
"                 occur when neighbouring cdps contain traces with      ",
"                 unique sets of offsets or receivers or sources.       ",
"                 This parameter effectively bundles multiple input     ",
"                 cdps together during static derivation.               ",
" 									",
" maxsources=10000 Maximum Amount of Sources. This is also the          ",
"                  maximum unique combinations of skeyloc values.       ",
"                                                                       ",
" maxreceivers=20000 Maximum Amount of Receivers. This is also the      ",
"                    maximum unique combinations of rkeyloc values.     ",
"                                                                       ",
" maxcdps=40000 Maximum Amount of CDPs (divided by cdivider). This is   ",
"               also maximum unique combinations of ckeyloc / cdivider. ",
"                                                                       ",
" maxtraces=1000000 Maximum Amount of Traces. Note that most memory for ",
"                   the traces is only allocated for the actual amount  ",
"                   of traces read-in.                                  ",
"                                                                       ",
" maxshift=40.     Maximum Time Shift Per Iteration (milliseconds).     ",
"                  (The output statics can be larger than this).        ",
"                                                                       ",
" numiter=5        Number of Iterations to Perform.                     ",
"                                                                       ",
" useoff=          Maximum offset to include traces while deriving the  ",
"                  time shifts. Rapid changes in NMO velocity, dip,     ",
"                  structure, or spread layout may result in some bad   ",
"                  static values. These issues may be reduced by only   ",
"                  using traces with smaller offsets.                   ",
"             ***  If not specified, this parameter value defaults to   ",
"                  the average offset of all traces multipled by 1.5    ",
"                                                                       ",
" iuseoff=0        Iteration number to start only using offsets less or ",
"                  equal to parameter useoff while deriving shifts.     ",
"                  All iterations less than this value use all offsets. ",
"                  Example: If this value is 2 then iterations 0,1,2    ",
"                           use all offsets, but iterations 3,4,5,..    ",
"                           only use offsets less or equal to useoff.   ",
"                           The default (0) therefor means that offsets ",
"                           are limited to less than parameter useoff   ",
"                           for all iterations.                         ",
"                                                                       ",
" longwave=        For complicated reasons, inaccurate NMO correction,  ",
"                  changes in dip, or changes in geometry layouts may   ",
"                  cause the statics to contain errors in long (spatial)",
"                  wavelengths (roughly the spread length). For every   ",
"                  iteration the time shifts for all sources within this",
"                  radius of each individual source are averaged and    ",
"                  subtracted from the shift of that individual source. ",
"                  This reduces the longer spatial wavelengths in the   ",
"                  output shifts. This is also done for the receivers.  ",
"             ***  This parameter uses the sx,sy and gx,gy key values   ",
"                  of the first trace input for each source location    ",
"                  and each receiver location.                          ",
"             ***  If not specified, this parameter value defaults to   ",
"                  the average offset of all traces.                    ",
"            =-1.0 Do not reduce the longer spatial wavelengths.        ",
"            Note: A large value here acts similarly to option -1.      ",
"                                                                       ",
" srderive=0       Derive time shifts for both sources and receivers.   ",
"              =-1 Sources only.                                        ",
"              =1  Receivers only.                                      ",
"            Note: This is not the same as just applying one set of     ",
"                  statics since all time differences are resolved as   ",
"                  far as possible into whichever points are chosen.    ",
"                                                                       ",
" sout=sstat.csv   Output source statics file name.                     ",
"                  This is a comma-separated-values q-file useable by   ",
"                  multiple SU programs, and spreadsheet programs.      ",
"                  It contains skeyloc values, source coordinates,      ",
"                  input source statics (named numb1_sstat),            ",
"                  source fold after useoff (named numb2_sfold), and    ",
"                  surface consistent residual source static sstat.     ",
"            Note: The input source statics are copied to numb1_sstat   ",
"                  but NOT APPLIED OR USED IN ANY WAY by this program.  ",
"                  They are output because they may have some benefits  ",
"                  (including computation of simple CDP floating datum).",
"                                                                       ",
" rout=rstat.csv   Output receiver statics file name.                   ",
"                  This is a comma-separated-values q-file useable by   ",
"                  multiple SU programs, and spreadsheet programs.      ",
"                  It contains rkeyloc values, receiver coordinates,    ",
"                  input receiver statics (named numb1_gstat),          ",
"                  receiver fold after useoff (named numb2_gfold), and  ",
"                  surface consistent residual receiver static gstat.   ",
"            Note: The input receiver statics are copied to numb1_gstat ",
"                  but NOT APPLIED OR USED IN ANY WAY by this program.  ",
"                  They are output because they may have some benefits  ",
"                  (including computation of simple CDP floating datum).",
"                                                                       ",
NULL};

/* Author:
 *  Andre Latour. June 2023
 *  1. Looked at suresstat.c but no code from that program survives herein.                                                  
 *  2. Copied the unique-identifier-keys code from sustackup.c                
 *  3. Copied (the needed part of) the kdtree code from sunearcsv.c                 
 */
/**************** end self doc ***********************************/

segy tr;

/* Note: The kd tree code here is just a partial copy of what is in sunearcsv.*/
/*       See sunearcsv for more information.                                  */

typedef struct node {
   unsigned long long elem; 
   struct node * l;
   struct node * r;
} node;

void connect_nodes (node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd,
                   int ihop);

void connect_all (node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd);

void find_in (node *tree_nodes, double **tree_dl, int tree_numd,
             double *extent_min, double *extent_max,  
             unsigned long long *out_elem, unsigned long long *num_out);

void find_in_rcur (double **tree_dl, int tree_numd, node *now_node, int naxe,
                  double *extent_min, double *extent_max, 
                  unsigned long long *out_elem, unsigned long long *num_out);


struct OrderInfo {
  int ndxa;
  int ndxb;
  int ndxc;
  int ndxd;
};
struct OrderInfo *sndx;
struct OrderInfo *rndx;
struct OrderInfo *cndx;
int comparenn (const void * q1, const void * q2) ;


int num_keyloc;           /* needed within comparedd (and therefor bhighdd)   */

int bhighdd(double** all, int last, double* guy); 
int comparedd (const void * q1, const void * q2) ;

int main(int argc, char **argv) {

  int tree_numd = 2;
  int ihop = 1;
  double **tree_dls = NULL;
  double **tree_dlr = NULL;
  double *extent_min = NULL;
  double *extent_max = NULL;
  unsigned long long tree_nsources = 0;
  node *tree_nodes = NULL;
  unsigned long long tree_nreceivers = 0;
  node *tree_noder = NULL;

  unsigned long long *out_elem = NULL;
  unsigned long long num_out = 0;


  cwp_String *skeyloc = NULL;
  int *skaseloc = NULL;
  int *swriteloc = NULL;
  int num_skeyloc=0;                                                                
  double **sunqlocs=NULL; /* source unique identifiers*/
  double **strclocs=NULL; /* trace source identifiers */
  double *sguy=NULL;
  double *sunqx=NULL; /* source x coordinates          */
  double *sunqy=NULL; /* source y coordinates          */
  float  *sunqs=NULL; /* accumulating source statics   */
  float  *sunqi=NULL; /* source static buffer          */  
  float  *sunqq=NULL; /* input source statics          */
  int    *sunqf=NULL; /* source fold                   */
  int maxsources=10000;
  int lastsource=0;
  int nowsource=0;

  cwp_String *rkeyloc = NULL;
  int *rkaseloc = NULL;
  int *rwriteloc = NULL;
  int num_rkeyloc=0;                                                                
  double **runqlocs=NULL; /* receiver unique identifiers */
  double **rtrclocs=NULL; /* trace receiver  identifiers */
  double *rguy=NULL;
  double *runqx=NULL; /* receiver x coordinates          */
  double *runqy=NULL; /* receiver y coordinates          */
  float  *runqs=NULL; /* accumulating receiver statics   */
  float  *runqi=NULL; /* receiver static buffer          */ 
  float  *runqq=NULL; /* input receiver statics          */ 
  int    *runqf=NULL; /* receiver fold                   */
  int maxreceivers=20000;
  int lastreceiver=0;
  int nowreceiver=0;

  cwp_String *ckeyloc = NULL;
  int *ckaseloc = NULL;
  int num_ckeyloc=0;                                                                
  double **cunqlocs=NULL; /* cdp unique identifiers     */
  double **ctrclocs=NULL; /* trace cdp identifiers      */
  double *cguy=NULL;
  double *cdivider=NULL;
  int    *cunqf=NULL; /* cdp fold                        */
  int maxcdps=40000;
  int lastcdp=0;

  float **allsamps=NULL; /* the sample values of all input traces */
  int maxtraces=1000000;
  int lensam=0;

  float *tpad=NULL;
  double *txcor=NULL;
  float maxshift=40.0;
  float sampi=0.0;
  int lenxcor=0;
  double bigx=0.0;
  int ibig=0;
  double tbig=0.0;

  float **cdpamp=NULL; /* the sample values of stacked cdps */
  int *samplive=NULL;
  float *onetim=NULL;
  float *trshifted=NULL;
  int nowcdp=0;

  int i=0;
  int j=0;
  int k=0;
  int nproct=0;

  int ihere=0;
  int jhere=0;
  int khere=0;

  double sx=0.0;
  double sy=0.0;
  float  sq=0.0;
  double rx=0.0;
  double ry=0.0;
  float  rq=0.0;

  int inear=1;
  double useoff=-10.0;
  int iuseoff=0;
  double toff=0.0;

  int iiter=0;
  int numiter=5; 

  double avoff=0.0;

  double longwave=-10.0;

  int srderive=0;

  cwp_String sout=NULL;
  FILE *fpS=NULL;
  cwp_String rout=NULL;
  FILE *fpR=NULL;

/* hook up getpar */

  initargs(argc, argv);
  requestdoc(1); /* note: cannot put a warn above here */

  if(isatty(STDOUT_FILENO)!=1) err("**** Error: this program does not output traces.");

  extent_min = ealloc1double(2);
  extent_max = ealloc1double(2);
  tree_dls = ealloc1(2,sizeof(double *));
  tree_dlr = ealloc1(2,sizeof(double *));

/* Set defaults for extent ranges.                                          */

  for(i=0; i<2; i++) {
    extent_min[i] = -DBL_MAX;
    extent_max[i] =  DBL_MAX;
  }

/* Get key case numbers from GetCase.                                        */

  if(countparval("skeyloc")>0) {
    num_skeyloc = countparval("skeyloc");
    skeyloc = ealloc1(num_skeyloc,sizeof(cwp_String *)); 
    getparstringarray("skeyloc", skeyloc);
  }    
  else {
    num_skeyloc = 1; 
    skeyloc = ealloc1(num_skeyloc,sizeof(cwp_String *)); 
    skeyloc[0] = ealloc1(3,1);
    strcpy(skeyloc[0],"fldr");
  }    
  skaseloc = ealloc1int(num_skeyloc); 
  swriteloc = ealloc1int(num_skeyloc); 
  for (i=0; i<num_skeyloc; ++i) {
    skaseloc[i] = GetCase(skeyloc[i]);
    if(skaseloc[i]<1) err("**** Error: Specified skeyloc name %s is not recognized.",skeyloc[i]);
/* If sx,sy,sstat names are used in skeyloc, avoid printing them twice in output Q-file. */ 
    if(strcmp(skeyloc[i],"sx")==0 || strcmp(skeyloc[i],"sy")==0 || strcmp(skeyloc[i],"sstat")==0) swriteloc[i] = 0;
    else swriteloc[i] = 1;
  }

  sguy = ealloc1double(num_skeyloc);

  if(countparval("maxsources")>0) getparint("maxsources",&maxsources);
  if(maxsources<1) err("**** Error: maxsources must be greater than 0");

  sunqlocs = (double **)ealloc1(sizeof(double*),maxsources);
  sunqx = (double *)ealloc1(sizeof(double),maxsources);
  sunqy = (double *)ealloc1(sizeof(double),maxsources);
  sunqs = (float *)ealloc1(sizeof(float),maxsources);
  sunqi = (float *)ealloc1(sizeof(float),maxsources);
  sunqq = (float *)ealloc1(sizeof(float),maxsources);
  sunqf = (int *)ealloc1(sizeof(int),maxsources);

  for (i=0; i<maxsources; i++) {
    sunqs[i] = 0.0;
    sunqi[i] = 0.0;
    sunqq[i] = 0.0;
    sunqf[i] = 0.0;
  }


  if(countparval("rkeyloc")>0) {
    num_rkeyloc = countparval("rkeyloc");
    rkeyloc = ealloc1(num_rkeyloc,sizeof(cwp_String *)); 
    getparstringarray("rkeyloc", rkeyloc);
  }    
  else {
    num_rkeyloc = 1; 
    rkeyloc = ealloc1(num_rkeyloc,sizeof(cwp_String *)); 
    rkeyloc[0] = ealloc1(3,1);
    strcpy(rkeyloc[0],"gaps");
  }    
  rkaseloc = ealloc1int(num_rkeyloc); 
  rwriteloc = ealloc1int(num_rkeyloc); 
  for (i=0; i<num_rkeyloc; ++i) {
    rkaseloc[i] = GetCase(rkeyloc[i]);
    if(rkaseloc[i]<1) err("**** Error: Specified rkeyloc name %s is not recognized.",rkeyloc[i]);
/* If gx,gy,gstat names are used in rkeyloc, avoid printing them twice in output Q-file. */ 
    if(strcmp(rkeyloc[i],"gx")==0 || strcmp(rkeyloc[i],"gy")==0 || strcmp(rkeyloc[i],"gstat")==0) rwriteloc[i] = 0;
    else rwriteloc[i] = 1;
  }

  rguy = ealloc1double(num_rkeyloc);

  if(countparval("maxreceivers")>0) getparint("maxreceivers",&maxreceivers);
  if(maxreceivers<1) err("**** Error: maxreceivers must be greater than 0");

  runqlocs = (double **)ealloc1(sizeof(double*),maxreceivers);
  runqx = (double *)ealloc1(sizeof(double),maxreceivers);
  runqy = (double *)ealloc1(sizeof(double),maxreceivers);
  runqs = (float *)ealloc1(sizeof(float),maxreceivers);
  runqi = (float *)ealloc1(sizeof(float),maxreceivers);
  runqq = (float *)ealloc1(sizeof(float),maxreceivers);
  runqf = (int *)ealloc1(sizeof(int),maxreceivers);

  for (i=0; i<maxreceivers; i++) {
    runqs[i] = 0.0;
    runqi[i] = 0.0;
    runqq[i] = 0.0;
    runqf[i] = 0.0;
  }

  if(countparval("ckeyloc")>0) {
    num_ckeyloc = countparval("ckeyloc");
    ckeyloc = ealloc1(num_ckeyloc,sizeof(cwp_String *)); 
    getparstringarray("ckeyloc", ckeyloc);
  }    
  else {
    num_ckeyloc = 1; 
    ckeyloc = ealloc1(num_ckeyloc,sizeof(cwp_String *)); 
    ckeyloc[0] = ealloc1(3,1);
    strcpy(ckeyloc[0],"cdp");
  }    
  ckaseloc = ealloc1int(num_ckeyloc); 
  for (i=0; i<num_ckeyloc; ++i) {
    ckaseloc[i] = 0;
    ckaseloc[i] = GetCase(ckeyloc[i]);
    if(ckaseloc[i]<1) err("**** Error: Specified ckeyloc name %s is not recognized.",ckeyloc[i]);
  }

  if(countparval("maxcdps")>0) getparint("maxcdps",&maxcdps);
  if(maxcdps<1) err("**** Error: maxcdps must be greater than 0");

  cguy = ealloc1double(num_ckeyloc);
  cdivider = ealloc1double(num_ckeyloc);

  if(countparval("cdivider")>0) {
    if(countparval("cdivider")!=num_ckeyloc) err("**** Error: cdivider list must have same amount as ckeyloc ");
    getpardouble("cdivider",cdivider);
    for (i=0; i<num_ckeyloc; ++i) {
      if(cdivider[i]<=0.0) err("**** Error: cdivider values must be greater than 0.0 ");
    }
  }
  else {
    for (i=0; i<num_ckeyloc; ++i) cdivider[i] = 4.0;
  }

  cunqlocs = (double **)ealloc1(sizeof(double*),maxcdps);
  cunqf = (int *)ealloc1(sizeof(int),maxcdps);

  if(countparval("maxtraces")>0) getparint("maxtraces",&maxtraces);
  if(maxtraces<1) err("**** Error: maxtraces must be greater than 0");

  strclocs = (double **)ealloc1(sizeof(double*),maxtraces);
  rtrclocs = (double **)ealloc1(sizeof(double*),maxtraces);
  ctrclocs = (double **)ealloc1(sizeof(double*),maxtraces);

  sndx = calloc(maxtraces+1,sizeof(struct OrderInfo));
  rndx = calloc(maxtraces+1,sizeof(struct OrderInfo));
  cndx = calloc(maxtraces+1,sizeof(struct OrderInfo));

  allsamps = (float **)ealloc1(sizeof(float*),maxtraces);

  if(countparval("maxshift")>0) {
    getparfloat("maxshift",&maxshift);
    if(maxshift<=0.0) err("**** Error: maxshift must be greater than 0.0");
  }

  if(countparval("numiter")>0) getparint("numiter",&numiter);
  if(numiter<1) err("**** Error: numiter must be greater than 0");

  if(countparval("useoff")>0) {
    getpardouble("useoff",&useoff);
    if(useoff<0.0) err("**** Error: useoff must be greater than or equal to 0.0");
  }

  if(countparval("iuseoff")>0) {
    getparint("iuseoff",&iuseoff);
    if(iuseoff<0) err("**** Error: iuseoff must be greater than or equal to 0");
  }

  if(countparval("longwave")>0) {
    getpardouble("longwave",&longwave);
    if(longwave!=-1.0 && longwave<=0.0) err("**** Error: longwave must be -1.0 or greater than 0.0");
  }

  if(countparval("srderive")>0) getparint("srderive",&srderive);
  if(srderive<-1 || srderive>1) err("**** Error: srderive option is out-of-range.");

  if(countparval("sout")>0) {
    getparstring("sout", &sout);
    fpS = fopen(sout,"w");
  }
  else {
    fpS = fopen("sstat.csv","w");
  }
  if (fpS == NULL) err("error: output source static file did not open correctly.");

  if(countparval("rout")>0) {
    getparstring("rout", &rout);
    fpR = fopen(rout,"w");
  }
  else {
    fpR = fopen("rstat.csv","w");
  }
  if (fpR == NULL) err("error: output receiver static file did not open correctly.");

  checkpars();

/* Loop over the input traces.                                               */

  while(gettr(&tr)) {
 
    if(nproct<1) { 
      lensam = tr.ns;
      sampi =  ((float)tr.dt) / 1000.0;
      lenxcor = (maxshift+0.00001) / sampi;
    }
    if(nproct>=maxtraces) err("error: Number of input traces is greater than maxtraces=%d",maxtraces);

    sq = tr.sstat;
    rq = tr.gstat;
    sx = tr.sx;
    sy = tr.sy;
    rx = tr.gx;
    ry = tr.gy;
    if(tr.scalco > 1) {
      sx *= tr.scalco;
      sy *= tr.scalco;
      rx *= tr.scalco;
      ry *= tr.scalco;
    }
    else if(tr.scalco < 0) {
      sx /= -tr.scalco;
      sy /= -tr.scalco;
      rx /= -tr.scalco;
      ry /= -tr.scalco;
    }

    avoff += sqrt((sx-rx)*(sx-rx) + (sy-ry)*(sy-ry));

    for (i=0; i<num_skeyloc; ++i) sguy[i] = fromhead(tr,skaseloc[i]);

/* Note that num_keyloc is inside comparedd, which is inside bhighdd, so it   */
/* must be reset for whichever list is being searched.                        */

    num_keyloc = num_skeyloc;
    ihere = bhighdd(sunqlocs, lastsource, sguy);

/* Is this the same location as already encountered ?                         */
/* (If ihere=0 then it is lower than any already encountered).                */

    if(ihere>0 && comparedd(sguy,sunqlocs[ihere-1]) == 0) { 

/* Check the trace sx,sy against stored values.                               */

      if(fabs(sx-sunqx[ihere-1]) > 0.01 || fabs(sy-sunqy[ihere-1]))  
      warn("trace %d has different sx,sy than other traces with same skeyloc values ",nproct+1);

    }
    else { /* This is a new combination of keyloc values                      */

      if(lastsource>=maxsources) 
        err("error: At input trace=%d  number of unique skeyloc combinations is greater than maxsources=%d",nproct,maxsources);

/* Pull pointers and values down to accomodate new location, get new memory.  */

      for (i=lastsource; i>=ihere; --i) {
        sunqlocs[i+1] = sunqlocs[i];
        sunqx[i+1] = sunqx[i];
        sunqy[i+1] = sunqy[i];
        sunqq[i+1] = sunqq[i];
      }
      sunqlocs[ihere]  = ealloc1double(num_skeyloc);

/* Copy the values for the new location into new memory.                      */

      for (i=0; i<num_skeyloc; ++i) sunqlocs[ihere][i] = sguy[i];
      sunqx[ihere] = sx;
      sunqy[ihere] = sy;
      sunqq[ihere] = sq;

      lastsource++;
    }  /* end of  if(ihere>0 && comparedd(sguy,sunqlocs[ihere-1]) == 0) { */

/* Same for receivers.  */

    for (i=0; i<num_rkeyloc; ++i) rguy[i] = fromhead(tr, rkaseloc[i]);
    num_keyloc = num_rkeyloc; 
    jhere = bhighdd(runqlocs, lastreceiver, rguy);

    if(jhere>0 && comparedd(rguy,runqlocs[jhere-1]) == 0) { 
      if(fabs(rx-runqx[jhere-1]) > 0.01 || fabs(ry-runqy[jhere-1]))  
      warn("trace %d has different gx,gy than other traces with same rkeyloc values ",nproct+1);
    }
    else { 
      if(lastreceiver>=maxreceivers) 
        err("error: At input trace=%d  number of unique rkeyloc combinations is greater than maxreceivers=%d",nproct,maxreceivers);

      for (i=lastreceiver; i>=jhere; --i) {
        runqlocs[i+1] = runqlocs[i];
        runqx[i+1] = runqx[i];
        runqy[i+1] = runqy[i];
        runqq[i+1] = runqq[i];
      }
      runqlocs[jhere]  = ealloc1double(num_rkeyloc);

      for (i=0; i<num_rkeyloc; ++i) runqlocs[jhere][i] = rguy[i];
      runqx[jhere] = rx;
      runqy[jhere] = ry;
      runqq[jhere] = rq;

      lastreceiver++;
    }  /* end of  if(jhere>0 && comparedd(rguy,runqlocs[jhere-1]) == 0) { */

/* Same for (super) cdps.       */

    for (i=0; i<num_ckeyloc; ++i) cguy[i] = floor(fromhead(tr, ckaseloc[i]) / cdivider[i]);  

    num_keyloc = num_ckeyloc; 
    khere = bhighdd(cunqlocs, lastcdp, cguy);

/* Is this the same location as already encountered ?                         */
/* (If khere=0 then it is lower than any already encountered).                */

    if(khere>0 && comparedd(cguy,cunqlocs[khere-1]) == 0) { 
/* No coordinates to check here. But keep code structure same as sources, receivers. */
    }

    else { /* This is a new combination of keyloc values                      */

      if(lastcdp>=maxcdps) 
        err("error: At input trace=%d  number of unique ckeyloc combinations is greater than maxcdps=%d",nproct,maxcdps);

/* Pull the pointers down to accomodate new location. And get new memory.     */
 
      for (i=lastcdp; i>=khere; --i) {
        cunqlocs[i+1] = cunqlocs[i];
      }
      cunqlocs[khere] = ealloc1double(num_ckeyloc);

/* Copy the values for the new location into new memory.                      */

      for (i=0; i<num_ckeyloc; ++i) cunqlocs[khere][i] = cguy[i];

      lastcdp++;
    }  /* end of  if(khere>0 && comparedd(cguy,cunqlocs[khere-1]) == 0) { */

    allsamps[nproct] = ealloc1float(lensam);
    for (i=0; i<lensam; ++i) allsamps[nproct][i] = tr.data[i];

/* Allocate and store source location values for each trace                   */

    strclocs[nproct] = ealloc1double(num_skeyloc);
    for (i=0; i<num_skeyloc; ++i) strclocs[nproct][i] = sguy[i]; 

/* Allocate and store receiver location values for each trace                 */

    rtrclocs[nproct] = ealloc1double(num_rkeyloc);
    for (i=0; i<num_rkeyloc; ++i) rtrclocs[nproct][i] = rguy[i]; 

/* Allocate and store cdp location values for each trace                      */

    ctrclocs[nproct] = ealloc1double(num_ckeyloc);
    for (i=0; i<num_ckeyloc; ++i) ctrclocs[nproct][i] = cguy[i]; 

/* Note for testing: Since the header is not being stored and there is a lot  */
/* of indexing going on, how can you confirn that you are using the correct   */
/* traces for each CDP and each source and each receiver? One way to do this  */
/* is to set values into the trace samples themselves, as follows:            */
/*    allsamps[nproct][1] = fromhead(tr, skaseloc[0]) * 0.0001;  */                  
/*    allsamps[nproct][2] = fromhead(tr, rkaseloc[0]) * 0.0001;  */                                 
/*    allsamps[nproct][3] = floor(fromhead(tr, ckaseloc[0]) / cdivider[0]) * 0.0001; */     
/* Later, check these values to confirm.                                      */
/* Then run a bunch of tests with input that is sorted differently.           */
/* The reason to multiply by 0.0001 is to get small enough values that they   */
/* do not affect the output statics much, so these tests also check that      */
/* differently ordered traces do not affect the static answers except for     */
/* some differences related to floating point precision.                      */
/*    Find  testit  for all subsequent commented-out code involved in this.   */
  
    nproct++;
  } /* end of  while(gettr(&tr)) {  */

/* Finished with trace input. And source, receiver, and cdp identification.   */
/* Allocate buffers for the cdp stack. And the padded sample buffers.         */

  cdpamp = (float **)ealloc1(sizeof(float*),lastcdp);
  for (i=0; i<lastcdp; ++i) cdpamp[i] = ealloc1float(lensam);
  samplive  = ealloc1int(lensam);
  onetim    = ealloc1float(lensam);
  trshifted = ealloc1float(lensam);

  tpad  = ealloc1float(lensam+2*lenxcor+1);
  for (i=0; i<lensam+2*lenxcor+1; ++i) tpad[i] = 0.0;
  txcor = ealloc1double(2*lenxcor+1);

/* Loop over the individual trace identifiers and find where they are in the  */
/* unique identifier lists.                                                   */

  for (j=0; j<nproct; ++j) {

/* Note that bhighdd returns 1 greater when equal. And since    */
/* the unique list has all individual values, always subtract 1.*/

    num_keyloc = num_skeyloc; 
    ihere = bhighdd(sunqlocs, lastsource, strclocs[j]) - 1;  

    num_keyloc = num_rkeyloc; 
    jhere = bhighdd(runqlocs, lastreceiver, rtrclocs[j]) - 1;  

    num_keyloc = num_ckeyloc; 
    khere = bhighdd(cunqlocs, lastcdp, ctrclocs[j]) - 1;  

    sndx[j].ndxa = ihere;  /* source index of this trace   */
    sndx[j].ndxb = j;      /* storage index of this trace  */
    sndx[j].ndxc = khere;  /* cdp index of this trace      */ 
    sndx[j].ndxd = jhere;  /* receiver index of this trace */ 

    rndx[j].ndxa = jhere;  /* receiver index of this trace */
    rndx[j].ndxb = j;      /* storage index of this trace  */
    rndx[j].ndxc = khere;  /* cdp index of this trace      */
    rndx[j].ndxd = ihere;  /* source index of this trace   */

    cndx[j].ndxa = khere;  /* cdp index of this trace      */
    cndx[j].ndxb = j;      /* storage index of this trace  */
    cndx[j].ndxc = ihere;  /* source index of this trace   */
    cndx[j].ndxd = jhere;  /* receiver index of this trace */

  } /* end of   for (j=0; j<nproct; ++j) {  */

/* Sort trace indexes by their source, receiver, and cdp indexes.             */

  qsort(sndx,nproct,sizeof(struct OrderInfo),comparenn);

  qsort(rndx,nproct,sizeof(struct OrderInfo),comparenn);

  qsort(cndx,nproct,sizeof(struct OrderInfo),comparenn);

/* Set some defaults that use the average offset value. */ 

  avoff /= nproct;

  if(useoff<-9.0) useoff = 1.5 * avoff;
  useoff = useoff*useoff; 

  if(longwave<-9.0) longwave = avoff;

/* Create the search trees to find all sources within longwave of each source */
/* and all receivers within longwave of each receiver.                        */

/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */

  if(longwave>0.0) {

    tree_numd = 2; /* just 2 dimensions */
    ihop = 1; 

    tree_nsources = lastsource;
    tree_nodes = ealloc1(tree_nsources,sizeof(node));
    tree_dls[0] = sunqx;
    tree_dls[1] = sunqy;

    connect_nodes (tree_nodes, tree_nsources, tree_dls, tree_numd, ihop);

    tree_nreceivers = lastreceiver;
    tree_noder = ealloc1(tree_nreceivers,sizeof(node));
    tree_dlr[0] = runqx;
    tree_dlr[1] = runqy;

    connect_nodes (tree_noder, tree_nreceivers, tree_dlr, tree_numd, ihop);

    if(lastsource>lastreceiver) out_elem = (unsigned long long *)ealloc1(sizeof(unsigned long long),lastsource);
    else out_elem = (unsigned long long *)ealloc1(sizeof(unsigned long long),lastreceiver);

  }

/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */
/* Set an extra index to impossible value in ndxa to trigger processing       */
/* of the last true cdp, source, receiver at the ends of the loops below.     */
/* (The others must be set to any legitimate index number as well).           */

  cndx[nproct].ndxa = -1;
  sndx[nproct].ndxa = -1;
  rndx[nproct].ndxa = -1;
  cndx[nproct].ndxb = 0;
  sndx[nproct].ndxb = 0;
  rndx[nproct].ndxb = 0;
  cndx[nproct].ndxc = 0;
  sndx[nproct].ndxc = 0;
  rndx[nproct].ndxc = 0;
  cndx[nproct].ndxd = 0;
  sndx[nproct].ndxd = 0;
  rndx[nproct].ndxd = 0;


  for (iiter=0; iiter<numiter; iiter++) {

    if(srderive<=0) { /* -1 Sources only. 0=both               */

/* Stack the traces into their cdps.                                          */

      for (i=0; i<lensam; ++i) { /* initialize for first cdp                    */
        samplive[i] = 0;
        cdpamp[0][i] = 0.0;
      }
      nowcdp = 0;
      cunqf[0] = 0;

      for (j=0; j<nproct+1; ++j) {

/*  cndx[nproct].ndxa = khere;     cdp index of this trace      */
/*  cndx[nproct].ndxb = nproct;    storage index of this trace  */
/*  cndx[nproct].ndxc = ihere;     source index of this trace   */
/*  cndx[nproct].ndxd = jhere;     receiver index of this trace */

        inear = 1;
        if(iiter>=iuseoff) {
          toff = (sunqx[cndx[j].ndxc]-runqx[cndx[j].ndxd])*(sunqx[cndx[j].ndxc]-runqx[cndx[j].ndxd])
               + (sunqy[cndx[j].ndxc]-runqy[cndx[j].ndxd])*(sunqy[cndx[j].ndxc]-runqy[cndx[j].ndxd]);
          if(toff>useoff) inear = 0;
        }

// add the source and receiver static together and apply the time shift */

        if(inear>0) {
          for (k=0; k<lensam; k++) onetim[k] = (float) k - (sunqs[cndx[j].ndxc]+runqs[cndx[j].ndxd]);
          ints8r(lensam, 1.0, 0.0, allsamps[cndx[j].ndxb], 0.0, 0.0, lensam, onetim, trshifted);
        }
        else {
          for (k=0; k<lensam; k++) trshifted[k] = 0.0;
        }

        if(cndx[j].ndxa==nowcdp) {
/*  if(iiter==0 && fabs(allsamps[cndx[j].ndxb][3]-cunqlocs[nowcdp][0]*0.0001)>0.0000001) */ 
/*  warn("testit0553 %d %d %f %f ",j,cndx[j].ndxa,allsamps[cndx[j].ndxb][3],cunqlocs[nowcdp][0]); */
          for (i=0; i<lensam; ++i) {
            if(trshifted[i] != 0.0) { 
              cdpamp[nowcdp][i] += trshifted[i];
              samplive[i] += 1;
            }
          }
          cunqf[nowcdp] += inear;
        }
        else {
          for (i=0; i<lensam; ++i) { /* Normalize stack trace of previous cdp.*/
            if(samplive[i]>1) cdpamp[nowcdp][i] /= samplive[i];
          }

          nowcdp++;
          if(nowcdp==lastcdp) break;
/*  if(iiter==0 && fabs(allsamps[cndx[j].ndxb][3]-cunqlocs[nowcdp][0]*0.0001)>0.0000001) */ 
/*  warn("testit0663 %d %d %f %f ",j,cndx[j].ndxa,allsamps[cndx[j].ndxb][3],cunqlocs[nowcdp][0]); */
          cunqf[nowcdp] = 1;
          for (i=0; i<lensam; ++i) {
            cdpamp[nowcdp][i] = trshifted[i];
            if(trshifted[i] != 0.0) samplive[i] = 1;
            else samplive[i] = 0;
          }
        }

      } /* end of  for (j=0; j<nproct; ++j) {  */

/* Loop through all sources and derive new time difference     */

      for (i=0; i<2*lenxcor+1; ++i) txcor[i] = 0.0;
      nowsource = 0;
      sunqf[0] = 0;

      for (j=0; j<nproct+1; ++j) {

/*  sndx[nproct].ndxa = ihere;     source index of this trace   */
/*  sndx[nproct].ndxb = nproct;    storage index of this trace  */
/*  sndx[nproct].ndxc = khere;     cdp index of this trace      */ 
/*  sndx[nproct].ndxd = jhere;     receiver index of this trace */ 

        inear = 1; 
        if(iiter>=iuseoff) {
          toff = (sunqx[sndx[j].ndxa]-runqx[sndx[j].ndxd])*(sunqx[sndx[j].ndxa]-runqx[sndx[j].ndxd])
               + (sunqy[sndx[j].ndxa]-runqy[sndx[j].ndxd])*(sunqy[sndx[j].ndxa]-runqy[sndx[j].ndxd]);
          if(toff>useoff) inear = 0;
        }
     
        if(inear>0) { 
          for (k=0; k<lensam; k++) onetim[k] = (float) k - (sunqs[sndx[j].ndxa]+runqs[sndx[j].ndxd]);
          ints8r(lensam, 1.0, 0.0, allsamps[sndx[j].ndxb], 0.0, 0.0, lensam, onetim, trshifted);
        }
        else {
          for (k=0; k<lensam; k++) trshifted[k] = 0.0;
        }

        if(sndx[j].ndxa==nowsource) {
/*  if(iiter==0 && fabs(allsamps[sndx[j].ndxb][1]-sunqlocs[nowsource][0]*0.0001)>0.0000001) */ 
/*  warn("testit0551 %d %d %f %f ",j,sndx[j].ndxa,allsamps[sndx[j].ndxb][1],sunqlocs[nowsource][0]); */
          for (i=0; i<lensam; i++) tpad[i+lenxcor] = cdpamp[sndx[j].ndxc][i];
          for (i=0; i<2*lenxcor+1; i++) {
            for (k=0; k<lensam; k++) txcor[i] += trshifted[k] * tpad[k+i];
          }
          sunqf[nowsource] += inear;
        }
        else {
/* Find maximum of txcor for previous source, then increment nowsource. */
          bigx = txcor[lenxcor]; /* set to middle sample (incase all are zero)*/
          ibig = lenxcor;
          for (i=0; i<2*lenxcor+1; i++) {
            if(txcor[i] > bigx) {
              bigx = txcor[i];
              ibig = i;
            }
          }

/* 3 point interpolation around peak xcor.  */

          tbig = ibig;
          if(ibig>0 && ibig<2*lenxcor && (txcor[ibig-1] - 2.0*bigx + txcor[ibig+1])!=0.0) { 
            tbig += 0.5 * (txcor[ibig-1]-txcor[ibig+1]) / (txcor[ibig-1] - 2.0*bigx + txcor[ibig+1]);
          }

          sunqs[nowsource] += tbig - lenxcor; /* Accumulate the source shift  */ 

          nowsource++;
          if(nowsource==lastsource) break;
/*  if(iiter==0 && fabs(allsamps[sndx[j].ndxb][1]-sunqlocs[nowsource][0]*0.0001)>0.0000001) */ 
/*  warn("testit0661 %d %d %f %f ",j,sndx[j].ndxa,allsamps[sndx[j].ndxb][1],sunqlocs[nowsource][0]); */
          for (i=0; i<2*lenxcor+1; ++i) txcor[i] = 0.0;
          if(inear>0) {
            for (i=0; i<lensam; i++) tpad[i+lenxcor] = cdpamp[sndx[j].ndxc][i];
            for (i=0; i<2*lenxcor+1; i++) {
              for (k=0; k<lensam; k++) txcor[i] += trshifted[k] * tpad[k+i];
            }
          }
          sunqf[nowsource] = inear;

        }

      } /* end of  for (j=0; j<nproct; ++j) { */

/* Remove spatial long waves in source statics ? */

      if(longwave>0.0) {

        for (i=0; i<lastsource; i++) { 

          extent_min[0] = sunqx[i] - longwave;
          extent_max[0] = sunqx[i] + longwave;
          extent_min[1] = sunqy[i] - longwave;
          extent_max[1] = sunqy[i] + longwave;

          find_in (tree_nodes,tree_dls,tree_numd, 
                   extent_min, extent_max, 
                   out_elem, &num_out);

          sunqi[i] = 0.0;
          k = 0;
          for (j=0; j<num_out; j++) { 
            if((sunqx[i]-sunqx[out_elem[j]])*(sunqx[i]-sunqx[out_elem[j]]) +
               (sunqy[i]-sunqy[out_elem[j]])*(sunqy[i]-sunqy[out_elem[j]]) <= longwave*longwave) {
              sunqi[i] += sunqs[out_elem[j]]; 
              k++;
            }
          }
          sunqi[i] /= k;

        } /* end of  for (i=0; i<lastsource; i++) { */

        for (i=0; i<lastsource; i++) sunqs[i] -= sunqi[i]; /* subtract averaged */

      } /* end of  if(longwave>0.0) { */

    } /* end of  if(srderive<=0) {           */


    if(srderive>=0) { /*  1 Receivers only. 0=both                            */
/* ---------------------------------------------------------------------      */
/* Re-stack cdps using the new source statics (and old receiver statics).     */

      for (i=0; i<lensam; ++i) {                
        samplive[i] = 0;
        cdpamp[0][i] = 0.0;
      }
      nowcdp = 0;
      cunqf[0] = 0;

      for (j=0; j<nproct+1; ++j) {
        inear = 1;
        if(iiter>=iuseoff) {
          toff = (sunqx[cndx[j].ndxc]-runqx[cndx[j].ndxd])*(sunqx[cndx[j].ndxc]-runqx[cndx[j].ndxd])
               + (sunqy[cndx[j].ndxc]-runqy[cndx[j].ndxd])*(sunqy[cndx[j].ndxc]-runqy[cndx[j].ndxd]);
          if(toff>useoff) inear = 0;
        }
        if(inear>0) {
          for (k=0; k<lensam; k++) onetim[k] = (float) k - (sunqs[cndx[j].ndxc]+runqs[cndx[j].ndxd]);
          ints8r(lensam, 1.0, 0.0, allsamps[cndx[j].ndxb], 0.0, 0.0, lensam, onetim, trshifted);
        }
        else {
          for (k=0; k<lensam; k++) trshifted[k] = 0.0;
        }
        if(cndx[j].ndxa==nowcdp) {
/*  if(iiter==0 && fabs(allsamps[cndx[j].ndxb][3]-cunqlocs[nowcdp][0]*0.0001)>0.0000001) */
/*  warn("testit0223 %d %d %f %f ",j,cndx[j].ndxa,allsamps[cndx[j].ndxb][3],cunqlocs[nowcdp][0]); */
          for (i=0; i<lensam; ++i) {
            if(trshifted[i] != 0.0) { 
              cdpamp[nowcdp][i] += trshifted[i];
              samplive[i] += 1;
            }
          }
          cunqf[nowcdp] += inear;
        }
        else {
          for (i=0; i<lensam; ++i) { 
            if(samplive[i]>1) cdpamp[nowcdp][i] /= samplive[i];
          }
          nowcdp++;
          if(nowcdp==lastcdp) break;
/*  if(iiter==0 && fabs(allsamps[cndx[j].ndxb][3]-cunqlocs[nowcdp][0]*0.0001)>0.0000001) */ 
/*  warn("testit0333 %d %d %f %f ",j,cndx[j].ndxa,allsamps[cndx[j].ndxb][3],cunqlocs[nowcdp][0]); */
          cunqf[nowcdp] = 1;
          for (i=0; i<lensam; ++i) {
            cdpamp[nowcdp][i] = trshifted[i];
            if(trshifted[i] != 0.0) samplive[i] = 1;
            else samplive[i] = 0;
          }
        }
      } /* end of  for (j=0; j<nproct; ++j) {  */

/* Now iter through all receivers and derive new time differences */

      for (i=0; i<2*lenxcor+1; ++i) txcor[i] = 0.0;
      nowreceiver=0;
      runqf[0] = 0;
      for (j=0; j<nproct+1; ++j) {

/*  rndx[nproct].ndxa = jhere;     receiver index of this trace */
/*  rndx[nproct].ndxb = nproct;    storage index of this trace  */
/*  rndx[nproct].ndxc = khere;     cdp index of this trace      */
/*  rndx[nproct].ndxd = ihere;     source index of this trace   */

        inear = 1;
        if(iiter>=iuseoff) {
          toff = (sunqx[rndx[j].ndxd]-runqx[rndx[j].ndxa])*(sunqx[rndx[j].ndxd]-runqx[rndx[j].ndxa])
               + (sunqy[rndx[j].ndxd]-runqy[rndx[j].ndxa])*(sunqy[rndx[j].ndxd]-runqy[rndx[j].ndxa]);
          if(toff>useoff) inear = 0;
        }

        if(inear>0) {
          for (k=0; k<lensam; k++) onetim[k] = (float) k - (sunqs[rndx[j].ndxd]+runqs[rndx[j].ndxa]);
          ints8r(lensam, 1.0, 0.0, allsamps[rndx[j].ndxb], 0.0, 0.0, lensam, onetim, trshifted);
        }
        else {
          for (k=0; k<lensam; k++) trshifted[k] = 0.0;
        }

        if(rndx[j].ndxa==nowreceiver) {
/*  if(iiter==0 && fabs(allsamps[rndx[j].ndxb][2]-runqlocs[nowreceiver][0]*0.0001)>0.0000001) */ 
/*  warn("testit0552 %d %d %f %f ",j,rndx[j].ndxa,allsamps[rndx[j].ndxb][2],runqlocs[nowreceiver][0]); */
          for (i=0; i<lensam; i++) tpad[i+lenxcor] = cdpamp[rndx[j].ndxc][i];
          for (i=0; i<2*lenxcor+1; i++) {
            for (k=0; k<lensam; k++) txcor[i] += trshifted[k] * tpad[k+i];
          }
          runqf[nowreceiver] += inear;
        }
        else {
          bigx = txcor[lenxcor];
          ibig = lenxcor;
          for (i=0; i<2*lenxcor+1; i++) {
            if(txcor[i] > bigx) {
              bigx = txcor[i];
              ibig = i;
            }
          }
          tbig = ibig;
          if(ibig>0 && ibig<2*lenxcor && (txcor[ibig-1] - 2.0*bigx + txcor[ibig+1])!=0.0) { 
            tbig += 0.5 * (txcor[ibig-1]-txcor[ibig+1]) / (txcor[ibig-1] - 2.0*bigx + txcor[ibig+1]);
          }
          runqs[nowreceiver] += tbig - lenxcor; /* accumulate the receiver time  */

          nowreceiver++;
          if(nowreceiver==lastreceiver) break;
/*  if(iiter==0 && fabs(allsamps[rndx[j].ndxb][2]-runqlocs[nowreceiver][0]*0.0001)>0.0000001) */ 
/*  warn("testit0662 %d %d %f %f ",j,rndx[j].ndxa,allsamps[rndx[j].ndxb][2],runqlocs[nowreceiver][0]); */
          for (i=0; i<2*lenxcor+1; ++i) txcor[i] = 0.0;
          if(inear>0) {
            for (i=0; i<lensam; i++) tpad[i+lenxcor] = cdpamp[rndx[j].ndxc][i];
            for (i=0; i<2*lenxcor+1; i++) {
              for (k=0; k<lensam; k++) txcor[i] += trshifted[k] * tpad[k+i];
            }
          }
          runqf[nowreceiver] = inear;

        }

      } /* end of  for (j=0; j<nproct; ++j) { */

/* Remove spatial long waves in receiver statics ? */

      if(longwave>0.0) {

        for (i=0; i<lastreceiver; i++) { 

          extent_min[0] = runqx[i] - longwave;
          extent_max[0] = runqx[i] + longwave;
          extent_min[1] = runqy[i] - longwave;
          extent_max[1] = runqy[i] + longwave;

          find_in (tree_noder,tree_dlr,tree_numd, 
                   extent_min, extent_max, 
                   out_elem, &num_out);

          runqi[i] = 0.0;
          k = 0;
          for (j=0; j<num_out; j++) { 
            if((runqx[i]-runqx[out_elem[j]])*(runqx[i]-runqx[out_elem[j]]) +
               (runqy[i]-runqy[out_elem[j]])*(runqy[i]-runqy[out_elem[j]]) <= longwave*longwave) {
              runqi[i] += runqs[out_elem[j]]; 
              k++;
            }
          }
          runqi[i] /= k;

        } /* end of  for (i=0; i<lastreceiver; i++) { */

        for (i=0; i<lastreceiver; i++) runqs[i] -= runqi[i]; /* subtract averaged */

      } /* end of  if(longwave>0.0) { */

    } /* end of   if(srderive>=0) {        */

  } /* end of  for (iiter=0; iiter<numiter; iiter++) { */

  fprintf(fpS,"C_SURESCSV numiter avrgoff longwave useoff iuseoff =,%d,%.12g,%.12g,%.12g,%d\n",numiter,avoff,longwave,sqrt(useoff),iuseoff);
  fprintf(fpS,"C_SU_MATCH");
  for (i=0; i<num_skeyloc; i++) fprintf(fpS,",%s",skeyloc[i]); 
  fprintf(fpS,"\nC_SU_SETID,Q\nC_SU_FORMS\nC_SU_ID");
  for (i=0; i<num_skeyloc; i++) {
    if(swriteloc[i]>0) fprintf(fpS,"%s",",%.12g");
  }
  fprintf(fpS,"%s",",%.12g,%.12g,%.12g,%.12g,%.12g\nC_SU_NAMES\nC_SU_ID");
  for (i=0; i<num_skeyloc; i++) {
    if(swriteloc[i]>0) fprintf(fpS,",%s",skeyloc[i]);
  }
  fprintf(fpS,",sx,sy,numb1_sstat,numb2_sfold,sstat\n");
  
  for (i=0; i<lastsource; i++) { 
    fprintf(fpS,"Q");
    for (j=0; j<num_skeyloc; j++) {
      if(swriteloc[j]>0) fprintf(fpS,",%.12g",sunqlocs[i][j]);
    }
    fprintf(fpS,",%.12g,%.12g,%.12g,%d,%.12g\n",sunqx[i],sunqy[i],sunqq[i],sunqf[i],sunqs[i]*sampi);
  }

  fprintf(fpR,"C_SURESCSV numiter avrgoff longwave useoff iuseoff =,%d,%.12g,%.12g,%.12g,%d\n",numiter,avoff,longwave,sqrt(useoff),iuseoff);
  fprintf(fpR,"C_SU_MATCH");
  for (i=0; i<num_rkeyloc; i++) fprintf(fpR,",%s",rkeyloc[i]); 
  fprintf(fpR,"\nC_SU_SETID,Q\nC_SU_FORMS\nC_SU_ID");
  for (i=0; i<num_rkeyloc; i++) {
    if(rwriteloc[i]>0) fprintf(fpR,"%s",",%.12g");
  }
  fprintf(fpR,"%s",",%.12g,%.12g,%.12g,%.12g,%.12g\nC_SU_NAMES\nC_SU_ID");
  for (i=0; i<num_rkeyloc; i++) {
    if(rwriteloc[i]>0) fprintf(fpR,",%s",rkeyloc[i]);
  }
  fprintf(fpR,",gx,gy,numb1_gstat,numb2_gfold,gstat\n");
  
  for (i=0; i<lastreceiver; i++) { 
    fprintf(fpR,"Q");
    for (j=0; j<num_rkeyloc; j++) {
      if(rwriteloc[j]>0) fprintf(fpR,",%.12g",runqlocs[i][j]);
    }
    fprintf(fpR,",%.12g,%.12g,%.12g,%d,%.12g\n",runqx[i],runqy[i],runqq[i],runqf[i],runqs[i]*sampi);
  }

  return(CWP_Exit());
}

/* -------------------------------------------------------------------------- */
/* Specify compare function for index qsorting.                               */

int comparenn (const void * q1, const void * q2) {
  
  struct OrderInfo* p1 = (struct OrderInfo*) q1;
  struct OrderInfo* p2 = (struct OrderInfo*) q2;

  if(p1->ndxa < p2->ndxa) return (-1);
  if(p1->ndxa > p2->ndxa) return (1); 

  return (0); 

}


/* -------------------------------------------------------------------------- */
/* Specify compare function for bhighdd function.                             */

int comparedd (const void * q1, const void * q2) {
  
  int n=0; 

  double* p1 = (double*) q1;
  double* p2 = (double*) q2;

  for(n=0; n<num_keyloc; n++) {  
    if(p1[n] < p2[n]) return (-1);
    if(p1[n] > p2[n]) return (1); 
  }

  return (0); 

}

/* ---------------------------------------------------------------------------*/
/* This is just a standard binary search which returns 1 above exact match    */
/* and stays there for exact+0.1                                              */
/* But note that, unlike other bhigh functions I have written, the argument   */
/* double **all is not a contiguous set of structures referenced from         */
/* a single pointer. It is an array of pointers (thus all[mid] is specified). */

int bhighdd(double **all, int last, double *guy) {

  int mid;
  int low = 0;
  int high = last;

  while (low < high) {
    mid = low + (high - low) / 2;
    if (comparedd(guy,all[mid]) >= 0) low = mid +1;
    else high = mid;
  }

  return low;
}

/* Note: The kd tree code here is just a partial copy of what is in sunearcsv.*/
/*       See sunearcsv for more information.                                  */
/* --------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------------- */

void connect_nodes (node *tree_nodes, unsigned long long tree_numc, 
                    double **tree_dl, int tree_numd, int ihop) {

/*          This function connects the already allocated tree nodes.                                   */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated tree nodes.                                               */     
/* tree_numc     Number of nodes in tree_nodes.                                                        */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/* ihop      = 1 means process the points in dispersed order (approximately random).                   */     
/*               The worst search times for kd trees occur when there are just a few long branches.    */     
/*               This happens in simple kd tree code if the points are ordered in increasing or        */     
/*               decreasing coordinates. Unfortunately, that is often the case for survey files.       */     
/*               The literature about kd trees explains how to create trees with branches of the same  */     
/*               depth (perfectly balanced trees). That is not done here. Instead, this option hops    */     
/*               through the points and loads them in more-or-less random order. This creates a        */     
/*               reasonably-balanced tree (unless you are very unlucky).                               */     
/*               Note that seriously unbalanced trees still work, but they may be very slow.           */     
/*           = 0 process the points in the order they exist within their arrays.                       */     

  unsigned long long nrat = 0;
  unsigned long long nd = 0;
  unsigned long long n = 0;
  unsigned long long m = 0;

  for(m=0; m<tree_numc; m++) {
    tree_nodes[m].l    = 0; 
    tree_nodes[m].r    = 0;  
    tree_nodes[m].elem = m;
  }

/* nrat,nd and the for-loops are just heuristically set (I made them up).         */
/* The basic concept is: Survey points are usually in order. So, we want to load  */
/* them in big hops so that the top nodes will have far-apart coordinates. Then   */
/* reduce hop size so next deeper nodes will also be dispersed nicely. And so on. */

  if(ihop==1) {
    m = 0;
    for (nrat=tree_numc; nrat>0; nrat=nrat*0.6) {
      if(nrat>6) nd = sqrt((double) nrat); 
      else nd = 0;
      for (n=nd+nrat/2; n<tree_numc; n=n+nd+nrat) {
        if (tree_nodes[n].l == 0) {     /* pointer value just used here as a flag */
          tree_nodes[n].l = tree_nodes; /* (so we know we already did this point) */
          tree_nodes[m].elem = n;
          m++;
        }
      }  
    }  
    for(m=0; m<tree_numc; m++) tree_nodes[m].l = 0; /* un-flag it */  
  }

  connect_all(tree_nodes,tree_numc,tree_dl,tree_numd);

  return;

}

/* --------------------------------------------------------------------------------------------------- */

void connect_all(node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd) {

/*          This function connects the already allocated tree nodes in their order in elem.            */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated tree nodes.                                               */     
/* tree_numc     Number of nodes in tree_nodes.                                                        */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/*                                                                                                     */     
/* Note that, technically, this function processes the nodes in their sequential order in tree_nodes.  */
/* But the coordinates of each node are pointed to via node->elem, which means the points              */
/* are actually loaded into the tree in a different order than the order in their arrays.              */

  unsigned long long m = 1; 
  int naxe = 0;

  for(m=1; m<tree_numc; m++) { /* First m is 1 since tree_nodes[0] is set correct.  */ 

    node *now_node, *next_node, *m_node;

    now_node  = NULL; /* just to prevent compiler warning of possible uninitialed use.*/
    m_node    = tree_nodes + m; /* the m-th node to connect                           */ 
    next_node = tree_nodes;     /* walk down tree from top for each node to connect   */

    naxe = 0;
    while(next_node!=0) {
      now_node = next_node;

      naxe++;
      if(naxe==tree_numd) naxe = 0;

      if(tree_dl[naxe][m_node->elem] < tree_dl[naxe][now_node->elem]) 
           next_node = now_node->l;
      else next_node = now_node->r;
    }

    if(tree_dl[naxe][m_node->elem] < tree_dl[naxe][now_node->elem]) 
         now_node->l = m_node; 
    else now_node->r = m_node;

  }

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void find_in (node *tree_nodes, double **tree_dl, int tree_numd,
              double *extent_min, double *extent_max,  
              unsigned long long *out_elem, unsigned long long *num_out) {

/*          This function finds all points between specified extents of the dimensions.                */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated and connected tree nodes.                                 */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/* extent_min    Minimum value of this dimension to find points. Size tree_numd.                       */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/*                                                                                                     */     
/* Output arguments:                                                                                   */     
/* out_elem      The element numbers of the points in the tree_dl arrays. With m meaning the           */     
/*               coordinates of the point are tree_dl[n][m] where n is the dimension number.           */     
/* num_out       Is the number of points found within the extent ranges.                               */
/*                                                                                                     */     
/* Return:false  means some kind of input argument error.                                              */     
/*               NOTE: inputting an impossible extent_min,extent_max range is not an error, you        */     
/*                     just get 0 for num_out.                                                         */     

  int naxe = 1;
  node *now_node;

  *num_out = 0;
  now_node = tree_nodes;
  find_in_rcur(tree_dl, tree_numd, now_node, naxe, extent_min, extent_max, out_elem, num_out);

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void find_in_rcur (double **tree_dl, int tree_numd,
                   node * now_node, int naxe, 
                   double *extent_min, double *extent_max, 
                   unsigned long long *out_elem, unsigned long long *num_out) {

/*          This function finds all points between specified extents of the dimensions.                */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/* extent_min    Minimum value of this dimension to find points. Size tree_numd.                       */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/*                                                                                                     */     
/* Input/Output arguments:                                                                             */     
/* now_node      The current node to start from.                                                       */     
/* naxe          The current splitting dimension.                                                      */     
/* out_elem      Accumulating element numbers of the points in the tree_dl arrays. With m meaning the  */     
/*               coordinates of the point are tree_dl[n][m] where n is the dimension number.           */     
/* num_out       Accumulating number of points found within the extent ranges.                         */

  if(now_node==0) return;

  bool in = true;                
  int i = 0;

  for(i=0; i<tree_numd; i++) {
    if(tree_dl[i][now_node->elem] < extent_min[i] || tree_dl[i][now_node->elem] >= extent_max[i]) { 
      in = false;
      break;
    }
  }
  if(in) { 
    out_elem[*num_out] = now_node->elem;  
    *num_out = *num_out + 1;
  }

/* Find more points. */

  if(naxe==tree_numd) naxe = 0;

  if(tree_dl[naxe][now_node->elem] >= extent_min[naxe] && now_node->l)
    find_in_rcur(tree_dl, tree_numd, now_node->l, naxe+1, extent_min, extent_max, out_elem, num_out);

  if(tree_dl[naxe][now_node->elem] <  extent_max[naxe] && now_node->r)
    find_in_rcur(tree_dl, tree_numd, now_node->r, naxe+1, extent_min, extent_max, out_elem, num_out);

  return;
}

