/* Copyright (c) Colorado School of Mines, 2022.*/
/* All rights reserved.                       */

/* SUSEMBASE: $Revision: 1.00 $ ; $Date: 2023/02/12 00:01:00 $      */

#include <stdio.h>
#include <string.h>

#include "su.h"
#include "segy.h"
#include "headcase.h"

segy tr;
segy trd;

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                            ",
" SUSEMGROUP - Semblance Based Structure-Oriented Smooth In Trace Groups.    ",
"                                                                            ",
" This program performs semblance scanning to derive (apparent) dips in the  ",
" direction of offsets within cdps (or other key values). It then uses those ",
" dips to align trace sample values and performs a normalized sum (which     ",
" reduces noise). Since the dip compensation produces locally-flat horizons  ",
" during summing, the result is less smearing of those horizons.             ",
"      This program does not interpolate. No traces will be created.         ",
"      This program can process 2D and 3D stacked data. Since interpolation  ",
"      is often desirable for stacked data, see program susembase for that.  ",
"                                                                            ",
" susemgroup   <in.su >out.su  (other parameters)                            ",
"                                                                            ",
" Parameter overview:                                                        ",
"                                                                            ",
" lockey=offset Trace key that indicates location within groupkey group.     ",
"               (Traces within groups do not have to be sorted by this key). ",
"         Note: Other parameters default to values corresponding to this     ",
"               lockey being offset. For 2D stacked data, you should SPECIFY ",
"               lockkey=cdp and groupkey=none and approximately scanstep=0.5 ",
"               and scanmax=12 and locmaxscan=3. For 3D stacked data, you    ",
"               should SPECIFY lockkey=igi or igc and groupkey=igc or igi and",
"               approximately scanstep=0.5 and scanmax=12 and locmaxscan=3.  ",
"                                                                            ",
" groupkey=cdp  Trace key that indicates a group of traces. Traces with the  ",  
"               same groupkey value are processed as a group. Any CHANGE of  ",  
"               this groupkey value indicates a new group. For cdps this     ",  
"               grouping is typically achieved by sorting. But, for instance,",  
"               a 3D shot gather typically contains traces in receiver line  ",  
"               groups, with NEW groups being defined herein for subsequent  ",  
"               shots even though the receiver line numbers REPEAT.          ",  
"         =none There is only 1 group and all lockey values are in the group.",  
"               Typically this option is for stacked input. For instance,    ",  
"               to input a 2D stack specify lockey=cdp (and groupkey=none).  ",  
"               For a 3D stack then lockey,groupkey are igi,igc then igc,igi ",  
"               (or vice-versa, depending on which direction you sort first).",  
"               For large stacked data, also increase the maxfold value.     ",
"         Note: If you need to use susort, remember that the sort keys are   ",  
"               listed in reverse order of lockey and groupkey here.         ",  
"                                                                            ",
" maxfold=999  This is maximum number of traces in a group. For instance,    ",
"              if groupkey=cdp this is maximum number of traces in any cdp.  ",
"                                                                            ",
" locmaxscan=75 Maximum Relative lockey to include in scanning for dips.     ",
"               Must be greater than or equal to 0. This program scans traces",
"               from -locmaxscan to locmaxscan (inclusive) at each location. ",
"           =0  Do not scan for dips. You must specify parameter locmaxsum   ",
"               and you cannot specify scanstep, scanmax, scanmin, dipext.   ",
"               Note: The default value corresponds to a lockey=offset,      ",
"                     so the 75 typically means 75 metres or 75 feet.        ",
"                                                                            ",
" scanstep=0.05      Time Step to Scan (milliseconds per 1 lockey unit).     ",
" scanmax =1.1       Maximum Time to Scan (milliseconds per 1 lockey unit).  ",
" scanmin =-scanmax  Minimum Time to Scan (milliseconds per 1 lockey unit).  ",
"              Note: These defaults correspond to a lockey=offset, so these  ",
"                    values are typically milliseconds per metre or foot.    ",
"                                                                            ",
" dipext=12. Dip Extender (milliseconds).                                    ",
"            At each location the semblance scanning produces an estimate    ",
"            of the dip at each sample time. This parameter performs a       ",
"            weighted-average of those estimated dips symmetrically from     ",
"            -dipext to +dipext. The weights used are the sum of the         ",
"            squared amplitudes of seismic values along the estimated dip    ",
"            in the locmaxscan range of input traces. In cruder terms: the   ",
"            power along the dip value chozen by the semblance scanning.     ",
"            Effectively, this causes the dip values from large horizons     ",
"            to be trusted more and extended up and down nearby times.       ",
"                                                                            ",
" dipuse=1  Estimate and use dips at input locations. (Same options as the   ",
"           susembase program, but since input and output locations are same ",
"           herein, the actual decision is how you want to use the dips).    ",
"       =0  Estimate and use dips at output locations.                       ",
"                                                                            ",
" locmaxsum=locmaxscan  Maximum Relative lockey to include in normalized sum.",
"                       Must be greater than 0. This program sums the input  ",
"                       traces from -locmaxsum to locmaxsum (inclusive) at   ",
"                       each location (and divides by the total number).     ",
"                 Note: Remember that the sample values of these traces are  ",
"                       corrected by their estimated dips before summing, so ",
"                       the result is less smeared than uncorrected values.  ",
"                                                                            ",
" outdip=   Do not output this file.                                         ",
"       =(file name) Output the dip values chosen by the semblance scanning  ",
"           into trace samples in this file. The trace header values are     ",
"           copied from the seismic traces.                                  ",
"     Note: These dips are after dipext processing. But even with dipext=0   ",
"           they will not have scanstep granularity because the scanned      ",
"           maximum semblance dip undergoes a 3-point interpolation to get   ",
"           a better approximation of the dip at the true peak in semblance. ",
"     Note: It is technically more correct to call the dips made and used    ",
"           by this program APPARANT dips. The dip values are put into the   ",
"           trace samples and are in milliseconds per 1 location unit.       ",
"                                                                            ",
" outpow=   Do not output this file.                                         ",
"       =(file name) Output the average power along the dip direction chosen ",
"           by the semblance scanning. (Same header values as for outdip).   ",
"                                                                            ",
" outsem=   Do not output this file.                                         ",
"       =(file name) Output the semblance value computed and chosen by the   ",
"           semblance scanning. (Same header values as for outdip).          ",
"                                                                            ",
" indip=    Name of trace file which contains (apparant) dip values.         ",
"           Dip scanning is disabled if you use this parameter. Thus you     ",
"           cannot specify parameters scanstep, scanmax, scanmin, dipext,    ",
"           and locmaxscan - and you must specify parameter locmaxsum.       ",
"           This file is usually initially made by specifying outdip above.  ",
"     Note: The intention here is to allow you to output dips from one run   ",
"           of this program and use them in a second run of this program.    ",
"           The first run can input smoothed, denoised seismic to get dips,  ",
"           and the second run can input the un-smoothed, noisy seismic and  ",
"           use the dips from the first run. As well, you can also actually  ",
"           smooth and denoise the DIP VALUES in this file before input here.",
"                                                                            ",
" Note: This program enforces non-hierarchical parameter error checking.     ",
"       This means if you disable a main option, all parameters related      ",
"       to it must be removed from the command line. That is, for novice     ",
"       users this program is sufficiently complex that it refuses to guess  ",
"       which parameter was specified in error.                              ",
"                                                                            ",
" ------------------------------------------------------------------------   ", 
"                                                                            ",
NULL};

