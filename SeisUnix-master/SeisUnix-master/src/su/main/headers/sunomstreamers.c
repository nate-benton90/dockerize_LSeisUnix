/* Copyright (c) Colorado School of Mines, 2022.*/
/* All rights reserved.                       */

/* SUNOMSTREAMERS: $Revision: 1.00 $ ; $Date: 2022/12/12 00:01:00 $      */

#include "su.h"
#include "segy.h"
#include "headcase.h"

segy tr;

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                            ",
" SUNOMSTREAMERS - Nominal Geometry and 2D CDPS for Towed-Streamer Marine.   ",
"                                                                            ",
" sunomstreamers  >out.su (many parameters, input traces are optional).      ",
"                                                                            ",
" This program allows you to define a towed-streamer marine layout with      ",
" multiple airgun arrays (or other sources) amd multiple towed-streamers.    ",
" This basically consists of inline and crossline offset positions as well   ",
" as airgun and streamer identifiers and streamer channel ranges and spacing.",
" From this definition, output traces are (optionally) updated with offset,  ",
" sx,sy,gx,gy,cdp, and airgun and streamer sequence (grnofr and grnors keys).",
" These values are nominal geometry and nominal inline cdps since they are   ",
" computed only from the layout definition and not any INPUT coordinates.    ",
"                                                                            ",
"   **********************************************************************   ",
"   To output this documentation:  sunomstreamers 2> nomstreamers.txt        ",
"   **********************************************************************   ",
"   Also consult: src/demos/Geom3D/Sunomstreamers for layout diagram.        ",
"   **********************************************************************   ",
"                                                                            ",
" Parameters:                                                                ",
"                                                                            ",
" shotkey=fldr    Shot Key. This value is multiplied by shotspacing to shift ",
"                 the shot reference point forward in the inline direction.  ",
"                 The reference point is the point from where all inline and ",
"                 crossline offsets are measured. The reference point may    ",
"                 also be called antenna position or centre-of-boat.         ",
"                                                                            ",
" shotspacing=    Inline distance between shotkey values. Must be specified. ",
"                                                                            ",
" gunkey=         Key that indicates which airgun array is firing for shots. ",
"                 The default is the shotkey specified above.              . ",
"                 This value and gunmod identify which gun array is firing   ",
"                 (typical acquizition has just port and starboard arrays).  ",
"           Note: Usually the gunkey is a sequential shot or field record    ",
"                 number and odd shots are from the port airgun array and    ",
"                 even shots are from starboard airgun array (or vice-versa) ",
"                 so the gunid list should be 0,1 (or 1,0).                  ",
"                                                                            ",
" gunmod=    Modulus value for gunkey. The remainder is the value to         ",
"            specify in the gunid list. The default here is the amount       ",
"            of values specified in the gunid list (typically 2).            ",
"                                                                            ",
" gunid=     List of gun array identifier values. Must be specified.         ",
"            See parameters gunkey and gunmod.                               ",
"                                                                            ",
" guncross=  List of gun array crossline offset distances.                   ",
"            Must specify the same amount of values as the gunid list.       ",
"            Usually port offsets are negative, starboard are positive.      ",
"                                                                            ",
" guninline= List of gun array inline offset distances.                      ",
"            You can specify either 1 value for all gun arrays or you        ",
"            can list the same amount of values as the gunid list.           ",
"            Fore offsets are positive, aft offsets are negative. Since      ",
"            the gun arrays are typically behind the boat, these values      ",
"            are usually negative (and also usually all the same).           ",
"                                                                            ",
" streamercross=    List of streamer crossline offset distances.             ",
"                   Must specify a value for each streamer.                  ",
"                   Usually port offsets are negative, starboard positive.   ",
"                                                                            ",
" streamerinline=   List of inline offset distances at start of streamers.   ",
"                   You can specify either 1 value for all streamers or you  ",
"                   can list the same amount of values as streamercross.     ",
"                   Fore offsets are positive, aft are negative. Since the   ",
"                   streamers are typically behind the boat, these values    ",
"                   are usually negative (and also usually all the same).    ",
"                                                                            ",
" streamerkey=      Streamer Identifier key. If not specified then also the  ",
"                   streameridnt cannot be specified, and channelstart and   ",
"                   channelend range must not overlap amoung any streamers.  ",
"                                                                            ",
" streameridnt=     List of streamer identifier numbers. If streamerkey is   ",
"                   not specified, you cannot specify this parameter either. ",
"                   Otherwise, list same amount of values as streamercross.  ",
"                                                                            ",
" channelkey=tracf  Channel Identifier key. Channel numbers also often have  ",
"                   the same values as trace numbers.                        ",
"                                                                            ",
" channelstart=     List of channel numbers at start of streamers.           ",
"                   You can specify either 1 channelstart and 1 channelend   ",
"                   or each with the same amount of values as streamercross. ",
"                   If you do specify only 1 channelstart and 1 channelend   ",
"                   and streamerkey is specified, the channel range for      ",
"                   subsequent streamers is identical to the first streamer. ",
"                   If you do specify only 1 channelstart and 1 channelend   ",
"                   and streamerkey is NOT specified, you must specify the   ",
"                   lowest channel range. Other streamers will have the      ",
"                   same channel amount and their channel numbers will be    ",
"                   contiguous with and greater than your specified range.   ",
"                                                                            ",
" channelend=       List of channel numbers at end of streamers. This value  ",
"                   can be less than the channelstart of this streamer.      ",
"                   (Channel numbers can decrease away from streamer start). ",
"                                                                            ",
" channelspacing=   List of channel spacing on streamers.                    ",
"                   You can specify either 1 value for all streamers or you  ",
"                   can list the same amount of values as streamercross.     ",
"                                                                            ",
" cdpspacing=  Inline distance between cdps. Default is the minimum channel  ",
"              spacing of any streamer divided by 2. Any positive value sets ",
"              the cdp key to a computed nominal INLINE cdp number. The cdp  ",
"              is computed only from the layout parameters specified here,   ",
"              and only from the INLINE offset components of the layout.     ",
"           =0. Do not reset the cdp key values in the traces.               ",
"                                                                            ",
" offpi=2  Set offset key to Pythagorean value computed from the inline and  ",
"          crossline offsets and other values of the layout definition.      ",
"      =1  Set offset key to the absolute difference of INLINE components.   ",
"          (This is also known as the nominal inline offset distance).       ",
"      =0  Do not reset offset key.                                          ",
"    Note: This program NEVER computes offset from input trace coordinates.  ",
"    Note: Option 1 is used in some differential NMO processing situations.  ",
"                                                                            ",
" linelocs=1 Set the grnofr,grnors,grnlof,gaps keys as follows:              ",
"              grnofr to airgun sequence (from 1 to amount of airguns)       ",
"              grnors to streamer sequence (from 1 to amount of streamers)   ",
"              grnlof to nominal inline cdp computed at shot location        ",
"              gaps   to nominal inline cdp computed at channel location     ",
"         =0 Do not reset the grnofr,grnors,grnlof,gaps keys.                ",
"      Note: grnofr multiplied by grnors forms a subsurface line number      ",
"            which is useful in some data processing situations.             ",
"                                                                            ",
" scalco=10  Multiply coordinates by this power of 10 (1,10,100...)          ",
"            before putting them in traces. Default is 10 which means        ",
"            that sx,sy,gx,gy are multiplied by 10. The actual value of      ",
"            scalco in the traces is therefore set to -10 (meaning these     ",
"            values need to be divided by 10). Also counit key is set to 1.  ",
"      Note: The x coordinates have 100000 added to them before output and   ",
"            the y coordinates have 10000 added to them before output.       ",
"            The 100000 and 10000 values are added just to avoid having      ",
"            negative coordinates for the output sx,sy,gx,gy key values.     ",
"       =0   Do not reset sx,sy,gx,gy,scalco,counit. There are situations    ",
"            when you want to compute nominal inline cdps and other values   ",
"            but traces already have coordinates (which you wish to retain). ",
"                                                                            ",
" verbose=  Default is not to print expanded gun and streamer information.   ",
"        =1 Print expanded gun and streamer information. All airgun array    ",
"           and streamer information is printed after expanding all defaults.",
"                                                                            ",
" create=     Default is not to create output traces.                        ",
"       =n    Create n shots (all traces of each shot).                      ", 
"       Note: Trying to also input a trace file error-halts unless spikes=n. ", 
"                                                                            ", 
" firstshot=1 This is the shotkey value of the first shot. The shotkey       ",
"             values increment by 1 for each output shot.                    ",
"                                                                            ", 
" spikes=     This option will error-halt if create= is not specified.       ", 
"             By default, create= outputs only 1 zero-sample at 4 ms.        ", 
"             This parameter accepts a list of time,amplitude pairs.         ", 
"                                                                            ", 
"               Example: spikes=4,0.001,800,1000,1500,-2000,3000,0           ", 
"                                                                            ", 
"             The maximum time sets the trace length (3000 ms. above).       ", 
"             The first pair in the list is special, it specifies the        ", 
"             sample interval (in ms.) and base-amplitude.                   ", 
"             In this example, the sample interval is 4, and the             ", 
"             base-amplitude is 0.001. The base-amplitude is the value       ", 
"             samples are set to if they are not spiked by remainder of      ", 
"             the list. (For geometry checking and other QC and survey       ", 
"             design reasons, base-amplitude of 0.0 is not a nice value.     ", 
"             For instance, 0.0 makes it difficult to check mutes).          ", 
"             All times should be whole multiples of the sample interval     ", 
"             but will be rounded to nearest sample time if not.             ", 
"             Times do not have to increase but cannot be negative.          ", 
"                                                                            ", 
"             -or-                                                           ", 
" spikes=n    Where n is the sequential number of an input trace. The        ", 
"             sample values of this trace will be copied to all created      ", 
"             output traces. The intention is to allow input reflectivity    ", 
"             type of trace, and duplicate the samples for all output.       ", 
"                                                                            ",
" ------------------------------------------------------------------------   ", 
"                                                                            ",
NULL};

