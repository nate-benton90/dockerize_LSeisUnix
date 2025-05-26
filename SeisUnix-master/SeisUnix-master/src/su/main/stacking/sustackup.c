/* Copyright (c) Colorado School of Mines, 2023.*/
/* All rights reserved.			*/

/* SUSTACKUP: $Revision: 1.0 $ ; $Date: 2023/04/01 11:00:01 $	*/

#include "su.h"
#include "segy.h"
#include "header.h"
#include "headcase.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                       ",
" SUSTACKUP - Input Any Trace Order and Stack To Any Key Combination.   ",
"                                                                       ",
" sustackup <stdin >stdout keyloc=cdp                                   ",
"                                                                       ",
" By default this program inputs any trace order and makes a cdp stack  ",
" using all input traces. But this program can also make other stacks.  ",
" Stacks can be made for any user-specified combination of key values.  ",
" All input traces with the same set of key values are stacked together.",
" In particular, 2D and 3D shot or receiver stacks can be made.         ",
" Additionally, two range limits can be specified to exclude traces     ",
" from stacking. For instance, the offset range can be limited. Range   ",
" limiting can also be done by suwind, of course. But an important      ",
" difference here is that, by default, this program outputs a zero      ",
" amplitude trace with full header values even when the range limits    ",
" exclude all traces for that shot or receiver.                         ",
"                                                                       ",
" keyloc=cdp List of keys that define output stack trace locations.     ",
"          A different stacked trace is output for each unique          ",
"          combination of these key values. The output traces have the  ",
"          header values of the first trace with lowest absolute offset ",
"          encountered for each keyloc unique value combination.        ",
"          Except the nhs key is reset to the number of traces within   ",
"          the range limits (i.e. the stack fold).                      ",
"    Note: Output header is copied from lowest absolute offset trace    ",
"          even if it is not within the range limits (specified below). ",
"      *** Offset is always absolute valued on output (non-negative).   ",
" 									",
" 									",
" keyabs=  Key name for absolute value range limit. If specified then   ",
"          traces are included in stack if within the range limit(s).   ",
"          Often, the offset key name is used here.                     ",
" 									",
" keyabs2= If specified, this is subtracted from keyabs and the result  ",
"          absolute valued before applying minabs,maxabs range.         ",
" 									",
" minabs=  Default is no minimum limit. See range specification note.   ",
" maxabs=  Default is no maximum limit. See range specification note.   ",
" 									",
"  Note RANGE SPECIFICATION:                                            ",
"          If minabs is less than maxabs then the range to be included  ",
"          in stack is GREATER OR EQUAL to minabs and LESS than maxabs. ",
"     ***  But if minabs is greater or equal to maxabs then the range   ",
"          is GREATER OR EQUAL to minabs -or- LESS than maxabs.         ",
"          This backwards specification is intended for some situations ",
"          where the key contains an azimuth or an angle. For instance, ",
"          if minabs=340 and maxabs=20 then traces >=340 or <20 degrees ",
"          are in stack range.                                          ",
" 									",
" 									",
" keysign= Key name for signed value range limit. If specified then     ",
"          traces are included in stack if in the minsign,maxsign range.",
"    Note: If you specify both keyabs and keysign, traces must be in    ",
"          both ranges to be included in stack.                         ",
" 									",
" keysign2= If specified, this is subtracted from keysign before        ",
"           applying the minsign,maxsign range.                         ",
" 									",
" minsign= Default is no minimum limit. See range specification note.   ",
" maxsign= Default is no maximum limit. See range specification note.   ",
" 									",
"  Note RANGE SPECIFICATION:                                            ",
"          If minsign is less than maxsign then range to be included in ",
"          stack is GREATER OR EQUAL to minsign and LESS than maxsign.  ",
"     ***  But if minsign is greater or equal to maxsign then the range ",
"          is GREATER OR EQUAL to minsign -or- LESS than maxsign.       ",
"          This backwards specification is intended for some situations ",
"          where the key contains an azimuth or angle. For instance, if ",
"          minsign=340 and maxsign=20 then traces >=340 or <20 degrees  ",
"          are in stack range.                                          ",
" 									",
" 									",
" keep=1   Make stacked output traces for all unique keyloc combinations",
"          even if the range limits eliminates all input traces. These  ",
"          stacked output traces have the same header values as output  ",
"          for other traces (but key nhs=0 and all amplitudes are 0.)   ",
"     =0   Only output stacked traces if at least one trace is in range.",
"    Note: keep=1 makes it easier to compare the result of different    ",
"          ranges since the same amount of stacked traces are output.   ",
"                                                                       ",
" maxstack=100000 Maximum Amount of Stacked Output Traces. This is also ",
"                 the maximum unique combinations of keyloc values.     ",
"           Note: Initial memory obtained is about 24*maxstack bytes.   ",
"                 Then main memory for stacked traces is only obtained  ",
"                 as each unique keyloc combination is encountered      ",
"                 (about 12*trace_samples bytes per unique combination).",
"                                                                       ",
"                                                                       ",
"  Note On Output Headers:                                              ",
"          Key nhs is set to the number of traces within the ranges of  ",
"          the unique combination of keyloc values that identify the    ",
"          stacked output trace (i.e. the stack fold).                  ",
"          All other keys are set to the values of the first trace      ",
"          encountered with the smallest absolute offset for the        ",
"          unique combination of keyloc values that identify the        ",
"          traces to stack together.                                    ",
"          The reason to output the header from smallest offset trace   ",
"          is it means shot-stacks and receiver-stacks have cdp numbers ",
"          that correspond approximately to their cdp-stack locations.  ",
"          It also means cdp-stacks from this program have shot and     ",
"          receiver coordinates and elevations (etc) of near locations. ",
"                                                                       ",
NULL};

