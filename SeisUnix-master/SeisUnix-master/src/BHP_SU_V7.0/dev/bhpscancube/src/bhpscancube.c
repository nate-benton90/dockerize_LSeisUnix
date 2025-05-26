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
* KEYWORDS:  $RCSfile: bhpscancube.c,v $
*            $Revision: 1.4 $
*            $Date: 2003/01/28 23:27:16 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpscancube.c,v $
* Revision 1.4  2003/01/28 23:27:16  ahmilb
* Replace hard-coded lengths with NAMELEN
* Implement isadir function
*
* Revision 1.3  2002/08/06 19:09:14  ahmilb
* Changed arg list for parse_keylist.
*
* Revision 1.2  2002/01/30 16:12:59  ahmilb
* Add endian parameter
*
* Revision 1.1  2001/11/14 22:50:56  ahmilb
* Initial version of program to scan gridded BHPIO dataset for missing traces.
*
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
"                                                                  ",
"  BHPSCAN is a utility program used to find holes in a BHPIO      ",
"  dataset. A hole is defined as adjacent traces where one or more ",
"  key values differ by more than the increment defined for each key.",
"                                                                  ",
" bhpscan filename=      [optional parameters]                     ",
"                                                                  ",
" Required Parameters:                                             ",
"  filename=               File name which was used to create the dataset",
"                                                                  ",
" Optional Parameters:                                             ",
"  pathlist='filename'.dat Pathlist file which was used to create  ",
"                          the dataset                             ",
"  verbose=0               Use verbose=1 for debug print           ",
"                                                                  ",
NULL};

/* prototypes */
void get_hdr_limits(char *fpath, int verbose);
void search_list(int *hdr_vals, int **key_vals, int *radius, int ntraces,
                 int *start, int verbose);
void get_hdr_vals(segy *tr, int nkeys, char **type, int *hdr_vals, int *key_index, 
                  int my_endian, int endian, int verbose);

int main(int argc, char **argv)
{

  char fpath[NAMELEN];    /* Partition zero path name */
  char *pathlist;         /* File containing user-specified path-list for forming filenames */
  char *path;             /* First path from pathlist */
  char *aname;            /* aliased filename */
  char *keylist_subset;   /* input to parser to get complete keylist */ 
  char cindex[8];         /* part number as string */
  char **key_type;        /* key types */

  int verbose;            /* debug printout */
  int i, j;               /* Loop counters */
  int *nklist;            /* number of occurences of each key */
  int **klist;            /* Expanded key lists */
  int ntraces;            /* total number of traces to read */
  int **key_vals;         /* list of all expected key combinations */
  int *hdr_vals;          /* trace header vals */
  int *key_index;         /* Header key indices */
#ifdef CWP_BIG_ENDIAN     /* Set default endian */
  int my_endian = 1;
#else
  int my_endian = 0;
#endif
  int endian;             /* 1=force BIG, 0=force LITTLE, if unspecified, use my_endian */
  int *next;              /* next key value for each key */
  int found=0;            /* number of matching traces found */
  int *radius;            /* half the increment for each key */
  int start;              /* starting point for next search */

  FILE *fp;               /* data */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  if(!getparint("endian",&endian))
    endian = my_endian;
  if(verbose && endian != my_endian) {
    if(endian == 1)
      printf("Forcing BIG endian\n");
    else
      printf("Forcing LITTLE endian\n");
  }

  if(!getparstring("filename",&file_hdr.filename))
    err("Filename is required\n");
  /* save filename */
  i = strlen(file_hdr.filename);
  aname = calloc(i+1,sizeof(char));
  strcpy(aname,file_hdr.filename);

  /* check for alias */
  if(!strcmp(&file_hdr.filename[i-10],"_as_events")) {
    /* trim */
    file_hdr.filename[i-10] = '\0';
    if(verbose) {
      printf("Aliased filename %s\n",aname);
      printf("Trimmed filename %s\n",file_hdr.filename);
    }
  }

  /* Get pathlist */
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc(strlen(file_hdr.filename)+5,sizeof(char));
    strcpy(pathlist,file_hdr.filename);
    strcat(pathlist,".dat");
  }
  if(verbose)
    printf("Opening %s\n", pathlist);
  if(!(hdrfp = fopen(pathlist,"r")))
    err("pathlist specified, but cannot open %s\n", pathlist);
  else {
    /* Only need first path to get to partition zero */
    path = calloc(NAMELEN,sizeof(char));
    fscanf(hdrfp,"%s\n", path);
    fclose(hdrfp);
  }

  if(verbose)
    printf("Path: %s,  Filename: %s\n",path,file_hdr.filename);

  /* Make sure file exists */
  if(get_bhpio_path(path,aname,"_0000.HDR",fpath,verbose)) {
    if(verbose)
      printf("FILE: path=%s, filename=%s exists\n",path,file_hdr.filename);
  }
  else {
    printf("FILE: path=%s, filename=%s does not exist\n",path,file_hdr.filename);
    return EXIT_SUCCESS;
  }

  /* Get file header */
  read_file_hdr(fpath,verbose);

  /* header limits */
  key_index = calloc(file_hdr.nkeys,sizeof(int));
  key_type = calloc(file_hdr.nkeys,sizeof(char *));
  next = calloc(file_hdr.nkeys,sizeof(int));
  radius = calloc(file_hdr.nkeys,sizeof(int));
  for(i=0; i<file_hdr.nkeys; i++)
    key_type[i] = calloc(2,sizeof(char));
  hdr_limits = calloc(file_hdr.nkeys,sizeof(bhp_hdr_limits));
  if(!(get_bhpio_path(path,file_hdr.filename,"_0000.0001.HDR",fpath,verbose)))
    err("Cannot access header-limits: %s\n", fpath);
  /* Load hdr_limits */
