/*

LICENSE FOR BHP SU Suite of Programs

The following is the license that applies to the copy of the software hereby
provided to Licensee. BHP's Software Manager may be contacted at the following
address:

Colorado School of Mines
1500 Illinois Street
Golden, Colorado 80401
Attention: John Stockwell
e-mail: john@dix.mines.edu
Telephone: 303-273-3049

Copyright 2001 BHP Software. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software") to deal
in the Software, without restriction, except as hereinafter provided,
including without limitation the rights to use, copy, modify merge,
publish, and distribute the Software and to permit persons
to whom the Software is furnished to do so, provided that the above
copyright notice and this permission notice appear in all copies of the
Software and that both the above copyright notice and this permission
notice appear in supporting documentation. No charge may be made for
any redistribution of the Software, including modified or merged versions
of the Software. The complete source code must be included
in any distribution. For an executable file, complete source code means the
source code for all modules it contains.

Modified or merged versions of the Software must be provided to the Software
Manager, regardless of whether such modified or merged versions are
distributed to others.

THE SOFTWARE IS PROVIDED 'AS IS" WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE
COPYRIGHT HOLDER INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM OR
ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OF PERFORMANCE OF
THIS SOFTWARE.

The name of the copyright holder shall not be used in advertising or
otherwise to promote the use or other dealings in this Software, without
prior written consent of the copyright holder.

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "par.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 					                        ",
" BHPVELUTIL - BHP Velocity Utility                             ",
" 					                        ",
" BHPVELUTIL converts velocities between various formats        ",
" 					                        ",
" bhpvelutil < infile > outfile [optional parameters]           ",
" 					                        ",
" Required Parameters: none		                        ",
" 					                        ",
" Optional Parameters:			                        ",
" verbose=0      Use verbose=1 for long printout                ",
" infmt=segy     Format of input data (segy means SU-segy)      ",
" outfmt=par     Format of output data                          ",
"                                                               ",
" Valid values for infmt/outfmt:                                ",
" segy           SU SEGY format                                 ",
" par            SU ASCII Format                                ",
" par3d          SU ASCII Format Modified for 3D                ",
"                NB: par3d does not use stdout, specify parfile ",
" pro            ProMAX ASCII Format                            ",
" velf, velf1    Western ASCII Format 1                         ",
" velf2          Western ASCII Format 2                         ",
" velf3          Western ASCII Format 3                         ",
" veff           Vastar/Phillips/Charisma ASCII Format          ",
" vel3d          Shell ASCII Format                             ",
" lm             Landmark XYTV ASCII Format                     ",
" avf            Landmark 'ASCII Velocity File', used by TDQ    ",
" v2             Veritas V2 ASCII Format                        ",
" views          ADS VIEWS ASCII vtp Format                     ",
" fdmodel        ADS VIEWS Binary 2D FDModel Format             ",
"                NOTE: fdmodel input/output does not use        ",
"                stdin/stdout. Use model_in/model_out parameters",
" tgs            TGS-NOPEC ASCII Format                         ",
" sbase2         SeisBase 2D ASCII                              ",
" sbase3         SeisBase 3D ASCII                              ",
" ntout=         Number of samples per trace in output          ",
"                If infmt=segy default is input samples per trace",
"                If infmt=fdmodel, default is model samples per trace",
"                If infmt=[any ASCII], samples per trace is     ",
"                whatever is in each input ASCII trace          ",
"                For ASCII to SEGY, defaults to 1000.           ",
" dt=            Use for ASCII to SEGY, defaults to 0.004.      ",
" line=          Line name for ASCII output. If not specified,  ",
"                and input is ASCII, get line name from input,  ",
"                if available. Line is required when writing    ",
"                formats velf, veff, par3d, sbase2              ",
"                                                               ",
" interp=yes     For ASCII to SEGY, default action is to        ",
"                linearly interpolate input data to a regular   ",
"                sample interval. To disbale interpolation, and ",
"                just copy input, use interp=no. This is useful ",
"                if ASCII is already sampled regularly.         ",
"                                                               ",
" model_in=      Binary input file required if infmt=fdmodel    ",
" model_out=     Complete path/filename of output               ",
"                                                               ",
"  If outfmt=par3d, parfiles are used instead of stdout.        ",
"  Each parfile is named line_'lnum'.par3d, where 'lnum' is the ",
"  3D line number, e.g., line_1000.par3d, line_1002.par3d, etc. ",
"  The parfiles are written to a directory specified by pardir  ",
"                                                               ",
"  pardir=3dvels Directory name to which to write parfiles.     ",
"                                                               ",
"  If outfmt=fdmodel, parameters nx through cdpnum              ",
"  are required by the ADS VIEWS program                        ",
" nx=100         Number of velocity traces                      ",
" dx=50          Distance between velocity traces               ",
" cdpmin=1       Minimum CDP number of model                    ",
" cdpmax=100     Maximum CDP number of model                    ",
" cdpinc=1       CDP increment                                  ",
" cdpdist=50     Distance between CDPs                          ",
" firstcdpvel=1  First CDP                                      ",
" lastcdpvel=1   Last CDP                                       ",
" cdpnum=1       Number of CDPs between velocity traces         ",
"  If outfmt=fdmodel, parameters nx through cdpnum              ",
"  are required by the ADS VIEWS program                        ",
"                                                               ",
" modname=views  Model name for FDModel output                  ",
" cdphdr=cdp     For FDModel output, trace header containing CDP",
" depthhdr=f2    For FDModel output, trace header containing depth",
"                If cdphdr and depthhdr are not used, no horizon",
"                data will be written to model_out              ",
" east=x         For Landmark ASCII format, assign X-coord to   ",
"                Landmark Easting field, and Y-coord to Landmark",
"                Northing field; east=y reverses the assignment ",
" xy=yes         Use xy=no to omit XY coordinates.              ",
"                If XYs are omitted the output file may not be  ",
"                useable, if processed by a program which expects",
"                XYs. If xy=yes, and input format does not have ",
"                XYs, then no XYs are in the output.            ",
" xhdr=gx        Trace header to use for X coordinate           ",
" yhdr=gy        Trace header to use for Y coordinate           ",
" lhdr=ep        Trace header to use for 3D iline number        ",
" xlhdr=cdp      Trace header to use for 3D xline number        ",
"                Formats par3d, vel3d, views, and sbase3 require",
"                3D line number.                                ",
" 					                        ",
" NOTES:                                                        ",
"  1)When converting from segy to ASCII, use suresamp prior to  ",
"    bhpvelutil to set trace length and sample interval to      ",
"    whatever is desired for the ASCII output.                  ",
"                                                               ",
" EXAMPLES:                                                     ",
"  TGS to Landmark:                                             ",
"    bhpvelutil infmt=tgs outfmt=lm < tgs.asc > tgs.lm          ",
"  SEGY to VIEWS Binary:                                        ",
"    bhpvelutil outfmt=fdmodel < v4sm.su model_out=views.bin \\ ",
"    dx=164 cdpmin=18800 cdpmax=20400 cdpinc=4 cdpdist=164 \\   ",
"    firstcdpvel=18800 lastcdpvel=20400 cdpnum=4 nx=401         ",
"  SEGY to SeisBase:                                            ",
"    suresamp < 2637-BES.vrmst.su nt=200 |                      ",
"    bhpvelutil > 2637-BES.sb2 outfmt=sbase2 line=1367-BS       ",
NULL};

/**************** end self doc ********************************/

/* Prototypes */
int check_fmt(char *fmt, int verbose);
int read_segy(int verbose);
void write_segy(int verbose, char *xy);
int read_par(int verbose, int *totpair);
int ccount(char c, char *s);
void write_par(int nfcns, int verbose);
void write_par3d(int *ncdps, int verbose, char *pardir);
int read_pro(int verbose, int *totpair, char *xy);
void write_pro(int verbose, char *xy);
int read_velf(int verbose, int *totpair, int ifmt);
void write_velf(int verbose);
int read_veff(int verbose, int *totpair);
void write_veff(int verbose);
int read_vel3d(int verbose, int *totpair);
void write_vel3d(int verbose);
int read_lm(int verbose, int *totpair, int *nextloc);
void write_lm(int verbose);
int read_views(int verbose, int *totpair);
void write_views(int verbose);
int init_fdmodel(char *name, int verbose);
int read_fdmodel(char *name, int verbose, int *totpair);
void write_fdmodel(int eof, int verbose);
int read_tgs(int verbose, int *totpair);
void write_tgs(int verbose);
void write_sbase2(int verbose);
void write_sbase3(int verbose);
void swapxy(float *x, float *y);
void checkval(char *name, int val);
int read_v2(int verbose, int *totpair, int *nextloc);
void write_v2(int verbose);
void write_avf(int nfcns, int verbose);

