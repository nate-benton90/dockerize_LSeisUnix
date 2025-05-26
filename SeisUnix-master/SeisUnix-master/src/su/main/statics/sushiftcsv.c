/* Copyright (c) Colorado School of Mines, 2023.*/
/* All rights reserved.                       */

/* SUSHIFTCSV: $Revision: 1.01 $ ; $Date: 2023/08/01 00:00:01 $		*/
 
#include "su.h"
#include "segy.h" 
#include <stdbool.h>
#include "qdefine.h"
#include "headcase.h"


/*********************** self documentation ******************************/
char *sdoc[] = {
"									     ",
" SUSHIFTCSV - Apply Static Shifts From Nearest Q-file Records.              ",
"									     ",
"  sushiftcsv [parameters].                                                  ",
"									     ",
" Parameters:	         						     ",
"                                                                            ",
" sin=sstat.csv  Input csv Q-file containing Source Information.             ",
"    =none       No sin file. Value 0 is used for all source statics.        ",
"									     ",
" skeyloc=fldr   List of Source Location Key Names. At least 1 is needed.    ",
"                Must be a key name and exist in the sin Q-file.             ",
"									     ",
" sstat=sstat    This is the default source static name. This name must be   ",
"                in the sin Q-file, but does not have to be a key name.      ",
"									     ",
" smult=1.0      Source Static Multiplier. Statics in Q-files should be      ",
"                milliseconds, if not, you can scale them before use here.   ",
"          Note: Also, to reverse static application, specify negative here. ",
"                                                                            ",
" slimit=        Source Location Limit. This is the maximum Pythagorean      ",
"                difference between skeyloc values from trace and nearest    ",
"                sin Q-file point for trace to use source static, otherwise  ",
"                a source static of zero is used for this trace.             ",
"           ***  If not specified, there is no limit, so the source static   ",
"                value comes from whichever sin Q-file point is nearest.     ",
"           ***  If you wish to only use the static at exact locations then  ",
"                a value such as 0.1 is recommended here because trace keys  ",
"                are integer but Q-file values are double precision float.   ",
"                (Float values may not be exact whole numbers).              ",
"                                                                            ",
"									     ",
" rin=rstat.csv  Input csv Q-file containing Receiver Information.           ",
"    =none       No rin file. Value 0 is used for all receiver statics.      ",
"									     ",
" rkeyloc=gaps   List of Receiver Location Key Names. At least 1 is needed.  ",
"                Must be a key name and exist in the rin Q-file.             ",
"									     ",
" rstat=gstat    This is the default receiver static name. This name must be ",
"                in the rin Q-file, but does not have to be a key name.      ",
"									     ",
" rmult=1.0      Receiver Static Multiplier. Statics in Q-files should be    ",
"                milliseconds, if not, you can scale them before use here.   ",
"          Note: Also, to reverse static application, specify negative here. ",
"                                                                            ",
" rlimit=        Receiver Location Limit. This is the maximum Pythagorean    ",
"                difference between rkeyloc values from trace and nearest    ",
"                rin Q-file point for trace to use receiver static, otherwise",
"                a receiver static of zero is used for this trace.           ",
"           ***  If not specified, there is no limit, so the receiver static ",
"                value comes from whichever rin Q-file point is nearest.     ",
"                                                                            ",
"									     ",
" cin=none       Input csv Q-file containing CDP Information.                ",
"    =none       Default. No cin file. Value 0 is used for all CDP statics.  ",
"									     ",
" ckeyloc=cdp    List of CDP Location Key Names. At least 1 is needed.       ",
"                Must be a key name and exist in the cin Q-file.             ",
"									     ",
" cstat=tstat    This is the default CDP static name. This name must be      ",
"                in the cin Q-file, but does not have to be a key name.      ",
"									     ",
" cmult=-2.0     CDP Static Multiplier.                                      ",
"           ***  The default here is negative 2. ***                         ",
"                By default, using all 3 files means the applied static is:  ",
"                    sstat + rstat - 2*cstat                                 ",
"                This corresponds to a common situation when CDP datum       ",
"                statics are used (full statics in sin and rin Q-files and   ",
"                smoothed and/or averaged statics in cin Q-file).            ",
"									     ",
" climit=        CDP Location Limit. This is the maximum Pythagorean         ",
"                difference between ckeyloc values from trace and nearest    ",
"                cin Q-file point for trace to use CDP static, otherwise     ",
"                a CDP static of zero is used for this trace.                ",
"           ***  If not specified, there is no limit, so the CDP static      ",
"                value comes from whichever cin Q-file point is nearest.     ",
"                                                                            ",
"                                                                            ",
" SIGN CONVENTION:                                                           ",
"      ***       A negative static value results in shifting seismic values  ",
"                towards the beginning of traces.                            ",
"                For example -18.3 will shift a peak at 1000 to 981.7        ",
"                                                                            ",
" OTHER INFORMATION:                                                         ",
"                This program uses the static values FROM NEAREST LOCATIONS. ",
"                This is the Pythagorean nearest record in the Q-files       ",
"                considering skeyloc,rkeyloc,ckeyloc values just as numbers. ",
"                It is deliberately coded to be the nearest location rather  ",
"                than the exact location. This is done to simplify certain   ",
"                static situations (beyond the scope of this document).      ",
"         ***    In usual useage, the sin,rin,cin files contain all locations",
"                and therefor nearest location is also the exact location.   ",
"         ***    If you specify x,y keys for any locations the scalco flag   ",
"                in the trace header is used to scale the trace x,y but      ",
"                Q-file standards require they contain unscaled values.      ",
"                Similary, trace scalel is applied to elevations and related ",
"                values from the trace header, but not to the Q-file values. ",
"									     ",
"         ***    This program does not use the input header statics at all.  ",
"         ***    This program does not change output header statics at all.  ",
"									     ",
"   - - - - - - - - - -                                                      ",
"									     ",
" The following 2 parameters affect cpu time, but not results. The search    ",
" for the nearest source and/or receiver and/or cdp is done using kdtrees    ",
" built for skeyloc,rkeyloc,ckeyloc values from the sin,rin,cin records.     ",
"									     ",
" kdist=5   Initial search distance. If positive, this value is added to the ",
"           distance between the previous trace locations and their nearest  ",
"           kdtree locations and used as the initial search distance to find ",
"           the kdtree locations of the current trace locations.             ",
"           If negative, the initial search distance for all trace locations ",
"           is set to this absolute value.                                   ",
" kmult=2   Search multiplier.                                               ",
"           If the nearest kdtree location is not found after searching with ",
"           the initial search distance, the search distance is multiplied   ",
"           by this value, and the kdtree searches are performed again.      ",
"           This repeats until finding the nearest locations in the kdtrees. ",
"									     ",
"   ------------------------------------------------------------------       ",
"   ------------------------------------------------------------------       ",
"									     ",
NULL};

