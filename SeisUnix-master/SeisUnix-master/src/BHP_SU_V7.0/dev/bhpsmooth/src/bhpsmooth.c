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

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
" bhpsmooth <stdin >stdout [optional parameters]                   ",
"                                                                  ",
" BHPSMOOTH is a panel smoother                                 ",
"  bhpsmooth is used in the wavelet-reservoir workflow to smooth   ",
"  and normalize wavelet transforms                                ",
"                                                                  ",
" Required Parameters: none                                        ",
"                                                                  ",
" Optional Parameters:                                             ",
"   option=none       Normalization options: none, divide, average ",
"                     none: do smoothing only                      ",
"                     divide: do smoothing, then divide each trace ",
"                             in the smoothed transform by the     ",
"                             corresponding trace in data from file",
"                     average: do smoothing, replace each sample of",
"                              the first smoothed transform        ",
"                              by the average of the corresponding ",
"                              sample in all smoothed transforms, then",
"                              write the average transform as the  ",
"                              only output ensemble                ",
"   file=norm.su      Name of dataset used if option=divide        ",
"                     the data must consist of a single ensemble which",
"                     conforms to each transform, i.e. same number ",
"                     of traces and samples                        ",
"   npass=1           Number of times to apply smoothing filter per",
"                     ensemble                                     ",
"   length=10         Length of square smoothing filter,           ",
"                     in milliseconds                              ",
"   key=fldr          Ensemble trace header key                    ",
"   nxmax=50          Number of traces per transform ensemble      ",
"   ngath=1           Number of ensembles per panel                ",
NULL};

/* Prototypes */
void get_ens(int i, int start, int nxmax, segy *trace, float **traces, int **headers, int *nx,
             int nsamp, int *havetrace, int *gottrace, int index, cwp_String type, int verbose);
float *make_ones(int flen, int verbose);
float **make_ones2d(int nt, int ns, int verbose);
void smooth2d(float **traces, float **work, int nsamp, int npass, int flen, int ngath,
              int *nx, float *ones, float **ones2d, float **corr, int verbose);

