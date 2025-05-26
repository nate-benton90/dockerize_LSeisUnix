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
"  BHPMAKERC converts a time model built by BHPMAKEMODEL to a reflection-coefficient",
"            model, which can then be used to make time synthetics",
"                          ",
"  bhpmakerc < stdin > stdout    ",
"  stdin and stdout are both assumed to be BHP horizon-based model datasets",
"  Required Parameters: none                                               ",
"  Optional Parameters:                                                    ",
"  verbose=0          Debug print=1                                        ",
NULL};

/* Globals */
segy trin;                     /* Input Trace */
segy trout;                    /* Output Trace */

/* Prototypes */

int main(int argc, char **argv)
{

  float v1, v2;                /* adjacent layer velocities */
  float rho1, rho2;            /* adjacent layer densities */

  int iv, id;                  /* sample indices of first vel and density layers */
  int verbose;                 /* debug printout */
  int i, j;                    /* Loop counters */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* check first trace */
  if(!gettr(&trin))
    err("Can't get first trace");

  if(verbose)
    fprintf(stderr,"Setting samples per trace to %d\n", trin.ns-(trin.ns/3)-2);

  do {
    memcpy((void *)&trout,(const void *)&trin,HDRBYTES);
    trout.ns = trin.ns - (trin.ns / 3) - 2;
    trout.dt = 1000;
    /* Zero fill output */
    for(i=0; i<trout.ns; i++)
      trout.data[i] = 0;
    /* compute refl coefs */
    for(i=0,iv=0,id=trout.ns/2+1; i<trout.ns/2; i++,iv++,id++) {
      v1 = trin.data[iv];
      v2 = trin.data[iv+1];
      rho1 = trin.data[id];
      rho2 = trin.data[id+1];
      trout.data[i] = (v2 * rho2 - v1 * rho1) / (v2 * rho2 + v1 * rho1);
    }
    /* depths or times */
    for(i=2*(trin.ns/3)+1,j=trout.ns/2,iv=0; j<trout.ns; i++,j++,iv++)
      trout.data[j] = trin.data[i];

    puttr(&trout);
  } while(gettr(&trin));

  return EXIT_SUCCESS;

}
