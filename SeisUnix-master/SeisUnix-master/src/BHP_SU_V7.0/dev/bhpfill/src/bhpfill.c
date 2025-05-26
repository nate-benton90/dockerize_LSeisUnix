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
#include <math.h>
#include "bhpio_api.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 					                        ",
" BHPFILL - Fill Holes in BHPIO Cube Dataset                    ",
" 					                        ",
" bhpfill fills missing traces in a BHPIO cube dataset by       ",
" borrowing traces to fill in each empty slot. As an alternative,",
" traces with all zeros may be written to empty slots.          ",
" To fill each missing trace, multiple traces may be mixed via  ",
" inverse-distance weighting, or a single nearest trace can be used",
" The type of search done to locate traces to be mixed can be   ",
" a nearest-trace search, or a 'circular' nearest trace search. ",
" In a circular search, the iline,xline of the current output location",
" is used as the center point of a grid, in which each quadrant of the",
" grid is bounded by the iline and xline. The nearest trace in each",
" quadrant is used in a 4-trace mix.                            ",
" 					                        ",
" bhpfill filename=                                             ",
" 					                        ",
" Required Parameters: filename=name                            ",
"                      name of BHPIO cube to be filled          ",
" 					                        ",
" Optional Parameters:			                        ",
"  pathlist='name'.dat      File containing filename partitions ",
"  verbose=0                Use verbose=1 for long printout     ",
"  foldfile='name'_fold.asc Name of file output from foldfile option",
"                           of bhpreadcube. foldfile is used to locate",
"                           occupied bins in the data           ",
"  mode=mix                 Normal operating mode is to build   ",
"                           output traces by mixing input traces.",
"                           If mode=zero, bhpfill simply writes ",
"                           traces containing valid headers and ",
"                           all zeros for samples to each empty ",
"                           cube slot.                          ",
"  nmix=1                   Number of traces to mix for each output",
"                           location; default is to just get nearest",
"                           trace                               ",
"  search=circle            Circle means use the nearest trace in",
"                           each quadrant surrounding each output",
"                           location.                           ",
"                                                               ",
NULL};

/**************** end self doc ********************************/

/* internal prototypes */
int get_trace_locs(char *search, int nl, int nmix, int *phdr, int *shdr,
                   int *keyvals, int **vals, float *dist, int *idist,
                   float *mind, int *il, int *xl, int verbose);
void trace_mix(int nmix, float *dist, segy **traces, segy *trace, int verbose);
float get_dist(int k1, int k2, int p1, int p2);