/* Credits:                                                                  */
/* Author: Andre Latour, Dec 2022                                            */ 
/*                                                                           */
/* Keys involved: fldr,tracf,offset,sx,sy,gx,gy,cdp,grnlof,gaps,grnofr,grnors*/
/*                                                                           */
/**************** end self doc ***********************************************/

int main(int argc, char **argv) {

  cwp_String shotkey=NULL;     
  int shotkase=0;
  cwp_String gunkey=NULL;     
  int gunkase=0;
  int gunvalue=0;
  cwp_String channelkey=NULL;     
  int chankase=0;
  int chanvalue=0;
  cwp_String idntkey=NULL;     
  int idntkase=0;
  int idntvalue=0;
  int verbose=0;

  double *sourcei=NULL;
  double *sourcex=NULL;
  int    *sourceg=NULL;

  double *stxoff=NULL;
  double *stioff=NULL;
  int    *stidnt=NULL;
  int    *stcbeg=NULL;
  int    *stcend=NULL;
  double *stdist=NULL;
  int    iuseidnt=0;

  double distmin=9999.;
  double shotspacing=0.0;
  double cdpspacing=0.0;
  double backmost=0.0; 

  double referencei=0.0;
  int offpi=2;
  int iscalco=0;
  double dscalco=0.0;
  int nscalco=0;
  double xs=0.0;
  double ys=0.0;
  double xg=0.0;
  double yg=0.0;
  double xbase=100000.0;
  double ybase=10000.0;
  int linelocs=1;

  int numsource=0;
  int numstreamer=0;
  int istreamer=0;
  int nproct=0;
  int i=0;
  int n=0;
  int isource=0;
  int gunmod=0;
  int modsource=0;

  int ifakelast=0;
  int ifakegun=0;
  int ifakechan=0;
  int ifakestreamer=0;

  int ncreate=0;
  int ifakeshot=1;
  float *spikes=NULL;
  int ireplicate=0;
  int nspikes=0;
  float tmax=0.;
  int nsamps=0;
  int igo=1;

/* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  if (!getparint("verbose", &verbose)) verbose = 0;
  if(verbose<0 || verbose>1) err("error: verbose= is out of range.");

  if (!getparint("offpi", &offpi)) offpi = 2;
  if(offpi<0 || offpi>2) err("error: offpi= is out of range.");

  if (!getparint("linelocs", &linelocs)) linelocs = 1;
  if(linelocs<0 || linelocs>1) err("error: linelocs= is out of range.");

  if (!getparint("scalco", &iscalco)) iscalco = 10;
  if(iscalco != 0) {
    if(iscalco==-1) iscalco = 1; 
    nscalco = abs(iscalco);
    if(nscalco!=1 && nscalco!=10 && nscalco!=100 && nscalco!=1000 && nscalco!=10000 
      && nscalco!=100000 && nscalco!=1000000 && nscalco!=10000000) {
      err("**** Error: scalco= must be signed powers of 10 (1,10,100...-10,-100,...)");
    }
    if(iscalco>0) dscalco = (double)(iscalco);
    else          dscalco = -1./(double)(iscalco);
    iscalco *= -1; /* to output in header */
  }
  
  if (!getparstring("shotkey", &shotkey)) shotkey = "fldr";
  shotkase = GetCase(shotkey);
  if(shotkase<1) err("error: shotkey= is not a recognized header key name");

  if(!getpardouble("shotspacing", &shotspacing)) err("error: shot spacing must be specified.");
  if(shotspacing<=0.0) err("error: shot spacing must be greater than 0.");

