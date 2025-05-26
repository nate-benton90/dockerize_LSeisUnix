
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
#include "bhpio.h"
#include "bhpiocube.h"

int *alloc_fold(int fkeys, int *nklist, int *nfold, int verbose)
{

  int *ftable;   /* fold table pointer */
  int i;

  (*nfold) = nklist[0];

  for(i=1; i<fkeys; i++)
    (*nfold) *= nklist[i];

  ftable = calloc(*nfold,sizeof(int));

  if(verbose > 0)
    fprintf(stderr,"Allocated %d ints for ftable\n",*nfold);

  return ftable;

}

void check_fold(segy *trout, segy *tr, int *fold, int *ftable, int nfold,
                float **klist, int *nklist, int *index, int nkeys, int last,
                char *request, int *is_in_cube, int my_endian, int endian, int verbose)
{

  int i, j;
  int loc;
  int end_ens;
  static int first=1;
  static int *offset;

  short nsamp;

  Value hval1, hval2;

  if(first == 1) {
    first = 0;
    offset = calloc(nfold,sizeof(int));
  }

  end_ens = 0;
  (*fold)++;
  for(i=0; i<nkeys; i++) {
    gethval(trout,index[is_in_cube[i]],&hval1);
    gethval(tr,index[is_in_cube[i]],&hval2);
    if(hval1.i != hval2.i) {
      end_ens = 1;
      break;
    }
  }
  if(end_ens == 1 || last == 1) {
    for(i=0; i<nkeys; i++)
      offset[i] = -1;
    for(i=0; i<nkeys; i++) {
      if(last == 1)
        gethval(tr,index[is_in_cube[i]],&hval1);
       else
        gethval(trout,index[is_in_cube[i]],&hval1);
      for(j=0; j<nklist[i]; j++) {
        if(hval1.i == klist[i][j]) {
          offset[i] = j;
          break;
        }
      }
    }
    for(i=0; i<nkeys; i++) {
      if(offset[i] == -1) {
        if(verbose)
          fprintf(stderr,"Skipping fold\n");
        break;
      }
    }
    loc = offset[0];
    for(i=1; i<nkeys; i++) {
      loc *= nklist[i];
      loc += offset[i];
    }
    ftable[loc] = *fold;
    if(!strcmp(request,"fold")) {
      nsamp = 1;
      if(last == 1) {
        tr->ns = 1;
        tr->data[0] = (float)*fold;
        check_endian(my_endian,endian,tr,tr->ns,verbose);
        write_trace(tr,nsamp);
      }
      else {
        trout->ns = 1;
        trout->data[0] = (float)*fold;
        check_endian(my_endian,endian,trout,trout->ns,verbose);
        write_trace(trout,nsamp);
      }
    }
    *fold = 0;
  }
}

void write_fold(FILE *ffp, int *ftable, int nfold, int nkeys, int *nklist,
                float **klist, int verbose)
{

  int i, j;
  int *next;

  next = calloc(nkeys,sizeof(int));
  for(i=0; i<nkeys; i++)
    next[i] = 0;

  /* loop over klist, dump fold count */
  for(i=0; i<nfold; i++) {
    for(j=0; j<nkeys; j++)
      fprintf(ffp,"%d ",(int)klist[j][next[j]]);
    fprintf(ffp,"%d\n",ftable[i]);
    for(j=nkeys-1; j>=0; j--) {
      next[j]++;
      if(next[j] == nklist[j])
        next[j] = 0;
      else
        break;
    }
  }
}