/* Globals */
/* Structure to hold auxiliary info, shot point, XYs, etc, for ASCII files */
typedef struct {
  char company[32];
  char area[32];
  char comment[64];
  char line[32];  /* Alpha-numeric line name */
  int iline;      /* Integer line number */
  int xline;      /* Integer xline number */
  char *units;    /* F or M */
  int sp;         /* Shot-point number */
  int cdp;        /* CDP/trace number */
  float xcoord;   /* X-coordinate */
  float ycoord;   /* Y-coordinate */
  } vel_fcn;       

typedef struct {    /* VIEWS FDModel */
  int nx;           /* Number of velocity traces */
  int nz;           /* Number of samples for each velocity trace */
  float dx;         /* Distance between velocity traces */
  float dz;         /* Vertical distance */
  int cdpmin;       /* Minimum cdp number of model */
  int cdpmax;       /* Maximum cdp number of model */
  int cdpinc;       /* CDP increment */
  float cdpdist;    /* Distance between CDPs */
  int firstcdpvel;  /* First Cdp */
  int lastcdpvel;   /* Last Cdp */
  int cdpnum;       /* Number of CDPs between velocity traces */
  int ntopo;        /* Number of topography pairs */
  int numHorizons;  /* Number of horizons */
  int numPts;       /* Number of points in next horizon */
  } FDModel;       

FDModel fdmodel;  /* VIEWS FDModel */

vel_fcn velfunc;  /* Velocity function */

segy vin, vout;   /* Vel traces in and out */

float *asc_td;    /* Times/depths for ASCII input/output */
float **model;    /* FDModel velocities - stored as horizons */

cwp_String xtype; /* xhdr type */
cwp_String ytype; /* yhdr type */
cwp_String ltype; /* lhdr type */
cwp_String xltype; /* xlhdr type */

int ixhdr;        /* Header index of X-coord */
int iyhdr;        /* Header index of Y-coord */
int ilhdr;        /* Header index of 3D iline */
int ixlhdr;       /* Header index of 3D xline */
int *cdpbuf;      /* CDP numbers for par3d */

char *modname;    /* FDModel Name */

FILE *imodfp;     /* model_in */
FILE *omodfp;     /* model_out */
FILE *parfp;      /* par3d */

