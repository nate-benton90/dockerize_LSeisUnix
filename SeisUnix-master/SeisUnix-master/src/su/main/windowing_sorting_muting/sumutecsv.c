/* Copyright (c) Colorado School of Mines, 2021.*/
/* All rights reserved.                       */

/* SUMUTECSV: $Revision: 1.3 $ ; $Date: 2022/03/26 02:50:15 $		*/
 
#include "su.h"
#include "segy.h" 
#include "qdefine.h"
#include "gridread.h"
#include "gridxy.h"
#include "bilinear.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"									     ",
" SUMUTECSV - MUTE above (or below) bilinearly interpolated polygonal curves ",
"									     ",
"  sumutecsv <stdin >stdout [required parameters] [optional parameters]      ",
"									     ",
" Optional Parameters:                                                       ",
"                                                                            ",
" qin=             Mute functions can be input via this file.                ",
"                  This file is optional, but if you do not input it,        ",
"                  you must use parameters cdp=, offs=, tims=.               ",
"                  See external document Q_FILE_STANDARDS.                   ",
"                                                                            ",
"                  The following 3 parameters cannot be specified if         ",
"                  you input mute functions via the qin= file.               ",
"                                                                            ",
" cdp=             CDPs for which offs & tims are specified.                 ",
" offs=            offsets corresponding to times in tims.                   ",
" tims=            times corresponding to offsets in offs.                   ",
"                  If any tims value is greater than 100 all times are       ",
"                  assumed to be milliseconds. Otherwise seconds.            ",
"            ***   If qin= is not specified, all 3 previous parameters       ",
"                  must be specified. There must be at least 1 number        ",
"                  in the cdp= list. There must be the same number of        ",
"                  tims= parameters as numbers in the cdp= list.             ",
"                  There must be the same number of offs= parameters         ",
"                  as numbers in the cdp= list, or, there can be just        ",
"                  one offs= parameter provided there are the same           ",
"                  number of tims in all tims= lists.                        ", 
"									     ",
" rfile=           If set, read a K-file containing 3D grid definition.      ", 
"                  Assume 2D if no K-file and no grid definition is          ", 
"                  supplied via command line parameters. The required        ", 
"                  command line parameters are: grid_xa,grid_ya,             ", 
"                  grid_xb,grid_yb, grid_xc,grid_yc, grid_wb, grid_wc.       ", 
"                  (See program SUBINCSV for 3D grid details).               ", 
"                  If this is a 3D then the input CDPs for the mute          ", 
"                  locations and the seismic trace CDP key should be         ", 
"                  3D grid cell numbers as produced by SUBINCSV.             ", 
"                  A 3D also forces restrictions on the locations of         ", 
"                  the input mute locations. Their CDP locations must        ", 
"                  form aligned rectangles (see Notes).                      ", 
"									     ",
" offkey=offset    Key header word specifying trace offset                   ",
" abs=1            use the absolute value of offkey.                         ",
"               =0 do not use absolute value of offkey.                      ",
" ntaper=0         number of samples to taper (sine-squared) from            ",
"                  computed full-mute sample                                 ",
" mode=0           mute ABOVE the polygonal curves                           ",
"               =1 to zero BELOW the polygonal curves                        ",
"               =2 to mute below AND above a straight line. In this case     ",
"                  offs,tims describe the total time length of the muted     ",
"                  zone as a function of offs. the slope of the line is      ",
"                  given by vel=                                             ",
"               =3 to mute below AND above a constant velocity hyperbola     ",
"                  as in mode=2 offs,tims describe the total time length of  ",
"                  mute zone as a function of offs, the velocity is given by ",
"                  the value of vel=                                         ",
" vel=330          constant velocity for linear or hyperbolic mute           ",
" tzero=0          time shift (ms.) of linear or hyperbolic mute at the      ",
"                  offkey value of 0. Note: If any tims value is greater     ",
"                  than 100, tzero must be MILLISECONDS. Otherwise seconds.  ",
"									     ",
" extrapi=0        do not extrapolate at ends in igi direction.              ",
"                  (Mute times beyond ends are held constant).               ",
"               =1 extrapolate at both igi ends                              ",
"               =2 extrapolate only at lower igi end                         ",
"               =3 extrapolate only at higher igi end                        ",
"									     ",
" extrapc=0        do not extrapolate at ends in igc direction.              ",
"                  (Mute times beyond ends are held constant).               ",
"               =1 extrapolate at both igc ends                              ",
"               =2 extrapolate only at lower igc end                         ",
"               =3 extrapolate only at higher igc end                        ",
"									     ",
" extrapt=0        do not extrapolate at ends in offset direction.           ",
"                  (Mute times beyond ends are held constant).               ",
"               =1 extrapolate at both offset ends                           ",
"               =2 extrapolate only at lower offset end                      ",
"               =3 extrapolate only at higher offset end                     ",
"									     ",
" check=0          Do not print grid checking and function locations.        ",
"               =1 If grid definition input, run some grid functions on      ",
"                  the 4 grid corner points and print results. Also print    ",
"                  input mute function location information by using the     ",
"                  grid definition and the input cdp= values.                ",
"                  The output values are:                                    ",
"                     G,cdp,igi,igc,xgrid,ygrid,xworld,yworld.               ",
"                  (The format is intended to make it easy to review and     ",
"                  also to make it easy to load to spreadsheets).            ",
"                  This information can be written to a file by              ",
"                  putting 2>yourfile on the command line.                   ",
"									     ",
" print=0          Do not print INPUT mute functions.                        ",
"               =1 Print INPUT mute functions. The cdp number and its        ",
"                  input mute function values are printed. This print is     ",
"                  not intended to be pretty. It just allows easy checking   ",
"                  that the q-file contains the expected values.             ",
"                  It also allows programmers and others to confirm that     ",
"                  command line input via cdp=,offs=,tims= produces the      ",
"                  same results as identical input via the qin= file.        ",
"                                                                            ",
" Notes:								     ",
"									     ",
" The offsets specified in the offs array must be monotonically increasing.  ",
"									     ",
" For 3D, user needs to input mute locations (cdp numbers) which form aligned",
" rectangles. That is, how-ever-many grid inlines the user chooses to put    ",
" mute locations on, there must always be the same number of mute locations  ",
" on each inline and those functions must be located at same grid crosslines.",
" For instance, if user inputs locations for inline 7 at crosslines 15,25,40 ",
" then the user must input locations at crosslines 15,25,40 for any other    ",
" inlines that the user wants to supply locations for. (If user is lucky     ",
" enough that the grid has 100 inlines, then the input locations could be at ",
" CDPs 715,725,740 and 1115,1125,1140 and 2015,2025,2040 and 2515,2525,2540. ",
" Note that the CDP numbers specified on the cdp= parameter and also in the  ",
" input seismic trace cdp key are translated to inline and crossline numbers ",
" using the input 3D grid definition - so those cdp numbers need to          ",
" correspond to the input 3D grid definition.                                ",
"									     ",
" For trace CDPs not listed in cdp= parameter or qin= file, bilinear         ",
" interpolation is done if the trace CDP location is surrounded by 4 mute    ",
" functions specified in the cdp= list. If the trace CDP is not surrounded   ",
" by 4 input mute functions, the result depends on the extrapi and extrapc   ",
" options. The result can be any combination of linear interpolation and     ",
" linear extrapolation and constant extrapolation. If input mute functions   ",
" are only located along 1 grid inline or 1 grid crossline, result is linear ", 
" interpolation in that direction (and linear or constant extrapolation      ",
" the outer ending functions).                                               ",
"									     ",
" The interpolation related to cdp is actually done after the interpolation  ",
" related to offset. That is, first the trace offset is used to compute times",
" from the offs,tims arrays for 4, 2, or 1 mute functions and then weighting ",
" based on the cdp locations of those mute functions is applied. Note also   ",
" that restricting the mute to the earliest and latest trace time is done    ",
" just before application to each trace. A consequence of this is that both  ",
" negative offs= and negative tims= values are allowed and honored even if   ",
" the abs= option is 1 (the default).                                        ",
"									     ",
NULL};

