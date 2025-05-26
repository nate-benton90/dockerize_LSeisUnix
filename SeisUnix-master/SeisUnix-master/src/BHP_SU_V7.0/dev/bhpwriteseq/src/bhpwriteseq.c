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
* KEYWORDS:  $RCSfile: bhpwriteseq.c,v $
*            $Revision: 1.12 $
*            $Date: 2003/04/01 22:16:16 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpwriteseq.c,v $
* Revision 1.12  2003/04/01 22:16:16  ahmilb
* New properties type.
*
* Revision 1.11  2003/02/24 17:39:05  ahmilb
* Add shutdown code.
*
* Revision 1.10  2003/01/14 19:24:26  ahmilb
* Replace hard-coded lengths with NAMELEN, NFILES.
* Add isadir function.
*
* Revision 1.9  2002/11/08 15:46:19  ahmilb
* Abort if dt or ns changes in input data.
* Separate stats for horizons.
*
* Revision 1.8  2002/08/06 19:56:30  ahmilb
* Implement common file-header.
* Change default stdin_endian to native.
* Separate stats for each property in model data.
* Vertical key info as floats.
*
* Revision 1.7  2002/05/08 14:16:53  ahmilb
* Added stdin_endian, units parameters.
* Delete trace stats calculation.
*
* Revision 1.6  2002/01/29 16:01:08  ahmilb
* Enforce maximum of 1 million traces per partition.
*
* Revision 1.5  2001/10/30 18:50:46  ahmilb
* Change check_endian arglist.
*
* Revision 1.4  2001/10/12 20:14:44  ahmilb
* Move write_file_hdr code to bhpioseq_lib.
* Fix problem with allocation of data_type string.
*
* Revision 1.3  2001/09/14 18:25:15  ahmilb
* Use check_endian function in bhpio_lib.
* Delete obselete bhp_filesys code.
*
* Revision 1.2  2001/09/13 19:14:11  ahmilb
* Add HORIZONS code.
*
* Revision 1.1  2001/07/25 18:26:26  ahmilb
* Changed oldbhpwrite to bhpwriteseq.
* Changed datadir.dat to 'filename'.dat.
*
* Revision 1.1  2001/07/25 18:15:56  ahmilb
* Changed oldbhpwrite to bhpwriteseq.
* Changed datadir.dat to 'filename'.dat.
*
*
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include "bhpio.h"
#include "bhpioseq.h"
#include "cwp.h"
#include <signal.h>
#include <assert.h>

#define MAX_KEY_VALS MAX_TRACES 

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
" bhpwriteseq < stdin filename=fname [optional parameters]         ",
"                                                                  ",
" bhpwriteseq writes a BHPIO \"sequential\" dataset, which can be  ",
" randomly accessed by bhpread. The orders in which bhpread can read",
" the dataset are defined by up to 5 trace-header keys which are   ",
" saved when bhpwriteseq creates the dataset. The sequential term  ",
" refers to the way in which the trace-header key index is written.",
" The user does not have to have any knowledge about the trace-header",
" values in the data. Contrast with bhpwritecube, which requires   ",
" the user to define a n-dimensional cube for holding trace header ",
" values. bhpwriteseq is useful when the limits of the data being  ",
" processed are not well-defined. However, bhpwritecube is         ",
" significantly more efficient and uses less system resources than ",
" bhpwriteseq.                                                     ",
"                                                                  ",
" Required Parameters:                                             ",
"  filename=fname        File name                                 ", 
"      Complete filenames are formed as path/filename_part-num     ",
"      where part-num is a 4-digit partition number                ",
"      EXAMPLE:                                                    ",
"        Suppose pathlist=paths.dat, filename=offset-gathers,      ",
"        and paths.dat consists of:                                ",
"        /data/D_170_005                                           ",
"        /data/D_169_001                                           ",
"        /data/D_169_003                                           ",
"        Filenames will be /data/D_170_005/offset-gathers_0001,    ",
"                          /data/D_169_001/offset-gathers_0002,    ",
"                          /data/D_169_003/offset-gathers_0003,    ",
"                          /data/D_170_005/offset-gathers_0004,    ",
"        ... until all data are written                            ",
" Optional Parameters:                                             ",
"    pathlist='filename'.dat  ASCII file containing list of paths  ",
"                             to use                               ",
"    verbose=0                For debug print                      ",
"    stripe=[yes,no]          Whether to create multiple partitions",
"    size=1999                Size of each file partition(stripe=yes)",
"    key1=fldr,R,1            First key, type, increment           ",
"    key2=offset,I,50         Second key, type, nominal increment  ",
"    key3=...                 Third key, type, increment           ",
"    key4=...                 Fourth key, type, increment          ",
"    key5=...                 Fifth key, type, increment           ",
"    properties=...           List of ROFF properties              ",
"                             If the input data were created by    ",
"                             bhproffread, properties should be a  ",
"                             list of the properties which         ",
"                             were extracted                       ",
"                                                                  ",
"    horizons=...             List of horizons, if input data were ",
"                             created by bhphorizon.               ",
"    stdin_endian=2           Default endianness for data from stdin is NATIVE,",
"                             where NATIVE is the endianness of the platform",
"                             on which bhpwriteseq is running",
"                             Specify stdin_endian=0 to read LITTLE_ENDIAN from stdin",
"                             Specify stdin_endian=1 to read BIG_ENDIAN from stdin",
"    endian=native            Omit endian parameter to write native",
"                             Specify endian=1 to force BIG_ENDIAN ",
"                             Specify endian=0 to force LITTLE_ENDIAN",
"    units=                   0=Seconds, 1=Feet, 2=Meters. If unspecified, the first trace",
"                             is checked. If dt/1000 > 8, units=2 is used.",
"                             If dt/1000 <= 8, units=0 is used. The units value is saved",
"                             in the header file for bhpread to use if required.",
" EXAMPLE: Write 3D gathers, saving line, trace, and offset header ",
"          keys.                                                   ",
" segyread tape=gathers.sgy |                                      ",
" bhpwriteseq filename=lines3014-3025 key1=ep,R,1 key2=cdp,R,1 \\  ",
"   key3=offset,I,50                                               ",
" Contents of lines3014-3025.dat:                                  ",
"  /hou/data/D_169_005/trinidad                                    ",
"  /hou/data/D_170_003/trinidad                                    ",
NULL};