/* -------------------------------------------------------------------------- */

  numsource = countparval("guncross");
  if(numsource<1) err("**** Error: at least 1 gun array crossline offset must be specified.");

  if(countparval("guninline") != 1 && numsource != countparval("guninline")) 
    err("**** Error: must specify 1 gun array inline offset -OR- same amount as gun array crossline offsets.");

  if(numsource != countparval("gunid")) 
    err("**** Error: different amount of gun array crossline offsets and gun identifiers.");

/* -------------------------------------------------------------------------- */

  if(!getparint("gunmod", &gunmod)) gunmod = numsource;
  if(gunmod<1) err("**** Error: gunmod must be greater than 0.");

  if (!getparstring("gunkey", &gunkey)) gunkase = shotkase;
  else {
    gunkase = GetCase(gunkey);
    if(gunkase<1) err("error: gunkey= is not a recognized header key name");
  }

  sourcex = ealloc1double(numsource); 
  sourcei = ealloc1double(numsource); 
  sourceg = ealloc1int(numsource+1); /* +1 makes first trace easier to handle */ 
  getpardouble("guncross",sourcex);
  getpardouble("guninline",sourcei);
  if(countparval("guninline")==1) {
    for(i=1; i<numsource; i++) sourcei[i] = sourcei[0];
  }
  getparint("gunid",sourceg); 

  for(i=0; i<numsource; i++) {
    if(sourceg[i] >= gunmod || sourceg[i]<0) 
      err("error: specified gunid=%d cannot occur with gunmod=%d",sourceg[i],gunmod);
    for(n=i+1; n<numsource; n++) {
      if(sourceg[i] == sourceg[n]) 
        err("error: gunid=%d has been specified at least twice.",sourceg[i]);
      if(sourcex[i] == sourcex[n]) 
        warn("Unusual: some gun arrays have the same crossline offset.");
      if(sourcei[i] != sourcei[n]) 
        warn("Unusual: some gun arrays have different inline offsets.");
    }
/* unlikely but possible that a gun array is furthest back                    */
    if(sourcei[i] < backmost) backmost = sourcei[i]; 
  }
  isource = numsource;            /* helps handle the first trace             */
  sourceg[isource] = -2147483645; /* helps handle the first trace             */