/* Amalgamated Credits:
 *	SEP: Shuki Ronen, Chuck Sword
 *	CWP: Shuki Ronen, Jack K. Cohen, Dave Hale, Bjoern Rommel, 
 *           Carlos E. Theodoro, Sang-yong Suh, John Stockwell                                 
 *      DELPHI: Alexander Koek.
 *      USBG: Florian Bleibinhaus. 
 *
 *      Merged/Modified: Oct 2021: Andre Latour   
 *	 1. This program started as a copy of sunmocsv.c which itself
 * 	    started from sunmo.c (yes, NMO). The reason to do this is    
 *	    that sumute.c just has input parameters of tmute[],xmute[].     
 *	    For 3d muting, this program needs  cdp[], tims[][], offs[][].
 *          And, as it happens, sunmocsv.c has cdp[], tnmo[][], vnmo[][]. 
 *          So the easiest thing was to start from sunmocsv.c and 
 *	    rename input parameters. Parts of sumute.c were then copied
 *	    into this program (the code for ntaper= and mode=0,1,2,3).       
 *	    The mode=4 option/code was not copied to here since it does  
 *	    not seem to make sense combined with bilinear interpolation
 *	    of CDP mute function locations (but I could be wrong).          
 *	 2. Changed to expect milliseconds for all parameter inputs.               
 *	 3. Put in error checks to stop users from accidentally         
 *	    trying to use sumute parameter names.                       
 *      Modified: Feb 2022: Andre Latour   
 *        1. Reworked to use lib routines to get mute function values
 *           either from command line parameters or from input q-files.      
 *        2. Reworked to use lib routines for bilinear interpolation. 
 */
