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
#include "bhpio_api.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" BHPMAKEEVENT -  Build NULL Event Trace                                ",
" 									",
" bhpmakeevent [optional parameters]                                    ",
" 									",
" bhpmakeevent makes BHPIO event data consisting of all NULL events,    ",
" or adds NULL events to an existing BHPIO event dataset.               ",
" 									",
" Required parameters: none                                             ",
" 									",
" Optional parameters:						        ",
" filename=stack     Name of existing BHPIO cross-section cube dataset  ",
" pathlist=stack.dat File containing filename partitions                ",
"                    The output dataset is created with bhpwritecube,   ",
"                    and is automatically named 'filename_events'.      ",
"                    A pathlist file is created in the same directory   ",
"                    as pathlist.                                       ",
" endian=2           Use native endianness for output data. Specify     ",
"                    endian=0 to write LITTLE, or endian=1 to write BIG,",
"                    regardless of the platform on which bhpmakeevent is",
"                    running.                                           ",
" type=new           Replace samples of input data with all NULLs.      ",
"                    Use type=append to add NULLs to existing input data.",
"                    If type=new, input data must be in cross-section   ",
"                    order, but may be of any type: seismic, model, or  ",
"                    event. If type=append, input data must be event data",
"                    in cross-section order.                            ",
" ename=null         Partial name to use for new events. Actual names will",
"                    be 'ename'1, 'ename'2,...                          ",
" num=20             Number of NULL events to create or append.         ",
" null=-999.25       Value to use for null                              ",
" verbose=0          Use verbose=1 for long printout                    ",
"                                                                       ",
NULL}; 

/* Prototypes */
void init_data(segy *tr, int npicks);

