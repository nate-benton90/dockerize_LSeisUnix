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
* KEYWORDS:  $RCSfile: bhpaliascube.c,v $
*            $Revision: 1.4 $
*            $Date: 2003/04/01 22:07:21 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bhpio.h"
#ifdef CUBE
  #include "bhpiocube.h"
#else
  #include "bhpioseq.h"
#endif

/*********************** self documentation **********************/
char *sdoc[] = {
"  BHPALIAS makes an aliased version of a BHPIO dataset.           ",
"  An aliased dataset is just a copy of the dataset header file    ",
"  with the suffix '_as_events'. An alias of trini_model would be  ",
"  trini_model_as_events. Aliasing allows a dataset to be treated  ",
"  as if each of it's samples is an event or horizon. The events   ",
"  are named 'layern_name', where n is the sample index, and name  ",
"  is the name of one of the properties in the data, or if the     ",
"  dataset is not model data, the layer names are just 'data'.     ",
"  Since an event dataset already has a name for every sample,     ",
"  it cannot be aliased.                                           ",
"                                                                  ",
"  bhpalias filename=file                                          ",
"  Required arguments:                                             ",
"    filename=file        The name of the BHPIO dataset being aliased",
"  Optional arguments:                                             ",
"    pathlist='file'.dat  File containing dataset directories      ",
"    verbose=0            For debug print                          ", 
"                                                                  ",
NULL};

int main(int argc, char **argv)
{

  char fpath[NAMELEN];    /* Partition zero path name */
  char cindex[8];         /* Use to append file index to path */
  char *pathlist;         /* File containing user-specified path-list for forming filenames */
  char *path;             /* First path from pathlist */
  char string[32];        /* scratch */
  char **prop;            /* pre-existing property names */

  int verbose;            /* debug printout */
  int i, j;               /* Loop counters */
  int nprop;              /* pre-existing number of properties */
  int nval;               /* number of values per property */

  FILE *ifp;            /* Existing header file */
  FILE *ofp;            /* New header file */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  if(!getparstring("filename",&file_hdr.filename))
    err("Filename is required\n");

  /* Get pathlist */
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc(strlen(file_hdr.filename)+5,sizeof(char));
    strcpy(pathlist,file_hdr.filename);
    strcat(pathlist,".dat");
  }
  if(verbose)
    printf("Opening %s\n", pathlist);
  if(!(ifp = fopen(pathlist,"r")))
    err("pathlist specified, but cannot open %s\n", pathlist);
  else {
    /* Only need first path to get to partition zero */
    path = calloc(NAMELEN,sizeof(char));
    fscanf(ifp,"%s\n", path);
    fclose(ifp);
  }

  /* Make sure file exists */
  i = get_bhpio_path(path,file_hdr.filename,"_0000.HDR",fpath,verbose);
  if(i != 0 && verbose != 0)
    printf("FILE: path=%s, filename=%s exists\n",path,file_hdr.filename);
  else if(i == 0) {
    printf("FILE: path=%s, filename=%s does not exist\n",path,file_hdr.filename);
    return EXIT_FAILURE;
  }

  /* Get file header */
  read_file_hdr(fpath,verbose);

  /* If horizons already exist, quit */
  if(file_hdr.nprop < 0)
    err("%s already has horizons\n",file_hdr.filename);

  /* append "_as_events" to filename */
  strcpy(fpath,path);
  strcat(fpath,"/");
  strcat(fpath,file_hdr.filename);
  strcat(fpath,"_as_events");
  strcat(fpath,"_0000.HDR");
  if((ofp = fopen(fpath,"w")) == NULL)
    err("Cannot open %s\n",fpath);
  if(verbose)
    fprintf(stderr,"Opened %s\n",fpath);
  
  if(file_hdr.nsamp > 2048)
    err("maximum of 2048 events allowed\n");

  /* save exisitng number of properties */
  nprop = file_hdr.nprop;
  prop = calloc(nprop,sizeof(char **));
  /* copy properties to prop, and reuse */
  for(i=0; i<nprop; i++) {
    prop[i] = calloc(128,sizeof(char));
    strcpy(prop[i],properties[i]);
    free(properties[i]);
  } 
  free(properties);
  if(verbose)
    fprintf(stderr,"Copied %d properties\n",nprop);
  /* fill in event names */
  file_hdr.nprop = -file_hdr.nsamp;
  properties = calloc(file_hdr.nsamp,sizeof(char *));
  for(i=0; i<file_hdr.nsamp; i++)
    properties[i] = calloc(NAMELEN,sizeof(char));
  if(verbose)
    fprintf(stderr,"Allocated %d chars for event names\n",128*file_hdr.nsamp);
  /* if nprop is zero, write only layer1_data thru layern_data */
  if(nprop == 0) {
    for(i=0; i<file_hdr.nsamp; i++) {
      strcpy(properties[i],"layer");
      sprintf(string,"%1d",i+1);
      strcat(properties[i],string);
      strcat(properties[i],"_data");
    }
  }
  /* if nprop > 0, write layer1_prop thru layern_prop for each property, then layer1_depth thru layern_depth */
  else {
    nval = (file_hdr.nsamp - 1) / (nprop + 1);
    for(i=0; i<nprop+1; i++) {
      if(i < nprop) {
        for(j=0; j<nval; j++) {
          strcpy(properties[i*nval+j],"layer");
          sprintf(string,"%1d",j+1);
          strcat(properties[i*nval+j],string);
          strcat(properties[i*nval+j],"_");
          strcat(properties[i*nval+j],prop[i]);
        }
      }
      /* depths */
      else {
        for(j=0; j<nval; j++) {
          strcpy(properties[i*nval+j],"layer");
          sprintf(string,"%1d",j+1);
          strcat(properties[i*nval+j],string);
          strcat(properties[i*nval+j],"_top");
        }
        /* bottom of model */
        strcpy(properties[i*nval+nval],"layer");
        sprintf(string,"%1d",nval);
        strcat(properties[i*nval+nval],string);
        strcat(properties[i*nval+nval],"_bottom");
      }
    }
  }

  /* set data type */
  file_hdr.data_type = 2;
  /* vkey */
  strcpy(file_hdr.vkey,"tracl");
  file_hdr.vmin = 1;
  file_hdr.vmax = file_hdr.nsamp;
  file_hdr.vinc = 1;
  /* set stats to all zero */
  file_hdr.prop_minval = calloc(file_hdr.nsamp,sizeof(float));
  file_hdr.prop_maxval = calloc(file_hdr.nsamp,sizeof(float));
  file_hdr.prop_meanval = calloc(file_hdr.nsamp,sizeof(float));
  file_hdr.prop_rmsval = calloc(file_hdr.nsamp,sizeof(float));

  if(write_file_hdr(fpath,verbose))
    err("Error writing %s\n",fpath);

  return EXIT_SUCCESS;

}