int main (int argc, char **argv)
{

  char *infmt="",*outfmt=""; /* data formats */
  char string[32];           /* Scratch */
  char *east;                /* Landmark Easting/Northing assignment */
  char *xhdr;                /* Trace header to use for X-coord */
  char *yhdr;                /* Trace header to use for Y-coord */
  char *lhdr;                /* Trace header to use for 3D iline number */
  char *xlhdr;               /* Trace header to use for 3D xline number */
  char *xy;                  /* yes=output xy, no=don't */
  char *model_in;            /* FDModel Input File */
  char *model_out;           /* FDModel Output File */
  char *line;                /* Line name parameter */
  char *pardir;              /* directory for par3d files */
  char cmdbuf[128];          /* mkdir command for pardir */
  char *domain;              /* auto,time,depth */
  char *interp;              /* interp=yes or no for ASCII --> SEGY */

  int sp;                    /* Shot point number */
  int sp1;                   /* Shot point number */
  int cdp;                   /* CDP number */
  int totpair=0;             /* Total number of TV pair */
  int nextloc=0;             /* Next location in velfunc */
  int seqno;                 /* Record sequence number */
  int cardnum;               /* Record sequence within function */
  int ntout;                 /* Samples per trace - output */
  int NTOUT=0;               /* NTOUT=1 if ntout is specified */
  int verbose;               /* Debug print */
  int nfcns=0;               /* Number of velocity functions */
  int ncdps=0;               /* Number of CDPs in current line - par3d */
  int nrec;                  /* Number of records per function */
  int nrem;                  /* nrec % 5 for TGS vels */
  int i, j;                  /* Loop count */
  int eof=0;                 /* Set when EOF received on input */
  int ifmt=0,ofmt=0;         /* Variables for formats:
                                1=segy,2=par,3=pro,4=velf,5=veff,6=lm,7=views,
                                8=tgs,9=fdmodel,10=sbase2,11=sbase3,12=vel3d,13=par3d */

  float xcoord, ycoord;      /* X, Y coords */
  float dt=0.;               /* sample interval if ofmt=segy */
  float *times;              /* segy output times */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* check ifmt */
  if(!getparstring("infmt",&infmt))
    infmt = "segy";
  ifmt = check_fmt(infmt,verbose);

  /* check ofmt */
  if(!getparstring("outfmt",&outfmt))
    outfmt = "segy";
  ofmt = check_fmt(outfmt,verbose);

  if(verbose) {
    fprintf(stderr,"Input data format is %s\n", infmt);
    fprintf(stderr,"Output data format is %s\n", outfmt);
  }

  /* verify legal formats */
  if(!ifmt)
    err("%s is not a valid infmt\n", infmt);
  if(!ofmt)
    err("%s is not a valid outfmt\n", outfmt);

  /* avf input no implemented */
  if(ifmt == 15)
    err("Landamrk avf format has not been implemented for input\n");

  /* SEGY --> SEGY not supported */
  if(ifmt == 1 && ofmt == 1)
    err("SEGY to SEGY is not allowed\n");

  /* ASCII -> SEGY, check dt, and ntout, build times */ 
  if(ifmt > 1  && ifmt != 9 && ofmt == 1) {
    /* check interp */
    if(!getparstring("interp",&interp))
      interp = "yes";
    if(verbose && !strcmp(interp,"yes"))
      fprintf(stderr,"Interpolating output traces vertically\n");
    else if(verbose && !strcmp(interp,"no"))
      fprintf(stderr,"Not interpolating output traces\n");
    if(!strcmp(interp,"yes")) {
      if(!getparfloat("dt",&dt))
        dt = 0.004;
      if(!getparint("ntout",&ntout))
        ntout = 1000;
      if(verbose)
        fprintf(stderr,"Writing SU output with sampling interval = %f seconds and trace length = %d samples\n",dt,ntout);
      vout.ns = ntout;
      vout.dt = 1000000 * dt;
      NTOUT = 1;
      times = calloc(ntout,sizeof(float));
      times[0] = 0.0;
      for(i=1; i<ntout; i++)
        times[i] = times[i-1] + dt;
    }
  }

  /* check ntout */
  if(!getparint("ntout",&ntout)) {
    /* if input is segy, use input trace length */
    if(ifmt == 1)
      ntout = -1;
  }
  else if(ntout) {
    NTOUT = 1;
    vout.ns = ntout;
  }

  /* check whether to write XYs */
  if(!getparstring("xy",&xy))
    xy = "yes";
  if(verbose)
    if(!strcmp(xy,"yes"))
      fprintf(stderr,"Writing XYs to output\n");
    else
      fprintf(stderr,"Not writing XYs to output\n");

  /* Initialize XY, 3D line number trace header info */
  if(ifmt == 1 || ofmt == 1) {
    if(!getparstring("xhdr",&xhdr))
      xhdr = "gx";
    if(!getparstring("yhdr",&yhdr))
      yhdr = "gy";
    ixhdr = getindex(xhdr);
    iyhdr = getindex(yhdr);
    xtype = hdtype(xhdr);
    ytype = hdtype(yhdr);
    if(verbose)
      fprintf(stderr,"Using %s for xhdr, and %s for yhdr\n", xhdr,yhdr);
    /* 3D iline trace header */
    if(!getparstring("lhdr",&lhdr))
      lhdr = "ep";
    ilhdr = getindex(lhdr);
    ltype = hdtype(lhdr);
    /* 3D xline trace header */
    if(!getparstring("xlhdr",&xlhdr))
      xlhdr = "cdp";
    ixlhdr = getindex(xlhdr);
    xltype = hdtype(xlhdr);
    if(verbose)
      fprintf(stderr,"Using %s for lhdr and %s for xlhdr\n",lhdr,xlhdr);
  }


  /* If reading SEGY, check first trace */
  if(ifmt == 1) {
    /* First vel trace */
    eof = read_segy(verbose);
    if(eof == EOF)
      err("Cannot get first vel trace\n");
    /* Show input time or depth interval */
    fprintf(stderr,"Raw sampling interval from first trace = %d\n",vin.dt);
    /* if dt >= 1000, assume time, else depth */
    if(vin.dt >= 1000) {
      fprintf(stderr,"ASSUMING DOMAIN = TIME\n");
      fprintf(stderr,"Input sample interval in seconds = %f\n", (float)(vin.dt)/1000000.);
      domain = "time";
    }
    else {
      fprintf(stderr,"ASSUMING DOMAIN = DEPTH\n");
      fprintf(stderr,"Input sample interval in feet/meters = %f\n", (float)vin.dt);
      domain = "depth";
    }
    /* If ntout wasn't set, use input or estimate */
    if(ntout == -1)
      ntout = vin.ns;
    fprintf(stderr,"ntout set to %d\n", ntout);
  }

  /* If reading or writing ASCII, allocate asc_td */
  if(ifmt > 1 || ofmt > 1) {
    /* segy in, use ntout */
    if(ifmt == 1) {
      asc_td = calloc(ntout,sizeof(float));
      /* Fill td from first trace */
      asc_td[0] = vin.delrt;
      for(i=1; i<ntout; i++)
        if(!strcmp(domain,"time"))
          asc_td[i] = asc_td[i-1] + (float)vin.dt/1000.; 
        else
          asc_td[i] = asc_td[i-1] + (float)vin.dt;
      if(verbose)
        fprintf(stderr,"Allocated %d floats for asc_td\n", ntout);
    }
    /* ASCII input, use ntout if specified, otherwise 4096 */
    else {
      if(!NTOUT)
        ntout = 4096;
      asc_td = calloc(ntout,sizeof(float));
      if(verbose)
        fprintf(stderr,"Allocated %d floats for asc_td\n", ntout);
    }
  }

  /* Do format-specific stuff for ASCII formats */
  /* If reading/writing LM, get east/north assignment */
  if(ifmt == 6 || ofmt == 6) {
    if(!getparstring("east",&east))
      east = "x";
    if(verbose)
      if(!strcmp(east,"x"))
        fprintf(stderr,"Assigning X to Easting, and Y to Northing\n");
      else if(!strcmp(east,"y"))
       fprintf(stderr,"Assigning X to Northing, and Y to Easting\n");
    if(strcmp(east,"x") && strcmp(east,"y")) {
      fprintf(stderr,"east = %s\n", east);
      err("east must be x or y\n");
    }
  }

  /* If output is velf, write heading */
  if(ofmt == 4) {
    printf("CONT BHP Petroleum\n");
    printf("AREA Gulf of Mexico\n");
    printf("INFO \n");
    printf("COM1 \n");
    printf("COM2 \n");
  }

  /* If output is veff, write heading */
  if(ofmt == 5)
    printf("INFO \n");
  
  /* If output is vel3d, check units */
  if(ofmt == 12) {
    if(!getparstring("units",&velfunc.units))
      velfunc.units = "M";
    if(verbose)
      fprintf(stderr,"Setting units to %s\n", velfunc.units);
    if(strcmp(velfunc.units,"F") && strcmp(velfunc.units,"M"))
      err("%s is illegal for units\n", velfunc.units);
  }

  /* if input is vel3d, allocate units */
  if(ifmt == 12)
    velfunc.units = calloc(2,sizeof(char));
  
  /* If output is views, write heading */
  if(ofmt == 7)
    printf("INFO VIEWS Velocities created by bhpvelutil\n");
  
  /* get line name for output, if specified */
  if(getparstring("line",&line))
    strcpy(velfunc.line,line);

  /* For fdmodel input, model_in is required */
  if(ifmt == 9) {
    if(!getparstring("model_in",&model_in))
      err("model_in is required for infmt=fdmodel\n");
  }

  /* For fdmodel output, extra parms are required */
  if(ofmt == 9) {
    if(!getparstring("model_out",&model_out))
      err("model_out is required for outfmt=fdmodel\n");
    if(!getparfloat("dx",&fdmodel.dx))
      fdmodel.dx = 50.;
    if(!getparint("cdpmin",&fdmodel.cdpmin))
      fdmodel.cdpmin = 1;
    if(!getparint("cdpmax",&fdmodel.cdpmax))
      fdmodel.cdpmax = 100;
    if(!getparint("cdpinc",&fdmodel.cdpinc))
      fdmodel.cdpinc = 1;
    if(!getparfloat("cdpdist",&fdmodel.cdpdist))
      fdmodel.cdpdist = 50.;
    if(!getparint("firstcdpvel",&fdmodel.firstcdpvel))
      fdmodel.firstcdpvel = 1;
    if(!getparint("lastcdpvel",&fdmodel.lastcdpvel))
      fdmodel.firstcdpvel = 1;
    if(!getparint("cdpnum",&fdmodel.cdpnum))
      fdmodel.cdpnum = 1;
    if(!getparint("nx",&fdmodel.nx))
      err("nx is required for outfmt=fdmodel\n");
    /* modname is optional */
    if(!getparstring("modname",&modname))
      modname = "views";
  }

  if(verbose) {
    if(ifmt == 9)
      fprintf(stderr,"FDModel input file is %s\n", model_in);
    if(ofmt == 9) {
      fprintf(stderr,"FDModel output file is %s\n", model_out);
      fprintf(stderr,"FDModel name is %s\n", modname);
    }
  }

  /* If output is FDModel initialize binary file */
  if(ofmt == 9)
    if(init_fdmodel(model_out,verbose))
      err("Error opening %s\n", model_out);

  /* if writing par or par3d, allocate space to hold up to 4096 CDP numbers */
  if(ofmt == 2 || ofmt == 13)
    cdpbuf = calloc(4096,sizeof(int));
  /* if output is par3d, check pardir */
  if(ofmt ==  13) {
    if(!getparstring("pardir",&pardir))
      pardir = "3dvels";
    strcpy(cmdbuf,"mkdir ");
    strcat(cmdbuf,pardir);
    parfp = popen(cmdbuf,"w");
    pclose(parfp);
  }

  /* Loop until done */
  for(;;) {

    /* Clear trace if reading ASCII */
    if(ifmt > 1)
      for(i=0; i<ntout; i++)
        vin.data[i] = 0;

    /* Read next input record; first SEGY already done */
    if(ifmt == 1 && nfcns)
      eof = read_segy(verbose);
    else if(ifmt == 2)
      eof = read_par(verbose,&totpair);
    else if(ifmt == 3)
      eof = read_pro(verbose,&totpair,xy);
    else if(ifmt == 41 || ifmt == 42 || ifmt == 43)
      eof = read_velf(verbose,&totpair,ifmt);
    else if(ifmt == 5)
      eof = read_veff(verbose,&totpair);
    else if(ifmt == 6)
      eof = read_lm(verbose,&totpair,&nextloc);
    else if(ifmt == 7)
      eof = read_views(verbose,&totpair);
    else if(ifmt == 8)
      eof = read_tgs(verbose,&totpair);
    else if(ifmt == 9)
      eof = read_fdmodel(model_in,verbose,&totpair);
    else if(ifmt == 12)
      eof = read_vel3d(verbose,&totpair);
    else if(ifmt == 14)
      eof = read_v2(verbose,&totpair,&nextloc);

    /* EOF? */
    if(eof == EOF && vin.ns == 0)
      break;

    /* copy header */
    memcpy((void *)&vout,(const void *)&vin,HDRBYTES);

    /* If ASCII --> SEGY, interpolate output trace */
    if(ifmt != 1 && ifmt != 9 && ofmt == 1) {
      if(!strcmp(interp,"yes")) {
        /* convert asc_td to seconds */
        for(i=0; i<vin.ns; i++)
          asc_td[i] *= 0.001;
        intlin((int)vin.ns,asc_td,vin.data,vin.data[0],vin.data[(int)vin.ns-1],
               ntout,times,vout.data);
      }
      else {
        vout.dt = asc_td[1] - asc_td[0];
        vout.ns = vin.ns;
        for(i=0; i<vin.ns; i++)
          vout.data[i] = vin.data[i];
      }
    }
    /* else copy trace */
    else {
      for(i=0; i<vin.ns; i++)
        vout.data[i] = vin.data[i];
    }

    /* Use ntout and dt if present */
    if(NTOUT)
      vout.ns = ntout;
    if(dt)
      vout.dt = 1000000 * dt;

    /* Write output */
    if(ofmt == 1)
      write_segy(verbose,xy);
    else if(ofmt == 2)
      write_par(nfcns,verbose);
    else if(ofmt == 3)
      write_pro(verbose,xy);
    else if(ofmt == 41)
      write_velf(verbose);
    else if(ofmt == 5)
      write_veff(verbose);
    else if(ofmt == 6)
      write_lm(verbose);
    else if(ofmt == 7)
      write_views(verbose);
    else if(ofmt == 8)
      write_tgs(verbose);
    else if(ofmt == 9)
      write_fdmodel(nfcns,verbose);
    else if(ofmt == 10)
      write_sbase2(verbose);
    else if(ofmt == 11)
      write_sbase3(verbose);
    else if(ofmt == 12)
      write_vel3d(verbose);
    else if(ofmt == 13)
      write_par3d(&ncdps,verbose,pardir);
    else if(ofmt == 14)
      write_v2(verbose);
    else if(ofmt == 15)
      write_avf(nfcns,verbose);

    /* Bump counter */
    nfcns++;

    /* If last trace is followed by EOF */
    if(eof == EOF)
      break;

  }

  /* If output is par or par3d, write CDPS record */
  if(ofmt == 2) {
    printf("cdp=");
    for(i=0; i<nfcns-1; i++)
      printf("%d,",cdpbuf[i]);
    printf("%d\n", cdpbuf[nfcns-1]);
  }
  if(ofmt == 13) {
    fprintf(parfp,"cdp=");
    for(i=0; i<ncdps-1; i++)
      fprintf(parfp,"%d,",cdpbuf[i]);
    fprintf(parfp,"%d\n", cdpbuf[ncdps-1]);
    fclose(parfp);
  }

  fprintf(stderr,"Processed %d traces\n", nfcns);

  /* Close files */
  if(ifmt == 9)
    fclose(imodfp);
  if(ofmt == 9)
    fclose(omodfp);

  return EXIT_SUCCESS;

}

