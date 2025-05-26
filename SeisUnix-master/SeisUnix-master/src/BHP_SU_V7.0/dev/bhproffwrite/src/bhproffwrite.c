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
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "su.h"
#include "bhpio_api.h"

/* prototypes */
int write_item(void *buf, int nitems, char *ftype, char *dtype, FILE *fp, int verbose);

/*********************** self documentation **********************/
char *sdoc[] = {
" BHPROFFWRITE - convert SU data to ROFF model format              ",
" bhproffwrite filename= [optional parameters]                     ",
"                                                                  ",
" Required Parameters:                                             ",
"  filename=             Name of BHPIO cube dataset to be converted.",
"                        The data must be a model file in cross-section",
"                        order.                                    ",
"                                                                  ",
" Optional Parameters:                                             ",
"  model=model.roff      ROFF model output                         ",
"  pathlist=             Name of BHPIO pathlist file. If unspecified,",
"                        'filename'.dat is used.                   ",
"  properties=           Subset of properties to be loaded. If     ",
"                        unspecified, all properties from filename ",
"                        are loaded.                               ",
"  offsets=0,0,0         Offsets to add to x,y,z in roff model     ",
"  scales=1,1,-1         Scalars to apply to x,y,z in roff model   ",
"                        x and y for roff model are taken from gx  ",
"                        gy trace headers.                         ",
"                                                                  ",
"                                                                  ",
NULL};

/*  Prototypes */

/* Globals */
#define FNULL -999.0
#define INULL -999
#define CNULL 255

