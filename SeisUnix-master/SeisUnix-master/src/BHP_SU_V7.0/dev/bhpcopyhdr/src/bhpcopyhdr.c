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
#include "bhp_hdr.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"                                                                       ",
" BHPCOPYHDR Copy Trace Header to Trace Sample or Trace Sample to Trace Header",
" 									",
"   bhpcopyhdr < stdin > stdout [optional parameters]                  ",
" 									",
" Required Parameters:                                                  ",
"   None                                                                ",
" 									",
" Optional parameters:						        ",
"   hdrs=d2         Header to copy from/to                              ",
"   samples=1       Samples to copy to/from. 1 means first sample in trace",
"                   NOTE: Multiple headers and samples may be specified ",
"                   by using comma-separated list. If more than one is  ",
"                   specified, list lengths must be equal.              ",
"   dir=htos        Default direction of copy is header-to-sample.      ",
"                   To copy sample-to-header, use dir=stoh              ",
"   scalar=1        Scalar to apply to copied value.                    ",
"   bias=0          Bias to add to scaled destination value.            ",
"                                                                       ",
"   destination = bias + scalar * source , for each item in list        ",
"                                                                       ",
NULL}; 

segy tr;

int main(int argc, char **argv)
{

  int *index;           /* header indices */
  int nhdrs=0;          /* Number of hdrs specified */
  int nsamp=0;          /* number of samples - must be equal nhdrs */
  int i;                /* Counter */
  int verbose;          /* Debug */
  int samples[SU_NKEYS]; /* sample indices to copy (1-based) */

  char *dir;            /* direction of transfer, htos or stoh */
  char **type;          /* header types */
  cwp_String hdrs[SU_NKEYS]; /* header names to copy */

  float bias;           /* Bias to add to scalar * z */
  float scalar;         /* Scalar to apply to z */
  float val;            /* value from header or horizon */

  Value h;              /* header value */

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* debug option */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* hdrs and samples parms */
  nhdrs = countparval("hdrs");
  if(nhdrs == 0) {
    nhdrs = 1;
    hdrs[0] = "d2";
  }
  else 
    getparstringarray("hdrs",hdrs);
  nsamp = countparval("samples");
  if(nsamp == 0) {
    nsamp = 1;
    samples[0] = 1;
  }
  else 
    getparint("samples",samples);

  /* direction */
  if(!getparstring("dir",&dir))
    dir = "htos";

  /* get types and indexes corresponding to keys and hdrs */
  index = calloc(nhdrs,sizeof(int));
  type = calloc(nhdrs,sizeof(char *));
  for(i=0; i<nhdrs; i++)
    type[i] = calloc(8,sizeof(char));
  for(i=0; i<nhdrs; i++) {
    type[i]=hdtype(hdrs[i]);
    index[i]=getindex(hdrs[i]);
  }

  /*Get bias, scalar */
  if(!getparfloat("bias",&bias))
    bias = 0.;
  if(!getparfloat("scalar",&scalar))
    scalar = 1.;

  if(verbose) {
     if(!strcmp(dir,"htos"))
       fprintf(stderr,"Transferring data FROM headers TO samples\n");
     else if(!strcmp(dir,"stoh"))
       fprintf(stderr,"Transferring data FROM samples TO headers\n");
     for(i=0; i<nhdrs; i++)
       fprintf(stderr,"HDR: %s, INDEX: %d, TYPE: %s\n",hdrs[i],index[i],type[i]);
     fprintf(stderr,"SAMPLES: \n");
     for(i=0; i<nsamp; i++)
       fprintf(stderr," %d ",samples[i]);
     fprintf(stderr,"\n");
     if(bias != 0 || scalar != 1)
       fprintf(stderr,"Destination values = %.3f + %.3f * source values\n",bias,scalar);
  }

  /* check first trace */
  if(!gettr(&tr))
    err("Can't get first trace\n");

  if(strcmp(dir,"htos") && strcmp(dir,"stoh"))
    err("Direction must be htos or stoh\n");
  if(nhdrs != nsamp)
    err("Number of headers to transfer must be equal number of samples\n");
  /* make sure sample indices are in range */
  for(i=0; i<nsamp; i++) {
    if(samples[i] > (int)tr.ns)
      err("Sample %d is past end of trace (ns=%d)\n",samples[i],tr.ns);
    if(samples[i] <= 0)
      err("Sample %d is too small, specify 1 thru ns (ns=%d)\n",samples[i],tr.ns);
  }
 
  /* loop over traces */
  do {
    for(i=0; i<nhdrs; i++) {
      if(!strcmp(dir,"htos")) {
        gethval(&tr,index[i],&h);
        val = vtof(type[i],h);
        if(val == -999.0)
          val = -999.25;
        scalhdr(&tr,hdrs[i],&val,STORE);
        val *= scalar + bias;
        tr.data[samples[i]-1] = val;
      }
      else {
        val = tr.data[samples[i]-1];
        scalhdr(&tr,hdrs[i],&val,LOAD);
        val *= scalar + bias;
        setval1(type[i],&h,(double)val);
        puthval(&tr,index[i],&h);
      }
    }
    puttr(&tr);
  } while(gettr(&tr));

  return EXIT_SUCCESS;
}