int read_segy(int verbose)
{

  float factor;

  int n;

  Value val;

  if(!gettr(&vin)) {
    vin.ns = 0;
    return EOF;
  }

  /* Coordinate scalar */
  factor = vin.scalco;
  if(!factor)
    factor = 1.0;
  else if(factor < 0)
    factor = -1. / factor;

  /* Vel fcn stuff */
  gethval(&vin,ixhdr,&val);
  velfunc.xcoord = vtof(xtype,val);
  velfunc.xcoord *= factor;
  gethval(&vin,iyhdr,&val);
  velfunc.ycoord = vtof(ytype,val);
  velfunc.ycoord *= factor;
  velfunc.sp = vin.ep;
  velfunc.cdp = vin.cdp;
  gethval(&vin,ilhdr,&val);
  velfunc.iline = vtoi(ltype,val);
  gethval(&vin,ixlhdr,&val);
  velfunc.xline = vtoi(xltype,val);

  return vin.ns;

}

void write_segy(int verbose, char *xy)
{

  Value hval;

  float factor=1.0;

  /* If xy, get X,Y from velfunc */
  if(!strcmp(xy,"yes")) {
    /* If scalco is set, apply scalar */
    if(vout.scalco > 1)
      factor = 1. / (int)vout.scalco;
    else if(vout.scalco < 0)
      factor = -(int)vout.scalco;
    hval.i = factor * velfunc.xcoord;
    puthval(&vout,ixhdr,&hval);
    hval.i = factor * velfunc.ycoord;
    puthval(&vout,iyhdr,&hval);
  }
  /* get line, cdp */
  if(velfunc.cdp)
    vout.cdp = velfunc.cdp;
  if(velfunc.iline) {
    hval.i = velfunc.iline;
    puthval(&vout,ilhdr,&hval);
  }
  if(velfunc.xline) {
    hval.i = velfunc.xline;
    puthval(&vout,ixlhdr,&hval);
  }
  if(velfunc.sp)
    vout.ep = velfunc.sp;

  puttr(&vout);

}

int read_par(int verbose, int *totpair)
{

  char record1[4096], record2[4096];

  char *eqptr;
  char *name1, *name2;
  char *val1, *val2;

  int i;
  int stat;
  int nt, nv;

  static int ncdp=0;
  static int cdpnum=0;
  static int *cdps;

  /* count and load cdps */
  if(ncdp == 0) {
    while((stat = scanf("%s\n", &record1)) != EOF) {
      if(strstr(record1,"cdp") != NULL) {
        eqptr = strchr(record1,'=');
        val1 = eqptr + 1;
        ncdp = ccount(',',val1) + 1;
        if(verbose)
          fprintf(stderr,"Found %d CDPs\n",ncdp);
        cdps = calloc(ncdp,sizeof(int));
        /* load CDPs */
        for(i=0; i<ncdp; i++) {
          cdps[i] = atoi(val1);
          while(*val1++ != ',');
        }
        if(verbose) {
          for(i=0; i<ncdp; i++)
            fprintf(stderr,"%d ",cdps[i]);
          fprintf(stderr,"\n");
        }
      }
    }
    if(cdps == 0)
      err("No cdp record found in parfile\n");
    rewind(stdin);
  }
  /* Separate names, values */
  if((stat = scanf("%s\n", &record1)) == EOF) {
    vin.ns = 0;
    return stat;
  }
  if((stat = scanf("%s\n", &record2)) == EOF)
    err("Premature EOF reading par file\n");
  if(cdpnum >= ncdp)
    err("parfile has more tnmo,vnmo pairs than cdps\n");
  if(strstr(record1,"time") != NULL)
    name1 = "time";
  else if(strstr(record1,"tnmo") != NULL)
    name1 = "tnmo";
  else
    name1 = "";
  eqptr = strchr(record1,'=');
  val1 = eqptr + 1;
  if(strstr(record2,"vel") != NULL)
    name2 = "vel";
  else if(strstr(record2,"vnmo") != NULL)
    name2 = "vnmo";
  else
    name2 = "";
  eqptr = strchr(record2,'=');
  val2 = eqptr + 1;

  /* Verify a tnmo or time + a vnmo or vel */
  if((strcmp(name1,"time") && strcmp(name1,"tnmo")) ||
     (strcmp(name2,"vel") && strcmp(name2,"vnmo"))) {
    fprintf(stderr,"name1, name2 %s %s\n", name1,name2);
    fprintf(stderr,"Should be time vel or tnmo vnmo\n");
    err("Illegal names in par velocity file\n");
  }

  /* Get number of times and vels, and verify equal */
  nt = ccount(',',val1) + 1;
  nv = ccount(',',val2) + 1;

  if(nt != nv) {
    fprintf(stderr,"CDP %d, has %d times, and %d vels\n", cdps[cdpnum], nt, nv);
    err("Number of times must be equal number of vels\n");
  }

  /* Set velfunc */
  velfunc.iline = 0;
  velfunc.cdp = cdps[cdpnum];
  velfunc.xcoord = 0.;
  velfunc.ycoord = 0.;
  vin.ns = nt;
  /* Load times, vels to traces */
  for(i=0; i<nt; i++) {
    asc_td[i] = atof(val1);
    /* mills */
    asc_td[i] *= 1000.;
    vin.data[i] = atof(val2);
    val1++;
    val2++;
    while(*val1++ != ',');
    while(*val2++ != ',');
  }
  cdpnum++;
  if(cdpnum == ncdp)
    stat = EOF;

  if(verbose) {
    for(i=0; i<nt; i++)
      fprintf(stderr,"%f %f ", asc_td[i],vin.data[i]);
    fprintf(stderr,"\n");
  }

  return stat;

}
int ccount (char c, char *s)
{
  int i, count;

  for(i=0,count=0; s[i]!=0; i++)
    if(s[i] == c) count++;
  return count;

}
void write_par(int nfcns, int verbose)
{

  int i;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("CDP",velfunc.cdp);
  }

  /* Save CDPs until done */
  cdpbuf[nfcns] = velfunc.cdp;
  /* Times */
  printf("tnmo=%g", 0.001*asc_td[0]);
  for(i=1; i<vout.ns; i++)
    printf(",%g", 0.001*asc_td[i]);
  printf("\n");
  /* Vels */
  printf("vnmo=%g", vout.data[0]);
  for(i=1; i<vout.ns; i++)
    printf(",%g", vout.data[i]);
  printf("\n");

}

void write_par3d(int *ncdps, int verbose, char *dir)
{

  static char name[256];
  char iline[10];

  int i;

  static int line=0;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("CDP",velfunc.cdp);
    checkval("LINE",velfunc.iline);
    /* establish 3dvels directory */
  }

  /* If new line, close old file, and start new one */
  if(line == 0 || (line != 0 && velfunc.iline != line)) {
    if(line) {
      fprintf(parfp,"cdp=%d", cdpbuf[0]);
      for(i=1; i<*ncdps; i++)
        fprintf(parfp,",%d", cdpbuf[i]);
      fprintf(parfp,"\n");
      fclose(parfp);
    }
    strcpy(name,dir);
    strcat(name,"/");
    strcat(name,"line_");
    sprintf(iline,"%d",velfunc.iline);
    strcat(name,iline);
    strcat(name,".par3d");
    parfp = fopen(name,"w+");
    *ncdps = 0;
    line = velfunc.iline;
  }

  /* Save CDPs until done */
  cdpbuf[*ncdps] = velfunc.cdp;
  (*ncdps)++;
  /* Times */
  fprintf(parfp,"tnmo=%g", 0.001*asc_td[0]);
  for(i=1; i<vout.ns; i++)
    fprintf(parfp,",%g", 0.001*asc_td[i]);
  fprintf(parfp,"\n");
  /* Vels */
  fprintf(parfp,"vnmo=%g", vout.data[0]);
  for(i=1; i<vout.ns; i++)
    fprintf(parfp,",%g", vout.data[i]);
  fprintf(parfp,"\n");

}