/* Created: July 2023: Andre Latour                                          */ 
/* This program started from sunearcsv.                                      */ 
/**************** end self doc *******************************************/

segy tr;

struct QInfo *SInfo;
struct QInfo *RInfo;
struct QInfo *CInfo;

/* Note: I make no claim that this is a particularly good kd tree implementation.     */
/*       It is not explicitly balanced (it has an option to get approximate balance). */
/* Note: See sunearcsv for details and variations.                                    */

typedef struct node {
   unsigned long long elem; 
   struct node * l;
   struct node * r;
} node;

void connect_nodes (node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd,
                   int ihop);

void connect_all (node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd);

void find_near_rcur (double **tree_dl, int *tree_nt, int tree_numd, node *now_node, int naxe, 
                    double *extent_min, double *extent_max, double *target,
                    unsigned long long *near_elem, double *near_dist, unsigned long long *num_found);

void cycle_for_near (node *tree_nodes, double **tree_dl, int *tree_nt, int tree_numd,
                    double *extent_min, double *extent_max, double *target, 
                    double init_rad, double rad_scal,  
                    unsigned long long *near_elem, double *near_dist, 
                    unsigned long long *num_found, int *ncycles);

double scaledfromhead(segy tr, int k);

/*----------------------------------------------------------------------*/

