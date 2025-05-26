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
#include "segy.h"
#include "header.h"
#include "bhpio_api.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"                                                                       ",
"  BHPSYNTHETIC - make time and depth synthetic seismic data from model data",
"                                                                       ",
"  bhpsynthetic builds synthetics from BHPIO cube model datasets.       ",
"  The synthetic data are written in BHPIO cube format.                 ",
"                                                                       ",
"  USAGE: bhpsynthetic wavelet=                                         ",
"                                                                       ",
"  Required Parameters:                                                 ",
"    wavelet=                  Location of Mexican hat wavelet produced ",
"                              by bhpwavelet                            ",
"  Optional Parameters:                                                 ",
"    tmodel=                   Name of BHPIO cube dataset containing    ",
"                              time model. The model must have at least ",
"                              two properties named vp and rho, for     ",
"                              velocity and density needed to calculate ",
"                              reflection coefficients.                 ",
"    dmodel=                   Name of BHPIO cube dataset containing    ",
"                              depth model. It must also contain vp and rho.",
"    tsynth=                   Output time synthetic data, in BHPIO cube format",
"    dsynth=                   Output depth synthetic data, in BHPIO cube format",
"  NOTE: If tmodel is specified, then tsynth must be specified and vice versa.",
"        Likewise for dmodel/dsynth. Either or both types of synthetics ",
"        can be generated in a single execution of bhpsynthetic.        ",
"    mint=0                    Minimum output time, in milliseconds     ",
"    maxt=4000                 Maximum output time, in milliseconds     ",
"    dt=1                      Sample interval, in milliseconds         ",
"    mind=0                    Minimum output depth                     ",
"    maxd=500                  Maximum output depth                     ",
"    dz=10                     Depth interval                           ",
"    freq=40                   Frequency(Hz)                            ",
"    verbose=0                 Debug print                              ",
"                                                                       ",
NULL};

/* prototypes */
void makerc(segy *mtrace, segy *otrace, int nlayers, int ivp, int irho, int nprop, int verbose);

