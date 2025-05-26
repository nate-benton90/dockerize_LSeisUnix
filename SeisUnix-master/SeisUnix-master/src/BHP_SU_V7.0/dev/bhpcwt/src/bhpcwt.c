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
"  BHPCWT generates wavelet transform coefficients for traces.     ",
"         bhpcwt is adapted from the Matlab cwt function.          ",
"                                                                  ",
"  bhpcwt < stdin > stdout                                         ",
"  Required Parameters: none                                       ",
"  Optional Parameters:                                            ",
"    base=10         Base value for wavelet transform scales       ",
"    first=0         First exponent value for wavelet transform scales",
"    inc=0.01        Exponent increment for wavelet transform scales",
"    last=1.5        Last exponent value for wavelet transform scales",
"    wavelet=wavelet File containing a Mexican hat wavelet         ",
"                    You can use bhpwavelet to generate the wavelet",
"    hdr=f2          Trace header used to hold scale value for each trace",
"                    In addition, the sequential number of each transform",
"                    is stored in header cdpt, i.e. 1,2,,,         ",
"    velocity=2500   Velocity used to calculate value stored in hdr",
"                    Calculation used is:                          ",
"                     dt-in-mills*velocity*base**exp*0.001         ",
"    verbose=0       Debug print                                   ",
NULL};