int main(int argc, char **argv) {

  int npoints = 0;

  int ifixd = 0;          /* flag for all tuples same size or vary   */
  int iztuple = 0;        /* element number where first tuple exists */
  int ktuple = 0;         /* type of tuples (2=pairs, 3=triplets)    */

  cwp_String *pname = NULL;                                         
  cwp_String *ndims = NULL;                                                 
  int numpname = 0;

  double *pindepa = NULL;                                               
  int numdind = 0;
	
  int i = 0;                                                                         
  int j = 0;                                                                       
  int errwarn = 0; 

  double kdist = 5.0;
  double kmult = 2.;
  int kcycles = 0;
  int kdadd   = 1;

  cwp_String Sname=NULL;                                             
  FILE *fpS=NULL;                                                       
  cwp_String skey[9];  
  int slocn[9];
  int skase[9];
  int stree_numd = 0; 
  double smult=1.0;
  double slimit=DBL_MAX;
  cwp_String sstat=NULL; 
  int osloc  = 0;  
  double *stree_dl[9];
  unsigned long long stree_npoints = 0;
  node *stree_nodes = NULL;
  double sdist = 100.0; 
  double sdist2 = 100.0; 
  unsigned long long snum_found = 0;
  double snear_dist = 0.;

  cwp_String Rname=NULL;                                             
  FILE *fpR=NULL;                                                       
  cwp_String rkey[9];  
  int rlocn[9];
  int rkase[9];
  int rtree_numd = 0; 
  double rmult=1.0;
  double rlimit=DBL_MAX;
  cwp_String rstat=NULL; 
  int orloc  = 0;  
  double *rtree_dl[9];
  unsigned long long rtree_npoints = 0;
  node *rtree_nodes = NULL;
  double rdist = 100.0; 
  double rdist2 = 100.0; 
  unsigned long long rnum_found = 0;
  double rnear_dist = 0.;

  cwp_String Cname=NULL;                                             
  FILE *fpC=NULL;                                                       
  cwp_String ckey[9];  
  int clocn[9];
  int ckase[9];
  int ctree_numd = 0; 
  double cmult=-2.0;
  double climit=DBL_MAX;
  cwp_String cstat=NULL; 
  int ocloc  = 0;  
  double *ctree_dl[9];
  unsigned long long ctree_npoints = 0;
  node *ctree_nodes = NULL;
  double cdist = 100.0; 
  double cdist2 = 100.0; 
  unsigned long long cnum_found = 0;
  double cnear_dist = 0.;

  unsigned long long near_elem = 0;
  int ihop = 1;
  double dt = 0.;
  int nproct = 0;

  double target[9];
  int tree_nt[9];
  double extent_min[9];
  double extent_max[9];

  int lentrc = 0;
  float *onetrc=NULL;
  float *oneshift=NULL;
  float sampi=0.0;

/* hook up getpar */
  initargs(argc, argv);
  requestdoc(1);

/* Set infinit extents. And to use all dimensions in Pythagorean nearest.   */
/* The tree code herein is copied from sunearcsv. That program allows       */
/* specification of extent_min, extent_max, and tree_nt options. This       */
/* program would run faster by removing the code for extents and tree_nt.   */

  for(i=0; i<9; i++) {
    extent_min[i] = -DBL_MAX;
    extent_max[i] =  DBL_MAX;
    tree_nt[i] = 1; 
  }

/* Read kdist and kmult first (just to smooth out the code flow).           */

  if(!getpardouble("kdist",&kdist)) kdist = 5.;
  if(kdist==0.)  err("**** Error: kdist cannot be 0."); 

  if(!getpardouble("kmult",&kmult)) kmult = 2.;
  if(kmult<0.)  err("**** Error: kmult must be non-negative."); 

  if(kdist<0.) {
    kdist = 0. - kdist;
    kdadd = 0;
  }

  sdist = kdist;
  sdist2 = kdist;
  rdist = kdist;
  rdist2 = kdist;
  cdist = kdist;
  cdist2 = kdist;

/*--------------------------------------------------------------------------  */
/*- Read the parameters related to source statics. -------------------------  */
/*--------------------------------------------------------------------------  */

  stree_numd = 1;
  if(countparval("sin")>0) {
    getparstring("sin", &Sname);
    if(strcmp(Sname,"none")==0) stree_numd = 0; /* just a flag here */
  }
  else {
    Sname = ealloc1(9,1);
    strcpy(Sname,"sstat.csv");
  }

  if(stree_numd>0) {

    stree_numd = countparval("skeyloc");
    if(stree_numd<1) {
      stree_numd = 1;
      skey[0] = ealloc1(4,1);
      strcpy(skey[0],"fldr");
    }
    else {
      getparstringarray("skeyloc",skey);
      if(stree_numd>9) err("**** Error for sin parameters: Maximum of 9 location names can be specified.");
    }

    for(i=0; i<stree_numd; i++) {

      skase[i] = -1;
      if(strcmp(skey[i],"sx")==0) skase[i] = 101;
      else if(strcmp(skey[i],"sy")==0) skase[i] = 102;
      else if(strcmp(skey[i],"gx")==0) skase[i] = 103;
      else if(strcmp(skey[i],"gy")==0) skase[i] = 104; /* 105-110 in sunearcsv   */
      else if(strcmp(skey[i],"selev")==0) skase[i] = 111;
      else if(strcmp(skey[i],"gelev")==0) skase[i] = 112;
      else if(strcmp(skey[i],"sdepth")==0) skase[i] = 113;
      else if(strcmp(skey[i],"sdel")==0) skase[i] = 114;
      else if(strcmp(skey[i],"gdel")==0) skase[i] = 115;
      else if(strcmp(skey[i],"swdep")==0) skase[i] = 116;
      else if(strcmp(skey[i],"gwdep")==0) skase[i] = 117;
      else {
        skase[i] = GetCase(skey[i]);
        if(skase[i]<1) err("**** Error for sin parameters: Specified location name %s is not recognized.",skey[i]);
      }

    } /* end of  for(i=0; i<stree_numd; i++) { */

    if (!getparstring("sstat", &sstat)) sstat = "sstat";

    if(!getpardouble("smult",&smult)) smult = 1.0;
    if(smult==0.0)  err("**** Error for sin parameters: smult cannot be 0.0"); 

/* Note it might use less cpu time to set slimit into extent_min,extent_max   */
/* but that would require re-working the kdadd-related logic...so, maybe not? */

    if(getpardouble("slimit",&slimit)) {
      if(slimit<0.0)  err("**** Error for sin parameters: slimit cannot be less than 0.0"); 
      slimit *= slimit;
    }

    fpS = fopen(Sname, "r");
    if(fpS==NULL) err("error sin file did not open correctly.");
    
/* Set input numpname,pname to just store what is going to be accessed.       */
/* numpname>0 is a flag to ONLY store values if they are on pname list.       */
/* numpname<1 is a flag to NOT store values if they are on pname list.        */

    pname = ealloc1(stree_numd + 1,sizeof(cwp_String *));
    numpname = 0;
  
    for (i=0; i<stree_numd; ++i) {
      pname[numpname] = ealloc1(strlen(skey[i]),1);
      strcpy(pname[numpname],skey[i]);
      numpname++;
    }
    pname[numpname] = ealloc1(strlen(sstat),1);
    strcpy(pname[numpname],sstat);
    numpname++;

    getviaqfile(fpS, &pname, &numpname, &iztuple, numdind,   
                &ktuple, &ifixd, &SInfo, &npoints, 
                &pindepa,  &ndims, &errwarn) ;

    if(errwarn==1) err("sin file getqinfo error: extra C_SU_NAMES record in q-file");
    else if(errwarn==2) err("sin file getqinfo error: extra C_SU_NDIMS record in q-file");
    else if(errwarn==3) err("sin file getqinfo error: C_SU_ID record not found immediately after C_SU_NAMES record.");
    else if(errwarn==11) 
      err("sin file readqhead error: if C_SU_NDIMS not vary, its numbers must align with C_SU_NAMES");
    else if(errwarn==12) 
      err("sin file readqhead error: C_SU_ID record not found immediately after C_SU_NAMES record.");
    else if(errwarn==22) err("sin file getviaqfile error: C_SU_NDIMS record not same length as C_SU_NAMES record.");
    else if(errwarn==23) err("sin file getviaqfile error: C_SU_NAMES tupled names out-of-order, changed");
    else if(errwarn==24) err("sin file getviaqfile error: C_SU_NDIMS blank where valid number expected");
    else if(errwarn==25) err("sin file getviaqfile error: C_SU_NDIMS non-number where valid number expected");
    else if(errwarn==26) err("sin file getviaqfile error: C_SU_NDIMS value must be same for all members of tuple");
    else if(errwarn==27) err("sin file getviaqfile error: C_SU_NAMES record followed by C_SU_ID record not found.");
    else if(errwarn>100) 
      err("sin file getviaqfile error: record %d (wrong comma count, damaged, non-numbers, ...)",errwarn-99);
    else if(errwarn>0) err("sin file getviaqfile error: unrecognized error code %d",errwarn);

    if(ifixd==0) err("sin file error: input with varying number of tuples is not allowed.");

    for (i=0; i<stree_numd; ++i) {
      slocn[i] = -1;
      for (j=0; j<iztuple; ++j) {
        if(strcmp(pname[j],skey[i])==0) slocn[i] = j;
      }
      if(slocn[i] < 0) err("sin file error: skeyloc %s not found in non-tuple part of sin Q-file.",skey[i]);
    }

    osloc = -1;
    for (j=0; j<iztuple; ++j) { 
      if(strcmp(pname[j],sstat)==0) osloc = j;
    }
    if(osloc<0) err("sin file error: Q-file must have sstat parameter value %s (among non-tuple names).",sstat);

    if(npoints<1) err("sin file error: no records starting with Q found in file."); 

    stree_npoints = npoints;
    stree_nodes = ealloc1(stree_npoints,sizeof(node));

    for(i=0; i<stree_numd; i++) {
      stree_dl[i] = ealloc1double(stree_npoints);
      for(j=0; j<stree_npoints; j++) stree_dl[i][j] = SInfo[j].dlots[slocn[i]];
    }

    ihop = 1;
    connect_nodes (stree_nodes, stree_npoints, stree_dl, stree_numd, ihop);

  } /* end of  if(stree_numd>0) */

/*--------------------------------------------------------------------------  */
/*- Read the parameters related to receiver statics. -----------------------  */
/*--------------------------------------------------------------------------  */

  rtree_numd = 1;
  if(countparval("rin")>0) {
    getparstring("rin", &Rname);
    if(strcmp(Rname,"none")==0) rtree_numd = 0; /* just a flag here */
  }
  else {
    Rname = ealloc1(9,1);
    strcpy(Rname,"rstat.csv");
  }

  if(rtree_numd>0) {

    rtree_numd = countparval("rkeyloc");
    if(rtree_numd<1) {
      rtree_numd = 1;
      rkey[0] = ealloc1(4,1);
      strcpy(rkey[0],"gaps");
    }
    else {
      getparstringarray("rkeyloc",rkey);
      if(rtree_numd>9) err("**** Error for rin parameters: Maximum of 9 location names can be specified.");
    }

    for(i=0; i<rtree_numd; i++) {

      rkase[i] = -1;
      if(strcmp(rkey[i],"sx")==0) rkase[i] = 101;
      else if(strcmp(rkey[i],"sy")==0) rkase[i] = 102;
      else if(strcmp(rkey[i],"gx")==0) rkase[i] = 103;
      else if(strcmp(rkey[i],"gy")==0) rkase[i] = 104; /* 105-110 in sunearcsv   */
      else if(strcmp(rkey[i],"selev")==0) rkase[i] = 111;
      else if(strcmp(rkey[i],"gelev")==0) rkase[i] = 112;
      else if(strcmp(rkey[i],"sdepth")==0) rkase[i] = 113;
      else if(strcmp(rkey[i],"sdel")==0) rkase[i] = 114;
      else if(strcmp(rkey[i],"gdel")==0) rkase[i] = 115;
      else if(strcmp(rkey[i],"swdep")==0) rkase[i] = 116;
      else if(strcmp(rkey[i],"gwdep")==0) rkase[i] = 117;
      else {
        rkase[i] = GetCase(rkey[i]);
        if(rkase[i]<1) err("**** Error for rin parameters: Specified location name %s is not recognized.",rkey[i]);
      }

    } /* end of  for(i=0; i<rtree_numd; i++) { */

    if (!getparstring("rstat", &rstat)) rstat = "gstat";

    if(!getpardouble("rmult",&rmult)) rmult = 1.0;
    if(rmult==0.0)  err("**** Error for rin parameters: rmult cannot be 0.0"); 

    if(getpardouble("rlimit",&rlimit)) {
      if(rlimit<0.0)  err("**** Error for rin parameters: rlimit cannot be less than 0.0"); 
      rlimit *= rlimit;
    }

    fpR = fopen(Rname, "r");
    if(fpR==NULL) err("error: rin file did not open correctly.");
    
/* Set input numpname,pname to just store what is going to be accessed.       */
/* numpname>0 is a flag to ONLY store values if they are on pname list.       */
/* numpname<1 is a flag to NOT store values if they are on pname list.        */

    pname = ealloc1(rtree_numd + 1,sizeof(cwp_String *));
    numpname = 0;
  
    for (i=0; i<rtree_numd; ++i) {
      pname[numpname] = ealloc1(strlen(rkey[i]),1);
      strcpy(pname[numpname],rkey[i]);
      numpname++;
    }
    pname[numpname] = ealloc1(strlen(rstat),1);
    strcpy(pname[numpname],rstat);
    numpname++;

    getviaqfile(fpR, &pname, &numpname, &iztuple, numdind,   
                &ktuple, &ifixd, &RInfo, &npoints, 
                &pindepa,  &ndims, &errwarn) ;

    if(errwarn==1) err("rin file getqinfo error: extra C_SU_NAMES record in q-file");
    else if(errwarn==2) err("rin file getqinfo error: extra C_SU_NDIMS record in q-file");
    else if(errwarn==3) err("rin file getqinfo error: C_SU_ID record not found immediately after C_SU_NAMES record.");
    else if(errwarn==11) 
      err("rin file readqhead error: if C_SU_NDIMS not vary, its numbers must align with C_SU_NAMES");
    else if(errwarn==12) 
      err("rin file readqhead error: C_SU_ID record not found immediately after C_SU_NAMES record.");
    else if(errwarn==22) err("rin file getviaqfile error: C_SU_NDIMS record not same length as C_SU_NAMES record.");
    else if(errwarn==23) err("rin file getviaqfile error: C_SU_NAMES tupled names out-of-order, changed");
    else if(errwarn==24) err("rin file getviaqfile error: C_SU_NDIMS blank where valid number expected");
    else if(errwarn==25) err("rin file getviaqfile error: C_SU_NDIMS non-number where valid number expected");
    else if(errwarn==26) err("rin file getviaqfile error: C_SU_NDIMS value must be same for all members of tuple");
    else if(errwarn==27) err("rin file getviaqfile error: C_SU_NAMES record followed by C_SU_ID record not found.");
    else if(errwarn>100) 
      err("rin file getviaqfile error: record %d (wrong comma count, damaged, non-numbers, ...)",errwarn-99);
    else if(errwarn>0) err("rin file getviaqfile error: unrecognized error code %d",errwarn);

    if(ifixd==0) err("rin file error: input with varying number of tuples is not allowed.");

    for (i=0; i<rtree_numd; ++i) {
      rlocn[i] = -1;
      for (j=0; j<iztuple; ++j) {
        if(strcmp(pname[j],rkey[i])==0) rlocn[i] = j;
      }
      if(rlocn[i] < 0) err("rin file error: rkeyloc %s not found in non-tuple part of rin Q-file.",rkey[i]);
    }

    orloc = -1;
    for (j=0; j<iztuple; ++j) { 
      if(strcmp(pname[j],rstat)==0) orloc = j;
    }
    if(orloc<0) err("error: rin Q-file must have rstat parameter value %s (among non-tuple names).",rstat);

    if(npoints<1) err("rin file error: no records starting with Q found in file."); 

    rtree_npoints = npoints;
    rtree_nodes = ealloc1(rtree_npoints,sizeof(node));

    for(i=0; i<rtree_numd; i++) {
      rtree_dl[i] = ealloc1double(rtree_npoints);
      for(j=0; j<rtree_npoints; j++) rtree_dl[i][j] = RInfo[j].dlots[rlocn[i]];
    }

    ihop = 1;
    connect_nodes (rtree_nodes, rtree_npoints, rtree_dl, rtree_numd, ihop);

  } /* end of  if(rtree_numd>0) */

/*--------------------------------------------------------------------------  */
/*- Read the parameters related to cdp statics.    -------------------------  */
/*--------------------------------------------------------------------------  */

  ctree_numd = 1;
  if(countparval("cin")>0) {
    getparstring("cin", &Cname);
    if(strcmp(Cname,"none")==0) ctree_numd = 0; /* just a flag here */
  }
  else {
    ctree_numd = 0; /* default for cin is none */
  }

  if(ctree_numd>0) {

    ctree_numd = countparval("ckeyloc");
    if(ctree_numd<1) {
      ctree_numd = 1;
      ckey[0] = ealloc1(3,1);
      strcpy(ckey[0],"cdp");
    }
    else {
      getparstringarray("ckeyloc",ckey);
      if(ctree_numd>9) err("**** Error for cin parameters: Maximum of 9 location names can be specified.");
    }

    for(i=0; i<ctree_numd; i++) {

      ckase[i] = -1;
      if(strcmp(ckey[i],"sx")==0) ckase[i] = 101;
      else if(strcmp(ckey[i],"sy")==0) ckase[i] = 102;
      else if(strcmp(ckey[i],"gx")==0) ckase[i] = 103;
      else if(strcmp(ckey[i],"gy")==0) ckase[i] = 104; /* 105-110 in sunearcsv   */
      else if(strcmp(ckey[i],"selev")==0) ckase[i] = 111;
      else if(strcmp(ckey[i],"gelev")==0) ckase[i] = 112;
      else if(strcmp(ckey[i],"sdepth")==0) ckase[i] = 113;
      else if(strcmp(ckey[i],"sdel")==0) ckase[i] = 114;
      else if(strcmp(ckey[i],"gdel")==0) ckase[i] = 115;
      else if(strcmp(ckey[i],"swdep")==0) ckase[i] = 116;
      else if(strcmp(ckey[i],"gwdep")==0) ckase[i] = 117;
      else {
        ckase[i] = GetCase(ckey[i]);
        if(ckase[i]<1) err("**** Error for cin parameters: Specified location name %s is not recognized.",ckey[i]);
      }

    } /* end of  for(i=0; i<ctree_numd; i++) { */

    if (!getparstring("cstat", &cstat)) cstat = "tstat";

    if(!getpardouble("cmult",&cmult)) cmult = -2.0;
    if(cmult==0.0)  err("**** Error for cin parameters: cmult cannot be 0.0"); 

    if(getpardouble("climit",&climit)) {
      if(climit<0.0)  err("**** Error for cin parameters: climit cannot be less than 0.0"); 
      climit *= climit;
    }

    fpC = fopen(Cname, "r");
    if(fpC==NULL) err("error: cin file did not open correctly.");
    
/* Set input numpname,pname to just store what is going to be accessed.       */
/* numpname>0 is a flag to ONLY store values if they are on pname list.       */
/* numpname<1 is a flag to NOT store values if they are on pname list.        */

    pname = ealloc1(ctree_numd + 1,sizeof(cwp_String *));
    numpname = 0;
  
    for (i=0; i<ctree_numd; ++i) {
      pname[numpname] = ealloc1(strlen(ckey[i]),1);
      strcpy(pname[numpname],ckey[i]);
      numpname++;
    }
    pname[numpname] = ealloc1(strlen(cstat),1);
    strcpy(pname[numpname],cstat);
    numpname++;

    getviaqfile(fpC, &pname, &numpname, &iztuple, numdind,   
                &ktuple, &ifixd, &CInfo, &npoints, 
                &pindepa,  &ndims, &errwarn) ;

    if(errwarn==1) err("cin file getqinfo error: extra C_SU_NAMES record in q-file");
    else if(errwarn==2) err("cin file getqinfo error: extra C_SU_NDIMS record in q-file");
    else if(errwarn==3) err("cin file getqinfo error: C_SU_ID record not found immediately after C_SU_NAMES record.");
    else if(errwarn==11) 
      err("cin file readqhead error: if C_SU_NDIMS not vary, its numbers must align with C_SU_NAMES");
    else if(errwarn==12) 
      err("cin file readqhead error: C_SU_ID record not found immediately after C_SU_NAMES record.");
    else if(errwarn==22) err("cin file getviaqfile error: C_SU_NDIMS record not same length as C_SU_NAMES record.");
    else if(errwarn==23) err("cin file getviaqfile error: C_SU_NAMES tupled names out-of-order, changed");
    else if(errwarn==24) err("cin file getviaqfile error: C_SU_NDIMS blank where valid number expected");
    else if(errwarn==25) err("cin file getviaqfile error: C_SU_NDIMS non-number where valid number expected");
    else if(errwarn==26) err("cin file getviaqfile error: C_SU_NDIMS value must be same for all members of tuple");
    else if(errwarn==27) err("cin file getviaqfile error: C_SU_NAMES record followed by C_SU_ID record not found.");
    else if(errwarn>100) 
      err("cin file getviaqfile error: record %d (wrong comma count, damaged, non-numbers, ...)",errwarn-99);
    else if(errwarn>0) err("cin file getviaqfile error: unrecognized error code %d",errwarn);

    if(ifixd==0) err("cin file error: input with varying number of tuples is not allowed.");

    for (i=0; i<ctree_numd; ++i) {
      clocn[i] = -1;
      for (j=0; j<iztuple; ++j) {
        if(strcmp(pname[j],ckey[i])==0) clocn[i] = j;
      }
      if(clocn[i] < 0) err("cin file error: ckeyloc %s not found in non-tuple part of cin Q-file.",ckey[i]);
    }

    ocloc = -1;
    for (j=0; j<iztuple; ++j) { 
      if(strcmp(pname[j],cstat)==0) ocloc = j;
    }
    if(ocloc<0) err("error: cin Q-file must have cstat parameter value %s (among non-tuple names).",cstat);

    if(npoints<1) err("cin file error: no records starting with Q found in file."); 

    ctree_npoints = npoints;
    ctree_nodes = ealloc1(ctree_npoints,sizeof(node));

    for(i=0; i<ctree_numd; i++) {
      ctree_dl[i] = ealloc1double(ctree_npoints);
      for(j=0; j<ctree_npoints; j++) ctree_dl[i][j] = CInfo[j].dlots[clocn[i]];
    }

    ihop = 1;
    connect_nodes (ctree_nodes, ctree_npoints, ctree_dl, ctree_numd, ihop);

  } /* end of  if(ctree_numd>0) */

/*--------------------------------------------------------------------------  */

  checkpars(); 

/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */
/*--------------------------------------------------------------------------  */

  if (!gettr(&tr))  err("Error: cannot get first trace");

  lentrc = tr.ns;
  onetrc = ealloc1float(lentrc);
  oneshift = ealloc1float(lentrc);
  sampi = ((float)tr.dt) / 1000.0;

/* loop over traces   */ 

  do {

    dt = 0.0;

    if(stree_numd>0) {
      for(i=0; i<stree_numd; i++) target[i] = scaledfromhead(tr, skase[i]); 

      if(kdadd==1) sdist2 = sdist + sqrt(snear_dist);
      cycle_for_near(stree_nodes,stree_dl,tree_nt,stree_numd,
                     extent_min, extent_max, target, sdist2, kmult,
                     &near_elem, &snear_dist, &snum_found, &kcycles);

      if(snear_dist<=slimit) dt += SInfo[near_elem].dlots[osloc] * smult;
    } /* end of    if(stree_numd>0) */

    if(rtree_numd>0) {
      for(i=0; i<rtree_numd; i++) target[i] = scaledfromhead(tr, rkase[i]); 

      if(kdadd==1) rdist2 = rdist + sqrt(rnear_dist);
      cycle_for_near(rtree_nodes,rtree_dl,tree_nt,rtree_numd,
                     extent_min, extent_max, target, rdist2, kmult,
                     &near_elem, &rnear_dist, &rnum_found, &kcycles);

      if(rnear_dist<=rlimit) dt += RInfo[near_elem].dlots[orloc] * rmult;
    } /* end of    if(rtree_numd>0) */

    if(ctree_numd>0) {
      for(i=0; i<ctree_numd; i++) target[i] = scaledfromhead(tr, ckase[i]); 

      if(kdadd==1) cdist2 = cdist + sqrt(cnear_dist);
      cycle_for_near(ctree_nodes,ctree_dl,tree_nt,ctree_numd,
                     extent_min, extent_max, target, cdist2, kmult,
                     &near_elem, &cnear_dist, &cnum_found, &kcycles);

      if(cnear_dist<=climit) dt += CInfo[near_elem].dlots[ocloc] * cmult;
    } /* end of    if(ctree_numd>0) */

    for (i=0; i<lentrc; i++) {
      onetrc[i] = tr.data[i];
      oneshift[i] = (float) i - dt/sampi;
    }
    ints8r(lentrc, 1.0, 0.0, onetrc, 0.0, 0.0, lentrc, oneshift, tr.data);

    puttr(&tr);
    nproct++;

  } while (gettr(&tr));

  warn("Number of traces=%d",nproct);

  return 0;

} /* end of main for sushiftcsv */

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