#ifdef CUBE
  if(read_hdr_limits(fpath,verbose))
    err("Error reading %s\n", fpath);
#else
  get_hdr_limits(fpath,verbose);
#endif
  for(i=0; i<file_hdr.nkeys; i++) {
    key_index[i] = getindex(hdr_limits[i].bhp_hdr_name);
    key_type[i] = hdtype(hdr_limits[i].bhp_hdr_name);
    radius[i] = hdr_limits[i].bhp_hdr_inc / 2;
  }

  keylist_subset = calloc(256,sizeof(char));
  nklist = calloc(file_hdr.nkeys,sizeof(int));
  klist = calloc(file_hdr.nkeys,sizeof(int *));
  hdr_vals = calloc(file_hdr.nkeys,sizeof(int));

  /* use parsing code to fill in keylist */
  for(i=0; i<file_hdr.nkeys; i++) {
    strcpy(keylist_subset,"*");
    klist[i] = parse_keylist(keylist_subset,hdr_limits[i].bhp_hdr_min,
                             hdr_limits[i].bhp_hdr_max,hdr_limits[i].bhp_hdr_inc,
                             &nklist[i],0,verbose);
  }
  if(verbose) {
    printf("Expanded keylists\n");
    for(i=0; i<file_hdr.nkeys; i++) {
      for(j=0; j<nklist[i]; j++) {
        printf("%d ", klist[i][j]);
        if(!((j+1)%8))
          printf("\n");
      }
      printf("\n");
    }
  }

  /* count number of traces expected */
  ntraces = 1;
  for(i=0; i<file_hdr.nkeys; i++)
    ntraces *= nklist[i];
  printf("Expecting %d traces\n",ntraces);
  start = 0;
  
  /* build full key_vals list */
  key_vals = calloc(ntraces,sizeof(int *));
  for(i=0; i<ntraces; i++)
    key_vals[i] = calloc(file_hdr.nkeys,sizeof(int));
  for(i=0; i<file_hdr.nkeys; i++)
    next[i] = 0;
  for(i=0; i<ntraces; i++) {
    /* next set of key_vals */
    for(j=0; j<file_hdr.nkeys; j++)
      key_vals[i][j] = klist[j][next[j]];
    /* Check key wrap around */
    for(j=file_hdr.nkeys-1; j>=0; j--) {
      next[j]++;
      if(next[j] == nklist[j])
        next[j] = 0;
      else
        break;
    }
  }
  /*for(i=0; i<ntraces; i++) {
    for(j=0; j<file_hdr.nkeys; j++)
      printf("%d ",key_vals[i][j]);
    printf("\n");
  }*/

  /* read all trace headers, and match against list */
  for(i=0; i<file_hdr.nparts; i++) {
    strcpy(path,&filesys[i*NAMELEN]);
    /* if path is a dir, append filename, else filename is already appended */
    if(isadir(path) == 1) {
      if(verbose)
        fprintf(stderr,"%s is a directory, appending filename\n",path);
      strcat(path,"/");
      strcat(path,file_hdr.filename);
      strcat(path,"_");
      sprintf(cindex,"%04i\0",i+1);
      strcat(path,cindex);
      strcat(path,".su");
    }
    else {
      if(verbose)
        fprintf(stderr,"%s is a file, not appending filename\n",path);
    }
    if(verbose)
      printf("Opening %s\n", path);
    fp = fopen(path,"r");
    if(!fp)
      err("Could not open %s\n", path);
    while(efread(&tr,HDRBYTES,1,fp)) {
      get_hdr_vals(&tr,file_hdr.nkeys,key_type,hdr_vals,key_index,my_endian,endian,verbose);
      search_list(hdr_vals,key_vals,radius,ntraces,&start,verbose);
      fseek(fp,(long)(file_hdr.nsamp*sizeof(float)),SEEK_CUR);
    }
    fclose(fp);
  }

  for(i=0; i<ntraces; i++) {
    if(key_vals[i][0] == -1)
      found++;
    else {
      printf("MISSING TRACE: ");
      for(j=0; j<file_hdr.nkeys; j++)
        printf("%s = %d ",hdr_limits[j].bhp_hdr_name,key_vals[i][j]);
      printf("\n");
    }
  }
  printf("Found %d traces\n",found);
  return EXIT_SUCCESS;

}