/* Credits:                                                                  */
/* Author: Andre Latour, Feb 2023                                            */ 
/* 1. This program derived from susembase, so retaining the rolling names    */
/*    even though this program is not employing rolling storage technique.   */
/* 2. As of Feb 2023, scanit functions in susembase and herein are identical.*/
/*                                                                           */
/* Trace keys usually involved: offset,cdp                                   */
/*                                                                           */
/**************** end self doc ***********************************************/

void scanit (segy **atrace, int *aloc, int numrolling, double scanloc, 
            double locminscan, double locmaxscan, float smin, float sstep, int jsmax, 
            float *adip, float *apow, float *asem, float *awrk, float *awrq, 
            int *arng, int **ifori, float **afori, int lendip, int *numout) ;

void wsmooth (float *val, float *wgt, int len, int minsd, int maxsd, float *sval) ;

int main(int argc, char **argv) {

  cwp_String lockey=NULL;
  int lockase=-1;

  cwp_String groupkey=NULL;
  int groupkase=-1;
  int usegroup=1;
  double thisgroup=-1.e30;

  segy **rolltrace=NULL; 
  segy *onetrace=NULL; 
  int    *rollloc=NULL;
  float  **rolldip=NULL;
  float  **jsumdat=NULL;
  float  *onedip=NULL;
  float  *onepow=NULL;
  float  *onesem=NULL;
  float  *onetim=NULL;
  float  *dipwrk=NULL;
  float  *dipwrq=NULL;
  int    *arng=NULL;
  int   **ifori=NULL;
  float **afori=NULL;

  int lendip=0;
  int nproct=0;
  int mproct=0;
  int numout=0;
  int icycle=1;
  int iroll=1;
  int jroll=1;
  int dipuse=1;
  int i=0;
  int j=0;
  int jloc=0;
  int jsum=0;
  double outputloc = 0.0;
  double locminscan = -3.0;
  double locmaxscan = 3.0;
  double locminsum  = -3.0;
  double locmaxsum  = 3.0;
  int numrolling=999;                                                                   
  float dt=0.;
  float sstep=1.0;
  float smin=-8.;
  float smax=8.;
  float jsmax=0;
  float dipext=12.;
  int   minsd=-1;
  int   maxsd=1;

  cwp_String dname=NULL; 

  int indip = 0;
  FILE *fpindip=NULL;      

  int outdip = 0;
  FILE *fpoutdip=NULL;      
  int outpow = 0;
  FILE *fpoutpow=NULL;      
  int outsem = 0;
  FILE *fpoutsem=NULL;      

/* Initialize */

  initargs(argc, argv);
  requestdoc(1);

  if(countparval("lockey") > 0) {
    getparstring("lockey", &lockey);
  }
  else {
    lockey = ealloc1(6,1);
    strcpy(lockey,"offset");
  }
  lockase = GetCase(lockey);
  if(lockase<1) err("**** Error: Specified lockey name %s is not recognized.",lockey);

  usegroup = 1;
  if(countparval("groupkey") > 0) {
    getparstring("groupkey", &groupkey);
    if(strcmp(groupkey,"none") == 0) usegroup = 0;
  }
  else {
    groupkey = ealloc1(3,1);
    strcpy(groupkey,"cdp");
  }
  if(usegroup>0) {
    groupkase = GetCase(groupkey);
    if(groupkase<1) err("**** Error: Specified groupkey name %s is not recognized.",groupkey);
  }

  if(!getpardouble("locmaxscan", &locmaxscan)) locmaxscan = 75.0;
  if(locmaxscan<0.0) err("error: locmaxscan must be greater than or equal to 0.");
  locminscan = 0.0 - locmaxscan;

  if(locmaxscan==0.0) {
    if(countparval("scanstep") > 0 || countparval("scanmax") > 0 || countparval("scanmin") > 0 ||
       countparval("dipext") > 0 || countparval("locmaxsum") < 1 ) {
       err("error: locmaxscan=0 does not allow parameters scanstep, scanmax, scanmin, dipext, and REQUIRES locmaxsum");
    }
  }

  if(!getpardouble("locmaxsum", &locmaxsum)) locmaxsum = locmaxscan;
  if(locmaxsum<=0.0) err("error: locmaxsum must be greater than 0.");
  locminsum = 0.0 - locmaxsum;
 
  if(!getparint("dipuse", &dipuse)) dipuse = 1;
  if(dipuse<0 || dipuse>1) err("error: dipuse parameter is out-of-range.");
 
  if(!getparfloat("dipext", &dipext)) dipext = 12.0;
  if(dipext<0.0) err("error: dipext must be equal to or greater than 0.0");

  if(!getparfloat("scanstep", &sstep)) sstep = 0.05;
  if(sstep<0.0) err("error: scanstep must be equal to or greater than 0.0");

  if(!getparfloat("scanmax", &smax)) smax = 1.1;

  if(!getparfloat("scanmin", &smin)) smin = 0.0 - smax;
  if(smax<smin) err("error: scanmax must be equal to or greater than scanmin");

/* This program derived from susembase, so retaining the rolling names,       */
/* even though this program is not employing the rolling storage technique.   */

  if(!getparint("maxfold", &numrolling)) numrolling = 999;
  if(numrolling<2) err("error: maxfold must be greater than 1");

  if(countparval("outdip") > 0) {
    getparstring("outdip", &dname);
    fpoutdip = efopen(dname, "w");
    if(fpoutdip==NULL) err("error: outdip file did not open correctly.");
    outdip = 1;
  }

  if(countparval("outpow") > 0) {
    getparstring("outpow", &dname);
    fpoutpow = efopen(dname, "w");
    if(fpoutpow==NULL) err("error: outpow file did not open correctly.");
    outpow = 1;
  }

  if(countparval("outsem") > 0) {
    getparstring("outsem", &dname);
    fpoutsem = efopen(dname, "w");
    if(fpoutsem==NULL) err("error: outsem file did not open correctly.");
    outsem = 1;
  }

/* Do this last to retain dname in case of error print */

  if(countparval("indip") > 0) { 
    if(countparval("scanstep") > 0 || countparval("scanmax") > 0 || countparval("scanmin") > 0 ||
       countparval("dipext") > 0 || countparval("locmaxscan") > 0 ) {
      err("error: indip file does not allow parameters scanstep, scanmax, scanmin, dipext, and locmaxscan");
    }
    getparstring("indip", &dname); 
    fpindip = efopen(dname, "r");
    if(fpindip==NULL) err("error: indip file did not open correctly.");
    indip = 1;
  }

/* -------------------------------------------------------------------------- */

  rollloc   = (int *)ealloc1(sizeof(int),numrolling);
  rolltrace = (segy **)ealloc1(sizeof(segy*),numrolling);
  onetrace  = (segy *)ealloc1(sizeof(segy),sizeof(char));
  rolldip   = (float **)ealloc1(sizeof(float*),numrolling);
  jsumdat   = (float **)ealloc1(sizeof(float*),numrolling);

  for (i=0; i<numrolling; i++) {
    rollloc[i]  = 0;
    rolltrace[i] = (segy *)ealloc1(sizeof(segy),sizeof(char));
  }

  arng = ealloc1int(numrolling);
  ifori = (int **)ealloc1(sizeof(int*),numrolling);
  afori = (float **)ealloc1(sizeof(float*),numrolling);

  checkpars(); 

/* ---Start processing traces.--------------------------------    */

  icycle = 1;
  outputloc = 0.0;

/* loop over traces   */ 

  while(icycle==1) {
    if(gettr(&tr)) {

/* To get started, pretend that numgather locations were already stored but   */
/* were lower than the scan range for the first input location.               */

      if(nproct==0) {
        iroll  = 0;                                      
        jroll  = 0;                                      
        dt     = ((double) tr.dt)/1000.0;
        sstep  = sstep / dt;
        smin   = smin  / dt;
        smax   = smax  / dt;
        maxsd  = (dipext+0.01) / dt;
        minsd  = 0 - maxsd;
        jsmax  = 1 + (smax-smin)/sstep;
        dipwrk = ealloc1float(jsmax);
        dipwrq = ealloc1float(jsmax);
        lendip = tr.ns;
        onedip = ealloc1float(lendip);
        onepow = ealloc1float(lendip);
        onesem = ealloc1float(lendip);
        onetim = ealloc1float(lendip);
        for (i=0; i<numrolling; i++) {
          rolldip[i] = ealloc1float(lendip);
          jsumdat[i] = ealloc1float(lendip);
          ifori[i] = ealloc1int(jsmax);
          afori[i] = ealloc1float(jsmax);
/* Initialize location values to definitly out-of-range */
          rollloc[i] = fromhead(tr,lockase) + 3.0*(locminscan + locminsum) - i - 5;
        }
      }
      nproct++;

      if(usegroup>0) {
        jroll = 0;
        if(thisgroup != fromhead(tr,groupkase)) {
          thisgroup = fromhead(tr,groupkase);
          jroll = iroll;
          iroll = 0;
        }
      }

    }
    else { /* no more traces to input */
      icycle = 0;
      jroll = iroll;
      iroll = 0;
    }

    if(iroll>=numrolling) 
      err("There are too many traces in groupkey value %g    Parameter maxfold=%d ",thisgroup,numrolling);

/* Compute the dip? */          

    if(indip<1) {  
      for (jloc=0; jloc<jroll; jloc++) {

        outputloc = rollloc[jloc];

        scanit (rolltrace,rollloc,jroll,outputloc,
                locminscan,locmaxscan,smin,sstep,jsmax,
                onedip,onepow,onesem,dipwrk,dipwrq,
                arng,ifori,afori,lendip,&numout);

/* Smooth the dip values in time (weighted by the average power along the     */
/* semblance-chozen dip). To call this smoothing is misleading since weights  */ 
/* actually cause dip values of strong horizons to expand up and down in time.*/
/* The expansion is the primary desire, not the smoothing.                    */

        wsmooth (onedip,onepow,lendip,minsd,maxsd,rolldip[jloc]);

        if(outdip>0 || outpow>0 || outsem>0) {
          memcpy(onetrace,rolltrace[jloc],sizeof(segy));
          if(outdip>0) {
            for (i=0; i<lendip; i++) onetrace->data[i] = rolldip[jloc][i] * dt; 
            fputtr(fpoutdip,onetrace); 
          }
          if(outpow>0) {
            for (i=0; i<lendip; i++) onetrace->data[i] = onepow[i]; 
            fputtr(fpoutpow,onetrace); 
          }
          if(outsem>0) {
            for (i=0; i<lendip; i++) onetrace->data[i] = onesem[i]; 
            fputtr(fpoutsem,onetrace); 
          }
        }

      } /* end of   for (jloc=0; jloc<jroll; jloc++) { */
    } /* end of   if(indip<1) { */

    for (jloc=0; jloc<jroll; jloc++) {

      outputloc = rollloc[jloc];

      jsum = 0;

      for (j=0; j<jroll; j++) {

        if(rollloc[j] >= outputloc+locminsum && rollloc[j] <= outputloc+locmaxsum) { 

          if(dipuse > 0) { /* use the dip at each contributing trace ? */ 
            for (i=0; i<lendip; i++) onetim[i] = (float) i + rolldip[j][i] * (rollloc[j] - outputloc);
          } 
          else { /* use the dip at the output location ? */
            for (i=0; i<lendip; i++) onetim[i] = (float) i + rolldip[jloc][i] * (rollloc[j] - outputloc);
          }

/* Shift sample values by the time computed from dips and location difference.*/

          ints8r(lendip, 1.0, 0.0, rolltrace[j]->data, 0.0, 0.0, lendip, onetim, jsumdat[jsum]);

          jsum++;

        } /* end of  if(rollloc[j] >= outputloc+locminsum && .... */

      } /* end of  for (j=0; j<jroll; j++) { */
      
      memcpy(onetrace,rolltrace[jloc],sizeof(segy));          

      for (i=0; i<lendip; i++) {
        onetrace->data[i] = 0.;
        for (j=0; j<jsum; j++) onetrace->data[i] += jsumdat[j][i];
        onetrace->data[i] /= jsum;
      }

      puttr(onetrace); 
      mproct++;

    } /* end of   for(jloc...  */

/* Store input traces and values of next location. Remember the trace already*/
/* read-into tr is the first trace to store for THIS input location.         */

    rollloc[iroll] = fromhead(tr,lockase);
    memcpy(rolltrace[iroll],&tr,sizeof(segy));

    if(indip>0 && icycle>0) { 
      if(!fgettr(fpindip,&trd)) 
        err("next input seismic trace needs a trace from indip file: %s   but get failed (near location=%d)",
          dname,rollloc[iroll]); 
      if((double)rollloc[iroll] != fromhead(trd,lockase)) 
        err("lockey value for dipfile trace %g   and input seismic trace %d are not the same ",
          fromhead(trd,lockase),rollloc[iroll]); 
      for (i=0; i<lendip; i++) rolldip[iroll][i] = trd.data[i] / dt;
    }

    iroll++;

  } /* end of  while(icycle==1) {  */

  if(indip>0) efclose(fpindip);
  if(outdip>0) efclose(fpoutdip);
  if(outsem>0) efclose(fpoutsem);
  if(outpow>0) efclose(fpoutpow);
 
  warn("Number of traces input=%d.  Number output=%d.",nproct,mproct);

  return 0;

}