void find_near_rcur (double **tree_dl, int *tree_nt, int tree_numd, node * now_node, int naxe, 
                     double *extent_min, double *extent_max, double *target, 
                     unsigned long long *near_elem, double *near_dist, unsigned long long *num_found) {

/*          This function finds a nearest point between specified extents of the dimensions.           */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_nt       Near type flags. 1 means standard pythagorean nearest. See note for what 0 means.     */     
/* tree_numd     Number of pointers in tree_dl and flags in tree_nt (i.e. number of dimensions).       */     
/* extent_min    Minimum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/* target        Find the point nearest here considering extents and tree_nt. Size tree_numd.          */     
/*                                                                                                     */     
/* Input/Output arguments:                                                                             */     
/* now_node      The current node to start from.                                                       */     
/* naxe          The current splitting dimension.                                                      */     
/*                                                                                                     */     
/* Output arguments:                                                                                   */     
/* near_elem     The element number of a nearest point in the tree_dl arrays. For instance, a value of */     
/*               m means the coordinates of point are tree_dl[n][m] where n is the dimension number.   */     
/* near_dist     The SQUARED distance between the target and nearest point (if num_found>0).           */
/* num_found     0 if no point is found within the extent ranges.                                      */
/*              >0 is as many equally-near points as are found within the extent ranges.               */
/*                 The returned near_elem is the highest-numbered elem of all the nearest points.      */
/*                                                                                                     */     
/*    NOTE: A tree_nt value of 0 means the difference between the point coordinate and the target      */     
/*          coordinate are not added to the Pythagorean sum for the specified dimension(s).            */     
/*          So, the point distances are determined as if that dimension was not specified at all.      */     
/*          However, the extent_min,extent_max values for that dimension are still used.               */     
/*          The result is therefor the nearest point considering only the other dimensions, but        */     
/*          still restricted by the extent range of that dimension(s).                                 */     

  if(now_node==0) return; 

  bool in = true;          
  int i = 0;
  double rad = 0.;
 
  for(i=0; i<tree_numd; i++) {
    if(tree_dl[i][now_node->elem] < extent_min[i] || tree_dl[i][now_node->elem] >= extent_max[i]) { 
      in = false;
      break;
    }
  }

  if(in) { 
    rad = 0.;
    for(i=0; i<tree_numd; i++) {
      if(tree_nt[i] != 0) {
        rad += (target[i]-tree_dl[i][now_node->elem]) * (target[i]-tree_dl[i][now_node->elem]);
      }
    }

/* The following set of ifs could be done differently. But I think coding it  */
/* this way saves cpu time by immediately rejecting the > cases using 1 if.   */
/* But who knows how modern optimizers will alter this.                       */

    if(rad <= *near_dist) {
      if(rad < *near_dist) { /* if distance is smaller, reset count to 1 */
        *num_found = 1;
        *near_elem = now_node->elem;
        *near_dist = rad; 
      }
      else { /* so, distances are equal. Increment count, set output to higher elem.*/
        *num_found = *num_found + 1;
        if(now_node->elem > *near_elem) *near_elem = now_node->elem;
      }
    }

  }

/* Find more. */

  if(naxe==tree_numd) naxe = 0;

  if(tree_dl[naxe][now_node->elem] >= extent_min[naxe] && now_node->l)
    find_near_rcur(tree_dl, tree_nt, tree_numd, now_node->l, naxe+1,
                   extent_min, extent_max, target, 
                   near_elem, near_dist, num_found);

  if(tree_dl[naxe][now_node->elem] <  extent_max[naxe] && now_node->r)
    find_near_rcur(tree_dl, tree_nt, tree_numd, now_node->r, naxe+1, 
                   extent_min, extent_max, target, 
                   near_elem, near_dist, num_found);

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void cycle_for_near(node *tree_nodes, double **tree_dl, int *tree_nt, int tree_numd,
                    double *extent_min, double *extent_max, double *target, 
                    double init_rad, double rad_scal,  
                    unsigned long long *near_elem, double *near_dist,
                    unsigned long long *num_found, int *ncycles) {

/*          This function finds a nearest point between specified extents of the dimensions.           */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated and connected tree nodes.                                 */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_nt       Near type flags. 1 means standard pythagorean nearest. See note for what 0 means.     */     
/* tree_numd     Number of pointers in tree_dl and flags in tree_nt (i.e. number of dimensions).       */     
/* extent_min    Minimum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/* target        Find the point nearest here considering extents and tree_nt. Size tree_numd.          */     
/* init_rd       Initially search within radius init_rad. If no point within extents is found,         */     
/*               init_rad is multiplied by rad_scal and the seach is repeated. And so on.              */     
/* rad_scal      Scale value to apply to init_rd until a point is found within extents, or the         */     
/*               init_rad has been scaled to larger than the extents.                                  */     
/*                                                                                                     */     
/* Output arguments:                                                                                   */     
/* near_elem     The element number of a nearest point in the tree_dl arrays. For instance, a value of */     
/*               m means the coordinates of point are tree_dl[n][m] where n is the dimension number.   */     
/* near_dist     The SQUARED distance between the target and nearest point (if num_found>0).           */
/* num_found     0 if no point is found within the extent ranges.                                      */
/*              >0 is as many equally-near points as are found within the extent ranges.               */
/*                 The returned near_elem is the highest-numbered elem of all the nearest points.      */
/*                                                                                                     */     
/* ncycles       Number of cycles to find nearest point. This is just for informational purposes.      */
/*               This is the number of times that the rad_scal had to be applied before finding the    */
/*               nearest point. This could help set init_rad and rad_scal for faster searches.         */
/*                                                                                                     */     
/*    NOTE: A tree_nt value of 0 means the difference between the point coordinate and the target      */     
/*          coordinate are not added to the Pythagorean sum for the specified dimension(s).            */     
/*          So, the point distances are determined as if that dimension was not specified at all.      */     
/*          However, the extent_min,extent_max values for that dimension are still used.               */     
/*          The result is therefor the nearest point considering only the other dimensions, but        */     
/*          still restricted by the extent range of that dimension(s).                                 */     

  int naxe = 1;
  node *now_node;
  double now_rad = init_rad;
  double loc_min[9];
  double loc_max[9]; 
  int nset = 0;
  int i = 0;
  int nbig = 0;

  for(i=0; i<tree_numd; i++) {
    if(tree_nt[i] == 0) {
      loc_min[i] = extent_min[i];
      loc_max[i] = extent_max[i];
      nset+=2;
    }
  }

  *ncycles = 0;

CYCLE:

  *ncycles = *ncycles + 1;

/* Set extents using current radius. Also set nbig which tells us whether */
/* we are beyond the extents (therefor no reason to keep looking).        */

  nbig = nset;
  for(i=0; i<tree_numd; i++) {
    if(tree_nt[i] != 0) {
      loc_min[i] = target[i] - now_rad;
      loc_max[i] = target[i] + now_rad;
      if(loc_min[i] <= extent_min[i]) {
        loc_min[i] = extent_min[i];
        nbig++;
      }
      if(loc_max[i] >= extent_max[i]) { /* yes, >= is better than > here */ 
        loc_max[i] = extent_max[i];
        nbig++;
      }
    }
  }

  *num_found = 0;
  *near_dist = DBL_MAX;
  *near_elem = 0; 
  
  naxe = 1;
  now_node = tree_nodes; 
  find_near_rcur(tree_dl, tree_nt, tree_numd, now_node, naxe, 
                 loc_min, loc_max, target, 
                 near_elem, near_dist, num_found);

  if(*num_found<1) {
    if(nbig==2*tree_numd) return; /* None found. Are we outside the extents?  */
    now_rad = now_rad * rad_scal;
    goto CYCLE;
  }

/* Here, we need to consider the difference between a square and a circle.    */ 
/* The now_rad value is half the size of the square we just searched. So, if  */
/* current nearest point is in a circle with that radius, we are finished.    */
/* But, otherwise, there might be nearer points hiding in the area outside    */
/* the searched-square, but inside the circle. So increase the search size    */
/* to the CURRENT nearest point radius. On the next cycle, we will definitly  */        
/* get the nearest point because the CURRENT point is definitly within the    */
/* square with the now_rad that we are now setting. So the CURRENT point here */
/* will be among the points returned by the next cycle. And its radius will   */
/* definitly satisfy this condition because we explicitly made now_rad big    */
/* enough (but another point hiding in the square-circle area might sneak in).*/

  if(*near_dist > now_rad*now_rad && nbig!=2*tree_numd) {
    now_rad = sqrt(*near_dist) * 1.001;
    goto CYCLE;
  }

  return;
}




double scaledfromhead(segy tr, int k) {

  double dval;

  switch (k) {
      
    case 101:
      dval = (double)(tr.sx);
      if(tr.scalco > 1) dval *= tr.scalco;
      else if(tr.scalco < 0) dval /= -tr.scalco;
    break;
        
    case 102:
      dval = (double)(tr.sy);
      if(tr.scalco > 1) dval *= tr.scalco;
      else if(tr.scalco < 0) dval /= -tr.scalco;
    break;
        
    case 103:
      dval = (double)(tr.gx);
      if(tr.scalco > 1) dval *= tr.scalco;
      else if(tr.scalco < 0) dval /= -tr.scalco;
    break;
        
    case 104:
      dval = (double)(tr.gy);
      if(tr.scalco > 1) dval *= tr.scalco;
      else if(tr.scalco < 0) dval /= -tr.scalco;
    break;
        
    case 111:
      dval = (double)(tr.selev);
      if(tr.scalel > 1) dval *= tr.scalel;
      else if(tr.scalel < 0) dval /= -tr.scalel;
    break;

    case 112:
      dval = (double)(tr.gelev);
      if(tr.scalel > 1) dval *= tr.scalel;
      else if(tr.scalel < 0) dval /= -tr.scalel;
    break;
   
    case 113:
      dval = (double)(tr.sdepth);
      if(tr.scalel > 1) dval *= tr.scalel;
      else if(tr.scalel < 0) dval /= -tr.scalel;
    break;

    case 114:
      dval = (double)(tr.sdel);
      if(tr.scalel > 1) dval *= tr.scalel;
      else if(tr.scalel < 0) dval /= -tr.scalel;
    break;

    case 115:
      dval = (double)(tr.gdel);
      if(tr.scalel > 1) dval *= tr.scalel;
      else if(tr.scalel < 0) dval /= -tr.scalel;
    break;

    case 116:
      dval = (double)(tr.swdep);
      if(tr.scalel > 1) dval *= tr.scalel;
      else if(tr.scalel < 0) dval /= -tr.scalel;
    break;

    case 117:
      dval = (double)(tr.gwdep);
      if(tr.scalel > 1) dval *= tr.scalel;
      else if(tr.scalel < 0) dval /= -tr.scalel;
    break;

    default:
      dval = fromhead(tr, k);
    break;

  } /* end of switch */

  return (dval);
}
