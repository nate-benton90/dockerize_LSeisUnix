/* Copyright (c) Colorado School of Mines, 2021.*/
/* All rights reserved.                       */

/* SUNMOCSV: $Revision: 1.2 $ ; $Date: 2022/01/10 11:01:01 $		*/
 
#include "su.h"
#include "segy.h" 
#include "qdefine.h"
#include "gridread.h"
#include "gridxy.h"
#include "bilinear.h"
#include "linterpd.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"									     ",
" SUNMOCSV - NMO for an arbitrary velocity function of time and 3D or 2D CDP ",
"									     ",
"  sunmocsv <stdin >stdout [optional parameters]			     ",
"									     ",
" Optional Parameters:							     ",
"									     ",
" qin=                  Velocity functions can be input via this file.       ",
"                       This file is optional, but if you do not input it,   ",
"                       you must use parameters cdp=, tnmo=, vnmo=.          ",
"                       See external document Q_FILE_STANDARDS.              ",
"									     ",
"                       The following 3 parameters cannot be specified if    ",
"                       you input velocity functions via the qin= file.      ",
"									     ",
" cdp=		        CDPs for which vnmo & tnmo are specified.            ",
" tnmo=                 NMO times corresponding to velocities in vnmo.       ",
"                       If any tnmo value is greater than 100 they are all   ",
"                       assumed to be milliseconds. Otherwise seconds.       ",
" vnmo=                 NMO velocities corresponding to times in tnmo.	     ",
"                       If qin= is not specified, all 3 of these parameters  ",
"                       must be specified. There must be at least 1 number   ",
"                       in the cdp= list. There must be the same number of   ",
"                       vnmo= parameters as numbers in the cdp= list.        ",
"                       There must be the same number of tnmo= parameters    ",
"                       as numbers in the cdp= list, or, there can be just   ",
"                       one tnmo= parameter provided there are the same      ",
"                       number of velocities in all vnmo= lists.             ",
"									     ",
" smute=1.5             samples with NMO stretch exceeding smute are zeroed  ",
" lmute=25		length (in samples) of linear ramp for stretch mute  ",
" sscale=1		=1 to divide output samples by NMO stretch factor    ",
" invert=0		=1 to perform (approximate) inverse NMO		     ",
" upward=0		=1 to scan upward to find first sample to kill	     ",
" voutfile=		if set, interplolated velocity function v[cdp][t] is ",
"			output to named file.			     	     ",
"									     ",
" rfile=                If set, read a K-file containing 3D grid definition. ", 
"                       Assume 2D if no K-file and no grid definition is     ", 
"                       supplied via command line parameters. The required   ", 
"                       command line parameters are: grid_xa,grid_ya,        ", 
"                       grid_xb,grid_yb, grid_xc,grid_yc, grid_wb, grid_wc.  ", 
"                       (See program SUBINCSV for 3D grid details).          ", 
"                       If this is a 3D then the input CDPs for the velocity ", 
"                       functions and the seismic trace CDP key should be    ", 
"                       3D grid cell numbers as produced by SUBINCSV.        ", 
"                       A 3D also forces restrictions on the locations of the", 
"                       input velocity functions. Their CDP locations must   ", 
"                       form aligned rectangles (see Notes).                 ", 
"									     ",
" check=0               Do not print grid checking and function locations.   ",
"                       =1 If grid definition input, run some grid functions ",
"                          on the 4 grid corner points and print results.    ",
"                          Also print input velocity function location       ",
"                          information by using the grid definition and      ",
"                          the input cdp= values. The format and values are: ",
"                            G,cdp,igi,igc,xgrid,ygrid,xworld,yworld.        ",
"                          (The format is intended to make it easy to see    ",
"                          and also to make it easy to load to spreadsheets).",
"                          This information can be written to a file by      ",
"                          putting 2>yourfile on the command line.           ",
"									     ",
" print=0               Do not print INPUT velocity functions.               ",
"                       =1 Print INPUT velocity functions. The cdp number    ",
"                          and its input velocity function are printed.      ",
"                          This print is not intended to be pretty. It just  ",
"                          allows easy checking that the q-file contains     ",
"                          the expected values (or just use a text editor).  ",
"                          It also allows programmers and others to confirm  ",
"                          that command line input via cdp=,tnmo=,vnmo=      ",
"                          is the same as identical input via the qin= file. ",
"									     ",
" Notes:								     ",
"									     ",
" NMO interpolation error is less than 1% for frequencies less than 60% of   ",
" the Nyquist frequency.						     ",
"									     ",
" Exact inverse NMO is impossible, particularly for early times at large     ",
" offsets and for frequencies near Nyquist with large interpolation errors.  ",
" 								     	     ",
" The \"offset\" header field must be set.				     ",
" Use suazimuth to set offset header field when sx,sy,gx,gy are all	     ",
" nonzero. 							   	     ",
"									     ",
" The times specified in tnmo= parameters must be monotonically increasing.  ",
" Velocities can also be input via the qin= file (see C_QFILE_STANDARDS).    ",
"									     ",
" Velocities at each input cdp are interpolated to the sample times and then ",
" spatial interpolation is done, as explained next:                          ",
"									     ",
" For 3D, the user needs to input velocity functions which form aligned      ",
" rectangles. That is, how-ever-many grid inlines the user chooses to put    ",
" velocity functions on, there must always be the same number of functions   ",
" on each inline and those functions must be located at same grid crosslines.",
" For instance, if user inputs functions for inline 7 at crosslines 15,25,40 ",
" then the user must input the functions at crosslines 15,25,40 for any other",
" inlines that the user wants to supply functions for. (If user is lucky     ",
" enough that the grid has 100 inlines, then the input functions could be at ",
" CDPs 715,725,740 and 1115,1125,1140 and 2015,2025,2040 and 2515,2525,2540. ",
" Note that the CDP numbers specified on the cdp= parameter and also in the  ",
" input seismic trace cdp key are translated to inline and crossline numbers ",
" using the input 3D grid definition - so those cdp numbers need to          ",
" correspond to the input 3D grid definition.                                ",
"									     ",
" For CDPs not listed in cdp= parameter or qin= file, bilinear interpolation ",
" of 1/velocity_squared is done if CDP is surrounded by 4 input functions,   ",
" linear interpolation of 1/velocity_squared is done if the CDP is outside   ",
" the sides of fully surrounded area, constant extrapolation if the CDP is   ",
" outside the corners of fully surrounded area. If functions are only input  ",
" along 1 inline (or 1 crossline) then the result is linear interpolation    ",
" of 1/velocity_squared along that direction (and constant beyond ends).     ",
"									     ",
" The format of the output interpolated velocity file is unformatted C floats",
" with vout[cdp][t], with time as the fast dimension and may be used as an   ",
" input velocity file for further processing.				     ",
"									     ",
NULL};

