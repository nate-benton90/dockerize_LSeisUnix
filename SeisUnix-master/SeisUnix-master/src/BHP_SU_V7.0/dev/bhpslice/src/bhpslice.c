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
#include "su.h"
#include "segy.h"
#include "header.h"

/* globals */
#define IFAC 8   /* over-sample factor */
#define NULLVAL -999.25

/* prototypes */

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                       ",
" bhpslice < stdin > stdout [optional parameters]                       ",
"                                                                       ",
"  Required Parameters: none                                            ",
"                                                                       ",
"  Optional Parameters:                                                 ",
"                                                                       ",
"    hlist=f1            Trace headers in input data containing events ",
"                        to be flattened to times in output             ",
"    times=500           Constant output times to which events are flattened",
"    dt=4                Output sample interval, milliseconds           ",
"    tmin=0              Minumum output time, in milliseconds           ",
"    tmax=1000           Maximum output time, in milliseconds           ",
"                                                                       ",
NULL};

int main(int argc, char **argv)
{

  cwp_String hlist[SU_NKEYS]; /* list of input trace headers containing iso-time events */

  float *times;     /* flattened output times */
  float tmin, tmax; /* min/max output times */
  float *rtimes;    /* resampled input times */
  float *rdat;      /* re-sampled input data */
  float rdt;        /* re-sampled data interval */
  float *step;      /* percentage step size for each output window */
  float odt;        /* output sample interval */
  float idt;        /* input sample interval */
  float *events;    /* event times for current input trace */
  float *mtimes;    /* times needed to map input to output */
  float avg;        /* average of input values */
  float time;       /* next output time */
  float time1, time2; /* nearest times to neede time */
  float frac;       /* interp fraction */
  float d;          /* interpolated data point */
  float ratio;      /* ratio of output win len to input win len */

  int i, j, k;      /* loop count */
  int k1, k2;       /* indices of samples nearest needed time in resampled input */
  int verbose;      /* debug print */
  int ntimes;       /* number of flattened output times */
  int nhlist;       /* number of entries in hlist */
  int rns;          /* number of samples in re-sampled input */
  int ifac;         /* override parameter for IFAC */
  int nwin;         /* number of windows */
  int *owlen;       /* number of samples in each output window */
  int **osamp;      /* starting & ending zero-based sample indices for each output window */
  int *iwlen;       /* number of samples in each input window */
  int **isamp;      /* starting & ending zero-based sample indices for each input window */
  int *index;       /* trace header index for each input event */
  int null;         /* flag to signal null event */

  segy intr;        /* input trace */
  segy outtr;       /* output trace */

  Value hval;       /* header values */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* starting output time */
  if(!getparfloat("tmin",&tmin))
    tmin = 0;

  /* ending output time */
  if(!getparfloat("tmax",&tmax))
    tmax = 1000;

  /* output sample interval */
  if(!getparfloat("dt",&odt))
    odt = 4;

  /* flattened output times */
  ntimes = countparval("times");
  if(ntimes > 0) {
    times = calloc(ntimes,sizeof(float));
    getparfloat("times",times);
  }
  else {
    ntimes = 1;
    times = calloc(1,sizeof(float));
    times[0] = 500;
  }

  if(!getparint("ifac",&ifac))
    ifac = IFAC;

  /* input event trace headers */
  nhlist = countparval("hlist");
  if(nhlist > 0)
    getparstringarray("hlist",hlist);
  else {
    nhlist = 1;
    hlist[0] = "f1";
  }

  /* first trace */
  if(!gettr(&intr))
    err("Cannot get first trace\n");
  idt = intr.dt * 0.001;
  outtr.ns = (tmax - tmin + odt) / odt;
  outtr.delrt = tmin;
  outtr.dt = odt * 1000.;

  if(verbose) {
    fprintf(stderr,"Event Trace Headers: ");
    for(i=0; i<nhlist; i++)
      fprintf(stderr," %s ",hlist[i]);
    fprintf(stderr,"\n");
    fprintf(stderr,"OUTPUT: Start Time: %.4f, End Time %.4f, Interval: %.4f  (Milliseconds)\n",
            tmin,tmax,odt);
    fprintf(stderr,"INPUT: Start Time: %.3f, End Time: %.3f, Interval: %.4f  (Milliseconds)\n",
            (float)intr.delrt,intr.delrt+intr.ns*idt-idt,idt);
    fprintf(stderr,"OUTPUT: Flattened Times in Milliseconds: ");
    for(i=0; i<ntimes; i++)
      fprintf(stderr," %.4f ",times[i]);
    fprintf(stderr,"\n");
  }

  /* number of events in input should be = ntimes */
  if(nhlist != ntimes)
    err("Number of events in input, %d, must be equal number of times in output, %d\n",nhlist,ntimes);

  /* verify times are in order, etc */
  for(i=1; i<ntimes; i++) {
    if(times[i] <= times[i-1])
      err("Times should be increasing: times(%d) is less than or equal times(%d), %f <= %f\n",
          i+1,i,times[i],times[i-1]);
  }
  for(i=0; i<ntimes; i++) {
    if(times[i] <= tmin)
      err("%f is less than minimum: %f\n",times[i],tmin);
    if(times[i] >= tmax)
      err("%f is greater than maximum: %f\n",times[i],tmax);
  }
  /* input event times */
  events = calloc(nhlist,sizeof(float));

  /* output time windows */
  nwin = ntimes + 1;
  owlen = calloc(nwin,sizeof(int));
  step = calloc(nwin,sizeof(float));
  osamp = calloc(nwin,sizeof(int *));
  for(i=0; i<nwin; i++)
    osamp[i] = calloc(2,sizeof(int));
  /* start of first output window is greater of tmin or input start time */
  if(tmin >= intr.delrt)
    osamp[0][0] = 0;
  else
    osamp[0][0] = (intr.delrt - tmin) / odt;
  osamp[0][1] = (times[0] - tmin - odt) / odt;
  owlen[0] = osamp[0][1] - osamp[0][0] + 1;
  osamp[nwin-1][0] = (times[ntimes-1] - tmin) / odt;
  /* end of last output window is lesser of tmax or end of input */
  if(tmax <= intr.delrt + (intr.ns - 1) * idt)
    osamp[nwin-1][1] = outtr.ns - 1;
  else
    osamp[nwin-1][1] = (intr.delrt + (intr.ns - 1) * idt - tmin) / odt;
  owlen[nwin-1] = osamp[nwin-1][1] - osamp[nwin-1][0] + 1;
  for(i=1; i<nwin-1; i++) {
    osamp[i][0] = (times[i-1] - tmin) / odt;
    osamp[i][1] = (times[i] - tmin - odt) / odt;
    owlen[i] = osamp[i][1] - osamp[i][0] + 1;
  }
  for(i=0; i<nwin; i++)
    step[i] = 1. / (owlen[i] - 1);

  /* input windows */
  iwlen = calloc(nwin,sizeof(int));
  isamp = calloc(nwin,sizeof(int *));
  index = calloc(nwin,sizeof(int));
  for(i=0; i<nwin; i++)
    isamp[i] = calloc(2,sizeof(int));
  for(i=0; i<ntimes; i++)
    index[i] = getindex(hlist[i]);

  /* alloc interp space */
  rns = intr.ns * ifac;
  rdt = 0.001 * idt / ifac;
  rtimes = calloc(rns,sizeof(float));
  mtimes = calloc(rns,sizeof(float));
  rdat = calloc(rns,sizeof(float));
  rtimes[0] = intr.delrt * 0.001;
  for(i=1; i<rns; i++)
    rtimes[i] = rtimes[i-1] + rdt;
  if(verbose) {
    fprintf(stderr,"rdt=%f, rns=%d\n",rdt,rns);
    /*fprintf(stderr,"rtimes\n");
    for(i=0; i<rns; i++) {
      fprintf(stderr," %.6f ",rtimes[i]);
      if((i+1)%8 == 0)
        fprintf(stderr,"\n");
    }
    if(rns%8 != 0)
      fprintf(stderr,"\n");*/
  }

  /* loop over input */
  do {
    /* clear output */
    for(i=0; i<outtr.ns; i++)
      outtr.data[i] = 0;
    if(verbose) {
      /*fprintf(stderr,"Input Trace: ");
      for(i=0; i<intr.ns; i++)
        fprintf(stderr," %.2f ",intr.data[i]);
      fprintf(stderr,"\n");*/
    }
    ints8r(intr.ns,idt*0.001,(float)intr.delrt*0.001,intr.data,0.0,0.0,rns,rtimes,rdat);
    /*fprintf(stderr,"Resampled Input Trace\n");
    for(i=0; i<rns; i++) {
      fprintf(stderr," %.4f ",rdat[i]);
      if((i+1)%8 == 0)
        fprintf(stderr,"\n");
    }*/
    if(rns%8 != 0)
      fprintf(stderr,"\n");
    /* events */
    for(i=0; i<nhlist; i++) {
      gethval(&intr,index[i],&hval);
      events[i] = hval.f;
    }
    if(verbose) {
      fprintf(stderr,"ep=%d, cdp=%d, Event Times: ",intr.ep,intr.cdp);
      for(i=0; i<ntimes; i++)
        fprintf(stderr," %.3f ",events[i]);
      fprintf(stderr,"\n");
    }
    null = 0;
    /* check for nulls and events ouside output limits , output zeros */
    for(i=0; i<nhlist; i++) {
      if(events[i] == NULLVAL) {
        null = 1;
        if(verbose)
          fprintf(stderr,"%s has null event, write zeros to output\n",hlist[i]);
      }
    }
    /* if events are not increasing, write zeros and skip trace */
    if(null == 0) {
      for(i=1; i<nhlist; i++) {
        if(events[i] < events[i-1]) {
          fprintf(stderr,"ep=%d, cdp=%d, Decreasing Times: %.3f, %.3f\n",intr.ep,intr.cdp,events[i-1],events[i]);
          fprintf(stderr,"Write zeros to output\n");
          null = 1;
        }
      }
    }
    /* process if no null events */
    if(null == 0) {
      /* make sure events are inside trace and increasing in time */
      for(i=0; i<nhlist; i++) {
        if(events[i] < intr.delrt)
          err("%f is less than trace start time of %f\n",events[i],(float)intr.delrt);
        if(events[i] > intr.delrt+(intr.ns-1)*idt)
          err("%f is greater than trace end time of %f\n",events[i],(float)intr.delrt+intr.ns*idt-idt);
      }
      /* interior windows */
      for(i=1; i<nwin-1; i++) {
        isamp[i][0] = ((events[i-1] - intr.delrt) + idt) / idt;
        isamp[i][1] = (events[i] - intr.delrt) / idt;
        iwlen[i] = isamp[i][1] - isamp[i][0] + 1;
      }
      /* special case -- only one event */
      if(nwin == 2) {
        ratio = 1;
        iwlen[0] = owlen[0];
        iwlen[1] = owlen[1];
        isamp[0][1] = (events[0] - intr.delrt) / idt;
        isamp[0][0] = isamp[0][1] - iwlen[0] + 1;
        isamp[1][0] = isamp[0][1] + 1;
        isamp[1][1] = isamp[1][0] + iwlen[1] - 1;
      } 
      else   {
        /* use same owlen/iwlen ratio for first window as for second, and for last window as for next-to-last */
        ratio = (float)owlen[1] / (float)iwlen[1];
        iwlen[0] = owlen[0] / ratio;
        isamp[0][1] = isamp[1][0] - 1;
        isamp[0][0] = isamp[0][1] - iwlen[0] + 1;
        ratio = (float)owlen[nwin-2] / (float)iwlen[nwin-2];
        iwlen[nwin-1] = owlen[nwin-1] / ratio;
        isamp[nwin-1][0] = isamp[nwin-2][1] + 1;
        isamp[nwin-1][1] = isamp[nwin-1][0] + iwlen[nwin-1] - 1;
      }
      
      if(verbose) {
        fprintf(stderr,"Input Windows:\n");
        for(i=0; i<nwin; i++)
          fprintf(stderr,"Samples: %d --> %d, Len=%d\n",isamp[i][0],isamp[i][1],iwlen[i]);
        fprintf(stderr,"Output Windows:\n");
        fprintf(stderr,"Times: %.2f --> %.2f, Samples: %d --> %d, LEN=%d, Fractional STEP=%.4f\n",
                tmin,times[0]-odt,osamp[0][0],osamp[0][1],owlen[0],step[0]);
        for(i=1; i<nwin-1; i++)
          fprintf(stderr,"Times: %.2f --> %.2f, Samples: %d --> %d, LEN=%d, Fractional STEP=%.4f\n",
                  times[i-1],times[i]-odt,osamp[i][0],osamp[i][1],owlen[i],step[i]);
        fprintf(stderr,"Times: %.2f --> %.2f, Samples: %d --> %d, LEN=%d, Fractional STEP=%.4f\n",
                times[ntimes-1],tmax,osamp[nwin-1][0],osamp[nwin-1][1],owlen[nwin-1],step[nwin-1]);
      }
      /* stretch or squeeze each input window --> output window */
      for(i=0; i<nwin; i++) {
        /* map horizon */
        time = (intr.delrt + (isamp[i][0] * idt));
        time *= 0.001;
        k = time / rdt;
        k1 = time / rdt;
        k2 = (time + rdt) / rdt;
        time1 = k1 * rdt;
        time2 = k2 * rdt;
        if(time < 0 || time > (intr.delrt + (intr.ns - 1) * idt * 0.001)) {
          outtr.data[osamp[i][0]] = 0;
          if(verbose && intr.ep == 7423 && intr.cdp == 21112)
            fprintf(stderr,"Off end of trace, fill with zero\n");
          k = k1 = k2 = 0;
          d = 0;
        }
        else if(k1 != k2) {
          frac = (time - time1) / (time2 - time1);
          d = rdat[k1] + frac * (rdat[k2] - rdat[k1]);
        }
        else
          d = rdat[k1];
        outtr.data[osamp[i][0]] = d;
        if(verbose && intr.ep == 7423 && intr.cdp == 21112) {
          fprintf(stderr,"map horizon: k=%d,k1=%d,k2=%d,itime=%f,time1=%f,time2=%f\n",k,k1,k2,time,time1,time2);
          fprintf(stderr,"otime=%f,frac=%f,rdatk1=%f,rdatk2=%f,rdat=%f\n",tmin+osamp[i][0]*odt,frac,rdat[k1],rdat[k2],d);
        }
        /* map interior points */
        if(iwlen[i] > 1) {
          for(j=1; j<owlen[i]; j++) {
            time = (intr.delrt + (isamp[i][0] * idt)) + ((j * step[i] * (iwlen[i] - 1)) * idt);
            time *= 0.001;
            k = time / rdt;
            k1 = time / rdt;
            k2 = (time + rdt) / rdt;
            time1 = k1 * rdt;
            time2 = k2 * rdt;
            if(time < 0 || time > (intr.delrt + (intr.ns - 1) * idt * 0.001)) {
              outtr.data[osamp[i][0]+j] = 0;
              if(verbose && intr.ep == 7423 && intr.cdp == 21112)
                fprintf(stderr,"Off end of trace, fill with zero\n");
              continue;
            }
            else if(k1 != k2) {
              frac = (time - time1) / (time2 - time1);
              d = rdat[k1] + frac * (rdat[k2] - rdat[k1]);
            }
            else
              d = rdat[k1];
            if(verbose && intr.ep == 7423 && intr.cdp == 21112) {
              fprintf(stderr,"step=%f,j=%d,k=%d,k1=%d,k2=%d,itime=%f,time1=%f,time2=%f\n",step[i],j,k,k1,k2,time,time1,time2);
              fprintf(stderr,"otime=%f,frac=%f,rdatk1=%f,rdatk2=%f,rdat=%f\n",tmin+(osamp[i][0]+j)*odt,frac,rdat[k1],rdat[k2],d);
            }
            outtr.data[osamp[i][0]+j] = d;
          }
          if(verbose) {
            fprintf(stderr,"inlen=%d,idt=%f\n",iwlen[i],idt*0.001);
            /*fprintf(stderr,"input: ");
            for(j=0; j<iwlen[i]; j++)
              fprintf(stderr," %.2f ",intr.data[isamp[i][0]+j]);
            fprintf(stderr,"\n");
            fprintf(stderr,"output: ");
            for(j=0; j<owlen[i]; j++)
              fprintf(stderr," %.2f ",outtr.data[osamp[i][0]+j]);
            fprintf(stderr,"\n");*/
          }
        }
      }
    }
    memcpy((void *)&outtr,(const void *)&intr,HDRBYTES);
    outtr.ns = (tmax - tmin + odt) / odt;
    outtr.delrt = tmin;
    outtr.dt = odt * 1000.;
    puttr(&outtr);
  } while(gettr(&intr));

  return EXIT_SUCCESS;

}