int main(int argc, char **argv)
{

  char *wavelet;        /* wavelet file */
  char *tmodel;         /* time model */
  char *dmodel;         /* depth model */
  char **keys;          /* model data keys */
  char *pathlist;       /* location of models and synthetics */
  char *tsynth;         /* time synthetics dataset */
  char *dsynth;         /* depth synthetics dataset */
  char model[256];      /* tmodel or dmodel, depending on pass */
  char synth[256];      /* tsynth or dsynth, depending on pass */
  char cmdbuf[BUFSIZ];  /* UNIX commands */
  char string1[256];    /* scratch */
  char *rcfile="/hou/data/D_169_002/ahmilb/test_data/seismic/rcmodel.su";

  float mint, maxt;     /* start, end time */
  float mind, maxd;     /* start, end depth */
  float mintd, maxtd;   /* mint/maxt or mind/maxd, depending on pass */
  float dt;             /* time interval */
  float dz;             /* depth interval */
  float delt;           /* dt or dz, depending on pass */
  float null;           /* null model data default = -999.25 */
  float *work;          /* convolved time wavelet and rc spike */
  float freq;           /* output frequency */
  float wdt, wlen;      /* wavelet dt, len (mills) */
  float mtime;          /* time/depth of each rc spike */
  float wstart, wend;   /* assigned start, end times for each rc spike */
  float f1, f2;         /* linear interp fractions */
  float otime;          /* output time counter */

  int w1, w2;           /* wavelet indices */
  int i, j, k, m, n;    /* loop counters */
  int start, end;       /* starting and ending output samples for each convolution */
  int verbose;
  int nst;              /* samples in output time trace */
  int nsd;              /* samples in output depth trace */
  int ns;               /* samples in output, depending on pass */
  int iout;             /* sample index of out */
  int debug_cdp=0;      /* for debug output */
  int *key_index;       /* key indices returned by open_bhpio_dataset */
  int nkeys;            /* num keys in horizons, output */
  int *min, *max, *incr; /* key limits */
  int *nbins;           /* num bins each key */
  int *keyvals;         /* key values */
  int *next;            /* next keys to get */
  int ntraces;          /* num traces in hfile */
  int trcount;          /* trace count returned from read */
  int pass;             /* two passes: 0=time, 1=depth */
  int ivp, irho;        /* indices of vp and rho in properties */
  int time=0, depth=0;  /* =1 --> generate synthetics */
  int nlayers;          /* number of model layers */
  int rc;               /* undocumented parameter 1=write refl coef data */

  FILE *fp;             /* wavelet file pointer, also time or depth synth, depending on pass */
  FILE *rcfp;           /* rc trace file */

  segy wtrace;           /* wavelet */
  segy mtrace;           /* model trace */
  segy otrace;           /* output trace */
  segy dtrace;           /* debug output - trace for each convolution */
  segy rctrace;          /* refl coef debug */

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);
  
  /* debug option */    
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* refl coefs */    
  if(!getparint("rc",&rc))
    rc = 0;

  /* wavelet file */
  if(!getparstring("wavelet",&wavelet))
    err("wavelet is required\n");

  /* tmodel and tsynth */
  if(getparstring("tmodel",&tmodel) && !getparstring("tsynth",&tsynth))
    err("If tmodel is specified, then tsynth must also be specified\n");
  if(getparstring("tsynth",&tsynth) && !getparstring("tmodel",&tmodel))
    err("If tsynth is specified, then tmodel must also be specified\n");
  if(!getparstring("tmodel",&tmodel) && !getparstring("tsynth",&tsynth)) {
    if(verbose)
      fprintf(stderr,"Not generating time synthetics\n");
  }
  else {
    /* min time millisec */
    if(!getparfloat("mint",&mint))
      mint = 0;
    /* max time millisec */
    if(!getparfloat("maxt",&maxt))
      maxt = 4000;
    /* sampling interval millisec */
    if(!getparfloat("dt",&dt))
      dt = 1;
    /* ns */
    nst = (maxt - mint + dt) / dt;
    time = 1;
    if(verbose)
      fprintf(stderr,"Time Model: %s, Time Synthetics: %s\n",tmodel,tsynth);
      fprintf(stderr,"  Start = %6.2f, End = %6.2f, Interval = %6.2f, Length = %4d\n",mint,maxt,dt,nst);
  }

  /* dmodel and dsynth */
  if(getparstring("dmodel",&dmodel) && !getparstring("dsynth",&dsynth))
    err("If dmodel is specified, then dsynth must also be specified\n");
  if(getparstring("dsynth",&dsynth) && !getparstring("dmodel",&dmodel))
    err("If dsynth is specified, then dmodel must also be specified\n");
  if(!getparstring("dmodel",&dmodel) && !getparstring("dsynth",&dsynth)) {
    if(verbose)
      fprintf(stderr,"Not generating depth synthetics\n");
  }
  else {
    /* min depth */
    if(!getparfloat("mind",&mind))
      mind = 0;
    /* max depth */
    if(!getparfloat("maxd",&maxd))
      maxd = 500;
    /* depth interval */
    if(!getparfloat("dz",&dz))
      dz = 1;
    /* ns */
    nsd = (maxd - mind + dz) / dz;
    depth = 1;
    if(verbose)
      fprintf(stderr,"Depth Model: %s, Depth Synthetics: %s\n",dmodel,dsynth);
      fprintf(stderr,"  Start = %6.2f, End = %6.2f, Interval = %6.2f, Length = %4d\n",mind,maxd,dz,nsd);
  }

  /* pathlist, if empty use tmodel.dat, else dmodel.dat */
  if(!getparstring("pathlist",&pathlist)) {
    if(time == 1) {
      pathlist = calloc((int)strlen(tmodel)+5,sizeof(char));
      strcpy(pathlist,tmodel);
      strcat(pathlist,".dat");
    }
    else if(depth == 1) {
      pathlist = calloc((int)strlen(dmodel)+5,sizeof(char));
      strcpy(pathlist,dmodel);
      strcat(pathlist,".dat");
    }
  }

  /* frequency */
  if(!getparfloat("freq",&freq))
    freq = 40;
  freq *= 4.;

  /* wavelet dt, len */
  wdt = 1. / freq * 16. / 1024. * 1000;
  wlen = 1024 * wdt;

  /* get wavelet */
  fp = fopen(wavelet,"r");
  if(fp == NULL)
    err("Cannot open wavelet file: %s\n",wavelet);
  if(!fgettr(fp,&wtrace))
    err("Cannot read wavelet trace\n");
  fclose(fp);

  if(verbose) {
    fprintf(stderr,"Getting wavelet from %s, %d samples\n",wavelet,wtrace.ns);
    fprintf(stderr,"Wavelet: Frequency = %f, Interval = %f, Length = %f\n",freq,wdt,wlen);
    fprintf(stderr,"pathlist=%s\n",pathlist);
  }

  /* alloc work for convolved output */
  work = calloc(wtrace.ns,sizeof(float));

  /* key indices, limits, etc */
  keyvals = calloc(nkeys,sizeof(int));
  min = calloc(nkeys,sizeof(int));
  max = calloc(nkeys,sizeof(int));
  incr = calloc(nkeys,sizeof(int));
  nbins = calloc(nkeys,sizeof(int));
  next = calloc(nkeys,sizeof(int));
  keys = calloc(nkeys,sizeof(char *));
  for(i=0; i<nkeys; i++)
    keys[i] = calloc(8,sizeof(char));

  /* first pass is time, second pass is depth */
  for(pass=0; pass<2; pass++) {
    /* time */
    if(pass == 0 && time == 1) {
      strcpy(model,tmodel);
      strcpy(synth,tsynth);
      mintd = mint;
      maxtd = maxt;
      delt = dt;
      ns = nst;
      /* rcmodel - debug */
      if(rc == 1) {
        rcfp = fopen(rcfile,"w");
        if(rcfp == NULL)
          err("Cannot open %s for writing\n",rcfile);
      }
    }
    /* depth */
    else if(pass == 1 && depth == 1) {
      strcpy(model,dmodel);
      strcpy(synth,dsynth);
      mintd = mind;
      maxtd = maxd;
      delt = dz;
      ns = nsd;
    }
    else
      continue;

    /* open model */
    key_index = open_bhpio_dataset(model,pathlist,&nkeys,verbose);

    /* verify input is cross-section */
    if(file_hdr.data_order > 1)
      err("Input data must be cross-section\n");

    /* make sure vp and rho are present */
    if(file_hdr.nprop < 2)
      err("At least two properties must be in %s\n",model);
    ivp = irho = -1;
    for(i=0; i<file_hdr.nprop; i++) {
      if(!strcmp(properties[i],"vp")) {
        if(verbose)
          fprintf(stderr,"vp is property %d\n",i+1);
        ivp = i;
      }
      else if(!strcmp(properties[i],"rho")) {
        if(verbose)
          fprintf(stderr,"rho is property %d\n",i+1);
        irho = i;
      }
    }
    if(ivp == -1 || irho == -1)
      err("%s must have properties vp and rho\n",model);

    /* number of layers, starting sample for vp, rho */
    nlayers = (file_hdr.nsamp - 1) / (file_hdr.nprop + 1);
    ivp *= nlayers;
    irho *= nlayers;
    if(verbose)
      fprintf(stderr,"%d layers in model, vel starts at %d, rho starts at %d\n",nlayers,ivp,irho);

    /* header limits */
    if((i = get_bhpio_header_limits(min,max,incr,verbose)) != 0)
      err("Error getting limits\n");
    /* key names */
    for(i=0; i<nkeys; i++)
      keys[i] = getkey(key_index[i]);

    /* fill in ntraces */
    nbins[0] = (max[0] - min[0] + incr[0]) / incr[0];
    ntraces = nbins[0];
    for(i=1; i<nkeys; i++) {
      nbins[i] = (max[i] - min[i] + incr[i]) / incr[i];
      ntraces *= nbins[i];
    }

    if(verbose) {
      fprintf(stderr,"%s Header Limits:\n",model);
      for(i=0; i<nkeys; i++)
        fprintf(stderr,"KEY: %s, MIN: %d, MAX: %d, INCR: %d\n",keys[i],min[i],max[i],incr[i]);
      fprintf(stderr,"Reading/writing %d total traces\n",ntraces);
    }

    /* initialize key values */
    for(i=0; i<nkeys; i++)
      next[i] = min[i];

    /* start bhpwritecube */
    sprintf(cmdbuf,"bhpwritecube filename=%s pathlist=%s init=yes verbose=%d ",
            synth,pathlist,verbose);
    if(pass == 1)
      strcat(cmdbuf,"units=2 ");
    for(i=0; i<nkeys; i++) {
      sprintf(string1,"key%1d=%s,%d,%d,%d ",i+1,keys[i],min[i],incr[i],nbins[i]);
      strcat(cmdbuf,string1);
    }
    if(verbose)
      fprintf(stderr,"Executing %s\n",cmdbuf);
    fp = popen(cmdbuf,"w");

    /* loop over input traces */
    for(n=0; n<ntraces; n++) {
      for(i=0; i<nkeys; i++)
        keyvals[i] = next[i];
      trcount = read_bhpio_trace(keyvals,&mtrace,verbose);
      if(trcount != 1) {
        fprintf(stderr,"Failed to get trace from model data ");
        for(i=0; i<nkeys; i++)
          fprintf(stderr," %d ",keyvals[i]);
        fprintf(stderr,"\n");
      }
      if(trcount == 1) {
        makerc(&mtrace,&otrace,nlayers,ivp,irho,file_hdr.nprop,verbose);
        /* clear otrace */
        for(i=0; i<ns; i++)
          otrace.data[i] = 0.0;
        memcpy((void *)&otrace,(const void *)&mtrace,HDRBYTES);
        otrace.ns = ns;
        otrace.dt = 1000 * delt;
        otrace.delrt = mintd;
        /* make rctrace */
        if(pass == 0 && time == 1 && rc == 1) {
          for(i=0; i<2*nlayers-1; i++)
            rctrace.data[i] = mtrace.data[i];
          memcpy((void *)&rctrace,(const void *)&mtrace,HDRBYTES);
          rctrace.ns = 2 * nlayers - 1;
          rctrace.dt = 1000 * delt;
          rctrace.delrt = mintd;
          fputtr(rcfp,&rctrace);
        }
      }
      else {
        for(k=nkeys-1; k>=0; k--) {
          next[k] += incr[k];
          if(next[k] > max[k])
            next[k] = min[k];
          else
            break;
        }
        continue;
      }
      /* loop over depths or times */
      for(i=0,j=mtrace.ns/2; i<mtrace.ns/2; i++,j++) {
        /* clear debug trace */
        memcpy((void *)&dtrace,(const void *)&mtrace,HDRBYTES);
        dtrace.ns = otrace.ns;
        dtrace.dt = otrace.dt;
        dtrace.delrt = otrace.delrt;
        dtrace.cdp = mtrace.cdp + i + 1;
        for(k=0; k<ns; k++)
          dtrace.data[k] = 0.0;
        /* convolve each non-zero or non-null in model with wavelet */
        if(mtrace.data[i] != 0 && mtrace.data[i] != null) {
          /* input model times are in mills, depths are in meters */
          mtime = mtrace.data[j];
          conv(1,0,&mtrace.data[i],wtrace.ns,0,wtrace.data,wtrace.ns,0,work);
          /* post output center point at model time/depth */
          /* assign wavelet start, end times based on mtime, wlen, wdt */
          wstart = mtime - (wlen / 2);
          wend = mtime + (wlen / 2);
          if(mtrace.cdp == debug_cdp)
            fprintf(stderr,"cdp=%d,mtime=%f,wstart=%f,wend=%f\n",
                    mtrace.cdp,mtime,wstart,wend,mtrace.data[i]);
          /* loop over output times */
          for(otime=mintd; otime<=maxtd; otime++) {
            if(otime >= wstart && otime <= wend) {
              /* wavelet sample indices */
              w1 = (otime - wstart) / wdt;
              w2 = w1 + 1;
              /* interp fractions */
              f1 = 1.0 - (otime - (wstart + w1 * wdt)) / wdt;
              f2 = 1.0 - ((wstart + w2 * wdt) - otime) / wdt;
              iout = (otime - mintd) / delt;
              if(mtrace.cdp == debug_cdp)
                fprintf(stderr,"outtime=%f,outindex=%d,wtime1=%f,w1=%d,wtime2=%f,w2=%d,f1=%f,f2=%f\n",
                        otime,iout,wstart+w1*wdt,w1,wstart+w2*wdt,w2,f1,f2);
              otrace.data[iout] += (work[w1] * f1 + work[w2] * f2);
              dtrace.data[iout] = (work[w1] * f1 + work[w2] * f2);
            }
          }
          if(mtrace.cdp == debug_cdp)
            fputtr(fp,&dtrace);
        }
      }
      if(debug_cdp == 0)
        fputtr(fp,&otrace);
      for(k=nkeys-1; k>=0; k--) {
        next[k] += incr[k];
        if(next[k] > max[k])
          next[k] = min[k];
        else
          break;
      }
    }
    pclose(fp);
    if(rc == 1)
      fclose(rcfp);
  }

  return EXIT_SUCCESS;

}
void makerc(segy *mtrace, segy *otrace, int nlayers, int ivp, int irho, int nprop, int verbose)
{


  float v1, v2, d1, d2;

  int i, j, k;

  /* copy mtrace to otrace */
  for(i=0; i<mtrace->ns; i++)
    otrace->data[i] = mtrace->data[i];
 
  /* loop over input, calc refl coef */
  for(i=ivp,j=irho,k=0; i<ivp+nlayers-1; i++,j++,k++) {
    v1 = otrace->data[i];
    v2 = otrace->data[i+1];
    d1 = otrace->data[j];
    d2 = otrace->data[j+1];
    mtrace->data[k] = (v2 * d2 - v1 * d1) / (v1 * d1 + v2 * d2);
  }

  /* copy depths/times */
  for(i=nlayers-1,j=nlayers*nprop+1; i<2*nlayers-1; i++,j++)
    mtrace->data[i] = otrace->data[j];

  /* new ns */
  mtrace->ns = 2 * nlayers - 1;

}