int read_pro(int verbose, int *totpair, char *xy)
{

  static char record[81];

  static int data=0;
  static int hold=0;
  static int first=1;

  float fcdp;

  /* Reset trace len */
  vin.ns = 0;

  /* if hold is set, first line of next trace is in record */
  if(hold) {
    if(sscanf(record,"%f %f %f %f %f",&fcdp,&velfunc.xcoord,&velfunc.ycoord,
                                        &asc_td[vin.ns],&vin.data[vin.ns]) == 5) {
      /*fprintf(stderr,"cdp,x,y,t,v %f %f %f %f %f\n", fcdp,velfunc.xcoord,velfunc.ycoord,
                                                     asc_td[vin.ns],vin.data[vin.ns]);*/
    }
    /* 3 floats --> cdp,t,v */
    else if(sscanf(record,"%f %f %f",&fcdp,&asc_td[vin.ns],&vin.data[vin.ns]) == 3) {
      /*fprintf(stderr,"cdp,t,v %f %f %f\n", fcdp,asc_td[vin.ns],vin.data[vin.ns]);*/
    }
    else
      err("Unrecognized record encountered in ProMAX file %s\n", record);
    velfunc.cdp = (int)fcdp;
    vin.ns = 1;
    hold = 0;
  }
  
  while(fgets(record,256,stdin) != NULL) {
    if(data) {
      /* 5 floats --> cdp,x,y,t,v */
      if(sscanf(record,"%f %f %f %f %f",&fcdp,&velfunc.xcoord,&velfunc.ycoord,
                                        &asc_td[vin.ns],&vin.data[vin.ns]) == 5) {
        /* If not first time, hold record and return trace */
        if(!first) {
          hold = 1;
          break;
        }
        else
          first = 0;
        velfunc.cdp = (int)fcdp;
        /*fprintf(stderr,"cdp,x,y,t,v %d %f %f %f %f\n", velfunc.cdp,velfunc.xcoord,velfunc.ycoord,
                                                       asc_td[vin.ns],vin.data[vin.ns]);*/
        vin.ns++;
      }
      /* 3 floats --> cdp,t,v */
      else if(sscanf(record,"%f %f %f",&fcdp,&asc_td[vin.ns],&vin.data[vin.ns]) == 3) {
        /* If not first time, hold record and return trace */
        if(!first) {
          hold = 1;
          break;
        }
        else
          first = 0;
        velfunc.cdp = (int)fcdp;
        /*fprintf(stderr,"cdp,t,v %d %f %f\n", velfunc.cdp,asc_td[vin.ns],vin.data[vin.ns]);*/
        vin.ns++;
      }
      /* 2 floats --> t,v */
      else if(sscanf(record,"%f %f",&asc_td[vin.ns],&vin.data[vin.ns]) == 2) {
        /*fprintf(stderr,"t,v %f %f\n", asc_td[vin.ns],vin.data[vin.ns]);*/
        vin.ns++;
      }
    }
    /* Line of ----- signals start of data */
    if(!data && !strncmp(record,"----",4)) {
      data = 1;
      vin.ns = 0;
      continue;
    }
  }

  if(!vin.ns)
    return EOF;
  else
    return 0;

}

void write_pro(int verbose, char *xy)
{

  char string[16];

  int i;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("CDP",velfunc.cdp);
    if(!strcmp(xy,"yes")) {
      checkval("XCOORD",(int)velfunc.xcoord);
      checkval("YCOORD",(int)velfunc.ycoord);
    }
  }

  if(!strcmp(xy,"yes")) {
    printf("%10d%10.1f%10.1f%10.1f%10.1f\n", velfunc.cdp,velfunc.xcoord,
                                               velfunc.ycoord,asc_td[0],vout.data[0]);
    for(i=1; i<vout.ns; i++)
      printf("                              %10.1f%10.1f\n", asc_td[i],vout.data[i]);
  }
  else {
    printf("%10d%10.1f%10.1f\n", velfunc.cdp,asc_td[0],vout.data[0]);
    for(i=1; i<vout.ns; i++)
      printf("          %10.1f%10.1f\n", asc_td[i],vout.data[i]);
  }

}

int read_velf(int verbose, int *totpair, int ifmt)
{

  static char record[81];

  float f1;

  int i;
  int eof;
  static int saved=0;
  static int first_velf;   /* first VELF record of each group */
  int i1, i2;

  velfunc.iline = 0;

  /* Loop until all TV pairs for a SPNT are read or EOF */
  for(;;) {
    if(!saved) {
      eof = scanf("%s", record);
      if(eof == EOF)
        break;
    }
    saved = 0;
    /* Skip comments etc */
    if(!strncmp(record,"CONT",4))
      continue;
    if(!strncmp(record,"AREA",4))
      continue;
    if(!strncmp(record,"INFO",4))
      continue;
    if(!strncmp(record,"COM1",4))
      continue;
    if(!strncmp(record,"COM2",4))
      continue;
    /* If LINE get line num */
    else if(!strncmp(record,"LINE",4))
      scanf("%d", &velfunc.iline);
    /* If SPNT, get sp , X, Y */
    else if(ifmt == 41 && !strncmp(record,"SPNT",4)) {
      scanf("%d %f %f", &velfunc.sp,&velfunc.xcoord,&velfunc.ycoord);
      vin.ns = 0;
    }
    else if(ifmt == 42 && !strncmp(record,"SPNT",4)) {
      scanf("%d %f %f %f %d", &velfunc.sp,&f1,&velfunc.xcoord,&velfunc.ycoord,&i2);
      vin.ns = 0;
      first_velf = 1;
    }
    /* if * get SP, X, Y */
    else if(!strncmp(record,"*",1)) {
      scanf("%s %f %d %d",record,&f1,&i1,&i2);
      velfunc.sp = f1;
      velfunc.xcoord = i1;
      velfunc.ycoord = i2;
      vin.ns = 0;
    }
    else if(!strncmp(record,"VELF",4)) {
      /* Read TV pairs until EOF or no match */
      for(;;) {
        if(first_velf) {
          eof = scanf(" %d %d %f %f", &i1,&i2,&asc_td[vin.ns],&vin.data[vin.ns]);
          first_velf = 0;
        }
        else
          eof = scanf("%f %f", &asc_td[vin.ns],&vin.data[vin.ns]);
        if(eof == EOF)
          break;
        /* Check for more VELF data */
        else if(eof == 0) {
          eof = scanf("%s", record);
          if(!strncmp(record,"VELF",4))
            continue;
          else {
            saved = 1;
            break;
          }
        }
        else {
          (*totpair)++;
          vin.ns++;
        }
      }
      break;
    }
  }

  if(verbose) {
    fprintf(stderr,"SPNT: %d, X: %f, Y: %f\n", velfunc.sp,velfunc.xcoord,velfunc.ycoord);
    for(i=0; i<vin.ns; i++)
      fprintf(stderr," %f %f ", asc_td[i],vin.data[i]);
    fprintf(stderr,"\n");
  }

  if(eof == EOF && !vin.ns)
    vin.ns = 0;
  return eof;
}

void write_velf(int verbose)
{

  char string[16];

  int i, j;
  int nrec;
  int nrem;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("SP",velfunc.sp);
    checkval("LINE",velfunc.iline);
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
  }

  printf("LINE %10d\n", velfunc.iline);
  printf("SPNT %10d          %10d%10d\n", velfunc.sp,(int)velfunc.xcoord,(int)velfunc.ycoord);

  nrec = vout.ns / 5;
  nrem = vout.ns % 5;
  if(nrem)
    nrec++;

  if(verbose) {
    if(!nrem)
      fprintf(stderr,"Writing %d VT pair to %d records\n", vout.ns,nrec);
    else
      fprintf(stderr,"Writing %d VT pair to first %d records, %d to last record\n",
                      vout.ns-nrem,nrec-1,nrem);
  }

  for(i=0; i<nrec-1; i++) {
    printf("VELF                ");
    for(j=0; j<5; j++)
      printf("%5d%5d", (int)asc_td[5*i+j],(int)vout.data[5*i+j]);
    printf("\n");
  }
  if(nrem)
    i = nrem;
  else
    i = 5;
  printf("VELF                ");
  for(j=0; j<i; j++)
    printf("%5d%5d", (int)asc_td[5*(nrec-1)+j],(int)vout.data[5*(nrec-1)+j]);
  printf("\n");

}

