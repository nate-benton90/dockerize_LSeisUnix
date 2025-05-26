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
#include "bhpio.h"
#include "bhpiocube.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"  bhptranspose filename=                                          ",
"                                                                  ",
"  bhptranspose makes a transposed version of a BHPIO gridded dataset",
"                                                                  ",
"  Required Parameters:                                            ",
"  filename=          Name of the BHPIO data set to be transposed  ",
"                                                                  ",
"  Optional Parameters:                                            ",
"  pathlist='filename'.dat                                         ",
"                     If pathlist is unspecified, the pathlist file",
"                     for the data to be transposed is assumed to be",
"                     in the directory from which bhptranspose is  ",
"                     launched. Use tpathlist to specify a different",
"                     pathlist file for the transposed data.       ",
"  tpathlist='pathlist'                                            ",
"                     If tpathlist is unspecified, pathlist is used",
"                     to determine where to write transposed output.",
"                     To write the transpsed data to a different   ",
"                     set of directories, specify tpathlist=...    ",
"                     To view the transposed data with BHPVIEW, you",
"                     will need a pathlist file named              ",
"                     'filename'_transpose_'key'.dat",
"  pkey=tracl         Primary key  of transposed data              ",
"                     Header tracl is used to store the old vertical",
"                     key values, e.g. times or depths             ",
"                     Other keys are moved down one slot, and the  ",
"                     last key becomes the new vertical key        ",
"  endian=2           endianness of transposed data. Use 0=LITTLE  ",
"                     1=BIG, or 2=native, with 2 being the default ",
"  verbose=0          1=debug print                                ",
"                                                                  ",
"  The transposed dataset is automatically named                   ",
"    'filename'_transpose_'key', where key is the new secondary key",
"                                                                  ",
"                                                                  ",
NULL};