/* -------------------------------------------------------------------------- */

  if (!getparstring("channelkey", &channelkey)) channelkey = "tracf"; 
  chankase = GetCase(channelkey);
  if(chankase<1) err("error: channelkey= is not a recognized header key name");

  numstreamer = countparval("streamercross");
  if(numstreamer<1) err("**** Error: at least 1 streamer crossline offset must be specified.");

  if(countparval("streamerinline") != 1 && numstreamer != countparval("streamerinline")) 
    err("**** Error: must specify 1 streamer inline offset -OR- same amount as streamer crossline offsets.");

  iuseidnt = 0;
  if(countparval("streameridnt") > 0) {
    iuseidnt = 1;
    if(numstreamer != countparval("streameridnt")) 
      err("**** Error: different amount of streamer crosslines and streamer idnts specified.");
  }

  if(iuseidnt>0) {
    if (getparstring("streamerkey", &idntkey)) {
      idntkase = GetCase(idntkey);
      if(idntkase<1) err("error: streamerkey= is not a recognized header key name");
    }
    else {
      err("**** Error: streameridnt and streamerkey must both be specified (or neither).");
    }
  }
  else {
    if (getparstring("idntkey", &idntkey)) {
      err("**** Error: streameridnt and streamerkey must both be specified (or neither).");
    } 
  }

  if(countparval("channelstart")!=1 || countparval("channelend")!=1) {
    if(numstreamer != countparval("channelstart")) 
      err("**** Error: different amount of streamer crosslines and channel starts specified.");
    if(numstreamer != countparval("channelend")) 
      err("**** Error: different amount of streamer crosslines and channel ends specified.");
  }

  if(countparval("channelspacing") != 1 && numstreamer != countparval("channelspacing")) 
    err("**** Error: must specify 1 streamer channel spacing -OR- same amount as streamer crossline offsets.");

  stxoff = ealloc1double(numstreamer);
  stioff = ealloc1double(numstreamer);
  stidnt = ealloc1int(numstreamer+1); /* the +1 makes the first trace easier to handle */
  stcbeg = ealloc1int(numstreamer+1); /* the +1 makes the first trace easier to handle */
  stcend = ealloc1int(numstreamer+1); /* the +1 makes the first trace easier to handle */
  stdist = ealloc1double(numstreamer);

  getpardouble("streamercross",stxoff);
  getpardouble("streamerinline",stioff);
  if(countparval("streamerinline")==1) {
    for(i=1; i<numstreamer; i++) stioff[i] = stioff[0];
  }

  if(iuseidnt>0) {
    getparint("streameridnt",stidnt);                                                    
    for(i=0; i<numstreamer; i++) {
      for(n=i+1; n<numstreamer; n++) {
        if(stidnt[i] == stidnt[n]) 
          err("error: streameridnt=%d has been specified at least twice.",stidnt[i]);
      }
    }
  }
  else { /* just so verbose=1 prints -9999 */
    for(i=0; i<numstreamer; i++) stidnt[i] = -9999;
  }

  getpardouble("channelspacing",stdist);
  if(countparval("channelspacing")==1) {
    for(i=1; i<numstreamer; i++) stdist[i] = stdist[0];
  }
  
  getparint("channelstart",stcbeg); 
  getparint("channelend",stcend);
  if(countparval("channelstart")==1 && countparval("channelend")==1) {
    for(i=1; i<numstreamer; i++) {
      if(iuseidnt==1) {
        stcbeg[i] = stcbeg[0];
        stcend[i] = stcend[0];
      }
      else {
        if(stcend[0]>=stcbeg[0]) {
          stcbeg[i] = stcbeg[0] + i*(stcend[0]-stcbeg[0]+1);
          stcend[i] = stcend[0] + i*(stcend[0]-stcbeg[0]+1);
        }
        else {
          stcbeg[i] = stcbeg[0] + i*(stcbeg[0]-stcend[0]+1);
          stcend[i] = stcend[0] + i*(stcbeg[0]-stcend[0]+1);
        }
      }
    }
  }

  for(i=0; i<numstreamer; i++) {
    for(n=i+1; n<numstreamer; n++) {
      if(iuseidnt==0) {
        if(stcbeg[i] >= stcbeg[n] && stcbeg[i] <= stcend[n]) 
          err("error: channelstart=%d for streamer=%d is in channel range of streamer=%d.",stcbeg[i],i+1,n+1);
        if(stcend[i] >= stcbeg[n] && stcend[i] <= stcend[n]) 
          err("error: channelend=%d for streamer=%d is in channel range of streamer=%d.",stcend[i],i+1,n+1);
      }
      if(stxoff[i] == stxoff[n]) 
        warn("Unusual: some streamers have the same crossline offset.");
      if(stioff[i] != stioff[n]) 
        warn("Unusual: some streamers have different inline offsets.");
      if(stdist[i] != stdist[n]) 
        warn("Unusual: some streamers have different channelspacings.");
    }
  }

