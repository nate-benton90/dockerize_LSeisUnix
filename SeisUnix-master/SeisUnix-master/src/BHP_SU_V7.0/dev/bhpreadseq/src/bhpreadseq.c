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
* KEYWORDS:  $RCSfile: bhpreadseq.c,v $
*            $Revision: 1.26 $
*            $Date: 2004/01/14 17:08:29 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpreadseq.c,v $
* Revision 1.26  2004/01/14 17:08:29  ahmilb
* Edit self-doc endian parameter.
*
* Revision 1.25  2004/01/12 17:04:37  ahmilb
* Add rule argument to key-parsing function.
*
* Revision 1.24  2003/05/14 16:43:17  ahmilb
* Add vdt, vdelrt to subset_trace arg list.
* If layer_boundary is specified, set need_stats flag.
*
* Revision 1.23  2003/04/22 17:27:09  ahmilb
* Disable calls to free.
*
* Revision 1.22  2003/04/01 22:09:27  ahmilb
* Fix for near parameter default.
*
* Revision 1.21  2003/02/24 17:25:38  ahmilb
* Add NULLVAL to bilin_interp_trace arguments.
* Fix to allow nearest and binsize lists to be > nkeys, ignore excess.
*
* Revision 1.20  2003/01/28 23:25:11  ahmilb
* Add null to arglist for bilin_interp_trace
*
* Revision 1.19  2003/01/14 19:21:51  ahmilb
* Replace hard-coded lengths with NAMELEN, NFILES.
* Add isadir function.
*
* Revision 1.18  2002/12/16 17:35:16  ahmilb
* Fix to allow 5 keys.
*
* Revision 1.17  2002/11/08 15:48:50  ahmilb
* Separate stats for horizons.
*
* Revision 1.16  2002/09/26 19:33:57  ahmilb
* Fix swap_short calls.
*
* Revision 1.15  2002/09/26 19:06:28  ahmilb
* Clean up memory release code.
*
* Revision 1.14  2002/09/26 16:03:39  ahmilb
* Re-write interpolation code.
* Allow bilinear and nearest trace interpolation for both normal and arbitrary access.
*
* Revision 1.13  2002/07/29 17:34:45  ahmilb
* Vkey as floats to allow sub-mill interval.
* Individual stats for each property.
* Common file header.
* Re-write stats calculation.
*
* Revision 1.12  2002/05/08 13:53:26  ahmilb
* Added trace hdr usage to self-doc, rewrote request=summary and moved to lib, moved stats struct to include.
* Moved trace_stats fcn to lib.
*
* Revision 1.11  2002/03/01 14:14:58  ahmilb
* fix sample interval, start for all data-type,data-order combos in summary
*
* Revision 1.10  2002/02/28 14:53:10  ahmilb
* ignore vkey if properties= or horizons= is used
*
* Revision 1.9  2002/02/20 22:53:07  ahmilb
* don't require binsize or nearest entry for vkey
*
* Revision 1.8  2002/02/20 17:42:16  ahmilb
* fix stats bug (ntraces=0); set incr=1 in build_line call (temporary)
*
* Revision 1.7  2002/01/29 16:39:55  ahmilb
* Add vertical key and map-view access.
*
* Revision 1.6  2001/10/30 19:16:33  ahmilb
* Move build_trace to bhpio_lib.
* Re-structure byte-swapping code.
* Change check_endian arglist.
* Replace puttr_endian with write_trace.
*
* Revision 1.5  2001/10/12 20:05:47  ahmilb
* Allow for aliased dataset names.
*
* Revision 1.4  2001/09/17 17:49:25  ahmilb
* Fix incorrect subscript in calls to trace_stats.
*
* Revision 1.3  2001/09/14 18:22:12  ahmilb
* Use check_endian function in bhpio_lib.
*
* Revision 1.2  2001/09/13 18:58:14  ahmilb
* Add horizons code.
* Move several hundred lines of similar code in bhpreadseq/bhpreadcube to functions in bhpio_lib
* See cvs log for bhpio_lib for a list of functions
*
* Revision 1.1  2001/07/25 18:54:42  ahmilb
* Changed oldbhpread to bhpreadseq.
*
*
*
******************************************************************/

#include <sys/stat.h>
#include "bhpio.h"
#include "bhpioseq.h"
#include "bhp_interp.h"
#ifdef IRIX
  #include <math.h>
#endif

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
" bhpreadseq  filename=fname [optional parameters] > stdout        ",
"                                                                  ",
" bhpreadseq reads a BHPIO dataset which has been created by       ",
" bhpwriteseq. The data can be accessed in order by any combination",
" of the trace headers which were indexed when the dataset was     ",
" created by bhpwriteseq.                                          ",
"                                                                  ",
" Required Parameters:                                             ",
"  filename=fname        Base file name; BHPIO will prepend        ", 
"                          and append file system, etc.            ",
" Optional Parameters:                                             ",
"    pathlist='filename'.dat ASCII file containing list of paths   ",
"    verbose=0              For debug print                        ",
"    request=data           data to get data, or summary to get    ",
"                           the following information about a data ",
"                           request: min,max,mean,rms values,      ",
"                           samples-per-trace,number of traces,    ",
"                           sample units(0=milliseconds,1=feet,2=meters),",
"                           sampling interval, start time          ",
"    properties=...         List of properties                ",
"                           If the input data were created by      ",
"                           bhproffread, properties should be a    ",
"                           list of the properties which           ",
"                           are requested                          ",
"    horizons=...           List of horizons.                      ",
"                           If the input data were created by      ",
"                           bhphorizon, horizons should be a       ",
"                           list of the horizons which             ",
"                           are requested                          ",
"    keys=fldr,offset       Up to 5 header keys                    ",
"    keylist=p1,p2:s1-s2    List, ranges to read for each          ",
"                            key, separated by colon;              ",
"                            Valid syntax includes:                ",
"                            k1,k2,...  List of specific keys      ",
"                            k1-k2      Range                      ",
"                            k1-k2[k3]  Range, with increment      ",
"                            p1,s1^p2,s2 Project arbitrary traverse",
"                                   from p1,s1 to p2,s2, and       ",
"                                   read traces along the line.    ",
"                                   By default, traces are formed  ",
"                                   by bilinear interpolation.     ",
"                                   To get nearest traces along    ",
"                                   arbitrary line, use interpolation=nearest",
"                            *      Read all                   ",
"      If keylist is specified, it must contain an entry for each  ",
"      specified key                                               ",
"    nearest=0,0            Comma-separated flags, one flag for    ",
"                           for each key; 1=take nearest key       ",
"                           if value in keylist not found;         ",
"                           0=take only values which match keylist ",
"      If nearest is specified, it must contain an entry for each  ",
"      specified key                                               ",
"    combine=1              Number of primary key ensembles to     ",
"                           combine for supergathers               ",
"    binsize=k1,k2          Binsize for each specified key         ",
"    timeslice=st,end,inc   Timeslice start time, end time, incr   ",
"    display=yes            Display time slices; no=don't display  ",
"                                                                  ",
"                                                                  ",
"    interpolate=bilinear   Spatial interpolation option           ",
"                           Requires primary and secondary keys    ",
"                           Use interpolate=nearest to get nearest ",
"                           trace instead of interpolating         ",
"    endian=my_endian       Endianness of stdout.                  ",
"                           Omit endian parameter to write native  ",
"                           Specify endian=1 to force BIG_ENDIAN   ",
"                           Specify endian=0 to force LITTLE_ENDIAN",
"                                                            ",
"  C SHELL ALERT:  If you use C shell, you will get the error message 'No match'",
"                  if you use a key increment, e.g. keylist=1-10[2]     ",
"                  To avoid the error, use a different shell or protect the square",
"                  brackets with backslashes, e.g. keylist=1-10\\[2\\]  ",
"                                                            ",
"  Trace Header Usage:                     ",
"    wevel=[0,1]        wevel=1 is used to flag last trace in ensemble",
"    tracr contains a sequentional ensemble number",
NULL};