int main(int argc, char **argv)
{

  char *option;       /* none, divide, average */
  char *file;         /* normalization traces if option=divide */

  int npass;          /* number of smoothing passes, default=1 */
  int havetrace=0;    /* Flag for trace already in trace */
  int lastval;        /* Key value, previous trace */
  int lastens;        /* Key value, previous ensemble */
  int val;            /* Key value, current trace */
  int nxmax;          /* Max traces per ensemble; default=10 */
  int ngath;          /* Max ensembles per buffer; default=1 */
  int ntraces;        /* ngath * nxmax */
  int verbose;        /* For printing info */
  int *nx;            /* Number of traces per ensemble */ 
  int index;          /* key index */
  int gottrace=1;     /* =0 if EOF already encountered */
  int i, j, k;
  int flen;           /* length of smoother in samples */
  int **headers;      /* ensemble of headers */
  int nsamp;          /* samples per trace */
  int **fold;         /* fold count traces for option=average */
  int **aheaders;     /* headers from first input transform - used for average */
  int nens=0;         /* ensemble count */

  cwp_String key;     /* Ensemble key */
  cwp_String type;    /* Key type */

  Value kval;         /* Key value */

  segy trace;         /* wavelet transform trace */
  segy otrace;        /* output trace */
  segy nrmtrace;      /* normalization trace */

  float **traces;     /* ensemble of traces to smooth */
  float **nrmtraces;  /* ensemble of normalization traces */
  float **atraces;    /* average traces for option=average */
  float **work;       /* convolution workspace */
  float length;       /* length of square filter(milliseconds) used for smoothing, default=10 */
  float *ones;        /* array of ones */
  float **ones2d;     /* 2D array of ones */
  float **corr;       /* 2D array of scalars */

  FILE *fp;           /* normalization data if option=divide */

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* debug option */
  if(!getparint("verbose",&verbose))
    verbose = 0;
  /* option */
  if(!getparstring("option",&option))
    option= "none";
  /* check data if option=divide */
  if(!strcmp(option,"divide")) {
    if(!getparstring("file",&file))
      file = "norm.su";
  }
  /* filter length */
  if(!getparfloat("length",&length))
    length = 10.;
  /* num passes */
  if(!getparint("npass",&npass))
    npass = 1;
  /* max ens */
  if(!getparint("nxmax",&nxmax))
    nxmax = 50;
  /* ensembles per panel */ 
  if(!getparint("ngath",&ngath))
    ngath = 1;
  /* ensemble key */
  if(!getparstring("key",&key))
    key = "fldr";

  /* Key info */
  type = hdtype(key);
  index = getindex(key);

  /* Print info */
  if(verbose) {
    if(!strcmp(option,"none"))
      fprintf(stderr,"Doing smoothing only\n");
    else if(!strcmp(option,"divide"))
      fprintf(stderr,"Each transform will be normalized by data from %s\n",file);
    else if(!strcmp(option,"average"))
      fprintf(stderr,"Sample-by-sample average of all transforms will be output\n");
    fprintf(stderr,"Smoothing length in milliseconds: %f\n",length);
    fprintf(stderr,"Number of smoothing passes: %d\n",npass);
    fprintf(stderr,"Getting up to %d traces per ensemble\n",nxmax);
    fprintf(stderr,"Getting %d ensemble(s) per panel\n",ngath);
    fprintf(stderr,"Ensemble key = %s\n",key);
  }

  /* legal values required */
  if(length <= 0)
    err("length must be > 0\n");
  if(npass <= 0)
    err("npass must be > 0\n");
  if(nxmax <= 0)
    err("nxmax must be > 0\n");
  if(ngath <= 0)
    err("ngath must be > 0\n");

  /* verify option is valid choice */
  if(strcmp(option,"none") && strcmp(option,"divide") && strcmp(option,"average"))
    err("%s is not a valid option\n",option);
  
  /* verify normalization data is readable */
  if(!strcmp(option,"divide")) {
    if((fp = fopen(file,"r")) == NULL)
      err("Can't open %s\n",file);
  }

  /* Allocate nx, number of traces per gather */
  nx = calloc(ngath,sizeof(int));

  /* Get first trace, convert length to samples */
  if(!gettr(&trace))
    err("Can't get first transform trace");
  nsamp = (int)trace.ns;
  flen = NINT(length / ((float)trace.dt * 0.001));
  flen = flen * 2 + 1;
  if(verbose)
    fprintf(stderr,"Filter length in samples = %d\n",flen);

  /* check first norm trace if option=divide */
  if(!strcmp(option,"divide")) {
    if(!fgettr(fp,&nrmtrace))
      err("Can't get first normalization trace");
    if(nrmtrace.ns != trace.ns)
      err("Normalization trace has %d samples, transform trace has %d samples\n",
          nrmtrace.ns,trace.ns);
  }

  /* Key from first trace */
  gethval(&trace,index,&kval);
  val = vtoi(type,kval);
  lastval = val;
  ntraces = ngath * nxmax;

  /* Allocate headers, traces */
  work = calloc(ntraces,sizeof(float *));
  traces = calloc(ntraces,sizeof(float *));
  headers = calloc(ntraces,sizeof(int *));
  corr = calloc(ntraces,sizeof(float *));
  for(i=0; i<ngath*nxmax; i++) {
    headers[i] = calloc(HDRBYTES,sizeof(int));
    traces[i] = calloc(nsamp*sizeof(float),sizeof(float));
    work[i] = calloc(nsamp*sizeof(float),sizeof(float));
    corr[i] = calloc(nsamp*sizeof(float),sizeof(float));
  }
  /* allocate norm data if option=divide */
  if(!strcmp(option,"divide")) {
    nrmtraces = calloc(ntraces,sizeof(float *));
    for(i=0; i<ntraces; i++)
      nrmtraces[i] = calloc(nsamp*sizeof(float),sizeof(float));
  }
  /* allocate average traces, fold traces if option=average */
  if(!strcmp(option,"average")) {
    atraces = calloc(ntraces,sizeof(float *));
    aheaders = calloc(ntraces,sizeof(int *));
    fold = calloc(ntraces,sizeof(int *));
    for(i=0; i<ntraces; i++) {
      atraces[i] = calloc(nsamp*sizeof(float),sizeof(float));
      aheaders[i] = calloc(HDRBYTES,sizeof(int));
      fold[i] = calloc(nsamp*sizeof(int),sizeof(int));
    }
    for(i=0; i<ntraces; i++) {
      for(j=0; j<nsamp; j++) {
        atraces[i][j] = 0;
        fold[i][j] = 0;
      }
    }
  }  

  if(verbose)
    fprintf(stderr,"Allocated %d traces with %d samples each\n",ntraces,nsamp);

  /* array of ones */
  ones = make_ones(flen,verbose);
  ones2d = make_ones2d(nxmax,nsamp,verbose);

  /* save first trace */
  memcpy((void *)headers[0],(const void *)&trace,HDRBYTES);
  memcpy((void *)traces[0],(const void *)&trace.data,nsamp*sizeof(float));
  if(!strcmp(option,"divide"))
    memcpy((void *)nrmtraces[0],(const void *)&nrmtrace.data,nsamp*sizeof(float));
  else if(!strcmp(option,"average"))
    memcpy((void *)aheaders[0],(const void *)&trace,HDRBYTES);

  /* Fill remaining traces in first ensemble, followed by subsequent ensembles */
  for(i=0; i<ngath; i++) {
    if(i == 0)
      j = 1;
    else
      j = 0;
    if(gottrace) {
      get_ens(i,j,nxmax,&trace,traces,headers,nx,nsamp,&havetrace,&gottrace,index,type,verbose);
      gethval((segy *)headers[0],index,&kval);
      lastens = vtoi(type,kval);
      if(nx[i] > 0)
        nens++;
      if(nx[i] > 0 && verbose != 0)
        fprintf(stderr,"Got ensemble %d, with %d traces, gottrace=%d\n",nens,nx[i],gottrace);
      /* normalization traces */
      if(!strcmp(option,"divide")) {
        for(j=1; j<nx[i]; j++) {
          if(!fgettr(fp,&nrmtrace))
            err("EOF reading normalization data after only %d traces were read\n",j);
          else
            memcpy((void *)nrmtraces[j],(const void *)&nrmtrace.data,nsamp*sizeof(float));
        }
        if(verbose)
          fprintf(stderr,"Got %d normalization traces\n",nx[i]);
      }
      /* if averaging, save first headers */
      else if(!strcmp(option,"average")) {
        for(j=1; j<nx[i]; j++)
          memcpy((void *)aheaders[j],(const void *)headers[j],HDRBYTES);
      }
    }
  }

  /* loop until EOF */
  for(;;) {
    /* Smooth panel */
    if(nx[0] > 0)
      smooth2d(traces,work,nsamp,npass,flen,ngath,nx,ones,ones2d,corr,verbose);
    /* normalize */
    if(!strcmp(option,"divide")) {
      for(i=0; i<ntraces; i++) {
        for(j=0; j<nsamp; j++) {
          if(nrmtraces[i][j] != 0)
            traces[i][j] /= nrmtraces[i][j];
        }
      }
    }
    else if(!strcmp(option,"average")) {
      for(j=0; j<ntraces; j++) {
        for(k=0; k<nsamp; k++) {
          atraces[j][k] += traces[j][k];
          if(traces[j][k] != 0)
            fold[j][k]++;
        }
      }
    }
    for(i=0; i<ngath; i++) {
      for(j=0; j<nx[i]; j++) {
        if(i == 0) {
          memcpy((void *)&otrace,(const void *)headers[j],HDRBYTES);
          memcpy((void *)&otrace.data,(const void *)traces[j],nsamp*sizeof(float));
        }
        else {
          memcpy((void *)&otrace,(const void *)headers[i*nx[i-1]+j],HDRBYTES);
          memcpy((void *)&otrace.data,(const void *)traces[i*nx[i-1]+j],nsamp*sizeof(float));
        }
        /* write output if not averaging */
        if(strcmp(option,"average"))
          puttr(&otrace);
      }
    }
    /* At EOF? */
    if(!gottrace)
      break;
    /* Fill buffer */
    for(i=0; i<ngath; i++) {
      nx[i] = 0;
      if(gottrace) {
        j = 0;
        get_ens(i,j,nxmax,&trace,traces,headers,nx,nsamp,&havetrace,&gottrace,index,type,verbose);
        if(nx[i] > 0)
          nens++;
        if(nx[i] > 0 && verbose != 0)
          fprintf(stderr,"Got ensemble %d, with %d traces, gottrace=%d\n",nens,nx[i],gottrace);
        /* if this ensemble has same key as previous, nxmax is too small */
        if(nx[i] > 0) {
          gethval((segy *)headers[0],index,&kval);
          val = vtoi(type,kval);
          if(val == lastens)
            err("nxmax is too small\n");
          lastens = val;
        }
      }
    }
  }

  /* compute average if option=average */
  if(!strcmp(option,"average")) {
    for(i=0; i<ntraces; i++) {
      for(j=0; j<nsamp; j++) {
        if(fold[i][j] != 0)
          atraces[i][j] /= fold[i][j];
      }
      memcpy((void *)&otrace,(const void *)aheaders[i],HDRBYTES);
      memcpy((void *)&otrace.data,(const void *)atraces[i],nsamp*sizeof(float));
      puttr(&otrace);
    }
  }

  return(EXIT_SUCCESS);

}