int read_veff(int verbose, int *totpair)
{

  static char record[81];

  int i;
  int eof;
  static int saved=0;

  velfunc.iline = 0;

  /* Loop until all VT pairs for a location are read or EOF */
  for(;;) {
    if(!saved) {
      eof = scanf("%s", record);
      if(eof == EOF)
        break;
    }
    saved = 0;
    /* Skip comments etc */
    if(!strncmp(record,"INFO",4))
      continue;
    /* If LINE get line num */
    else if(!strncmp(record,"LINE",4))
      scanf("%d", &velfunc.iline);
    /* If VELSSP, get sp , X, Y */
    else if(!strncmp(record,"VELSSP",6)) {
      scanf("%d %f %f", &velfunc.sp,&velfunc.xcoord,&velfunc.ycoord);
      vin.ns = 0;
    }
    else if(!strncmp(record,"VEFF",4)) {
      /* Read VT pairs until EOF or no match */
      for(;;) {
        eof = scanf("%f %f", &vin.data[vin.ns],&asc_td[vin.ns]);
        if(eof == EOF)
          break;
        /* Check for more VEFF data */
        else if(eof == 0) {
          eof = scanf("%s", record);
          if(!strncmp(record,"VEFF",4))
            continue;
          else {
            saved = 1;
            break;
          }
        }
        else {
          (*totpair)++;
          vin.ns++;
        }
      }
      break;
    }
  }

/*
  if(verbose) {
    fprintf(stderr,"VELSSP: %d, X: %f, Y: %f\n", velfunc.sp,velfunc.xcoord,velfunc.ycoord);
    for(i=0; i<vin.ns; i++)
      fprintf(stderr," %f %f ", vin.data[i],asc_td[i]);
    fprintf(stderr,"\n");
  }
*/

  if(eof == EOF && !vin.ns)
    vin.ns = 0;
  return eof;
}

void write_veff(int verbose)
{

  char string[16];
  int i, j;
  int nrec;
  int nrem;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("SP",velfunc.sp);
    checkval("LINE",velfunc.iline);
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
  }

  printf("LINE  %6d\n", velfunc.iline);
  printf("VELSSP %5d          %11.3f %11.3f\n", velfunc.sp,velfunc.xcoord,velfunc.ycoord);

  nrec = vout.ns / 6;
  nrem = vout.ns % 6;
  if(nrem)
    nrec++;

/*
  if(verbose) {
    if(!nrem)
      fprintf(stderr,"Writing %d VT pair to %d records\n", vout.ns,nrec);
    else
      fprintf(stderr,"Writing %d VT pair to first %d records, %d to last record\n",
                      vout.ns-nrem,nrec-1,nrem);
  }
*/

  for(i=0; i<nrec-1; i++) {
    printf("VEFF  ");
    for(j=0; j<6; j++)
      printf(" %5d %5d", (int)vout.data[6*i+j],(int)asc_td[6*i+j]);
    printf("\n");
  }
  if(nrem)
    i = nrem;
  else
    i = 6;
  printf("VEFF  ");
  for(j=0; j<i; j++)
    printf(" %5d %5d", (int)vout.data[6*(nrec-1)+j],(int)asc_td[6*(nrec-1)+j]);
  printf("\n");

}

int read_vel3d(int verbose, int *totpair)
{

  static char record[81];

  int i;
  int eof;
  static int saved=0;

  velfunc.iline = 0;
  velfunc.xline = 0;

  /* Loop until all VT pairs for a location are read or EOF */
  for(;;) {
    if(!saved) {
      eof = scanf("%s", record);
      if(eof == EOF)
        break;
    }
    saved = 0;
    /* Skip comments etc */
    if(!strncmp(record,"INFO",4))
      continue;
    /* If VEL3D , get xline,iline,units,x,y */
    else if(!strncmp(record,"VEL3D",5)) {
      scanf("%d %d %s %d %f", &velfunc.xline,&velfunc.iline,velfunc.units,
                              &velfunc.xcoord,&velfunc.ycoord);
      vin.ns = 0;
    }
    /* Read VT pairs until EOF or next VEL3D */
    for(;;) {
      eof = scanf("%f %f", &vin.data[vin.ns],&asc_td[vin.ns]);
      if(eof == 2) {
        (*totpair)++;
        vin.ns++;
      }
      else if(eof == EOF)
        break;
      /* Check for more data */
      else if(eof == 0) {
        eof = scanf("%s", record);
        if(!strncmp(record,"VEL3D",5)) {
          saved = 1;
          break;
        }
      }
    }
    break;
  }

/*
  if(verbose) {
    fprintf(stderr,"VELSSP: %d, X: %f, Y: %f\n", velfunc.sp,velfunc.xcoord,velfunc.ycoord);
    for(i=0; i<vin.ns; i++)
      fprintf(stderr," %f %f ", vin.data[i],asc_td[i]);
    fprintf(stderr,"\n");
  }
*/

  return eof;
}

void write_vel3d(int verbose)
{

  char string[16];

  int i, j;
  int nrec;
  int nrem;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("XLINE",velfunc.xline);
    checkval("ILINE",velfunc.iline);
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
  }
  
  printf("VEL3D %6d%6d %1s %10d%10d\n", velfunc.xline,velfunc.iline,
                                       velfunc.units,(int)velfunc.xcoord,(int)velfunc.ycoord);

  nrec = vout.ns / 6;
  nrem = vout.ns % 6;
  if(nrem)
    nrec++;

/*
  if(verbose) {
    if(!nrem)
      fprintf(stderr,"Writing %d VT pair to %d records\n", vout.ns,nrec);
    else
      fprintf(stderr,"Writing %d VT pair to first %d records, %d to last record\n",
                      vout.ns-nrem,nrec-1,nrem);
  }
*/

  for(i=0; i<nrec-1; i++) {
    for(j=0; j<6; j++)
      printf("%6d%6d", (int)vout.data[6*i+j],(int)asc_td[6*i+j]);
    printf("\n");
  }
  if(nrem)
    i = nrem;
  else
    i = 6;
  for(j=0; j<i; j++)
    printf("%6d%6d", (int)vout.data[6*(nrec-1)+j],(int)asc_td[6*(nrec-1)+j]);
  printf("\n");

}

int read_lm(int verbose, int *totpair, int *nextloc)
{

  int stat;

  static float xc, yc, td, vel;

  for(;;) {
    /* Check for read-ahead */
    if(*nextloc == 1) {
      velfunc.xcoord = xc;
      velfunc.ycoord = yc;
      asc_td[0] = td;
      vin.data[0] = vel;
      vin.ns= 1;
      *nextloc = 0;
    }
    if((stat = scanf("%f %f %f %f\n", &xc,&yc,&td,&vel)) == EOF) {
      if(!vin.ns)
        vin.ns = 0;
      break;
    }
    /* First record in file */
    if(vin.ns == 0) {
      velfunc.xcoord = xc;
      velfunc.ycoord = yc;
      asc_td[vin.ns] = td;
      vin.data[vin.ns] = vel;
      vin.ns++;
    }
    /* More records, same location */
    else if(xc == velfunc.xcoord && yc == velfunc.ycoord) {
      asc_td[vin.ns] = td;
      vin.data[vin.ns] = vel;
      vin.ns++;
    }
    /* New location */
    else if(xc != velfunc.xcoord || yc != velfunc.ycoord) {
      *nextloc = 1;
      break;
    }
  }

  (*totpair) += vin.ns;
  return stat;

}
void write_lm(int verbose)
{

  int i;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
  }

  for(i=0; i<vout.ns; i++)
    printf("%20.1f %20.1f %10.1f %10.1f\n", velfunc.xcoord,velfunc.ycoord,
           asc_td[i],vout.data[i]);

}

void swapxy(float *x, float *y)
{

  float t;

  t = *x;
  *x = *y;
  *y = t;

}

int read_views(int verbose, int *totpair)
{

  static char record[81];

  int i;
  int eof;
  static int saved=0;

  /* Loop until all VT pairs for a CDP are read or EOF */
  for(;;) {
    if(!saved) {
      eof = scanf("%s", record);
      if(eof == EOF)
        break;
    }
    saved = 0;
    /* Skip INFO records */
    if(!strncmp(record,"INFO",4))
      continue;
    /* If LINE get line num */
    else if(!strncmp(record,"LINE",4))
      scanf("%d", &velfunc.iline);
    /* If CDP, get cdpnum, X, Y */
    else if(!strncmp(record,"CDP",3)) {
      scanf("%d %f %f", &velfunc.cdp,&velfunc.xcoord,&velfunc.ycoord);
      vin.ns = 0;
    }
    else if(!strncmp(record,"VELRMS",6)) {
      /* Read VT pairs until EOF or no match */
      for(;;) {
        eof = scanf("%f %f", &vin.data[vin.ns],&asc_td[vin.ns]);
        if(eof == EOF)
          break;
        /* Check for more VELRMS data */
        else if(eof == 0) {
          eof = scanf("%s", record);
          if(!strncmp(record,"VELRMS",6))
            continue;
          else {
            saved = 1;
            break;
          }
        }
        else {
          (*totpair)++;
          vin.ns++;
        }
      }
      break;
    }
  }

/*
  if(verbose) {
    fprintf(stderr,"LINE,CDP: %d, %d, X: %f, Y: %f\n", velfunc.iline,velfunc.cdp,
            velfunc.xcoord,velfunc.ycoord);
    for(i=0; i<vin.ns; i++)
      fprintf(stderr," %f %f ", vin.data[i],asc_td[i]);
    fprintf(stderr,"\n");
  }
*/

  if(eof == EOF && !vin.ns)
    vin.ns = 0;
  return eof;

}