/* Globals */
int nkeys;                        /* number of header fields set          */
cwp_String type[SU_NKEYS];        /* SU header key types */
#ifdef CWP_BIG_ENDIAN
  int my_endian = 1;
#else
  int my_endian = 0;
#endif

data_stats stats;         /* Structure to hold min, max, etc */

/* Prototypes */
void sort_keys(int nkeys, segy **traces, int nt, int ns, int *key_index,
                 bhp_hdr_limits *hdr_limits, Value *val_list, size_t nvsize, int *order);
int cmp_list(const void *a, const void *b);
Value negval(cwp_String type, Value val);   /* reverse sign of value    */
int build_readlist(int *key_vector, int vlen, int *kindex, int combine, int *readlist);
void read_traces(int nread, segy **traces, int *readlist, int key_index, FILE **fp,
                 int my_endian, int verbose);
void pass_traces(int nread, segy **traces, int nkeys, int *korder, int *nklist,
                 int **klist, int *read_order, int *binsize, int imark, int iens,
                 int ensnum, int *key_index, int ngroup, size_t nvsize, Value *val_list,
                 int *pass, int *npass, char *request, int nprop, int *iprop, int iz, int nl,
                 int my_endian, int endian, float *maxdepth, float *mindepth, short nsamp, int nvklist,
                 float *vklist, int vflag, float vdt, float vdelrt, int need_stats,
                 int layer_boundary, int verbose);