/**************** end self doc *******************************************/

segy tr;

struct QInfo *MInfo; /* All mute function values are stored herein. */

int compSort2 (const void * q1, const void * q2) ; /* comparison function for qsort  */

#define SQ(x) ((x))*((x))

int main(int argc, char **argv) {

        char *key=NULL;         /* header key word from segy.h          */
        char *type=NULL;        /* ... its type                         */
        int index;              /* ... its index                        */
        Value val;              /* ... its value                        */
        float fval;             /* ... its value cast to float          */

	int ncdp;		/* number of cdps specified */
	int oldcdp;     	/* cdp of previous trace */

        cwp_String Rname=NULL;  /* text file name for values            */
        FILE *fpR=NULL;         /* file pointer for Rname input file    */
        double gvals[999];      /* to contain the grid definition       */
	
        float linvel;           /* linear velocity                      */
        float tm0;              /* time shift of mute=2 or 3 for key=0  */
        float *taper=NULL;      /* ...          taper values            */
        int ntaper;             /* ...          taper values            */

        cwp_String Qname=NULL;  /* text file name for Q input file      */
        FILE *fpQ=NULL;         /* file pointer for Q input file        */

        int mgiextr = 0;        /* for igi extrapolation option         */
        int mgcextr = 0;        /* for igc extrapolation option         */
        int mgtextr = 0;        /* for offset extrapolation option      */

        int mode;               /* kind of mute (top, bottom, linear)   */
        int iabsoff;            /* Take absolute value of offkey        */
        cwp_Bool seismic;       /* cwp_true if seismic, cwp_false not seismic */

        cwp_String *pname = NULL; /* to hold the names of values we want */
        int numpname = 3;         /* number of values that we want       */
        cwp_String *ndims = NULL; /* for an unused return argument       */

        int ifixd=0;            /* flag for all tuples same size or vary   */
        int iztuple=0;          /* element number where first tuple exists */
        int ktuple=0;           /* type of tuples (2=pairs, 3=triplets)    */

	int i;    		/* the rest of these variables are either trivial or so */
	int j;    		/* entangled that no short comment up here will help    */
	int k;    		
	int nmore = 0;
	int icdp;
	int jcdp;		
        int errwarn=0;          
        int jnloc1=0;
        double *pindepa = NULL;
        int kigi = 0;
        int kigc = 1;           /* set to 1 in case this is a 2D */ 
        int mgi_tot = -1;
        int mgc_tot = 0;
        int *mgi = NULL;
        int *mgc = NULL;
        int mgi_totdeg = 0;

        int mgix = 0;
        int mgcx = 0;
        double wi = 0.;
        double wc = 0.;
        int ndxi = 0; 
        int ndxc = 0;
        int iprint = 0;
        int maygrid=0;
        int is3d = 1;
        int icheck=0;
        double xg=0.0;
        double yg=0.0;
        double xw=0.0;
        double yw=0.0;
        double dfval=0.0;
        double dwbt=0.0;
        double tscale=1.;


	/* hook up getpar */
	initargs(argc, argv);
	requestdoc(1);

/* NOTE: In this program code I make an effort to conform to the       */
/*       original indentation style (even tho I find it awkward).      */

        getparstring("qin", &Qname);

        if (!getparint("mode", &mode))          mode = 0;
        if(mode<0 || mode>3) err ("error: mode must be 0,1,2, or 3");

        if (!getparint("ntaper", &ntaper))      ntaper = 0;
        if(ntaper<0) err ("error: ntaper cannot be negative.");

        if (!getparint("abs", &iabsoff)) iabsoff = 1;

        if (!getparint("extrapi", &mgiextr)) mgiextr = 0;
        if(mgiextr<0 || mgiextr>3) err ("error: extrapi option not in range ");

        if (!getparint("extrapc", &mgcextr)) mgcextr = 0;
        if(mgcextr<0 || mgcextr>3) err ("error: extrapc option not in range ");

        if (!getparint("extrapt", &mgtextr)) mgtextr = 0;
        if(mgcextr<0 || mgcextr>3) err ("error: extrapt option not in range ");

/* Set up taper weights if tapering requested */

        if (ntaper) {
                taper = ealloc1float(ntaper);
                for (k = 0; k < ntaper; ++k) {
                        float s = sin((k+1)*PI/(2*ntaper));
                        taper[k] = s*s;
                }
        }       

        if (!getparfloat("vel", &linvel))    linvel = 330;
        if (linvel==0) err ("error: vel cannot be 0");
        if (!getparfloat("tzero", &tm0))          tm0 = 0;

        if (!getparstring("offkey", &key)) key = "offset";
        type = hdtype(key);
        index = getindex(key);

/* ------------------------------------------------------------------- */
/* Process and set the grid definition values?                         */

        getparstring("rfile", &Rname);
      
        gridcommand(&maygrid);
      
        is3d = 1;
        if(maygrid==1  && Rname != NULL) err("error: input k-file not allowed when full grid on command line.");
        if(maygrid==-1 && Rname == NULL) err("error: input k-file required when partial grid on command line.");
        if(maygrid==0  && Rname == NULL) is3d = 0;

        if (!getparint("check", &icheck)) icheck = 0;
        if (!getparint("print", &iprint)) iprint = 0;

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

	/* get information from the first header */
	if (!gettr(&tr)) err("error: can't get first trace");

        oldcdp = tr.cdp - 1; /* make sure first trace goes thru the complete code for new cdp */

        seismic = ISSEISMIC(tr.trid);

        if (seismic) {
                if (!tr.dt) err("dt header field must be set");
        } else if (tr.trid==TRID_DEPTH) {   /* depth section */
                if (!tr.d1) err("d1 header field must be set");
        } else {
                err ("tr.trid = %d, unsupported trace id",tr.trid);
        }

/* Set parameters and names of the 3 values needed from getviaCommand or getviaqfile. */

        ktuple   = 0;
        iztuple  = 1;
        numpname = 3;
        pname = ealloc1(numpname,sizeof(cwp_String *));
        for(j=0; j<numpname; j++) pname[j] = ealloc1(4,1);
        strcpy(pname[0],"cdp");
        strcpy(pname[1],"offs");
        strcpy(pname[2],"tims");

        if(Qname == NULL) { /* no q-file, so readin mute functions via command line parameters */

                getviacommand(&pname, &numpname, &iztuple, nmore,
                              &ktuple, &ifixd, &MInfo, &ncdp,
                              &pindepa, &ndims, &errwarn) ;

                if(errwarn==1) err("getviacommand error: no non-tuple name passed in.");
                else if(errwarn==2) err("getviacommand error: non-tuple names have different amounts of values.");
                else if(errwarn==3) err("getviacommand error: independent dimension parameter is empty.");
                else if(errwarn==4) err("getviacommand error: an independent dimension parameter is empty.");
                else if(errwarn==5) err("getviacommand error: members of tuple have different amounts at same location."); 
                else if(errwarn>0) err("getviacommand error: returned unrecognized error number = %d",errwarn);

                if(ifixd==2 || iztuple!=1 || ktuple!=2) 
                  err("error: command line does not contain cdp,offs,tims (or they are incorrect combination). ");

        }

        else { /* Read-in mute functions via q-file? */

                fpQ = fopen(Qname, "r");
                if(fpQ==NULL) err("error: input Q-file did not open correctly.");

                getviaqfile(fpQ, &pname, &numpname, &iztuple, nmore,
                            &ktuple, &ifixd, &MInfo, &ncdp,
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
                  err("error: q-file does not contain cdp,offs,tims names (or they are in non-standard order)");

        } /* end of else for read-in via q-file */

        checkpars(); /* put this down here since getviacommand reads parameters */

/* Note that the ifixd flag indicates what is going on with the INPUT    */
/* q-records or parameter sets. The indepenent dimension name may be in  */
/* pname list (if ifixd=0 the offs are actually in each q-record along   */
/* with the tims, or offs are in multiple offs= parameters).             */
/* But if ifixd=1 there is only 1 set of offs values either in the offs= */
/* parameter or in the C_SU_NDIMS record in q-file.                      */
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

        if(jnloc1!=0) err("error: input must have cdp (before offs,tims).");

/* Copy cdp number to kinf[0] and also set kinf[1] and kinf[2].     */
/* Later, kinf[1] and kinf[2] are what qsort actually sorts on.     */
/* For 3D, kinf[1] and kinf[2] are inline and crossline 3d indexes. */
/* For 2D, we just set kinf[1] to cdp number, and kinf[2] to 1.     */

        for (icdp=0; icdp<ncdp; ++icdp) MInfo[icdp].kinf = ealloc1int(3);

        if(is3d>0) {
                for (icdp=0; icdp<ncdp; ++icdp) {
                        MInfo[icdp].kinf[0] = lrint(MInfo[icdp].dlots[jnloc1]);
                        gridcdpic(gvals,MInfo[icdp].kinf[0],MInfo[icdp].kinf+1,MInfo[icdp].kinf+2);
                        if(MInfo[icdp].kinf[1] < -2147483644)
                          err("error: input cdp %d is not in grid",MInfo[icdp].kinf[0]);
                        if(icheck>0) {
                                gridicgridxy(gvals,MInfo[icdp].kinf[1],MInfo[icdp].kinf[2],&xg,&yg);
                                gridicrawxy(gvals,MInfo[icdp].kinf[1],MInfo[icdp].kinf[2],&xw,&yw);
                                warn("G,%12d,%12d,%12d,  %.20g,%.20g,  %.20g,%.20g",
                                MInfo[icdp].kinf[0],MInfo[icdp].kinf[1],MInfo[icdp].kinf[2],xg,yg,xw,yw);
                        }
                }
        }
        else {
                for (icdp=0; icdp<ncdp; ++icdp) {
                        MInfo[icdp].kinf[0] = lrint(MInfo[icdp].dlots[jnloc1]);
                        MInfo[icdp].kinf[1] = MInfo[icdp].kinf[0]; /* set these for 2d */
                        MInfo[icdp].kinf[2] = 1;                   /* set these for 2d */
                }
        }

/* The following iprint option is not intended to be pretty. For the users it usually just   */
/* allows a quick look to confirm they have the correct q-file (or, just use a text editor). */
/* For you the programmer, it presents simple code to see where the offs and tims values     */
/* are stored by getviacommand and getviaqfile. Note in particular that tims always          */
/* start at dlots[iztuple] for each cdp. The offs are either stored AFTER all tims           */
/* for each cdp, or just ONCE at pointer pindepa (if ifixd flag is 1).                       */
/* So, yes, an input ifixd=0 q-file has values in offs,tims pair order, but they have been   */
/* de-multiplexed into all tims at that cdp followed by all offs at that cdp (i.e. arrays).  */

        if(iprint>0) {

                if(ifixd==0) {
                        for(jcdp=0; jcdp<ncdp; jcdp++) {
                                warn(" cpd= %d   Number of offs,tims pairs= %d",MInfo[jcdp].kinf[0],MInfo[jcdp].nto);
                                for(i=0; i<MInfo[jcdp].nto; i++) {
                                  warn(" %f %f ",MInfo[jcdp].dlots[iztuple+MInfo[jcdp].nto+i],MInfo[jcdp].dlots[iztuple+i]);
                                }
                        }
                }
                else if(ifixd==1) {
                        warn(" Number of offs= %d",MInfo[0].nto);
                        for(i=0; i<MInfo[0].nto; i++) warn(" %f ",pindepa[i]);
                        for(jcdp=0; jcdp<ncdp; jcdp++) {
                                warn(" cpd= %d   Number of tims= %d",MInfo[jcdp].kinf[0],MInfo[jcdp].nto);
                                for(i=0; i<MInfo[0].nto; i++) warn(" %f ",MInfo[jcdp].dlots[iztuple+i]);
                        }
                }

        } /* end of  if(iprint>0) { */

/* Check that independent dimension values are input in increasing order.   */
/* Note that we could just sort the tuples into increasing order ourselves. */
/* But that is far too likely to induce errors. One mis-typed value in the  */
/* input and the results will be subtly bad (and, if you don't know by now, */
/* subtly bad is the worst-kind-of-bad in seismic data processing).         */

        if(ifixd==0) {
                for(jcdp=0; jcdp<ncdp; jcdp++) {
                        pindepa = MInfo[jcdp].dlots+iztuple+MInfo[jcdp].nto;
                        for(i=1; i<MInfo[jcdp].nto; i++) {
                                if(pindepa[i-1] >= pindepa[i])
                                  err("error: offs values are not increasing (input cdp = %d)",MInfo[jcdp].kinf[0]);
                        }
                }
        }
        else if(ifixd==1) {
                for(i=1; i<MInfo[0].nto; i++) {
                        if(pindepa[i-1] >= pindepa[i]) err("error: offs values are not increasing.");
                }
        }

/* Determine whether times are seconds or milliseconds.                     */                      

        j = 0;
        if(ifixd==0) {
               for(jcdp=0; jcdp<ncdp; jcdp++) {
                        for(i=0; i<MInfo[jcdp].nto; i++) {
                                if(MInfo[jcdp].dlots[iztuple+i] > 100.) j = 1;
                        }
                        if(j>0) break;
                }
        }
        else if(ifixd==1) {
                for(jcdp=0; jcdp<ncdp; jcdp++) {
                        for(i=0; i<MInfo[0].nto; i++) {
                                if(MInfo[jcdp].dlots[iztuple+i] > 100.) j = 1;
                        }
                        if(j>0) break;
                }
        }
        
        if(j>0) tscale = 0.001;
        else tscale = 1.;
        tm0 = tm0 * tscale; 

/* For bilinear interpolation, user must input function locations which form aligned rectangles.  */
/* That is, howevermany inlines the user chooses to put functions on, there must be the same      */
/* number of functions on each inline and those functions must be located at the same crosslines. */
/* For instance, if user inputs functions for inline 7 at crosslines 15,25,40 then the user must  */
/* input the functions at crosslines 15,25,40 for any other inlines that the user wants to supply */
/* functions for. The qsplit function enforces that restriction on the user input - and also      */
/* separates the igi and igc values into 2 simple arrays.                                         */
/*   The qsplit function expects the values to be sorted by igi,igc values.                       */
/*   The results of qsplit are then used by binterpfind and binterpapply.                         */

        qsort(MInfo,ncdp,sizeof(struct QInfo),compSort2);

        for (jcdp=1; jcdp<ncdp; ++jcdp) {
                if(MInfo[jcdp-1].kinf[1] == MInfo[jcdp].kinf[1] &&
                   MInfo[jcdp-1].kinf[2] == MInfo[jcdp].kinf[2]) {
                        err("error: Two sets of values input for cdp,igi,igc = %d %d %d",
                        MInfo[jcdp].kinf[0],MInfo[jcdp].kinf[1],MInfo[jcdp].kinf[2]);
                }
        }

/* (Note: qsort is a standard c function, qsplit is a function in su/lib/qdefine.c). */

        qsplit(MInfo,ncdp,&mgi,&mgi_tot,&mgc,&mgc_tot,&errwarn);
        if(errwarn>0) err("error: Input cdps do not form enclosed-rectangles (needed for bilinear).");

        mgi_totdeg = mgi_tot; /* read explanation of mgi_totdeg later */
        if(mgi_tot==1 || mgc_tot==1) mgi_totdeg = 0;

	/* loop over traces */
	do {

                int nt     = (int) tr.ns;
                float tmin = tr.delrt/1000.0;
                float dt = ((double) tr.dt)/1000000.0;
                float t;
                int nmute;
                int itaper;
                int topmute;
                int botmute;
                int ntair=0;
                register int i;

                if (!seismic) {
                        tmin = 0.0;
                        dt = tr.d1;
                }

                /* get value of key and convert to float */
                gethval(&tr, index, &val);
                fval = vtof(type,val);
                if (iabsoff==1) fval = fabsf(fval);

                dfval = fval;
 
                if(ncdp<2) { /* if just one mute function */ 
                        if(ifixd==0) { /* see iprint for a simpler example of where these offs,tims are in dlots */
                          binterpvalue(dfval,mgtextr,
                          MInfo[0].dlots+iztuple+MInfo[0].nto,MInfo[0].dlots+iztuple,MInfo[0].nto,
                          MInfo[0].dlots+iztuple+MInfo[0].nto,MInfo[0].dlots+iztuple,MInfo[0].nto,
                          1,wi,
                          MInfo[0].dlots+iztuple+MInfo[0].nto,MInfo[0].dlots+iztuple,MInfo[0].nto,
                          MInfo[0].dlots+iztuple+MInfo[0].nto,MInfo[0].dlots+iztuple,MInfo[0].nto,
                          1,wc,&dwbt);
                        }
                        else {
                          binterpvalue(dfval,mgtextr,
                          pindepa,MInfo[0].dlots+iztuple,MInfo[0].nto,
                          pindepa,MInfo[0].dlots+iztuple,MInfo[0].nto,
                          1,wi,
                          pindepa,MInfo[0].dlots+iztuple,MInfo[0].nto,
                          pindepa,MInfo[0].dlots+iztuple,MInfo[0].nto,
                          1,wc,&dwbt);
                        }
                      
/* Note: So as not to change the code copied from sumute, t will be in seconds herein */
                        t = dwbt * tscale;
                }
                else if(tr.cdp==oldcdp) { /* just compute time for new offset */
                        if(ifixd==0) { /* see iprint for a simpler example of where these offs,tims are in dlots */
                          binterpvalue(dfval,mgtextr,
                          MInfo[ndxi-1].dlots+iztuple+MInfo[ndxi-1].nto,MInfo[ndxi-1].dlots+iztuple,MInfo[ndxi-1].nto,
                          MInfo[ndxi  ].dlots+iztuple+MInfo[ndxi  ].nto,MInfo[ndxi  ].dlots+iztuple,MInfo[ndxi  ].nto,
                          mgi_tot,wi,
                          MInfo[ndxc-1].dlots+iztuple+MInfo[ndxc-1].nto,MInfo[ndxc-1].dlots+iztuple,MInfo[ndxc-1].nto,
                          MInfo[ndxc  ].dlots+iztuple+MInfo[ndxc  ].nto,MInfo[ndxc  ].dlots+iztuple,MInfo[ndxc  ].nto,
                          mgc_tot,wc,&dwbt);
                        }
                        else {
                          binterpvalue(dfval,mgtextr,
                          pindepa,MInfo[ndxi-1].dlots+iztuple,MInfo[ndxi-1].nto,
                          pindepa,MInfo[ndxi  ].dlots+iztuple,MInfo[ndxi  ].nto,
                          mgi_tot,wi,
                          pindepa,MInfo[ndxc-1].dlots+iztuple,MInfo[ndxc-1].nto,
                          pindepa,MInfo[ndxc  ].dlots+iztuple,MInfo[ndxc  ].nto,
                          mgc_tot,wc,&dwbt);
                        }
                        t = dwbt * tscale;
                }
                else {
                        oldcdp = tr.cdp;
                        /* compute the grid indexes igi,igc */
                        if(is3d==1) { 
                                gridcdpic(gvals,tr.cdp,&kigi,&kigc);
                                if(kigi<-2147483644) err("Error: input cdp= %d not in grid.",tr.cdp); 
                        }
                        else kigi = tr.cdp; /* if no grid, just use igi=cdp and igc=1 */
             
                        /* find input cdp (higher) locations mgix,mgcx and weights wi,wc */
                        binterpfind(kigi,mgi,mgi_tot,mgiextr,kigc,mgc,mgc_tot,mgcextr,
                                    &mgix,&mgcx,&wi,&wc);

                        /* mgix and mgcx are the locations computed for each dimension seperately.    */
                        /* From them, compute the element numbers of the stored functions in MInfo.   */
                        /* Note that for the degenerate cases of mgi_tot=1 or mgc_tot=1 the           */
                        /* mgi_totdeg=0, which results in ndxc=ndxi, which in turn means the second   */
                        /* two functions passed to binterpvalue are the same as the first two         */
                        /* (which works because either weight wi or wc will be 0.0).                  */
                        /* BUT this would not work for both mgi_tot=1 and mgc_tot=1 (just 1 function) */
                        /* which is why we MUST skip this code when there is just 1 function.         */

                        ndxi = mgix + mgi_tot * (mgcx-1); 
                        ndxc = ndxi + mgi_totdeg;

                        /* mgix and mgcx are always returned as the highest of the 2 (near) locations.*/
                        /* (So, if mgi has 10 locations, mgix is only returned from 1 to 9, not 0).   */
                        /* That means the 4 locations are ndxi-1,ndxi and ndxc-1,ndxc.                */

                        /* use the 4 locations and their offs,tims and get mute time at fval offset)  */

                        if(ifixd==0) { /* see iprint for a simpler example of where these offs,tims are in dlots */
                          binterpvalue(dfval,mgtextr,
                          MInfo[ndxi-1].dlots+iztuple+MInfo[ndxi-1].nto,MInfo[ndxi-1].dlots+iztuple,MInfo[ndxi-1].nto,
                          MInfo[ndxi  ].dlots+iztuple+MInfo[ndxi  ].nto,MInfo[ndxi  ].dlots+iztuple,MInfo[ndxi  ].nto,
                          mgi_tot,wi,
                          MInfo[ndxc-1].dlots+iztuple+MInfo[ndxc-1].nto,MInfo[ndxc-1].dlots+iztuple,MInfo[ndxc-1].nto,
                          MInfo[ndxc  ].dlots+iztuple+MInfo[ndxc  ].nto,MInfo[ndxc  ].dlots+iztuple,MInfo[ndxc  ].nto,
                          mgc_tot,wc,&dwbt);
                        }
                        else {
                          binterpvalue(dfval,mgtextr,
                          pindepa,MInfo[ndxi-1].dlots+iztuple,MInfo[ndxi-1].nto,
                          pindepa,MInfo[ndxi  ].dlots+iztuple,MInfo[ndxi  ].nto,
                          mgi_tot,wi,
                          pindepa,MInfo[ndxc-1].dlots+iztuple,MInfo[ndxc-1].nto,
                          pindepa,MInfo[ndxc  ].dlots+iztuple,MInfo[ndxc  ].nto,
                          mgc_tot,wc,&dwbt);
                        }
                        t = dwbt * tscale;
                }

               /* do the mute */
                if (mode==0) {  /* mute above */
                        nmute = MIN(NINT((t - tmin)/dt),nt);
                        if (nmute>0) memset( (void *) tr.data, 0, nmute*FSIZE);
                        for (i = 0; i < ntaper; ++i)
                                if (i+nmute>0) tr.data[i+nmute] *= taper[i];
                        if (seismic) {
                                tr.muts = NINT(t*1000);
                        } else  {
                                tr.muts = NINT(t);
                        } 
                } else if (mode==1){    /* mute below */
                        nmute = MAX(0,NINT((tmin + nt*dt - t)/dt));
                        memset( (void *) (tr.data+nt-nmute), 0, nmute*FSIZE);
                        for (i = 0; i < ntaper; ++i)
                                if (nt>nmute+i && nmute+i>0)
                                        tr.data[nt-nmute-1-i] *= taper[i];
                        if (seismic) {
                                tr.mute = NINT(t*1000);
                        } else  {
                                tr.mute = NINT(t);
                        }
                } else if (mode==2){    /* air wave mute */
                        nmute = NINT((tmin+t)/dt);
                        ntair=NINT(tm0/dt+fval/linvel/dt);
                        topmute=MIN(MAX(0,ntair-nmute/2),nt);
                        botmute=MIN(nt,ntair+nmute/2);
                        memset( (void *) (tr.data+topmute), 0,
                                (botmute-topmute)*FSIZE);
                        for (i = 0; i < ntaper; ++i){
                                itaper=ntair-nmute/2-i;
                                if (itaper > 0) tr.data[itaper] *=taper[i];
                        }
                        for (i = 0; i < ntaper; ++i){
                                itaper=ntair+nmute/2+i;
                                if (itaper<nt) tr.data[itaper] *=taper[i];
                        }
                } else if (mode==3) {   /* hyperbolic mute */
                        nmute = NINT((tmin + t)/dt);
                        ntair=NINT(sqrt( SQ((float)(tm0/dt))+SQ((float)(fval/linvel/dt)) ));
                        topmute=MIN(MAX(0,ntair-nmute/2),nt);
                        botmute=MIN(nt,ntair+nmute/2);
                        memset( (void *) (tr.data+topmute), 0,
                                (botmute-topmute)*FSIZE);
                        for (i = 0; i < ntaper; ++i){
                                itaper=ntair-nmute/2-i;
                                if (itaper > 0) tr.data[itaper] *=taper[i];
                        }
                        for (i = 0; i < ntaper; ++i){
                                itaper=ntair+nmute/2+i;
                                if (itaper<nt) tr.data[itaper] *=taper[i];
                        }
                } /* this is where mode==4 goes in sumute */ 

	        puttr(&tr);
        } while (gettr(&tr));

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
