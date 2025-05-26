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

/*********************** self documentation ******************************/
char *sdoc[] = {
"  BHPSYNTH builds time synthetics from a reflection coefficient model dataset",
"  bhpsynth < stdin > stdout                          ",
"  Required Parameters: none                          ",
"  Optional Parameters:                               ",
"    verbose=0                 Debug print            ",
"    wavelet=mexh.su           Mexican hat wavelet file ",
"    min=0                     Minimum output time(ms)    ",
"    max=4000                  Maximum output time(ms)  ",
"    dt=1                      Sample interval(ms) ",
"    freq=40                   Frequency(Hz)    ",
"    null=-999.25              Nulls in model file are skipped ",
"                                                                       ",
NULL};

int main(int argc, char **argv)
{

  char *wavelet;        /* wavelet file */
  char *depth;          /* yes=depth */

  float min, max;       /* start, end time */
  float dt;             /* time interval */
  float null;           /* null model data default = -999.25 */
  float *work;          /* convolved time wavelet and rc spike */
  float freq;           /* output frequency */
  float vel;            /* velocity */
  float wdt, wlen;      /* wavelet dt, len (mills) */
  float mtime;          /* time/depth of each rc spike */
  float wstart, wend;   /* assigned start, end times for each rc spike */
  float f1, f2;         /* linear interp fractions */
  float otime;          /* output time counter */

  int w1, w2;           /* wavelet indices */
  int i, j, k, m;
  int start, end;       /* starting and ending output samples for each convolution */
  int verbose;
  int ns;               /* samples in output time trace */
  int iout;             /* sample index of out */
  int debug_cdp=0;      /* for debug output */

  FILE *fp;             /* wavelet file pointer */

  segy wtrace;           /* wavelet */
  segy mtrace;           /* model trace */
  segy otrace;           /* output time trace */
  segy dtrace;           /* debug output - trace for each convolution */

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);
  
  /* debug option */    
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* depth? */
  if(!getparstring("depth",&depth))
    depth = "no";

  /* wavelet file */
  if(!getparstring("wavelet",&wavelet))
    wavelet = "mexh.su";

  /* min time millisec */
  if(!getparfloat("min",&min))
    min = 0;

  /* max time millisec */
  if(!getparfloat("max",&max))
    max = 4000;

  /* sampling interval millisec */
  if(!getparfloat("dt",&dt))
    dt = 1;

  /* frequency */
  if(!getparfloat("freq",&freq))
    freq = 40;
  freq *= 4.;

  /* velocity */
/*
  if(!getparfloat("vel",&vel))
    vel = 9000;
*/

  /* null value */
  if(!getparfloat("null",&null))
    null = -999.25;

  /* ns */
  ns = (max - min + dt) / dt;

  /* wavelet dt, len */
  wdt = 1. / freq * 16. / 1024. * 1000;
  wlen = 1024 * wdt;

  /* depth trace */
/*
  mind = (vel * min) / 2000;
  maxd = (vel * max) / 2000;
  maxd = ((maxd + dz - 1) / dz) * dz;
  nd = (maxd - mind + dz) / dz;
*/

  /* get wavelet */
  fp = fopen(wavelet,"r");
  if(fp == NULL)
    err("Cannot open wavelet file: %s\n",wavelet);
  if(!fgettr(fp,&wtrace))
    err("Cannot read wavelet trace\n");
  fclose(fp);

  if(verbose) {
    fprintf(stderr,"Getting wavelet from %s, %d samples\n",wavelet,wtrace.ns);
    fprintf(stderr,"Synthetics: Start = %f, End = %f\n",min,max);
    fprintf(stderr,"            Number of Samples = %d, Interval = %f\n",ns,dt);
    fprintf(stderr,"Wavelet: Frequency = %f, Interval = %f, Length = %f\n",freq,wdt,wlen);
    if(!strcmp(depth,"yes"))
      fprintf(stderr,"DEPTH\n");
  }

  /* alloc work for convolved output */
  work = calloc(wtrace.ns,sizeof(float));

  /* first model trace */
  if(!gettr(&mtrace))
    err("Cannot get first model trace\n");

  do {
    /* clear otrace */
    for(k=0; k<ns; k++)
      otrace.data[k] = 0.0;
    memcpy((void *)&otrace,(const void *)&mtrace,HDRBYTES);
    otrace.ns = ns;
    otrace.dt = 1000 * dt;
    otrace.delrt = min;
    /* loop over depths */
    for(i=0,j=mtrace.ns/2; i<mtrace.ns/2; i++,j++) {
      /* clear debug trace */
      memcpy((void *)&dtrace,(const void *)&mtrace,HDRBYTES);
      dtrace.ns = ns;
      dtrace.dt = 1000 * dt;
      dtrace.delrt = min;
      dtrace.cdp = mtrace.cdp + i + 1;
      for(k=0; k<ns; k++)
        dtrace.data[k] = 0.0;
      /* convolve each non-zero or non-null in model with wavelet */
      if(mtrace.data[i] != 0 && mtrace.data[i] != null) {
        /* input model times are in mills */
        mtime = 0.001 * mtrace.data[j];
        /* convert to ms */
        if(strcmp(depth,"yes"))
          mtime *= 1000.;
        conv(1,0,&mtrace.data[i],wtrace.ns,0,wtrace.data,wtrace.ns,0,work);
        /* post output center point at model time/depth */
        /* assign wavelet start, end times based on mtime, wlen, wdt */
        wstart = mtime - (wlen / 2);
        wend = mtime + (wlen / 2);
        /*if(wend > wlen)
          wend = wlen;*/
        if(mtrace.cdp == debug_cdp)
          fprintf(stderr,"cdp=%d, mtime=%f, wstart=%f, wend=%f, rc=%f\n",mtrace.cdp,mtime,wstart,wend,mtrace.data[i]);
        /* loop over output times */
        for(otime=min; otime<=max; otime+=dt) {
          if(otime >= wstart && otime <= wend) {
            /* wavelet sample indices */
            w1 = (otime - wstart) / wdt;
            w2 = w1 + 1;
            /* interp fractions */
            f1 = 1.0 - (otime - (wstart + w1 * wdt)) / wdt;
            f2 = 1.0 - ((wstart + w2 * wdt) - otime) / wdt;
            iout = (otime - min) / dt;
            if(mtrace.cdp == debug_cdp)
              fprintf(stderr,"outtime=%f,outindex=%d,wtime1=%f,w1=%d,wtime2=%f,w2=%d,f1=%f,f2=%f\n",
                      otime,iout,wstart+w1*wdt,w1,wstart+w2*wdt,w2,f1,f2);
            otrace.data[iout] += (work[w1] * f1 + work[w2] * f2);
            dtrace.data[iout] = (work[w1] * f1 + work[w2] * f2);
          }
        }
        if(mtrace.cdp == debug_cdp)
          puttr(&dtrace);
      }
    }
    if(debug_cdp == 0)
      puttr(&otrace);

  } while(gettr(&mtrace));

  return EXIT_SUCCESS;

}