int main(int argc, char **argv)
{

  char *keylist;          /* key lists */
  char keylist_subset[2560]; /* Part of keylist pertaining to current key */
  char fpath[NAMELEN];    /* File header path */
  char path[NAMELEN];     /* File path */
  char cindex[8];         /* For appending partition numbers */
  char *path1;            /* First path from pathlist */
  char *request;          /* request type=data or summary */
  char *interpolate;      /* Spatial interpolation option */
  char *p1;               /* Used to check for arb access */
  char *aname;            /* aliased filename */
  char data_type[16];     /* seismic, horizon, property */
  char data_order[16];    /* cross-section, map-view */

  int verbose;            /* debug printout */
  int i, j, k, m;         /* Loop counters */
  int *nklist;            /* Number of entries in each key list */
  int nvklist=0;          /* length of expanded vkey list */
  int vflag=0;            /* =1 if vkey specified in keys */
  int *korder;            /* Order of expanded keylist; 0=descending, 1=constant or ascending */
  int *near;              /* Nearest trace flags */
  int combine=1;          /* Supergather - number to combine */
  int *read_order;        /* Read order for each key: 0=no order, 1=ordered,all, 2=ordered, subset */
  int *readlist;          /* List to hold disk addresses of ensemble of traces */
  int kindex;             /* Current index into key-vector for building readlist */
  int maxens;             /* Number of traces in largest ensemble */
  int nread;              /* Traces to read for current ensemble */
  int *key_index;         /* Key indices in header */
  int min;                /* Minimum skey value */
  int **klist=NULL;       /* Expanded key lists */
  int **key_vector;       /* Header key vectors */
  int *binsize;           /* Bin sizes */
  int ptr;                /* Pointer to next subset */
  int *temp;              /* Temporary key-list */
  int ngroup;             /* Size of sort group in val_list */
  int *pass;              /* Flag for each trace, to pass out or not */
  int imark;              /* Index of wevel trace header - used for end ens flag */
  int iens;               /* Index of tracr trace header - used for ensemble number */
  int ensnum=1;           /* Relative ensemble number for each bhpreadseq execution, 1,2,... */
  int merged=0;           /* 1=reading merged files */
  int npass;              /* Actual number of output traces */
  int nprop;              /* Number of properties or horizons in model */
  int *iprop;             /* Sample index of each property or horizon */
  int iz=0;               /* Sample index of first top in model */
  int nlayers=INT_MIN;    /* Number of model layers */
  int layernum=INT_MIN;   /* Layer number for properties=name:n and properties=layer_boundary:n */
  int layer_boundary=0;   /* 1 --> properties=layer_boundary:n specified, else 0 */
  int interp_type=0;      /* Spatial interpolation; 0=none, 1=nearest, 2=bilinear */
  int arbflag=0;          /* Flag for arbitrary access */
  int npair;              /* Key pairs for arbitrary traverses */
  int narb;               /* Length of arbitrary trace list */
  int pk1,pk2,sk1,sk2;    /* Line end-points */
  int endian;             /* endian parameter 1=BIG, 0=LITTLE */
  int need_stats=0;       /* set to 1 if no stats in file_hdr */
  int **corner;           /* keys of corner traces for interpolation */
  int key_min[2];         /* limits for interpolation */
  int key_max[2];         /* limits for interpolation */
  int key_incr[2];        /* limits for interpolation */

  short nsamp;            /* nsamp from tr.ns */

  segy **traces=NULL;     /* Ensemble of traces */

  float *arblist;         /* Arbitrary line list */
  float factor;           /* Scale factor */
  float vdt;              /* sampling interval if vkey specified */
  float dt;               /* Sampling interval from header divided by UNITS from bhpio.h */
  float delrt;            /* Start time from header, assumed to be milliseconds/meters/feet */
  float vdelrt;           /*  first vertical value if vflag is set */
  float mindepth=FLT_MAX; /* Min depth, if trace is a model property */
  float maxdepth=FLT_MIN; /* Max depth, if trace is a model property */
  float *vklist;          /* Expanded vkey list */
  float xfrac, yfrac;     /* fractions for bilinear interp */

  cwp_String keys[SU_NKEYS];  /* SU header key names */

  FILE *fp[NFILES];         /* Open file pointers */
  FILE **ofp;               /* Timeslice file pointers */
  FILE *vfp;                /* Key-vector file pointer */

  Value hval;               /* Trace header value */
  static Value *val_list;   /* List of keys for qsort */
 
  size_t nvsize;            /* Width of group in val_list */

  timeslice_def timeslice;
  timeslice.ntimes = 0;

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  if(!getparstring("filename",&file_hdr.filename))
    err("Filename is required\n");
  /* save filename */
  i = strlen(file_hdr.filename);
  aname = calloc(i+1,sizeof(char));
  strcpy(aname,file_hdr.filename);

  /* get path to first partition */
  path1 = get_path1(file_hdr.filename,verbose);

  /* check for alias */
  if(!strcmp(&file_hdr.filename[i-10],"_as_events")) {
    /* trim */
    file_hdr.filename[i-10] = '\0';
    if(verbose) {
      fprintf(stderr,"Aliased filename %s\n",aname);
      fprintf(stderr,"Trimmed filename %s\n",file_hdr.filename);
    }
  }

  /* set request, initialize stats */
  if(!getparstring("request",&request))
    request = "data";
  if(!strcmp(request,"summary"))
    stats.ntr = 0;
  
  /* Make sure file is there */
  if(!get_bhpio_path(path1,aname,"_0000.HDR",fpath,verbose))
    err("FILE: path=%s, filename=%s does not exist\n",path1,file_hdr.filename);
  /* Get file header */
  read_file_hdr(fpath,verbose);

  /* need stats? */
  if((file_hdr.nprop == 0 && file_hdr.minval == FLT_MAX) ||
     (file_hdr.nprop > 0 && (nprop = countparval("properties") == 0) && file_hdr.minval == FLT_MAX) ||
     (file_hdr.nprop > 0 && (nprop = countparval("properties") != 0) && file_hdr.prop_minval[0] == FLT_MAX) ||
     (file_hdr.nprop < 0 && (nprop = countparval("horizons") == 0) && file_hdr.minval == FLT_MAX) ||
     (file_hdr.nprop < 0 && (nprop = countparval("horizons") != 0) && file_hdr.prop_minval[0] == FLT_MAX)) {
    need_stats = 1;
    /* alloc nvals */
    if(file_hdr.nprop == 0)
      stats.nvals = calloc(1,sizeof(int));
    else {
      j = (file_hdr.nprop > 0) ? file_hdr.nprop : -file_hdr.nprop;
      stats.nvals = calloc(j,sizeof(int));
    }
    if(verbose)
      fprintf(stderr,"Calculating stats on-the-fly\n");
  }

  /* If MERGED is in filename, set flag */
  if(strstr(file_hdr.filename,"MERGED") != NULL) {
    merged = 1;
    if(verbose)
      fprintf(stderr,"Reading merged dataset\n");
  }

  /* data type and order(sequential is always X-section) */
  strcpy(data_order,"CROSS-SECTION");
  if(file_hdr.nprop == 0)
    strcpy(data_type,"SEISMIC");
  else if(file_hdr.nprop < 0)
    strcpy(data_type,"HORIZON");
  else if(file_hdr.nprop > 0)
    strcpy(data_type,"PROPERTY");

  /* Open files */
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
      fprintf(stderr,"Opening %s\n", path);
    fp[i] = fopen(path,"r");
    if(!fp[i])
      err("Could not open %s\n", path);
  }

  /* Get dt, delrt from first trace */
  efread(&tr,HDRBYTES,1,fp[0]);
  rewind(fp[0]);
  /* check endian */
  if(file_hdr.endian != my_endian) {
    swap_short_2(&tr.dt);
    swap_short_2(&tr.ns);
    swap_short_2(&tr.delrt);
  } 
  dt = tr.dt / UNITS;
  delrt = tr.delrt;
  delrt /= 1000.;
  nsamp = tr.ns;
   
  /* if units are -1, nothing has been set, check dt, else just print units */
  if(file_hdr.units == -1) {
    /* if dt > 8 assume units=meters */
    if(dt > 8)
      file_hdr.units = 2;
    /* seconds */
    else
      file_hdr.units = 0;
  }

  if(verbose) {
    fprintf(stderr,"%s is %s data, stored in %s order\n",file_hdr.filename,data_type,data_order);
    fprintf(stderr,"Units = %d, (0=milliseconds,1=feet,2=meters)\n",file_hdr.units);
    fprintf(stderr,"First Path %s\n", path1);
    fprintf(stderr,"File Name from File Header %s\n", file_hdr.filename);
    if(file_hdr.endian)
      fprintf(stderr,"%s is in BIG_ENDIAN format\n", file_hdr.filename);
    else
      fprintf(stderr,"%s is in LITTLE_ENDIAN format\n", file_hdr.filename);
    fprintf(stderr,"Partition Size from File Header %d\n", file_hdr.size);
    fprintf(stderr,"Number of Partitions from File Header %d\n", file_hdr.nparts);
    fprintf(stderr,"Samples per Trace from File Header %d\n", file_hdr.nsamp);
    fprintf(stderr,"Traces per Partition from File Header %d\n", file_hdr.traces_per_part);
    fprintf(stderr,"Number of Keys from File Header %d\n", file_hdr.nkeys);
    for(i=0; i<file_hdr.nparts; i++)
      fprintf(stderr,"Partition %d from File Header %s\n", i+1,&filesys[NAMELEN*i]);
    if(file_hdr.nprop > 0) {
      fprintf(stderr,"Number of Properties: %d\n",file_hdr.nprop);
      if(file_hdr.data_order <= 1)
        fprintf(stderr,"Number of Layers: %d\n",(file_hdr.nsamp-1)/(file_hdr.nprop+1));
      else if(file_hdr.data_order == 2)
        fprintf(stderr,"Number of Layers: %d\n",hdr_limits[0].bhp_hdr_max/file_hdr.nprop);
      fprintf(stderr,"Available PROPERTIES: %s ",properties[0]);
      for(i=1; i<file_hdr.nprop; i++)
        fprintf(stderr," %s ",properties[i]);
      fprintf(stderr,"\n");
    }
    else if(file_hdr.nprop < 0) {
      fprintf(stderr,"Number of Horizons: %d\n",-file_hdr.nprop);
      fprintf(stderr,"Available HORIZONS: %s ",properties[0]);
      for(i=1; i<-file_hdr.nprop; i++)
        fprintf(stderr," %s ",properties[i]);
      fprintf(stderr,"\n");
    }
    else {
      fprintf(stderr,"Sampling interval, in milliseconds, from first trace %.3f\n", dt);
      fprintf(stderr,"Start time, in milliseconds, from first trace %.3f\n", delrt);
    }
  }

  /* if request=data, check output type */
  if(!strcmp(request,"data"))
    check_stdout();

  /* endian */
  endian = get_endian(my_endian,verbose);

  /* check properties and horizons */
  iprop = get_prop_horz(properties,&nprop,&iz,&nlayers,file_hdr.nprop,
                        file_hdr.nsamp,&layernum,&layer_boundary,verbose);
  /* if layer_boundary is set, reset any stats in file_hdr */
  if(layer_boundary == 1) {
    file_hdr.minval = FLT_MAX;
    file_hdr.maxval = -FLT_MAX;
    file_hdr.meanval = 0;
    file_hdr.rmsval = 0;
    need_stats = 1;
    stats.nvals = calloc(1,sizeof(int));
  }

  /* vertical key */
  if(!strcmp(file_hdr.vkey,"")) {
    strcpy(file_hdr.vkey,"tracl");
    if(!strcmp(data_type,"PROPERTY") || !strcmp(data_type,"HORIZON")) {
      file_hdr.vmin = 1;
      if(!strcmp(data_type,"PROPERTY"))
        file_hdr.vmax = (file_hdr.nsamp - 1) / (file_hdr.nprop + 1);
      else
        file_hdr.vmax = file_hdr.nsamp;
      file_hdr.vinc = 1;
    }
    else {
      file_hdr.vmin = delrt;
      file_hdr.vmax = delrt + dt * (nsamp - 1);
      file_hdr.vinc = dt;
    }
  }
  /* for cross-section model data, ignore possibly bad info in hdr */
  if(!strcmp(data_type,"PROPERTY") && !strcmp(data_order,"CROSS-SECTION")) {
    file_hdr.vmin = 1.0;
    file_hdr.vinc = 1.0;
    file_hdr.vmax = (float)((file_hdr.nsamp-1)/(file_hdr.nprop+1));
  }
  /* check vmax and adjust if not correct in hdr */
  if(!strcmp(data_type,"SEISMIC") && !strcmp(data_order,"CROSS-SECTION"))
    file_hdr.vmax = file_hdr.nsamp * file_hdr.vinc - file_hdr.vinc + file_hdr.vmin;
  if(verbose)
    fprintf(stderr,"Vertical Key from File Header: %s, Min: %.3f, Max: %.3f, Incr: %.3f\n", file_hdr.vkey,
            file_hdr.vmin,file_hdr.vmax,file_hdr.vinc); 

  /* Check keys */
  if((nkeys=countparval("keys"))!=0) {
    getparstringarray("keys",keys);
    /*if(nkeys > 5)
      err("Five or fewer keys are allowed\n");*/
    /* check arbitrary traverse */
    if(getparstring("keylist",&keylist)) {
      /* If ^ present, set arb access flag, and change ^ to , */
      for(;;) {
        if((p1 = strstr(keylist,"^")) != NULL) {
          p1[0] = ',';
          arbflag = 1;
          continue;
        }
        else
          break;
      }
    }
    /* vertical key must be last, if specified */
    if(!strcmp(file_hdr.vkey,keys[nkeys-1])) {
      vflag = 1;
      nkeys--;
    }
    else
      for(i=0; i<nkeys-1; i++)
        if(!strcmp(file_hdr.vkey,keys[i]))
          err("Vertical key must be specified last\n");
  }
  /* if vflag set, check for vertical keylist */
  if(vflag == 1 && (getparstring("keylist",&keylist) != 0)) {
    ptr = 0;
    /* if arbflag is set, assume vkeylist follows first :, otherwise vkeylist follows last : */
    if(arbflag == 1)
      j = 1;
    else
      j = nkeys;
    for(i=0; i<j; i++)
      get_keylist_subset(&keylist[ptr],keylist_subset,&ptr);
    /* if ptr is at end of keylist, assume no keylist entry for vkey */
    if(ptr > strlen(keylist))
      vflag = 0;
    else {
      get_keylist_subset(&keylist[ptr],keylist_subset,&ptr);
      if(!strcmp(keylist_subset,"*"))
        vflag = 0;
      else {
        if(file_hdr.vinc ==  0)
          err("Vertical sub-set not allowed. Vertical key was not saved when file was created\n");
        vklist = parse_keylistf(keylist_subset,file_hdr.vmin,file_hdr.vmax,
                                file_hdr.vinc,&nvklist,0,"near",verbose);
      }
    }
  }

  /* Get key types */
  for (i=0; i<nkeys; i++) {
    type[i] = hdtype(keys[i]);
    if(verbose)
      fprintf(stderr,"Key: %s, Type: %s\n", keys[i], type[i]);
  }
 
  /* If any keys present, need hdr_limits */
  if(nkeys) {
    /* Alloc hdr_limits space */
    hdr_limits = calloc(nkeys,sizeof(bhp_hdr_limits));
    /* Alloc key indices */
    key_index = calloc(nkeys,sizeof(int));
    if(verbose)
      fprintf(stderr,"Allocated %d bytes for hdr_limits\n", nkeys*sizeof(bhp_hdr_limits));
    /* Build hdr_limits filename */
    if(!get_bhpio_path(path1,file_hdr.filename,"_0000.0001.HDR",fpath,verbose))
      err("Could not access file: %s\n", fpath);
  }
  for(i=0; i<nkeys; i++) {
    /* Locate key in file header, and load info */
    if(!read_hdr_limits(fpath,i,keys[i],verbose))
      err("The key %s was not saved when the file was written\n", keys[i]);
    key_index[i] = getindex(hdr_limits[i].bhp_hdr_name);
    factor = hdr_limits[i].bhp_hdr_scalar;
    if(factor < 0)
      factor = -1. / factor;
    hdr_limits[i].bhp_hdr_min *= factor;
    hdr_limits[i].bhp_hdr_max *= factor;
    /* Bin size default is .5*inc */
    if(verbose) {
      fprintf(stderr,"Key Info\n");
      fprintf(stderr,"Name: %s, Min: %d, Max: %d, Incr: %d, Num: %d\n",
              hdr_limits[i].bhp_hdr_name,
              hdr_limits[i].bhp_hdr_min,
              hdr_limits[i].bhp_hdr_max,
              hdr_limits[i].bhp_hdr_inc,
              hdr_limits[i].bhp_hdr_num);
      fprintf(stderr,"Order: %d, Type: %d\n", hdr_limits[i].bhp_hdr_order,
              hdr_limits[i].bhp_hdr_type);
      fprintf(stderr,"Key Vector Byte Offset: %d, Number of Ints: %d\n", hdr_limits[i].bhp_hdr_vloc,
              hdr_limits[i].bhp_hdr_vlen);
    }
  }

  /* check interpolation */
  if(getparstring("interpolate",&interpolate)) {
    if(!strcmp(interpolate,"bilinear")) {
      interp_type = 2;
      if(verbose)
        fprintf(stderr,"Interpolating output traces via bilinear interpolation\n");
    }
    else if(!strcmp(interpolate,"nearest")) {
      interp_type = 1;
      if(verbose)
        fprintf(stderr,"Interpolating output traces via nearest trace\n");
    }
    /* Need 2 keys */
    if(nkeys != 2)
      err("interpolation requires 2 keys\n");
  }

  if(arbflag == 1) {
    /* set interpolation=bilinear if not specified */
    if(interp_type == 0)
      interp_type = 2;
    /* Need 2 keys */
    if(nkeys != 2 || file_hdr.nkeys != 2)
      err("Arbitrary traverse requires exactly 2 keys\n");
    /* keylist needs even number of entries, and at least 4 */
    i = countparval("keylist");
    if((i % 2) != 0)
      err("For arbitrary traverse, number of primary and secondary keys must be equal\n");
    npair = i / 2;
    if(npair < 2)
      err("For arbitrary traverse, there must be at least 2 primary and 2 secondary values\n");
    if(verbose)
      fprintf(stderr,"Building output traces via arbitrary lines\n");
  }

  /* if interpolating, set limits */
  if(interp_type != 0) {
    key_min[0] = hdr_limits[0].bhp_hdr_min;
    key_min[1] = hdr_limits[1].bhp_hdr_min;
    key_max[0] = hdr_limits[0].bhp_hdr_max;
    key_max[1] = hdr_limits[1].bhp_hdr_max;
    key_incr[0] = hdr_limits[0].bhp_hdr_inc;
    key_incr[1] = hdr_limits[1].bhp_hdr_inc;
  }

  /* If not arbitrary traverse, parse keylist */
  if(nkeys > 0 && arbflag == 0) {
    nklist = calloc(nkeys,sizeof(int));
    korder = calloc(nkeys,sizeof(int));
    /* Set default order flag for each expanded keylist */
    for(i=0; i<nkeys; i++)
      korder[i] = 1;
    read_order = calloc(nkeys,sizeof(int));
    if(getparstring("keylist",&keylist)) {
      klist = calloc(nkeys,sizeof(int *));
      /* Alloc number of occurences */
      ptr = 0;
      for(i=0; i<nkeys; i++) {
        get_keylist_subset(&keylist[ptr],keylist_subset,&ptr);
        /* Set read-order */
        if(strstr(keylist_subset,"*") == NULL) {
          read_order[i] = 2;
          klist[i] = parse_keylist(keylist_subset,hdr_limits[i].bhp_hdr_min,
                                   hdr_limits[i].bhp_hdr_max,hdr_limits[i].bhp_hdr_inc,
                                   &nklist[i],interp_type,verbose);
        }
        else {
          read_order[i] = 1;
          nklist[i] = 0;
          klist[i] = NULL;
        }
        if(verbose)
          fprintf(stderr,"Order for %s is %d\n", hdr_limits[i].bhp_hdr_name,read_order[i]);
        if(nklist[i] > 1 && (klist[i][1] - klist[i][0] < 0))
          korder[i] = 0;
        if(verbose && nklist[i]) {
          fprintf(stderr,"Expanded keylist\n");
          for(j=0; j<nklist[i]; j++) {
            fprintf(stderr,"%d ", klist[i][j]);
            if(j > 0 && !(j%6))
              fprintf(stderr,"\n");
          }
          fprintf(stderr,"\n");
          if(korder[i])
            fprintf(stderr,"Keylist is constant or ascending\n");
          else
            fprintf(stderr,"Keylist is descending\n");
        }
      }
    }
    else {
      for(i=0; i<nkeys; i++)
        read_order[i] = 1;
      klist = NULL;
      if(vflag == 1)
        vflag = 0;
    }
  }

  /* horizons= overrides vklist */
  if((i = countparval("horizons")) > 0 && vflag == 1) {
    vflag = 0;
      if(verbose)
        fprintf(stderr,"Horizons parameter overriding vertical key specification\n");
  }

  /* For cross-section order, properties=name:n overrides vkey specification */
  if(!strcmp(data_order,"CROSS-SECTION") && layernum != INT_MIN) {
    if(verbose && vflag == 1)
      fprintf(stderr,"Properties parameter overriding vkey specification\n");
    if(vflag == 1)
      free(vklist);
    else
      vflag = 1;
    nvklist = 1;
    vklist = calloc(1,sizeof(float));
    vklist[0] = layernum;
  }

  /* if vflag is set, convert vklist to zero-based sample numbers */
  if(vflag == 1) {
    vdelrt = vklist[0];
    if(nvklist > 1)
      vdt = vklist[1] - vklist[0];
    else
      vdt = file_hdr.vinc;
    for(i=0; i<nvklist; i++)
      vklist[i] = (vklist[i] - file_hdr.vmin) / file_hdr.vinc;
    /* constrain vklist to nlayers if property, or nsamp if events */
    if(!strcmp(data_type,"PROPERTY") && !strcmp(data_order,"CROSS-SECTION") && nprop > 0) {
      /* layer_boundary can be 1 --> nlayers + 1 */
      if(layer_boundary == 0) {
        j = nvklist;
        for(i=0; i<j; i++) {
          if(vklist[i] > (file_hdr.nsamp - 1) / file_hdr.nprop - 1)
            nvklist--;
        }
      }
    }
    if(!strcmp(data_type,"HORIZON") && !strcmp(data_order,"CROSS-SECTION") && nprop > 0) {
      j = nvklist;
      for(i=0; i<j; i++) {
        if(vklist[i] > file_hdr.nsamp - 1)
          nvklist--;
      }
    }
  }
  if(verbose) {
    if(vflag == 1) {
        fprintf(stderr,"Vertical Sub-Set(zero-based samples):\n");
        for(i=0; i<nvklist; i++)
          fprintf(stderr," %.3f ",vklist[i]);
        fprintf(stderr,"\n");
    }
    else
      fprintf(stderr,"Getting all vertical samples\n");
  }

  /* set stats members */
  set_stats(&stats,data_type,data_order,vflag,file_hdr.nsamp,dt,delrt,
            nvklist,vdt,vdelrt,file_hdr.vmin,file_hdr.vinc,
            file_hdr.units,verbose);
 
  /* Load key vectors */
  if(nkeys != 0) {
    key_vector = calloc(nkeys,sizeof(int *));
      if(!get_bhpio_path(path1,file_hdr.filename,"_0000.0002.HDR",fpath,verbose))
        err("Error reading %s\n", fpath);
    if(verbose)
      fprintf(stderr,"Opening %s\n", fpath);
    vfp = fopen(fpath,"r");
    if(!vfp)
      err("Failed to open key vectors\n");
    for(i=0; i<nkeys; i++) {
      key_vector[i] = calloc(hdr_limits[i].bhp_hdr_vlen,sizeof(int));
      efseek(vfp,hdr_limits[i].bhp_hdr_vloc,SEEK_SET);
      efread(key_vector[i],sizeof(int),hdr_limits[i].bhp_hdr_vlen,vfp);
      /* Swap key-vector bytes if necessary */
      if(file_hdr.endian != my_endian) {
        for(j=0; j<hdr_limits[i].bhp_hdr_vlen; j++)
          swap_int_4(&key_vector[i][j]);
      }
      /*for(j=0; j<hdr_limits[i].bhp_hdr_vlen; j+=key_vector[i][j+1]+2)
        fprintf(stderr,"Key Value: %d, Number: %d\n", key_vector[i][j],key_vector[i][j+1]);*/
    }
    fclose(vfp);
  }

  /* Get index of wevel for end-ensemble and tracr for ens num */
  imark = getindex("wevel");
  iens = getindex("tracr");

  /* Check binsize, nearest */
  if(nkeys > 0 && interp_type == 0) {
    i = countparval("binsize");
    if(i > 0 && i < nkeys)
      err("Number of binsize entries must be same as number of keys\n");
    else if(i > 0) {
      binsize = calloc(i,sizeof(int));
      getparint("binsize",binsize);
      if(i > nkeys && verbose == 1)
        fprintf(stderr,"Ignoring extra entries in binsize parameter\n");
    }
    else if(i == 0) {
      binsize = calloc(nkeys,sizeof(int));
      for(i=0; i<nkeys; i++)
        binsize[i] = 0.5 * hdr_limits[i].bhp_hdr_inc;
    }
    if(verbose) {
      fprintf(stderr,"Bin Sizes: ");
      for(i=0; i<nkeys; i++)
        fprintf(stderr,"%d ", binsize[i]);
      fprintf(stderr,"\n");
    }
    i = countparval("nearest");
    if(i > 0 && i < nkeys)
      err("Number of nearest entries must be same as number of keys\n");
    else if(i > 0) {
      near = calloc(i,sizeof(int));
      getparint("nearest",near);
      if(i > nkeys && verbose == 1)
        fprintf(stderr,"Ignoring extra entries in nearest parameter\n");
    }
    else if(i == 0)
      near = calloc(nkeys,sizeof(int));
  }

  /* Check combine parameter */
  if(getparint("combine",&combine) && interp_type == 0) {
    if(combine < 1)
      err("combine must be >= 1");
    if(verbose && combine > 1)
      fprintf(stderr,"Combining %d ensembles to form super-gathers\n", combine);
  }

  /* Ignore nearest parm if interpolating */
  if(!interp_type) {
    /* If nearest is set for a key, scan it's key-vector and possibly modify it's expanded keylist */
    for(i=0; i<nkeys; i++) {
      if(near[i]) {
        if(nklist[i]) {
          temp = calloc(hdr_limits[i].bhp_hdr_num,sizeof(int));
          for(j=0; j<nklist[i]; j++) {
            for(k=0,m=0; k<hdr_limits[i].bhp_hdr_vlen; k+=key_vector[i][k+1]+2,m++)
              temp[m] = ABS(klist[i][j] - key_vector[i][k]);
            min = INT_MAX;
            for(k=0,m=0; k<hdr_limits[i].bhp_hdr_vlen; k+=key_vector[i][k+1]+2,m++) {
              if(temp[m] < min) {
                min = temp[m];
                ptr = k;
              }
            }
            klist[i][j] = key_vector[i][ptr];
          }
          if(verbose) {
            fprintf(stderr,"Modified keylist\n");
            for(j=0; j<nklist[i]; j++) {
              fprintf(stderr,"%d ", klist[i][j]);
              if(j > 0 && !(j%6))
                fprintf(stderr,"\n");
            }
            fprintf(stderr,"\n");
          }
          free(temp);
        }
      }
    }
  }

  /* If any keys specified, and not interpolating, find max ensemble size, and alloc readlist */
  if(nkeys > 0 && interp_type == 0) {
    maxens = 0;
    for(i=0; i<hdr_limits[0].bhp_hdr_vlen; i+=key_vector[0][i+1]+2) {
      if(key_vector[0][i+1] > maxens)
        maxens = key_vector[0][i+1];
    }
    if(verbose)
      fprintf(stderr,"Max ensemble size: %d\n", maxens);
    readlist = calloc(combine*maxens,sizeof(int));
    if(verbose) {
      if(read_order[0] == 2)
        fprintf(stderr,"Reading subset of %s\n", keys[0]);
      else
        fprintf(stderr,"Reading all of %s\n", keys[0]);
    }
    /* Alloc traces, pass flags */
    traces = calloc(combine*maxens,sizeof(float *));
    for(j=0; j<combine*maxens; j++)
      traces[j] = calloc(file_hdr.nsamp+(HDRBYTES/4),sizeof(float));
    if(verbose)
      fprintf(stderr,"Allocated %d traces, %d floats per trace\n",
              combine*maxens,file_hdr.nsamp+(HDRBYTES/4));
    pass = calloc(combine*maxens,sizeof(int));
    /* Parameters, etc for qsort */
    ngroup = nkeys + 1;
    nvsize = ngroup * sizeof(Value);
    val_list = (Value *)ealloc1(combine*maxens,nvsize);
  }
  /* For interpolation, allocate 4 corner traces plus output trace */
  else if(interp_type > 0) {
    traces = calloc(5,sizeof(float *));
    for(j=0; j<5; j++)
      traces[j] = calloc(file_hdr.nsamp+(HDRBYTES/4),sizeof(float));
    if(verbose)
      fprintf(stderr,"Allocated 5 traces, %d floats per trace\n",
              file_hdr.nsamp+(HDRBYTES/4));
    /* corner points for interpolation */
    corner = calloc(4,sizeof(int *));
    for(i=0; i<4; i++)
      corner[i] = calloc(2,sizeof(int));
  }

  /* timeslice */
  check_timeslice(&timeslice,nkeys,verbose);

  /* If doing timeslices, get dt from first trace */
  if(timeslice.ntimes) {
    if(dt*file_hdr.nsamp < timeslice.times[timeslice.ntimes-1]) {
      fprintf(stderr,"Restricting time slices to trace length: %d\n", (int)dt*file_hdr.nsamp);
      j = timeslice.ntimes;
      for(i=0; i<j; i++)
        if(timeslice.times[i] > dt*file_hdr.nsamp)
          timeslice.ntimes--;
    }
    /* Alloc ofiles and initialize */
    ofp = make_tfiles(timeslice.ntimes,timeslice.times,timeslice.opath,verbose);
  }

  /* If no keys, read all partitions as written */
  if(!nkeys) {
    if(verbose)
      fprintf(stderr,"No keys specified, reading all data\n");
    for(i=0; i<file_hdr.nparts; i++) {
      while(efread(&tr,HDRBYTES,1,fp[i])) {
        efread(tr.data,FSIZE,file_hdr.nsamp,fp[i]);
        stats.ntr++;
        check_endian(file_hdr.endian,my_endian,&tr,(short)file_hdr.nsamp,verbose);
        if(nprop != 0)
          build_trace(&tr,file_hdr.nprop,nprop,iprop,iz,nlayers,layer_boundary,
                      &maxdepth,&mindepth,0,0,&stats,verbose);
        /* if summary, save stats */
        if(!strcmp(request,"summary") && need_stats == 1)
          trace_stats(&tr,nlayers,nvklist,nprop,iprop,layer_boundary,verbose);
        else if(!strcmp(request,"data")) {
          nsamp = tr.ns;
          check_endian(my_endian,endian,&tr,nsamp,verbose);
          write_trace(&tr,nsamp);
       }
      }
    }
  }
  /* keys specified, but no interpolation */
  else if(interp_type == 0) {
    /* If not reading all */
    if(read_order[0] == 2) {
      for(j=0; j<nklist[0]; j++) {
        /* Loop thru header key vector */
        kindex = 0;
        for(;;) {
          if(key_vector[0][kindex] >= (klist[0][j] - binsize[0]) &&
             key_vector[0][kindex] <= (klist[0][j] + binsize[0])) {
            /* Build readlist for number of ensembles to combine */
            nread = build_readlist(key_vector[0],hdr_limits[0].bhp_hdr_vlen,&kindex,combine,readlist);
            /* If end of key-vector, nread is zero */
            if(verbose)
              fprintf(stderr,"NREAD %d\n", nread);
            if(!nread)
              break;
            read_traces(nread,traces,readlist,key_index[0],fp,my_endian,verbose);
            npass = 0;
            pass_traces(nread,traces,nkeys,korder,nklist,klist,read_order,binsize,imark,iens,ensnum,
                        key_index,ngroup,nvsize,val_list,pass,&npass,request,nprop,iprop,iz,nlayers,
                        my_endian,endian,&maxdepth,&mindepth,nsamp,nvklist,vklist,vflag,
                        vdt,vdelrt,need_stats,layer_boundary,verbose);
            /* If at least one trace was passed, bump ensnum */
            if(npass)
              ensnum++;
            if(!strcmp(request,"summary"))
              stats.ntr += npass;
            /* Timeslice */
            if(timeslice.ntimes)
              save_timeslice(traces,nread,timeslice.ntimes,timeslice.times,ofp);
          }
          else {
            kindex += key_vector[0][kindex+1] + 2;
            if(kindex >= hdr_limits[0].bhp_hdr_vlen)
              break;
          }
        }
      }
    }
    /* Read all primary */
    else {
      /* Loop thru primary header key vector */
      kindex = 0;
      for(;;) {
        /* Build readlist for number of ensembles to combine */
        nread = build_readlist(key_vector[0],hdr_limits[0].bhp_hdr_vlen,&kindex,combine,readlist);
        /* If end of key-vector, nread is zero */
        if(verbose)
          fprintf(stderr,"NREAD %d\n", nread);
        if(!nread)
          break;
        read_traces(nread,traces,readlist,key_index[0],fp,my_endian,verbose);
        npass = 0;
        pass_traces(nread,traces,nkeys,korder,nklist,klist,read_order,binsize,imark,iens,ensnum,
                    key_index,ngroup,nvsize,val_list,pass,&npass,request,nprop,iprop,iz,nlayers,
                    my_endian,endian,&maxdepth,&mindepth,nsamp,nvklist,vklist,vflag,
                    vdt,vdelrt,need_stats,layer_boundary,verbose);
        /* If at least one trace was passed, bump ensnum */
        if(npass)
          ensnum++;
        if(!strcmp(request,"summary"))
          stats.ntr += npass;
        if(timeslice.ntimes)
          save_timeslice(traces,nread,timeslice.ntimes,timeslice.times,ofp);
      }
    }
  }
  /* If interpolating, treat each primary-secondary pair as a coordinate location */
  else {
    /* if "normal" reading, put klists in arblist */
    if(arbflag == 0) {
      narb = 2*nklist[0]*nklist[1];
      arblist = calloc(narb,sizeof(float));
      for(i=0; i<nklist[0]; i++) {
        for(j=0; j<nklist[1]; j+=2) {
          arblist[i*nklist[1]+j] = klist[0][i];
          arblist[i*nklist[1]+j+1] = klist[1][j];
        }
      }
    }
    /* If arbflag set, treat pairs of keys as line end-points */
    else if(arbflag == 1) {
      /* Process pairs of entries from keylist */
      pk1 = atoi(strtok(keylist,","));
      sk1 = atoi(strtok(NULL,","));
      pk2 = atoi(strtok(NULL,","));
      sk2 = atoi(strtok(NULL,","));
    }
    for(;;) {
      if(arbflag == 1) {
        arblist = build_line(pk1,pk2,sk1,sk2,1,1,&narb,verbose);
        /* constrain list to key limits */
        for(i=0; i<2*narb; i+=2) {
          if(arblist[i] < key_min[0])
            arblist[i] = key_min[0];
          if(arblist[i] > key_max[0])
            arblist[i] = key_max[0];
          if(arblist[i+1] < key_min[1])
            arblist[i+1] = key_min[1];
          if(arblist[i+1] > key_max[1])
            arblist[i+1] = key_max[1];
        }
      }
      npass = 0;
      for(j=0; j<2*narb; j+=2) {
        if((get_corners(arblist[j],arblist[j+1],key_min,key_incr,key_max,corner,verbose)) != 0) {
          if(verbose)
           fprintf(stderr,"X,Y=%f,%f, C1: %d, %d, C2: %d, %d, C3: %d, %d. C4: %d, %d\n",
                   arblist[j],arblist[j+1],corner[0][0],corner[0][1],corner[1][0],corner[1][1],
                   corner[2][0],corner[2][1],corner[3][0],corner[3][1]);
          if(interp_type == 1) {
            k = get_nearest(arblist[j],arblist[j+1],corner,verbose);
            get_trace(fp,traces[4],corner[k][0],corner[k][1],key_vector[0],hdr_limits[0].bhp_hdr_vlen,
                      key_vector[1], hdr_limits[1].bhp_hdr_vlen,file_hdr.nsamp,file_hdr.endian,
                      my_endian,verbose);
          }
          else if(interp_type == 2) {
            compute_fractions(arblist[j],arblist[j+1],corner,&xfrac,&yfrac,verbose);
            if(verbose)
              fprintf(stderr,"xfrac=%f,yfrac=%f\n",xfrac,yfrac);
            for(k=0; k<4; k++)
              get_trace(fp,traces[k],corner[k][0],corner[k][1],key_vector[0],hdr_limits[0].bhp_hdr_vlen,
                        key_vector[1],hdr_limits[1].bhp_hdr_vlen,file_hdr.nsamp,
                        file_hdr.endian,my_endian,verbose);
            memcpy((void *)traces[4],(const void *)traces[0],HDRBYTES);
            bilin_interp_trace(traces,traces[4],xfrac,yfrac,NULLVAL,verbose);
            hval.i = arblist[j];
            puthval(traces[4],key_index[0],&hval);
            hval.i = arblist[j+1];
            puthval(traces[4],key_index[1],&hval);
          }
          npass++;
          if(nprop != 0)
            build_trace(traces[4],file_hdr.nprop,nprop,iprop,iz,nlayers,layer_boundary,
                        &maxdepth,&mindepth,nvklist,vklist,&stats,verbose);
          hval.i = 1;
          puthval(traces[4],iens,&hval);
          hval.h = 0;
          if(j + 1 == 2 * narb - 1 && npair == 2)
            hval.h = 1;
          puthval(traces[4],imark,&hval);
          if(vflag == 1 && nprop == 0)
            subset_trace(traces[4],traces[4]->ns,nvklist,vklist,vdt,vdelrt,verbose);
          if(!strcmp(request,"summary")) {
            stats.ntr++;
            if(need_stats == 1)
              trace_stats(traces[4],nlayers,nvklist,nprop,iprop,layer_boundary,verbose);
          }
          else if(!strcmp(request,"data")) {
            nsamp = traces[4]->ns;
            check_endian(my_endian,endian,traces[4],nsamp,verbose);
            write_trace(traces[4],nsamp);
          }
        }
      }
      free(arblist);
      if(npass)
        ensnum++;
      if(arbflag == 1) {
        if((p1 = strtok(NULL,",")) != NULL) {
          pk1 = pk2;
          sk1 = sk2;
          pk2 = atoi(p1);
          sk2 = atoi(strtok(NULL,","));
          npair--;
          continue;
        }
        else
          break;
      }
      else if(arbflag == 0)
        break;
    }
  }

  for(i=0; i<file_hdr.nparts; i++)
    efclose(fp[i]);

  /* Close timeslices, and start suximage */
  if(timeslice.ntimes)
    close_tfiles(&timeslice,ofp,verbose);

  /*free(path1);
  if(hdr_limits != NULL)
    free(hdr_limits);
  if(binsize != NULL)
    free(binsize);
  if(nklist != NULL)
    free(nklist);
  if(korder != NULL)
    free(korder);
  if(klist != NULL) {
    for(i=0; i<nkeys; i++)
      free(klist[i]);
    free(klist);
  }
  if(key_vector != NULL) {
    for(i=0; i<nkeys; i++)
      free(key_vector[i]);
    free(key_vector);
  }
  if(key_index != NULL)
    free(key_index);
  if(read_order != NULL)
    free(read_order);
  if(readlist != NULL)
    free(readlist);
  if(traces != NULL) {
    if(!interp_type) {
      for(i=0; i<combine*maxens; i++)
        free(traces[i]);
    }
    else {
      for(i=0; i<5; i++)
        free(traces[i]);
    }
    free(traces);
    if(!interp_type)
      free(pass);
  }
 fprintf(stderr,"EOF\n");
  free(filesys); */

  /* write stats */
  if(!strcmp(request,"summary")) {
    if(!strcmp(data_type,"PROPERTY") && nprop > 0) {
      stats.n1 = maxdepth;
      stats.n2 = mindepth;
    }
    /* use single-value stats for seismic and layer_boundary */
    if(file_hdr.nprop == 0 || (file_hdr.nprop != 0 && nprop == 0) || layer_boundary == 1) {
      if(need_stats == 1 && stats.nvals[0] > 0) {
        file_hdr.meanval /= stats.nvals[0];
        file_hdr.rmsval = sqrt(file_hdr.rmsval / stats.nvals[0]);
      }
      printf("%f %f %f %f %d %d %d %f %f\n",file_hdr.minval,file_hdr.maxval,
           file_hdr.meanval,file_hdr.rmsval,stats.nsamp,stats.ntr,file_hdr.units,stats.n1,stats.n2);
    }
    /* use multi-value stats for properties and horizons */
    else if(file_hdr.nprop != 0 && nprop > 0) {
      /* stats index for first property in properties= or horizon in horizons= */
      if(file_hdr.nprop > 0)
        i = iprop[0] / nlayers;
      else
        i = iprop[0] - 1;
      if(need_stats == 1 && stats.nvals[i] > 0) {
        file_hdr.prop_meanval[i] /= stats.nvals[i];
        file_hdr.prop_rmsval[i] = sqrt(file_hdr.prop_rmsval[i] / stats.nvals[i]);
      }
      printf("%f %f %f %f %d %d %d %f %f\n",file_hdr.prop_minval[i],file_hdr.prop_maxval[i],
             file_hdr.prop_meanval[i],file_hdr.prop_rmsval[i],stats.nsamp,stats.ntr,
             file_hdr.units,stats.n1,stats.n2);
    }
  }

  return EXIT_SUCCESS;
}