/* Credits:
 *	SEP: Shuki Ronen, Chuck Sword
 *	CWP: Shuki Ronen, Jack, Dave Hale, Bjoern Rommel
 *      Modified: 08/08/98 - Carlos E. Theodoro - option for lateral offset
 *      Modified: 07/11/02 - Sang-yong Suh -
 *	  added "upward" option to handle decreasing velocity function.
 *      CWP: Sept 2010: John Stockwell
 *	  1. replaced Carlos Theodoro's fix 
 *	  2. added  the instruction in the selfdoc to use suazimuth to set 
 *	      offset so that it accounts for lateral offset. 
 *        3. removed  Bjoren Rommel's anisotropy stuff. sunmo_a is the 
 *           version with the anisotropy parameters left in.
 *        4. note that scalel does not scale the offset field in
 *           the segy standard.
 *        Modified Sept 2021: Andre Latour   
 *	  1. added 3D option (bilinear interpolation of velocity functions).
 *           This is activated by input of a 3D grid definition.                   
 *           See the SUBINCSV program for grid definition details.               
 *        Modified Jan  2022: Andre Latour   
 *	  1. Reworked to use lib routines to get velocity function values
 *           either from command line parameters or from input q-files.      
 *	  2. Reworked to use lib routines for bilinear interpolation. 
 *        Modified Apr  2022: Andre Latour   
 *	  1. changed so that if any tnmo value is greater than 100 all tnmo   
 *	     values are assumed to be milliseconds. Otherwise seconds.
 * Technical Reference:
 *	The Common Depth Point Stack
 *	William A. Schneider
 *	Proc. IEEE, v. 72, n. 10, p. 1238-1254
 *	1984
 *
 * Trace header fields accessed: ns, dt, delrt, offset, cdp, scalel
 */