/* Semblance Scanner                                                          */
/*                                                                            */
/* Semblance herein is computed as:                                           */
/*                                                                            */
/*  (Square of (Sum of amplitudes in locminscan to locmaxscan range)) /       */
/*             (Sum of squared amplitudes in that range))                     */
/*             divided by the number in that range.                           */
/*                                                                            */
/* Scans from smin to smin*sstep*(jsmax-1) are done for each sample time and  */
/* the maximum semblance value computed as above is found. That maximum is    */
/* then 3-point interpolated using the power at that maximum and the power    */
/* at its 2 neighbours (i.e. the values of their denominators above).         */
/* Note that this 3 point interpolation improves the result                   */
/* and also has the benefit of reducing the sstep granularity.                */
/*   Note this semblance is computed over just 1 time sample, but see dipext. */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* atrace     = The input traces.                                             */
/* aloc       = Location value for each input trace.                          */
/* numrolling = Number of traces in atrace and locations in aloc.             */
/* scanloc    = The location value where the scan takes place.                */
/* locminscan = The minimum relative location to include in scan (inclusive). */
/* locmaxscan = The maximum relative location to include in scan (inclusive). */
/* smin       = Minimum relative sample to scan.                              */
/* sstep      = Fractional sample interval to scan.                           */
/* jsmax      = Number of intervals to scan (from smin, increment by sstep).  */
/*                                                                            */
/* adip       = Output apparant dip (in samples per 1 location difference).   */
/* apow       = Output power along the chosen dip.                            */
/* asem       = Output semblance of the chosen dip.                           */
/* awrk       = Work buffer.                                                  */
/* awrq       = Work buffer.                                                  */
/* arng       = Work buffer (size: numrolling).                               */
/* ifori      = Work buffer (size: numrolling by jsmax)                       */
/* afori      = Work buffer (size: numrolling by jsmax)                       */
/*                                                                            */
/* lendip     = Length of traces and adip,apow,asem,awrk,awrq.                */
/* numout     = Number of traces in scanloc+locminscan to scanloc+locminscan. */
/*              0 returns all adip=0 and asem=0 and apow=0                    */
/*              1 returns all adip=0 and asem=1 and apow=trace amp squared    */