void write_views(int verbose)
{

  int nvel;
  int rem;
  int i, j;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("LINE",velfunc.iline);
    checkval("CDP",velfunc.cdp);
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
  }

  /* If line non-zero, write LINE record */
  printf("LINE      %10d\n", velfunc.iline);
  /* CDP, X, Y */
  printf("CDP       %10d     %10d     %10d\n", velfunc.cdp,(int)velfunc.xcoord,(int)velfunc.ycoord);
  /* VT pairs */
  nvel = vout.ns / 6;
  rem = vout.ns % 6;
  if(rem)
    nvel++;
  for(i=0,j=0; i<nvel-1; i++,j+=6) {
    printf("VELRMS ");
    printf("%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d",
           (int)vout.data[j],(int)asc_td[j],(int)vout.data[j+1],(int)asc_td[j+1],
           (int)vout.data[j+2],(int)asc_td[j+2],(int)vout.data[j+3],(int)asc_td[j+3],
           (int)vout.data[j+4],(int)asc_td[j+4],(int)vout.data[j+5],(int)asc_td[j+5]);
    printf("\n");
  }
  if(!rem)
    rem = 6;
  printf("VELRMS ");
  for(i=vout.ns-rem; i<vout.ns; i++) {
    printf("%6d%6d",(int)vout.data[i],(int)asc_td[i]);
  }
  printf("\n");

}

int read_tgs(int verbose, int *totpair)
{

  int i, j;          /* Loop count */
  int cardnum;       /* Record num */
  int seqnum;        /* seqnum */
  int sp1;           /* Shot point */
  int nrec;          /* Number of TV records */
  int nrem;          /* nrec%5 */
  int stat;          /* scanf return */
  int ns;            /* nsamp */
  int nitems;        /* number of fields from scan */

  char seq[7];
  char tgs_vel[4];   /* TGS VEL */
  char tgs_type[4];  /* TGS xxx */
  char tgs_line[12]; /* TGS line name */
  char string[32];   /* Scratch */
  char record[81];   /* 80-char record */

  /* Read TGS VEL record and verify */
  if(fgets(record,81,stdin) != NULL) {
    nitems = sscanf(record,"%3c%3c%12c%6c%6c%6c%6c%8c%10c%2c%10c%8c",
                  &string,&string,&string,&string,&string,&string,
                  seq,&string,&string,&string,&string,&string);
    seq[6] = '\0';
    if(strncmp(seq,"      ",6))
      sscanf(record,"%3s%3s%12s%6d%6d%6d%6d%8f%10f%2d%10s%8d\n",
             &tgs_vel,&tgs_type,velfunc.line,&velfunc.sp,&velfunc.cdp,&ns,
             &seqnum,&velfunc.xcoord,&velfunc.ycoord,&cardnum,&string,&sp1);
    else
      sscanf(record,"%3s%3s%12s%6d%6d%6d%8f%10f%2d%10s%8d\n",
             &tgs_vel,&tgs_type,velfunc.line,&velfunc.sp,&velfunc.cdp,&ns,
             &velfunc.xcoord,&velfunc.ycoord,&cardnum,&string,&sp1);
    vin.ns = ns;
/*
    if(verbose)
      fprintf(stderr,"%s, %s, %s, %d, %d, %d, %8.2f, %10.2f\n",
              tgs_vel,tgs_type,velfunc.line,velfunc.sp,velfunc.cdp,
              vin.ns,velfunc.xcoord,velfunc.ycoord);
*/

    if(strcmp(tgs_vel,"VEL")) {
      fprintf(stderr,"Processed %d V-T pair so far\n", *totpair);
      fprintf(stderr,"%s, %s\n", tgs_vel,tgs_type);
      err("First record of velocity function is not a VEL record\n");
    }

    *totpair += vin.ns;
    nrec = vin.ns / 5;
    nrem = vin.ns % 5;
    if(nrem)
      nrec++;

/*
    if(verbose) {
      if(!nrem)
        fprintf(stderr,"Reading %d TV pair from %d records\n", vin.ns,nrec);
      else
        fprintf(stderr,"Reading %d TV pair from first %d records, %d from last record\n",
                        vin.ns-nrem,nrec-1,nrem);
    }
*/

    for(i=0; i<nrec-1; i++) {
      for(j=0; j<4; j++)
        scanf("%6f%6f", &asc_td[5*i+j],&vin.data[5*i+j]);
      scanf("%6f%6f%2d%10s%8d\n", &asc_td[5*i+4],&vin.data[5*i+4],&cardnum,&string,&sp1);
    }
    if(nrem)
      i = nrem-1;
    else
      i = 4;
    for(j=0; j<i; j++)
      scanf("%6f%6f", &asc_td[5*(nrec-1)+j],&vin.data[5*(nrec-1)+j]);
    scanf("%6f%6f%2d%10s%8d\n", &asc_td[5*(nrec-1)+i],&vin.data[5*(nrec-1)+i],
          &cardnum,&string,&sp1);

    return nitems;

  }
  else {
    vin.ns = 0;
    return EOF;
  }

}

void write_tgs(int verbose)
{
}

int init_fdmodel(char *name, int verbose)
{

  int i, j;
  float x;

  /* Open for R/W */
  if(verbose)
    fprintf(stderr,"Opening %s\n", name);
  omodfp = fopen(name,"w+");
  if(omodfp == NULL)
    return 1;

  /* Write up to point of first velocity trace */
  i = strlen(modname);
  fwrite(&i,sizeof(int),1,omodfp);
  fwrite(modname,sizeof(char),i,omodfp);
  /* nx */
  fwrite(&fdmodel.nx,sizeof(int),1,omodfp);   
  /* nz */
  fdmodel.nz = vin.ns;
  fwrite(&fdmodel.nz,sizeof(int),1,omodfp);
  /* dx */
  fwrite(&fdmodel.dx,sizeof(float),1,omodfp);   
  /* dz */
  fdmodel.dz = vin.dt;
  fwrite(&fdmodel.dz,sizeof(float),1,omodfp);   
  /* cdpmin */
  fwrite(&fdmodel.cdpmin,sizeof(int),1,omodfp);
  /* cdpmax */
  fwrite(&fdmodel.cdpmax,sizeof(int),1,omodfp);
  /* cdpinc */
  fwrite(&fdmodel.cdpinc,sizeof(int),1,omodfp);
  /* cdpdist */
  fwrite(&fdmodel.cdpdist,sizeof(float),1,omodfp);
  /* firstcdpvel */
  fwrite(&fdmodel.firstcdpvel,sizeof(int),1,omodfp);
  /* lastcdpvel */
  fwrite(&fdmodel.lastcdpvel,sizeof(int),1,omodfp);
  /* cdpnum */
  fwrite(&fdmodel.cdpnum,sizeof(int),1,omodfp);

  /* Allocate vels */
  model = calloc(fdmodel.nz,sizeof(float *));
  for(i=0; i< fdmodel.nz; i++)
    model[i] = calloc(fdmodel.nx,sizeof(float));
  
  return 0;

}
int read_fdmodel(char *name, int verbose, int *totpair)
{

  int i, j, k;
  int eof;

  float point[2];

  /* If first time, open model file */
  if(!(*totpair)) {
    if(verbose)
      fprintf(stderr,"Opening %s\n", name);
    imodfp = fopen(name,"r");
    if(imodfp == NULL)
      err("Cannot open %s\n", name);
    /* Read up to first vel trace */
    eof = fread(&i,sizeof(int),1,imodfp);
    modname = calloc(i,sizeof(char));
    eof = fread(modname,sizeof(char),i,imodfp);
    /* nx */
    eof = fread(&fdmodel.nx,sizeof(int),1,imodfp);
    /* nz */
    eof = fread(&fdmodel.nz,sizeof(int),1,imodfp);
    /* dx */
    eof = fread(&fdmodel.dx,sizeof(int),1,imodfp);
    /* dz */
    eof = fread(&fdmodel.dz,sizeof(int),1,imodfp);
    /* cdpmin */
    eof = fread(&fdmodel.cdpmin,sizeof(int),1,imodfp);
    /* cdpmax */
    eof = fread(&fdmodel.cdpmax,sizeof(int),1,imodfp);
    /* cdpinc */
    eof = fread(&fdmodel.cdpinc,sizeof(int),1,imodfp);
    /* cdpdist */
    eof = fread(&fdmodel.cdpdist,sizeof(float),1,imodfp);
    /* firstcdpvel */
    eof = fread(&fdmodel.firstcdpvel,sizeof(int),1,imodfp);
    /* lastcdpvel */
    eof = fread(&fdmodel.lastcdpvel,sizeof(int),1,imodfp);
    /* CDPinc */
    eof = fread(&fdmodel.cdpnum,sizeof(int),1,imodfp);
    /* Fill asc_td */
    for(i=0; i<fdmodel.nz; i++)
      asc_td[i] = i * fdmodel.dz;
    if(verbose) {
      fprintf(stderr,"modname: %s\n", modname);
      fprintf(stderr,"nx: %d, nz: %d\n", fdmodel.nx,fdmodel.nz);
      fprintf(stderr,"dx: %f, dz: %f\n", fdmodel.dx,fdmodel.dz);
      fprintf(stderr,"cdpmin: %d, cdpmax: %d\n", fdmodel.cdpmin,fdmodel.cdpmax);
      fprintf(stderr,"cdpinc: %d, cdpdist: %f\n", fdmodel.cdpinc,fdmodel.cdpdist);
      fprintf(stderr,"firstcdpvel: %d, lastcdpvel: %d\n", fdmodel.firstcdpvel,fdmodel.lastcdpvel);
      fprintf(stderr,"cdpnum: %d\n", fdmodel.cdpnum);
    }
    /* read model */
    model = calloc(fdmodel.nz,sizeof(float *));
    for(i=0; i< fdmodel.nz; i++) {
      model[i] = calloc(fdmodel.nx,sizeof(float));
      eof = fread(model[i],sizeof(float),fdmodel.nx,imodfp);
    }
    if(verbose)
      fprintf(stderr,"Read %d floats for model\n", fdmodel.nx*fdmodel.nz); 
  }

  vin.ns = fdmodel.nz;
  vin.dt = fdmodel.dz;
  for(i=0,j=*totpair; i<fdmodel.nz; i++)
    vin.data[i] = model[i][j];
  (*totpair)++;
  if(*totpair == fdmodel.nx) {
    eof = EOF;
    /* Read ntopo, etc */
    i = fread(&fdmodel.ntopo,sizeof(int),1,imodfp);
    /* Read past Points */
    for(j=0; j<fdmodel.ntopo; j++) {
      i = fread(point,sizeof(float),2,imodfp);
      /*if(verbose)
        fprintf(stderr,"%f %f\n", point[0],point[1]);*/
    }
    i = fread(&fdmodel.numHorizons,sizeof(int),1,imodfp);
    if(verbose)
      fprintf(stderr,"NTOPO: %d, NUMHORIZONS: %d\n", fdmodel.ntopo,fdmodel.numHorizons);
    for(j=0; j<fdmodel.numHorizons; j++) {
      i = fread(&fdmodel.numPts,sizeof(int),1,imodfp);
      if(verbose)
        fprintf(stderr,"NUMPTS: %d\n", fdmodel.numPts);
      for(k=0; k<fdmodel.numPts; k++) {
        i = fread(point,sizeof(float),2,imodfp);
        /*if(verbose)
          fprintf(stderr,"%f %f\n", point[0],point[1]);*/
      }
    }
  }

  return eof;

}