/* Globals */
FILE *fp;                              /* Current open file pointer */
bhp_hdr_limits *global_hdr_limits;     /* File-wide hdr_limits */
int **global_vlen;                     /* Length of key-vectors for each partition */
int **global_vloc;                     /* Location of key-vectors for each partition */
 
#ifdef CWP_BIG_ENDIAN
  int my_endian = 1;
#else
  int my_endian = 0;
#endif

/* Prototypes */
int bhp_open_write(int npaths, char **path, char *name, int striping, int size,
                   int verbose, int index, int *next);
int *build_key_vector(float *keys, int count, int part, int *len, int ndx, int verbose);
int write_hdr_limits(char *path, int verbose);
int write_file_hdr(char *path, int verbose);
void quick_sort(int *sortv, int *index, int m);
int bhp_next_path(char **path, int npaths, int size, int verbose, int *next);

int main(int argc, char **argv)
{

  char *stripe;           /* yes/no  */
  char fpath[NAMELEN];    /* Partiiton zero path name */
  char lpath[NAMELEN];    /* Header limits path */
  char vpath[NAMELEN];    /* Key vector path */
  char *data_type;        /* Header key data type */
  char cindex[8];         /* Use to append file index to path */
  char *pathlist;         /* File containing user-specified path-list for forming filenames */
  char **path;            /* Paths from pathlist */
  char errmsg[128];       /* error messages */

  int verbose;            /* debug printout */
  int i, j, k, m, stat;   /* Loop counters, status */
  int file_index;         /* npaths counter */
  int next_filesys;       /* index into pathlist of next partition */
  int ikey;               /* key counter */
  int bytes_per_trace;    /* Bytes per trace+hdr */
  int trace_count;        /* Trace counter */
  int bytes_per_part;     /* Bytes per partition */
  int key_incr[5];        /* key1 - key5 incrs */
  int key_index[5];       /* Header key indices */
  int *key_vector_len;    /* Length of each key-vector */
  int loc;                /* Key vector byte address */
  int gottrace=1;         /* 0=EOF */
  int numg;               /* Number of key occurences - global */
  int *slot;              /* 1=this partition contributed to current pass of global build */
  int *vndx;              /* Current location in each key vector */
  int gndx;               /* Current location in global key vector */
  int *index;             /* Sort work array for global vector */
  int gvlen;              /* Length of global vector */
  int vlen;               /* Length of local key vector */
  int striping;           /* 1=yes, 0=no */
  int npaths;             /* Number of user-specified paths */
  int nrealloc=2;         /* Multiplier for expanding key_vals, for single partition case */
  int **key_vector;       /* Sorted keys, disk addrs - per partition */
  int *tempv;             /* Array for building global vectors */
  int *sortv;             /* Sort work array for global vector */
  int min;                /* For building global vector */
  int endian;             /* endian parameter 1=BIG, 0=LITTLE */
  int stdin_endian;       /* endianness of stdin */
  int nlayers;            /* layers per property for model data */
  int nvklist=0;          /* nvklist needed for trace_stats */
  int *iprop=NULL;        /* sample index of each property in model */
  int *last_key_vals;     /* previous trace key values */
  int abort;              /* flag to signal shutdown */

  short dt;               /* dt from first trace */

  float **key_vals;       /* Header key values as written */

  FILE *vfp;              /* Pointer to vector file */
  FILE *vfpg;             /* Pointer to global vector file */

  cwp_String key_name[5];   /* key1 - key5 names */
  cwp_String key_type[5];   /* key1 - key5 types */
  cwp_String key_string[3]; /* Temporary key,type,incr holder */

  Value hval;      /* Header values */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* Filename is required */
  if(!getparstring("filename",&file_hdr.filename))
    err("Filename is required\n");

  npaths = 0;
  /* Get pathlist */
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc(strlen(file_hdr.filename)+5,sizeof(char));
    strcpy(pathlist,file_hdr.filename);
    strcat(pathlist,".dat");
  }
  if(verbose)
    fprintf(stderr,"Opening %s\n", pathlist);
  if(!(fp = fopen(pathlist,"r")))
    err("pathlist specified, but cannot open %s\n", pathlist); 
  else {
    /* Count number of paths in pathlist */
    while(fgets(fpath,255,fp) != NULL)
      npaths++;
    rewind(fp);
    /* Allocate paths and load from pathlist */
    path = calloc(npaths,sizeof(char *));
    for(i=0; i<npaths; i++)
      path[i] = calloc(NAMELEN,sizeof(char));
    for(i=0; i<npaths; i++) {
      fscanf(fp,"%s\n", fpath);
      if(!strcmp(fpath,"."))
        path[i] = getenv("PWD");
      else if(!strcmp(fpath,".."))
        err(".. in pathlist is illegal; use explicit path\n");
      else
        strcpy(path[i],fpath);
    }
    fclose(fp);
  }
  if(!npaths)
    err("File %s is empty\n", pathlist);

  /* Set flag in file header */
  if(verbose)
    fprintf(stderr,"%d paths in pathlist\n", npaths);
  file_hdr.npaths = npaths;

  /* Initialize number of partiitons */
  file_hdr.nparts = 1;

  /* Striping ? */
  if(!getparstring("stripe",&stripe))
    striping = 1;
  else {
    if(strstr(stripe,"yes"))
      striping = 1;
    else
      striping = 0;
  }

  /* If striping, get size parameter */
  if(striping) {
    /* Set size to default if unpecified  and striping=yes */
    if(!getparint("size",&file_hdr.size))
      file_hdr.size = BHP_SIZE;
    else
      getparint("size",&file_hdr.size);
    if(file_hdr.size <= 0)
      err("size = %d is illegal\n", file_hdr.size);
  }
  else
    file_hdr.size =0;

  /* data_order for sequential is always cross-section */
  file_hdr.data_order = 1;

  /* Check key1,key2,.. */
  file_hdr.nkeys = 0;
  if(getparstringarray("key1",key_string) && (i = countparval("key1")) != 3)
    err("key1 should be name,typr,incr , is %s,%s,%s\n", key_string[0],key_string[1],key_string[2]);
  else if(getparstringarray("key1",key_string)) {
    key_name[0] = key_string[0];
    key_type[0] = key_string[1];
    key_incr[0] = atoi(key_string[2]);
    if(!key_incr[0])
      err("key1 increment should be an integer, is %s\n",key_string[2]);
    file_hdr.nkeys++;
  }
  if(getparstringarray("key2",key_string) && (i = countparval("key2")) != 3)
    err("key2 should be name,typr,incr , is %s,%s,%s\n", key_string[0],key_string[1],key_string[2]);
  else if(getparstringarray("key2",key_string)) {
    if(file_hdr.nkeys != 1)
      err("key2 cannot be specified unless key1 is specified\n");
    key_name[1] = key_string[0];
    key_type[1] = key_string[1];
    key_incr[1] = atoi(key_string[2]);
    if(!key_incr[1])
      err("key2 increment should be an integer, is %s\n",key_string[2]);
    file_hdr.nkeys++;
  }
  if(getparstringarray("key3",key_string) && (i = countparval("key3")) != 3)
    err("key3 should be name,typr,incr , is %s,%s,%s\n", key_string[0],key_string[1],key_string[2]);
  else if(getparstringarray("key3",key_string)) {
    if(file_hdr.nkeys != 2)
      err("key3 cannot be specified unless key1,key2 are specified\n");
    key_name[2] = key_string[0];
    key_type[2] = key_string[1];
    key_incr[2] = atoi(key_string[2]);
    if(!key_incr[2])
      err("key3 increment should be an integer, is %s\n",key_string[2]);
    file_hdr.nkeys++;
  }
  if(getparstringarray("key4",key_string) && (i = countparval("key4")) != 3)
    err("key4 should be name,typr,incr , is %s,%s,%s\n", key_string[0],key_string[1],key_string[2]);
  else if(getparstringarray("key4",key_string)) {
    if(file_hdr.nkeys != 3)
      err("key4 cannot be specified unless key1,key2,key3 are specified\n");
    key_name[3] = key_string[0];
    key_type[3] = key_string[1];
    key_incr[3] = atoi(key_string[2]);
    if(!key_incr[3])
      err("key4 increment should be an integer, is %s\n",key_string[2]);
    file_hdr.nkeys++;
  }
  if(getparstringarray("key5",key_string) && (i = countparval("key5")) != 3)
    err("key5 should be name,typr,incr , is %s,%s,%s\n", key_string[0],key_string[1],key_string[2]);
  else if(getparstringarray("key5",key_string)) {
    if(file_hdr.nkeys != 4)
      err("key5 cannot be specified unless key1,key2,key3,key4 are specified\n");
    key_name[4] = key_string[0];
    key_type[4] = key_string[1];
    key_incr[4] = atoi(key_string[2]);
    if(!key_incr[4])
      err("key5 increment should be an integer, is %s\n",key_string[2]);
    file_hdr.nkeys++;
  }
    
  /* endian */
  if(!getparint("endian",&endian)) {
    file_hdr.endian = my_endian;        /* native */
    if(verbose)
      fprintf(stderr,"Writing native format\n");
  }
  else if(endian == 0 || endian == 1) {
    file_hdr.endian = endian;
    if(verbose) {
      if(endian)
        fprintf(stderr,"Force BIG_ENDIAN format\n");
      else
        fprintf(stderr,"Force LITTLE_ENDIAN format\n");
    }
  }
  else {
    fprintf(stderr,"endian = %d\n", endian);
    err("If endian is specified, it must be 0 or 1\n");
  }

  /* endianness of stdin (via gettr) defaults to native */
  if(!getparint("stdin_endian",&stdin_endian))
    stdin_endian = 2;
  if(verbose) {
    if(stdin_endian == 1)
      fprintf(stderr,"Reading BIG endian from stdin\n");
    else if(stdin_endian == 0)
      fprintf(stderr,"Reading LITTLE endian from stdin\n");
    else if(stdin_endian == 2) {
      fprintf(stderr,"Reading NATIVE endian from stdin\n");
    }
    else
      err("%d is illegal value for stdin_endian\n",stdin_endian);
  }
  /* if reading native, set to my_endian */
  if(stdin_endian == 2)
    stdin_endian = my_endian;

  /* can't have properties AND horizons */
  if(countparval("properties") > 0 && countparval("horizons") > 0)
      err("Can't have both properties and horizons\n");

  /* load properties or horizons */
  if(countparval("properties") > 0) {
    file_hdr.nprop = countparval("properties");
    properties = calloc(file_hdr.nprop,sizeof(char *));
    getparstringarray("properties",properties);
    file_hdr.data_type = 3;
  }
  else if(countparval("horizons") > 0) {
    file_hdr.nprop = countparval("horizons");
    properties = calloc(file_hdr.nprop,sizeof(char *));
    getparstringarray("horizons",properties);
    file_hdr.nprop = -file_hdr.nprop;
    file_hdr.data_type = 2;
  }
  else {
    file_hdr.nprop = 0;
    file_hdr.data_type = 1;
  }

  /* Fill in header limits */
  if(file_hdr.nkeys) {
    hdr_limits = calloc(file_hdr.nkeys,sizeof(bhp_hdr_limits));
    global_hdr_limits = calloc(file_hdr.nkeys,sizeof(bhp_hdr_limits));
  }
  if(verbose)
    fprintf(stderr,"Allocated %d bytes for hdr_limits\n", file_hdr.nkeys*sizeof(bhp_hdr_limits));
  for(i=0; i<file_hdr.nkeys; i++) {
    strcpy(hdr_limits[i].bhp_hdr_name,key_name[i]);
    strcpy(global_hdr_limits[i].bhp_hdr_name,key_name[i]);
    hdr_limits[i].bhp_hdr_inc = key_incr[i];
    hdr_limits[i].bhp_hdr_scalar = 1;
    global_hdr_limits[i].bhp_hdr_scalar = 1;
    global_hdr_limits[i].bhp_hdr_inc = key_incr[i];
    if(strcmp(key_type[i],"R") && strcmp(key_type[i],"I"))
      err("Type for key= %s, should be R or I, is %s\n", key_name[i],key_type[i]);
    if(!strcmp(key_type[i],"R")) {
      hdr_limits[i].bhp_hdr_type = 1;
      global_hdr_limits[i].bhp_hdr_type = 1;
    }
    else if(!strcmp(key_type[i],"I")) {
      hdr_limits[i].bhp_hdr_type = 0;
      global_hdr_limits[i].bhp_hdr_type = 0;
    }
    /* Data Type */
    data_type = hdtype(hdr_limits[i].bhp_hdr_name);
    strcpy(hdr_limits[i].bhp_hdr_data,data_type);
    strcpy(global_hdr_limits[i].bhp_hdr_data,data_type);
    key_index[i] = getindex(hdr_limits[i].bhp_hdr_name);
    global_hdr_limits[i].bhp_hdr_min = INT_MAX;
    global_hdr_limits[i].bhp_hdr_max = INT_MIN;
  }

  if(verbose) {
    fprintf(stderr,"Filename: %s\n", file_hdr.filename);
    fprintf(stderr,"Paths:\n");
    for(i=0; i<npaths; i++)
      fprintf(stderr,"    %s\n", path[i]);
    if(striping)
      fprintf(stderr,"Number of partitions: Filesize divided by %d MB\n", file_hdr.size);
    else
      fprintf(stderr,"Number of partitions: 1\n");
    if(file_hdr.size)
      fprintf(stderr,"Partition Size: %d MB\n", file_hdr.size);
    else
      fprintf(stderr,"Partition Size: Size of Dataset\n");
    fprintf(stderr,"Headers to Save:\n");
    for(i=0; i<file_hdr.nkeys; i++)
      fprintf(stderr,"    Key: %s, Incr: %d, Type: %d, Data Type: %s\n",
              hdr_limits[i].bhp_hdr_name,hdr_limits[i].bhp_hdr_inc,
              hdr_limits[i].bhp_hdr_type,hdr_limits[i].bhp_hdr_data);
    if(file_hdr.nprop > 0) {
      fprintf(stderr,"Model Properties: %s ", properties[0]);
      for(i=1; i<file_hdr.nprop; i++)
        fprintf(stderr," %s ", properties[i]);
      fprintf(stderr,"\n");
    }
    else if(file_hdr.nprop < 0) {
      fprintf(stderr,"Horizons: %s ", properties[0]);
      for(i=1; i<-file_hdr.nprop; i++)
        fprintf(stderr," %s ", properties[i]);
      fprintf(stderr,"\n");
    }
  }

  /* See if file exists */
  if(get_bhpio_path(path[0],file_hdr.filename,"_0000.HDR",fpath,verbose)) {
    fprintf(stderr,"FILE: path=%s, filename=%s exists\n",path[0],file_hdr.filename);
    fprintf(stderr,"run 'bhpio pathlist=%s filename=%s delete=yes' to delete it\n",
            pathlist,file_hdr.filename);
    return(EXIT_FAILURE);
  }
  else
    if(verbose)
      fprintf(stderr,"File does not exist\n");

  /* Get first trace */
  if(!bhp_gettr(&tr,my_endian,stdin_endian))
    err("can't get first trace");
  file_hdr.nsamp = tr.ns;
  trace_count = 0;
  dt = tr.dt;

  /* check units parm */
  if(!getparint("units",&file_hdr.units)) {
    if(tr.dt/1000 > 8)
      file_hdr.units = 2;
    else
      file_hdr.units = 0;
  }
  if(file_hdr.units < 0 || file_hdr.units > 2)
    err("%d is illegal units value, use 0=seconds, 1=feet, or 2=meters\n",file_hdr.units);
  if(verbose)
    fprintf(stderr,"Setting units to %d, (0=seconds,1=feet,2=meters)\n",file_hdr.units);

  /* If any key is "scalable" save scalco */
  for(i=0; i<file_hdr.nkeys; i++)
    if(!strcmp(global_hdr_limits[i].bhp_hdr_name,"sx") ||
       !strcmp(global_hdr_limits[i].bhp_hdr_name,"sy") ||
       !strcmp(global_hdr_limits[i].bhp_hdr_name,"gx") ||
       !strcmp(global_hdr_limits[i].bhp_hdr_name,"gy"))
      global_hdr_limits[i].bhp_hdr_scalar = tr.scalco;
  
  /* Calculate traces per partition */
  bytes_per_trace = file_hdr.nsamp * sizeof(float) + 240;
  file_hdr.traces_per_part = (file_hdr.size * 1024 * 1024) / bytes_per_trace;
  if(file_hdr.traces_per_part > MAX_TRACES) {
    file_hdr.traces_per_part = MAX_TRACES;
    if(verbose)
      fprintf(stderr,"Traces per partition limited to %d\n",file_hdr.traces_per_part);
  }
  if(verbose) {
    if(striping)
      fprintf(stderr,"bytes_per_trace: %d, traces_per_part: %d\n", bytes_per_trace,
                                                                  file_hdr.traces_per_part);
    else
      fprintf(stderr,"bytes_per_trace: %d, traces_per_part: All traces in data\n", bytes_per_trace); 
  }

  /* Allow for up to NFILES partitions in filesystems list */
  filesys = calloc(NFILES*NAMELEN,sizeof(char));

  /* Open file */
  file_index = 1;
  next_filesys = 0;
  stat = bhp_open_write(npaths,path,file_hdr.filename,striping,
                        file_hdr.size,verbose,file_index,&next_filesys);
  /* Quit if open failed */
  if(stat != 0) {
    fprintf(stderr," Could not open file\n");
  }
 fprintf(stderr,"file_index=%d,file=%s\n",file_index,&filesys[(file_index-1)*NAMELEN]);

  /* Initialize key vectors */
  key_vector = calloc(file_hdr.nkeys,sizeof(int *));
  global_vlen = calloc(file_hdr.nkeys,sizeof(int *));
  global_vloc = calloc(file_hdr.nkeys,sizeof(int *));
  key_vector_len = calloc(file_hdr.nkeys,sizeof(int *));
  key_vals = calloc(file_hdr.nkeys,sizeof(float *));
  last_key_vals = calloc(file_hdr.nkeys,sizeof(int));
  for(i=0; i<file_hdr.nkeys; i++) {
    if(file_hdr.traces_per_part)
      key_vals[i] = calloc(file_hdr.traces_per_part,sizeof(float));
    else
      key_vals[i] = calloc(MAX_KEY_VALS,sizeof(float));
    global_vlen[i] = calloc(NFILES,sizeof(int));
    global_vloc[i] = calloc(NFILES,sizeof(int));
  }

  /* nlayers, nsamp for stats */
  file_hdr.nsamp = tr.ns;
  if(file_hdr.nprop <= 0) {
    nlayers = 0;
    stats.nsamp = file_hdr.nsamp;
  }
  /* cross-section model */
  else {
    nlayers = (file_hdr.nsamp - 1) / (file_hdr.nprop + 1);
    stats.nsamp = file_hdr.nsamp - (nlayers + 1);
  }
  /* Initialize stats */
  file_hdr.minval = FLT_MAX;
  file_hdr.maxval = -FLT_MAX;
  file_hdr.meanval = 0;
  file_hdr.rmsval = 0;
  /* single nvals if not prop or horz */
  j = 0;
  if(file_hdr.nprop == 0)
    stats.nvals = calloc(1,sizeof(int));
  /* if properties or horizons/events, set iprop, alloc prop_stats */
  else {
    j = (file_hdr.nprop > 0) ? file_hdr.nprop : -file_hdr.nprop;
    stats.nvals = calloc(j,sizeof(int));
    iprop = calloc(j,sizeof(int));
    file_hdr.prop_minval = calloc(j,sizeof(float));
    file_hdr.prop_maxval = calloc(j,sizeof(float));
    file_hdr.prop_meanval = calloc(j,sizeof(float));
    file_hdr.prop_rmsval = calloc(j,sizeof(float));
    for(i=0; i<j; i++) {
      if(file_hdr.nprop > 0)
        iprop[i] = i * nlayers;
      else
        iprop[i] = i + 1;
      file_hdr.prop_minval[i] = FLT_MAX;
      file_hdr.prop_maxval[i] = -FLT_MAX;
      file_hdr.prop_meanval[i] = 0;
      file_hdr.prop_rmsval[i] = 0;
    }
  }

  /* seismic vkey  */
  if(file_hdr.nprop == 0) {
    strcpy(file_hdr.vkey,"tracl");
    file_hdr.vmin = (float)tr.delrt;
    file_hdr.vinc = (float)tr.dt * 0.001;
    file_hdr.vmax = (float)tr.delrt + file_hdr.vinc * (tr.ns - 1);
  }
  /* model vkey */
  else if(file_hdr.nprop > 0) {
    strcpy(file_hdr.vkey,"tracl");
    file_hdr.vmin = 1.;
    file_hdr.vinc = 1.;
    file_hdr.vmax = (float)nlayers;
  }
  /* horizon vkey */
  else if(file_hdr.nprop < 0) {
    strcpy(file_hdr.vkey,"tracl");
    file_hdr.vmin = 1.;
    file_hdr.vinc = 1.;
    file_hdr.vmax = (float)tr.ns;
  }

  for(;;) {
    abort = 0;
    /* verify consistency of ns, dt */
    if(tr.dt != dt) {
      sprintf(errmsg,"dt from first trace = %d, dt from current trace is %d\n",dt,tr.dt);
      abort = 1;
    }
    if((int)tr.ns != file_hdr.nsamp) {
      sprintf(errmsg,"ns from file header = %d, ns from current trace is %d\n",file_hdr.nsamp,tr.ns);
      abort = 1;
    }
    if(abort == 1)
      printmsg(errmsg,key_name,last_key_vals,&filesys[(file_index-1)*NAMELEN],trace_count);
    else if(abort == 0) {
      /* Save keys */
      for(i=0; i<file_hdr.nkeys; i++) {
        gethval(&tr,key_index[i],&hval);
        key_vals[i][trace_count] = vtof(hdr_limits[i].bhp_hdr_data,hval);
      }
      /* Accumulate stats */
      trace_stats(&tr,nlayers,nvklist,file_hdr.nprop,iprop,0,verbose);
      /* Swap bytes if necessary */
      check_endian(my_endian,file_hdr.endian,&tr,(short)file_hdr.nsamp,verbose);
      efwrite(&tr,HDRBYTES,1,fp);
      efwrite(tr.data,FSIZE,file_hdr.nsamp,fp);
      /* save keys in case error on next trace */
      for(i=0; i<file_hdr.nkeys; i++)
        last_key_vals[i] = key_vals[i][trace_count];
      /* Check if key_vals is full */
      if(!file_hdr.traces_per_part) {
        if((trace_count) && (trace_count%MAX_KEY_VALS == 0)) {
          for(i=0; i<file_hdr.nkeys; i++) {
            key_vals[i] = (float *)realloc(key_vals[i],MAX_KEY_VALS*nrealloc*sizeof(float));
          }
          if(verbose)
            fprintf(stderr,"Expanded key_vals to %d\n", MAX_KEY_VALS*nrealloc);
          nrealloc++;
        }
      }
      trace_count++;
    }
    if(abort == 1 || !bhp_gettr(&tr,my_endian,stdin_endian))
      gottrace = 0;
    /* Check end of partition or end of data */
    if(trace_count == file_hdr.traces_per_part || gottrace == 0) {
      fprintf(stderr,"Wrote %d traces to file %s\n",
              trace_count,&filesys[(file_index-1)*NAMELEN]);
      efclose(fp);

      /* Build sorted key vectors */
      loc = 0;
      for(i=0; i<file_hdr.nkeys; i++) {
        key_vector[i] = build_key_vector(key_vals[i],trace_count,file_index-1,
                                         &key_vector_len[i],i,verbose);
        /* Set vector length in hdr_limits */
        hdr_limits[i].bhp_hdr_vlen = key_vector_len[i];
        hdr_limits[i].bhp_hdr_vloc = loc;
        /* Save vlen, vloc in global arrays */
        global_vlen[i][file_index-1] = key_vector_len[i];
        global_vloc[i][file_index-1] = loc;
        if(verbose) {
          fprintf(stderr,"Key Vector Byte Offset: %d, Number of Floats: %d\n", loc,key_vector_len[i]);
          fprintf(stderr,"Save loc in global_vloc, i,j = %d, %d\n", i,file_index-1);
        }
        loc += hdr_limits[i].bhp_hdr_vlen * sizeof(float);
/*
        for(j=0; j<key_vector_len[i]; j+=key_vector[i][j+1]+2) {
          fprintf(stderr,"Key Value: %d, Number: %d\n", key_vector[i][j],key_vector[i][j+1]);
          for(k=0; k<key_vector[i][j+1]; k++) {
            fprintf(stderr,"%d ", key_vector[i][j+2+k]);
            if(((k+1)%8) == 0)
              fprintf(stderr,"\n");
          }
          fprintf(stderr,"\n");
        }
*/
      }

      /* set unused file_hdr members to zero */
      file_hdr.bin = 0;
      file_hdr.cube_size = 0;
      /* calculate mean, rms */
      if(file_hdr.nprop == 0) {
        if(stats.nvals[0] > 0) {
          file_hdr.meanval /= stats.nvals[0];
          file_hdr.rmsval = sqrt(file_hdr.rmsval / stats.nvals[0]);
        }
      }
      else {
        j = (file_hdr.nprop > 0) ? file_hdr.nprop : -file_hdr.nprop;
        for(i=0; i<j; i++) {
          if(stats.nvals[i] > 0) {
            file_hdr.prop_meanval[i] /= stats.nvals[i];
            file_hdr.prop_rmsval[i] = sqrt(file_hdr.prop_rmsval[i] / stats.nvals[i]);
          }
        }
      }
      get_bhpio_path(path[0],file_hdr.filename,"_0000.HDR",fpath,verbose);
      /* Write file header */
      if(write_file_hdr(fpath,verbose))
          err("Failed to write file header\n");

      /* Set lpath  and write hdr_limits */
      strcpy(lpath,path[next_filesys]);
      strcat(lpath,"/");
      strcat(lpath,file_hdr.filename);
      strcat(lpath,"_");
      sprintf(cindex,"%04i",file_index);
      strcat(lpath,cindex);
      strcat(lpath,".0001.HDR");
      if(write_hdr_limits(lpath,verbose))
          err("Failed to write header limits\n");

      /* Set vpath  and write key vectors */
      strcpy(vpath,path[next_filesys]);
      strcat(vpath,"/");
      strcat(vpath,file_hdr.filename);
      strcat(vpath,"_");
      sprintf(cindex,"%04i",file_index);
      strcat(vpath,cindex);
      strcat(vpath,".0002.HDR");
      if(verbose)
        fprintf(stderr,"Opening %s\n", vpath);
      vfp = fopen(vpath,"w");
      if(!vfp)
        err("Failed to open key vectors %s\n", vpath);
      for(i=0; i<file_hdr.nkeys; i++)
        efwrite(key_vector[i],sizeof(int),key_vector_len[i],vfp);  
      efclose(vfp);

      /* Release key_vectors */
      for(i=0; i<file_hdr.nkeys; i++)
        free(key_vector[i]);

      /* If EOF, quit */
      if(!gottrace)
        break;

      /* Open next partition */
      file_hdr.nparts++;
      next_filesys++;
      if(next_filesys == npaths)
        next_filesys = 0;
      file_index++;
      stat = bhp_open_write(npaths,path,file_hdr.filename,striping,
                            file_hdr.size,verbose,file_index,&next_filesys);
      /* Quit if open failed */
      if(stat != 0) {
        file_hdr.nparts--;
        sprintf(errmsg,"Could not open next partition\n");
        printmsg(errmsg,key_name,last_key_vals,&filesys[(file_index-2)*NAMELEN],trace_count);
        break;
      }
      trace_count = 0;
 fprintf(stderr,"file_index=%d,file=%s\n",file_index,&filesys[(file_index-1)*NAMELEN]);
    }
  }

  /* Retrieve key vectors and merge into global key vector */
  vndx = calloc(file_hdr.nparts,sizeof(int));
  slot = calloc(file_hdr.nparts,sizeof(int));
  loc = 0;

  /* Release key_vector and re-allocate */
  free(key_vector);
  key_vector = calloc(file_hdr.nparts,sizeof(int *));

  /* Open global key vector file */
  get_bhpio_path(path[0],file_hdr.filename,"_0000.0002.HDR",vpath,verbose);
  fprintf(stderr,"Opening %s\n", vpath);
  vfpg = fopen(vpath,"w");
  if(!vfpg)
    err("Failed to open key vectors %s\n", vpath);

  for(i=0; i<file_hdr.nkeys; i++) {
    /* Load all vectors for current header key */
    for(j=0; j<file_hdr.nparts; j++) {
      key_vector[j] = calloc(global_vlen[i][j],sizeof(int));
      /* Set vpath  and load key vector */
      strcpy(fpath,&filesys[j*NAMELEN]);
      strcpy(vpath,dirname(fpath));
      /*strcpy(vpath,dirname(&filesys[j*NAMELEN]));*/
      strcat(vpath,"/");
      strcat(vpath,file_hdr.filename);
      strcat(vpath,"_");
      sprintf(cindex,"%04i",j+1);
      strcat(vpath,cindex);
      strcat(vpath,".0002.HDR");
      fprintf(stderr,"Opening %s\n", vpath);
      vfp = fopen(vpath,"r");
      if(!vfp)
        err("Failed to open key vectors %s\n", vpath);
      if(verbose)
        fprintf(stderr,"Seek to %d to read %d ints for next key-vector\n", global_vloc[i][j],
                       global_vlen[i][j]);
      efseek(vfp,global_vloc[i][j],SEEK_SET);
      efread(key_vector[j],sizeof(int),global_vlen[i][j],vfp);  
      efclose(vfp);
    }
    /* Build global vector for current key */
    numg = 0;
    vlen = 0;
    for(j=0; j<file_hdr.nparts; j++) {
      if(verbose)
        fprintf(stderr,"Key-vector length for partition %d is %d\n", j+1,global_vlen[i][j]);
      vlen += global_vlen[i][j];
    }
    /* Final len will be <= vlen */
    tempv = calloc(vlen,sizeof(int));
    gndx = 0;
    sortv = calloc(vlen,sizeof(int));
    index = calloc(vlen,sizeof(int));
    if(verbose)
      fprintf(stderr,"Allocated %d ints for tempv\n", vlen);
    gvlen = 0;
    /* Set location in each vector to zero */
    for(j=0; j<file_hdr.nparts; j++)
      vndx[j] = 0;
    for(;;) {
      /* Initialize indices for sort */
      for(j=0; j<vlen; j++)
        index[j] = j;
      /* Find next lowest key */
      min = INT_MAX;
      /* Set all slots as not containing min */
      for(j=0; j<file_hdr.nparts; j++)
        slot[j] = -1;
      /* Find minimum remaining key value */
      for(j=0; j<file_hdr.nparts; j++) {
        if(vndx[j] < global_vlen[i][j] && key_vector[j][vndx[j]] < min) {
          min = key_vector[j][vndx[j]];
        }
      }
      /* Set slot for all parts which contain min */
      for(j=0; j<file_hdr.nparts; j++) {
        if(vndx[j] < global_vlen[i][j] && key_vector[j][vndx[j]] == min) {
          slot[j] = 1;
          /*if(verbose)
            fprintf(stderr,"Partiton %d has current key\n", j+1);*/
        }
      }

      k = 0;
      for(j=0; j<file_hdr.nparts; j++) {
        if(slot[j] >= 0) {
          k++;
          break;
        }
      }
      if(!k)
          err("Build global vector failed - MIN not found\n");
      /*if(verbose && i == 2)
        fprintf(stderr,"Next key: %d\n", min);*/
      /* Put key value in next slot in global vector and bump number of key values */
      tempv[gndx] = min;
      numg++;
      /* Count all nums for min */
      m = 0;
      for(j=0; j<file_hdr.nparts; j++) {
        if(vndx[j] < global_vlen[i][j] && key_vector[j][vndx[j]]  == min) {
          m += key_vector[j][vndx[j]+1];
          /*if(verbose && i == 2)
            fprintf(stderr,"Partiton %d has num = %d\n", j+1,key_vector[j][vndx[j]+1]);*/
        }
      }
      if(m == 0)
        err("Build global vector failed - NUMG=0\n");
      /*if(verbose && i == 2)
        fprintf(stderr,"Num: %d\n", m);*/
      /* Store number of occurences in next global slot and update global vector length */
      tempv[gndx+1] = m;
      gvlen += 2 + m;
      m = 0;
      /* Sort all addresses for current slot */
      for(j=0; j<file_hdr.nparts; j++) {
        if(slot[j] == 1) {
          for(k=0; k<key_vector[j][vndx[j]+1]; k++) {
            sortv[m] = key_vector[j][vndx[j]+2+k];
            m++;
          }
        }
      }
      /* Integer sort */
      quick_sort(sortv,index,m);
      for(j=0; j<m; j++)
        tempv[gndx+2+j] = sortv[index[j]];
      /* Increment global vector ndex */
      gndx += 2 + tempv[gndx+1];
      /* For each vector which contributed current key value, increment its index */
      for(j=0; j<file_hdr.nparts; j++) {
        if(slot[j] == 1) {
          if(vndx[j] < global_vlen[i][j])
            vndx[j] += 2 + key_vector[j][vndx[j]+1];
        }
      }
      /*if(verbose && i == 2)
        fprintf(stderr,"Incremented vndx pointers\n");*/
      /* See if at least one vector still has data */
      m = 0;
      for(j=0; j<file_hdr.nparts; j++) {
        if(vndx[j] < global_vlen[i][j])
          m = 1;
      }
      /* All are at end, go to next key */
      if(!m)
        break;
/*
      if(verbose && gndx%1000 == 0) {
        fprintf(stderr,"vtemp up to %d\n", gndx);
        for(j=0; j<gndx; j+=8) {
          for(m=0; m<8; m++)
            fprintf(stderr,"%d ", tempv[j+m]);
          fprintf(stderr,"\n");
        }
      }
*/
    }

    /* Update global hdr limits */
    global_hdr_limits[i].bhp_hdr_num = numg;
    global_hdr_limits[i].bhp_hdr_vlen = gvlen;
    global_hdr_limits[i].bhp_hdr_vloc = loc;
    /* Set loc to start of next key within global vector */
    loc += global_hdr_limits[i].bhp_hdr_vlen * sizeof(float);

    /* Write key vector */
    efseek(vfpg,global_hdr_limits[i].bhp_hdr_vloc,SEEK_SET);
    if(my_endian != file_hdr.endian)
      for(j=0; j<global_hdr_limits[i].bhp_hdr_vlen; j++)
        swap_int_4(&tempv[j]);
    efwrite(tempv,sizeof(int),global_hdr_limits[i].bhp_hdr_vlen,vfpg);  
    fflush(vfpg);
    if(verbose) {
      fprintf(stderr,"Finished key vector for %s\n", global_hdr_limits[i].bhp_hdr_name);
      fprintf(stderr,"Seek to byte %d to write %d ints\n", global_hdr_limits[i].bhp_hdr_vloc,
                      global_hdr_limits[i].bhp_hdr_vlen);
    }

    /* release buffers */
    free(tempv);
    free(sortv);
    free(index);
    for(j=0; j<file_hdr.nparts; j++)
      free(key_vector[j]);
  }
  efclose(vfpg);
  if(verbose)
    fprintf(stderr,"Finished global key-vector\n");

  /* Get path and write global hdr_limits */
  get_bhpio_path(path[0],file_hdr.filename,"_0000.0001.HDR",fpath,verbose);
  /* Copy global_hdr_limits */
  memcpy(hdr_limits,global_hdr_limits,file_hdr.nkeys*sizeof(bhp_hdr_limits));
  if(write_hdr_limits(fpath,verbose))
    err("Failed to write header limits\n");

  /* Release buffers */
  free(hdr_limits);
  free(global_hdr_limits);
  free(filesys);
  free(key_vector_len);
  for(i=0; i<file_hdr.nkeys; i++) {
    free(key_vals[i]);
    free(global_vlen[i]);
    free(global_vloc[i]);
  }
  free(key_vector);
  free(vndx);
  free(slot);
  free(key_vals);
  free(global_vlen);
  free(global_vloc);

  return EXIT_SUCCESS;

}