int main (int argc, char **argv)
{

  char *filename;            /* filename */
  char *pathlist;            /* pathlist */
  char *foldfile;            /* fold info */
  char *search;              /* type of search */
  char *mode;                /* mix or zero */
  char record[256];

  FILE *fp;                  /* fold file pointer */

  float *dist;               /* distances from output to all traces */
  float *mind;               /* minimum distance in each quadrant */

  int verbose;               /* Debug print */
  int i, j, k;
  int *keyvals;              /* bhpio header key values */
  int **vals;                /* keys for reading traces from foldfile */
  int trcount;               /* number of traces returned by read_bhpio_trace */
  int *key_index;            /* key indices returned by open_bhpio_dataset */
  int nkeys;                 /* nkeys returned by open_bhpio_dataset */
  int *min, *max, *incr;     /* header limits */
  int ntraces;               /* total number of traces as per header limits */
  int *num;                  /* number of occurences of each key */
  int *next;                 /* next expected header value for each key */
  int nr;                    /* number of records in foldfile */
  int nl;                    /* number of "live" bins form foldfile */
  int **fvals;               /* value of keys from foldfile */
  int nmix;                  /* number of traces to mix */
  int *idist;                /* indices for sort */
  int match;                 /* =1 if exact match found */
  int nfound;                /* number of locs returned by get_trace_locs */
  int *il, *xl;              /* for saving quadrant headers */

  Value hval;                /* header value */
                                
  segy trace;                /* trace returned from bhpio */
  segy **traces;             /* traces to mix, if nmix > 1 */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* filename, path */
  if(!getparstring("filename",&filename))
    err("filename is required\n");
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc((int)strlen(filename)+5,sizeof(char));
    strcpy(pathlist,filename);
    strcat(pathlist,".dat");
  }
 
  /* foldfile */
  if(!getparstring("foldfile",&foldfile)) {
    foldfile = calloc((int)strlen(filename)+10,sizeof(char));
    strcpy(foldfile,filename);
    strcat(foldfile,"_fold.asc");
  }

  /* mode=mix or mode=zero */
  if(!getparstring("mode",&mode))
    mode = "mix";

  /* number to mix, default is 1 */
  if(!getparint("nmix",&nmix))
    nmix = 1;

  /* type of search */
  if(!getparstring("search",&search))
    search = "circle";

  /* verify */
  if(nmix < 1)
    err("nmix must be >= 1\n");
  if(strcmp(search,"near") && strcmp(search,"circle"))
    err("search must be near or circle\n");

  /* if search is circle, force nmix to 4 */
  if(!strcmp(search,"circle"))
    nmix = 4;

  /* open dataset */
  key_index = open_bhpio_dataset(filename,pathlist,&nkeys,verbose);

  /* if mode=mix, dataset must have exactly 2 keys */
  if(!strcmp(mode,"mix") && nkeys != 2)
    err("Exactly 2 keys are required to compute distances for trace mixing\n");

  /* traces to mix if nmix > 1 */
  if(nmix > 1) {
    traces = calloc(nmix,sizeof(float *));
    for(i=0; i<nmix; i++)
      traces[i] = calloc(NSAMP+(HDRBYTES/4),sizeof(float));
  }
  mind = calloc(nmix,sizeof(float));
  vals = calloc(nmix,sizeof(int *));
  for(i=0; i<nmix; i++)
    vals[i] = calloc(nkeys,sizeof(int));

  if(verbose) {
    fprintf(stderr,"Filename is %s\n",filename);
    fprintf(stderr,"Pathlist is %s\n",pathlist);
    fprintf(stderr,"Foldfile is %s\n",foldfile);
    if(!strcmp(mode,"mix")) {
      fprintf(stderr,"Mixing %d traces\n",nmix);
      fprintf(stderr,"Search type is %s\n",search);
    }
    else
      fprintf(stderr,"Filling empty slots with traces containing all zeroes\n");
  }
  if(strcmp(mode,"mix") && strcmp(mode,"zero"))
    err("mode must be mix or zero\n");

  set_bhpio_binning_rule("match",verbose);
  set_bhpio_interp_flag("no",verbose);
  
  /* key indices, limits, etc */
  keyvals = calloc(nkeys,sizeof(int));
  min = calloc(nkeys,sizeof(int));
  max = calloc(nkeys,sizeof(int));
  incr = calloc(nkeys,sizeof(int));
  num = calloc(nkeys,sizeof(int));
  next = calloc(nkeys,sizeof(int));

  /* header limits */
  if((i = get_bhpio_header_limits(min,max,incr,verbose)) != 0)
    err("Error getting limits\n");

  /* fill in num */
  for(i=0; i<nkeys; i++)
    num[i] = (max[i] - min[i] + incr[i]) / incr[i];
  /* ntraces is product of nums */
  ntraces = num[0];
  for(i=1; i<nkeys; i++)
    ntraces *= num[i];

  if(verbose) {
    fprintf(stderr,"Header Limits: \n");
    for(i=0; i<nkeys; i++)
      fprintf(stderr,"MIN: %d, MAX: %d, INCR: %d, NUM: %d\n",min[i],max[i],incr[i],num[i]);
    fprintf(stderr,"Reading/writing %d total traces\n",ntraces);
  }

  /* load fold info */
  fp = fopen(foldfile,"r");
  if(fp == NULL)
    err("Unable to open %s\n",foldfile);
  /* count lines */
  nr = 0;
  nl = 0;
  while(fgets(record,255,fp) != NULL)
    nr++;
  if(verbose)
    fprintf(stderr,"Counted %d records in %s\n",nr,foldfile);
  rewind(fp);

  /* alloc space for foldfile keys */
  fvals = calloc(nkeys,sizeof(int *));
  for(i=0; i<nkeys; i++)
    fvals[i] = calloc(nr,sizeof(int));
  dist = calloc(nr,sizeof(float));
  idist = calloc(nr,sizeof(int));
  il = calloc(nr,sizeof(int));
  xl = calloc(nr,sizeof(int));
  for(i=0; i<nr; i++) {
    fgets(record,255,fp);
    fvals[0][i] = atoi(strtok(record," "));
    for(j=1; j<nkeys; j++)
      fvals[j][i] = atoi(strtok(NULL," "));
    k = atoi(strtok(NULL," "));
    if(k != 0)
      nl++;
  }
  if(verbose)
    fprintf(stderr,"%d bins out of %d contain a trace\n",nl,nr);
  if(nl != nr)
    err("Error loading foldfile\n");

  /* if mode=zero, get a trace with a good header */
  if(!strcmp(mode,"zero")) {
    for(i=0; i<nkeys; i++)
      keyvals[i] = fvals[i][0];
    trcount = read_bhpio_trace(keyvals,&trace,verbose);
    if(trcount != 1) {
      fprintf(stderr,"Could not read keys=");
      for(i=0; i<nkeys; i++)
        fprintf(stderr," %d ",fvals[i][0]);
      fprintf(stderr,"\n");
      err("Error reading first trace in foldfile\n");
    }
    for(i=0; i<file_hdr.nsamp; i++)
      trace.data[i] = 0;
  }
  
  /* initialize keys */
  for(i=0; i<nkeys; i++)
    next[i] = min[i];
  /* Loop over ntraces */
  for(i=0; i<ntraces; i++) {
    for(j=0; j<nkeys; j++)
      keyvals[j] = next[j];
    trcount = 0;
    /* mode=mix */
    if(!strcmp(mode,"mix")) {
      nfound = get_trace_locs(search,nl,nmix,fvals[0],fvals[1],keyvals,vals,dist,idist,mind,il,xl,verbose);
      if(nmix > 1) {
        for(j=0; j<nfound; j++) {
          match = 0;
          trcount = read_bhpio_trace(vals[j],traces[j],verbose);
          if(trcount == 1 && vals[j][0] == keyvals[0] && vals[j][1] == keyvals[1]) {
            memcpy((void *)&trace,(const void *)traces[j],HDRBYTES+(NSAMP*sizeof(float)));
            match = 1;
            fprintf(stderr,"Found match for %d,%d\n",keyvals[0],keyvals[1]);
            break;
          }
          else if(trcount != 1)
            err("No trace returned for %d,%d\n",vals[j][0],vals[j][1]);
        }
        if(match == 0) {
          fprintf(stderr,"Mixing %d traces for %d,%d: ",nfound,keyvals[0],keyvals[1]);
          for(j=0; j<nfound; j++)
            fprintf(stderr," %d,%d ",vals[j][0],vals[j][1]);
          fprintf(stderr,"\n");
          if(nfound > 0) {
            trace_mix(nfound,mind,traces,&trace,verbose);
            trcount = 1;
          }
        }
      }
      else {
        trcount = read_bhpio_trace(vals[0],&trace,verbose);
        if(trcount == 1) {
          if(vals[0][0] == keyvals[0] && vals[0][1] == keyvals[1]) 
            fprintf(stderr,"Found match for %d,%d\n",keyvals[0],keyvals[1]);
          else
            fprintf(stderr,"Found nearest trace for %d,%d at %d,%d\n",keyvals[0],keyvals[1],vals[0][0],vals[0][1]);
        }
        else if(trcount == 0)
          err("No trace returned for %d,%d\n",vals[0],vals[1]);
        else if(trcount > 1)
          err("trcount > 1\n");
      }
      /* update keys in header */
      if(trcount == 1) {
        hval.i = keyvals[0];
        puthval(&trace,key_index[0],&hval);
        hval.i = keyvals[1];
        puthval(&trace,key_index[1],&hval);
        puttr(&trace);
      }
    }
    /* mode=zero */
    else {
      trcount = read_bhpio_trace(keyvals,traces[0],verbose);
      if(trcount == 0) {
        for(j=0; j<nkeys; j++) {
          hval.i = keyvals[j];
          puthval(&trace,key_index[j],&hval);
        }
        puttr(&trace);
      }
      else {
        puttr(traces[0]);
      }
    }
    for(j=nkeys-1; j>=0; j--) {
      next[j] += incr[j];
      if(next[j] > max[j])
        next[j] = min[j];
      else
        break;
    }
  }

  close_bhpio_dataset();

  return EXIT_SUCCESS;

}

