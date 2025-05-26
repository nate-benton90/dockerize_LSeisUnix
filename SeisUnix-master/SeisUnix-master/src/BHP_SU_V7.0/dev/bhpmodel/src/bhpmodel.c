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
*
* HISTORY:
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "bhpio_api.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" BHPMODEL - Make BHPIO Time and Depth Models                           ",
" 									",
" bhpmodel combines depths from a BHPIO horizon dataset with            ",
" velocity and other information from an ASCII model description        ",
" file to build BHPIO horizon-based time and depth model datasets.      ",
"                                                                       ",
" USAGE: bhpmodel mfile=                                                ",
" 									",
" Required Parameters:                                                  ",
"   mfile=             Location of ASCII model description (see below)  ",
" Optional Parameters:                                                  ",
"   null=-999.25            null value. The horizon data may contain    ",
"                           null values where no picks were made        ",
"   hfile=horizons          BHPIO dataset containing model depths       ",
"   tmodel=time_model       BHPIO output time model                     ",
"   dmodel=depth_model      BHPIO output depth model                    ",
"   pathlist=time_model.dat BHPIO pathlist file used to locate datasets ",
"   verbose=0               debug print                                 ",
"                                                                       ",
" Trace Header Usage:                                                   ",
"                                                                       ",
" ASCII Model Description Example:                                      ",
"shift event   layer   name     facies   N/G	vp	vs	rho	vp*rho",
"250	1	1    top_cover	2	65	2674	1150	2.20	5873",
"250	2	2    slope1	2	65	2674	1150	2.20	5873",
"250	3	3    drape1	4	15	2793	1217	2.29	6400",
"250	4	4    slope2	2	65	2674	1150	2.20	5873",
"250	5	5    slope3	2	45	2718	1174	2.23	6073",
"250	6	6    slope4	2	30	2754	1195	2.26	6232",
"250	7	7    channel1	3	100	2607	1113	2.13	5552",
"250	8	8    slope5	2	30	2754	1195	2.26	6232",
"250	9	9    slope6	2	65	2674	1150	2.20	5873",
"250	10	10   debri1	5	0	2977	1303	2.44	7252",
"250	11	11   slope7	2	65	2674	1150	2.20	5873",
"250	12	12   drape2	4	15	2793	1217	2.29	6400",
"250	13	13   background 1	3	2827	1236	2.31	6542",
"250	14	bottom                                                      ",
"        avgvel = 2760                                                      ",
"                                                                       ",
" Rules for Constructing Model Descriptions:                            ", 
"Column separators must be spaces or tabs.                              ",
"First non-blank line in file is assumed to be heading line.            ",
"Each column heading is limited to one word, e.g.                       ",
"time-shift or time_shift, or shift, but not time shift                 ",
"Columns may be in any order                                            ",
"                                                                       ",
"Certain columns are required:                                          ",
"  Keyword         Contents                                             ",
"  -------         --------                                             ",
"  shift           time-shift each layer                                ",
"  event           event number                                         ",
"  layer           layer number                                         ",
"  name            layer name                                           ",
"  vp              compressional velocity                               ",
"  rho             density                                              ",
"                                                                       ",
"Headings containing the above strings in any combination of upper and  ",
" lower case are required to construct model and synthetic datasets.    ",
"                                                                       ",
"Any other headings in the model file are used as property names and    ",
" their respective column contents are loaded into the time and depth models.",
"                                                                       ",
"A line with bottom anywhere on the line is bottom-model event          ",
"The bottom-model record maust have information in fixed columns as follows:",
" Column 1 = time-shift, Column 2 = event-number, Column 3 = bottom     ",
"                                                                       ",
"First non-blank line after bottom is parsed for:                       ",
"  string = value, and value is used as vp for top layer                ",
"                                                                       ",
NULL}; 

/* globals */
char record[256];       /* text record */
#define nreq 6          /* number of required columns: shift,event,layer,name,vp,rho */

/* prototypes */
char **get_heading(FILE *fp, int *nh, int verbose);