int main(int argc, char **argv)
{

  char *filename;            /* input, output filenames */
  char *pathlist;            /* input pathlist */
  char *pathlist_events;     /* output pathlist */
  char *type;                /* new or append */
  char cmdbuf[BUFSIZ];       /* UNIX commands */
  char string1[256];         /* scratch */
  char string2[256];         /* scratch */
  char **name;               /* key names */
  char *ename;               /* root event name */

  float null;                /* null value, default=-999.25 */

#ifdef CWP_BIG_ENDIAN
  int my_endian = 1;
#else
  int my_endian = 0;
#endif
  int verbose;               /* Debug print */
  int i, j;
  int *keyvals;              /* bhpio header key values */
  int trcount;               /* number of traces returned by read_bhpio_trace */
  int *key_index;            /* key indices returned by open_bhpio_dataset */
  int nkeys;                 /* nkeys returned by open_bhpio_dataset */
  int *min, *max, *incr;     /* header limits */
  int *nbins;                /* number of bins per key */
  int ntraces;               /* total number of traces as per header limits */
  int num;                   /* number of events to create, default=20 */
  int nevents=0;             /* number of existing events if appending */
  int *next;                 /* next expected header value for each key */
  int endian;                /* endian parm for bhpwritecube 1=BIG,0=LITTLE,2=Native */

  Value hval;                /* header value */
                                
  segy trace;                /* trace returned from bhpio */

  FILE *pipefp;              /* UNIX commands */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* filename, path */
  if(!getparstring("filename",&filename))
    filename="stack";
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc((int)strlen(filename)+5,sizeof(char));
    strcpy(pathlist,filename);
    strcat(pathlist,".dat");
  }
 
  /* endian */
  if(!getparint("endian",&endian))
    endian = my_endian;

  /* type */
  if(!getparstring("type",&type))
    type = "new";

  /* number of events to create/append */
  if(!getparint("num",&num))
    num = 20;

  /* base name for events */
  if(!getparstring("ename",&ename))
    ename = "null";

  /* null value */
  if(!getparfloat("null",&null))
    null = -999.25;

  /* print parms */
  if(verbose) {
    fprintf(stderr,"FILENAME: %s, PATHLIST: %s\n",filename,pathlist);
    fprintf(stderr,"TYPE: %s, NUM: %d, ENAME: %s, NULL: %7.2f\n",type,num,ename,null);
    if(endian == 2)
      fprintf(stderr,"ENDIANNESS: Native\n");
    else if(endian == 1)
      fprintf(stderr,"ENDIANNESS: Big\n");
    else if(endian == 0)
      fprintf(stderr,"ENDIANNESS: Little\n");
  }
  if(endian == 2)
    endian = my_endian;
  /* verify */
  if(endian < 0 || endian > 2)
    err("%d is illegal endian value, use 0(LITTLE), 1(BIG), or 2(Native)\n",endian);
  if(strcmp(type,"new") && strcmp(type,"append"))
    err("type must be new or append\n");

  /* open dataset */
  key_index = open_bhpio_dataset(filename,pathlist,&nkeys,verbose);

  set_bhpio_binning_rule("match",verbose);
  set_bhpio_interp_flag("no",verbose);
  
  /* verify input is cross-section */
  if(file_hdr.data_order > 1)
    err("Input data must be cross-section\n");

  /* if appending, verify input has horizons */
  if(!strcmp(type,"append") && file_hdr.nprop >= 0)
    err("When type=append, input must be event dataset\n");

  /* if appending, get existing events list */
  if(!strcmp(type,"append")) {
    nevents = -file_hdr.nprop;
    if(verbose) {
      fprintf(stderr,"Adding events to existing list:\n");
      for(i=0; i<-file_hdr.nprop; i++)
        fprintf(stderr," %s ",properties[i]);
      fprintf(stderr,"\n");
    }
  }

  /* key indices, limits, etc */
  keyvals = calloc(nkeys,sizeof(int));
  min = calloc(nkeys,sizeof(int));
  max = calloc(nkeys,sizeof(int));
  incr = calloc(nkeys,sizeof(int));
  nbins = calloc(nkeys,sizeof(int));
  next = calloc(nkeys,sizeof(int));
  name = calloc(nkeys,sizeof(char *));
  for(i=0; i<nkeys; i++)
    name[i] = calloc(8,sizeof(char));

  /* header limits */
  if((i = get_bhpio_header_limits(min,max,incr,verbose)) != 0)
    err("Error getting limits\n");
  /* key names */
  for(i=0; i<nkeys; i++)
    name[i] = getkey(key_index[i]);

  /* fill in ntraces */
  nbins[0] = (max[0] - min[0] + incr[0]) / incr[0];
  ntraces = nbins[0];
  for(i=1; i<nkeys; i++) {
    nbins[i] = (max[i] - min[i] + incr[i]) / incr[i];
    ntraces *= nbins[i];
  }

  if(verbose) {
    fprintf(stderr,"Header Limits: \n");
    for(i=0; i<nkeys; i++)
      fprintf(stderr,"NAME: %s, MIN: %d, MAX: %d, INCR: %d\n",name[i],min[i],max[i],incr[i]);
    fprintf(stderr,"Reading/writing %d total traces\n",ntraces);
  }

  /* make pathlist file */
  pathlist_events = calloc((int)strlen(filename)+12,sizeof(char));
  strcpy(pathlist_events,filename);
  strcat(pathlist_events,"_events");
  strcat(pathlist_events,".dat");
  sprintf(cmdbuf,"cp %s %s\n",pathlist,pathlist_events);
  if(verbose)
    fprintf(stderr,"Executing %s\n",cmdbuf);
  pipefp = epopen(cmdbuf,"w");
  epclose(pipefp);
  /* establish pipe to bhpwritecube */
  strcat(filename,"_events");
  sprintf(cmdbuf,"bhpwritecube filename=%s pathlist=%s init=yes endian=%d verbose=%d ",
         filename,pathlist_events,endian,verbose);
  for(i=0; i<nkeys; i++) {
    sprintf(string1,"key%1d=%s,%d,%d,%d ",i+1,name[i],min[i],incr[i],nbins[i]);
    strcat(cmdbuf,string1);
  }
  strcat(cmdbuf,"horizons=");
  /* existing events */
  for(i=0; i<nevents; i++) {
    sprintf(string1,"%s,",properties[i]);
    strcat(cmdbuf,string1);
  } 
  /* new events: 'ename'1,'ename'2,... */
  sprintf(string1,"%s",ename);
  for(i=0; i<num-1; i++) {
    sprintf(string2,"%1d,",i+1);
    strcat(cmdbuf,string1);
    strcat(cmdbuf,string2);
  } 
  sprintf(string2,"%1d",num);
  strcat(cmdbuf,string1);
  strcat(cmdbuf,string2);
  if(verbose)
    fprintf(stderr,"Executing %s\n",cmdbuf);
  pipefp = epopen(cmdbuf,"w");
  /* initialize keys */
  for(i=0; i<nkeys; i++)
    next[i] = min[i];
  /* Loop over ntraces */
  for(i=0; i<ntraces; i++) {
    for(j=0; j<nkeys; j++)
      keyvals[j] = next[j];
    trcount = read_bhpio_trace(keyvals,&trace,verbose);
    if(trcount != 1) {
      fprintf(stderr,"No trace returned for ");
      for(j=0; j<nkeys; j++)
        fprintf(stderr," %d ",keyvals[j]);
      fprintf(stderr,"\n");
      err("Trace not found\n");
    }
    /* create or append nulls */
    if(!strcmp(type,"new")) {
      trace.ns = (unsigned short)num;
      for(j=0; j<num; j++)
        trace.data[j] = null;
    }
    else {
      for(j=trace.ns; j<trace.ns+num; j++)
        trace.data[j] = null;
      trace.ns += num;
    }
    /* update keys in header */
    for(j=0; j<nkeys; j++) {
      hval.i = keyvals[j];
      puthval(&trace,key_index[j],&hval);
    }
    fputtr(pipefp,&trace);
    for(j=nkeys-1; j>=0; j--) {
      next[j] += incr[j];
      if(next[j] > max[j])
        next[j] = min[j];
      else
        break;
    }
  }

  close_bhpio_dataset();
  epclose(pipefp);

  return EXIT_SUCCESS;

}
