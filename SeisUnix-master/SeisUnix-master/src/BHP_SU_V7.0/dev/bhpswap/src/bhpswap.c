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
* KEYWORDS:  $RCSfile: bhpswap.c,v $
*            $Revision: 1.3 $
*            $Date: 2003/04/01 22:10:11 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpswap.c,v $
* Revision 1.3  2003/04/01 22:10:11  ahmilb
* Fix for non-zero start time.
*
* Revision 1.2  2002/05/08 14:00:34  ahmilb
* Add secondary key parameter and use its value to map transposed samples.
* Pre-fill output traces with zeros in case not all secondaries are present.
*
* Revision 1.1  2002/01/30 16:16:25  ahmilb
* Initial version of transpose tool
*
* Revision 1.1  2001/02/07 19:55:46  ahglim
*
*
*
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "su.h"
#include "segy.h"
#include "header.h"


/*********************** self documentation **********************/
char *sdoc[] = {
" BHPSWAP transposes a dataset                                     ",
"  bhpswap                                                         ",
" Required Parameters: none                                        ",
" Optional Parameters:                                             ",
"  nxmax=350            Max traces per ensemble                    ",
"  pkey=ep              Ensemble key                               ",
"  skey=cdp,1,1         Secondary key name,min,incr                ",
"  nprop=0              If properties or horizons are present      ",
"                                                                  ",
NULL};

/* Globals */
segy intrace;      /* Input trace */
int havetrace=0;   /* Flag for trace already in intrace */
int lastval;       /* Key value, previous trace */
int val;           /* Key value, current trace */

/* Prototypes */
int get_ens(int nxmax, int **headers, float **traces, int *gottrace,
            int index, cwp_String ptype, int ns);