int main(int argc, char **argv)
{

  char *hfile;          /* depths file */
  char *mfile;          /* model description file */
  char **string;        /* strings for loading layer info */
  char *tmodel;         /* time model output */
  char *dmodel;         /* time model output */
  char **head;          /* list of headings */
  char **contents;      /* column contents description */
  char s1[32],s2[32];   /* used to scan for avgvel */
  char *pathlist;       /* output pathlist */
  char **keys;          /* keys */
  char cmdbuf[BUFSIZ];  /* UNIX commands */
  char string1[256];    /* scratch */
  char **onames;        /* other properties */

  int verbose;          /* Debug */
  int nlayers;          /* layer count from model info file */
  int nevents;          /* event count from model info file */
  int i, j, k, i1, i2;
  int i3, i4;
  int iz;               /* zero-based sample index of start of depths in model traces */
  int nprop;            /* number of properties in model */
  int first=1;          /* first trace flag */
  int nh;               /* number of headings found in get_heading */
  int ishift, iev, ilay;/* indices of column contents */
  int iname, ivp, irho; /* indices of column contents */
  int *key_index;       /* key indices returned by open_bhpio_dataset */
  int nkeys;            /* num keys in horizons, output */
  int *min, *max, *incr; /* key limits */
  int *nbins;           /* num bins each key */
  int *keyvals;         /* key values */
  int *next;            /* next keys to get */
  int ntraces;          /* num traces in hfile */
  int trcount;          /* trace count returned from read */
  int nother=0;         /* number of other properties */

  float null;           /* null event value */
  float mind, maxd;     /* min, max depths in model */
  float mint, maxt;     /* min, max times in model */
  float *savetr;        /* save last no-null input trace */
  float avgvel;         /* conversion velocity */
  float dthick, tthick; /* depth, time thicknesses */

  segy trace;           /* horizon input */
  segy time_trace;      /* time model output */
  segy depth_trace;     /* depth model output */

  FILE *infp;           /* model file */
  FILE *tfp;            /* time model file */
  FILE *dfp;            /* depth model file */

  typedef struct {      /* layer info structure */
    int shift;          /* time shift to apply */
    int evnum;          /* event number */
    int laynum;         /* layer number */
    char layname[16];   /* layer name */
    float vp;           /* velocity */
    float rho;          /* density */
    float *other;       /* all other properties */
  } layer_info;

  layer_info *layer_table;

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* debug option */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* null */
  if(!getparfloat("null",&null))
    null = -999.25;

  /* ascii layer info file */
  if(!getparstring("mfile",&mfile))
    err("Model Description File is required\n");

  /* horizons data */
  if(!getparstring("hfile",&hfile))
    hfile = "horizons";

  /* time model output */
  if(!getparstring("tmodel",&tmodel))
    tmodel = "time_model";

  /* depth model output */
  if(!getparstring("dmodel",&dmodel))
    dmodel = "depth_model";

  /* pathlist, use hfile.dat if unspecified */
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc((int)strlen(tmodel)+5,sizeof(char));
    strcpy(pathlist,tmodel);
    strcat(pathlist,".dat");
  }

  if(verbose) {
    fprintf(stderr,"Horizons File: %s\n",hfile);
    fprintf(stderr,"Model Description File: %s\n",mfile);
    fprintf(stderr,"Time Model File: %s\n",tmodel);
    fprintf(stderr,"Depth Model File: %s\n",dmodel);
    fprintf(stderr,"pathlist: %s\n",pathlist);
  }

  /* open model file */
  if((infp = fopen(mfile,"r")) == NULL)
    err("Cannot open %s\n",mfile);

  mind = mint = FLT_MAX;
  maxd = maxt = FLT_MIN;
  nlayers = 0;
  ishift = iev = ilay = iname = ivp = irho = -1;

  /* first record is heading */
  head = get_heading(infp,&nh,verbose);

  /* headings, descriptions, and layer contents */ 
  nother = nh - nreq;
  contents = calloc(nh,sizeof(char *));
  string = calloc(nh,sizeof(char *));
  onames = calloc(nother,sizeof(char *));
  for(i=0; i<nh; i++) {
    contents[i] = calloc(32,sizeof(char));
    string[i] = calloc(32,sizeof(char));
  }
  for(i=0; i<nother; i++)
    onames[i] = calloc(32,sizeof(char));

  /* parse heading for keywords, fill in contents description, indices */
  for(i=0,k=0; i<nh; i++) {
    j = (int)strlen(head[i]);
    for(i1=0; i1<j; i1++)
      head[i][i1] = tolower(head[i][i1]);
    if(strstr(head[i],"shift") != NULL) {
      contents[i] = "Time Shift Each Layer";
      ishift = i;
    }
    else if(strstr(head[i],"event") != NULL) {
      contents[i] = "Event Number";
      iev = i;
    }
    else if(strstr(head[i],"layer") != NULL && strstr(head[i],"name") == NULL) {
      contents[i] = "Layer Number";
      ilay = i;
    }
    else if(strstr(head[i],"name") != NULL) {
      contents[i] = "Layer Name";
      iname = i;
    }
    else if(strstr(head[i],"vp") != NULL && strstr(head[i],"rho") == NULL) {
      contents[i] = "Compressional Velocity";
      ivp = i;
    }
    else if(strstr(head[i],"rho") != NULL && strstr(head[i],"vp") == NULL) {
      contents[i] = "Density";
      irho = i;
    }
    /* other properties */
    else {
      strcpy(contents[i],head[i]);
      strcpy(onames[k],head[i]);
      k++;
    }
  }
  /* verify required headings found: shift, event, layer, name, vp, rho */
  if(ishift == -1)
    fprintf(stderr,"Time Shift Each Layer not found\n");
  if(iev == -1)
    fprintf(stderr,"Event Number not found\n");
  if(ilay == -1)
    fprintf(stderr,"Layer Number not found\n");
  if(iname == -1)
    fprintf(stderr,"Layer Name not found\n");
  if(ivp == -1)
    fprintf(stderr,"Compressional Velocity not found\n");
  if(irho == -1)
    fprintf(stderr,"Density not found\n");
  if(ishift == -1 || iev == -1 || ilay == -1 || iname == -1 || ivp == -1 || irho == -1)
     err("Check column headings\n");

  if(verbose) {
    fprintf(stderr,"Column Number Column Heading  Contents\n");
    fprintf(stderr,"------------- --------------  --------\n");
    for(i=0; i<nh; i++)
      fprintf(stderr,"  %2d       %16s    %s\n",i+1,head[i],contents[i]);
  }   

  /* count layers */
  while(fgets(record,512,infp) != NULL) {
    j = (int)strlen(record);
    for(i1=0; i1<j; i1++)
      string[0][i1] = tolower(record[i1]);
    if(strstr(string[0],"bottom") != NULL)
      break;
    /* skip blank records */
    if(!strcmp(record,""))
      continue;
    nlayers++;
  }
  if(verbose)
    fprintf(stderr,"%d layers in %s\n", nlayers,mfile);
  nevents = nlayers + 1;
  savetr = calloc(nevents,sizeof(float));
  rewind(infp);

  /* Load layer info */
  layer_table = calloc(nevents,sizeof(layer_info));
  /* total num prop is vp, rho + others */
  nprop = 2 + nother;
  /* allocate space in layer_table for other properties */
  for(i=0; i<nlayers; i++)
    layer_table[i].other = calloc(nother,sizeof(float));

  /* first non-blank is heading */
  while(fgets(record,512,infp) != NULL) {
    if(!strcmp(record,""))
      continue;
    break;
  }
  /* second non-blank is first event */
  while(fgets(record,512,infp) != NULL) {
    if(!strcmp(record,""))
      continue;
    break;
  }
  /* read events, skip blanks until bottom is found */
  i = 0;
  do {
    if(!strcmp(record,""))
      continue;
    if(verbose)
      fprintf(stderr,"%d chars in record\n",(int)strlen(record));
    for(j=0; j<(int)strlen(record); j++) {
      if(record[j] == '\t' || record[j] == '\n')
        record[j] = ' ';
    }
    if(i == nevents - 1) {
      sscanf(record,"%s %s %s",string[0],string[1],string[2]);
      layer_table[i].shift = atoi(string[0]);
      layer_table[i].evnum = atoi(string[1]);;
      layer_table[i].laynum = 0;
      strcpy(layer_table[i].layname,"bottom");
      layer_table[i].vp = 0;
      layer_table[i].rho = 0;
      break;
    }
    else {
      for(j=0,k=0; j<nh; j++) {
        if(j == 0)
          string[0] = strtok(record," ");
        else
          string[0] = strtok(NULL," ");
        if(j == ishift)
          layer_table[i].shift = atoi(string[0]);
        else if(j == iev)
          layer_table[i].evnum = atoi(string[0]);
        else if(j == ilay) {
          layer_table[i].laynum = atoi(string[0]);
          if(layer_table[i].laynum < 1 || layer_table[i].laynum > nlayers)
            err("Layer number %d is out of range\n",layer_table[i].laynum);
        }
        else if(j == iname)
          strcpy(layer_table[i].layname,string[0]);
        else if(j == ivp)
          layer_table[i].vp = atof(string[0]);
        else if(j == irho)
          layer_table[i].rho = atof(string[0]);
        else {
          layer_table[i].other[k] = atof(string[0]);
          k++;
        }
      }
    }
    i++;
  } while(fgets(record,512,infp) != NULL);
  /* look for 'name = value' to get vel for first layer */
  avgvel = -999;
  while(fgets(record,512,infp) != NULL) {
    if(strstr(record,"=") != NULL)
      i = sscanf(record,"%s %s %f",s1,s2,&avgvel);
  }
  fclose(infp);
  if(avgvel == -999.)
    err("Could not find avgvel = value\n");

  if(verbose) {
    fprintf(stderr,"Velocity for First Layer: %.0f\n",avgvel);
    fprintf(stderr,"Layer Info Table\n");
    fprintf(stderr,"Shift    Enum   Lnum            Lname    VP      RHO\n");
    for(i=0; i<nlayers+1; i++)
      fprintf(stderr,"%5d   %4d   %4d   %16s  %6.2f  %4.2f\n",
              layer_table[i].shift,layer_table[i].evnum,layer_table[i].laynum,
              layer_table[i].layname,layer_table[i].vp,layer_table[i].rho);
    if(nother > 0) {
      fprintf(stderr,"Other Properties\n");
      for(i=0; i<nother; i++)
        fprintf(stderr," %s ",onames[i]);
      fprintf(stderr,"\n");
      for(i=0; i<nlayers; i++) {
        for(j=0; j<nother; j++)
          fprintf(stderr," %f ",layer_table[i].other[j]);
        fprintf(stderr,"\n");
      }
    }
  }

  /* open hfile */
  key_index = open_bhpio_dataset(hfile,pathlist,&nkeys,verbose);
  if(verbose)
    fprintf(stderr,"opened %s\n",hfile);

  /* verify input is cross-section */
  if(file_hdr.data_order > 1)
    err("Input data must be cross-section\n");

  /* key indices, limits, etc */
  keyvals = calloc(nkeys,sizeof(int));
  min = calloc(nkeys,sizeof(int));
  max = calloc(nkeys,sizeof(int));
  incr = calloc(nkeys,sizeof(int));
  nbins = calloc(nkeys,sizeof(int));
  next = calloc(nkeys,sizeof(int));
  keys = calloc(nkeys,sizeof(char *));
  for(i=0; i<nkeys; i++)
    keys[i] = calloc(8,sizeof(char));

  /* header limits */
  if((i = get_bhpio_header_limits(min,max,incr,verbose)) != 0)
    err("Error getting limits\n");
  /* key names */
  for(i=0; i<nkeys; i++)
    keys[i] = getkey(key_index[i]);

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
      fprintf(stderr,"KEY: %s, MIN: %d, MAX: %d, INCR: %d\n",keys[i],min[i],max[i],incr[i]);
    fprintf(stderr,"Reading/writing %d total traces\n",ntraces);
  }

  /* set output indices, etc */
  iz = nprop * nlayers;
  if(verbose)
    fprintf(stderr,"IZ=%d, NS=%d\n",iz,nprop*nlayers+nlayers+1);

  /* initialize key values */
  for(i=0; i<nkeys; i++)
    next[i] = min[i];

  /* start bhpwritecube */
  sprintf(cmdbuf,"bhpwritecube filename=%s pathlist=%s init=yes verbose=%d ",
          tmodel,pathlist,verbose);
  for(i=0; i<nkeys; i++) {
    sprintf(string1,"key%1d=%s,%d,%d,%d ",i+1,keys[i],min[i],incr[i],nbins[i]);
    strcat(cmdbuf,string1);
  }
  strcat(cmdbuf,"properties=vp,rho");
  for(i=0; i<nother; i++) {
    strcat(cmdbuf,",");
    strcat(cmdbuf,onames[i]);
  }
  if(verbose)
    fprintf(stderr,"Executing %s\n",cmdbuf);
  tfp = popen(cmdbuf,"w");
  sprintf(cmdbuf,"bhpwritecube filename=%s pathlist=%s init=yes verbose=%d ",
          dmodel,pathlist,verbose);
  for(i=0; i<nkeys; i++) {
    sprintf(string1,"key%1d=%s,%d,%d,%d ",i+1,keys[i],min[i],incr[i],nbins[i]);
    strcat(cmdbuf,string1);
  }
  strcat(cmdbuf,"units=2 ");
  strcat(cmdbuf,"properties=vp,rho");
  for(i=0; i<nother; i++) {
    strcat(cmdbuf,",");
    strcat(cmdbuf,onames[i]);
  }
  if(verbose)
    fprintf(stderr,"Executing %s\n",cmdbuf);
  dfp = popen(cmdbuf,"w");

  /* Loop over ntraces */
  for(i=0; i<ntraces; i++) {
    for(j=0; j<nkeys; j++)
      keyvals[j] = next[j];
    trcount = read_bhpio_trace(keyvals,&trace,verbose);
    /* put events in layer-number order in output */
    for(j=0; j<trace.ns; j++)
      time_trace.data[iz+j] = null;
    for(j=0; j<trace.ns; j++) {
      /* see if event is in table, if so get laynum, zero is bottom model event */
      i1 = null;
      for(k=0; k<nevents; k++) {
        if(layer_table[k].evnum == j + 1) {
          i1 = layer_table[k].laynum;
          /* bottom */
          if(i1 == 0)
            i1 = nevents;
          break;
        }
      }
      time_trace.data[iz+i1-1] = depth_trace.data[iz+i1-1] = trace.data[j];
    }

    /* first trace cannot be null */
    if(first) {
      /* skip traces until non-null top event is found */
      if(time_trace.data[iz] == null) {
        fprintf(stderr,"Skipping trace with top null event: ");
        for(j=0; j<nkeys; j++)
          fprintf(stderr," %d ",next[j]);
        fprintf(stderr,"\n");
        /*err("First trace cannot have null top event\n");*/
        for(j=nkeys-1; j>=0; j--) {
          next[j] += incr[j];
          if(next[j] > max[j])
            next[j] = min[j];
          else
            break;
        }
        continue;
      }
      else
        first = 0;
    }

    /* if trace has non-null top event, save it to replace null trace */
    if(time_trace.data[iz] != null)
      for(j=0; j<nevents; j++)
        savetr[j] = time_trace.data[iz+j];

    /* if top event null use save trace */
    if(time_trace.data[iz] == null)
      for(j=0; j<nevents; j++)
        time_trace.data[j+iz] = depth_trace.data[j+iz] = savetr[j];

    /* make sure first layer top is not null */
    if(depth_trace.data[iz] == null) {
      fprintf(stderr,"null in first sample for %s=%d\n",keys[0],keyvals[0]);
      err("First event cannot be NULL\n");
    }

    /* replace null with previous layer depth, and constrain depth to >= previous depth */
    for(j=iz+1; j<iz+nevents; j++)
      if(time_trace.data[j] == null || time_trace.data[j] < time_trace.data[j-1])
        time_trace.data[j] = depth_trace.data[j] = time_trace.data[j-1];

    /* update min, max depths */
    if(depth_trace.data[iz] < mind)
      mind = depth_trace.data[iz];
    if(depth_trace.data[iz+nlayers] > maxd)
      maxd = depth_trace.data[iz+nlayers];

    /* fill in property values - vp, rho, followed by others*/
    for(i1=0; i1<nevents; i1++) {
      i2 = layer_table[i1].laynum;
      /* skip zero laynum */
      if(i2 == 0)
        continue;
      i2--;
      time_trace.data[i2] = depth_trace.data[i2] = layer_table[i1].vp;
      time_trace.data[nlayers+i2] = depth_trace.data[nlayers+i2] = layer_table[i1].rho;
      for(j=0; j<nother; j++)
        time_trace.data[(j+2)*nlayers+i2] = depth_trace.data[(j+2)*nlayers+i2] = layer_table[i1].other[j];
    }

    /* depth-time conversion using layer thickness to time thickness */
    /* use avgvel for first time */
    time_trace.data[iz] = 2 * time_trace.data[iz] / avgvel;
    for(j=iz+1,i2=0; j<iz+nlayers+1; j++,i2++) {
      dthick = depth_trace.data[j] - depth_trace.data[j-1];
      tthick = 2. * dthick / depth_trace.data[i2];
      time_trace.data[j] = time_trace.data[j-1] + tthick;
    }

    /* apply layer shifts */
    for(j=iz,k=0; j<iz+nlayers+1; j++,k++)
      time_trace.data[j] += 0.001 * layer_table[k].shift;

    /* update mint, maxt */
    if(time_trace.data[iz] < mint)
      mint = time_trace.data[iz];
    if(time_trace.data[iz+nlayers] > maxt)
      maxt = time_trace.data[iz+nlayers];

    /* convert model times to mills */
    for(j=iz,k=0; j<iz+nlayers+1; j++,k++)
      time_trace.data[j] *= 1000.0;

    memcpy((void *)&time_trace,(const void *)&trace,HDRBYTES);
    memcpy((void *)&depth_trace,(const void *)&trace,HDRBYTES);
    time_trace.ns = depth_trace.ns = nprop * nlayers + nlayers + 1;  
    time_trace.dt = depth_trace.dt = 1000;
    fputtr(tfp,&time_trace);
    fputtr(dfp,&depth_trace);
    for(j=nkeys-1; j>=0; j--) {
      next[j] += incr[j];
      if(next[j] > max[j])
        next[j] = min[j];
      else
        break;
    }
  }

  if(verbose) {
    fprintf(stderr,"Min Depth in Model(meters) = %f, Max Depth in Model(meters) = %f\n",mind,maxd);
    fprintf(stderr,"Min Time in Model(seconds) = %f, Max Time in Model(seconds) = %f\n",mint,maxt);
  }

  pclose(tfp);
  pclose(dfp);
  return EXIT_SUCCESS;

}