/* Author:
 *	Andre Latour. March 2023
 *	1. This program sums amplitudes into double precision buffers,
 *	   then divides them by the non-zero fold of each sample, and    
 *	   copies them to output tr.data (which is single precison).
 *	   Since double precision has about 16 significant digits,    
 *	   it does not matter very much what trace order is input.        
 *	   (Whereas stacking different trace order into float            
 *	    buffers does produce noticable differences).                                       
 */
/**************** end self doc ***********************************/

segy tr;



int num_keyloc;           /* needed within comparedd                          */

int bhighdd(double** all, int last, double* guy); 
int comparedd (const void * q1, const void * q2) ;

int main(int argc, char **argv) {

  cwp_String *keyloc = NULL;
  int *kaseloc = NULL;

  cwp_String keyabs = NULL;
  cwp_String keyabs2 = NULL;
  double minabs=-1.e31;
  double maxabs=1.e31;
  int ikaseabs = 0;
  int kaseabs = 0;
  int ikaseabs2 = 0;
  int kaseabs2 = 0;

  cwp_String keysign = NULL;
  cwp_String keysign2 = NULL;
  double minsign=-1.e31;
  double maxsign=1.e31;
  int ikasesign = 0;
  int kasesign = 0;
  int ikasesign2 = 0;
  int kasesign2 = 0;

  int maxstack=100000;

  int keep=1;
  int ilive=1;
  double dval=0.;

  int lensam=0;
  double *guy;             /* To contain one set of location values.            */

  double **alllocs=NULL;   /* For all sets of locations for all stacked traces. */
  segy **sumtrace=NULL;    /* For all stacked traces.                           */
  double **sumamp=NULL;    /* For all stacked traces (double prec amplitudes).  */

  int i=0;
  int j=0;
  int nproct=0;

  int ihere=0;
  int lastloc=0;

/* hook up getpar */

  initargs(argc, argv);
  requestdoc(1);

/* Get key case numbers from GetCase.                                        */

  if(countparval("keyloc")>0) {
    num_keyloc = countparval("keyloc");
    keyloc = ealloc1(num_keyloc,sizeof(cwp_String *)); 
    getparstringarray("keyloc", keyloc);
  }    
  else {
    num_keyloc = 1; 
    keyloc = ealloc1(num_keyloc,sizeof(cwp_String *)); 
    keyloc[0] = ealloc1(3,1);
    strcpy(keyloc[0],"cdp");
  }    
  kaseloc = ealloc1int(num_keyloc); 
  for (i=0; i<num_keyloc; ++i) {
    kaseloc[i] = 0;
    kaseloc[i] = GetCase(keyloc[i]);
    if(kaseloc[i]<1) err("**** Error: Specified keyloc name %s is not recognized.",keyloc[i]);
  }

  if(countparval("keyabs")>0) {
    ikaseabs = 1;
    getparstring("keyabs", &keyabs);
    kaseabs = GetCase(keyabs);
    if(kaseabs<1) err("**** Error: Specified keyabs name %s is not recognized.",keyabs);
  }
  else {
    if(countparval("keyabs2")>0) err("**** Error: specifying keyabs2 requires specifying keyabs.");
    if(countparval("minabs")>0)  err("**** Error: specifying minabs requires specifying keyabs.");
    if(countparval("maxabs")>0)  err("**** Error: specifying maxabs requires specifying keyabs.");
  }

  if(countparval("keyabs2")>0) {
    ikaseabs2 = 1;
    getparstring("keyabs2", &keyabs2);
    kaseabs2 = GetCase(keyabs2);
    if(kaseabs2<1) err("**** Error: Specified keyabs2 name %s is not recognized.",keyabs2);
  }

/* No error check here since maxabs is allowed to be smaller than minabs      */
  if(countparval("minabs")>0) getpardouble("minabs",&minabs);
  if(countparval("maxabs")>0) getpardouble("maxabs",&maxabs);

  if(countparval("keysign")>0) {
    ikasesign = 1;
    getparstring("keysign", &keysign);
    kasesign = GetCase(keysign);
    if(kasesign<1) err("**** Error: Specified keysign name %s is not recognized.",keysign);
  }
  else {
    if(countparval("keysign2")>0) err("**** Error: specifying keysign2 requires specifying keyabs.");
    if(countparval("minsign")>0)  err("**** Error: specifying minsign requires specifying keyabs.");
    if(countparval("maxsign")>0)  err("**** Error: specifying maxsign requires specifying keyabs.");
  }

  if(countparval("keysign2")>0) {
    ikasesign2 = 1;
    getparstring("keysign2", &keysign2);
    kasesign2 = GetCase(keysign2);
    if(kasesign2<1) err("**** Error: Specified keysign2 name %s is not recognized.",keysign2);
  }

/* No error check here since maxsign is allowed to be smaller than minsign    */
  if(countparval("minsign")>0) getpardouble("minsign",&minsign); 
  if(countparval("maxsign")>0) getpardouble("maxsign",&maxsign);


  if(countparval("keep")>0) getparint("keep",&keep);
  if(keep<0 || keep>1) err("**** Error: keep parameter is out-of-range.");

  if(countparval("maxstack")>0) getparint("maxstack",&maxstack);
  if(maxstack<1) err("**** Error: maxstack must be greater than 0");

/* Allocate the arrays to hold pointers for stacked output traces. But only   */
/* allocate the needed memory when each unique location is encountered later. */

  alllocs = (double **)ealloc1(sizeof(double*),maxstack);
  sumamp   = (double **)ealloc1(sizeof(double*),maxstack);
  sumtrace = (segy **)ealloc1(sizeof(segy*),maxstack);

  guy = ealloc1double(num_keyloc);

  checkpars();

/* Loop over the input traces.                                               */

  while(gettr(&tr)) {
 
    if(nproct<1) {
      lensam = tr.ns;
      lastloc = 0;
    }
    nproct++;

    ilive = 1;

    if(ikaseabs>0) {
      dval = fromhead(tr, kaseabs);
      if(ikaseabs2>0) dval = dval - fromhead(tr, kaseabs2);
      dval = fabs(dval);
      if(minabs<maxabs) {
        if(dval<minabs || dval>=maxabs) ilive = 0;
      }
      else {
        if(dval<minabs && dval>=maxabs) ilive = 0;
      }
    }

    if(ikasesign>0) {
      dval = fromhead(tr, kasesign);
      if(ikasesign2>0) dval = dval - fromhead(tr, kasesign2);
      if(minsign<maxsign) {
        if(dval<minsign || dval>=maxsign) ilive = 0;
      }
      else {
        if(dval<minsign && dval>=maxsign) ilive = 0;
      }
    }

    if(ilive<1 && keep<1) continue; /* Not outputting stack traces of 0 fold? */

    for (i=0; i<num_keyloc; ++i) guy[i] = fromhead(tr, kaseloc[i]);

    ihere = bhighdd(alllocs, lastloc, guy);

/* Next line is part of use-input-header-of-least-offset. Note this is after  */
/* setting guy just in case someone uses offset (signed) in keyloc list.      */

    tr.offset = abs(tr.offset); 

/* Is this the same location as already encountered ?                         */
/* (If ihere=0 then it is lower than any already encountered).                */

    if(ihere>0 && comparedd(guy,alllocs[ihere-1]) == 0) { 

/* For output, copy input header that has the smallest offset (except fold).  */

      if(tr.offset<sumtrace[ihere-1]->offset) { 
        j = sumtrace[ihere-1]->nhs;
        memcpy(sumtrace[ihere-1], &tr, HDRBYTES);
        sumtrace[ihere-1]->nhs = j;
      }

      if(ilive>0) { 
        sumtrace[ihere-1]->nhs = sumtrace[ihere-1]->nhs + 1;
        for (i=0; i<lensam; ++i) {
          if(tr.data[i] != 0.0) {
            sumamp[ihere-1][i] += tr.data[i];  /* sum amplitudes in double buf*/ 
            sumtrace[ihere-1]->data[i] += 1.0; /* yes, stores the SAMPLE fold */
          }
        }
      } /* end of if(ilive>0) { */

    }
    else { /* This is a new combination of keyloc values                      */

      if(lastloc>=maxstack) 
        err("error: At input trace=%d  number of unique keyloc combinations is greater than maxstack=%d",nproct,maxstack);

/* Pull the pointers down to accomodate new location. And get new memory.     */

      for (i=lastloc; i>=ihere; --i) {
        sumamp[i+1] = sumamp[i];
        sumtrace[i+1] = sumtrace[i];
        alllocs[i+1] = alllocs[i];
      }
      alllocs[ihere]  = ealloc1double(num_keyloc);
      sumamp[ihere]   = ealloc1double(lensam);
      sumtrace[ihere] = (segy *)ealloc1(sizeof(segy),sizeof(char));

/* Copy the values for the new location into new memory.                      */

      for (i=0; i<num_keyloc; ++i) alllocs[ihere][i] = guy[i];

      memcpy(sumtrace[ihere],&tr,sizeof(segy)); /* copy header and samples    */
      for (i=0; i<lensam; ++i) {
        if(ilive<1 || tr.data[i] == 0.0) {
          sumamp[ihere][i] = 0.0;   
          sumtrace[ihere]->data[i] = 0.0;  
        }
        else {
          sumamp[ihere][i] = tr.data[i];   /* copy amplitude to double buffer */
          sumtrace[ihere]->data[i] = 1.0;  /* use trace buffer as sample fold */ 
        }
      }
      sumtrace[ihere]->nhs = ilive; /* Set initial fold value                 */

      lastloc++;

    }
  } /* end of  while(gettr(&tr)) {  */

  for (j=0; j<lastloc; ++j) {
    for (i=0; i<lensam; ++i) {
      if(sumtrace[j]->data[i]>0.0) { /* remember, here ->data is sample fold  */
        sumtrace[j]->data[i] = sumamp[j][i] / sumtrace[j]->data[i];
      }
    }
    puttr(sumtrace[j]);
  }

  warn("Number of traces input=%d  Number stacked output=%d ",nproct,lastloc);

  return(CWP_Exit());
}

/* -------------------------------------------------------------------------- */
/* Specify compare function for bhighdd function (also used above).           */

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