int bhp_open_write(int npaths, char **path, char *name, int striping, int size,
                   int verbose, int index, int *next)
{

  char fpath[NAMELEN];
  char cindex[8];
  char command[256];

  int i, stat;

  /* Get path with most space */
  stat = bhp_next_path(path,npaths,size,verbose,next);
  if(stat != 0)
    return stat;

  /* If index = 1, create new file, else create next partition */
  if(index == 1) {
    if(verbose) {
      if(striping)
        fprintf(stderr,"Writing multiple partitions, each up to %d MBytes\n", size);
      else
        fprintf(stderr,"Writing single partition\n");
    }
  
    /* Build partition zero filename */
    strcpy(fpath,path[*next]);
    strcat(fpath,"/");
    strcat(fpath,name);
    strcat(fpath,"_0000");
  }

  /* Build complete file name */
  strcpy(fpath,path[*next]);
  strcat(fpath,"/");
  strcat(fpath,name);
  strcat(fpath,"_");
  sprintf(cindex,"%04i",index);
  strcat(fpath,cindex);
  strcat(fpath,".su");
  fprintf(stderr,"Opening partition %s\n", fpath);
  fp = efopen(fpath,"w");
  if(fp == NULL)
    return stat = 1;
  else {
    /* Save filesystem in list */
    strcpy(&filesys[(index-1)*NAMELEN],fpath);
    return stat = 0;
  }

}

