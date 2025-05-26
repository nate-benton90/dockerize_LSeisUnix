/* Copyright (c) Colorado School of Mines, 2022.*/
/* All rights reserved.                       */

/* SUBINBIGCSV: $Revision: 1.00 $ ; $Date: 2022/04/29 00:09:00 $      */

#include <stdio.h>
#include <string.h>

#include "su.h"
#include "segy.h"
#include "gridread.h"
#include "gridxy.h"

segy tr;

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                            ",
" SUBINBIGCSV - Make Some CDPs Larger and Delete All Traces Outside Them.    ",
"                                                                            ",
" subinbigcsv  rfile=in.csv <in.su >out.su  (other parameters)               ",
"                                                                            ",
" Parameter overview:                                                        ",
"                                                                            ",
"  rfile=    If specified, read a K-file containing 3D Grid definition.      ", 
"            See subincsv for 3D Grid documentation.                         ",
"            If a 3D Grid is not specified, assume input has 2D cdp numbers. ", 
"                                                                            ",
" igiout=f,l,i  Output igi range. If specified, all 3 integers required.     ",
"               Start at f, increment by i, end at l or nearest igi that     ",
"               is not greater than maximum igi of Grid. See EXAMPLE below.  ",
"           *** Or, specify the next igilist paramter.                       ",
"                                                                            ",
" igilist=      Output igi list. Or specify previous igiout parame3ter.      ",
"               This parameter still produces locations which satisfy the    ",
"               enclosing-rectangle requirements for bilinear interpolation. ",
"                                                                            ",
" igcout=f,l,i  Output igc range. If specified, all 3 integers required.     ",
"               Start at f, increment by i, end at l or nearest igc that     ",
"               is not greater than maximum igc of Grid. See EXAMPLE below.  ",
"           *** Or, specify the next igclist paramter.                       ",
"                                                                            ",
" igclist=      Output igc list. Or specify previous igcout parame3ter.      ",
"               This parameter still produces locations which satisfy the    ",
"               enclosing-rectangle requirements for bilinear interpolation. ",
"                                                                            ",
" igiext=0   Output igi extent (default 0 means extent is only igiout itself)",
"            List with one or two non-negative integers which form an        ",
"            extent of igi values around the central igiout values.          ",
"       =l     with only 1 value, h defaults to l (symmetric extent)         ",
"       =l,h   start at l lower than each central igiout value, continue     ",
"              until reaching h higher than each central igiout value.       ",
"      Note: Must be less than the igiout increment.                         ",
"                                                                            ",
" igcext=0   Output igc extent (default 0 means extent is only igcout itself)",
"            List with one or two non-negative integers which form an        ",
"            extent of igc values around the central igcout values.          ",
"       =l     with only 1 value, h defaults to l (symmetric extent)         ",
"       =l,h   start at l lower than each central igcout value, continue     ",
"              until reaching h higher than each central igcout value.       ",
"      Note: Must be less than the igcout increment.                         ",
"                                                                            ",
" intype=0   Default 3 if a 3D Grid is input, otherwise -2.                  ",
"       =3   Compute trace midpoint coordinates (sx+gx)/2 and (gx+gy)/2 and  ",
"            determine cdp,igi,igc values from them (using the 3D Grid).     ",
"            For this option traces do not need cdp,igi,igc numbers on input ",
"            (they do not need to have been through grid program subincsv).  ",
"      NOTE: For 3D STACKed traces, options 2 or 1 are required (usually).   ",
"       =2   Use input cdp (cell) key and 3D Grid to determine igi,igc.      ",
"       =1   Use input igi,igc keys and 3D Grid to determine cdp.            ",
"       =-2  Use input cdp (cell) key. A 3D Grid cannot be input.            ",
"      NOTE: For option -2 parameters igiout and igiext refer to cdp key     ",
"            numbers and igcout, igcext, reigi, reigc cannot be specified    ",
"            (reigi and reigc both use option 0).                            ",
"                                                                            ",
"  qout=     Output file containing the locations specified by the           ",
"            igiout (or igilist) and igcout (or igclist) parameters.         ",
"                                                                            ",
"                                                                            ",
" recdp=1    Reset the trace cdp key to its central cdp number.              ",
"      =0    Do not reset cdp key numbers.                                   ",
"                                                                            ",
" reigi=1    Reset the trace igi key to its central igi number.              ",
"      =0    Do not reset igi key numbers.                                   ",
"                                                                            ",
" reigc=1    Reset the trace igc key to its central igc number.              ",
"      =0    Do not reset igc key numbers.                                   ",
"                                                                            ",
"   check=0  Do not print checking details.                                  ",
"         1  Run functions on the 4 grid corner points and print results.    ",
"            This output print may be useful for users.                      ",
"                                                                            ",
" EXAMPLE ****************************************************************** ",
"   igiout=40,9999,40                                                        ",
"   igcout=5,9999,5                                                          ",
"   igiext=2                                                                 ",
"   igcext=1                                                                 ",
" This defines a central cdp every 40 cells in igi direction by 5 cells in   ",
" igc direction. Traces will ONLY BE OUTPUT if they are within 2 cells in    ", 
" igi direction and also within 1 cell in igc direction of the central cdps. ",
" By default, the cdp,igi,igc key values in the output traces will be reset  ",
" to the values of the central cdp of those traces. One purpose of this is   ",
" to create super-cdps for Velocity analysis.                                ",
" ************************************************************************** ",
" This program does not sort, it deletes traces, and renumbers cdp key.      ",
" Since cdp is renumbered the output is NOT cdp ordered even if you input a  ",
" cdp ordered dataset.                                                       ",
" ************************************************************************** ",
"                                                                            ",
" ***********************************************************                ",
"   To output this documentation:  subinbigcsv 2> binbigdoc.txt              ",
" ***********************************************************                ",
"                                                                            ",
" ------------------------------------------------------------------------   ", 
"                                                                            ",
NULL};