void write_fdmodel(int nt, int verbose)
{

  int i, j;

  for(i=0; i<fdmodel.nz; i++)
    model[i][nt] = vin.data[i];

  /* Check EOF */
  if(nt == fdmodel.nx - 1) {
    for(i=0; i<fdmodel.nz; i++)
      j = fwrite(model[i],sizeof(float),fdmodel.nx,omodfp);
    /* Set ntopo, numHorizons zero */
    i = 0;
    j = fwrite(&i,sizeof(int),1,omodfp);
    j = fwrite(&i,sizeof(int),1,omodfp);
    if(verbose)
      fprintf(stderr,"Wrote %d velocity traces to fdmodel\n", fdmodel.nx);
  }

}

void write_sbase3(int verbose)
{

  int i;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("LINE",velfunc.iline);
    checkval("CDP",velfunc.cdp);
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
  }

  for(i=0; i<vout.ns; i++)
    printf("%10d %10d %20.1f %20.1f %10.1f %10.1f\n", velfunc.iline,velfunc.cdp,
           velfunc.xcoord,velfunc.ycoord,asc_td[i],vin.data[i]);

}
void write_sbase2(int verbose)
{

  int i;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("SP",velfunc.sp);
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
  }

  for(i=0; i<vout.ns; i++)
    printf("%s %10d %20.1f %20.1f %10.1f %10.1f\n", velfunc.line,velfunc.sp,
           velfunc.xcoord,velfunc.ycoord,asc_td[i],vin.data[i]);

}
int check_fmt(char *fmt, int verbose)
{

  int ifmt=0;

  if(!strcmp(fmt,"segy"))
    ifmt = 1;
  else if(!strcmp(fmt,"par"))
    ifmt = 2;
  else if(!strcmp(fmt,"pro"))
    ifmt = 3;
  else if(!strcmp(fmt,"velf") || !strcmp(fmt,"velf1"))
    ifmt = 41;
  else if(!strcmp(fmt,"velf2"))
    ifmt = 42;
  else if(!strcmp(fmt,"velf3"))
    ifmt = 43;
  else if(!strcmp(fmt,"veff"))
    ifmt = 5;
  else if(!strcmp(fmt,"lm"))
    ifmt = 6;
  else if(!strcmp(fmt,"views"))
    ifmt = 7;
  else if(!strcmp(fmt,"tgs"))
    ifmt = 8;
  else if(!strcmp(fmt,"fdmodel"))
    ifmt = 9;
  else if(!strcmp(fmt,"sbase2"))
    ifmt = 10;
  else if(!strcmp(fmt,"sbase3"))
    ifmt = 11;
  else if(!strcmp(fmt,"vel3d"))
    ifmt = 12;
  else if(!strcmp(fmt,"par3d"))
    ifmt = 13;
  else if(!strcmp(fmt,"v2") || !strcmp(fmt,"V2"))
    ifmt = 14;
  else if(!strcmp(fmt,"avf"))
    ifmt = 15;

  return ifmt;

}

void checkval(char *name, int val)
{

  /* Issue warning if value associated with any name is zero */
  if(val == 0)
    fprintf(stderr,"WARNING!! %s=0 detected in output\n", name);

}
int read_v2(int verbose, int *totpair, int *nextloc)
{

  char v2[3];

  int stat;

  static int il, xl;
  static int xc, yc, td, vel;

  for(;;) {
    /* Check for read-ahead */
    if(*nextloc == 1) {
      velfunc.iline = il;
      velfunc.xline = xl;
      asc_td[0] = td;
      vin.data[0] = vel;
      velfunc.xcoord = xc;
      velfunc.ycoord = yc;
      vin.ns= 1;
      *nextloc = 0;
    }
    if((stat = scanf("%s %d %d %d %d %d %d\n",v2,&il,&xl,&td,&vel,&xc,&yc)) == EOF) {
      if(!vin.ns)
        vin.ns = 0;
      break;
    }
    /* First record in file */
    if(vin.ns == 0) {
      velfunc.iline = il;
      velfunc.xline = xl;
      asc_td[vin.ns] = td;
      vin.data[vin.ns] = vel;
      velfunc.xcoord = xc;
      velfunc.ycoord = yc;
      vin.ns++;
    }
    /* More records, same location */
    else if(il == velfunc.iline && xl == velfunc.xline) {
      asc_td[vin.ns] = td;
      vin.data[vin.ns] = vel;
      vin.ns++;
    }
    /* New location */
    else if(il != velfunc.iline || xl != velfunc.xline) {
      *nextloc = 1;
      break;
    }
  }

  (*totpair) += vin.ns;
  return stat;

}

void write_v2(int verbose)
{

  int i;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("ILINE",(int)velfunc.iline);
    checkval("XLINE",(int)velfunc.xline);
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
  }

  for(i=0; i<vout.ns; i++)
    printf("V2%12d%6d%20d%21d%8d%7d\n",velfunc.iline,velfunc.xline,
           (int)asc_td[i],(int)vout.data[i],(int)velfunc.xcoord,(int)velfunc.ycoord);

}

void write_avf(int nfcns, int verbose)
{

  static char *function;
  static char *num;

  int i, j, k;
  static int first=1;

  /* Look for bogus values */
  if(first) {
    first = 0;
    checkval("XCOORD",(int)velfunc.xcoord);
    checkval("YCOORD",(int)velfunc.ycoord);
    function = calloc(16,sizeof(char));
    num = calloc(9,sizeof(char));
    /* write avf heading */
    printf("#\n");
    printf("#FIELDS = Function ID, X, Y, Depth, Vint\n");
    printf("#FUNCTION_TYPE = DVint\n");
    printf("#LINEAR_UNITS = FEET\n");
    printf("#DATUM = 0.000000\n");
    printf("#\n");
  }

  strcpy(function,"");
  sprintf(num,"%d",nfcns+1);
  i = strlen(num);
  j = 15 - i;
  k = j - 8;
  for(i=0; i<k; i++)
    strcat(&function[i]," ");
  strcat(&function[k],"Function");
  strcat(&function[j],num);

  for(i=0; i<vout.ns; i++)
    printf("%15s%13.2f%13.2f%13.4f%13.4f\n",function,velfunc.xcoord,velfunc.ycoord,
           asc_td[i],vout.data[i]);
  printf("#\n");

}
