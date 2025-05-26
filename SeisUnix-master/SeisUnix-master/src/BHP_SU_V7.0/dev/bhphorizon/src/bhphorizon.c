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
#include <assert.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "cwp.h"
#include "bhp_interp.h"
#include "bhp_hdr.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" BHPHORIZON -  Build Horizon Trace                                     ",
" 									",
" bhphorizon < stdin > stdout [optional parameters]                     ",
" 									",
" bhphorizon builds 'horizon traces' by loading horizon times from      ",
" an ASCII file to a trace as samples. Existing seismic data are deleted",
" from traces. The output from bhphorizon is a dataset consisting of    ",
" trace headers with horizon times as samples.                          ",
" 									",
" bhphorizon uses the trace header 'trid' to determine if input traces  ",
" have already been populated with one or more horizons. If trid is not ",
" = 910, it is assumed that the input data is seismic. All trace values ",
" are replaced by horizon data, and the trid is set to 910.             ",
" If the input data already have trid=910, the horizon value is         ",
" added to the end of the trace. For horizon traces, the header 'ns' is ",
" set to the number of horizons, and 'dt' is set to 1000.               ",
" 									",
" Required parameters: none                                             ",
" 									",
" Optional parameters:						        ",
" keys=ep,cdp   Trace headers to use as keys. At least 2 keys must be   ",
"               present, with no upper limit.                           ",
"    OR                                                                 ",
" phdr=ep shdr=cdp Old-style key specification                          ",
" npicks=1      Number of horizons/picks in each ASCII record           ",
"               Multiple horizons may be loaded in one pass             ",
"               The total number of fields expected in each ASCII record",
"               is npicks plus the number of keys specified in keys=    ",
" file=horz.dat ASCII file containing key values and horizon data       ",
"               on each line, separated by spaces.                      ",
" extrap=no     For traces whose key values are outside the limits of   ",
"               the horizon data in the ASCII file, extrap=no returns a ",
"               null horizon value. extrap=yes returns the nearest      ",
"               horizon value.                                          ",
" interp=no     For traces whose key values are within the limits of the",
"               data but for which there is no horizon value,           ",
"               interp=no returns a null horizon value. The value       ",
"               returned for interp=yes depends on the number of keys.    ",
"               For 2 keys, interp=yes returns a bilinearly-iterpolated value.",
"               For >2 keys, interp=yes returns the nearest non-null value.",
" bias=0        The bias value is added to each horizon time/depth.     ",
" scalar=1      Each horizon value is multiplied by scalar before adding bias.",
" null=-999.25  Value to use as null for primaries,secondaries not found.",
"                                                                       ",
NULL}; 

segy tr;


/* Prototypes */
void init_data(segy *tr, int npicks);