/* Credits:                                                       */
/* Andre Latour                                                   */ 
/*                                                                */
/* Started from subincsv, April 19 2022.                          */
/* Modified: Andre Latour, May 11 2022.                           */
/*  Added igilist,igclist parameters and code. These give more    */
/*  flexibility to where functions can be located for bilinear    */
/*  interpolation (NMO velocities, mutes). Also added qout        */
/*  parameter and code. This simplifies the Interactive Velocity  */
/*  Analysis (Velan) and Mute Definition (Mutan) scripts.         */
/*                                                                */
/* Trace keys involved: sx,sy,gx,gy,cdp,igi,igc                   */
/*                                                                */
/**************** end self doc ************************************/

int main(int argc, char **argv) {

  cwp_String Rname=NULL;  /* text file name for values             */
  FILE *fpR=NULL;         /* file pointer for Rname input file     */
  cwp_String Qname=NULL;  /* text file name for output Q values    */
  FILE *fpQ=NULL;         /* file pointer for Q output file        */

  int *igiout = NULL;   /* igi range for output of traces          */
  int *igiext = NULL;   /* igi extent for output of traces         */
  int *igcout = NULL;   /* igc range for output of traces          */
  int *igcext = NULL;   /* igc extent for output of traces         */

  int irecdp = 1;       /* flag to reset cdp numbers               */
  int ireigi = 1;       /* flag to reset igi numbers               */
  int ireigc = 1;       /* flag to reset igi numbers               */
  int intype=0;         /* option for which input keys to use      */

  double gvals[999];    /* to store 3d grid definition             */

  int nproct = 0; 
  int mproct = 0; 
  int icheck=0;
  int errwarn=0;
  int maygrid=0;
  int is3d = 1;
  int minigi = 0;     
  int maxigi = 0;     
  int minigc = 0;     
  int maxigc = 0;     
  int keepi = 0;
  int keepc = 1; /* stays 1 for the 2D case */
  int modi = 0;
  int modc = 0;

  double dx = 0.;
  double dy = 0.;
  double xg = 0.;
  double yg = 0.;
  int icdp = 1;
  int icdpt = 0;
  int igi = 1;
  int igc = 1; /* stays 1 for the 2D case */

  int i = 0;
  int j = 0;
  int mink = 0; 
  int maxk = 0; 
  int numlisti = 0;
  int *listi = NULL;
  int numlistc = 0;
  int *listc = NULL;
  int ngi = 0;
  int ngc = 0;
  int nlisti = 0;
  int nlistc = 0;

/* Initialize */
  initargs(argc, argv);
  requestdoc(1);

/* Process and set the grid definition values?                         */

  getparstring("rfile", &Rname);
    
  gridcommand(&maygrid);
    
  is3d = 1;
  if(maygrid==1  && Rname != NULL) err("error: input k-file not allowed when full grid on command line.");
  if(maygrid==-1 && Rname == NULL) err("error: input k-file required when partial grid on command line.");
  if(maygrid==0  && Rname == NULL) is3d = 0;

  if (!getparint("check", &icheck)) icheck = 0;

  if(is3d==1) {

    if(maygrid!=1) { /* open if not full grid on command line (else pass fpR still NULL) */
      fpR = fopen(Rname, "r");
      if(fpR==NULL) err("error: input K-file did not open correctly.");
    }

    errwarn = 1; /* print if error or unusual thing inside gridread */
    gridread(fpR,gvals,&errwarn); 
    if(errwarn>0) err("error reading grid (from K-file or command line parameters)");

    gridset(gvals,&errwarn); 

    if(errwarn==1) err ("gridset error: grid_wb cell width must be positive.");
    else if(errwarn==2) err ("gridset error: grid_wc cell width must be positive.");
    else if(errwarn==3) err ("gridset error: corner B is within grid_wb cell width of corner A.");
    else if(errwarn>0) err ("gridset error: returned with some unrecognized error code.");
    else if(errwarn==-1) warn ("gridset warning: corner C is near A and is reset to A.");
 
    gridcheck(gvals,icheck,&errwarn); 
    if(errwarn>0) err ("gridcheck error: returned with some unrecognized error code.");
  }

  if(!getparint("intype", &intype)) intype = 0;
  if(intype==0) {
    if(is3d==1) intype = 3;
    else        intype = -2;
  }
  if(intype!=-2 && intype!=1 && intype!=2 && intype!=3) err("error: intype= option not in range.");

  if(intype==-2) {
    if(is3d==1) err("error: intype -2 not allowed when a 3D Grid is input");
    if(countparval("igcout")>0) err("error: igcout= cannot be specified when intype is -2 (no 3D Grid).");
    if(countparval("igcext")>0) err("error: igcext= cannot be specified when intype is -2 (no 3D Grid).");
    if(countparval("igclist")>0) err("error: igclist= cannot be specified when intype is -2 (no 3D Grid).");
    if(countparval("reigi")>0) err("error: reigi= cannot be specified when intype is -2 (no 3D Grid).");
    if(countparval("reigc")>0) err("error: reigc= cannot be specified when intype is -2 (no 3D Grid).");
  }
  else {
    if(is3d==0) err("error: only intype -2 allowed when no 3D Grid is input");
    if(!getparint("reigi", &ireigi)) ireigi = 1;
    if(ireigi<0 || ireigi>1) err("error: reigi= option not in range.");
    if(!getparint("reigc", &ireigc)) ireigc = 1;
    if(ireigc<0 || ireigc>1) err("error: reigc= option not in range.");
    if(countparval("igclist")>0 && countparval("igcout")>0) 
      err("error: igcout= and igclist= cannot both be specified.");
    if(countparval("igcext")>2)  err("error: igcext= has too long a list.");
  }

  if(!getparint("recdp", &irecdp)) irecdp = 1;
  if(irecdp<0 || irecdp>1) err("error: recdp= option not in range.");
  if(countparval("igilist")>0 && countparval("igiout")>0) 
    err("error: igiout= and igilist= cannot both be specified.");
  if(countparval("igiext")>2)  err("error: igiext= has too long a list.");

  if(countparval("igilist")>0) {
    numlisti = countparval("igilist");
    listi = ealloc1int(numlisti);
    getparint("igilist",listi);
  }
  else {
    if(countparval("igiout")!=3) err("error: igiout= must have 3 integers in list.");
  }

  if(is3d>0) {
    if(countparval("igclist")>0) {
      numlistc = countparval("igclist");
      listc = ealloc1int(numlistc);
      getparint("igclist",listc);
    }
    else {
      if(countparval("igcout")!=3) err("error: igcout= must have 3 integers in list.");
    }
  }

/* Read the extents first. */

  igiext = ealloc1int(2);
  igiext[0] = 0;
  igiext[1] = 0;
  if(countparval("igiext")>0) getparint("igiext",igiext);
  if(countparval("igiext")==1) igiext[1] = igiext[0];
  if(igiext[0]<0 || igiext[1]<0) err("error: igiext= values cannot be less than 0");

  igcext = ealloc1int(2);
  igcext[0] = 0;
  igcext[1] = 0;
  if(countparval("igcext")>0) getparint("igcext",igcext);
  if(countparval("igcext")==1) igcext[1] = igcext[0];
  if(igcext[0]<0 || igcext[1]<0) err("error: igcext= values cannot be less than 0");

  if(is3d>0) {
    warn("igi extent total cells=%d, total width=%g    igc extent total cells=%d, total width=%g",
         igiext[0]+igiext[1]+1,(igiext[0]+igiext[1]+1)*gvals[10],
         igcext[0]+igcext[1]+1,(igcext[0]+igcext[1]+1)*gvals[11]);
  }
  else {
    warn("igi extent total cells=%d.",igiext[0]+igiext[1]+1);
  }

/* Get cdp range. */

  if(is3d>0) {
    minigi = 1;
    maxigi = (int) (0.1 + gvals[12]);
    minigc = 1;
    maxigc = (int) (0.1 + gvals[13]);
  }
  else {
    minigi = -2000000000; /* not -2147483645 (to leave room for igiext[0])  */
    maxigi = 2000000000;  /* not 2147483645 (to leave room for igiext[1])   */
    minigc = 1;
    maxigc = 1;
  }

/* Now that defaults are available, read igiout and igcout parameters.      */

  if(numlisti==0) {
    igiout = ealloc1int(3);
    igiout[0] = minigi;
    igiout[1] = maxigi;
    igiout[2] = 1;
    if(countparval("igiout")>0) getparint("igiout",igiout);
    if(igiout[2]<1) err("error: igiout= increment is less than 1");
    if(is3d>0) {
      if(igiout[0]<minigi) err("error: igiout= first is less than minimum grid igi index %d",minigi);
      if(igiout[1]>maxigi) igiout[1] = maxigi;
    }
    if(igiout[0]>igiout[1]) err("error: igiout= first is greater than last. ");
    if(igiext[0]+igiext[1]>=igiout[2]) err("error: igiext= total igi extent must be less than igiout increment.");

/* Make some range and extent adjustments for more-convenient ifs. Note that minigi,maxigi can actually */
/* end-up outside the grid. That is deliberate. It means edge super-bins will be a-symmetric (but user  */
/* can do that intentionally anyway, and it can happen elsewhere UNintentionally due to lack-of-traces).*/

    minigi = igiout[0] - igiext[0];    /* in next line, force non-optimize of multiply/divide by igiout[2] */
    maxigi = igiout[0] + igiout[2] * (int)((igiout[1]-igiout[0]+0.1)/igiout[2]) + igiext[1]; 
    igiext[0] = igiout[2] - igiext[0] - 1;
    igiext[1] = igiext[1] + 1;
  }
  else {
    mink = 2000000000; 
    maxk = -2000000000;
    for(i=0; i<numlisti; i++) {  
      if(listi[i]<mink) mink = listi[i];
      if(listi[i]>maxk) maxk = listi[i];
      for(j=i+1; j<numlisti; j++) { /* check they do not overlap */ 
        if(abs(listi[i]-listi[j]) < igiext[0] + igiext[1] + 1) {
          err("error: igilist= has values that are too close to each other (extents overlap)");
        }
      }
    }
    if(is3d>0) {
      if(mink<minigi) err("error: igilist= has value less than minimum grid igi index %d",minigi);
      if(maxk>maxigi) err("error: igilist= has value greater than maximum grid igi index %d",maxigi);
    }
    minigi = mink - igiext[0];
    maxigi = maxk + igiext[1];
    igiext[0] = igiext[0] + 1;
    igiext[1] = igiext[1] + 1;
  }

  if(is3d>0) {
    if(numlistc==0) {
      igcout = ealloc1int(3);
      igcout[0] = minigc;
      igcout[1] = maxigc;
      igcout[2] = 1;
      if(countparval("igcout")>0) getparint("igcout",igcout);
      if(igcout[2]<1) err("error: igcout= increment is less than 1");
      if(is3d>0) {
        if(igcout[0]<minigc) err("error: igcout= first is less than minimum grid igc index %d",minigc);
        if(igcout[1]>maxigc) igcout[1] = maxigc;
      }
      if(igcout[0]>igcout[1]) err("error: igcout= first is greater than last. ");
      if(igcext[0]+igcext[1]>=igcout[2]) err("error: igcext= total igc extent must be less than igcout increment.");

      minigc = igcout[0] - igcext[0]; 
      maxigc = igcout[0] + igcout[2] * (int)((igcout[1]-igcout[0]+0.1)/igcout[2]) + igcext[1]; 
      igcext[0] = igcout[2] - igcext[0] - 1;
      igcext[1] = igcext[1] + 1;
    }
    else {
      mink = 2000000000; 
      maxk = -2000000000;
      for(i=0; i<numlistc; i++) { 
        if(listc[i]<mink) mink = listc[i];
        if(listc[i]>maxk) maxk = listc[i];
        for(j=i+1; j<numlistc; j++) { /* check they do not overlap */ 
          if(listc[i]-listc[j] < igcext[0] && listc[j]-listc[i] < igcext[1]) {
            err("error: igclist= has values that are too close to each other (extents overlap)");
          }
        }
      }
      if(mink<minigc) err("error: igclist= has value less than minimum grid igc index %d",minigc);
      if(maxk>maxigc) err("error: igclist= has value greater than maximum grid igc index %d",maxigc);
      minigc = mink - igcext[0];
      maxigc = maxk + igcext[1];
      igcext[0] = igcext[0] + 1;
      igcext[1] = igcext[1] + 1;
    }
  }

  getparstring("qout", &Qname);

  checkpars(); 

/* ---Finished parameter input and checking. Produce qout file? --    */

  if(Qname != NULL) {
    fpQ = fopen(Qname, "w");
    if (fpQ == NULL) err("qout error: file did not open correctly.");
    fprintf(fpQ,"C_SU_SETID,Q\nC_SU_FORMS\nC_SU_ID");
    if(is3d>0) { /* replicate what subinqcsv does with the C_SU_FORM formats */
      fprintf(fpQ,"%s",",%.20g,%.20g,%.20g,%.20g,%.2f,%.2f,%.2f,%.2f");
    }
    else {
      fprintf(fpQ,"%s",",%.20g,%.20g");
    }
    fprintf(fpQ,"\nC_SU_NAMES\nC_SU_ID");
    if(is3d>0) {
      fprintf(fpQ,"%s",",cdp,cdpt,igi,igc,gx,gy,sx,sy");
    }
    else {
      fprintf(fpQ,"%s",",cdp,cdpt");
    }
    fprintf(fpQ,"\n");

    if(numlisti>0) nlisti = numlisti; 
    else nlisti = (igiout[1]-igiout[0])/igiout[2] + 1;

    if(is3d>0) {

      if(numlistc>0) nlistc = numlistc; 
      else nlistc = (igcout[1]-igcout[0])/igcout[2] + 1;

      for(i=0; i<nlisti; i++) {
        if(numlisti>0) ngi = listi[i];
        else ngi = igiout[0] + i*igiout[2];
        for(j=0; j<nlistc; j++) {
          if(numlistc>0) ngc = listc[j];
          else ngc = igcout[0] + j*igcout[2];
          gridiccdp(gvals,ngi,ngc,&icdp);
          gridiccdp90(gvals,ngi,ngc,&icdpt);
          gridicgridxy(gvals,ngi,ngc,&xg,&yg);
          gridicrawxy(gvals,ngi,ngc,&dx,&dy);
          fprintf(fpQ,"Q,%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f",icdp,icdpt,ngi,ngc,xg,yg,dx,dy);
          fprintf(fpQ,"\n");
        } 
      } 
    }
    else {
      for(i=0; i<nlisti; i++) {
        if(numlisti>0) ngi = listi[i];
        else ngi = igiout[0] + i*igiout[2];
        fprintf(fpQ,"Q,%d,1",ngi);
        fprintf(fpQ,"\n");
      }
    }

  }

/* ---Start processing traces.--------------------------------    */

  if (!gettr(&tr))  err("Error: cannot get first trace");

/* loop over traces   */ 

  do {

    nproct++;

/* Compute cdp,igi,igc from coordinates (actual things needed are igi,igc).   */

    if(intype==3) {

      dx = 0.5 * (double)(tr.sx + tr.gx);
      dy = 0.5 * (double)(tr.sy + tr.gy);

      if(tr.scalco > 1) { 
        dx *= tr.scalco;
        dy *= tr.scalco;
      }
      else if(tr.scalco < 0) { 
        dx /= -tr.scalco;
        dy /= -tr.scalco;
      }

      gridrawxycdpic(gvals,dx,dy,&icdp,&igi,&igc);
      if(icdp<-2147483644) 
        err("Error: input midpoint XYs not in grid (cannot compute cdp number). Trace= %d",nproct);
    }

    else if(intype==2) { /* get igi,igc from cdp key */
      icdp = tr.cdp;
      gridcdpic(gvals,icdp,&igi,&igc);
      if(igi<-2147483644) 
        err("Error: input cdp number not in grid (cannot compute igi,igc). Trace= %d",nproct);
    } 

    else if(intype==1) { /* get cdp from igi,igc keys */
      igi = tr.igi;
      igc = tr.igc;
      gridiccdp(gvals,igi,igc,&icdp); 
      if(icdp<-2147483644)  
        err("Error: input igi,igc numbers not in grid (cannot compute cdp).      Trace= %d",nproct);
    } 

    else if(intype==-2) { /* for 2d data, use cdp key as igi number */
      igi = tr.cdp;
    }

    if(igi<minigi || igi>maxigi || igc<minigc || igc>maxigc) continue;

    keepi = 0;

/* If user specified a range of igi indexes, modulo to get/reset igi.                */ 

    if(numlisti==0) {
      modi = (igi-igiout[0]) % igiout[2];
      if(modi > igiext[0]) {
        keepi = 1;
        igi   = igi + igiout[2] - modi;
      }
      else if(modi < igiext[1]) {
        keepi = 1;
        igi   = igi - modi;
      }
    }

/* If the user specified a list of igi indexes, just find it within extents and reset.*/ 
/* Note that listi has already been checked to make sure that extents do not overlap. */ 
/* (I am presuming numlisti is small, otherwise would sort listi and binary search).  */ 

    else {
      for(i=0; i<numlisti; i++) {
        if(listi[i]-igi < igiext[0] && igi-listi[i] < igiext[1]) {
          keepi = 1;
          igi = listi[i];
          break;
        } 
      }
    }

    if(intype!=-2) {
      keepc = 0;
      if(numlistc==0) {
        modc = (igc-igcout[0]) % igcout[2];
        if(modc > igcext[0]) {
          keepc = 1;
          igc   = igc + igcout[2] - modc;
        }
        else if(modc < igcext[1]) {
          keepc = 1;
          igc   = igc - modc;
        }
      }
      else {
        for(i=0; i<numlistc; i++) {
          if(listc[i]-igc < igcext[0] && igc-listc[i] < igcext[1]) {
            keepc = 1;
            igc = listc[i];
            break;
          } 
        }
      }
    }

    if(keepi>0 && keepc>0) {
      if(intype==-2) {
        if(irecdp>0) tr.cdp = igi;
      }
      else {
        if(irecdp>0) {
          gridiccdp(gvals,igi,igc,&icdp); 
          if(icdp<-2147483644)   
            err("Error: input igi,igc numbers not in grid (cannot compute cdp). Trace= %d   %d",nproct,icdp);
          tr.cdp = icdp;
        }
        if(ireigi>0) tr.igi = igi;
        if(ireigc>0) tr.igc = igc;
      }

      puttr(&tr);
      mproct++;
    }

  } while (gettr(&tr));

  warn("Number of traces input=%d.  Number output=%d.",nproct,mproct);

  return 0;

}