void sort_keys(int nkeys, segy **traces, int nt, int ns, int *key_index,
                 bhp_hdr_limits *hdr_limits, Value *val_list, size_t nvsize, int *order)

{

  int i, j;

  Value hval;
  Value *vptr;

  /* Sort trace indices for all keys except primary */
  vptr = val_list;
  for(i=0; i<nt; i++) {
    vptr++->l = i;
    for(j=0; j<nkeys; j++) {
    /* Get header values */
      gethval(traces[i],key_index[j],&hval);
      *vptr++ = order[j] ? hval : negval(type[j],hval);
    }
  }
  qsort(val_list,nt,nvsize,cmp_list);

}

/* Comparison routine for qsort */
int cmp_list(const void *a, const void *b)
{
        register int i;
        Value va, vb;
        register const Value *pa, *pb;
        int compare;

        pa = (Value *) a;
        pb = (Value *) b;

        /* Can order as soon as any components are unequal */
        for (i = 0; i < nkeys; ++i) {
                va = *++pa; vb = *++pb; /* advance and dereference */
                if ((compare = valcmp(type[i], va, vb)))
                        return compare;
        }
        return 0;
}

/* Reverse sign of value */
Value negval(cwp_String type, Value val)
{
        switch (*type) {
        case 'h':
                val.h = -val.h;
        break;
        case 'u':
                val.u = USHRT_MAX -val.u;
        break;
        case 'l':
                val.l = -val.l;
        break;
        case 'v':
                val.v = ULONG_MAX -val.v;
        break;
        case 'i':
                val.i = -val.i;
        break;
        case 'p':
                val.p = (int) -val.p;
        break;
        case 'f':
                val.f = -val.f;
        break;
        case 'd':
                val.d = -val.d;
        break;
        default: err("%d: mysterious type %s", __LINE__, type);
        }

        return val;
}

