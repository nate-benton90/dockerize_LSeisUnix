/* Copyright (c) Colorado School of Mines, 2022.*/
/* All rights reserved.                       */

/* SUPROFCSV: $Revision: 1.02 $ ; $Date: 2023/11/14 00:00:01 $		*/
 
#include "su.h"
#include "segy.h" 
#include "qdefine.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"									     ",
" SUPROFCSV - Input Q-file or parameters, make profile, output other Q-file. ",
"									     ",
"  suprofcsv [parameters].   (No traces in or out).                          ",
"									     ",
" Note: You should start by looking at the 2d cdp numbering computation that ",
"       exists in the subincsv program. For 2d lines with consistent shot    ",
"       and receiver coordinates and bends of less than 10 degrees or so,    ",
"       subincsv can usually give nicer results for people who are not very  ",
"       familiar with crooked profiles (and even for people who are).        ",
"									     ",
" Note: All defaults here are roughtly set for a simple situation consisting ",
"       of an input profile of receiver XY locations with a 25 metre spacing.",
"       Those receivers might have come from an SPS2 R-file converted to     ",
"       a q-file using program sutoolcsv.                                    ",
"									     ",
" Parameters:	         						     ",
"                                                                            ",
" qin=       Input file containing q-records. This parameter is optional,    ",
"            but if you do not specify it, you must specify a set of         ",
"            parameters which you can see lower down in this document.       ",
"            Those parameters allow specification similar to sunmo.          ",
"      Note: The keyp,keyx,keyy parameters recognize root key names even if  ",
"            they are surrounded by underscores and digits in the qin file.  ",
"									     ",
" keyp=gaps Point Key Name. The default (gaps) is the receiver point name    ",
"           used by sugeomcsv and sutoolcsv.                                 ",
"           Name must be in the input Q-file (or parameters listed below).   ",
"           This is used to order (sort) the input locations based on point  ",
"           numbers, station numbers, receiver numbers, or shot numbers.     ",
"     =asis Means use the points in the order they are input.                ",
"									     ",
" keyx=gx   X coordinate name. The default (gx) is the receiver x name.      ",
"           Name must be in the input Q-file (or parameters listed below).   ",
"									     ",
" keyy=gy   Y coordinate name. The default (gy) is the receiver y name.      ",
"           Name must be in the input Q-file (or parameters listed below).   ",
"     =none Means use only keyx as coordinate of points. Usually this means  ",
"           that keyx is a value such as a station number, or a cdp number,  ",
"           or the accumulated distance between points.                      ",
"									     ",
" chordi=12.5   Initial Chord Distance. Start at lowest point and linearly   ",
"           interplate new points at increments of this distance using the   ",
"           surrounding two input points. Original points are not preserved. ",
"           For typical situations, this should be your desired cpd spacing. ",
"           Note: This chording occurs before profile averaging or smoothing.",
"       =0.0 is a flag that means to not perform initial chording. So the    ",
"            input is the initial profile. Generally this means the input    ",
"            points should already have a constant interval and profile ends ",
"            should have more than max(nmaxa,nmaxs) spare points.            ",
"									     ",
"  nmina=0  Minimum Averaging Number. Must be less or equal to nmaxa.        ",
"									     ",
"  nmaxa=10   Maximum Averaging Number.                                      ",
"           A value of 0 means no averaging is done.                         ",
"           Each location in the current profile is averaged symmetrically   ",
"           with its neighbours (from nmina to nmaxa on each side).          ",
"           For an input profile with a simple bend, this results in a       ",
"           new profile towards the inside of the bend. The smaller this     ",
"           value, the nearer the resulting profile follows input profile.   ",
"           For typical surveys with shots located approximately along the   ",
"           input (receiver) profile, the maximum reasonable value here is   ",
"           nominal maximum source-receiver offset dist divided by chordi.   ",
"									     ",
" dextra=nmaxa*chordi  Extra distance to extrapolate before Initial Chording.",
"           This linear extrapolation uses the first 2 input points and the  ",
"           last 2 input points. Cannot be specified if chordi=0.0           ",
"									     ",
"  nmins=0  Minimum Centred Smoothing Number. Must be less or equal to nmaxs.",
"           Note: Usually should be 0 even if nmina is non-zero.             ",
"									     ",
"  nmaxs=nmaxa   Maximum Centred Smoothing Number.                           ",
"           A value of 0 means no smoothing is done.                         ",
"           Each location in the current profile is smoothed symmetrically   ",
"           with its neighbours (from nmins to nmaxs on each side).          ",
"           This smoothing uses a technique which leaves the profile         ",
"           approximately centred along the same positions.                  ",
"									     ",
" chordf=chordi   Final Chord Distance. Start at lowest point and linearly   ",
"           interpolate new points at increments of this distance using the  ",
"           surrounding two points. For typical situations, this should be   ",
"           your desired cpd spacing.                                        ",
"       =0.0 is a flag that means to not perform final chording.             ",
"           Note: This chording is done after initial chording, averaging,   ",
"                 and smoothing.                                             ",
"									     ",
" ename=     Names to eliminate. These names will be ignored in the input    ",
"            q-records and therefore not be in the output q-records.         ",
"            Recommend not eliminating any (unless filesize is an issue).    ",
"      Note: Must be the full names, including any underscores and digits.   ",
"									     ",
" formxy=%.20g  The C format code for printing all values to the q-records.  ",
"              Note that the default format prints up to 20 digits           ",
"              (but not trailing zeroes to the right of the decimal point).  ",
"									     ",
"									     ",
"									     ",
"            The following two parameters default to doing nothing. To use   ",
"            them, view/plot the cdp locations after this program and compare",
"            them to trace midpoint locations. Sometimes it is beneficial to ",
"            align the cdp locations to the centre of clumps of midpoints    ",
"            (or align them to midpoints with small source-receiver offsets).",
"              For more information, consult src/demos/Geom3D/Suprofcsv      ",
"                                    or my YouTube video about profiles      ",
" shiftrec=  List of Q-record numbers to apply small shifts along profile.   ",
"            The first record starting with Q in the qin file is record 1.   ",
"            If the Q-file was previously output by this program,            ",
"            and Q-records were not deleted or rearranged, the cdp numbers   ",
"            in that Q-file match the record numbers to specify here.        ",
" shiftdist= List of distances to shift the Q-records in shiftrec. These     ",
"            shifts are along the profile. Positive shifts are in direction  ",
"            of increasing Q-record numbers. The amount of shift for the     ",
"            records between your specified points is partitioned according  ",
"            to the distances between records. Note that, if records between ",
"            your specified points have a constant interval on input, they   ",
"            will have a different but still constant interval on output.    ",
"            The shifts specified for the lowest and highest records are     ",
"            held constant outward to their ends of the profile.             ",
"									     ",
"									     ",
"  ***  Note that the following two parameters also exist in subincsv.       ",
"									     ",
" point_crz=1.0  cdp number of point zero.                                   ",
"                Note: NOT cdp of first output point unless point_cru=0.0    ",
"									     ",
" point_cru=0.0  cdps per one point unit.     Must be 0.0 if keyp=asis.      ",
"            The first output cdp is set to point_crz plus first output      ",
"            value of your specified keyp name multiplied by point_cru.      ",
"              (first cdp) = (first keyp) * point_cru + point_crz            ",
"       ***  Subsequent output cdps just increment by 1.                     ",
"            Subsequent cdps are NOT computed using output keyp values and   ",
"            will deviate from cdp numbers computed from output keyp values  ",
"            unless you use the setup described next.                        ",
"            If you have specified these parameters in subincsv then you     ",
"            can output similar cdp numbers here - using some specific setup.",
"             - keyp here should be rpkey name in subincsv (typically gaps). ",
"             - keyx here should be the same as keyp (typically gaps).       ",
"             - keyy here should be none.                                    ",
"             - chordi here should be 1/point_cru (typically 0.5)            ",
"            You can still average and smooth other values using this setup. ",
"            (But for this setup the keyp values are linear and so they will ",
"            be the same values because averaging/smoothing are symmetrical).",
"									     ",
"									     ",
" qout=      Output file for q-records. Must be specified.                   ",
"									     ",
"      Note: A sequential cdp number is always generated in the qout file.   ",
"      Note: The name cdp can be used from the input qin file for parameters ",
"            keyp or keyx or keyy, but its name will be null in qout file    ",
"            because another cdp number is created by this program.          ",
"									     ",
"									     ",
"   ------------------------------------------------------------------       ",
"   ------------------------------------------------------------------       ",
"   The following parameters can only be specified if you                    ",
"   do not specify an input q-file using parameter qin=.                     ",
" *** For consistency, these are the same parameters as for subinqcsv. ***   ",
"									     ",
" cdp=             The parameters to the left are all optional but at least  ",
" cdpt=            one of them must be specified. These are the names which  ",
" fldr=            contain single values at each location (also known as     ",
" grnors=    	   the non-tuple names). Typically, gaps is the most likely  ",
" grnofr=    	   name to specify. Each name specified here must have the   ",
" grnlof=    	   same amount of values (separated by commas).              ",
" gaps=      	   For example: gaps=2,7,23,44 means that you must also      ",
" igi=       	   list 4 values if you specify numa=.                       ",
" igc=       	                                                             ",
" gx=                                                                        ",
" gy=                                                                        ",
" sx=                                                                        ",
" sy=                                                                        ",
" numa=                                                                      ",
" numb=                                                                      ",
" vuma=                                                                      ",
" vumb=                                                                      ",
"									     ",
" tupa=            The parameters to the left are all optional. These are    ",
" offs=            the names which have multiple values at each location     ",
" tims=            (also known as the tuple names). Typically, tims= and     ",
" tnmo=            vnmo= are the most likely names to specify. Each name     ",
" dpth=      	   specified here must be repeated same number of times as   ",
" vels=            amount of values in non-tuple names above (except Note 1).",
" vnmo=                                                                      ",
" tupb=    Note 1: Usually, any of the previous parameter names must be      ",
" vupa=            repeated for each value in the non-tuple lists. But the   ",
"                  independent dimension name can also be specified just one ",
"                  time if all the other names here have the same amount of  ",
"                  values as that single set of independent dimension values.",
"          Note 2: The independent dimension defaults to first name that is  ",
"            	   found in order here (NOT their order on the command line).",
"									     ",
"   ------------------------------------------------------------------       ",
"									     ",
NULL};