int *build_key_vector(float *keys, int count, int part, int *len, int ndx, int verbose)
{

  int *index;
  int i, num, n1;
  int loc;
  int dir;   /* 1=increasing,2=decreasing,-1=random,0=constant */
  int part1m;
  int *vector;

  float *work;
  float min, max;

  work = calloc(count,sizeof(float));
  index = calloc(count,sizeof(int));

  /* Index sort */
  for(i=0; i<count; i++)
    index[i] = i;
  qkisort(count,keys,index);
  for(i=0; i<count; i++)
    work[i] = keys[index[i]];
 
  /* Count occurences */
  num = 1;
  for(i=1; i<count; i++) {
    if(work[i] != work[i-1])
      num++;
  }

  /* Add min, max, num to hdr_limits */
  min = work[0];
  max = work[0];
  for(i=1; i<count; i++) {
    if(work[i] > max)
      max = work[i];
    if(work[i] < min)
      min = work[i];
  }
  hdr_limits[ndx].bhp_hdr_min = min;
  hdr_limits[ndx].bhp_hdr_max = max;
  hdr_limits[ndx].bhp_hdr_num = num;
  if(min < global_hdr_limits[ndx].bhp_hdr_min)
    global_hdr_limits[ndx].bhp_hdr_min = min;
  if(max > global_hdr_limits[ndx].bhp_hdr_max)
    global_hdr_limits[ndx].bhp_hdr_max = max;

  /* Set header order - test constant order */
  dir = 0;
  /* See if key is constant */
  for(i=1; i<count; i++) {
    if((keys[i] - keys[i-1]) != 0) {
      /* Try increasing */
      dir = 1;
      break;
    }
  }
  if(dir == 1) {
    for(i=1; i<count; i++) {
      if((keys[i] - keys[i-1]) < 0) {
        /* Try decreasing */
        dir = 2;
        break;
      }
    }
    if(dir == 2) {
      for(i=1; i<count; i++) {
        if((keys[i] - keys[i-1]) > 0) {
          /* Random */
          dir = -1;
          break;
        }
      }
    }
  }
  /* Update hdr-limits */
  hdr_limits[ndx].bhp_hdr_order = dir;
  
  /* Build vector for reading */
  part1m = part * MAX_TRACES;
  *len = count + num * 2;
  vector = calloc(*len,sizeof(int));
  vector[0] = work[0];
  loc = 0;
  n1 = 1;
  vector[loc+n1+1] = part1m + index[0];

  for(i=1; i<count; i++) {
    if(work[i] != work[i-1]) {
      vector[loc+1] = n1;
      loc += n1 + 2;
      n1 = 1;
      vector[loc] = work[i];
      vector[loc+n1+1] = part1m + index[i];
    }
    else {
      n1++;
      vector[loc+n1+1] = part1m + index[i];
    }
  }
  vector[loc+1] = n1;

  free(work);
  free(index);

  return vector;

}