void get_ens(int i, int start, int nxmax, segy *trace, float **traces, int **headers, int *nx,
             int nsamp, int *havetrace, int *gottrace, int index, cwp_String type, int verbose)
{

  int j, k;
  int end_ens;
  int val, lastval;

  Value kval;

  end_ens = 0;
  nx[i] = start;
  for(j=start; j<nxmax; j++) {
    if(!*gottrace || end_ens)
      break;
    /* First trace is already in trace? */
    if(*havetrace) {
      *havetrace = 0;
      if(i == 0) {
        memcpy((void *)headers[j],(const void *)trace,HDRBYTES);
        memcpy((void *)traces[j],(const void *)trace->data,nsamp*sizeof(float));
      }
      else {
        memcpy((void *)headers[i*nx[i-1]+j],(const void *)trace,HDRBYTES);
        memcpy((void *)traces[i*nx[i-1]+j],(const void *)trace->data,nsamp*sizeof(float));
      }
      gethval(trace,index,&kval);
      val = vtoi(type,kval);
      lastval = val;       
      nx[i]++;
      continue;
    }
    if(gettr(trace)) {
      gethval(trace,index,&kval);
      val = vtoi(type,kval);
      if(j == start)
        lastval = val;
      if(val == lastval) {
        if(i == 0) {
          memcpy((void *)headers[j],(const void *)trace,HDRBYTES);
          memcpy((void *)traces[j],(const void *)trace->data,nsamp*sizeof(float));
        }
        else {
          memcpy((void *)headers[i*nx[i-1]+j],(const void *)trace,HDRBYTES);
          memcpy((void *)traces[i*nx[i-1]+j],(const void *)trace->data,nsamp*sizeof(float));
        }
      }
      else {
        end_ens = 1;
        *havetrace = 1;
        break;
      }
    }
    else {
      *gottrace = 0;
      break;
    }
    if(*gottrace && !end_ens)
      nx[i]++;
  }
}