int main(int argc, char **argv)
{

  char *model;               /* ROFF model file */
  char *filename;            /* BHPIO cube dataset */
  char *pathlist;            /* BHPIO cube dataset */
  char c8[9];                /* 8=char + null */
  char *string;              /* general-purpose string */
  char *ftype;               /* model file type - asc or bin */
  char *timestring;          /* date and time from asctime */
  char **prop;               /* properties parameter */
  char *pname, *sname;       /* key names */

  unsigned char *splits;     /* splitEnz 1 byte per corner */
  unsigned char *active;     /* active cells, 1 byte per cell */

  float f;
  float scalco;              /* coordinate scalar */
  float *cL;                 /* cornerLines */
  float **pdata;             /* property data */
  float *zvals;              /* corner point depths */
  float *offsets;            /* x, y offsets in roff model */
  float *scales;             /* x, y scalars in roff model */

  FILE *fp;                  /* Output file pointer */

  int i, j, k, m, n;
  int verbose;               /* debug print */
  int trcount;               /* number of traces returned by read_bhpio_trace */
  int *key_index;            /* key indices returned by open_bhpio_dataset */
  int nkeys;                 /* nkeys returned by open_bhpio_dataset */
  int *min, *max, *incr;     /* header limits */
  int ntraces;               /* total number of traces as per header limits */
  int *num;                  /* number of occurences of each key */
  int *next;                 /* next expected header value for each key */
  int stat;                  /* return from write_item, 0=OK */
  int ncL;                   /* num cornerLines */
  int nlayers;               /* number of layers in model */
  int top;                   /* trace index of model top */
  int nprop;                 /* number of properties to load */
  int *iprop;                /* trace index of each required property */
  int nx, ny, nxcl, nycl;    /* num rows and columns of traces and cornerLines */
  int minp, maxp, mins, maxs; /* min, max keys found in data */

  segy tr;                   /* Output trace */

  struct tm timeptr;         /* time and date */

  time_t now;

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  /* No stdin required */
  requestdoc(0);

  /* Debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* input file required */
  if(!getparstring("filename",&filename))
    err("filename is required\n");

  /* Model */
  if(!getparstring("model",&model))
    model = "model.roff";

  /* type */
  /* only binary for now */
  /*if(!getparstring("ftype",&ftype))*/
    ftype = "bin";
  /*if(strcmp(ftype,"bin") && strcmp(ftype,"asc"))
    err("ftype should be 'asc' or 'bin', is %s\n",ftype);
  if(!strcmp(ftype,"bin"))
    printf("Creating binary model file\n");
  else
    printf("Creating ASCII model file\n"); */

  /* pathlist */
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc((int)strlen(filename)+5,sizeof(char));
    strcpy(pathlist,filename);
    strcat(pathlist,".dat");
  }
  printf("Using %s for pathlist\n",pathlist);

  /* open input */
  key_index = open_bhpio_dataset(filename,pathlist,&nkeys,verbose);
  if(nkeys != 2)
    err("Exactly 2 BHPIO keys required\n");
  if(verbose) {
    printf("Key Indices: ");
    for(i=0; i<nkeys; i++)
      printf(" %d",key_index[i]);
    printf("\n");
  }

  nlayers = (file_hdr.nsamp - 1) / (file_hdr.nprop + 1);
  /* properties to load to model */
  nprop = countparval("properties");
  if(nprop == 0) {
    nprop = file_hdr.nprop;
    iprop = calloc(nprop,sizeof(int));
    prop = calloc(nprop,sizeof(char *));
    for(i=0; i<nprop; i++) {
      iprop[i] = i * nlayers;
      prop[i] = calloc(NAMELEN+1,sizeof(char));
      strcpy(prop[i],properties[i]);
    }
    printf("Loading %d properties: ",nprop);
    for(i=0; i<nprop; i++)
      printf("name: %s, index: %d ",prop[i],iprop[i]);
    printf("\n");
  }
  else {
    prop = calloc(nprop,sizeof(char *));
    getparstringarray("properties",prop);
    iprop = calloc(nprop,sizeof(int));
    /* make sure requested properties are in data */
    for(i=0; i<nprop; i++) {
      k = -1;
      for(j=0; j<file_hdr.nprop; j++) {
        if(!strcmp(prop[i],properties[i])) {
          iprop[i] = j * nlayers;
          k = j;
          break;
        }
      }
      if(k == -1)
        err("%s is not a valid property\n",prop[i]);
    }
    printf("Loading %d properties: ",nprop);
    for(i=0; i<nprop; i++)
      printf("name: %s, index: %d ",prop[i],iprop[i]);
    printf("\n");
  }
  
  /* offsets and scales */
  i = countparval("offsets");
  if(i > 0 && i != 3)
    err("If offsets is specified, exactly 3 values are required\n");
  offsets = calloc(3,sizeof(float));
  if(i > 0)
    getparfloat("offsets",offsets);
  else {
    offsets[0] = 0;
    offsets[1] = 0;
    offsets[2] = 0;
  }
  i = countparval("scales");
  if(i > 0 && i != 3)
    err("If scales is specified, exactly 3 values are required\n");
  scales = calloc(3,sizeof(float));
  if(i > 0)
    getparfloat("scales",scales);
  else {
    scales[0] = 1;
    scales[1] = 1;
    scales[2] = -1;
  }

  if(verbose) {
    printf("OFFSETS - X: %f, Y: %f, Z: %f\n",offsets[0],offsets[1],offsets[2]);
    printf("SCALES - X: %f, Y: %f, Z: %f\n",scales[0],scales[1],scales[2]);
  }

  set_bhpio_binning_rule("match",verbose);
  set_bhpio_interp_flag("no",verbose);

  /* key indices, limits, etc */
  min = calloc(nkeys,sizeof(int));
  max = calloc(nkeys,sizeof(int));
  incr = calloc(nkeys,sizeof(int));
  num = calloc(nkeys,sizeof(int));
  next = calloc(nkeys,sizeof(int));

  /* header limits */
  if((i = get_bhpio_header_limits(min,max,incr,verbose)) != 0)
    err("Error getting limits\n");

  /* find number of occupied slots */
  for(i=0; i<nkeys; i++)
    num[i] = (max[i] - min[i] + incr[i]) / incr[i];
  /* max ntraces is product of nums */
  ntraces = num[0] * num[1];
  minp = maxp = mins = maxs = 0;
  /* initialize keys for forward search */
  for(i=0; i<nkeys; i++)
    next[i] = min[i];
  /* forward search for first trace */
  for(i=0; i<ntraces; i++) {
    trcount = read_bhpio_trace(next,&tr,verbose);
    if(trcount == 1) {
      minp = next[0];
      mins = next[1];
      break;
    }
    /* cycle keys */
    for(j=nkeys-1; j>=0; j--) {
      next[j] += incr[j];
      if(next[j] > max[j])
        next[j] = min[j];
      else
        break;
    }
  }
  if(minp == 0)
    err("Empty dataset\n");
  /* initialize keys for backward search */
  for(i=0; i<nkeys; i++)
    next[i] = max[i];
  /* backward search for first trace */
  for(i=0; i<ntraces; i++) {
    trcount = read_bhpio_trace(next,&tr,verbose);
    if(trcount == 1) {
      maxp = next[0];
      maxs = next[1];
      break;
    }
    /* cycle keys */
    for(j=nkeys-1; j>=0; j--) {
      next[j] -= incr[j];
      if(next[j] < min[j])
        next[j] = max[j];
      else
        break;
    }
  }

  pname = getkey(key_index[0]);
  sname = getkey(key_index[1]);
  fprintf(stderr,"Trace Limits from HDR File: \n");
  fprintf(stderr,"NAME: %s, MIN: %d, MAX: %d, INCR: %d, NUM: %d\n",pname,min[0],max[0],incr[0],num[0]);
  fprintf(stderr,"NAME: %s, MIN: %d, MAX: %d, INCR: %d, NUM: %d\n",sname,min[1],max[1],incr[1],num[1]);
  min[0] = minp;
  min[1] = mins;
  max[0] = maxp;
  max[1] = maxs;
  num[0] = (max[0] - min[0] + incr[0]) / incr[0];
  num[1] = (max[1] - min[1] + incr[1]) / incr[1];
  ntraces = num[0] * num[1];
  fprintf(stderr,"Actual Trace Limits:\n");
  fprintf(stderr,"NAME: %s, MIN: %d, MAX: %d, INCR: %d, NUM: %d\n",pname,min[0],max[0],incr[0],num[0]);
  fprintf(stderr,"NAME: %s, MIN: %d, MAX: %d, INCR: %d, NUM: %d\n",sname,min[1],max[1],incr[1],num[1]);
  fprintf(stderr,"Reading %d total traces\n",ntraces);

  nxcl = num[0];
  nycl = num[1];
  ncL = 6 * nxcl * nycl;
  nx = nxcl - 1;
  ny = nycl - 1;
  if(verbose)
    printf("NX: %d, NY: %d, NXCL: %d, NYCL: %d\n",nx,ny,nxcl,nycl);
  top = nlayers * file_hdr.nprop;
  printf("Allocating %d ints for cornerLines\n",ncL);
  cL = calloc(ncL,sizeof(float));
  /* property data array */
  pdata = calloc(nprop,sizeof(float *));
  for(i=0; i<nprop; i++)
    pdata[i] = calloc(nx*ny*nlayers,sizeof(float));
  if(verbose)
    printf("Allocated %d floats for property data\n",nprop*nlayers*ntraces);
  /* FILL SPLITS WITH ALL ONES */
  splits = calloc(ntraces*(nlayers+1),sizeof(unsigned char));
  zvals = calloc(ntraces*(nlayers+1),sizeof(float));
  if(verbose)
    printf("Allocated %d bytes for splitEnz\n",ntraces*(nlayers+1));
  for(i=0; i<ntraces*(nlayers+1); i++) {
    splits[i] = (unsigned char)1;
  }
  /* FILL SPLITS WITH ALL ONES */
  /* make all cells active */
  active = calloc(nx*ny*nlayers,sizeof(unsigned char));
  for(i=0; i<nx*ny*nlayers; i++)
    active[i] = (unsigned char)1;
  /* scratch character space */
  string = calloc(128,sizeof(char));

  /* open model for output */
  fp = fopen(model,"w");
  if(fp == NULL)
    err("Can't open %s for writing\n",model);
  printf("Opened %s for writing\n",model);

  /* write header stuff to model */
  if(!strcmp(ftype,"asc")) {
    if((stat = write_item("roff-asc",(int)strlen("roff-asc"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write roff-asc");
  }
  else {
    if((stat = write_item("roff-bin",(int)strlen("roff-bin"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write roff-bin");
  }
  strcpy(string,"#Created by bhproffwrite from ");
  strcat(string,filename);
  strcat(string,"#");
  if((stat = write_item("#ROFF file#",(int)strlen("#ROFF file#"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write #ROFF file#\n");
  if((stat = write_item("#Creator: bhproffwrite       #",(int)strlen("#Creator: bhproffwrite       #"),
                        ftype,"char",fp,verbose)) != 0)
    err("Failed to write #Creator: bhproffwrite       #\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("filedata",(int)strlen("filedata"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write filedata\n");
  if(!strcmp(ftype,"bin")) {
    if((stat = write_item("int",(int)strlen("int"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write int\n");
    if((stat = write_item("byteswaptest",(int)strlen("byteswaptest"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write byteswaptest\n");
    j = 1;
    if((stat = write_item(&j,1,ftype,"int",fp,verbose)) != 0)
      err("Failed to write 1 %d\n",stat);
  }
  if(!strcmp(ftype,"asc")) {
    if((stat = write_item("char",(int)strlen("char"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write char\n");
    if((stat = write_item("filetype",(int)strlen("filetype"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write filetype\n");
    strcpy(string," grid");
    string[0] = '\"';
    string[5] = '\"';
    string[6] = '\n';
    string[7] = '\0';
    if((stat = write_item(string,(int)strlen(string),ftype,"char",fp,verbose)) != 0)
      err("Failed to write string\n");
  }
  else {
    if((stat = write_item("char",(int)strlen("char"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write char\n");
    if((stat = write_item("filetype",(int)strlen("filetype"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write filetype\n");
    if((stat = write_item("grid",(int)strlen("grid"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write grid\n");
  }
  strcpy(string,"char creationDate");
  time(&now);
  timestring = asctime(localtime(&now));
  if(!strcmp(ftype,"asc")) {
    j = (int)strlen(string);
    string[j] = '\"';
    string[j+1] = '\0';
    strcat(string,timestring);
    i = strlen(string);
    string[i-1] = '\"';
    string[i] = '\n';
    string[i+1] = '\0';
  }
  else {
    strcat(string,timestring);
    i = strlen(string);
    string[i-1] = '\0';
  }
  if((stat = write_item("char",(int)strlen("char"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write char\n");
  if((stat = write_item("creationDate",(int)strlen("creationDate"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write creationDate\n");
  if((stat = write_item("Unknown date",(int)strlen("Unknown date"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write Unknown date\n");
  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("version",(int)strlen("version"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write version\n");
  if((stat = write_item("int",(int)strlen("int"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write int\n");
  if((stat = write_item("major",(int)strlen("major"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write major\n");
  j = 2;
  if((stat = write_item(&j,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write j\n");
  if((stat = write_item("int",(int)strlen("int"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write int\n");
  if((stat = write_item("minor",(int)strlen("minor"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write minor\n");
  j = 0;
  if((stat = write_item(&j,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write j\n");
  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("dimensions",(int)strlen("dimensions"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write dimensions\n");
  if((stat = write_item("int",(int)strlen("int"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write int\n");
  if((stat = write_item("nX",(int)strlen("nX"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write nX\n");
  if((stat = write_item(&nx,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write nx\n");
  if((stat = write_item("int",(int)strlen("int"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write int\n");
  if((stat = write_item("nY",(int)strlen("nY"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write nY\n");
  if((stat = write_item(&ny,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write ny\n");
  if((stat = write_item("int",(int)strlen("int"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write int\n");
  if((stat = write_item("nZ",(int)strlen("nZ"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write nZ\n");
  if((stat = write_item(&nlayers,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write nlayers\n");
  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("translate",(int)strlen("translate"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write translate\n");
  if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write float\n");
  if((stat = write_item("xoffset",(int)strlen("xoffset"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write xoffset\n");
  if((stat = write_item(&offsets[0],1,ftype,"float",fp,verbose)) != 0)
    err("Failed to write offsets[0]\n");
  if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write float\n");
  if((stat = write_item("yoffset",(int)strlen("yoffset"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write yoffset\n");
  if((stat = write_item(&offsets[1],1,ftype,"float",fp,verbose)) != 0)
    err("Failed to write offsets[1]\n");
  if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write float\n");
  if((stat = write_item("zoffset",(int)strlen("zoffset"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write zoffset\n");
  if((stat = write_item(&offsets[2],1,ftype,"float",fp,verbose)) != 0)
    err("Failed to write offsets[2]\n");
  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("scale",(int)strlen("scale"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write scale\n");
  if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write float\n");
  if((stat = write_item("xscale",(int)strlen("xscale"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write xscale\n");
  if((stat = write_item(&scales[0],1,ftype,"float",fp,verbose)) != 0)
    err("Failed to write scales[0]\n");
  if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write float\n");
  if((stat = write_item("yscale",(int)strlen("yscale"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write yscale\n");
  if((stat = write_item(&scales[1],1,ftype,"float",fp,verbose)) != 0)
    err("Failed to write scales[1]\n");
  if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write float\n");
  if((stat = write_item("zscale",(int)strlen("zscale"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write zscale\n");
  if((stat = write_item(&scales[2],1,ftype,"float",fp,verbose)) != 0)
    err("Failed to write scales[2]\n");
  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("cornerLines",(int)strlen("cornerLines"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write cornerLines\n");
  if((stat = write_item("array",(int)strlen("array"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write array\n");
  if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write float\n");
  if((stat = write_item("data",(int)strlen("data"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write data\n");

  /* initialize keys */
  for(i=0; i<nkeys; i++)
    next[i] = min[i];

  /* loop thru data, load pdata, write cornerlines */
  for(i=0; i<nxcl; i++) {
    for(j=0; j<nycl; j++) {
      trcount = read_bhpio_trace(next,&tr,verbose);
      if(trcount != 1) {
        /*if(verbose)
          printf("%d traces returned for keys=%d,%d\n",trcount,next[0],next[1]);*/
        if(trcount > 1)
          err("More than 1 trace returned from read_bhpio_trace\n");
      }

      if(trcount > 0) {
        /* coordinate scalar */
        if(tr.scalco < 0)
          scalco = -1. / tr.scalco;
        else if(tr.scalco > 0)
          scalco = tr.scalco;
        else
          scalco = 1.;

        /* cornerLines */
        cL[6*(i*nycl+j)] = ((float)tr.gx * scalco) / scales[0] - offsets[0];
        cL[6*(i*nycl+j)+1] = ((float)tr.gy * scalco) / scales[1] - offsets[1];
        cL[6*(i*nycl+j)+2] = (tr.data[tr.ns-1] / scales[2]) - offsets[2];
        /*cL[6*(i*nycl+j)+2] = tr.data[tr.ns-1] / scales[2];*/
        cL[6*(i*nycl+j)+3] = cL[6*(i*nycl+j)];
        cL[6*(i*nycl+j)+4] = cL[6*(i*nycl+j)+1];
        cL[6*(i*nycl+j)+5] = (tr.data[top] / scales[2]) - offsets[2];

        /* load pdata bottom-up, don't load edges */
        if(i < nxcl - 1 && j < nycl - 1) {
          for(k=0; k<nprop; k++) {
            for(m=nlayers-1,n=0; m>=0; m--,n++) {
              pdata[k][i*ny*nlayers+j*nlayers+n] = tr.data[iprop[k]+m];
            }
          }
        }

        /* zvals - bottom-up */
        for(k=0,m=nlayers; k<nlayers+1; k++,m--)
          zvals[i*nycl*(nlayers+1)+j*(nlayers+1)+k] = (tr.data[top+m] / scales[2]) - offsets[2];; 
      }

      /* cycle keys */
      for(k=nkeys-1; k>=0; k--) {
        next[k] += incr[k];
        if(next[k] > max[k])
          next[k] = min[k];
        else
          break;
      }
    }
  }
  /*if(verbose) {
    for(i=0; i<file_hdr.nprop; i++) {
      printf("Property: %s\n",prop[i]);
      for(j=0; j<nx*ny*nlayers; j++) {
        printf("%f ",pdata[i][j]);
        if((j+1)%8 == 0)
          printf("\n");
      }
      printf("\n");
    }
    printf("ZVALS:\n");
    for(i=0; i<ntraces*(nlayers+1); i++) {
      printf("%f ",zvals[i]);
      if((i+1)%4 == 0)
        printf("\n");
    }
    printf("\n");
  }*/

  if((stat = write_item(&ncL,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write ncL\n");
  if((stat = write_item(cL,ncL,ftype,"float",fp,verbose)) != 0)
    err("Failed to write cL\n");
  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("zvalues",(int)strlen("zvalues"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write zvalues\n");
  if((stat = write_item("array",(int)strlen("array"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write array\n");
  if((stat = write_item("byte",(int)strlen("byte"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write byte\n");
  if((stat = write_item("splitEnz",(int)strlen("splitEnz"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write splitEnz\n");
  j = ntraces * (nlayers + 1);
  if((stat = write_item(&j,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write j\n");
  if((stat = write_item(splits,j,ftype,"unsigned char",fp,verbose)) != 0)
    err("Failed to write splits\n");
  if((stat = write_item("array",(int)strlen("array"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write array\n");
  if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write float\n");
  if((stat = write_item("data",(int)strlen("data"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write data\n");
  if((stat = write_item(&j,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write j\n");
  if((stat = write_item(zvals,j,ftype,"float",fp,verbose)) != 0)
    err("Failed to write zvals\n");
  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("active",(int)strlen("active"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write active\n");
  if((stat = write_item("array",(int)strlen("array"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write array\n");
  if((stat = write_item("bool",(int)strlen("bool"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write bool\n");
  if((stat = write_item("data",(int)strlen("data"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write data\n");
  j = nx * ny * nlayers;
  if((stat = write_item(&j,1,ftype,"int",fp,verbose)) != 0)
    err("Failed to write j\n",stat);
  if((stat = write_item(active,j,ftype,"unsigned char",fp,verbose)) != 0)
    err("Failed to write active\n");
  /* loop thru pdata, write properties */
  for(i=0; i<nprop; i++) {
    if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write endtag\n");
    if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write tag\n");
    if((stat = write_item("parameter",(int)strlen("parameter"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write parameter\n");
    if((stat = write_item("char",(int)strlen("char"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write char\n");
    if((stat = write_item("name",(int)strlen("name"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write name\n");
    strcpy(string,prop[i]);
    if((stat = write_item(string,(int)strlen(string),ftype,"char",fp,verbose)) != 0)
      err("Failed to write string\n");
    if((stat = write_item("array",(int)strlen("array"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write array\n");
    if((stat = write_item("float",(int)strlen("float"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write float\n");
    if((stat = write_item("data",(int)strlen("data"),ftype,"char",fp,verbose)) != 0)
      err("Failed to write data\n");
    if((stat = write_item(&j,1,ftype,"int",fp,verbose)) != 0)
      err("Failed to write j\n",stat);
    if((stat = write_item(&pdata[i][0],j,ftype,"float",fp,verbose)) != 0)
      err("Failed to write pdata\n");
  }

  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");
  if((stat = write_item("tag",(int)strlen("tag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write tag\n");
  if((stat = write_item("eof",(int)strlen("eof"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write eof\n");
  if((stat = write_item("endtag",(int)strlen("endtag"),ftype,"char",fp,verbose)) != 0)
    err("Failed to write endtag\n");

  fclose(fp);

  return EXIT_SUCCESS;

}

int write_item(void *buf, int nitems, char *ftype, char *dtype, FILE *fp, int verbose)
{

  int i, nw;
  int *ibuf;

  float *fbuf;

  /* binary file */
  if(!strcmp(ftype,"bin")) {
    if(!strcmp(dtype,"char")) {
      if((nw = fwrite((char *)buf,sizeof(char),nitems+1,fp)) != nitems+1)
        return nw;
    }
    else if(!strcmp(dtype,"unsigned char")) {
      if((nw = fwrite((char *)buf,sizeof(char),nitems,fp)) != nitems)
        return nw;
    }
    else if(!strcmp(dtype,"int")) {
      if((nw = fwrite((int *)buf,sizeof(int),nitems,fp)) != nitems)
        return nw;
    }
    else if(!strcmp(dtype,"float")) {
      if((nw = fwrite((float *)buf,sizeof(float),nitems,fp)) != nitems)
        return nw;
    }
    return 0;
  }
  /* ascii */
  else {
    if(!strcmp(dtype,"char")) {
      if((nw = fprintf(fp,"%s",(char *)buf)) < 0)
        return nw;
    }
    else if(!strcmp(dtype,"unsigned char")) {
      if((nw = fprintf(fp,"%s",(char *)buf)) < 0)
        return nw;
    }
    else if(!strcmp(dtype,"int")) {
      ibuf = buf;
      for(i=0; i<nitems; i++) {
        if((nw = fprintf(fp,"%8d ",ibuf[i])) < 0)
          return nw;
      }
    }
    else if(!strcmp(dtype,"float")) {
      fbuf = buf;
      for(i=0; i<nitems; i++) {
        if((nw = fprintf(fp,"%10.2f ",fbuf[i])) < 0)
          return nw;
      }
    }
    return 0;
  }

}
