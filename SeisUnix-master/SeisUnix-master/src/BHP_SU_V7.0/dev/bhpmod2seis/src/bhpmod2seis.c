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

/*********************** self documentation **********************/
char *sdoc[] = {
" BHPMOD2SEIS converts horizon-based velocities to SU velocity traces",
"  bhpmod2seis < stdin > stdout maxdepth=                    ",
"  Required Parameters:   ",
"    maxdepth=          Used to set output trace length ",
"    verbose=0          Debug print                     ",
"    fill=no            Don't fill in missing values",
"                       Use fill=ext to extrapolate",
NULL};

/* Globals */
segy trin;                     /* Input Trace */
segy trout;                    /* Output Trace */

/* Prototypes */

int main(int argc, char **argv)
{

  char *fill;                  /* no=leave zeros, ext=extrapolate, int=interpolate */

  float v1, v2;                /* adjacent layer velocities */
  float rho1, rho2;            /* adjacent layer densities */
  float dzkm;                  /* dz in km */
  float maxdepthkm;            /* Max depth in model in km */
  float null;                  /* null value = -999.25 */

  int iv, id;                  /* sample indices of first vel and density layers */
  int verbose;                 /* debug printout */
  int i, j;                    /* Loop counters */
  int maxdepth;                /* Max depth in model */
  int dz;                      /* Model depth interval */

  short nsout;                 /* Number of samples in output = maxdepth/dz + 1 */
  short nsin;                  /* Number of samples per property in input = (tr.ns-1)/2 */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* output max depth */
  if(!getparint("maxdepth",&maxdepth))
    err("maxdepth must be specified\n");
  maxdepthkm = 0.001 * maxdepth;

  /* get ns from first trace */
  if(!gettr(&trin))
    err("Can't get first trace");
  nsin = trin.ns;

  /* fill holes? */
  if(!getparstring("fill",&fill))
    fill = "no";

  /* output depth interval */
  if(!getparint("dz",&dz))
    dz = 10;
  dzkm = 0.001 * dz;
  if(dz <= 0)
    err("dz=%d, dz must be > 0\n", dz);

  /* null value */
  if(!getparfloat("null",&null))
    null = -999.25;

  /* Set nsout */
  nsout = maxdepth / dz;

  if(verbose) {
    fprintf(stderr,"maxdepth=%d, maxdepthkm=%f\n",maxdepth,maxdepthkm);
    fprintf(stderr,"dz=%d, dzkm=%f\n",dz,dzkm);
    fprintf(stderr,"Setting samples per trace to %d\n", nsout);
    fprintf(stderr,"null=%f\n",null);
    if(!strcmp(fill,"ext"))
      fprintf(stderr,"Using extrapolation to fill traces\n");
    else if(!strcmp(fill,"int"))
      fprintf(stderr,"Using interpolation to fill traces\n");
    else
      fprintf(stderr,"Allowing zeros in output\n");
  }

  do {
    memcpy((void *)&trout,(const void *)&trin,HDRBYTES);
    trout.dt = dz;
    trout.ns = nsout;
    /* Zero fill output */
    for(i=0; i<nsout; i++)
      trout.data[i] = 0;
    /* map non-zero property values to their output locations */
    for(i=0; i<(nsin-1)/2; i++) {
      /* put sample out if non-zero and non-null */
      if(trin.data[i] != 0 && trin.data[i] != null) {
        j = trin.data[(nsin-1)/2+i]/dzkm;
        if(j < nsout)
          trout.data[j] = trin.data[i];
      }  
    }
    /* check fill */
    if(!strcmp(fill,"ext")) {
      for(i=0; i<nsout; i++) {
        /* find first non_zero */
        if(trout.data[i] != 0) {
          /* back fill */
          for(j=0; j<i; j++)
            trout.data[j] = trout.data[i];
          for(j=i+1; j<nsout; j++) {
            if(trout.data[j] == 0)
              trout.data[j] = trout.data[j-1];
          }
          break;
        }
      }
    }

    puttr(&trout);
  } while(gettr(&trin));

  return EXIT_SUCCESS;

}