/* Created: June 2022: Andre Latour                                          */ 
/* This program started from subinqcsv which also inputs and outputs q-files.*/ 
/* That program calls routines to perform bilinear-OR-linear interpolation,  */ 
/* which are some of the routines used herein, but restricted to linear only.*/ 
/* Modified: Nov  2023: Andre Latour                                         */ 
/*   Added point_crz and point_cru parameters and code. These allow changing */ 
/*   the first output cdp number (useful for some floating datum situations).*/ 
/**************** end self doc *******************************************/

segy tr;

struct QInfo *RecInfo; /* Storage for all function location value pointers */
int locp = -1;      

int compSort1 (const void * q1, const void * q2) ; /* comparison function for qsort  */

void runav (double **dall, int ncdp, int klast, int nmin, int nmax, int *ierr) ;

/*----------------------------------------------------------------------*/

int main(int argc, char **argv) {

  int ncdp = 0;		/* number of cdps specified */
  int jcdp = 0;         /* index into arrays dimensioned by ncdp */

  int ifixd = 0;          /* flag for all tuples same size or vary   */
  int iztuple = 0;        /* element number where first tuple exists */
  int ktuple = 0;         /* type of tuples (2=pairs, 3=triplets)    */

  cwp_String Pname=NULL;  /* text file name for Q input file      */
  FILE *fpP=NULL;         /* file pointer for Q input file        */
  cwp_String Qname=NULL;  /* text file name for output Q values   */
  FILE *fpQ=NULL;         /* file pointer for Q output file       */

  int knownnames = 26;         /* this set of variables is related to   */
  int ltuple = 17;             /* handling of parameter and value names */
  cwp_String *zname = NULL;                                         
  cwp_String *pname = NULL;                                         
  cwp_String *ndims = NULL;                                                 
  cwp_String *ename = NULL;                                              
  int *ihere = NULL;
  int numpname = 0;

  double *pindepa = NULL; /* input of independent dimension values */ 
  int numdind = 0;
  double *dpatha = NULL;
  double *dpaths = NULL;
  double pint = 0.;
  double pint2 = 0.;
  double **dalla = NULL;
  double **dalls = NULL;
  double *dinput = NULL;
	
  cwp_String formxyt=NULL;
  cwp_String formxy=NULL;
  cwp_String formxylong=NULL;
  int lenformxy = 0;

  int i = 0;              /* this set of variables are either trivial or so   */
  int j = 0;              /* entangled that no short comment up here will help*/
  int k = 0;  
  int errwarn = 0; 
  int klast = 0;
  double wi = 0.;
  double chordhere = 0.;
  double chordimax = 0.;
  double chordi = 0.;
  double chordfmax = 0.;
  double chordf = 0.;
  double dextra = 0.0;
  double pcrz = 1.0;
  double pcru = 0.0;
  int    jcrf = 0;

  int nshift = 0;
  int *shiftrec=NULL;
  double *shiftdist=NULL;

  int locx = -1;      
  int locy = -1;      
  int ierr = 0;
  int nmina = 0;
  int nmaxa = 0;
  int nmins = 0;
  int nmaxs = 0;
  int nhere = 1;
  int msize = 0;
  int mout = 0;
  cwp_String ukey = NULL;  

/* hook up getpar */
  initargs(argc, argv);
  requestdoc(1);

  if(isatty(STDIN_FILENO)!=1 || isatty(STDOUT_FILENO)!=1)  
    err("**** Error: this program does not input or output traces.");
          
/* ------------------------------------------------------------------- */

  cwp_String keyp = NULL;  
  if(countparval("keyp") > 0) {
    getparstring("keyp", &keyp);
  }
  else {
    keyp = ealloc1(4,1);
    strcpy(keyp,"gaps");
  }


  cwp_String keyx = NULL;  
  if(countparval("keyx") > 0) {
    getparstring("keyx", &keyx);
  }
  else {
    keyx = ealloc1(2,1);
    strcpy(keyx,"gx");
  }


  cwp_String keyy = NULL;  
  if(countparval("keyy") > 0) {
    getparstring("keyy", &keyy);
  }
  else {
    keyy = ealloc1(2,1);
    strcpy(keyy,"gy");
  }


  if(!getpardouble("chordi",&chordi)) chordi = 12.5;
  if(chordi < 0.0) err("**** Error: chordi= must be greater than or equal to 0.0 ");

  if(!getparint("nmina",&nmina)) nmina = 0;
  if(!getparint("nmaxa",&nmaxa)) nmaxa = 10;
  if(nmaxa<0) err("**** Error: nmaxa= must be greater or equal to 0 ");
  if(nmina<0 || nmina>nmaxa) err("**** Error: nmaxa= must be greater or equal to nmina= ");

  if(!getpardouble("dextra",&dextra)) dextra = nmaxa*chordi;
  if(chordi==0.0 && dextra!=0.0) err("**** Error: dextra= must be 0.0 if chordi=0.0 ");
  if(dextra<0.0) err("**** Error: dextra= cannot be negative. ");
  
  if(!getparint("nmins",&nmins)) nmins = 0;
  if(!getparint("nmaxs",&nmaxs)) nmaxs = nmaxa;
  if(nmaxs<0) err("**** Error: nmaxs= must be greater or equal to 0 ");
  if(nmins<0 || nmins>nmaxs) err("**** Error: nmaxs= must be greater or equal to nmins= ");

  if(!getpardouble("chordf",&chordf)) chordf = chordi;
  if(chordf < 0.0) err("**** Error: chordf= must be greater than or equal to 0.0 ");

  if(!getpardouble("point_crz",&pcrz)) pcrz = 1.0;
  if(!getpardouble("point_cru",&pcru)) pcru = 0.0;
  if(strcmp(keyp,"asis") == 0 && pcru!=0.0) err("**** Error: point_cru is not 0.0 but keyp=asis ");

  getparstring("qin", &Pname);

  getparstring("qout", &Qname);
  if(Pname != NULL && Qname != NULL && strcmp(Pname,Qname) == 0) 
    err("**** Error: qout= output Q-file name must be different than qin= input Q-file name.");
      
  fpQ = fopen(Qname, "w");
  if (fpQ == NULL) err("qfile error: output Q-file did not open correctly.");

/*-------------------------------------------------------------------------  */

  getparstring("formxy",&formxyt);
  if(formxyt==NULL) {
    lenformxy = 5;
    formxy = ealloc1(lenformxy,1);
    strcpy(formxy,"%.20g");
  }
  else {
    lenformxy = strlen(formxyt);
    formxy = ealloc1(lenformxy,1);
    strcpy(formxy,formxyt);
  }

  formxylong = ealloc1(1+lenformxy,1);
  strcpy(formxylong,",");
  strcpy(formxylong+1,formxy);

  
  nshift = countparval("shiftrec");
  if(nshift != countparval("shiftdist")) err("**** Error: shiftrec and shiftdist different lengths.");
  if(nshift>0) {
    shiftrec = ealloc1int(nshift+2);       /* extra 2 for endpoints */
    shiftdist = ealloc1double(nshift+2);   /* extra 2 for endpoints */
    getparint("shiftrec",shiftrec+1);      /* load to pointer + 1 */    
    getpardouble("shiftdist",shiftdist+1); /* load to pointer + 1 */
    nshift += 2;
    shiftdist[0] = 0.;                     /* initialize incase keyy=none */
    shiftdist[nshift-1] = 0.;              /* initialize incase keyy=none */
  }

/* ----------------------------------------------------------------------  */
/* At this point we have read-in most parameters.                          */
/* And we have read-in the 3d grid definition (if input).                  */
/* And we have set formats and various other things in preperation         */
/* for making the output q-file (as far as possible).                      */
/*     So, now start on actual read-in of values and interpolation.        */
/* ----------------------------------------------------------------------  */

  if(Pname == NULL) { /* no q-file, so readin via command line parameters */ 

    pname = ealloc1(knownnames,sizeof(cwp_String *)); 
    zname = ealloc1(knownnames,sizeof(cwp_String *));
    ihere = ealloc1int(knownnames);
    for(j=0; j<knownnames; j++) {
      pname[j] = ealloc1(6,1);
      zname[j] = ealloc1(6,1);
      strcpy(pname[j],"null");
      ihere[j] = 0;
    }
    strcpy(zname[0],"cdp");
    strcpy(zname[1],"cdpt");
    strcpy(zname[2],"fldr");
    strcpy(zname[3],"grnors");
    strcpy(zname[4],"grnofr");
    strcpy(zname[5],"grnlof");
    strcpy(zname[6],"gaps");
    strcpy(zname[7],"igi");
    strcpy(zname[8],"igc");
    strcpy(zname[9],"gx");
    strcpy(zname[10],"gy");
    strcpy(zname[11],"sx");
    strcpy(zname[12],"sy");
    strcpy(zname[13],"numa");
    strcpy(zname[14],"numb");
    strcpy(zname[15],"vuma");
    strcpy(zname[16],"vumb");
    ltuple = 17; /* first tuple name is at 17 */
    strcpy(zname[17],"tupa"); 
    strcpy(zname[18],"offs");
    strcpy(zname[19],"tims");
    strcpy(zname[20],"tnmo");
    strcpy(zname[21],"dpth");
    strcpy(zname[22],"vels");
    strcpy(zname[23],"vnmo");
    strcpy(zname[24],"tupb");
    strcpy(zname[25],"vupa");

    ktuple = 0;
    numpname = 0;

    for(j=0; j<knownnames; j++) {
      if (countparname(zname[j])>0) ihere[j] += 1;
    }

    for(j=0; j<ltuple; j++) { 
      if(ihere[j] > 0) {
        pname[numpname] = zname[j];
        numpname++;
      }
    }
    iztuple = numpname;

    for(j=ltuple; j<knownnames; j++) { 
      if(ihere[j] > 0) {
        pname[numpname] = zname[j];
        numpname++;
      }
    }

/* Note that this program (subinqcsv) has a complicated default for pname */
/* because it allows various parameter set names for input values.        */
/* But for other programs, just set pname[0] to the independent values    */
/* parameter set name and pname[1] to the dependant values name.          */
/* For instance, sunmocsv uses pname[0]="tims" pname[1]="vels" ktuple=2   */
/* and  program sumutecsv uses pname[0]="offs" pname[1]="tims" ktuple=2   */

   getviacommand(&pname, &numpname, &iztuple, numdind,
                 &ktuple, &ifixd, &RecInfo, &ncdp,
                 &pindepa, &ndims, &errwarn) ;

    if(errwarn==1) err("getviacommand error: no non-tuple name passed in.");
    else if(errwarn==2) err("getviacommand error: non-tuple names have different amounts of values.");
    else if(errwarn==3) err("getviacommand error: independent dimension parameter is empty.");
    else if(errwarn==4) err("getviacommand error: an independent dimension parameter is empty.");
    else if(errwarn==5) err("getviacommand error: members of tuple have different amounts at same location."); 
    else if(errwarn>0) err("getviacommand error: returned unrecognized error number = %d",errwarn);

  }

  else { /* Read-in via q-file? */ 

    fpP = fopen(Pname, "r");
    if(fpP==NULL) err("error: input Q-file did not open correctly.");
    
/* User wants to get rid of some names from the input q-file? Put them on the pname list.*/

    if(countparval("ename")>0) {
      numpname = countparval("ename");
      ename = ealloc1(numpname,sizeof(cwp_String *));
      getparstringarray("ename", ename);
      pname = ealloc1(7+numpname,sizeof(cwp_String *));
      for (i=0; i<numpname; ++i) pname[i] = ename[i];
    }
    else {
      numpname = 0;
      pname = ealloc1(7,sizeof(cwp_String *));
    }

/* 0 or negative numpname is a flag to NOT store values if they are on pname list. */
/* positive numpname is a flag to ONLY store values if they are on pname list.     */

    numpname = 0 - numpname; 

    getviaqfile(fpP, &pname, &numpname, &iztuple, numdind,   
                &ktuple, &ifixd, &RecInfo, &ncdp, 
                &pindepa,  &ndims, &errwarn) ;

    if(errwarn==1) err("getqinfo error: extra C_SU_NAMES record in q-file");
    else if(errwarn==2) err("getqinfo error: extra C_SU_NDIMS record in q-file");
    else if(errwarn==3) err("getqinfo error: C_SU_ID record not found immediately after C_SU_NAMES record.");
    else if(errwarn==11) 
      err("readqhead error: if C_SU_NDIMS not vary, its numbers must align with C_SU_NAMES");
    else if(errwarn==12) 
      err("readqhead error: C_SU_ID record not found immediately after C_SU_NAMES record.");
    else if(errwarn==22) err("getviaqfile error: C_SU_NDIMS record not same length as C_SU_NAMES record.");
    else if(errwarn==23) err("getviaqfile error: C_SU_NAMES tupled names out-of-order, changed");
    else if(errwarn==24) err("getviaqfile error: C_SU_NDIMS blank where valid number expected");
    else if(errwarn==25) err("getviaqfile error: C_SU_NDIMS non-number where valid number expected");
    else if(errwarn==26) err("getviaqfile error: C_SU_NDIMS value must be same for all members of tuple");
    else if(errwarn==27) err("getviaqfile error: C_SU_NAMES record followed by C_SU_ID record not found.");
    else if(errwarn>100) 
      err("getviaqfile error: record %d (wrong comma count, damaged, non-numbers, ...)",errwarn-99);
    else if(errwarn>0) err("getviaqfile error: unrecognized error code %d",errwarn);

  } /* end read-in via q-file */

  checkpars(); 

  if(ifixd==0) err("error: input with varying number of tuples is not allowed.");

/* For fixed q-files, there is same number of values in each record (klast).  */

  klast = iztuple + (ktuple-1)*RecInfo[0].nto;

/*--------------------------------------------------------------*/

  locx = -1;
  for (i=0; i<iztuple; ++i) {
    if(strcmp(pname[i],keyx)==0) locx = i;
  }
  if(locx<0) { /* name may be between underscores. See sutoolcsv. */
    ukey = ealloc1(1,strlen(keyx)+2);
    for (j=0; j<strlen(keyx); ++j) ukey[j+1] = keyx[j];
    ukey[0] = '_';
    ukey[strlen(keyx)+1] = '_';
    for (i=0; i<iztuple; ++i) {
      if(strstr(pname[i],ukey)!=0) locx = i;
    }
  }
  if(locx<0) err("error: input q-file must have your keyx=%s (among non-tuple names).",keyx);

  if(strcmp(keyy,"none") != 0) {
    locy = -1;
    for (i=0; i<iztuple; ++i) {
      if(strcmp(pname[i],keyy)==0) locy = i;
    }
    ukey = ealloc1(1,strlen(keyy)+2);
    for (j=0; j<strlen(keyy); ++j) ukey[j+1] = keyy[j];
    ukey[0] = '_';
    ukey[strlen(keyy)+1] = '_';
    for (i=0; i<iztuple; ++i) {
      if(strstr(pname[i],ukey)!=0) locy = i;
    }
    if(locy<0) err("error: input q-file must have your keyy=%s (among non-tuple names).",keyy);
  }

/* If only one coordinate, set locy to x to avoid changing subsequent code.*/

  else { 
    locy = locx;
    chordi *= sqrt(2.0); 
    chordf *= sqrt(2.0);
    dextra *= sqrt(2.0);
    for (i=0; i<nshift; ++i) shiftdist[i] *= sqrt(2.0);
  }

  if(strcmp(keyp,"asis") != 0) {
    locp = -1;
    for (i=0; i<iztuple; ++i) {
      if(strcmp(pname[i],keyp)==0) locp = i;
    }
    ukey = ealloc1(1,strlen(keyp)+2);
    for (j=0; j<strlen(keyp); ++j) ukey[j+1] = keyp[j];
    ukey[0] = '_';
    ukey[strlen(keyp)+1] = '_';
    for (i=0; i<iztuple; ++i) {
      if(strstr(pname[i],ukey)!=0) locp = i;
    }
    if(locp<0) err("error: input q-file must have your keyp=%s (among non-tuple names).",keyp);

    qsort(RecInfo,ncdp,sizeof(struct QInfo),compSort1);
    for (jcdp=1; jcdp<ncdp; jcdp++) {
      if(RecInfo[jcdp-1].dlots[locp] == RecInfo[jcdp].dlots[locp]) {
           err("error: Two sets of values input for location = %f",RecInfo[jcdp].dlots[locp]);
      }
    }
  }


/*--------------------------------------------------------------*/
/* perform initial chording?  */

  if(chordi>0.0) {

/* Notes:                                                                     */
/*  1) The distances from point to point are pre-computed into dpatha.        */
/*     These distances could be computed on-the-fly within the while loop.    */
/*     But that would make the while loop code harder to understand.          */
/*  2) I reuse some pointer names without bothering to free memory.           */

    dpatha = ealloc1double(ncdp);

    dpatha[0] = 0.;
    for (jcdp=1; jcdp<ncdp; jcdp++) {
      pint = (RecInfo[jcdp].dlots[locx]-RecInfo[jcdp-1].dlots[locx])
           * (RecInfo[jcdp].dlots[locx]-RecInfo[jcdp-1].dlots[locx])
           + (RecInfo[jcdp].dlots[locy]-RecInfo[jcdp-1].dlots[locy])
           * (RecInfo[jcdp].dlots[locy]-RecInfo[jcdp-1].dlots[locy]);

      dpatha[jcdp] = dpatha[jcdp-1] + sqrt(pint);
    }

/* Set the initial and maximum distance values so that points will be         */
/* extrapolated at both ends.                                                 */
 
    chordhere = 0. - chordi*nmaxa - dextra;
    chordimax = dpatha[ncdp-1] + chordi*nmaxa + dextra;
    mout = 9 + (chordimax-chordhere) / chordi; /* (plus 9 incase of round-off)*/
    dalla = calloc(mout,sizeof(double *));
    if(dalla == NULL) err("**** Unable to allocate dalla pp memory ");
    dinput = calloc(mout*klast,sizeof(double));
    if(dinput == NULL) err("**** Unable to allocate dalla p memory ");

    for (jcdp=0; jcdp<mout; jcdp++) dalla[jcdp] = dinput + jcdp*klast;

    nhere = 1;
    msize = 0;
    while(chordhere<=chordimax) {

/* Use while here because the next location might be further than chordi away.*/

      while(chordhere>dpatha[nhere] && nhere<ncdp-1) nhere++; 

/* Linear interpolate between surrounding locations (extrapolate off ends).   */

      if(fabs(dpatha[nhere]-dpatha[nhere-1]) < 1.e-15) wi = 0.5;
      else wi = (chordhere-dpatha[nhere-1]) / (dpatha[nhere]-dpatha[nhere-1]);

      for(i=0; i<klast; i++) {
        dalla[msize][i] = RecInfo[nhere-1].dlots[i]*(1.0-wi) + RecInfo[nhere].dlots[i]*wi;
      }

      chordhere += chordi;
      msize += 1; 

    } /* end of   while(chordhere<chordimax) { */

  } /* end of   if(chordi>0.0) { */

/* Not performing initial chording?                                           */

  else { 
    mout  = ncdp+2; /* going to extrapolate 1 extra point at both ends.       */
    dalla = calloc(mout,sizeof(double *));
    if(dalla == NULL) err("**** Unable to allocate dalla pp memory ");
    dinput = calloc(mout*klast,sizeof(double));
    if(dinput == NULL) err("**** Unable to allocate dalla p memory ");
    for (jcdp=0; jcdp<mout; jcdp++) dalla[jcdp] = dinput + jcdp*klast;

    for (jcdp=0; jcdp<ncdp; jcdp++) {
      for(i=0; i<klast; i++) dalla[jcdp+1][i] = RecInfo[jcdp].dlots[i];
    }

    for(i=0; i<klast; i++) dalla[0][i] = dalla[1][i]*2.0 - dalla[2][i];
    for(i=0; i<klast; i++) dalla[ncdp+1][i] = dalla[ncdp][i]*2.0 - dalla[ncdp-1][i];

    msize = ncdp+2;
  }

/* -------------------------------------------------------------------------- */
/* perform averaging? */

  if(nmaxa>0) { 
    runav (dalla, msize, klast, nmina, nmaxa, &ierr);
    if(ierr==1) err("**** Error in runav routine for averaging. Profile too short for averaging length.");
    else if(ierr>0) err("**** Error in runav routine for averaging. Parameter out-of-range.");

/* Extrapolate the ends using 2 good points near them.                        */

    for (jcdp=0; jcdp<nmaxa; jcdp++) {
      wi = (double)(nmaxa-jcdp+1);
      for(i=0; i<klast; i++) dalla[jcdp][i] = dalla[nmaxa][i]*wi + dalla[nmaxa+1][i]*(1.0-wi);
    }
    for (jcdp=msize-nmaxa-1; jcdp<msize; jcdp++) {
      wi = (double)(nmaxa+jcdp-msize+3);
      for(i=0; i<klast; i++) dalla[jcdp][i] = dalla[msize-nmaxa-3][i]*(1.0-wi) + dalla[msize-nmaxa-2][i]*wi;
    }
    
  } /* end of  if(nmaxa>0) { */

/* -------------------------------------------------------------------------- */
/* perform smoothing? */

  if(nmaxs>0) {
    runav (dalla, msize, klast, nmins, 0-nmaxs, &ierr);
    if(ierr==1) err("**** Error in runav routine for pre-smoothing. Profile too short for smoothing length.");
    else if(ierr>0) err("**** Error in runav routine for pre-smoothing. Parameter out-of-range.");

    for (jcdp=0; jcdp<nmaxs; jcdp++) {
      wi = (double)(nmaxs-jcdp+1);
      for(i=0; i<klast; i++) dalla[jcdp][i] = dalla[nmaxs][i]*wi + dalla[nmaxs+1][i]*(1.0-wi);
    }
    for (jcdp=msize-nmaxs-1; jcdp<msize; jcdp++) {
      wi = (double)(nmaxs+jcdp-msize+3);
      for(i=0; i<klast; i++) dalla[jcdp][i] = dalla[msize-nmaxs-3][i]*(1.0-wi) + dalla[msize-nmaxs-2][i]*wi;
    }

    runav (dalla, msize, klast, nmins, nmaxs, &ierr);
    if(ierr==1) err("**** Error in runav routine for smoothing. Profile too short for smoothing length.");
    else if(ierr>0) err("**** Error in runav routine for smoothing. Parameter out-of-range.");

    for (jcdp=0; jcdp<nmaxs; jcdp++) {
      wi = (double)(nmaxs-jcdp+1);
      for(i=0; i<klast; i++) dalla[jcdp][i] = dalla[nmaxs][i]*wi + dalla[nmaxs+1][i]*(1.0-wi);
    }
    for (jcdp=msize-nmaxs-2; jcdp<msize; jcdp++) {
      wi = (double)(nmaxs+jcdp-msize+3);
      for(i=0; i<klast; i++) dalla[jcdp][i] = dalla[msize-nmaxs-3][i]*(1.0-wi) + dalla[msize-nmaxs-2][i]*wi;
    }
  } /* end of  if(nmaxs>0) { */ 

/* -------------------------------------------------------------------------- */
/* perform final chording?  */

  if(chordf>0.0) {

    ncdp = msize;

    dpaths = ealloc1double(ncdp);

    dpaths[0] = 0.;
    for (jcdp=1; jcdp<ncdp; jcdp++) {
      pint = (dalla[jcdp][locx]-dalla[jcdp-1][locx])
           * (dalla[jcdp][locx]-dalla[jcdp-1][locx])                    
           + (dalla[jcdp][locy]-dalla[jcdp-1][locy])
           * (dalla[jcdp][locy]-dalla[jcdp-1][locy]);

      dpaths[jcdp] = dpaths[jcdp-1] + sqrt(pint);
    }

    chordhere = 0. - chordf*nmaxs;
    chordfmax = dpaths[ncdp-1] + chordf*nmaxs;
    mout = 9 + (chordfmax-chordhere) / chordf;
    dalls = calloc(mout,sizeof(double *));
    if(dalls == NULL) err("**** Unable to allocate dalls pp memory ");
    dinput = calloc(mout*klast,sizeof(double));
    if(dinput == NULL) err("**** Unable to allocate dalls p memory ");

    for (jcdp=0; jcdp<mout; jcdp++) dalls[jcdp] = dinput + jcdp*klast;

    nhere = 1;
    msize = 0;
    while(chordhere<=chordfmax) {
      while(chordhere>dpaths[nhere] && nhere<ncdp-1) nhere++; 
      if(fabs(dpaths[nhere]-dpaths[nhere-1]) < 1.e-15) wi = 0.5;
      else wi = (chordhere-dpaths[nhere-1]) / (dpaths[nhere]-dpaths[nhere-1]);
      for(i=0; i<klast; i++) {
        dalls[msize][i] = dalla[nhere-1][i]*(1.0-wi) + dalla[nhere][i]*wi;
      }
      chordhere += chordf;
      msize += 1; 
    } 

  } /* end of   if(chordf>0.0) { */

  else {
    dalls = dalla;                                                            
  }

/* Does user want to shift the cdp centres?                */

  if(nshift>0) {

/* Default the ends to the outer shift values. */

    shiftrec[0] = 0;
    shiftrec[nshift-1] = msize-1;
    shiftdist[0] = shiftdist[1];
    shiftdist[nshift-1] = shiftdist[nshift-2];

    dalla = dalls;
    ncdp = msize;
    dpaths = ealloc1double(ncdp+1); /* Add an extra 1 to simplify pint2 code. */
    dpaths[0] = 0.;

    for (jcdp=1; jcdp<ncdp; jcdp++) {
      pint = (dalla[jcdp][locx]-dalla[jcdp-1][locx])
           * (dalla[jcdp][locx]-dalla[jcdp-1][locx])                    
           + (dalla[jcdp][locy]-dalla[jcdp-1][locy])
           * (dalla[jcdp][locy]-dalla[jcdp-1][locy]);

      dpaths[jcdp] = dpaths[jcdp-1] + sqrt(pint);
    }

/* Set the extra point distance to simplify next for loop code. */

    dpaths[ncdp] = dpaths[ncdp-1] + sqrt(pint);

    mout = 9 + ncdp;
    dalls = calloc(mout,sizeof(double *));
    if(dalls == NULL) err("**** Unable to allocate dalls ppshift memory ");
    dinput = calloc(mout*klast,sizeof(double));
    if(dinput == NULL) err("**** Unable to allocate dalls pshift memory ");

    for (jcdp=0; jcdp<mout; jcdp++) dalls[jcdp] = dinput + jcdp*klast;

    nhere = 1;
    pint  = 0.;
    pint2 = shiftdist[0];

    for (jcdp=1; jcdp<ncdp; jcdp++) {
      if(jcdp > shiftrec[nhere]) {
        nhere++;
        pint = (shiftdist[nhere]-shiftdist[nhere-1])   /* needed squeeze/expand */ 
             / (dpaths[shiftrec[nhere]]-dpaths[shiftrec[nhere-1]]); /* per metre*/
        pint2 = shiftdist[nhere-1];
      }
 
      if(fabs(dpaths[jcdp]-dpaths[jcdp-1]) < 1.e-15) wi = 0.5;
      else wi = pint2 / (dpaths[jcdp]-dpaths[jcdp-1]);
      for(i=0; i<klast; i++) {
        dalls[jcdp-1][i] = dalla[jcdp-1][i]*(1.0-wi) + dalla[jcdp][i]*wi;
      }
      
      pint2 += pint * (dpaths[jcdp]-dpaths[jcdp-1]);         

    }

  } /* end of  if(nshift>0) {  */

/*-------------------------------------------------------------------------- */
/* Write the q-file. See program SUBINQCSV for details.                      */
/*                                                                           */
/* Remember:   klast = iztuple + (ktuple-1)*RecInfo[0].nto;                  */
/* We need to output the cdp number after iztuple.                           */

  fprintf(fpQ,"C_SU_SETID,Q\nC_SU_FORMS\nC_SU_ID");
  for(i=0; i<klast+1; i++) fprintf(fpQ,",%s",formxy);

  if(ifixd!=2) {
    fprintf(fpQ,"\nC_SU_NDIMS,%s",ndims[0]); 
    for(i=1; i<iztuple+1; i++) fprintf(fpQ,","); 
    for(i=0; i<RecInfo[0].nto; i++) { 
      for(k=0; k<ktuple-1; k++) fprintf(fpQ,",%.15g",pindepa[i]);
    }
  }

/* cdp is always the first value (after the Q). If it exists on input,        */
/* override its name. Note: it was allowed to be used as keyp on input.       */

  fprintf(fpQ,"\nC_SU_NAMES\nC_SU_ID");
  fprintf(fpQ,",cdp");
  for(i=0; i<iztuple; i++) {
    if(strcmp(pname[i],"cdp")==0) fprintf(fpQ,",null");
    else fprintf(fpQ,",%s",pname[i]);
  }
  for(i=0; i<RecInfo[0].nto; i++) { 
    for(k=0; k<ktuple-1; k++) {
      if(strcmp(pname[k+iztuple],"cdp")==0) fprintf(fpQ,",null");
      else fprintf(fpQ,",%s",pname[k+iztuple]);
    }
  }  

  fprintf(fpQ,"\n");

/* Note that we do not output the two end-points. In all situations these     */
/* end-points could be output anyway without additional confusion since       */
/* multiple points were extrapolated to simplify code during initial chording,*/
/* averaging, smoothing, and final chording. But sometimes only shifting is   */
/* being done, and it is nicer to get same number of points output as input.  */

  if(locp>-1) jcrf = lrint(dalls[1][locp] * pcru + pcrz) - 1;
  else jcrf = lrint(pcrz) - 1; 

  for (jcdp=1; jcdp<msize-1; jcdp++) { 
    fprintf(fpQ,"Q");                                                       
    fprintf(fpQ,formxylong,(double)(jcdp+jcrf));
    for(i=0; i<klast; i++) fprintf(fpQ,formxylong,dalls[jcdp][i]);
    fprintf(fpQ,"\n");
  }

} /* end of suprofcsv */