void get_hdr_vals(segy *tr, int nkeys, char **type, int *hdr_vals, int *key_index, 
                  int my_endian, int endian, int verbose)
{

  int i;

  Value hval;

  for(i=0; i<nkeys; i++) {
    gethval(tr,key_index[i],&hval);
    hdr_vals[i] = vtoi(type[i],hval);
    if(my_endian != endian)
      swap_int_4(&hdr_vals[i]);
  }

}

void search_list(int *hdr_vals, int **key_vals, int *radius, int ntraces,
                 int *start, int verbose)
{

  int i, j;
  int match;
  int end;

  end = ntraces;
  for(;;) {
    for(i=*start; i<end; i++) {
      if(key_vals[i][0] == -1)
        continue;
      else {
        match = 1;
        for(j=0; j<file_hdr.nkeys; j++) {
          if(hdr_vals[j] < (key_vals[i][j] - radius[j]) ||
             hdr_vals[j] > (key_vals[i][j] + radius[j])) {
            match = 0;
            break;
          }
        }
        if(match == 1) {
          /*printf("Match %d %d %d\n",hdr_vals[0],hdr_vals[1],hdr_vals[2]);*/
          key_vals[i][0] = -1;
          *start = i + 1;
          break;
        }
      }
    }
    if(match == 0 && *start > 0) {
      end = *start - 1;
      *start = 0;
      continue;
    }
    else
      break;
  }
}

#ifdef SEQ
void get_hdr_limits(char *fpath, int verbose)
{

  int i;

  hdrfp = fopen(fpath,"r");
  if(!hdrfp)
    printf("Could not open %s\n", fpath);
  /* Load hdr_limits */
  for(i=0; i<file_hdr.nkeys; i++) {
    fscanf(hdrfp,"HDRNAME = %s\n", hdr_limits[i].bhp_hdr_name);
    fscanf(hdrfp,"MIN = %d\n", &hdr_limits[i].bhp_hdr_min);
    fscanf(hdrfp,"MAX = %d\n", &hdr_limits[i].bhp_hdr_max);
    fscanf(hdrfp,"INCR = %d\n", &hdr_limits[i].bhp_hdr_inc);
    fscanf(hdrfp,"SCALAR = %d\n", &hdr_limits[i].bhp_hdr_scalar);
    fscanf(hdrfp,"NUM = %d\n", &hdr_limits[i].bhp_hdr_num);
    fscanf(hdrfp,"ORDER = %d\n", &hdr_limits[i].bhp_hdr_order);
    fscanf(hdrfp,"TYPE = %d\n", &hdr_limits[i].bhp_hdr_type);
    fscanf(hdrfp,"DATATYPE = %s\n", &hdr_limits[i].bhp_hdr_data);
    fscanf(hdrfp,"VLEN = %d\n", &hdr_limits[i].bhp_hdr_vlen);
    fscanf(hdrfp,"VLOC = %d\n", &hdr_limits[i].bhp_hdr_vloc);
  }
  fclose(hdrfp);

}
#endif