int write_hdr_limits(char *path, int verbose)
{

  int i;

  FILE *lfp;

  /* Open header limits */
  if(verbose)
    fprintf(stderr,"Opening %s\n", path);
  lfp = fopen(path,"w");
  if(!lfp) {
    fprintf(stderr,"Could not open %s\n", path);
    return 1;
  }

  for(i=0; i<file_hdr.nkeys; i++) {
    fprintf(lfp,"HDRNAME = %s\n", hdr_limits[i].bhp_hdr_name);
    fprintf(lfp,"MIN = %d\n", hdr_limits[i].bhp_hdr_min);
    fprintf(lfp,"MAX = %d\n", hdr_limits[i].bhp_hdr_max);
    fprintf(lfp,"INCR = %d\n", hdr_limits[i].bhp_hdr_inc);
    fprintf(lfp,"SCALAR = %d\n", hdr_limits[i].bhp_hdr_scalar);
    fprintf(lfp,"NUM = %d\n", hdr_limits[i].bhp_hdr_num);
    fprintf(lfp,"ORDER = %d\n", hdr_limits[i].bhp_hdr_order);
    fprintf(lfp,"TYPE = %d\n", hdr_limits[i].bhp_hdr_type);
    fprintf(lfp,"DATATYPE = %s\n", hdr_limits[i].bhp_hdr_data);
    fprintf(lfp,"VLEN = %d\n", hdr_limits[i].bhp_hdr_vlen);
    fprintf(lfp,"VLOC = %d\n", hdr_limits[i].bhp_hdr_vloc);
  }
  fclose(lfp);

  return 0;

}