/* -----------------------------------------------------------         */
/* Specify compare function for qsort.                                 */

int compSort1 (const void * q1, const void * q2) {

  struct QInfo* p1 = (struct QInfo*) q1;
  struct QInfo* p2 = (struct QInfo*) q2;

  if(p1->dlots[locp] < p2->dlots[locp]) return (-1);
  if(p1->dlots[locp] > p2->dlots[locp]) return (1); 

  return (0); 

}

/*    Running average.                                                */
/* Input arguments:                                                   */
/* dall[ncdp][klast] = the values.                                    */
/* ncdp  = length of first  dimension of dall.                        */
/* klast = length of second dimension of dall.                        */
/* nmin  = minimum length in operator. >=0 and <=nmax.                */
/* nmax  = maximum length in operator. !=0.                           */
/*         nmin and |nmax| refer to a number of points in the first   */
/*         dimension of dall. The average for point i is all points   */
/*         from i-nmax to i-nmin and i+nmin to i+nmax inclusive       */ 
/*         (i.e. a symmetric average). For nmin of 0 the average is   */
/*         all points from i-nmax to i+nmax.                          */
/*         If nmax is positive, output the average. If nmax negative, */
/*         output the difference between the average and the original */
/*         value, added back to the original value.                   */
/*                                                                    */
/* Note: Realistically, nmax should be considerably less than ncdp/2  */
/*       Returns ierr=1 if 2*nmax+6 > ncdp)                           */
/*                                                                    */
/* Output arguments:                                                  */
/* dall[ncdp][klast] = contains the averaged values from dall[nmax]   */
/*                     to dall[ncdp-nmax]. The values outside that    */
/*                     are nonsense. In other words, on input, at     */
/*                     least the first nmax and last nmax locations   */
/*                     must be extrapolated or extra points that you  */
/*                     do not mind having twimmed back. The ends are  */
/*                     used by this routine but then altered for its  */
/*                     its own purposes.                              */
/* ierr = 0 (good).                                                   */
/*      = 1 too few points (ncdp) for nmax (number to average).       */
/*      = 2 some input argument is out-of-range.                      */