int get_trace_locs(char *search, int nl, int nmix, int *phdr, int *shdr, int *keyvals,
                   int **vals, float *dist, int *idist, float *mind, int *il, int *xl,
                   int verbose)
{

  int i, q;
  int nf;
  int nt;

  /* return locations depending on search type */
  if(!strcmp(search,"near")) {
    if(nl < nmix)
      err("Not enough data in foldfile\n");
    nf = nmix;
    /* distance from output to each occupied bin */
    for(i=0; i<nl; i++)
      dist[i] = sqrt(((phdr[i] - keyvals[0]) * (phdr[i] - keyvals[0])) +
                     ((shdr[i] - keyvals[1]) * (shdr[i] - keyvals[1])));
    /* sort the distances */
    for(i=0; i<nl; i++)
      idist[i] = i;
    qkisort(nl,dist,idist);
    for(i=0; i<nmix; i++) {
      mind[i] = dist[idist[i]];
      vals[i][0] = phdr[idist[i]];
      vals[i][1] = shdr[idist[i]];
    }
  }
  else if(!strcmp(search,"circle")) {
    nf = 0;
    /* find nearest trace in each quadrant */
    for(q=1; q<=4; q++) {
      nt = 0;
      /* Q1: il increasing, xl increasing */
      if(q == 1) {
        /* find all live bins whose phdr>= keyval[0] and shdr >= keyval[1] */
        for(i=0; i<nl; i++) {
          if(phdr[i] >= keyvals[0] && shdr[i] >= keyvals[1]) {
            il[nt] = phdr[i];
            xl[nt] = shdr[i];
            nt++;
          }
        }
        /* save nearest */
        if(nt > 0) {
          for(i=0; i<nt; i++)
            dist[i] = get_dist(keyvals[0],keyvals[1],il[i],xl[i]);
          for(i=0; i<nt; i++)
            idist[i] = i;
          qkisort(nt,dist,idist);
          vals[nf][0] = il[idist[0]];
          vals[nf][1] = xl[idist[0]];
          mind[nf] = dist[idist[0]];
          nf++;
        }
      }
      nt = 0;
      /* Q2: il increasing, xl decreasing */
      if(q == 2) {
        /* find all live bins whose phdr>= keyval[0] and shdr < keyval[1] */
        for(i=0; i<nl; i++) {
          if(phdr[i] >= keyvals[0] && shdr[i] < keyvals[1]) {
            il[nt] = phdr[i];
            xl[nt] = shdr[i];
            nt++;
          }
        }
        /* save nearest */
        if(nt > 0) {
          for(i=0; i<nt; i++)
            dist[i] = get_dist(keyvals[0],keyvals[1],il[i],xl[i]);
          for(i=0; i<nt; i++)
            idist[i] = i;
          qkisort(nt,dist,idist);
          vals[nf][0] = il[idist[0]];
          vals[nf][1] = xl[idist[0]];
          mind[nf] = dist[idist[0]];
          nf++;
        }
      }
      nt = 0;
      /* Q3: il decreasing, xl decreasing */
      if(q == 3) {
        /* find all live bins whose phdr< keyval[0] and shdr <= keyval[1] */
        for(i=0; i<nl; i++) {
          if(phdr[i] < keyvals[0] && shdr[i] <= keyvals[1]) {
            il[nt] = phdr[i];
            xl[nt] = shdr[i];
            nt++;
          }
        }
        /* save nearest */
        if(nt > 0) {
          for(i=0; i<nt; i++)
            dist[i] = get_dist(keyvals[0],keyvals[1],il[i],xl[i]);
          for(i=0; i<nt; i++)
            idist[i] = i;
          qkisort(nt,dist,idist);
          vals[nf][0] = il[idist[0]];
          vals[nf][1] = xl[idist[0]];
          mind[nf] = dist[idist[0]];
          nf++;
        }
      }
      nt = 0;
      /* Q4: il decreasing, xl increasing */
      if(q == 4) {
        /* find all live bins whose phdr< keyval[0] and shdr > keyval[1] */
        for(i=0; i<nl; i++) {
          if(phdr[i] < keyvals[0] && shdr[i] > keyvals[1]) {
            il[nt] = phdr[i];
            xl[nt] = shdr[i];
            nt++;
          }
        }
        /* save nearest */
        if(nt > 0) {
          for(i=0; i<nt; i++)
            dist[i] = get_dist(keyvals[0],keyvals[1],il[i],xl[i]);
          for(i=0; i<nt; i++)
            idist[i] = i;
          qkisort(nt,dist,idist);
          vals[nf][0] = il[idist[0]];
          vals[nf][1] = xl[idist[0]];
          mind[nf] = dist[idist[0]];
          nf++;
        }
      }
    }
  }

  return nf;
    
}