char **get_heading(FILE *fp, int *nh, int verbose)
{

  char **temp;
  char **head;

  int i;

  temp = calloc(1024,sizeof(char *));
  for(i=0; i<1024; i++)
    temp[i] = calloc(32,sizeof(char));

  /* count fields in heading */
  while(fgets(record,512,fp) != NULL) {
    if(verbose)
      fprintf(stderr,"%d chars in record\n",(int)strlen(record));
    if(!strcmp(record,""))
      continue;
    else {
      for(i=0; i<(int)strlen(record); i++) {
        if(record[i] == '\t' || record[i] == '\n')
          record[i] = ' ';
      }
      temp[0] = strtok(record," ");
      *nh = 1;
      for(;;) {
        if((temp[*nh] = strtok(NULL," ")) != NULL) {
          if(!strcmp(temp[*nh],"") || !strcmp(temp[*nh],"\n"))
            break;
          else
            (*nh)++;
        }
        else
          break;
      }
    }
    break;
  }

  if(verbose)
    fprintf(stderr,"Found %d headings\n",*nh);
  head = calloc(*nh,sizeof(char *));
  for(i=0; i<*nh; i++)
    head[i] = calloc(32,sizeof(char));
  for(i=0; i<*nh; i++)
    strcpy(head[i],temp[i]);
  if(verbose) {
    for(i=0; i<*nh; i++)
      fprintf(stderr," %s ",head[i]);
    fprintf(stderr,"\n");
  }

  return head;

}