/* If channels reversed, exchange ends and make channelspacing negative.      */
/* This allows the later computation to still use the stcbeg channel number.  */

  distmin = stdist[0];
  for(i=0; i<numstreamer; i++) {
    if(stdist[i]<=0.0) err("**** Error: every channelspacing must be positive.");
    if(stdist[i]<distmin) distmin = stdist[i];

    if(stcbeg[i]>stcend[i]) {
      stioff[i] -= (stcbeg[i]-stcend[i]) * stdist[i];
      n         = stcbeg[i];
      stcbeg[i] = stcend[i];
      stcend[i] = n;
      stdist[i] = 0. - stdist[i];
    }

/* Get backmost (inline offset distance to furthest channel on any streamer). */

    if(stdist[i]>0.0) {
      if(stioff[i] + stdist[i] * (stcbeg[i] - stcend[i]) < backmost) 
        backmost = stioff[i] + stdist[i] * (stcbeg[i] - stcend[i]);
    }
    else {
      if(stioff[i] < backmost) backmost = stioff[i]; 
    }
  }

/* Set these to initial values (so the code handles the first trace).         */ 

  istreamer = numstreamer;         /* helps handle the first trace            */
  stidnt[istreamer] = -2147483645; /* helps handle the first trace            */
  stcbeg[istreamer] =  2147483645; /* helps handle the first trace            */
  stcend[istreamer] = -2147483645; /* helps handle the first trace            */

  if(!getpardouble("cdpspacing", &cdpspacing)) cdpspacing = distmin * 0.5;
  if(cdpspacing<0.0) err("error: cdp spacing parameter cannot be negative.");

  if(cdpspacing!=0.0) {
    if(fmod(shotspacing,cdpspacing) != 0.0) 
      warn("Unusual: shot spacing is not a multiple of cdp spacing.");
    for(i=0; i<numstreamer; i++) {
      if(fmod(stdist[i],cdpspacing) != 0.0) 
        warn("Unusual: channel spacing is not a multiple of cdp spacing on streamer=%d",i+1);
    }
  }

  if(verbose>0) {
    warn(" -----------------------------------------------------------------------------");
    warn("     gunmod=%d",gunmod);
    warn("      gunid     guninline  guncross   output grnofr value");
    for(i=0; i<numsource; i++) {
      warn("%10d %10g %10g %15d ",sourceg[i],sourcei[i],sourcex[i],i+1);
    }
    warn(" --- ");
    warn("   streameridnt  streamerinline streamercross  channelstart channelend channelspacing  output grnors value");
    n = 0;
    for(i=0; i<numstreamer; i++) {
      warn("%10d %15g %12g %16d %10d %12g %15d ",stidnt[i],stioff[i],stxoff[i],stcbeg[i],stcend[i],stdist[i],i+1);
      if(stdist[i]<0.) n++;
    }
    if(n>0) warn(" There are %d negative channelspacings. Which means channel range was exchanged and streamerinline adjusted.",n);
    warn(" -----------------------------------------------------------------------------");
  }

  ncreate = 0;
  if(!getparint("create", &ncreate)) ncreate = 0;
  if(ncreate<0) err("**** Error create= cannot be negative.");

  ifakeshot = 1;
  if(!getparint("firstshot", &ifakeshot)) ifakeshot = 1;

  spikes = calloc(1,sizeof(float)); /* eliminates annoying compilor warning when unused for some option */ 
  ireplicate = 0;
  nspikes = countparval("spikes");
  if(nspikes>0) {
    if(ncreate<1) err("**** Error: spikes= can only be specified if create= is also specified.");
    if(nspikes>1) {
      if(nspikes%2 == 1) err("**** Error: spikes= must have an even number of values (time amplitude pairs).");
      spikes = calloc(nspikes,sizeof(float)); /* there are actually half this number of spikes */
      getparfloat("spikes",spikes);
    }
    else {
      if (!getparint("spikes", &ireplicate)) ireplicate = 0;
      if(ireplicate<1) err("**** Error: For spikes=n the n must be +integer (the trace to input).");
    }
  }

  if(ncreate>0) { /* creating traces */ 
    if(isatty(STDIN_FILENO)!=1) { /* have input trace file */
      if(ireplicate<1) err("**** Error: Input trace file not allowed when create= (unless spikes=n).");
    }
    else {
      if(ireplicate>0) err("**** Error: Input trace file must be specified when spikes=n.");
    }
    if(cdpspacing==0.0) warn("Caution: cdpspacing=0 so CDP key will not be set in these created traces.");
    if(offpi==0) warn("Caution: offpi=0 so OFFSET key will not be set in these created traces.");

/* Set these to initial values so that first fake trace will work.            */

    ifakeshot--;
    ifakelast = ncreate + ifakeshot;
    ifakegun = -1;
    ifakechan = 2147483645;
    ifakestreamer = numstreamer-1;
  }
  else { /* not creating traces */
    if(isatty(STDIN_FILENO)==1) err("**** Error: Input trace file must be specified when not create= ");
  }

  checkpars(); 