int main(int argc, char **argv)
{

  char *wfile;          /* file containing wavelet trace */
  char *hdr;            /* scale header */
  char *type;           /* scale header type */

  float base;           /* base */
  float first;          /* first exponent */
  float inc;            /* exponent increment */
  float last;           /* last exponent */
  float exp;            /* each exponent */
  float velocity;       /* velocity used in scale value */
  float x;
  float p1=-8;          /* initial xval */
  float p2=8;           /* last xval */
  float dx;             /* xval incr */
  float xmax;           /* last xval - first vval */
  float winc;           /* wavelet interval */
  float fmin, fmax;     /* min, max filt value (debug) */
  float *xval;          /* wavelet xvalues */
  float **filt;         /* filter used for each conv */
  float *f;             /* scratch for filter fliplr */
  float *buff;          /* scratch for convolution */
  float *scales;        /* scales */
  float *cumsum;        /* cumulative sum of wavelet */

  segy wavelet;         /* Mexican hat wavelet trace (from bhpwavelet) */
  segy trace;           /* data for which transform is calculated */
  segy coefs;           /* transforms */

  int np=1024;          /* number of wavelet points */
  int i, j, k;
  int verbose;
  int *index;           /* wavelet subscripts to use for filter */
  int *nconv;           /* length of each filter */
  int nscales;          /* number of scales */
  int hindex;           /* index of hdr */

  FILE *fp;             /* wavelet file */

  Value h;              /* hdr value */

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* debug option */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* scales limits, inc */
  if(!getparfloat("base",&base))
    base = 10.;
  if(!getparfloat("first",&first))
    first = 0.0;
  if(!getparfloat("inc",&inc))
    inc = 0.01;
  if(!getparfloat("last",&last))
    last = 1.5;
  if(!getparfloat("velocity",&velocity))
    velocity = 2500.;
  if(!getparstring("wavelet",&wfile))
    wfile = "wavelet";
  if(!getparstring("hdr",&hdr))
    hdr = "f2";
  hindex = getindex(hdr);
  type = hdtype(hdr);

  if(verbose) {
    fprintf(stderr,"Getting wavelet from %s\n",wfile);
    fprintf(stderr,"base=%f, first=%f, inc=%f, last=%f\n",base,first,inc,last);
    fprintf(stderr,"Using velocity=%f for scale calculation\n",velocity);
    fprintf(stderr,"Storing calculated scale value in %s\n",hdr);
  }

  /* check wavelet file */
  fp = fopen(wfile,"r");
  if(fp == NULL)
    err("Cannot open %s\n",wfile);

  xval = calloc(np,sizeof(float));
  winc = (p2 - p1) / (np - 1);
  if(verbose)
    fprintf(stderr,"p1=%f, p2=%f, np=%d, inc=%f\n",p1,p2,np,winc);
  for(i=0,x=p1; i<np; i++,x+=winc)
    xval[i] = x;
  xval[np-1] = p2;

  /* compute scales */
  scales = calloc(SU_NFLTS,sizeof(float));
  exp = first;
  x = 0;
  nscales = 0;
  while(x <= pow(base,last)) {
    x = pow(base,exp);
    scales[nscales] = x;
    exp += inc;
    nscales++;
    if(nscales == SU_NFLTS)
      err("Too many scales, change params and re-run\n");
  }
  nscales--;

  if(verbose) {
    fprintf(stderr,"%d scales will be used for transforms\n",nscales);
    for(i=0; i<nscales; i++)
      fprintf(stderr," %f ",scales[i]);
    fprintf(stderr,"\n");
  }

  if(!fgettr(fp,&wavelet))
    err("Cannot get wavelet trace\n");
  if(verbose)
    fprintf(stderr,"Wavelet has %d samples\n",wavelet.ns);

  nconv = calloc(nscales,sizeof(int));
  index = calloc(wavelet.ns,sizeof(int));
  cumsum = calloc(wavelet.ns,sizeof(float));
  filt = calloc(nscales,sizeof(float *));
  f = calloc(wavelet.ns,sizeof(float));
  for(i=0; i<nscales; i++)
    filt[i] = calloc(wavelet.ns,sizeof(float));
  for(i=np-1; i>=0; i--)
    xval[i] = xval[i] - xval[0];  
  dx = xval[1];
  xmax = xval[wavelet.ns-1];

  if(verbose) {
    fprintf(stderr,"first xval=%f, last xval=%f\n",xval[0],xval[wavelet.ns-1]);
    fprintf(stderr,"dx=%f, xmax=%f\n",dx,xmax);
  }
  
  /* cumsum is cumulative sum of wavelet times dx */
  fmin = 0;
  for(i=0; i<wavelet.ns; i++) {
    fmin += wavelet.data[i];
    cumsum[i] = fmin * dx;
  }

  /* make filters from summed wavelet */
  for(i=0; i<nscales; i++) {
    nconv[i] = 1 + (int)(scales[i] * xmax);
    for(j=0; j<nconv[i]; j++)
      index[j] = 1 + j / (scales[i] * dx);
    for(j=0; j<nconv[i]; j++)
      f[j] = cumsum[index[j]-1];
    /* fliplr */
    for(j=0,k=nconv[i]-1; j<nconv[i]; j++,k--)
      filt[i][j] = f[k];
  }
  if(verbose) {
    fprintf(stderr,"Convolution Lengths\n");
    for(i=0; i<nscales; i++)
      fprintf(stderr,"%d ",nconv[i]);
    fprintf(stderr,"\n");
  }
  if(!gettr(&trace))
    err("Cannot get first trace\n");

  /* alloc buff for longest convolution */
  buff = calloc((int)trace.ns+nconv[nscales-1]+1,sizeof(float));
  
  do {
    memcpy((void *)&coefs,(const void *)&trace,HDRBYTES);
    /* apply filters to produce wavelet transform */
    for(i=0; i<nscales; i++) {
      for(j=0; j<trace.ns+nconv[nscales-1]+1; j++)
        buff[j] = 0;
      conv(trace.ns,0,trace.data,nconv[i],0,filt[i],trace.ns,0,buff);
      for(j=0; j<coefs.ns; j++)
        coefs.data[j] = buff[j+nconv[i]/2-1];
      for(j=coefs.ns-1; j>0; j--)
        coefs.data[j] = coefs.data[j] - coefs.data[j-1];
      for(j=0; j<coefs.ns; j++)
        coefs.data[j] = -sqrt(scales[i]) * coefs.data[j];
      h.f = velocity*trace.dt*0.001*scales[i]*0.001;
      puthval(&coefs,hindex,&h);
      coefs.cdpt = i + 1;
      puttr(&coefs);
    }
  } while(gettr(&trace));
  
 
  return EXIT_SUCCESS;

}