void scanit (segy **atrace, int *aloc, int numrolling, double scanloc, 
            double locminscan, double locmaxscan, float smin, float sstep, int jsmax,
            float *adip, float *apow, float *asem, float *awrk, float *awrq, 
            int *arng, int **ifori, float **afori, int lendip, int *numout) {

  int i=0;
  int j=0;
  int m=0;
  int n=0;
  float alin=0.;
  float aval=0.;
  float sval=0.;
  int numinrange=0;

  float thesem=0.;
  int thej=0; 
  float sl=0.;
  float sr=0.;
  float sp=0.;

  for (n=0; n<lendip; n++) {
    adip[n] = 0.;
    apow[n] = 0.;
    asem[n] = 0.;
  }

  for (i=0; i<numrolling; i++) {
    if(aloc[i] >= scanloc+locminscan && aloc[i] <= scanloc+locmaxscan) {
      arng[numinrange] = i;
      numinrange++;
    }
  }

  *numout = 0;
  if(numinrange<1) return; 
  *numout = 1;

/* If only 1 trace in range, then dip is presumed 0 (as initialized above)    */
/* And set power and semblance values correctly.                              */

  if(numinrange<2) {
    for (n=0; n<lendip; n++) {
      apow[n] = atrace[arng[0]]->data[n] * atrace[arng[0]]->data[n];
      asem[n] = 1.;
    }
    return; 
  }

/* Precompute the whole and fractional samples for linear interpolation so    */
/* it does not get repeated for every trace sample.                           */
/* (It is possible the compiler is smart enough to do this, but ????)         */
/* Note to future programmers: you could also use ints8r (or similar)         */
/*      to get more accurate seismic values than linear interpolation.        */
/*      But that would require passing a buffer to hold the shifted traces.   */

  for(j=0; j<jsmax; j++) {
    sval = smin + j*sstep;
    for (i=0; i<numinrange; i++) {
      m = ((int) (sval*(aloc[arng[i]]-scanloc)));
      if(sval*(aloc[arng[i]]-scanloc) > 0.) m++;
      alin = m - (sval*(aloc[arng[i]]-scanloc));
      ifori[i][j] = m;
      afori[i][j] = alin;
    }
  }

  for (n=0; n<lendip; n++) {                 
    for(j=0; j<jsmax; j++) {
      awrk[j] = 0.;                                                                
      awrq[j] = 1.e-30;  /* prevents divide by 0 when all samples = 0.0       */
      for (i=0; i<numinrange; i++) {
        m = ifori[i][j];
        if(n+m > 0 && n+m < lendip) { /* totals near top,bottom != numinrange */
          alin = afori[i][j];        
          aval = alin * atrace[arng[i]]->data[n+m-1] + (1.-alin) * atrace[arng[i]]->data[n+m];
          awrk[j] += aval;
          awrq[j] += aval*aval;
        }
      } 
    } /* end of  for(j=0; j<jsmax; j++) { */ 

/* Keep biggest semblance value and which scan number this is (thej).         */

    thesem = 0.;
    thej = jsmax/2; /* in case all zeroes (like a mute zone) */
    for(j=0; j<jsmax; j++) {
      if(awrk[j]*awrk[j]/awrq[j] > thesem) {
        thesem  = awrk[j]*awrk[j]/awrq[j];
        thej    = j;
      }
    } /* end of  for(j=0; j<jsmax; j++) { */

 /* At this point in the code, the dip values have sstep granularity.         */
 /* So perform 3 point interpolation around the maximum semblance value.      */
 /* This is done because switching from one sstep value to another might      */
 /* cause some effects if a reflection has slowly changing actual dip.        */
 /* Also, it allows users easier testing of what sstep is good enough (if the */
 /* 3 point interpolated dip of a big sstep gives as good results as the      */
 /* 3 point interpolated dip of a small sstep, use big and save CPU time).    */
           
    if(thesem < 1.e-30) {   /* if all zeroes (mute zones).                    */
      adip[n] = 0.0;  
      asem[n] = 0.0;  
      apow[n] = 0.0;
    }
    else if(thej>0 && thej<jsmax-1) {
      sl = awrk[thej-1]*awrk[thej-1]/awrq[thej-1];
      sr = awrk[thej+1]*awrk[thej+1]/awrq[thej+1];
      sp = 0.5 * (sl-sr) / (sl - 2.0*thesem + sr + 1.e-30);
      adip[n] = smin + thej*sstep + sp*sstep;     /* dip at interpolated peak */
      asem[n] = (thesem - 0.25 * (sl-sr) * sp)/numinrange; /* semblnc of peak */
      apow[n] = awrq[thej]/numinrange; /* power along dip of max semblnc pick */
    }
    else {
      adip[n] = smin + thej*sstep;
      asem[n] = thesem/numinrange;
      apow[n] = awrq[thej]/numinrange;
    }

  } /* end of  for (n=0; n<lendip; n++) { */ 

  return;
}