/* ---Start processing traces.----------------------------------------------- */

  if(ncreate<1 || ireplicate>0) { /* normal input run? or replicating one input only? */
    if(!gettr(&tr)) err("can't read first trace");
    for(n=1; n<ireplicate; n++) {
      if(!gettr(&tr)) err("can't read enough traces for spikes=n option.");
    }
  }
  else { /* going to create first trace from nothing and maybe put spikes on it */
    if(nspikes==0) {
      tr.ns = 1; /* samples */
      tr.dt = NINT(1000.0 * 4.);
      tr.data[0] = 0.;
    }
    else {
      tmax = 0.;
      for(n=2; n<nspikes; n+=2) {
        if(spikes[n] > tmax) tmax = spikes[n];
      }
      nsamps = NINT((tmax)/spikes[0]);
      tr.ns = nsamps; 
      tr.dt = NINT(1000.0 * spikes[0]);
      for(n=0; n<nsamps; n++) tr.data[n] = spikes[1];
      for(n=0; n<nspikes/2; n++) { /* n=0 includes 0 sample in negative check */
        i = NINT(spikes[n*2]/spikes[0]) - 1;
        if(i < 0) err("Error: Spike times less than first sample are not permitted.");
        tr.data[i] = spikes[n*2+1]; 
      }
    }
  }