int main(int argc, char **argv)
{

  int nxmax=350;      /* Max traces per ensemble; default=350 */
  int verbose=0;      /* For printing info */
  int nx;             /* actual number of traces in each ensemble */ 
  int ns;             /* num samples per trace */
  int pindex;         /* Key index */
  int sindex;         /* secondary index */
  int gottrace=1;     /* =0 if EOF already encountered */
  int **headers;      /* ensemble of headers */
  int i, j, k, m;
  int nprop;          /* if >0 handle properties */
  int nlayers;        /* num layers if nprop>0 */
  int vindex;         /* index of vkey */
  int sinc;           /* secondary key incr */
  int s1;             /* secondary key min val */

  segy outtrace;      /* Output trace */

  float **traces;     /* ensemble of traces */

  cwp_String vkey;    /* Header to hold vertical info */
  cwp_String vtype;   /* Vertical Key type */
  cwp_String pkey;    /* Ensemble key */
  cwp_String ptype;   /* Primary Key type */
  char skey[8];       /* Secondary key */
  cwp_String key_string[3]; /* Temporary key holder */

  Value kval;         /* Key value */
  Value sval;         /* Key value */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* Get args*/
  if(!getparint("verbose",&verbose))
    verbose = 0;
  if(getparint("nxmax",&nxmax))
    if(nxmax <= 0)
      err("nxmax must be greater than zero\n");
  if(!getparstring("pkey",&pkey))
    pkey = "ep";
  if(!getparstringarray("skey",key_string)) {
    strcpy(skey,"cdp");
    sinc = 1;
    s1 = 1;
  }
  else {
    strcpy(skey,key_string[0]);
    s1 = atoi(key_string[1]);
    sinc = atoi(key_string[2]);
  }
  if(!getparstring("vkey",&vkey))
    vkey = "tracl";
  /* properties/horizons */
  if(!getparint("nprop",&nprop))
    nprop = 0;

  /* Key info */
  ptype = hdtype(pkey);
  pindex = getindex(pkey);
  sindex = getindex(skey);
  vtype = hdtype(vkey);
  vindex = getindex(vkey);

  /* Print info */
  if(verbose) {
    fprintf(stderr,"Getting up to %d traces per ensemble\n", nxmax);
    fprintf(stderr,"Using %s as ensemble key\n",pkey);
    fprintf(stderr,"Index of %s is %d\n",pkey,pindex);
    fprintf(stderr,"Using %s as secondary key\n",skey);
    fprintf(stderr,"Index of %s is %d\n",skey,sindex);
    if(nprop > 0)
      fprintf(stderr,"Processing %d properties\n",nprop);
    if(nprop < 0)
      fprintf(stderr,"Processing %d horizons\n",nprop);
  }

  /* Get first trace */
  if (!gettr(&intrace))
    err("can't get first trace");
  /* Key from first trace */
  gethval(&intrace,pindex,&kval);
  val = vtoi(ptype,kval);
  lastval = val;
  /* trace length */
  ns = intrace.ns;
  /* properties? */
  if(nprop > 0) {
    nlayers = (ns - 1) / (nprop + 1);
    if(verbose)
      fprintf(stderr,"Expecting %d layers\n",nlayers);
  }

  /* Allocate headers, traces */
  headers = calloc(nxmax,sizeof(char *));
  traces = calloc(nxmax,sizeof(float *));
  for(i=0; i<nxmax; i++) {
    headers[i] = calloc(HDRBYTES,sizeof(char **));
    traces[i] = calloc(ns,sizeof(float **));
  }
  if(verbose) {
    fprintf(stderr,"Allocated %d headers, %d bytes each\n", nxmax,HDRBYTES);
    fprintf(stderr,"Allocated %d traces , %d samples each\n", nxmax,ns);
  }

  /* Fill remaining traces in first ensemble */
  havetrace = 1;
  if(gottrace)
    nx = get_ens(nxmax,headers,traces,&gottrace,pindex,ptype,ns);

  /* Empty buffer, then re-fill until EOF */
  for(;;) {
    /* At EOF? */
    if(!gottrace)
      break;
    /* output each sample of all input traces as one output trace */
    memcpy((void *)&outtrace,(const void *)headers[0],HDRBYTES);
    outtrace.ns = nxmax;
    /* zero-fill in case not all samples present */
    for(i=0; i<nxmax; i++)
      outtrace.data[i] = 0;
    /* constrain ns if properties */
    if(nprop > 0)
      ns = nlayers * nprop;
    for(i=0; i<ns; i++) {
      /* map samples to transposed trace based on skey values */
      for(j=0; j<nx; j++) {
        gethval((segy *)headers[j],sindex,&sval);
        k = (sval.i - s1) / sinc;
        outtrace.data[k] = traces[j][i];
      }
      /* set vert key */
      if(nprop == 0)
        kval.i = outtrace.delrt + i * outtrace.dt * 0.001;
      else if(nprop != 0) {
        kval.i = i + 1;
        outtrace.dt = 1;
        outtrace.delrt = 1;
      }
      puthval(&outtrace,vindex,&kval);
      puttr(&outtrace);
    }
    nx = get_ens(nxmax,headers,traces,&gottrace,pindex,ptype,ns);
  }

  return(EXIT_SUCCESS);

}

int get_ens(int nxmax, int **headers, float **traces, int *gottrace,
            int index, cwp_String ptype, int ns)
{

  int i;
  int end_ens;
  int nx=0;

  Value kval;

  end_ens = 0;

  for(i=0; i<nxmax; i++) {
    if(!*gottrace || end_ens)
      break;
    /* First trace is already in intrace? */
    if(havetrace) {
      havetrace = 0;
      memcpy((void *)headers[i],(const void *)&intrace,HDRBYTES);
      memcpy((void *)traces[i],(const void *)intrace.data,ns*sizeof(float));
      gethval(&intrace,index,&kval);
      val = vtoi(ptype,kval);
      lastval = val;       
      nx = 1;
      continue;
    }
    if(gettr(&intrace)) {
      gethval(&intrace,index,&kval);
      val = vtoi(ptype,kval);
      if(i == 0)
        lastval = val;
      if(val == lastval) {
        memcpy((void *)headers[i],(const void *)&intrace,HDRBYTES);
        memcpy((void *)traces[i],(const void *)intrace.data,ns*sizeof(float));
        nx++;
      }
      else {
        end_ens = 1;
        havetrace = 1;
        break;
      }
    }
    else {
      if(nx == 0)
        *gottrace = 0;
      break;
    }
  }

  return nx;

}
