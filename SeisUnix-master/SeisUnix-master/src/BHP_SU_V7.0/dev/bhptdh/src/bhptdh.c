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
#include "bhpio_api.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 					                        ",
" BHPTDH - BHP Horizon Time/Depth Conversion                    ",
" 					                        ",
" BHPTDH     converts BHPIO horizon data                        ",
"            from time to depth and vice versa.                 ",
" 					                        ",
" bhptdh < infile > outfile [optional parameters]               ",
" 					                        ",
" Required Parameters: none		                        ",
" 					                        ",
" Optional Parameters:			                        ",
"  verbose=0         Use verbose=1 for long printout               ",
"  type=time-depth   Conversion type; use depth-time for depth to  ",
"                    time conversion                               ",
"  velfile=vels      Velocities must be stored as a BHPIO cube     ",
"                    dataset                                       ",
"  pathlist=vels.dat Pathlist of velocity dataset                ",
"                                                               ",
NULL};

/**************** end self doc ********************************/

int main (int argc, char **argv)
{

  char *type;                /* conversion type */
  char *velfile;             /* velocity filename */
  char *pathlist;            /* pathlist */

  int verbose;               /* Debug print */
  int i, j;
  int *keyvals;              /* list of key values */
  int trcount;               /* number of traces returned by read_bhpio_trace */
  int *key_index;            /* key indices returned by open_bhpio_dataset */
  int nkeys;                 /* nkeys returned by open_bhpio_dataset */

  Value hval;                /* header value */
                                
  segy veltrace;             /* velocity trace */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* check type */
  if(!getparstring("type",&type))
    type = "time-depth";

  /* velfile, path */
  if(!getparstring("velfile",&velfile))
    velfile = "vels";
  if(!getparstring("pathlist",&pathlist))
    pathlist = "vels.dat";

  if(verbose) {
    fprintf(stderr,"Conversion type is %s\n",type);
    fprintf(stderr,"Velocity filename is %s\n",velfile);
    fprintf(stderr,"Velocity file pathlist is %s\n",pathlist);
  }

  /* open vel dataset */
  key_index = open_bhpio_dataset(velfile,pathlist,&nkeys,verbose);

  set_bhpio_binning_rule("match",verbose);
  set_bhpio_interp_flag("yes",verbose);
  
  /* key indices, vals */
  keyvals = calloc(file_hdr.nkeys,sizeof(int));

  /* check first trace */
  if(!gettr(&tr))
    err("Cannot get first horizon trace\n");

  /* Loop until done */
  do {
    /* key values */
    for(i=0; i<file_hdr.nkeys; i++) {
      gethval(&tr,key_index[i],&hval);
      keyvals[i] = hval.i;
    }
    /* get vel trace */
    trcount = read_bhpio_trace(keyvals,&veltrace,verbose);
    /* loop over horizons */
    for(i=0; i<tr.ns; i++) {
      j = tr.data[i] / (0.001 * veltrace.dt);
      if(!strcmp(type,"time-depth"))
        tr.data[i] = veltrace.data[j] * ((j * 0.000001 * veltrace.dt) / 2.);
      else if(!strcmp(type,"depth-time"))
        tr.data[i] = 2.0 * ((j * 0.001 * veltrace.dt) / veltrace.data[j]);
    }
    puttr(&tr);
  } while(gettr(&tr));

  close_bhpio_dataset();

  return EXIT_SUCCESS;

}
