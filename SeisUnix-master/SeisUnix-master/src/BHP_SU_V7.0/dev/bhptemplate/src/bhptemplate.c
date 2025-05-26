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
* KEYWORDS:  $RCSfile: bhptemplate.c,v $
*            $Revision: 1.1 $
*            $Date: 2001/02/07 19:55:46 $
*            $Author: ahglim $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhptemplate.c,v $
* Revision 1.1  2001/02/07 19:55:46  ahglim
* added bhproffread, bhptemplate
* changed bhpio, bhpread, bhpwrite to handle ENDIAN issues
*
*
*
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "su.h"
#include "segy.h"
#include "header.h"


/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
" bhptemplate key=fldr ntmax=all nxmax=350 ngath=1 verbose=0       ",
"                                                                  ",
" BHPTEMPLATE is general-purpose SU source for getting panels      ",
"   of traces for user algorithm processing. Insert your source    ",
"   code between the two lines labelled                            ",
"                                                                  ",
" /* Insert processing code here */                                ",
"                                                                  ",
" Required Parameters: none                                        ",
"                                                                  ",
" Optional Parameters:                                             ",
"   key=fldr          Ensemble trace header key                    ",
"   ntmax=all         Number of samples per trace to get           ",
"   nxmax=350         Maximum number of traces per ensemble        ",
"                     NXMAX must be large enough to handle any     ",
"                      auxiliary traces, i.e. trid != 1            ",
"   ngath=1           Number of ensembles per panel to get         ",
"                                                                  ",
"                                                                  ",
" The trace processing loop delivers each trace as:                ",
"  Gather i,                                                       ",
"  Trace  j,                                                       ",
"  Trid   k                                                        ",
"                                                                  ",
"                                                                  ",
" You can copy the source code for bhptemplate from                ",
"  /hou/apps/gttgeo/ahmilb/src/bhptemplate/bhptemplate.c           ",
"                                                                  ",
"                                                                  ",
NULL};

/* Globals */
segy intrace;      /* Input trace */
int havetrace=0;   /* Flag for trace already in intrace */
#define NTEMP 10   /* Number of temps for counting trids */
int lastval;       /* Key value, previous trace */
int val;           /* Key value, current trace */

/* Prototypes */
void get_ens(int i, int start, int nxmax, int ntrid, int ****headers, float ****traces,
             int *nx, int *gottrace, int index, cwp_String type, int nt);