void runav (double **dall, int ncdp, int klast, int nmin, int nmax, int *ierr) {

  int iavr = 1;
  if(nmax<0) {
    nmax = 0 - nmax;
    iavr = 0;
  }

  if(ncdp<1 || klast<1 || nmin<0 || nmax<nmin) {
    *ierr = 2;
    return;
  }
  if(2*(nmax+3) > ncdp) {
    *ierr = 1;
    return;
  }

  *ierr = 0;

  int i = 0;
  int k = 0;
  int n = 0;

/* Basically this routine proceeds by keeping a running total.   */
/* As we move to the next location, the operator adds the values */
/* for the location that enters the sum range, and subtracts     */
/* the values for the location that leaves the sum range.        */
/* When nmin is non-zero the sum operator also has an inside     */
/* range which is added/subtracted in the same way.              */

  double *dtog = ealloc1double(klast);

/* So, first initialize the sum for the starting location        */
/* (which is also nmax since summing operator is symmetrical).   */

  n = 0;
  for (k=0; k<klast; k++) dtog[k] = 0.0;

  for (i=0; i<nmax*2+1; i++) {
    n++;
    for (k=0; k<klast; k++) dtog[k] += dall[i][k];
  } 

  for (i=nmax-nmin+1; i<nmax+nmin; i++) {
    n--;
    for (k=0; k<klast; k++) dtog[k] -= dall[i][k];
  } 

/* Add next values (at i+1+nmax) and subtract trailing values (at i-nmax)     */ 
/* Note this means the resulting totals are actually for the i+1 location.    */ 

  for (i=nmax; i<ncdp-nmax-1; i++) {

    for (k=0; k<klast; k++) dtog[k] += dall[i+1+nmax][k] - dall[i-nmax][k];

    if(nmin>0) {
      for (k=0; k<klast; k++) dtog[k] += dall[i+1-nmin][k] - dall[i+nmin][k];
    }
  
/* Temporarily store the values into the location that we are just beyond.    */

    if(iavr==1) {
      for (k=0; k<klast; k++) dall[i-nmax][k] = dtog[k] / n;
    }
    else {
      for (k=0; k<klast; k++) dall[i-nmax][k] = dall[i+1][k] + dall[i+1][k] - dtog[k] / n;
    }

  }

/* Move values from where they were temporarily stored to where they belong.  */

  for (i=ncdp-nmax-1; i>=nmax; i--) {
    for (k=0; k<klast; k++) dall[i][k] = dall[i-nmax][k];
  }

  return; 

}