void smooth2d(float **traces, float **work, int nsamp, int npass, int flen, int ngath,
              int *nx, float *ones, float **ones2d, float **corr, int verbose)
{

  int i, j, k;
  int n=0;

  /* count traces in panel */
  for(i=0; i<ngath; i++)
    n += nx[i];

  /* compute correction matrix */
  for(i=0; i<n; i++) {
    conv(nsamp,0,ones2d[i],flen,-flen/2,ones,nsamp,0,corr[i]);
    for(j=0; j<nsamp; j++)
      corr[i][j] = 1. / corr[i][j];
  }

  /* abs value of input */
  for(i=0; i<n; i++)
    for(j=0; j<nsamp; j++)
      traces[i][j] = ABS(traces[i][j]);

  if(verbose)
    fprintf(stderr,"Applying %d passes of smoother over %d traces\n",npass,n);
  /* apply smoother npass times */
  for(i=0; i<npass; i++) {
    for(j=0; j<n; j++) {
      conv(nsamp,0,traces[j],flen,-flen/2,ones,nsamp,0,work[j]);
      for(k=0; k<nsamp; k++)
        traces[j][k] = work[j][k] * corr[j][k];
    }
  }

}

float *make_ones(int flen, int verbose)
{

  int i;

  float *ones;

  ones = calloc(flen,sizeof(float));

  for(i=0; i<flen; i++)
    ones[i] = 1.0;

  if(verbose)
    fprintf(stderr,"Made array of %d ones\n",flen);

  return ones;

}

float **make_ones2d(int nt, int ns, int verbose)
{

  float **ones;

  int i, j;

  ones = calloc(nt,sizeof(float *));
  for(i=0; i<nt; i++)
    ones[i] = calloc(ns,sizeof(float));

  for(i=0; i<nt; i++) {
    for(j=0; j<ns; j++)
      ones[i][j] = 1.;
  }
 
  if(verbose)
    fprintf(stderr,"Made 2D array of ones: %d traces, with %d samples each\n",nt,ns);

  return ones;
}