int main(int argc, char **argv)
{

  char *path1;           /* first path in pathlist */
  char fpath[256];       /* File header path */
  char cmdbuf[BUFSIZ];   /* UNIX commands */
  char *filename;        /* swapped filename = filename'_transpose' */
  char string[256];      /* scratch */
  char *pathlist;        /* pathlist, if specified, else NULL */
  char *tpathlist;       /* pathlist for transposed data, if specified, else pathlist */
  char *pkey;            /* new primary key */
  char **key_name;       /* output keys */

  int verbose;           /* debug printout */
  int i;
#ifdef CWP_BIG_ENDIAN
  int my_endian = 1;
#else
  int my_endian = 0;
#endif
  int nkeys;             /* number of keys in output data */
  int *key_num;          /* number bins for keys */
  int *key_min;          /* min values */
  int *key_max;          /* max values */
  int *key_incr;         /* inc values */
  int endian;            /* endian parm for bhpwritecube 1=BIG,0=LITTLE,2=Native */

  FILE *fp1;             /* 1st path pointer */
  FILE *pipefp;          /* UNIX commands */
 
  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* endian */
  if(!getparint("endian",&endian))
    endian = my_endian;
  if(endian == 2)
    endian = my_endian;
  if(endian < 0 || endian > 2)
    err("%d is illegal endian value, use 0(LITTLE), 1(BIG), or 2(Native)\n",endian);

  /* filename is required */
  if(!getparstring("filename",&file_hdr.filename))
    err("Filename is required\n");

  /* get path to first partition */
  path1 = get_path1(file_hdr.filename,verbose);

  /* Make sure file is there */
  if(!get_bhpio_path(path1,file_hdr.filename,"_0000.HDR",fpath,verbose))
    err("FILE: path=%s, filename=%s does not exist\n",path1,file_hdr.filename);
  /* Get file header */
  read_file_hdr(fpath,verbose);

  /* verify gridded format */
  if(!get_bhpio_path(path1,file_hdr.filename,"_LOCK.HDR",fpath,verbose))
    err("Only gridded datasets can be transposed\n");

  /* pkey */
  if(!getparstring("pkey",&pkey))
    pkey = "tracl";

  if(verbose) {
    fprintf(stderr,"First Path %s\n", path1);
    fprintf(stderr,"File Name from File Header %s\n", file_hdr.filename);
    fprintf(stderr,"Number of Keys from File Header %d\n", file_hdr.nkeys);
    if(endian == 0)
      fprintf(stderr,"Writing LITTLE endian\n");
    else if(endian == 1)
      fprintf(stderr,"Writing BIG endian\n");
    else if(endian == 2)
      fprintf(stderr,"Writing native endian\n");
  }

  /* hdr_limits */
  hdr_limits = calloc(file_hdr.nkeys,sizeof(bhp_hdr_limits));
  if(!get_bhpio_path(path1,file_hdr.filename,"_0000.0001.HDR",fpath,verbose))
    err("Could not access file: %s\n", fpath);
  /* Load hdr_limits */
  if(read_hdr_limits(fpath,verbose))
    err("Error reading %s\n", fpath);

  /* alloc output keys */
  nkeys = file_hdr.nkeys + 1;
  key_name = calloc(nkeys,sizeof(char *));
  key_min = calloc(nkeys,sizeof(int));
  key_max = calloc(nkeys,sizeof(int));
  key_incr = calloc(nkeys,sizeof(int));
  key_num = calloc(nkeys,sizeof(int));
  for(i=0; i<nkeys; i++)
    key_name[i] = calloc(16,sizeof(char));

  strcpy(key_name[0],pkey);
  for(i=1; i<nkeys; i++) {
    strcpy(key_name[i],hdr_limits[i-1].bhp_hdr_name);
    key_min[i] = hdr_limits[i-1].bhp_hdr_min;
    key_max[i] = hdr_limits[i-1].bhp_hdr_max;
    key_incr[i] = hdr_limits[i-1].bhp_hdr_inc;
    key_num[i] = (hdr_limits[i-1].bhp_hdr_max - hdr_limits[i-1].bhp_hdr_min
                 + hdr_limits[i-1].bhp_hdr_inc) / hdr_limits[i-1].bhp_hdr_inc;
  }

  /* new keys */
  if(verbose) {
    fprintf(stderr,"Keys for TRANSPOSED Data:\n");
    for(i=0; i<nkeys; i++)
      fprintf(stderr,"  %s\n",key_name[i]);
    fprintf(stderr,"\n");
  }

  /* output filename is input + '_transpose_skey' */
  filename = calloc(strlen(file_hdr.filename)+strlen(pkey)+12,sizeof(char));
  strcpy(filename,file_hdr.filename);
  strcat(filename,"_transpose_");
  strcat(filename,hdr_limits[0].bhp_hdr_name);
  if(verbose)
    fprintf(stderr,"Transposed filename: %s\n",filename);

  /* get nsamp, etc from first trace */
  strcpy(fpath,filesys);
  /* if path is a dir, append filename, else filename is already appended */
  if(isadir(fpath) == 1) {
    if(verbose)
      fprintf(stderr,"%s is a directory, appending filename\n",fpath);
    strcat(fpath,"/");
    strcat(fpath,file_hdr.filename);
    strcat(fpath,"_0001.su");
  }
  else {
    if(verbose)
      fprintf(stderr,"%s is a file, not appending filename\n",fpath);
  }
  if(verbose)
    fprintf(stderr,"Opening %s\n",fpath);
  if(!(fp1 = fopen(fpath,"r")))
    err("Cannot open %s\n",fpath);
  efread(&tr,HDRBYTES,1,fp1);
  fclose(fp1);
  /* check endianness */
  if(file_hdr.endian != my_endian) {
    swap_short_2(&tr.ns);
    swap_short_2(&tr.dt);
    swap_short_2(&tr.delrt);
  }
  if(verbose)
    fprintf(stderr,"ns,dt,delrt from first trace %d %d %d\n",tr.ns,tr.dt,tr.delrt);
  /* fill in values for primary as follows:
     nprop=0 --> min=delrt,inc=dt,max=delrt+dt*(ns-1),num=max
     nprop>0 --> min=1,inc=1,max=(ns-1)/(nprop-1),num=max
     nprop<0 --> min=1,inc=1,max=-nprop,num=max */
  if(file_hdr.nprop == 0) {
    key_min[0] = tr.delrt;
    key_max[0] = tr.delrt + tr.dt * 0.001 * (tr.ns - 1);
    key_incr[0] = tr.dt * 0.001;
    key_num[0] = tr.ns;
  }
  else if(file_hdr.nprop > 0) {
    key_min[0] = 1;
    key_max[0] = file_hdr.nprop * (tr.ns - 1) / (file_hdr.nprop + 1);
    key_incr[0] = 1;
    key_num[0] = key_max[0];
  }
  else if(file_hdr.nprop < 0) {
    key_min[0] = 1;
    key_max[0] = -file_hdr.nprop;
    key_incr[0] = 1;
    key_num[0] = key_max[0];
  }

  /* build script to do transpose */
  /* keylist for bhpread */
  strcpy(string,hdr_limits[0].bhp_hdr_name);
  for(i=1; i<file_hdr.nkeys; i++) {
    strcat(string,",");
    strcat(string,hdr_limits[i].bhp_hdr_name);
  }
  /* bhpread */
  sprintf(cmdbuf,"bhpreadcube verbose=%d filename=%s keys=%s ",
          verbose,file_hdr.filename,string);
  /* pathlist */
  if(!getparstring("pathlist",&pathlist))
    pathlist = NULL;
  if(pathlist != NULL)
    sprintf(string,"pathlist=%s | ",pathlist);
  else
    sprintf(string," | ");
  strcat(cmdbuf,string);
  /* bhpswap */
  sprintf(string,"bhpswap verbose=%d pkey=%s skey=%s,%d,%d nxmax=%d nprop=%d vkey=%s | \n",
          verbose,hdr_limits[file_hdr.nkeys-2].bhp_hdr_name,key_name[nkeys-1],key_min[nkeys-1],
          key_incr[nkeys-1],key_num[nkeys-1],file_hdr.nprop,key_name[0]);
  strcat(cmdbuf,string);
  /* bhpwritecube */
  sprintf(string,"bhpwritecube verbose=%d filename=%s init=yes transpose=yes endian=%d ",
          verbose,filename,endian);
  strcat(cmdbuf,string);
  for(i=0; i<nkeys; i++) {
    sprintf(string,"key%1d=%s,%d,%d,%d ",i+1,key_name[i],key_min[i],key_incr[i],key_num[i]);
    strcat(cmdbuf,string);
  }

  /* tpathlist if specified */
  if(!getparstring("tpathlist",&tpathlist))
    tpathlist = pathlist;
  if(tpathlist == NULL) {
    sprintf(string,"pathlist=");
    strcat(string,file_hdr.filename);
    strcat(string,".dat");
  }
  else
    sprintf(string,"pathlist=%s",tpathlist);
  strcat(cmdbuf,string);

  /* if horizons or properties are present, copy them across */
  if(file_hdr.nprop > 0) {
    strcat(cmdbuf," properties=");
    for(i=0; i<file_hdr.nprop-1; i++) {
      sprintf(string,"%s,",properties[i]);
      strcat(cmdbuf,string);
    }
    sprintf(string,"%s",properties[file_hdr.nprop-1]);
    strcat(cmdbuf,string);
    if(verbose)
      fprintf(stderr,"Copied %d properties\n",file_hdr.nprop);
  }
  else if(file_hdr.nprop < 0) {
    strcat(cmdbuf," horizons=");
    for(i=0; i<-file_hdr.nprop-1; i++) {
      sprintf(string,"%s,",properties[i]);
      strcat(cmdbuf,string);
    }
    sprintf(string,"%s",properties[-file_hdr.nprop-1]);
    strcat(cmdbuf,string);
    if(verbose)
      fprintf(stderr,"Copied %d horizons\n",-file_hdr.nprop);
  }
  if(verbose)
    fprintf(stderr,"Executing %s\n",cmdbuf);
  pipefp = epopen(cmdbuf,"w");
  epclose(pipefp);

  return EXIT_SUCCESS;

}
