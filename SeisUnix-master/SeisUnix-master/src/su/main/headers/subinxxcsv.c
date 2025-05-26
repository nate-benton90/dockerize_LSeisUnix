/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.                       */

/* SUBINXXCSV: $Revision: 1.01 $ ; $Date: 2021/12/22 00:00:01 $		*/
 
#include "su.h"
#include "segy.h" 
#include "readkfile.h"
#include "gridread.h"
#include "gridxy.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"									     ",
" SUBINXXCSV - Expand Size and Exchange Corners of 3D Grid Definition.       ",
"									     ",
"  subinxxcsv <stdin >stdout [parameters]                                    ",
"									     ",
"									     ",
" Parameters:			          				     ",
"                                                                            ",
" rfile=   If specified, read a K-file containing a 3D grid definition.      ", 
"          Alternately you can supply the grid definition via command line   ", 
"          parameters. The required command line parameters are:             ",
"           grid_xa,grid_ya,grid_xb,grid_yb,grid_xc,grid_yc,grid_wb,grid_wc. ", 
"          Note that input grid definitions always run a routine which       ", 
"          adjusts the input values into compliance with grid standards.     ", 
"          (See program SUBINCSV for 3D grid details).                       ", 
"									     ",
" wfile=   File to write the new grid definition after changes caused by     ",
"          the remaining parameters. Must be specified.                      ",
"									     ",
" igilow=0  Add cells at low igi end (default is not to add cells).          ",
"       =n  Add n cells at low igi end (negative makes grid smaller).        ",
"									     ",
" igihigh=0 Add cells at high igi end (default is not to add cells).         ",
"        =n Add n cells at high igi end (negative makes grid smaller).       ",
"									     ",
" igclow=0  Add cells at low igc end (default is not to add cells).          ",
"       =n  Add n cells at low igc end (negative makes grid smaller).        ",
"									     ",
" igchigh=0 Add cells at high igc end (default is not to add cells).         ",
"        =n Add n cells at high igc end (negative makes grid smaller).       ",
"									     ",
" iflip=0   Defaults to not exchanging any grid corners.                     ",
"           This option is done AFTER any add-cell options listed above.     ",
"      =1   Exchange corner B with corner C and also exchange cell widths.   ",
"           This results in cdp numbers incrementing by 1 at right-angle     ",
"           to input grid and also exchanges igi,igc directions.             ",
"      =2   Exchange corner A with corner D and also exchange cell widths.   ",
"           This results in cdp numbers incrementing by 1 at right-angle     ",
"           to input grid and also exchanges igi,igc directions. But cdp     ",
"           numbering will now start where input corner D was.               ",
"      =-1  Exchange corner A with corner B and corner C with corner D.      ",
"      =-2  Exchange corner A with corner C and corner B with corner D.      ",
"									     ",
" check=0   Do not print grid checking details.                              ",
"     Note: The options of this program cannot repair a corrupt input grid.  ",
"           Grid checking is always done on input grid definition and will   ",
"           error-halt if corrupt.                                           ",
"      =1   print grid checking details. This runs some grid functions on    ",
"           the 4 corner points of input grid, expanded grid (if any), and   ",
"           flipped grid (if any). This information can be written to a file ",
"           by putting 2>yourfile on command line.                           ",
"									     ",
" CAUTION: Any grid change is likely to result in some pre-stack traces      ", 
"          being assigned to different cells (cdps) than pure mathematics    ", 
"          would predict. That is, NEVER EXPECT the group of traces assigned ", 
"          to each cdp by the input grid definition to be exactly the same   ", 
"          as the group of traces assigned to any corresponding cdps by the  ", 
"          output grid definition. This is true despite computations herein  ", 
"          being performed with 8 byte floating point (double precision).    ", 
"          This issue occurs because some trace midpoints lie exactly on     ", 
"          cell boundaries - and even the slightest change in the grid       ", 
"          definition results in slight adjustments of all cell boundaries.  ", 
"            So, you should make grid adjustments before you start ANY trace ", 
"            processing, or re-run all trace processes, including, but not   ", 
"            limited to, cdp sorting.                                        ", 
"          Notwithstanding this caution, you have a reasonable chance of     ", 
"          using an altered grid post-stack because the post-stack cdps are  ", 
"          essentially located at the cell centre XYs, and cell centres are  ", 
"          usually far away from your output grid cell boundaries.           ", 
"									     ",
NULL};

 /* Credits:                                       
 *  Andre Latour. Dec. 2021.   
 *                              
 */