int main(int argc, char **argv)
{

  /* Command-line arguments */
  int ntmax=0;        /* Max samples per trace; default=all */
  int nxmax=350;      /* Max traces per ensemble; default=350 */
  int ngath=1;        /* Max ensembles per buffer; default=1 */
  cwp_String key;     /* Ensemble key */
  int verbose=0;      /* For printing info */

  /* Results, used to control processing loop */
  int ntrid=0;        /* Number of trid's per ensemble */
  int ng=0;           /* Number of ensembles */
  int *nx;            /* Number of traces per ensemble */ 
  int nt=0;           /* Number of samples per trace */ 
  int trid1;          /* First trid */

  /* Ensemble key info */
  cwp_String type;    /* Key type */
  int index;          /* Key index */
  Value kval;         /* Key value */
  int gottrace=1;     /* =0 if EOF already encountered */

  /* data buffers */
  int ****headers;    /* Ensemble, trace, trid, sample */
  float ****traces;   /* Ensemble, trace, trid, sample */
  segy outtrace;      /* Output trace */
  int **headt;        /* Temp headers */
  float **tracet;     /* Temp traces */

  /* Loop counters */
  int i, j, k, m;

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* Get args*/
  if(getparint("ntmax",&ntmax))
    if(ntmax <= 0)
      err("ntmax must be greater than zero\n");
  if(getparint("nxmax",&nxmax))
    if(nxmax <= 0)
      err("nxmax must be greater than zero\n");
  if(getparint("ngath",&ngath))
    if(ngath <= 0)
      err("ngath must be greater than zero\n");
  getparint("verbose",&verbose);
  if(!getparstring("key",&key))
    key = "fldr";

  /* Key info */
  type = hdtype(key);
  index = getindex(key);

  /* Print info */
  if(verbose) {
    if(ntmax)
      fprintf(stderr,"Getting up to %d samples per trace\n", ntmax);
    else
      fprintf(stderr,"Getting all samples per trace\n");
    fprintf(stderr,"Getting up to %d traces per ensemble\n", nxmax);
    if(ngath)
      fprintf(stderr,"Getting %d ensembles per buffer\n", ngath);
    else
      fprintf(stderr,"Getting 1 ensemble per buffer\n");
    fprintf(stderr,"Ensemble key = %s\n", key);
  }

  /* Set nt, ng */
  if(ntmax)
    nt = ntmax;
  if(ngath)
    ng = ngath;

  /* Allocate nx, number of traces per gather */
  nx = calloc(ng,sizeof(int));

  /* Get first trace, check nt */
  if (!gettr(&intrace))
    err("can't get first trace");
  if(!nt || nt > intrace.ns) {
    nt = intrace.ns;
    if(verbose)
      fprintf(stderr,"Set ns = %d\n", nt);
  }

  /* Save 1st trid */
  trid1 = intrace.trid;
  ntrid = 1;

  /* Key from first trace */
  gethval(&intrace,index,&kval);
  val = vtoi(type,kval);
  lastval = val;

  /* Allocate NTEMP traces to save data until number of trids is known */
  headt = calloc(NTEMP,sizeof(char **));
  tracet = calloc(NTEMP,sizeof(float **));
  for(i=0; i<NTEMP; i++) {
    headt[i] = calloc(HDRBYTES,sizeof(char));
    tracet[i] = calloc(nt,sizeof(float));
  }

  /* Put first trace and header in buffer, then count trids */
  memcpy((void *)headt[0],(const void *)&intrace,HDRBYTES);
  memcpy((void *)tracet[0],(const void *)intrace.data,nt*sizeof(float));
  for(i=1; i<NTEMP; i++) {
    if(gettr(&intrace)) {
      if(intrace.trid == trid1)
        break;
      else {
        memcpy((void *)headt[ntrid],(const void *)&intrace,HDRBYTES);
        memcpy((void *)tracet[ntrid],(const void *)intrace.data,nt*sizeof(float));
        ntrid++;
      }
    }
    else
      err("Less than 1 trid group in input, aborting\n");
  }

  /* If ntrid = NTEMP, problem */
  if(ntrid == NTEMP)
    err("%d different trids found in first %d traces, aborting\n", NTEMP,NTEMP);
    
  if(verbose)
    fprintf(stderr,"Found %d TRIDS\n", ntrid);

  /* Allocate headers, traces */
  headers = calloc(ng,sizeof(char ***));
  traces = calloc(ng,sizeof(float ***));
  for(i=0; i<ng; i++) {
    headers[i] = calloc(nxmax/ntrid,sizeof(char **));
    traces[i] = calloc(nxmax/ntrid,sizeof(float **));
    for(j=0; j<nxmax/ntrid; j++) {
      headers[i][j] = calloc(ntrid,sizeof(char *));
      traces[i][j] = calloc(ntrid,sizeof(float *));
      for(k=0; k<ntrid; k++) {
        headers[i][j][k] = calloc(HDRBYTES,sizeof(char));
        traces[i][j][k] = calloc(nt,sizeof(float));
      }
    }
  }
  if(verbose) {
    fprintf(stderr,"Allocated %d headers, %d bytes each\n", ng*nxmax,HDRBYTES);
    fprintf(stderr,"Allocated %d traces , %d samples each\n", ng*nxmax,nt);
  }

  /* Move first trid group into buffers */
  for(i=0; i<ntrid; i++) {
    memcpy((void *)headers[0][0][i],(const void *)headt[i],HDRBYTES);
    memcpy((void *)traces[0][0][i],(const void *)tracet[i],nt*sizeof(float));
  }
  /* Release temps */
  for(i=0; i<NTEMP; i++) {
    free(headt[i]);
    free(tracet[i]);
  }
  free(headt);
  free(tracet);

  /* Fill remaining traces in first ensemble, followed by subsequent ensembles */
  for(i=0; i<ng; i++) {
    if(i == 0) {
      j = 1;
      havetrace = 1;
    }
    else
      j = 0;
    if(gottrace) {
      get_ens(i,j,nxmax/ntrid,ntrid,headers,traces,nx,&gottrace,index,type,nt);
      fprintf(stderr,"Got ens %d, %d traces\n", i,nx[i]);
    }
  }

  /* Empty buffer, then re-fill until EOF */
  for(;;) {
    for(i=0; i<ng; i++) {
      for(j=0; j<nx[i]; j++) {
        for(k=0; k<ntrid; k++) {
          memcpy((void *)&outtrace,(const void *)headers[i][j][k],HDRBYTES);
          outtrace.ns = nt;
          for(m=0; m<nt; m++)
            outtrace.data[m] = traces[i][j][k][m];

          /* Insert processing code here - next trace is in outtrace */






          /* Insert processing code here */

          puttr(&outtrace);
        }
      }
    }
    /* At EOF? */
    if(!gottrace)
      break;
    /* Fill buffer */
    for(i=0; i<ng; i++) {
      nx[i] = 0;
      if(gottrace) {
        j = 0;
        get_ens(i,j,nxmax/ntrid,ntrid,headers,traces,nx,&gottrace,index,type,nt);
        fprintf(stderr,"Got ens %d, %d traces\n", i,nx[i]);
      }
    }
  }

  return(EXIT_SUCCESS);

}

void get_ens(int i, int start, int nxmax, int ntrid, int ****headers, float ****traces,
             int *nx, int *gottrace, int index, cwp_String type, int nt)
{

  int j, k;
  int end_ens;

  Value kval;

  end_ens = 0;
  nx[i] = start;
  for(j=start; j<nxmax; j++) {
    if(!*gottrace || end_ens)
      break;
    for(k=0; k<ntrid; k++) {
      /* First trace is already in intrace? */
      if(havetrace) {
        havetrace = 0;
        memcpy((void *)headers[i][j][k],(const void *)&intrace,HDRBYTES);
        memcpy((void *)traces[i][j][k],(const void *)intrace.data,nt*sizeof(float));
        gethval(&intrace,index,&kval);
        val = vtoi(type,kval);
        lastval = val;       
        continue;
      }
      if(gettr(&intrace)) {
        gethval(&intrace,index,&kval);
        val = vtoi(type,kval);
        if(j == start && k == 0)
          lastval = val;
        if(val == lastval) {
          memcpy((void *)headers[i][j][k],(const void *)&intrace,HDRBYTES);
          memcpy((void *)traces[i][j][k],(const void *)intrace.data,nt*sizeof(float));
        }
        else {
          end_ens = 1;
          havetrace = 1;
          break;
        }
      }
      else {
        *gottrace = 0;
        break;
      }
    }
    if(*gottrace && !end_ens)
      nx[i]++;
  }
}