int main(int argc, char **argv)
{

  cwp_String keys[SU_NKEYS]; /* header keys */
  cwp_String hdrs[SU_NKEYS]; /* headers to load if bhploadhdr */

  char *file;	        /* horizon file */
  char **type;          /* header key types */
  char **htype;         /* header key types for header to load if bhploadhdr */
  char buffer[1024];    /* buffer for input file */
  char *s;              /* individual field from buffer */
  char *extrap;         /* extrap=yes for extrapolation of keys outside cube/grid limits */
  char *interp;         /* yes or no to get nearest non-null */
  char *runmode;        /* mode=loadhdr, run as loadhdr, else run as horizon */
  char *phdr, *shdr;    /* old-style parms */

  int nkeys=0;          /* number of keys */
  int nhdrs=0;          /* number of headers to load if bhploadhdr */
  int *index;           /* header key indices */
  int *hindex;          /* header key indices to load if bhploadhdr */
  int i, j, k;          /* Loop counter */
  int forward, backward;/* cube indices from non-null search */
  int nf;               /* return from fgets */
  int nr;               /* Number of ASCII records */
  int verbose;          /* Debug */
  int mode=1;           /* 1=update exisitng horizon trace, 0=initialize */
  int start=0;          /* where to add data, 0 for initialize, ns+1 for update */
  int cube_size;        /* size of key cube in floats */
  int debug=0;          /* trace horizons as written */
  int npicks;           /* number of horizons to load */

  FILE *infp=NULL;	/* pointer to input file */

  float bias;           /* Bias to add to scalar * z */
  float scalar;         /* Scalar to apply to z */
  float null;           /* NULL value */
  float *cube;          /* key cube */
  float *val;           /* key values from each trace */
  float *z;             /* horizon or pick values */
  float *aval;          /* vals adjusted to nearest key increment */

  Value hval;           /* Header values */

  minmax **limits;      /* min,max,inc of keys */

  xyz_def *grid;       /* 2D grid of primary, secondary key values */

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* debug option */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* runmode */
  if(!getparstring("runmode",&runmode))
    runmode = "horizon";
  if(!strcmp(runmode,"horizon"))
    fprintf(stderr,"Running as BHPHORIZON\n");
  else if(!strcmp(runmode,"loadhdr"))
    fprintf(stderr,"Running as BHPLOADHDR\n");
  else
    err("Illegal runmode\n");

  /* check for old style parms */
  if(!strcmp(runmode,"horizon")) {
    if(getparstring("phdr",&phdr) || getparstring("shdr",&shdr)) {
      nkeys = 2;
      if(!getparstring("phdr",&keys[0]))
        keys[0] = "ep";
      if(!getparstring("shdr",&keys[1]))
        keys[1] = "cdp";
    }
  }
  else if(!strcmp(runmode,"loadhdr")) {
    if(getparstringarray("key",keys)) {
      nkeys = 2;
      if((countparval("key")) == 3) {
        nhdrs = 1;
        hdrs[0] = keys[2];
      }
      else if((countparval("key")) == 4) {
        nhdrs = 2;
        hdrs[0] = keys[2];
        hdrs[1] = keys[3];
      }
      else
        err("Key= must contain 3 or 4 names\n");
    }
  }

  /* key names */
  if(nkeys == 0) {
    nkeys = countparval("keys");
    if(nkeys == 0) {
      nkeys = 2;
      keys[0] = "ep";
      keys[1] = "cdp";
    }
    else {
      getparstringarray("keys",keys);
    }
  }
  /* single key not yet implemented */
  if(nkeys == 1)
    err("Single key not allowed\n");
  val = calloc(nkeys,sizeof(float));
  aval = calloc(nkeys,sizeof(float));

  /* key indices and types */
  index = calloc(nkeys,sizeof(int));
  type = calloc(nkeys,sizeof(char *));
  for(i=0; i<nkeys; i++) {
    index[i] = getindex(keys[i]);
    type[i] = calloc(8,sizeof(char));
    type[i] = hdtype(keys[i]);
  }

  /* number of horizons to load from file */
  if(!getparint("npicks",&npicks))
    npicks = 1;

  /* headers to load if bhploadhdr */
  if(strcmp(runmode,"loadhdr") == 0) {
    if(nhdrs == 0) {
      nhdrs = countparval("hdrs");
      if(nhdrs == 0) {
        nhdrs = 1;
        hdrs[0] = "d1";
      }
      else
        getparstringarray("hdrs",hdrs);
    }

    /* override npicks */
    npicks = nhdrs;
    /* hdr indices and types */
    hindex = calloc(nhdrs,sizeof(int));
    htype = calloc(nhdrs,sizeof(char *));
    for(i=0; i<nhdrs; i++) {
      hindex[i] = getindex(hdrs[i]);
      htype[i] = calloc(8,sizeof(char));
      htype[i] = hdtype(hdrs[i]);
    }
  }
  /* alloc z */
  z = calloc(npicks,sizeof(float));

  /* get name of file - check old style also */
  if(!getparstring("file",&file) && !getparstring("infile",&file))
    file = "horz.dat";

  /* bias, scalar, null, extrap, interp */
  if(!getparfloat("bias",&bias))
    bias = 0.;
  if(!getparfloat("scalar",&scalar))
    scalar = 1.;
  if(!getparfloat("null",&null))
    null = -999.25;
  if(!getparstring("extrap",&extrap))
    extrap = "no";
  if(!getparstring("interp",&interp))
    interp = "no";

  if(verbose) {
    for(i=0; i<nkeys; i++)
      fprintf(stderr,"KEY: %s, INDEX: %d, TYPE: %s\n",keys[i],index[i],type[i]);
    fprintf(stderr,"BIAS: %f, SCALAR: %f, NULL: %f\n",bias,scalar,null);
    fprintf(stderr,"Horizon File: %s\n",file);
    fprintf(stderr,"EXTRAP: %s, INTERP: %s\n",extrap,interp);
    if(!strcmp(interp,"yes") && nkeys == 2)
      fprintf(stderr,"Using bilinear interpolation\n");
    if(!strcmp(interp,"yes") && nkeys > 2)
      fprintf(stderr,"Using nearest non-null interpolation\n");
    if(strcmp(runmode,"loadhdr") == 0) {
      fprintf(stderr,"Headers to Load:\n");
      for(i=0; i<nhdrs; i++)
        fprintf(stderr,"    HDR: %s, INDEX: %d, TYPE: %s\n",hdrs[i],hindex[i],htype[i]);
    }
  }

  /* load file to cube structure */
  limits = calloc(nkeys,sizeof(minmax *));
  if(nkeys == 2)
    grid = load_file_to_grid(file,null,npicks,interp,verbose);
  else if(nkeys > 2)
    cube = load_file_to_cube(nkeys,file,&cube_size,npicks,limits,null,verbose);
  
  /* check first trace, initialize if necessary */
  if(!gettr(&tr))
    err("Can't get first trace\n");
  else
    if(strcmp(runmode,"horizon") == 0 && tr.trid != 910)
      mode = 0;

  if(verbose && strcmp(runmode,"horizon") == 0 && mode == 1)
    fprintf(stderr,"Adding horizon to existing data\n");
  else if(verbose && strcmp(runmode,"horizon") == 0 && mode == 0)
    fprintf(stderr,"Writing first horizon\n");

  /* loop over traces */
  do {
    if(strcmp(runmode,"horizon") == 0 && mode == 0) {
      init_data(&tr,npicks);
      start = 0;
    }
    else if(strcmp(runmode,"horizon") == 0 && mode == 1) {
      start = tr.ns;
      tr.ns += npicks;
    }
    /* key values to floats */
    for(i=0; i<nkeys; i++) {
      gethval(&tr,index[i],&hval);
      val[i] = vtof(type[i],hval);
      /* scale if necessary */
      scalhdr(&tr,keys[i],&val[i],LOAD);
    }
    /* if 2 keys, get value from grid */
    if(nkeys == 2) {
      bilinear(z,grid,val[0],val[1],extrap,interp,null,verbose);
      if(debug == 1) {
        fprintf(stderr,"keys=%f,%f horz=",val[0],val[1]);
        for(i=0; i<npicks; i++)
          fprintf(stderr,"%f ",z[i]);
        fprintf(stderr,"\n");
      }
    }
    /* more than 2, use cube */
    else {
      /* test key values against cube data limits */
      i = check_limits(nkeys,val,limits,verbose);
      /* use null if outside cube and extrap=no */
      if(i == 1 && !strcmp(extrap,"no")) {
        for(k=0; k<npicks; k++)
          z[k] = null;
      }
      /* inside cube or outside and extrap=yes */
      else if(i == 0 || (i == 1 && !strcmp(extrap,"yes"))) {
        if(i == 1)
          j = cube_extrap(nkeys,val,limits,npicks,verbose);
        for(j=0; j<nkeys; j++)
          aval[j] = ((int)val[j] + (int)limits[j]->incx - 1 - (int)limits[j]->minx) /
                    (int)limits[j]->incx * (int)limits[j]->incx + (int)limits[j]->minx;
        j = cube_lookup(nkeys,aval,limits,npicks,verbose);
        assert(j >= 0 && j < cube_size);
        /* loop over npicks */
        for(k=0; k<npicks; k++) {
          /* if null and interp=yes, get nearest non-null */
          if(cube[j+k] == null && !strcmp(interp,"yes")) {
            forward = find_non_null(cube,j+k,npicks,cube_size,null,verbose);
            backward = find_non_null(cube,j+k,npicks,0,null,verbose);
            if(forward == -1 && backward == -1)
              err("find_non_null returned all null\n");
            if(forward != -1 && backward != -1)
              j = ((forward - j) <= (j - backward)) ? forward : backward;
            else if(forward != -1)
              j = forward;
            else
              j = backward;
          }
          z[k] = cube[j+k];
        }
      }
      if(debug == 1) {
        fprintf(stderr,"keys=%f,%f,%f horz=",val[0],val[1],val[2]);
        for(k=0; k<npicks; k++)
          fprintf(stderr,"%f ",z[k]);
        fprintf(stderr,"\n");
      }
    }
    /* if horizon, build trace */
    if(strcmp(runmode,"horizon") == 0) {
      for(i=start,j=0; i<start+npicks; i++,j++) {
        if(z[j] != null)
          tr.data[i] = bias + scalar * z[j];
        else
          tr.data[i] = z[j];
      }
    }
    /* else load headers */
    else {
      for(i=0; i<nhdrs; i++) {
        if(z[i] != null)
          z[i] = bias + scalar * z[i];
        setval1(htype[i],&hval,(double)z[i]);
        puthval(&tr,hindex[i],&hval);
        /*if(verbose) {
          fprintf(stderr,"Loaded %f to %s for keys=",z[i],hdrs[i]);
          for(j=0; j<nkeys; j++)
            fprintf(stderr,"%f ",val[j]);
          fprintf(stderr,"\n");
        }*/
      }
    }
    puttr(&tr);
  } while (gettr(&tr));


  return EXIT_SUCCESS;
}

void init_data(segy *tr, int npicks)
{

  int i;

  for(i=0; i<tr->ns; i++)
    tr->data[i] = 0;
  tr->ns = npicks;
  tr->dt = 1000;
  tr->trid = 910;

}