/* Weighted smoothing                                                         */
/*                                                                            */
/* val = input values                                                         */
/* wgt = input weights                                                        */
/* len = length of val,wgt,sval.                                              */
/* minsd = lower range to include in sum for i-th value (usually = -maxsd)    */
/* maxsd = higher range to include in sum for i-th value (usually positive)   */
/*                                                                            */
/* sval = output smoothed values                                              */
/*                                                                            */
/* Notes:                                                                     */
/* 1. Weights must be >= 0 or you could get divide by 0. (not checked herein) */
/* 2. Could do this by add and subtract to running totals as i increments,    */
/*    which would be faster for big minsd,maxsd. But do not expect that here. */
/*    (And the compiler-optimizer will unroll the inner loop).                */

void wsmooth (float *val, float *wgt, int len, int minsd, int maxsd, float *sval) {

  int i=0;
  int k=0;
  float twgt=0.;
  float tval=0.;

  for(i=0-minsd; i<len-maxsd; i++) { 
    twgt = 1.e-30;
    tval = 0.;
    for(k=minsd; k<=maxsd; k++) {
      twgt += wgt[i+k];
      tval += wgt[i+k] * val[i+k];
    }
    sval[i] = tval / twgt;
  }

  for(i=0; i<0-minsd; i++) sval[i] = sval[0-minsd]; 
  for(i=len-maxsd; i<len; i++) sval[i] = sval[len-maxsd-1];

  return;
}
