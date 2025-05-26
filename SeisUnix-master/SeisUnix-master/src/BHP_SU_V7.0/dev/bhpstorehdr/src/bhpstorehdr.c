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
/******************************************************************
**
*
* KEYWORDS:  $RCSfile: bhpstorehdr.c,v $
*            $Revision: 1.9 $
*            $Date: 2002/10/04 18:40:15 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpstorehdr.c,v $
* Revision 1.9  2002/10/04 18:40:15  ahmilb
* Initialize nhorz to zero.
*
* Revision 1.8  2002/10/03 13:52:08  ahmilb
* Update self-doc to show old-style and new-style parameters.
*
* Revision 1.7  2002/10/01 18:07:05  ahmilb
* Fix getparstringarray usage.
* Fix bug in handling old-style parameters.
*
* Revision 1.6  2002/09/26 16:05:57  ahmilb
* Allow more than 2 keys and multiple headers stored per pass.
*
* Revision 1.5  2001/09/13 19:04:22  ahmilb
* Move scalhdr function to libcube/src/hdr_lib.c
*
* Revision 1.4  2001/03/21 17:07:53  ahmilb
* Add puttr to avoid redundant SEGY output.
* Make all parameters optional.
* Add compress parameter to allow on-the-fly compression of ASCII files.
*
* Revision 1.3  2001/02/09 00:16:15  ahglim
* updated to Bob's ahmilb (beta) directory
*
* Revision 1.2  2001/02/06 03:45:18  ahglim
* corrected comment problem
*
*
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "bhp_interp.h"
#include "bhp_hdr.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"                                                                       ",
" BHPSTOREHDR Store Trace Headers or Horizon Values in an ASCII file    ",
" 									",
"   bhpstorehdr < stdin > stdout [optional parameters]                  ",
" 									",
" Required Parameters:                                                  ",
"   None                                                                ",
" 									",
" Optional parameters:						        ",
"   keys=ep,cdp     Trace headers to write as keys in output file.      ",
"                   Any number of keys can be specified.                ",
"   hdrs=d2         Multiple headers can be stored in a single execution",
"                   of bhpstorehdr by specifying a comma-separated list.",
"                   All of the headers along with the keys are written  ",
"                   on each line of the ASCII file                      ",
"     OR                                                                ",
"   key=fldr,cdp,d2 Old-style method of specifying 2 keys and one header",
"   horizons=no     If horizons=yes is present, then trace samples are  ",
"                   stored instead of header values. It is assumed that ",
"                   horizon data is being passed into bhpstorehdr via  ",
"                   bhpread. All of the samples of each trace are written",
"                   along with the keys on each line of the ASCII file  ",
"   file=picks.dat  ASCII file in which to write keys and values.       ",
"   scalar=1        Scalar to apply to hdrs or horizon values           ",
"   bias=0          Bias to add to scalar times hdrs or horizon values  ",
"   compress=no     Specify compress=yes to write the ASCII output via the",
"                   UNIX compress command; this option can save significant",
"                   disk space if you are dealing with large datasets;  ",
"                   If you use compression, the output file name will be",
"                   named 'file'.Z, and can be uncompressed with the",
"                   UNIX command, uncompress 'file'                    ",
"                                                                       ",
NULL}; 

segy tr;

int main(int argc, char **argv)
{

  int *index;           /* header indices */
  int nkeys=0;          /* Number of keys specified */
  int nhdrs=0;          /* Number of hdrs specified */
  int nhorz=0;          /* number of horizons, i.e. tr.ns if horizons=yes is specified */
  int nvals;            /* number of values to output, either nhdrs or nhorz */
  int i;                /* Counter */
  int verbose;          /* Debug */

  cwp_String keys[SU_NKEYS]; /* header keys */
  cwp_String hdrs[SU_NKEYS]; /* headers to store as picks */

  char **type;          /* header types */
  char *file;           /* name of output file */
  char *compress;       /* Compression parm */
  char cmdbuf[BUFSIZ];  /* UNIX compress command pipe */
  char *horizons;       /* horizons=yes or no */

  FILE *fp=NULL;        /* output file pointer file */
  FILE *pipefp;         /* compress command pipe */

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

  /* check for old style parms */
  nkeys = countparval("key");
  if(nkeys > 0) {
    getparstringarray("key",keys);
    if(nkeys != 3)
      err("Exactly 3 keynames must be specified\n");
    nhdrs = 1;
    hdrs[0] = keys[2];
    nkeys = 2;
  }

  /* key names */
  if(nkeys == 0) {
    nkeys = countparval("keys");
    if(nkeys == 0) {
      nkeys = 2;
      keys[0] = "ep";
      keys[1] = "cdp";
    }
    else
      getparstringarray("keys",keys);
  }

  /* check first trace */
  if(!gettr(&tr))
    err("Can't get first trace\n");

  /* if horizons specified, use them instead of hdrs */
  if(!getparstring("horizons",&horizons))
    nhorz = 0;
  else if((strcmp(horizons,"yes")) == 0) {
    nhorz = tr.ns;
    nhdrs = 0;
  }

  /* if no horizons, use hdrs */
  if(nhorz == 0) {
    if(nhdrs == 0) {
      nhdrs = countparval("hdrs");
      if(nhdrs == 0) {
        nhdrs = 1;
        hdrs[0] = "d2";
      }
      else
        getparstringarray("hdrs",hdrs);
    }
  }
  nvals = (nhorz == 0) ? nhdrs : nhorz;

  /* get types and indexes corresponding to keys and hdrs */
  index = calloc(nkeys+nhdrs,sizeof(int));
  type = calloc(nkeys+nhdrs,sizeof(char *));
  for(i=0; i<nkeys+nhdrs; i++)
    type[i] = calloc(8,sizeof(char));
  for(i=0; i<nkeys; i++) {
    type[i]=hdtype(keys[i]);
    index[i]=getindex(keys[i]);
  }
  for(i=0; i<nhdrs; i++) {
    type[nkeys+i]=hdtype(hdrs[i]);
    index[nkeys+i]=getindex(hdrs[i]);
  }

  /* get name of file */
  if(!getparstring("file",&file) && !getparstring("ofile",&file))
    file = "picks.dat";

  /*Get bias, scalar */
  if(!getparfloat("bias",&bias))
    bias = 0.;
  if(!getparfloat("scalar",&scalar))
    scalar = 1.;

  /* Compression? */
  if(!getparstring("compress",&compress))
    compress = "no";

  /* open file or pipe */
  if(!strcmp(compress,"no")) {
    if((fp=efopen(file,"w"))==NULL)
      err("cannot open file=%s\n",file);
  }
  else {
    strcat(file,".Z");
    sprintf(cmdbuf,"compress > %s", file);
    pipefp = epopen(cmdbuf,"w");
  }

  if(verbose) {
    for(i=0; i<nkeys; i++)
      fprintf(stderr,"KEY: %s, INDEX: %d, TYPE: %s\n",keys[i],index[i],type[i]);
    for(i=0; i<nhdrs; i++)
      fprintf(stderr,"HDR: %s, INDEX: %d, TYPE: %s\n",hdrs[i],index[nkeys+i],type[nkeys+i]);
    if(nhorz > 0)
      fprintf(stderr,"Writing horizons instead of hdrs\n");
    fprintf(stderr,"Writing %d keys plus %d header or horizon values\n",nkeys,nvals);
    fprintf(stderr,"BIAS: %f, SCALAR: %f\n",bias,scalar);
    fprintf(stderr,"FILE: %s\n",file);
    fprintf(stderr,"COMPRESS %s\n", compress);
  }

  /* loop over traces */
  while(1) {
    for(i=0; i<nkeys+nvals; i++) {
      /* get keys and hdrs from trace header */
      if(i < nkeys + nhdrs) {
        gethval(&tr,index[i],&h);
        val = vtof(type[i],h);
        if(val == -999.0)
          val = -999.25;
      }
      /* get horizons */
      if(i >= nkeys && nhorz > 0)
        val = tr.data[i-nkeys];
      /* Scale keys and hdrs, if necessary */
      if(i < nkeys)
        scalhdr(&tr,keys[i],&val,STORE);
      else if(i >= nkeys && nhdrs > 0)
        scalhdr(&tr,hdrs[i-nkeys],&val,STORE);
      /* Apply bias and scalar to hdrs and horizons */
      if(i >= nkeys)
        val *= scalar + bias;
      /* write keys and hdrs or horizons */
      if(!strcmp(compress,"no")) {
        if(i < nkeys)
          fprintf(fp,"%14.0f ",val);
        else
          fprintf(fp,"%30.4f ",val);
      }
      else {
        if(i < nkeys)
          fprintf(pipefp,"%14.0f ",val);
        else
          fprintf(pipefp,"%30.4f ",val);
      }
    }
    if(!strcmp(compress,"no"))
      fprintf(fp,"\n");
    else
      fprintf(pipefp,"\n");
    puttr(&tr);
    if(!gettr(&tr))
      break;
  }

  if(!strcmp(compress,"no"))
    efclose(fp);
  else
    epclose(pipefp);

  return EXIT_SUCCESS;
}