void trace_mix(int nmix, float *dist, segy **traces, segy *trace, int verbose)
{

  int i, j, nsamp;
  
  float sum;
  float invsum;

  memcpy((void *)trace,(const void *)traces[0],HDRBYTES);
  nsamp = (int)traces[0]->ns;
  for(i=0; i<nsamp; i++)
    trace->data[i] = 0;

  sum = 0.0;
  for(i=0; i<nmix; i++) {
    dist[i] = 1.0 / dist[i]; 
    sum += dist[i];
  }
  invsum = 1.0 / sum;
  for(i=0; i<nmix; i++)
    dist[i] *= invsum;
  
  for(i=0; i<nsamp; i++) {
    for(j=0; j<nmix; j++)
      trace->data[i] += dist[j] * traces[j]->data[i];
  }

  /* look for NaNs */
  for(i=0; i<nsamp; i++) {
    if(isnanf(trace->data[i])) {
      fprintf(stderr,"NaN at sample %d\n",i);
      fprintf(stderr,"Weights: ");
      for(j=0; j<nmix; j++)
        fprintf(stderr,"%f ",dist[j]);
      fprintf(stderr,"\n");
      err("NaN\n");
    }
  }
}

float get_dist(int k1, int k2, int p1, int p2)
{

  float d;

  d = sqrt(((p1 - k1) * (p1 - k1)) +
           ((p2 - k2) * (p2 - k2)));

  return d;

}
