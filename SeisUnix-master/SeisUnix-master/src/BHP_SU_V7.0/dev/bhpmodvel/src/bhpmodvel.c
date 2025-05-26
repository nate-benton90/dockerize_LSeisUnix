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
" BHPMODVEL - Convert ROFF model traces from depth to time, using",
"             a user-supplied velocity function                  ",
"                                                                ",
" bhpmodvel < input > output                                     ",
"                                                                ",
" Required Parameters: none                                      ",
"                                                                ",
" Optional Parameters:                                           ",
"   vels=vel.su          SU data file containing interval-in-depth ",
"                        velocity function to use for conversion",   
"                        Currently, only a single velocity function",
"                        is used for the depth-time mapping ",
"   nprop=1              Number of properties contained in the",
"                        ROFF model traces",
"                                                                ",
NULL};

/* Globals */
segy trace;                    /* model trace */
segy vtrace;                   /* velocity trace */

/* Prototypes */

int main(int argc, char **argv)
{

  char *vels;        /* velocity file */

  FILE *vfp;         /* vels file */

  int verbose;       /* debug printout */
  int i;             /* Loop counters */
  int dz;            /* vel depth interval */
  int nprop;         /* number of properties in model */
  int idepth;        /* starting sample number of depths in model */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* Get parms */
  if(!getparint("verbose",&verbose))
    verbose = 0;
  if(!getparstring("vels",&vels))
    vels = "vel.su";
  if(!getparint("nprop",&nprop))
    nprop = 1;

  if(verbose)
    fprintf(stderr,"Opening %s\n", vels);
  vfp = fopen(vels,"r");

  fgettr(vfp,&vtrace);
  /* Assume depth interval is times 1000 */
  dz = vtrace.dt / 1000;
  fclose(vfp);

  if(verbose) {
    fprintf(stderr,"Getting vels from %s\n", vels);
    fprintf(stderr,"Number of properties is %d\n", nprop);
    fprintf(stderr,"Depth interval of velocities is %d\n", dz);
  }

  if(!gettr(&trace))
    err("Can't get first model trace");
  idepth = nprop * ((trace.ns - 1) / (nprop + 1));

  do {
    trace.dt = 4000;
    /* Map depths to time */
    for(i=idepth; i<trace.ns; i++) {
      trace.data[i] = 2.0 * trace.data[i] / vtrace.data[(int)(trace.data[i])/dz];
      trace.data[i] *= 1000;
    }
    puttr(&trace);
  } while(gettr(&trace));

  return EXIT_SUCCESS;

}