/**************** end self doc *******************************************/

segy tr;

struct QInfo *VInfo; /* All velocity function values are stored herein. */

int compSort2 (const void * q1, const void * q2) ; /* comparison function for qsort  */

int main(int argc, char **argv) {

/* ------------------------------------------------------------------- */
/* NOTE: In this code I make an effort to conform to the original      */
/*       indentation style of sunmo.c (even tho I find it awkward).    */
/* ------------------------------------------------------------------- */

	int nt;			/* number of time samples per trace */
	float dt;		/* time sampling interval */
	float ft;		/* time of first sample */
	int it;			/* time sample index */

	int ncdp;		/* number of cdps specified */
	int icdp;		/* index into cdp array */
	int jcdp;		/* index into cdp array */

	float *ovvt=NULL;	/* array[nt] of sloth for a particular trace */

	float smute;		/* zero samples with NMO stretch exceeding */
				/*  smute */
	float osmute;		/* 1/smute */
	int lmute;		/* length in samples of linear ramp for mute */
	int itmute=0;		/* zero samples with indices less than itmute */
	int sscale;		/* if non-zero, apply NMO stretch scaling */
	int invert;		/* if non-zero, do inverse NMO */
	
	long oldoffset;		/* offset of previous trace */
	long oldcdp;		/* cdp of previous trace */

	int newsloth;		/* if non-zero, new sloth function was */
				/* computed */

	float tn;		/* NMO time (time after NMO correction) */
	float *qtn=NULL;	/* NMO-corrected trace q(tn) */
	float *ttn=NULL;	/* time t(tn) for NMO */
	float *atn=NULL;	/* amplitude a(tn) for NMO */
	float *qt=NULL;		/* inverse NMO-corrected trace q(t) */
	float *tnt=NULL;	/* time tn(t) for inverse NMO */
	float *at=NULL;		/* amplitude a(t) for inverse NMO */

	float temp;		/* temporary float */
	float tsq;		/* temporary float */
	int upward;		/* scans upward if it's nonzero. */

	float offset;		/* value of offset honoring scalel */

	char *voutfile="";	/* name of interpolated output vel file */
	FILE *voutfp=NULL;	/* ... its file pointer */
	cwp_Bool isvoutfile=cwp_false; /* is output vel file specified? */
	float voutt[1];	/* output velocities */

        cwp_String Rname=NULL;  /* text file name for values            */
        FILE *fpR=NULL;         /* file pointer for Rname input file    */
        double gvals[999];      /* to contain the grid definition       */

        cwp_String *pname = NULL; /* to hold the names of values we want */
        int numpname = 3;         /* number of values that we want       */
        cwp_String *ndims = NULL; /* for an unused return argument       */

        int ifixd=0;            /* flag for all tuples same size or vary   */
        int iztuple=0;          /* element number where first tuple exists */
        int ktuple=0;           /* type of tuples (2=pairs, 3=triplets)    */

        cwp_String Qname=NULL;  /* text file name for Q input file      */
        FILE *fpQ=NULL;         /* file pointer for Q input file        */

        int mgiextr=0;          /* if outside covered area, hold velocities constant in inline direction */
        int mgcextr=0;          /* if outside covered area, hold velocities constant in xline direction */
        int mgtextr=0;          /* if above or below covered time range, hold velocities constant */

        int maygrid=0;          /* to check if a 3d grid definition exists or not */
        int is3d=1;             /* 3d flag */
        int kigi=0;             /* 3d inline index */
        int kigc=1;             /* 3d crossline index. init to 1 incase this is a 2D */ 

	int i=0;                /* the rest of these variables are either trivial or so */
	int j=0;                /* entangled that no short comment up here will help    */
	int errwarn=0;
	int jnloc1=0;
        double *pindepa = NULL;  
        double *dind = NULL;
        double *dswap = NULL;
        int mgi_tot = -1;
        int mgc_tot = 0;
        int *mgi = NULL;
        int *mgc = NULL;
        int mgi_totdeg = 0;
        int mgix = 0;
        int mgcx = 0;
        int ndxi = 0;
        int ndxc = 0; 
        int mdxi = 0;
        int mdxc = 0;
        double wi = 0.;
        double wc = 0.;
        int icheck=0;
        int iprint=0;
	
/* ------------------------------------------------------------------- */
	/* hook up getpar */
	initargs(argc, argv);
	requestdoc(1);

        getparstring("qin", &Qname);
        getparstring("rfile", &Rname);
	getparstring("voutfile",&voutfile);

	if (!getparfloat("smute",&smute)) smute = 1.5;
	if (smute<=0.0) err("error: smute must be greater than 0.0");
	if (!getparint("lmute",&lmute)) lmute = 25;
	if (!getparint("sscale",&sscale)) sscale = 1;
	if (!getparint("invert",&invert)) invert = 0;
	if (!getparint("upward",&upward)) upward = 0;
        if (!getparint("check", &icheck)) icheck = 0;
        if (!getparint("print", &iprint)) iprint = 0;

/* Process and set the grid definition values?                         */
      
        gridcommand(&maygrid);
      
        if(maygrid==1  && Rname != NULL) err("error: input k-file not allowed when full grid on command line.");
        if(maygrid==-1 && Rname == NULL) err("error: input k-file required when partial grid on command line.");
        if(maygrid==0  && Rname == NULL) is3d = 0;

        if(is3d==1) {

                if(maygrid!=1) { /* open if not full grid on command line (else pass fpR still NULL) */
                        fpR = fopen(Rname, "r");
                        if(fpR==NULL) err("error: input K-file did not open correctly.");
                }

                int errwarn = 1; /* print if error or unusual thing inside gridread */
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

	/* get information from the first header */
	if (!gettr(&tr)) err("error: can't get first trace");
	nt = tr.ns;
	dt = ((double) tr.dt)/1000000.0;
	ft = tr.delrt/1000.0;

	/* allocate workspace */

	ovvt = ealloc1float(nt);
	ttn = ealloc1float(nt);
	atn = ealloc1float(nt);
	qtn = ealloc1float(nt);
	tnt = ealloc1float(nt);
	at = ealloc1float(nt);
	qt = ealloc1float(nt);
        dind = ealloc1double(nt);
        dswap = ealloc1double(nt);

	/* if specified, open output velocity file */
	if (*voutfile!='\0') {
		isvoutfile=cwp_true;
		if((voutfp=fopen(voutfile,"w"))==NULL)
                  err("error: cannot open voutfile=%s\n",voutfile);
	}

/* Compute the time array (the time at each sample). */

        for (i=0; i<nt; ++i) dind[i] = ft + i*dt;

/* Set parameters and names of the 3 values needed from getviaCommand or getviaqfile. */

        ktuple   = 0;
        iztuple  = 1;
        numpname = 3;
        pname = ealloc1(numpname,sizeof(cwp_String *));
        for(j=0; j<numpname; j++) pname[j] = ealloc1(4,1);
        strcpy(pname[0],"cdp");
        strcpy(pname[1],"tnmo");
        strcpy(pname[2],"vnmo");

        if(Qname == NULL) { /* no q-file, so readin velocity functions via command line parameters */

                getviacommand(&pname, &numpname, &iztuple, nt,
                              &ktuple, &ifixd, &VInfo, &ncdp,
                              &pindepa, &ndims, &errwarn) ;

                if(errwarn==1) err("getviacommand error: no non-tuple name passed in.");
                else if(errwarn==2) err("getviacommand error: non-tuple names have different amounts of values.");
                else if(errwarn==3) err("getviacommand error: independent dimension parameter is empty.");
                else if(errwarn==4) err("getviacommand error: an independent dimension parameter is empty.");
                else if(errwarn==5) err("getviacommand error: members of tuple have different amounts at same location."); 
                else if(errwarn>0) err("getviacommand error: returned unrecognized error number = %d",errwarn);

                if(ifixd==2 || iztuple!=1 || ktuple!=2) 
                  err("error: command line does not contain cdp,tnmo,vnmo (or they are incorrect combination). ");

        }

        else { /* Read-in velocity functions via q-file? */

                fpQ = fopen(Qname, "r");
                if(fpQ==NULL) err("error: input Q-file did not open correctly.");

                getviaqfile(fpQ, &pname, &numpname, &iztuple, nt,
                            &ktuple, &ifixd, &VInfo, &ncdp,
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

                if(ifixd==2 || iztuple!=1 || ktuple!=2) 
                  err("error: q-file does not contain cdp,tnmo,vnmo names (or they are in non-standard order)");


        } /* end of else for read-in via q-file */

        checkpars(); /* put this down here since getviacommand reads parameters */

/* Note that the ifixd flag indicates what is going on with the INPUT    */
/* q-records or parameter sets. The indepenent dimension name may be in  */
/* pname list (if ifixd=0 the times are actually in each q-record along  */
/* with the velocities, or times are in multiple tnmo= parameters).      */
/* For example a varying number of time,velocity pairs from velocity     */
/* analysis (velans). But if ifixd=1 there is only 1 set of time values  */
/* either in the tnmo= parameter or in the C_SU_NDIMS record in q-file.  */
/* But this does not matter since pname is not used (much) after here.   */
/*                                                                       */
/*      if(ifixd==0) { instead need to get rid of tnmo for ifixd=1?      */
/*        for(k=iztuple; k<iztuple+ktuple-1; k++) pname[k] = pname[k+1]; */
/*      }                                                                */
/*                                                                       */
/* Set jnloc1 to the location of the cdp values within dlots.            */
/* Note that, in this program version, we know that cdp is at dlots[0].  */
/* But we may want to add options to use other values (like 3d indexes)  */
/* such as for subinqcsv.c inloc parameter (would be nice to use same    */
/* option values in this program as for inloc parameter of subinqcsv.c)  */

        jnloc1 = -1;
        for (i=0; i<iztuple; ++i) {
                if(strcmp(pname[i],"cdp")==0) {
                        jnloc1 = i;
                        break;
                }
        }

        if(jnloc1!=0) err("error: input must have cdp (before tnmo,vnmo).");

/* Copy cdp number to kinf[0] and also set kinf[1] and kinf[2].     */
/* Later, kinf[1] and kinf[2] are what qsort actually sorts on.     */
/* For 3D, kinf[1] and kinf[2] are inline and crossline 3d indexes. */
/* For 2D, we just set kinf[1] to cdp number, and kinf[2] to 1.     */

        for (icdp=0; icdp<ncdp; ++icdp) VInfo[icdp].kinf = ealloc1int(3);

        if(is3d>0) {
                for (icdp=0; icdp<ncdp; ++icdp) {
                        VInfo[icdp].kinf[0] = lrint(VInfo[icdp].dlots[jnloc1]);
                        gridcdpic(gvals,VInfo[icdp].kinf[0],VInfo[icdp].kinf+1,VInfo[icdp].kinf+2);
                        if(VInfo[icdp].kinf[1] < -2147483644)
                          err("error: input cdp %d is not in grid",VInfo[icdp].kinf[0]);
                }
        }
        else {
                for (icdp=0; icdp<ncdp; ++icdp) {
                        VInfo[icdp].kinf[0] = lrint(VInfo[icdp].dlots[jnloc1]);
                        VInfo[icdp].kinf[1] = VInfo[icdp].kinf[0]; /* set these for 2d */
                        VInfo[icdp].kinf[2] = 1;                   /* set these for 2d */
                }
        }

/* The following iprint option is not intended to be pretty. For the users it usually just   */
/* allows a quick look to confirm they have the correct q-file (or, just use a text editor). */
/* For you the programmer, it presents simple code to see where the time and velocity values */
/* are stored by getviacommand and getviaqfile. Note in particular that velocities always    */
/* start at dlots[iztuple] for each cdp. The times are either stored AFTER all velocities    */
/* for each cdp, or just ONCE at pointer pindepa (if ifixd flag is 1).                       */
/* So, yes, an input ifixd=0 q-file has values in time,vel pair order, but they have been    */
/* de-multiplexed into all vels at that cdp followed by all times at that cdp (i.e. arrays). */

        if(iprint>0) {

                if(ifixd==0) {
                        for(jcdp=0; jcdp<ncdp; jcdp++) {
                                warn(" cpd= %d   Number of time,vel pairs= %d",VInfo[jcdp].kinf[0],VInfo[jcdp].nto);
                                pindepa = VInfo[jcdp].dlots+iztuple+VInfo[jcdp].nto;
                                for(i=0; i<VInfo[jcdp].nto; i++) warn(" %f %f ",pindepa[i],VInfo[jcdp].dlots[iztuple+i]);
                        }
                }
                else if(ifixd==1) {
                        warn(" Number of times= %d",VInfo[0].nto);
                        for(i=0; i<VInfo[0].nto; i++) warn(" %f ",pindepa[i]);
                        for(jcdp=0; jcdp<ncdp; jcdp++) {
                                warn(" cpd= %d   Number of vels= %d",VInfo[jcdp].kinf[0],VInfo[jcdp].nto);
                                for(i=0; i<VInfo[0].nto; i++) warn(" %f ",VInfo[jcdp].dlots[iztuple+i]);
                        }
                }

        } /* end of  if(iprint>0) { */

/* Check that independent dimension values are input in increasing order.   */
/* Note that we could just sort the tuples into increasing order ourselves. */
/* But that is far too likely to induce errors. One mis-typed value in the  */
/* input and the results will be subtly bad (and, if you don't know by now, */
/* subtly bad is the worst-kind-of-bad in seismic data processing).         */

        j = 0;
        if(ifixd==0) {
                for(jcdp=0; jcdp<ncdp; jcdp++) {
                        pindepa = VInfo[jcdp].dlots+iztuple+VInfo[jcdp].nto;
                        for(i=1; i<VInfo[jcdp].nto; i++) {
                                if(pindepa[i-1] >= pindepa[i])
                                  err("error: tnmo values are not increasing (input cdp = %d)",VInfo[jcdp].kinf[0]);
                        }
                        if(pindepa[VInfo[jcdp].nto-1] > 100.) j = 1; 
                }
        }
        else if(ifixd==1) {
                for(i=1; i<VInfo[0].nto; i++) {
                        if(pindepa[i-1] >= pindepa[i]) err("error: tnmo values are not increasing.");
                }
                if(pindepa[VInfo[0].nto-1] > 100.) j = 1; 
        }

/* If input times are milliseconds, convert them to seconds for use herein.                       */

        if(j==1) {
                if(ifixd==0) {
                        for(jcdp=0; jcdp<ncdp; jcdp++) {
                                pindepa = VInfo[jcdp].dlots+iztuple+VInfo[jcdp].nto;
                                for(i=0; i<VInfo[jcdp].nto; i++) pindepa[i] *= 0.001;
                        }
                }
                else if(ifixd==1) {
                        for(i=0; i<VInfo[0].nto; i++) pindepa[i] *= 0.001;
                }
        }

/* For bilinear interpolation, user must input function locations which form aligned rectangles.  */
/* That is, howevermany inlines the user chooses to put functions on, there must be the same      */
/* number of functions on each inline and those functions must be located at the same crosslines. */
/* For instance, if user inputs functions for inline 7 at crosslines 15,25,40 then the user must  */
/* input the functions at crosslines 15,25,40 for any other inlines that the user wants to supply */
/* functions for. The qsplit function enforces that restriction on the user input - and also      */
/* separates the igi and igc values into 2 simple arrays.                                         */
/*   The qsplit function expects the values to be sorted by igi,igc values.                       */
/*   The results of qsplit are then used by binterpfind and binterpapply.                         */

        qsort(VInfo,ncdp,sizeof(struct QInfo),compSort2);

        for (jcdp=1; jcdp<ncdp; ++jcdp) {
                if(VInfo[jcdp-1].kinf[1] == VInfo[jcdp].kinf[1] &&
                   VInfo[jcdp-1].kinf[2] == VInfo[jcdp].kinf[2]) {
                        err("error: Two sets of values input for cdp,igi,igc = %d %d %d",
                        VInfo[jcdp].kinf[0],VInfo[jcdp].kinf[1],VInfo[jcdp].kinf[2]);
                }
        }

/* (Note: qsort is a standard c function, qsplit is a function in su/lib/qdefine.c). */

        qsplit(VInfo,ncdp,&mgi,&mgi_tot,&mgc,&mgc_tot,&errwarn);
        if(errwarn>0) err("error: Input cdps do not form enclosed-rectangles (needed for bilinear).");

        mgi_totdeg = mgi_tot; /* read explanation of mgi_totdeg later */
        if(mgi_tot==1 || mgc_tot==1) mgi_totdeg = 0;

/* Change the stored dependent dimension(s) values in dlots to values at dind. */
/* (for a more generalized situation, use qelementout as seen in subinqcsv.c). */

        for(jcdp=0; jcdp<ncdp; jcdp++) {

                if(ifixd==0) pindepa = VInfo[jcdp].dlots+iztuple+VInfo[jcdp].nto;

                for(it=0; it<nt; it++) {
                        linterpd(dind[it],pindepa,VInfo[jcdp].dlots+iztuple,
                                 VInfo[jcdp].nto,mgtextr,dswap+it);

/* Might as well square-and-inverse the velocity values here.        */
/* Note the allowing of negative velocities. Could just error-halt.  */
/* But in some situations, extrapolation causes negative velocities  */
/* so far away from the actual seismic that they are never used. So? */

                        if(dswap[it] > 1.e-98) dswap[it] = 1./(dswap[it] * dswap[it]);
                        else if(dswap[it] < -1.e-98) dswap[it] = -1./(dswap[it] * dswap[it]);

                } /* end of  for(i=0; i<nt; i++) { */

/* Overwrite the input values with the (squared-inversed) velocity values. */
/* Note that nt was passed to getviacommand and getviaqfile so that they   */
/* knew to allocate enough memory for this.                                */

                for(it=0; it<nt; it++) VInfo[jcdp].dlots[it+iztuple] = dswap[it];

        }

	/* set old cdp and old offset nicely to trigger on first loop */
	oldcdp = tr.cdp-1;        
	oldoffset = tr.offset-1;                                         
	newsloth = 0;            /* set to 0 incase ncdp<2 */

        if(ncdp<2) { /* copy the only velocity function to ovvt */
      	        for (it=0; it<nt; ++it) ovvt[it]=VInfo[0].dlots[it+iztuple];

	        if(isvoutfile) { /* if specified, output the only velocity  */
		        for (it=0; it<nt; ++it) {	
	       			float veltemp=ovvt[it];
	        		voutt[0]=sqrt(1.0/veltemp);
		        	efwrite(voutt,FSIZE,1,voutfp);
		        }
	        }
        }

	/* loop over traces */

	do {
		/* if necessary, compute new sloth and anis function */
		if (tr.cdp!=oldcdp) {

                        if(ncdp>1) {

                                if(is3d==1) { 
                                        gridcdpic(gvals,tr.cdp,&kigi,&kigc);
                                        if(kigi<-2147483644) err("Error: input cdp= %d not in grid.",tr.cdp); 
                                }
                                else kigi = tr.cdp;

/* find input cdp (higher) locations mgix,mgcx and weights wi,wc */

                                binterpfind(kigi,mgi,mgi_tot,mgiextr,kigc,mgc,mgc_tot,mgcextr,
                                            &mgix,&mgcx,&wi,&wc);

/* mgix and mgcx are the locations computed for each direction seperately.     */
/* mgix and mgcx are always returned as the highest of the 2 (near) locations. */
/* (if mgi has 10 locations, mgix is only returned from 1 to 9, never 0).      */
/* So, the 4 locations are: mgix,mgcx  mgix-1,mgcx  mgix,mgcx-1  mgix-1,mgcx-1.*/
/* For those 4, compute the element numbers of the stored functions in VInfo.  */
/* Note that for the degenerate cases of mgi_tot=1 or mgc_tot=1 the            */
/* mgi_totdeg=0, which results in ndxc=ndxi, which in turn means the second    */
/* two functions passed to binterpapply are the same as first two (which works */
/* because either weight wi or wc will be 0.0).                                */

                                ndxi = mgix + mgi_tot * (mgcx-1);
                                ndxc = ndxi + mgi_totdeg;
                                mdxi = ndxi-1;
                                mdxc = ndxc-1;

                                binterpapply(VInfo[mdxi].dlots+iztuple, VInfo[ndxi].dlots+iztuple, mgi_tot, wi,
                                             VInfo[mdxc].dlots+iztuple, VInfo[ndxc].dlots+iztuple, mgc_tot, wc,
                                             nt,dswap);

      		                for (it=0; it<nt; ++it) ovvt[it]=dswap[it];

			        newsloth = 1;
			/* if specified, output velocity for each cdp to a file */
			        if(isvoutfile) {
				        for (it=0; it<nt; ++it) {	
		        			float veltemp=ovvt[it];
			        		voutt[0]=sqrt(1.0/veltemp);
				        	efwrite(voutt,FSIZE,1,voutfp);
				        }
			        }
                        } /* end of  if(ncdp>1) { */
		} else {
			newsloth = 0;
		}

		/* if sloth function or offset has changed */
		if (newsloth || tr.offset!=oldoffset) {

			offset = (float) (tr.offset);

			/* compute time t(tn) (normalized) */
			temp = ((float) offset*offset)/(dt*dt);
			for (it=0,tn=ft/dt; it<nt; ++it,tn+=1.0) {
				tsq = temp*ovvt[it];
				ttn[it] = sqrt (tn*tn + tsq);
			}
			/* compute inverse of stretch factor a(tn) */
			atn[0] = ttn[1]-ttn[0];
			for (it=1; it<nt; ++it)
				atn[it] = ttn[it]-ttn[it-1];
			
			/* determine index of first sample to survive mute */
			osmute = 1.0/smute;
			if(!upward) {
				for (it=0; it<nt-1 && atn[it]<osmute; ++it);
			} else {
				/* scan samples from bottom to top */
				for (it=nt-1; it>0 && atn[it]>=osmute; --it);
			}
			itmute = it;


			/* if inverse NMO will be performed */
			if (invert) {
							
				/* compute tn(t) from t(tn) */
				yxtoxy(nt-itmute,1.0,ft/dt+itmute,&ttn[itmute],
					nt-itmute,1.0,ft/dt+itmute,
					ft/dt-nt,ft/dt+nt,&tnt[itmute]);
			
				/* adjust mute time */
				itmute = 1.0+ttn[itmute]-ft/dt;
				itmute = MIN(nt-2,itmute);
								
				/* compute a(t) */
				if (sscale) {
					for (it=itmute+1; it<nt; ++it)
						at[it] = tnt[it]-tnt[it-1];
					at[itmute] = at[itmute+1];
				}
			}
		}
		
		/* if forward (not inverse) nmo */
		if (!invert) {
	
			/* do nmo via 8-point sinc interpolation */
			ints8r(nt,1.0,ft/dt,tr.data,0.0,0.0,
				nt-itmute,&ttn[itmute],&qtn[itmute]);
			
			/* apply mute */
			for (it=0; it<itmute; ++it)
				qtn[it] = 0.0;
			
			/* apply linear ramp */
			for (it=itmute; it<itmute+lmute && it<nt; ++it)
				qtn[it] *= (float)(it-itmute+1)/(float)lmute;
			
			/* if specified, scale by the NMO stretch factor */
			if (sscale)
				for (it=itmute; it<nt; ++it)
					qtn[it] *= atn[it];
			
			/* copy NMO corrected trace to output trace */
			memcpy( (void *) tr.data,
					(const void *) qtn, nt*sizeof(float));
		
		/* else inverse nmo */
		} else {
	
			/* do inverse nmo via 8-point sinc interpolation */
			ints8r(nt,1.0,ft/dt,tr.data,0.0,0.0,
				nt-itmute,&tnt[itmute],&qt[itmute]);
			
			/* apply mute */
			for (it=0; it<itmute; ++it)
				qt[it] = 0.0;
			
			/* if specified, undo NMO stretch factor scaling */
			if (sscale)
				for (it=itmute; it<nt; ++it)
					qt[it] *= at[it];
			
			/* copy inverse NMO corrected trace to output trace */
			memcpy( (void *) tr.data,
					(const void *) qt,nt*sizeof(float));
		}

		/* write output trace */
		puttr(&tr);

		/* remember offset and cdp */
		oldoffset = tr.offset;
		oldcdp = tr.cdp;

	} while (gettr(&tr));

	if (isvoutfile) efclose(voutfp);
	return(CWP_Exit());
}

/* -----------------------------------------------------------         */
/* Specify compare function for qsort.                                 */

int compSort2 (const void * q1, const void * q2) {

  struct QInfo* p1 = (struct QInfo*) q1;
  struct QInfo* p2 = (struct QInfo*) q2;

/* Note I decided so sort to igc,igi order (kinf[2], then kinf[1])     */

  if(p1->kinf[2] < p2->kinf[2]) return (-1);
  if(p1->kinf[2] > p2->kinf[2]) return (1); 
  if(p1->kinf[1] < p2->kinf[1]) return (-1);
  if(p1->kinf[1] > p2->kinf[1]) return (1); 

  return (0); 

}