/* loop over traces                                                           */ 

  igo = 1;
  do {

/* When creating fake traces, set keys so nonfake part of code runs the same. */

    if(ncreate>0) {
      ifakechan++;
      if(ifakechan>stcend[ifakestreamer]) {
        ifakestreamer++;
        if(ifakestreamer==numstreamer) {
          ifakeshot++;
          if(ifakeshot>ifakelast) break;
          ifakestreamer = 0;
          tohead(&tr, shotkase, (double)ifakeshot);
          if(gunkase!=shotkase) { /* Do not actually know what values these   */
            ifakegun++;           /* would be. So just cycle through them.    */
            if(ifakegun==numsource) ifakegun = 0;
            tohead(&tr, gunkase,  (double)sourceg[ifakegun]);
          }
        } /* end of  if(ifakestreamer==numstreamer) {                         */
        ifakechan = stcbeg[ifakestreamer];
        if(iuseidnt>0) tohead(&tr, idntkase, (double)stidnt[ifakestreamer]);
      } /* end of  if(ifakechan>stcend[ifakestreamer]) {                      */
      tohead(&tr, chankase, (double)ifakechan);
    } /* end of  if(ncreate>0) {                                              */

    gunvalue = (int) fromhead(tr,gunkase);
    modsource = gunvalue % gunmod;

/* Usually, incomming traces are in some kind of order. To save cpu time,     */
/* first check if the new trace is in same shot and/or streamer as previous.  */
/* If not, loop to find.                                                      */

    if(modsource != sourceg[isource]) {
      isource = -1;
      for(i=0; i<numsource; i++) {
        if(modsource==sourceg[i]) {
          isource = i;
          break;
        } 
      }
      if(isource<0) 
        err("**** Error: source gun array ident is not any specified gunid. Shot=%d Idnt=%d",tr.fldr,idntvalue);
    }

    if(iuseidnt>0) {
      idntvalue = (int) fromhead(tr,idntkase);
      if(idntvalue != stidnt[istreamer]) {
        istreamer = -1;
        for(i=0; i<numstreamer; i++) {
          if(idntvalue == stidnt[i]) {
            istreamer = i;
            break;
          }
        }
        if(istreamer<0) 
          err("**** Error: streamer idnt not in any specified streamer. Shot=%d Idnt=%d",tr.fldr,idntvalue);
      }
    }

    chanvalue = (int) fromhead(tr,chankase);
    if(chanvalue > stcend[istreamer] || chanvalue < stcbeg[istreamer]) {
      istreamer = -1;
      for(i=0; i<numstreamer; i++) {
        if(chanvalue <= stcend[i] && chanvalue >= stcbeg[i]) {
          istreamer = i;
          break;
        }
      }
      if(istreamer<0) 
        err("**** Error: channel not in range of any specified streamer. Shot=%d Channel=%d",tr.fldr,chanvalue);
    }

    referencei = fromhead(tr,shotkase) * shotspacing - backmost;

    xs = sourcei[isource] + referencei;
    ys = sourcex[isource];
    xg = stioff[istreamer] + referencei + stdist[istreamer] * (stcbeg[istreamer] - chanvalue) ;
    yg = stxoff[istreamer];

    if(offpi>1) tr.offset = lrint(sqrt((xs-xg)*(xs-xg) + (ys-yg)*(ys-yg)));
    else if(offpi==1) tr.offset = lrint(fabs(xs-xg));

    if(cdpspacing != 0.) tr.cdp = lrint((xs+xg) * 0.5 / cdpspacing);

    if(linelocs>0) {
      tr.grnofr = isource + 1;
      tr.grnors = istreamer + 1;
      tr.grnlof = lrint(xs / cdpspacing);
      tr.gaps   = lrint(xg / cdpspacing);
    }

    if(iscalco != 0) { /* if add xbase before, makes big cdp numbers, et al.  */
      tr.sx = lrint((xs+xbase) * dscalco);
      tr.sy = lrint((ys+ybase) * dscalco);
      tr.gx = lrint((xg+xbase) * dscalco);
      tr.gy = lrint((yg+ybase) * dscalco);
      tr.scalco = iscalco;
      tr.counit = 1;
    }

    puttr(&tr);
    nproct++;

    if(ncreate<1) {
      if(!gettr(&tr)) igo = 0;
    }

  } while (igo);

  warn("Number of traces output= %d ",nproct);

}