int build_readlist(int *key_vector, int vlen, int *loc, int combine, int *readlist)
{

  int i, j, k;
  int nread;

  nread = 0;
  k = 0;
  for(i=0; i<combine; i++) {
    if(*loc < vlen) {
      for(j=*loc; j<*loc+key_vector[*loc+1]; j++) {
        readlist[k] = key_vector[j+2];
        k++;
        nread++;
      }
      *loc += key_vector[*loc+1] + 2;
      if(*loc > vlen)
        break;
    }
  }
  return nread;
}

void read_traces(int nread,segy **traces, int *readlist, int key_index, FILE **fp,
                 int my_endian, int verbose)
{

  int i;
  int key;     /* Combined ensemble key */

  Value hval;  /* Trace header value */

  for(i=0; i<nread; i++) {
    efseek(fp[readlist[i]/1000000],
          (readlist[i] % 1000000)*(file_hdr.nsamp+60)*sizeof(float),SEEK_SET);
    efread(traces[i],sizeof(float),file_hdr.nsamp+(HDRBYTES/4),fp[readlist[i]/1000000]);
    check_endian(file_hdr.endian,my_endian,traces[i],(short)file_hdr.nsamp,verbose);
    /* Save primary key from 1st trace */
    if(i == 0) {
      gethval(traces[i],key_index,&hval);
      key = vtoi(hdr_limits[0].bhp_hdr_data,hval);
    }
    else {
      /* Set all primary keys equal to 1st in combined ensemble */
      hval.i = key;
      puthval(traces[i],key_index,&hval);
    }
  }
}
void pass_traces(int nread, segy **traces, int nkeys, int *korder, int *nklist,
                 int **klist, int *read_order, int *binsize, int imark, int iens,
                 int ensnum, int *key_index, int ngroup, size_t nvsize,
                 Value *val_list, int *pass, int *npass, char *request, int nprop,
                 int *iprop, int iz, int nl, int endian_in, int endian,
                 float *maxdepth, float *mindepth, short nsamp, int nvklist,
                 float *vklist, int vflag, float vdt, float vdelrt, int need_stats,
                 int layer_boundary, int verbose)
{

  int i, ikey, found, trkey, itr;
  int m;

  Value hval;

  if(nkeys > 1) {
    sort_keys(nkeys,traces,nread,file_hdr.nsamp+(HDRBYTES/4),key_index,hdr_limits,
              val_list,nvsize,korder);
    /* Set all pass flags true */
    for(i=0; i<nread; i++)
      pass[i] = 1;
    /* Loop thru traces, in sort order */
    for(i=0; i<nread; i++) {
      m = val_list[i*ngroup].l;
      /* See if trace m is on all klists */
      for(ikey=1; ikey<nkeys; ikey++) {
        if(read_order[ikey] == 2) {
          /* Set flag to 'not found' */
          found = 0;
          for(itr=0; itr<nklist[ikey]; itr++) {
            gethval(traces[m],key_index[ikey],&hval);
            trkey = vtoi(hdr_limits[ikey].bhp_hdr_data,hval);
            if(trkey >= (klist[ikey][itr] - binsize[ikey]) &&
               trkey <= (klist[ikey][itr] + binsize[ikey])) {
              found = 1;
              break;
            }
          }
          /* If not found, reset pass flag */
          if(!found)
            pass[m] = 0;
        }
      }
    }
    /* Reset found flag, use to find last trace to be passed */
    found = 0;
    /* Find last trace to be passed, and set end-ensemble (mark=1) */
    for(i=nread-1; i>=0; i--) {
      m = val_list[i*ngroup].l;
      hval.i = ensnum;
      puthval(traces[m],iens,&hval);
      /* If pass flag is set, trace is on all key lists */
      if(pass[m] && !found) {
        found = 1;
        hval.h = 1;
        puthval(traces[m],imark,&hval);
      }
      else if(pass[m]) {
        hval.h = 0;
        puthval(traces[m],imark,&hval);
      }
    }
    /* Pass all traces, in sort order, for which pass is set */
    for(i=0; i<nread; i++) {
      m = val_list[i*ngroup].l;
      if(pass[m]) {
        (*npass)++;
        if(nprop != 0)
          build_trace(traces[m],file_hdr.nprop,nprop,iprop,iz,nl,layer_boundary,
                      maxdepth,mindepth,nvklist,vklist,&stats,verbose);
        nsamp = traces[m]->ns;
        if(vflag == 1 && nprop == 0)
          subset_trace(traces[m],traces[m]->ns,nvklist,vklist,vdt,vdelrt,verbose);
        if(!strcmp(request,"data")) {
          nsamp = traces[m]->ns;
          check_endian(my_endian,endian,traces[m],nsamp,verbose);
          write_trace(traces[m],nsamp);
        }
        else if(need_stats == 1)
          trace_stats(traces[m],nl,nvklist,nprop,iprop,layer_boundary,verbose);
      }
    }
  }
  /* Only 1 key, pass all traces, set end-ens in last trace */
  else {
    for(i=0; i<nread; i++) {
      hval.i = ensnum;
      puthval(traces[i],iens,&hval);
      if(i == nread-1)
        hval.h = 1;
      else
        hval.h = 0;
      puthval(traces[i],imark,&hval);
      if(nprop != 0)
        build_trace(traces[i],file_hdr.nprop,nprop,iprop,iz,nl,layer_boundary,
                    maxdepth,mindepth,nvklist,vklist,&stats,verbose);
      nsamp = traces[i]->ns;
      if(vflag == 1 && nprop == 0)
        subset_trace(traces[i],traces[i]->ns,nvklist,vklist,vdt,vdelrt,verbose);
      if(!strcmp(request,"data")) {
        nsamp = traces[i]->ns;
        check_endian(my_endian,endian,traces[i],nsamp,verbose);
        write_trace(traces[i],nsamp);
      }
      else if(need_stats == 1)
        trace_stats(traces[i],nl,nvklist,nprop,iprop,layer_boundary,verbose);
    }
    *npass = nread;
  }
}