/**************** end self doc *******************************************/

int main(int argc, char **argv) {

  int icheck = 0;
  int igilow = 0;
  int igihigh = 0;
  int igclow = 0;
  int igchigh = 0;
  int iflip = 0;
  int jgilow = 0;
  int jgihigh = 0;
  int jgclow = 0;
  int jgchigh = 0;
  int numcases = 0;
  int errwarn = 0; 
  int numcasesout = 0;
  int ifound = 0;
  int maygrid = 0;

  int i = 0;   		/* just a general integer               */
  int j = 0;   		/* just a general integer               */
  int m = 0;   		/* just a general integer               */
  int n = 0;   		/* just a general integer               */

  cwp_String Rname=NULL;  /* text file name for input grid      */
  FILE *fpR=NULL;         /* file pointer for Rname input file  */
  cwp_String Wname=NULL;  /* text file name for output grid     */
  FILE *fpW=NULL;         /* file pointer for Wname output file */

  cwp_String names[999];  /* for names returned by readkfile    */
  cwp_String forms[999];  /* for formats returned by readkfile  */
  double dfield[999];     /* for values returned by readkfile   */

  int numgnams = 18;      /* number of names defining grid      */
  cwp_String gnams[18];   /* names of parameters defining grid  */
  double gvals[18];       /* to contain a set of grid values    */
  double gvalb[18];       /* for another set of grid values     */

/* hook up getpar */

  initargs(argc, argv);
  requestdoc(1);

  if(isatty(STDIN_FILENO)!=1 || isatty(STDOUT_FILENO)!=1)  
    err("**** Error: this program does not input or output traces.");
          
/* ------------------------------------------------------------------- */

  getparstring("rfile", &Rname);

  getparstring("wfile", &Wname);
  if(Rname != NULL && Wname != NULL && strcmp(Rname,Wname) == 0) 
    err("**** Error: wfile= output K-file must be different than rfile= input K-file.");
      
  if(Rname == NULL && Wname != NULL) 
    err("**** Error: wfile= cannot output K-file with no rfile= input K-file.");
  
  if (!getparint("check", &icheck)) icheck = 0;
  if(icheck<0 || icheck>1) err("**** Error: check= value out of range.");

  if (!getparint("igilow", &igilow)) igilow = 0;

  if (!getparint("igihigh", &igihigh)) igihigh = igilow;

  if (!getparint("igclow", &igclow)) igclow = 0;

  if (!getparint("igchigh", &igchigh)) igchigh = igclow;

  if (!getparint("iflip", &iflip)) iflip = 0;
  if(iflip<-2 || iflip>2) err("**** Error: iflip= value out of range.");

/* open and read K-file if not a full grid defined on command line. */

  gridcommand(&maygrid);
  if(maygrid==1  && Rname != NULL) err("error: input k-file not allowed when full grid on command line.");
  if(maygrid==-1 && Rname == NULL) err("error: input k-file required when partial grid on command line.");

  if(maygrid!=1) { 
    fpR = fopen(Rname, "r");
    if(fpR==NULL) err("error: input K-file did not open correctly.");

    readkfile(fpR,names,forms,dfield,&numcases,&errwarn);

    if(errwarn==1) err("K-file read error: more than one C_SU_NAMES parameter record.");
    else if(errwarn==2) err("K-file read error: no C_SU_NAMES parameter record.");
    else if(errwarn==3) err("K-file read error: more than one C_SU_FORMS parameter record.");
    else if(errwarn==4) err("K-file read error: no C_SU_FORMS parameter record.");
    else if(errwarn==5) err("K-file read error: more than one C_SU_SETID record.");
    else if(errwarn==6) err("K-file read error: no C_SU_SETID record.");
    else if(errwarn==7) err("K-file read error: different number of values on C_SU_NAMES and C_SU_FORMS.");
    else if(errwarn==8) err("K-file read error: unable to allocate memory.");
    else if(errwarn==9) err("K-file read error: name exists at least twice in C_SU_NAMES list.");
    else if(errwarn==10) err("K-file read error: at least 1 field-unreadable as a number.");
    else if(errwarn==11) err("K-file read error: at least 1 field containing 2 numbers.");
    else if(errwarn==12) err("K-file read error: not-enough-commas to get all values for C_SU_NAMES list.");
    else if(errwarn>0) err("K-file read error: returned with some unrecognized error code.");
    else if(errwarn==-1) warn("K-file read warning: at least 1 all-blank field, assumed zero for all.");

    for (n=0; n<numcases; n++) {
      for (m=0; m<strlen(names[n]); m++) {
        names[n][m] = tolower(names[n][m]);
      }
    }

  } /* end of  if(maygrid!=1) { */ 

/* -----------------------------------------------------------    */

  for(i=0; i<numgnams; i++) {
    gnams[i] = ealloc1(7,1);
  }

  strcpy(gnams[0],"bintype");  /* copied, but not used herein */
  gvals[0] = 0.; /* in case bintype not exist for some reason */

  strcpy(gnams[1],"grid_lf");
  strcpy(gnams[2],"grid_xa");
  strcpy(gnams[3],"grid_ya");
  strcpy(gnams[4],"grid_xb");
  strcpy(gnams[5],"grid_yb");
  strcpy(gnams[6],"grid_xc");
  strcpy(gnams[7],"grid_yc");
  strcpy(gnams[8],"grid_xd");
  strcpy(gnams[9],"grid_yd");
  strcpy(gnams[10],"grid_wb");
  strcpy(gnams[11],"grid_wc");
  strcpy(gnams[12],"grid_nb");
  strcpy(gnams[13],"grid_nc");
  strcpy(gnams[14],"grid_fp");
  strcpy(gnams[15],"grid_lp");
  strcpy(gnams[16],"grid_sb");
  strcpy(gnams[17],"grid_cb");

/* copy bintype, read-in corners A,B,C and cell widths, but not other stuff */

  for(i=0; i<12; i++) {    
    if(i==1 || i==8 || i==9) continue; 
    if(!getpardouble(gnams[i],gvals+i)) {
      for(j=0; j<numcases; j++) {
        if(strcmp(names[j],gnams[i]) == 0) gvals[i] = dfield[j];
      }
      if(gvals[i] < -1.e308) {
        gvals[i] = i+100;
        if(i>0) err("Error: parameter %s not found.",gnams[i]);
      }
    }
  } /* end of  for(i=2; i<12; i++) { */

  checkpars(); 

/* Process and set other grid values. */

  gridset(gvals,&errwarn);

  if(errwarn==1) err ("gridset error: grid_wb cell width must be positive.");
  else if(errwarn==2) err ("gridset error: grid_wc cell width must be positive.");
  else if(errwarn==3) err ("gridset error: corner B is within grid_wb cell width of corner A.");
  else if(errwarn>0) err ("gridset error: returned with some unrecognized error code.");
  else if(errwarn==-1) warn ("gridset warning: corner C is near A and is reset to A.");

  gridcheck(gvals,icheck,&errwarn);
  if(errwarn>0) err ("gridcheck error: returned with some unrecognized error code.");

/* Make alterations to input grid definition? */ 

  if(igilow!=0 || igihigh!=0 || igclow!=0 || igchigh!=0) {

/* Copy previous grid definition into gvalb.                          */

    for(i=0; i<numgnams; i++) gvalb[i] = gvals[i];

    jgilow  = 1 - igilow; 
    jgihigh = igihigh + (int)(gvals[12] + 0.1); 
    jgclow  = 1 - igclow; 
    jgchigh = igchigh + (int)(gvals[13] + 0.1); 

/* Use previous grid and reset corners A, B, and C.                 */
/* (the gridset routine will reset corner D, and other stuff).      */
/* Note that gridicrawxy does not care if indexes are outside grid, */
/* it computes the corresponding XYs anyway.                        */

    gridicrawxy(gvals,jgilow,jgclow,gvalb+2,gvalb+3);  /* reset corner A */

    gridicrawxy(gvals,jgihigh,jgclow,gvalb+4,gvalb+5); /* reset corner B */

    gridicrawxy(gvals,jgilow,jgchigh,gvalb+6,gvalb+7); /* reset corner C */

    for(i=0; i<numgnams; i++) gvals[i] = gvalb[i]; /* copy back to "previous" grid. */

    gridset(gvals,&errwarn);

    if(errwarn==1) err ("gridset error: grid_wb cell width must be positive.");
    else if(errwarn==2) err ("gridset error: grid_wc cell width must be positive.");
    else if(errwarn==3) err ("gridset error: corner B is within grid_wb cell width of corner A.");
    else if(errwarn>0) err ("gridset error: returned with some unrecognized error code.");
    else if(errwarn==-1) warn ("gridset warning: corner C is near A and is reset to A.");

    gridcheck(gvals,icheck,&errwarn);
    if(errwarn>0) err ("gridcheck error: returned with some unrecognized error code.");

  } /* end of  if(igilow!=0 || igihigh!=0 || igclow!=0 || igchigh!=0) { */

  if(iflip!=0) {

/* Copy previous grid definition into gvalb.                          */

    for(i=0; i<numgnams; i++) gvalb[i] = gvals[i];

    if(iflip==1) { /* Exchange corner B with corner C and exchange cell widths.*/
      gvalb[4]  = gvals[6];
      gvalb[5]  = gvals[7];
      gvalb[6]  = gvals[4];
      gvalb[7]  = gvals[5];
      gvalb[10] = gvals[11];
      gvalb[11] = gvals[10];
    }
    else if(iflip==2) { /* Exchange corner A with corner D and exchange cell widths.*/
      gvalb[2]  = gvals[8];
      gvalb[3]  = gvals[9];
/*    gvalb[8]  = gvals[2]; no actual need to set corner D, gridset does it. */
/*    gvalb[9]  = gvals[3]; no actual need to set corner D, gridset does it. */
      gvalb[10] = gvals[11];
      gvalb[11] = gvals[10];
    }
    else if(iflip==-1) { /* Exchange corner A with corner B and corner C with corner D. */
      gvalb[2]  = gvals[4];
      gvalb[3]  = gvals[5];
      gvalb[4]  = gvals[2];
      gvalb[5]  = gvals[3];
      gvalb[6]  = gvals[8];
      gvalb[7]  = gvals[9];
/*    gvalb[8]  = gvals[6]; no actual need to set corner D, gridset does it. */
/*    gvalb[9]  = gvals[7]; no actual need to set corner D, gridset does it. */
    }
    else if(iflip==-2) { /* Exchange corner A with corner C and corner B with corner D. */
      gvalb[2]  = gvals[6];
      gvalb[3]  = gvals[7];
      gvalb[6]  = gvals[2];
      gvalb[7]  = gvals[3];
      gvalb[4]  = gvals[8];
      gvalb[5]  = gvals[9];
/*    gvalb[8]  = gvals[4]; no actual need to set corner D, gridset does it. */
/*    gvalb[9]  = gvals[5]; no actual need to set corner D, gridset does it. */
    }

    for(i=0; i<numgnams; i++) gvals[i] = gvalb[i]; /* copy back to "previous" grid. */

    gridset(gvals,&errwarn);

    if(errwarn==1) err ("gridset error: grid_wb cell width must be positive.");
    else if(errwarn==2) err ("gridset error: grid_wc cell width must be positive.");
    else if(errwarn==3) err ("gridset error: corner B is within grid_wb cell width of corner A.");
    else if(errwarn>0) err ("gridset error: returned with some unrecognized error code.");
    else if(errwarn==-1) warn ("gridset warning: corner C is near A and is reset to A.");

    gridcheck(gvals,icheck,&errwarn);
    if(errwarn>0) err ("gridcheck error: returned with some unrecognized error code.");

  } /* end of  if(iflip!=0) { */

/*  Open output k-file... */

  numcasesout = numcases;

  fpW = fopen(Wname, "w");
  if (fpW == NULL) err("wfile error: output K-file did not open correctly.");

/* Update/add whatever names/values are used by whatever options.             */

  for(i=0; i<numgnams; i++) {

    ifound = 0;
    for(j=0; j<numcases; j++) { /* reset it, if already in input K-file */
      if(strcmp(names[j],gnams[i]) == 0) {
        dfield[j] = gvals[i];
        ifound = 1;
        break;
      }
    }
    if(ifound == 0) {               /* or add it */
      dfield[numcasesout] = gvals[i];
      names[numcasesout] = ealloc1(7,1);
      strcpy(names[numcasesout],gnams[i]);
      forms[numcasesout] = ealloc1(5,1);
      strcpy(forms[numcasesout],"%.20g");
      numcasesout++;
    }

  } /* end of  for(i=0; i<numgnams; i++) { */

  writekfile(fpW,names,forms,dfield,numcasesout,&errwarn);
  if(errwarn>0) err("K-file write error: returned with an unrecognized error code.");

}