/* The quick-sort algorithm */
/* Copied from DELPHI3.2 source */

void quick_sort(int *data, int *flag, int n)
{
  int i=0,j=n-1,iw;
  int x=data[(i+j)/2],w;

  do {
    /* narrow window around middle value */
    while (data[i]<x) i++;
    while (data[j]>x) j--;
    if (i>j) break;

    /* swap data and flag values */
    w=data[i];data[i]=data[j];data[j]=w;
    iw=flag[i];flag[i]=flag[j];flag[j]=iw;
    } while (++i <= --j);

  if (j>0) quick_sort(data,flag,j+1);
  if (i<n-1) quick_sort(&data[i],&flag[i],n-i);

}

int bhp_next_path(char **path, int npaths, int size, int verbose, int *next)
{

  int i, stat;
  int index;
  float mb_avail;
  float most;

  struct statvfs buf;

  index = -99;
  most = 0;

  for(i=0; i<npaths; i++) {
    stat = statvfs(path[i],&buf);
    fprintf(stderr,"Checking %s\n", path[i]);
    if(stat) {
      fprintf(stderr,"Error getting status for %s\n", path[i]);
      continue;
    }
    mb_avail = ((float)buf.f_frsize * (float)buf.f_bavail) / (1024. * 1024.);
    if(mb_avail > most) {
      most = mb_avail;
      index = i;
    }
  }
  if(most < size) {
    fprintf(stderr,"Not enough space to hold next partition\n");
    return stat = 1;
  }
  else {
    if(verbose)
      fprintf(stderr,"Allocate next partition in %s, with %fMB available\n", path[index],most);
    *next = index;
    return stat = 0;
  }
}
