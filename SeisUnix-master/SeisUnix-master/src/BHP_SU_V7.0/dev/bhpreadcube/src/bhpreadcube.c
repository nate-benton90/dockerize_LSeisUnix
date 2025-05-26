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
#include <assert.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include "limits.h"
#include "bhpio.h"
#include "bhpiocube.h"
#include "bhp_interp.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
" BHPREAD - Read bhpio dataset",
"                                                                  ",
" bhpread filename=      [optional parameters]",
"                                                                  ",
" Required Parameters:                                             ",
"  filename=             File name which was used by BHPWRITE to create",
"                        the dataset. ",
" Optional Parameters:                                             ",
"  pathlist=             ASCII file containing the same list of paths",
"  'filename'.dat        which were used with BHPWRITE to create the",
"                        dataset",
"  keys=fldr,offset      Up to 5 header keys which were used to",
"                        create the dataset.",
"  keylist=p1,p2:s1-s2   List of trace header values to read for each",
"                        key. Use a colon to delimit the list for each key.",
"                        Valid syntax includes:                ",
"                        k1,k2,...  List of specific keys      ",
"                        k1-k2      Range                      ",
"                        k1-k2[k3]  Range, with increment      ",
"                        p1,s1^p2,s2 Project arbitrary traverse",
"                                   from p1,s1 to p2,s2, and   ",
"                                   read traces along the line ",
"                                   By default, traces are formed",
"                                   by bilinear interpolation. ",
"                                   To get nearest traces along",
"                                   arbitrary line, use interpolation=nearest",
"                        *          Read all                   ",
"  BHPREAD passes traces in the order specified by keylist.     ",
"  If keylist is specified, it must contain an entry for each   ",
"  key in the keys parameter. If keys are not specified, BHPREAD passes",
"  traces in the order they were written by BHPWRITE.",
"  rule=near             Trace selection rule: near, match, all, or stack",
"                        rule=near - select trace nearest requested one",
"                        rule=match - select a trace only if  ",
"                                     it matches the request  ",
"                        rule=all - select all traces in the bin",
"                        rule=stack - stack all traces in the bin",
"                        Stack is normalized by the fold count of each",
"                        sample, and header nhs has number of summed traces.",
"  maxtraces=0           Maximum number of traces to read.      ",
"                        Zero means read all requested data. ",
"                        If maxtraces is specified, and the     ",
"                        specified keylist results in more  ",
"                        than maxtraces, output will be truncated.",
"  request=data          Default action is to pass traces out.  ",
"                        request=summary will pass information  ",
"                        about the requested data instead.     ",
"                        For request=summary, the following ",
"                        information will be written to stdout: ",
"                        min, max, mean, and rms values of the ",
"                        requested data, samples-per-trace, ",
"                        number of traces, sample units(0=milliseconds,1=feet,2=meters)",
"                        time/depth interval, start and end time/depth",
"                        request=fold is used to write fold traces to stdout.",
"                        Fold traces consist of a single sample containing the",
"                        fold value for the keys in the keys= specification,",
"                        and can be piped into bhpwritecube, bhptranspose, and",
"                        bhpwritecube to create a viewable fold map.",
"  foldfile=             Specify a filename to produce an ASCII fold count file.",
"                        Each ASCII record will contain",
"                        key-value key-value... fold count",
"                        NOTE: If both foldfile='file' and request=fold are specified,",
"                        both types of fold data will be written.",
"  properties=...        List of properties to read.",
"                        If the input data were created by      ",
"                        bhproffread, properties should be a    ",
"                        comma-separated list of the model",
"                        properties which are requested",
"  horizons=...          List of horizons.                      ",
"                        If the input data were created by      ",
"                        bhphorizon, horizons should be a       ",
"                        list of the horizons which             ",
"                        are requested                          ",
"  endian=my_endian      Omit endian parameter to write native format. ",
"                        Specify endian=1 to force BIG_ENDIAN   ",
"                        Specify endian=0 to force LITTLE_ENDIAN",
"  timeslice=st,end,inc  Timeslice start time, end time, incr   ",
"  display=yes           Display time slices; no=don't display  ",
"  interpolate=bilinear  Spatial interpolation option           ",
"                        Requires primary and secondary keys    ",
"                        Use interpolate=nearest to get nearest ",
"                        trace instead of interpolating         ",
"  verbose=0             For debug print, use verbose=1",
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


/* Prototypes */
void which_keys(char **keys, int nkeys, int *is_in_list);
int get_corner(float key, int which, int min, int inc, int max, int **corner, int verbose);

int main(int argc, char **argv)
{

  char *out;
  char *filename;        /* user filename param */
  char *keylist;         /* key lists */
  char **keylist_part;   /* each part of keylist between : */
  char fpath[NAMELEN];   /* File header path */
  char path[NAMELEN];    /* File path */
  char cindex[8];        /* For appending partition numbers */
  char *path1;           /* First path from pathlist */
  char *request;         /* request type=data or summary or fold */
  char *interpolate;     /* Spatial interpolation option */
  char *p1;              /* for parsing strings */
  char *rule;            /* near, match, all, or stack */
  char *foldfile="";     /* file to write fold count */
  char *aname;           /* aliased filename */
  char data_type[16];    /* seismic, horizon, property */
  char data_order[16];   /* cross-section, map-view */

  int i, j, k, m;        /* Loop counter */
  int *nklist;           /* Number of entries in each key list */
  int nvklist=0;         /* length of expanded vkey list */
  int nkeys;             /* number of specified keys */
  int fkeys;             /* number of fold keys */
  int verbose;           /* debug printout */
  int *key_index;        /* Key indices in header in cube order */
  int ptr;               /* Pointer to next keylist subset */
  int ensnum=1;          /* ensemble number -- 1,2,... */
  int nprop;             /* Number of model properties requested */
  int *iprop;            /* Sample index of each model property  or horizon */
  int iz=0;              /* Sample index of first top in model */
  int nlayers=INT_MIN;   /* Number of layers in model */
  int layernum=INT_MIN;  /* Layer number for properties=name:n or properties=layer_boundary:n */
  int layer_boundary=0;  /* 1 --> properties=layer_boundary:n specified, else 0 */
  int interp_type=0;     /* Spatial interpolation; 0=none, 1=nearest, 2=bilinear */
  int arbflag=0;         /* Flag for arbitrary access */
  int npair;             /* Key pairs for arbitrary traverses */
  int narb;              /* Length of arbitrary trace list */
  int pk1,pk2,sk1,sk2;   /* Arbitrary line end-points */
  int endian;            /* endian parameter 1=BIG, 0=LITTLE */
  int *is_in_list;       /* Flag for each key in cube, whether in keys parm */
  int *is_in_cube;       /* Flag for each key in keys list, where in cube-oder */
  int *next;             /* next klist entry */
  int ntraces=0;         /* number of traces to read */
  int *key_min;          /* Header min values from hdr_limits in cube order */
  int *key_max;          /* Header max values from hdr_limits in cube order */
  int *key_incr;         /* Header inc values from hdr_limits in cube order */
  int *cube_buffer;      /* Cube read/write buffer */
  int trcount;           /* Number of traces in bin */
  int maxtraces;         /* maxtraces to output */
  int first_trace=1;     /* flag for first output trace */
  int hold=0;            /* last trace flag */
  int fold=0;            /* fold count */
  int **corner;          /* keys of corner traces for interpolation */
  int cube_in_memory=0;  /* 1=cube resident */
#ifdef CWP_BIG_ENDIAN    /* Set default endian */
  int my_endian = 1;
#else
  int my_endian = 0;
#endif
  int vflag=0;           /* =1 is vertical key is specified */
  int need_stats=0;      /* set to 1 if no stats in file_hdr */
  int nkparts=0;         /* number of keylist_parts found in keylist */
  int *ikey_vals;        /* key values for each read as ints */
  int *ftable;           /* fold table, each entry has fold count for cube offset */
  int nfold;             /* length of ftable, in ints */

  short nsamp;           /* nsamp from first trace */

  long offset;           /* cube offset */

  segy **traces;         /* bin of traces */
  segy *trout;           /* next output trace */
  segy **ctrace;         /* traces for each corner needed for interpolation */

  float factor;          /* Scale factor for coordinates */
  float dt;              /* Sampling interval from header divided by UNITS from bhpio.h */
  float vdt;             /* vertical sampling if vflag is set */
  float delrt;           /* Start time from header, assumed to be milliseconds/meters/feet */
  float vdelrt;          /* first vertical value if vflag is set */
  float mindepth=FLT_MAX;/* Min depth, if trace is a model property */
  float maxdepth=FLT_MIN;/* Max depth, if trace is a model property */
  float xfrac, yfrac;    /* fractions for bilinear interp */
  float *vklist;         /* Expanded vkey list */
  float *arblist;        /* Arbitrary line list */
  float x, y;            /* key values for interpolation */
  float **klist;         /* Expanded key lists */
  float *key_vals;       /* key values for each read as floats */

  cwp_String keys[SU_NKEYS];  /* SU header key names */
  cwp_String type[SU_NKEYS];  /* header key types */

  FILE **ofp;               /* Timeslice file pointers */
  FILE *ffp=NULL;           /* foldfile pointer */
  FILE *fp[NFILES];         /* Open file pointers */

  Value hval1;              /* Trace header value */
  Value hval2;              /* Trace header value */

  struct rlimit rlp;        /* setrlimit structure */

  timeslice_def timeslice;
  timeslice.ntimes = 0;

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  /* print out param */
  if(!getparstring("out",&out))
    out = "NONE";
  else {
    fprintf(stderr,"out=%s\n",out);
    fflush(stderr);
  }

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* filename */
  filename = get_filename_pathlist(verbose);
  /* save as aname in case aliased */
  aname = calloc((int)strlen(filename)+1,sizeof(char));
  strcpy(aname,filename);

  /* get path to first partition */
  path1 = get_path1(filename,verbose);

  /* check for alias */
  i = (int)strlen(filename);
  if(!strcmp(&filename[i-10],"_as_events")) {
    /* trim */
    filename[i-10] = '\0';
    if(verbose) {
      fprintf(stderr,"Aliased filename %s\n",aname);
      fprintf(stderr,"Trimmed filename %s\n",filename);
    }
  }

  /* alloc name in file_hdr */
  file_hdr.filename = calloc((int)strlen(filename)+1,sizeof(char));

  /* set request, initialize stats */
  if(!getparstring("request",&request))
    request = "data";
  if(!strcmp(request,"summary"))
    stats.ntr = 0;

  /* Make sure file is there */
  if(!get_bhpio_path(path1,aname,"_0000.HDR",fpath,verbose))
    err("FILE: path=%s, filename=%s does not exist\n",path1,filename);
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

  /* data type and order */
  if(file_hdr.data_type == 1 || (file_hdr.data_type == 0 && file_hdr.nprop == 0))
    strcpy(data_type,"SEISMIC");
  else if(file_hdr.data_type == 2 || (file_hdr.data_type == 0 && file_hdr.nprop < 0))
    strcpy(data_type,"HORIZON");
  else if(file_hdr.data_type == 3 || (file_hdr.data_type == 0 && file_hdr.nprop > 0))
    strcpy(data_type,"PROPERTY");
  if(file_hdr.data_order <= 1)
    strcpy(data_order,"CROSS-SECTION");
  else if(file_hdr.data_order == 2)
    strcpy(data_order,"MAP-VIEW");

  /* hdr_limits */
  hdr_limits = calloc(file_hdr.nkeys,sizeof(bhp_hdr_limits));
  /* Alloc key indices, min, max, inc */
  key_index = calloc(file_hdr.nkeys,sizeof(int));
  key_min = calloc(file_hdr.nkeys,sizeof(int));
  key_max = calloc(file_hdr.nkeys,sizeof(int));
  key_incr = calloc(file_hdr.nkeys,sizeof(int));
  if(verbose)
    fprintf(stderr,"Allocated %d bytes for hdr_limits\n", file_hdr.nkeys*sizeof(bhp_hdr_limits));
  if(!get_bhpio_path(path1,filename,"_0000.0001.HDR",fpath,verbose))
    err("Could not access file: %s\n", fpath);
  /* Load hdr_limits */
  if(read_hdr_limits(fpath,verbose))
    err("Error reading %s\n", fpath);
  for(i=0; i<file_hdr.nkeys; i++) {
    key_index[i] = getindex(hdr_limits[i].bhp_hdr_name);
    key_min[i] = hdr_limits[i].bhp_hdr_min;
    key_max[i] = hdr_limits[i].bhp_hdr_max;
    key_incr[i] = hdr_limits[i].bhp_hdr_inc;
    factor = hdr_limits[i].bhp_hdr_scalar;
    if(factor < 0)
      factor = -1. / factor;
    hdr_limits[i].bhp_hdr_min *= factor;
    hdr_limits[i].bhp_hdr_max *= factor;
  }

  i = sysconf(_SC_OPEN_MAX);
  if(verbose)
    fprintf(stderr,"_SC_OPEN_MAX=%d\n",i);
  /* get current limits */
  if(i = getrlimit(RLIMIT_NOFILE,&rlp) != 0)
    perror("first getrlimit");
  if(verbose)
    fprintf(stderr,"Soft Limit: %d, Hard Limit: %d\n",(int)rlp.rlim_cur,(int)rlp.rlim_max);
  /* increase soft limit if necessary */
  if((int)rlp.rlim_cur < file_hdr.nparts + 10) {
    rlp.rlim_cur = (rlim_t)((int)rlp.rlim_cur + (file_hdr.nparts + 10 - (int)rlp.rlim_cur));
    fprintf(stderr,"Increasing soft limit to %d\n",(int)rlp.rlim_cur);
    if(i = setrlimit(RLIMIT_NOFILE,&rlp) != 0)
      perror("setrlimit");
    /* get new limits */
    if(i = getrlimit(RLIMIT_NOFILE,&rlp) != 0)
      perror("second getrlimit");
    fprintf(stderr,"Soft Limit: %d, Hard Limit: %d\n",(int)rlp.rlim_cur,(int)rlp.rlim_max);
  }

  /* alloc partition_map to keep track of partition numbers associated with each fp[] */
  partition_map = calloc(file_hdr.nparts,sizeof(int));
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
      fprintf(stderr,"Opening %s using fp[%d]\n",path,i);
    fp[i] = fopen(path,"rb");
    if(!fp[i]) {
      perror("bhpreadcube: fopen");
      err("Could not open %s\n", path);
    }
    partition_map[i] = get_part(path,verbose);
  }
  if(verbose > 0)
    fprintf(stderr,"BHPREADCUBE: Opened %d files\n",file_hdr.nparts);

  if(verbose > 0) {
    fprintf(stderr,"Partition Map\n");
    for(i=0; i<file_hdr.nparts; i++)
      fprintf(stderr,"%d ",partition_map[i]);
    fprintf(stderr,"\n");
  }

  /* Read first trace, get info from header */
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
    fprintf(stderr,"Number of Keys: %d\n", file_hdr.nkeys+1);
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

  /* if request=data or request=fold verify stdout */
  if(!strcmp(request,"data") || !strcmp(request,"fold"))
    check_stdout();

  /* endian */
  endian = get_endian(my_endian,verbose);

  /* vertical key */
  if(!strcmp(file_hdr.vkey,"")) {
    strcpy(file_hdr.vkey,"tracl");
    if(!strcmp(data_type,"PROPERTY") || !strcmp(data_type,"HORIZON")) {
      file_hdr.vmin = 1;
      if(!strcmp(data_type,"PROPERTY"))
        file_hdr.vmax = (file_hdr.nsamp-1)/(file_hdr.nprop+1);
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

  /* calc nlayers if map-view model data */
  if(!strcmp(data_order,"MAP-VIEW") && file_hdr.nprop > 0)
   nlayers = hdr_limits[0].bhp_hdr_max/file_hdr.nprop;
  /* check properties and horizons */
  iprop = get_prop_horz(properties,&nprop,&iz,&nlayers,file_hdr.nprop,file_hdr.nsamp,
                        &layernum,&layer_boundary,verbose);
  /* if name specified without layer number, ignore for map-view data, use keylist */
  if(layernum == INT_MIN && !strcmp(data_order,"MAP-VIEW") && !strcmp(data_type,"PROPERTY")) {
    if(verbose)
      fprintf(stderr,"Ignore 'properties=name' for MAP-VIEW order\n");
    nprop = 0;
    need_stats = 1;
    stats.nvals = calloc(1,sizeof(int));
  }
  /* if layer_boundary is set, reset any stats in file_hdr */
  if(layer_boundary == 1) {
    file_hdr.minval = FLT_MAX;
    file_hdr.maxval = -FLT_MAX;
    file_hdr.meanval = 0;
    file_hdr.rmsval = 0;
    need_stats = 1;
    stats.nvals = calloc(1,sizeof(int));
  }

  /* arrays for key parsing */
  ikey_vals = calloc(file_hdr.nkeys,sizeof(int));
  key_vals = calloc(file_hdr.nkeys,sizeof(float));
  nklist = calloc(file_hdr.nkeys,sizeof(int));
  klist = calloc(file_hdr.nkeys,sizeof(float *));
  next = calloc(file_hdr.nkeys,sizeof(int));
  is_in_list = calloc(file_hdr.nkeys,sizeof(int));
  is_in_cube = calloc(file_hdr.nkeys,sizeof(int));
  /* keylist part holds each colon-separated part of keylist */
  keylist_part = calloc(file_hdr.nkeys+1,sizeof(char *));
  for(i=0; i<file_hdr.nkeys+1; i++)
    keylist_part[i] = calloc(256,sizeof(char));

  /* if keylist= without keys= err */
  if((i = countparval("keys")) == 0 && (j = countparval("keylist")) != 0)
    err("If keylist is specified, keys must be specified\n");

  /* check keys, if specified */
  if((nkeys = countparval("keys")) != 0) {
    getparstringarray("keys",keys);
    /* vertical key must be last, if specified */
    if(!strcmp(file_hdr.vkey,keys[nkeys-1])) {
      vflag = 1;
      /* remove vkey from keys */
      nkeys--;
      if(file_hdr.vinc ==  0)
        err("Vertical sub-set not allowed. Vertical key was not saved when file was created\n");
    }
    /* dont allow vertical key elsewhere */
    else {
      for(i=0; i<nkeys-1; i++) {
        if(!strcmp(file_hdr.vkey,keys[i]))
          err("Vertical key must be specified last\n");
      }
    }
    /* Verify requested keys are available, fill in cube position */
    for(i=0; i<nkeys; i++) {
      is_in_cube[i] = -1;
      for(j=0; j<file_hdr.nkeys; j++) {
        if(!strcmp(keys[i],hdr_limits[j].bhp_hdr_name)) {
          is_in_cube[i] = j;
          break;
        }
      }
      if(is_in_cube[i] < 0)
        err("%s was not saved when %s was created\n", keys[i],file_hdr.filename);
    }
    /* Fill in cube-keys location in keys list */
    which_keys(keys,nkeys,is_in_list);
    if(verbose) {
      for(i=0; i<file_hdr.nkeys; i++)
        fprintf(stderr,"%s in-list is  %d\n",hdr_limits[i].bhp_hdr_name,is_in_list[i]);
      for(i=0; i<nkeys; i++)
        fprintf(stderr,"%s in-cube is  %d\n",keys[i],is_in_cube[i]);
    }
    /* Get key types */
    for (i=0; i<nkeys; i++) {
      type[i] = hdtype(keys[i]);
      if(verbose) {
        fprintf(stderr,"Requested KEYS:\n");
        fprintf(stderr,"  Key: %s, Type: %s\n", keys[i], type[i]);
      }
    }
    /* initialize keylist_parts to all wildcards */
    for(i=0; i<file_hdr.nkeys; i++)
      strcpy(keylist_part[i],"*");

    /* keylist */
    if(getparstring("keylist",&keylist)) {
      /* count colon-separated parts, check for ^ */
      i = strlen(keylist);
      k = 0;
      for(j=0; j<i; j++) {
        if(keylist[j] == ':')
          k++;
      }
      nkparts = k + 1;
      /* separate keylist into keylist_parts */
      ptr = 0;
      for(i=0; i<nkparts; i++)
        get_keylist_subset(&keylist[ptr],keylist_part[i],&ptr);
      if(verbose) {
        fprintf(stderr,"Keylist: %s, has %d parts\n",keylist,nkparts);
        for(i=0; i<nkparts; i++)
          fprintf(stderr," %s ",keylist_part[i]);
        fprintf(stderr,"\n");
      }
      /* check each keylist_part for arb traverse */
      for(i=0; i<nkparts; i++) {
        if(i == 0 && strstr(keylist_part[i],"^") != NULL)
          arbflag = 1;
        else if(i > 0 && strstr(keylist_part[i],"^") != NULL)
          err("Arbitrary traverse must be specified in first part of keylist\n");
      }
      /* if arbflag set, move keylist_parts down one slot */
      if(arbflag == 1) {
        nkparts++;
        for(i=nkparts-1; i>0; i--)
          strcpy(keylist_part[i],keylist_part[i-1]);
        if(verbose) {
          fprintf(stderr,"Updated Keylist: ");
          for(i=0; i<nkparts; i++)
            fprintf(stderr," %s ",keylist_part[i]);
          fprintf(stderr,"\n");
        }
      } 
      /* check nkparts vs nkeys */
      if((vflag == 1 && nkparts != (nkeys + 1)) || (vflag == 0 && nkparts != nkeys))
        err("Number of keylist parts must be same as number of keys\n");
      /* if vflag set, get vertical keylist */
      if(vflag == 1) {
        /* by rule, vertical part of keylist is last part */
        /* treat * as no vertical key */
        if(!strcmp(keylist_part[nkparts-1],"*")) {
          vflag = 0;
          if(verbose)
            fprintf(stderr,"Vertical Keylist is *\n");
        }
      }
      if(vflag == 1) {
        /* parse as floats */
        vklist = parse_keylistf(keylist_part[nkparts-1],file_hdr.vmin,file_hdr.vmax,
                                file_hdr.vinc,&nvklist,0,"near",verbose);
        if(verbose) {
          fprintf(stderr,"Vertical Keylist: ");
          for(i=0; i<nvklist; i++)
            fprintf(stderr," %.3f ",vklist[i]);
          fprintf(stderr,"\n");
        }
      }
      /* remove vklist from keylist */
      nkparts--;
    }

    /* if foldfile= or request=fold */
    if(getparstring("foldfile",&foldfile)) {
      ffp = fopen(foldfile,"w");
      if(ffp == NULL)
        err("Could not open %s\n", foldfile);
    }
    if(ffp || !strcmp(request,"fold")) {
      /* restriction: fold not allowed for arb traverse - could change */
      if(arbflag == 1)
        err("Fold output not implemented for arbitrary traverse");
      fkeys = nkeys;
      if(verbose) {
        fprintf(stderr,"Writing fold count for ");
        for(i=0; i<fkeys; i++)
          fprintf(stderr,"%s ",keys[i]);
        if(ffp && !strcmp(request,"fold"))
          fprintf(stderr," to %s and stdout\n",foldfile);
        else if(ffp)
          fprintf(stderr," to %s\n",foldfile);
        else if(!strcmp(request,"fold"))
          fprintf(stderr," to stdout\n");
      }
    }

    /* For keys not specified, fill in wildcard */
    for(i=0,k=nkeys; i<file_hdr.nkeys; i++) {
      if(is_in_list[i] < 0) {
        is_in_list[i] = k;
        is_in_cube[is_in_list[i]] = i;
        keys[k] = calloc(8,sizeof(char));
        strcpy(keys[k],hdr_limits[i].bhp_hdr_name);
        strcpy(keylist_part[k],"*");
        if(verbose)
          fprintf(stderr,"Added %s to keylist, in_list is %d\n",keys[k],is_in_list[i]);
        k++;
        nkeys++;
      }
    }

    /* if arb traverse, build line-segment list from first keylist part */
    if(arbflag == 1) {
      arblist = build_lines(keylist_part[0],&narb,key_incr[is_in_cube[0]],
                            key_incr[is_in_cube[1]],verbose);
      nklist[0] = narb;
      nklist[1] = narb;
      klist[0] = calloc(narb,sizeof(float));
      klist[1] = calloc(narb,sizeof(float));
      for(i=0,j=0; i<narb; i++,j+=2) {
        klist[0][i] = arblist[j];
        klist[1][i] = arblist[j+1];
      }
      if(verbose) {
        fprintf(stderr,"Arbitrary List before constrain\n");
        for(i=0; i<narb; i++) {
          fprintf(stderr,"%f,%f ",klist[0][i],klist[1][i]);
          if(!((i+1)%4))
            fprintf(stderr,"\n");
        }
        fprintf(stderr,"\n");
      }
      /* constrain list items to hdr-limits */
      for(i=0; i<narb; i++) {
        if(klist[0][i] < hdr_limits[is_in_cube[0]].bhp_hdr_min)
          klist[0][i] = hdr_limits[is_in_cube[0]].bhp_hdr_min;
        if(klist[0][i] > hdr_limits[is_in_cube[0]].bhp_hdr_max)
          klist[0][i] = hdr_limits[is_in_cube[0]].bhp_hdr_max;
        if(klist[1][i] < hdr_limits[is_in_cube[1]].bhp_hdr_min)
          klist[1][i] = hdr_limits[is_in_cube[1]].bhp_hdr_min;
        if(klist[1][i] > hdr_limits[is_in_cube[1]].bhp_hdr_max)
          klist[1][i] = hdr_limits[is_in_cube[1]].bhp_hdr_max;
      }
      if(verbose) {
        fprintf(stderr,"Arbitrary List after constrain\n");
        for(i=0; i<narb; i++) {
          fprintf(stderr,"%f,%f ",klist[0][i],klist[1][i]);
          if(!((i+1)%4))
            fprintf(stderr,"\n");
        }
        fprintf(stderr,"\n");
      }
    }

    /* parse keylist_parts into expanded keylists */
    /* arb traverse set rule=near, interp default to bilinear */
    if(arbflag == 1) {
      rule = "near";
      /* set interpolation=bilinear if not specified */
      if(interp_type == 0)
        interp_type = 2;
      if(verbose) {
        if(interp_type == 1)
          fprintf(stderr,"Building output traces via arbitrary lines, using nearest trace\n");
        else if(interp_type == 2)
          fprintf(stderr,"Building output traces via arbitrary lines, using bilinear interpolation\n");
      }
    }
    /* if not arbitrary traverse, get selection criteria */
    else if(arbflag == 0) {
      if(!getparstring("rule",&rule))
        rule = "near";
      if(strcmp(rule,"near") && strcmp(rule,"match") &&
         strcmp(rule,"all") && strcmp(rule,"stack"))
        err("rule=%s is illegal, use near, or match, or all, or stack\n", rule);
      if(verbose)
        fprintf(stderr,"Trace Selection: %s\n", rule);
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
      /* Need at least 2 keys */
      if(nkeys < 2)
        err("interpolation requires at least 2 keys\n");
    }

    if(verbose) {
      fprintf(stderr,"Keylist Parts: ");
      for(i=0; i<nkeys; i++)
        fprintf(stderr,"%s  ",keylist_part[i]);
      fprintf(stderr,"\n");
      fprintf(stderr,"Expanded Keylists\n");
    }
    for(i=0; i<nkeys; i++) {
      if((arbflag == 1 && i > 1) || arbflag == 0)
        klist[i] = parse_keylistf(keylist_part[i],hdr_limits[is_in_cube[i]].bhp_hdr_min,
                                  hdr_limits[is_in_cube[i]].bhp_hdr_max,
                                  hdr_limits[is_in_cube[i]].bhp_hdr_inc,&nklist[i],
                                  interp_type,rule,verbose);
      if(verbose) {
        for(j=0; j<nklist[i]; j++) {
          fprintf(stderr,"%.2f ", klist[i][j]);
          if(!((j+1)%8))
            fprintf(stderr,"\n");
        }
        fprintf(stderr,"\n");
      }
    }      
    /* alloc fold table */
    if(ffp || !strcmp(request,"fold"))
      ftable = alloc_fold(fkeys,nklist,&nfold,verbose);

    /* Enforce special syntax rules */
    /* cross-section horizon data: no vertical keylist allowed */
    i = countparval("horizons");
    if(!strcmp(data_order,"CROSS-SECTION") && i > 0) {
      if(vflag == 1 && verbose)
        fprintf(stderr,"Horizons parameter overriding vertical key specification\n");
      vflag = 0;
    }
    /* map-view model or horizons: properties=name:n or horizons=name overrides tracl values in keylist */
    i = countparval("horizons");
    if(!strcmp(data_order,"MAP-VIEW") && (layernum != INT_MIN || i > 0)) {
      if(verbose)
        fprintf(stderr,"horizons=name or properties=name:n overriding tracl keylist specification\n");
      /* find tracl in keylist */
      for(i=0; i<nkeys; i++) {
        if(!strcmp(keys[i],"tracl")) {
          free(klist[i]);
          nklist[i] = 1;
          klist[i] = calloc(1,sizeof(int));
          /* if layernmum is set, add it to property index, else use horizon number */
          if(layernum != INT_MIN)
            klist[i][0] = iprop[0] + layernum;
          else
            klist[i][0] = iprop[0];
        }
      }
    }      
    /* cross-section model: properties=name:n overrides vkey specification */
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
            if(vklist[i] > (file_hdr.nsamp - 1) / (file_hdr.nprop + 1) - 1)
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
    
    /* Alloc read/write buffer big enough for bin size */
    cube_buffer = calloc(file_hdr.bin,sizeof(int));
    /* Open cube */
    get_bhpio_path(path1,filename,"_0000.0002.HDR",fpath,verbose);
    cubefd = open(fpath,O_RDONLY);
    if(cubefd == -1)
      err("Failed to open cube %s\n", fpath);
    /* If cube will fit in memory, load it */
    if(file_hdr.cube_size <= MAX_CUBE_SIZE) {
      if(verbose) {
        fprintf(stderr,"Processing cube in memory\n");
        fprintf(stderr,"Allocating %d ints for cube\n", file_hdr.cube_size);
      }
      cube = calloc(file_hdr.cube_size,sizeof(int));
      /* Load cube */
      read(cubefd,cube,file_hdr.cube_size*sizeof(int));
      cube_in_memory = 1;
    }
    else
      if(verbose)
        fprintf(stderr,"Processing cube on disk\n");

    /* alloc enough traces to hold a bin */
    traces = calloc(file_hdr.bin,sizeof(float *));
    for(i=0; i<file_hdr.bin; i++)
      traces[i] = calloc(file_hdr.nsamp+(HDRBYTES/4),sizeof(float));
    /* for interpolation allocate traces for each corner */
    if(interp_type != 0) {
      ctrace = calloc(4,sizeof(float *));
      corner = calloc(4,sizeof(int *));
      for(i=0; i<4; i++) {
        ctrace[i] = calloc(file_hdr.nsamp+(HDRBYTES/4),sizeof(float));
        corner[i] = calloc(2,sizeof(int));
      }
    }
    /* Additional trace to facilitate ensemble checking */
    trout = calloc(file_hdr.nsamp+(HDRBYTES/4),sizeof(float));

    /* Set rw_flag for reading */
    rw_flag = 0;

  } /* End if(nkeys)... */

  /* set stats members */
  set_stats(&stats,data_type,data_order,vflag,file_hdr.nsamp,dt,delrt,
            nvklist,vdt,vdelrt,file_hdr.vmin,file_hdr.vinc,
            file_hdr.units,verbose);

  /* timeslice */
  check_timeslice(&timeslice,nkeys,verbose);

  /* Open timeslice files */
  if(timeslice.ntimes) {
    if(dt*file_hdr.nsamp < timeslice.times[timeslice.ntimes-1]) {
      fprintf(stderr,"Restricting time slices to trace length: %d\n", dt*file_hdr.nsamp);
      j = timeslice.ntimes;
      for(i=0; i<j; i++)
        if(timeslice.times[i] > dt*file_hdr.nsamp)
          timeslice.ntimes--;
    }
    /* Alloc ofiles and initialize */
    ofp = make_tfiles(timeslice.ntimes,timeslice.times,timeslice.opath,verbose);
  }

  /* If no keys specified, read data as written */
  if(!nkeys) {
    for(i=0; i<file_hdr.nparts; i++) {
      while(efread(&tr,HDRBYTES,1,fp[i])) {
        efread(tr.data,FSIZE,file_hdr.nsamp,fp[i]);
        stats.ntr++;
        check_endian(file_hdr.endian,my_endian,&tr,(short)file_hdr.nsamp,verbose);
        if(nprop > 0 && !strcmp(data_order,"CROSS-SECTION"))
          build_trace(&tr,file_hdr.nprop,nprop,iprop,iz,nlayers,layer_boundary,
                      &maxdepth,&mindepth,0,0,&stats,verbose);
        /* if summary, save stats, else write data */
        if(!strcmp(request,"summary") && need_stats == 1)
          trace_stats(&tr,nlayers,nvklist,nprop,iprop,layer_boundary,verbose);
        else if(!strcmp(request,"data")) {
          nsamp = tr.ns;
          check_endian(my_endian,endian,&tr,tr.ns,verbose);
          write_trace(&tr,nsamp);
        }
      }
    }
  }
  /* otherwise use keylists */
  else {
    ntraces = 1;
    /* for "normal" access number of traces is product of nklist values */
    if(arbflag == 0) {
      for(i=0; i<file_hdr.nkeys; i++)
        ntraces *= nklist[i];
    }
    /* arb access, use narb as multiplier instead of 1st two keys */
    else if(arbflag == 1) {
      ntraces *= narb;
      for(i=2; i<file_hdr.nkeys; i++)
        ntraces *= nklist[i];
    }
    /* check ntraces vs maxtraces */
    if(!getparint("maxtraces",&maxtraces))
      maxtraces = 0;
    if(maxtraces > 0 && maxtraces < ntraces)
      ntraces = maxtraces;
    /* read traces as per keylist */
    for(i=0; i<file_hdr.nkeys; i++)
      next[i] = 0;
    trcount = 0;
    /* set key values for next read */
    for(i=0; i<ntraces; i++) {
      for(j=0; j<file_hdr.nkeys; j++)
        key_vals[j] = klist[j][next[j]];
      /* get corners if interpolating */
      if(interp_type != 0) {
        if((get_corner(key_vals[0],0,key_min[is_in_cube[0]],key_incr[is_in_cube[0]],
                       key_max[is_in_cube[0]],corner,verbose)) != 0) {
          if((get_corner(key_vals[1],1,key_min[is_in_cube[1]],key_incr[is_in_cube[1]],
                         key_max[is_in_cube[1]],corner,verbose)) != 0) {
            if(verbose)
              fprintf(stderr,"X,Y=%f,%f, C1: %d, %d, C2: %d, %d, C3: %d, %d. C4: %d, %d\n",
                      key_vals[0],key_vals[1],
                      corner[0][0],corner[0][1],corner[1][0],corner[1][1],
                      corner[2][0],corner[2][1],corner[3][0],corner[3][1]);
          }
          else
            err("%f, %f is outside grid\n",key_vals[0],key_vals[1]);
        }
        else
          err("%f, %f is outside grid\n",key_vals[0],key_vals[1]);
      }
      if(interp_type == 2) {
        compute_fractions(key_vals[0],key_vals[1],corner,&xfrac,&yfrac,verbose);
        if(verbose)
          fprintf(stderr,"xfrac=%f,yfrac=%f\n",xfrac,yfrac);
        for(k=0; k<4; k++) {
          /* put key values in cube order as ints */
          ikey_vals[is_in_cube[0]] = corner[k][0];
          ikey_vals[is_in_cube[1]] = corner[k][1];
          for(m=2; m<file_hdr.nkeys; m++)
            ikey_vals[is_in_cube[m]] = key_vals[m];
          offset = cube_offset(ikey_vals,key_min,key_max,key_incr,file_hdr.bin);
          assert(offset >= 0 && offset < file_hdr.cube_size);
          read_cube(offset,cube_buffer,my_endian,cube_in_memory,verbose);
          trcount = get_trace(fp,ikey_vals,key_index,cube_buffer,traces,
                              ctrace[k],"near",file_hdr.endian,my_endian,verbose);
          if(trcount == 0)
            break;
          memcpy((void *)ctrace[k],(const void *)traces[0],HDRBYTES+4*file_hdr.nsamp);
        }
        if(trcount == 1) {
          bilin_interp_trace(ctrace,traces[0],xfrac,yfrac,NULLVAL,verbose);
          memcpy((void *)traces[0],(const void *)ctrace[0],HDRBYTES);
          /* use actual key values for trace header */
          hval1.i = key_vals[0];
          puthval(traces[0],key_index[is_in_cube[0]],&hval1);
          hval1.i = key_vals[1];
          puthval(traces[0],key_index[is_in_cube[1]],&hval1);
        }
        else
          err("Bilinear interpolation failed\n");
      }
      /* interpolate=nearest */
      else if(interp_type == 1) {
        k = get_nearest(key_vals[0],key_vals[1],corner,verbose);
        /* put key values in cube order as ints */
        ikey_vals[is_in_cube[0]] = corner[k][0];
        ikey_vals[is_in_cube[1]] = corner[k][1];
        for(m=2; m<file_hdr.nkeys; m++)
          ikey_vals[is_in_cube[m]] = key_vals[m];
        offset = cube_offset(ikey_vals,key_min,key_max,key_incr,file_hdr.bin);
        assert(offset >= 0 && offset < file_hdr.cube_size);
        read_cube(offset,cube_buffer,my_endian,cube_in_memory,verbose);
        trcount = get_trace(fp,ikey_vals,key_index,cube_buffer,traces,
                            ctrace[k],"match",file_hdr.endian,my_endian,verbose);
        /*memcpy((void *)&tr,(const void *)traces[0],HDRBYTES+4*file_hdr.nsamp);*/
      }
      /* not interpolating */
      else {
        /* put key values in cube order as ints */
        for(k=0; k<file_hdr.nkeys; k++)
          ikey_vals[is_in_cube[k]] = key_vals[k];
        offset = cube_offset(ikey_vals,key_min,key_max,key_incr,file_hdr.bin);
        assert(offset >= 0 && offset < file_hdr.cube_size);
        read_cube(offset,cube_buffer,my_endian,cube_in_memory,verbose);
        trcount = get_trace(fp,ikey_vals,key_index,cube_buffer,traces,&tr,
                            rule,file_hdr.endian,my_endian,verbose);
      }
      /* pass all traces in this bin which passed rule test */
      for(j=0; j<trcount; j++) {
        /* if very first trace, hold so that ensembles can be checked */
        if(first_trace) {
          first_trace = 0;
          memcpy((void *)&tr,(const void *)traces[0],HDRBYTES+4*file_hdr.nsamp);
          /* set flag, trace is in tr */
          hold = 1;
          continue;
        }
        /* copy tr to trout */
        memcpy((void *)trout,(const void *)&tr,HDRBYTES+4*file_hdr.nsamp);
        /* tr now "empty" */
        hold = 0;
        trout->tracr = ensnum;
        stats.ntr++;
        memcpy((void *)&tr,(const void *)traces[j],HDRBYTES+4*file_hdr.nsamp);
        /* set flag indicating trace in tr */
        hold = 1;
        /* are tr and trout same primary? */
        gethval(trout,key_index[is_in_cube[0]],&hval1);
        gethval(&tr,key_index[is_in_cube[0]],&hval2);
        if(hval1.i != hval2.i) {
          trout->wevel = 1;
          ensnum++;
        }
        else
            trout->wevel = 0;
        /* Check fold */
        if(ffp || !strcmp(request,"fold"))
          check_fold(trout,&tr,&fold,ftable,nfold,klist,nklist,key_index,
                     fkeys,0,request,is_in_cube,my_endian,endian,verbose);
        if(nprop > 0 && !strcmp(data_order,"CROSS-SECTION"))
          build_trace(trout,file_hdr.nprop,nprop,iprop,iz,nlayers,layer_boundary,
                      &maxdepth,&mindepth,nvklist,vklist,&stats,verbose);
        else if(vflag == 1)
          subset_trace(trout,trout->ns,nvklist,vklist,vdt,vdelrt,verbose);
        if(!strcmp(request,"data")) {
          nsamp = trout->ns;
          check_endian(my_endian,endian,trout,trout->ns,verbose);
          write_trace(trout,nsamp);
        }
        else if(!strcmp(request,"summary") && need_stats == 1)
          trace_stats(trout,nlayers,nvklist,nprop,iprop,layer_boundary,verbose);
      }
      /* Check key wrap around */
      if(arbflag == 0) {
        for(j=file_hdr.nkeys-1; j>=0; j--) {
          next[j]++;
          if(next[j] == nklist[j])
            next[j] = 0;
          else
            break;
        }
      }
      else if(arbflag == 1 && file_hdr.nkeys > 2) {
        for(j=file_hdr.nkeys-1; j>=2; j--) {
          next[j]++;
          if(next[j] == nklist[j]) {
            next[j] = 0;
            if(j == 2) {
              next[0]++;
              next[1]++;
            }
          }
          else
            break;
        }
      }
      else if(arbflag == 1 && file_hdr.nkeys == 2) {
        next[0]++;
        next[1]++;
      }
    }
    /* if hold is set, last trace is in tr */
    if(hold) {
      /* Check fold */
      if(ffp || !strcmp(request,"fold")) {
        check_fold(trout,&tr,&fold,ftable,nfold,klist,nklist,key_index,
                   fkeys,1,request,is_in_cube,my_endian,endian,verbose);
        write_fold(ffp,ftable,nfold,fkeys,nklist,klist,verbose);
      }
      tr.wevel = 1;
      tr.tracr = ensnum;
      stats.ntr++;
      if(nprop > 0 && !strcmp(data_order,"CROSS-SECTION"))
        build_trace(&tr,file_hdr.nprop,nprop,iprop,iz,nlayers,layer_boundary,
                    &maxdepth,&mindepth,nvklist,vklist,&stats,verbose);
      else if(vflag == 1)
        subset_trace(&tr,tr.ns,nvklist,vklist,vdt,vdelrt,verbose);
      if(!strcmp(request,"data")) {
        nsamp = tr.ns;
        check_endian(my_endian,endian,&tr,tr.ns,verbose);
        write_trace(&tr,nsamp);
      }
      else if(!strcmp(request,"summary") && need_stats == 1)
        trace_stats(&tr,nlayers,nvklist,nprop,iprop,layer_boundary,verbose);
    }
  }

  /* write stats if summary */
  if(!strcmp(request,"summary")) {
    if(!strcmp(data_type,"PROPERTY") && nprop > 0 && !strcmp(data_order,"CROSS-SECTION")) {
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
    /* use multi-value stats for properties  and horizons */
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

  /* close partitions */
  for(i=0; i<file_hdr.nparts; i++)
    fclose(fp[i]);
  if(verbose > 0)
    fprintf(stderr,"BHPREADCUBE: Closed %d files\n",file_hdr.nparts);
  close(cubefd);

  return EXIT_SUCCESS;

}

void which_keys(char **keys, int nkeys, int *is_in_list)
{
  
  int i, j;

  for(i=0; i<file_hdr.nkeys; i++) {
    is_in_list[i] = -1;
    for(j=0; j<nkeys; j++) {
      if(!strcmp(hdr_limits[i].bhp_hdr_name,keys[j])) {
        is_in_list[i] = j;
        break;
      }
    }
  }

}

void check_foldx(segy *trout, segy *tr, int *fold, FILE *ffp, char *request,
                int *index, int *list, int nkeys, int last, int my_endian,
                int endian, int verbose)
{

  int i;

  short nsamp;

  Value hval1, hval2;

  (*fold)++;
  gethval(trout,index[list[nkeys-1]],&hval1);
  /* check key in tr if not last trace */
  if(last == 0)
    gethval(tr,index[list[nkeys-1]],&hval2);
  else
    hval2.i = INT_MIN;
  if(hval1.i != hval2.i) {
    if(ffp) {
      for(i=0; i<nkeys; i++) {
        gethval(trout,index[list[i]],&hval1);
        fprintf(ffp,"%d ",hval1.i);
      }
      fprintf(ffp,"%d\n",*fold);
    }
    if(!strcmp(request,"fold")) {
      nsamp = trout->ns = 1;
      trout->data[0] = (float)*fold;
      check_endian(my_endian,endian,trout,trout->ns,verbose);
      write_trace(trout,nsamp);
    }
    *fold = 0;
  }
}

int get_corner(float key, int which, int min, int inc, int max, int **corner, int verbose)
{

  int step;
  int stat;

  step = 0;
  stat = 1;

  for(;;) {
    if(min + (step * inc) <= key && min + ((step + 1) * inc) >= key) {
      if(which == 0) {
        corner[0][0] = min + (step * inc);
        corner[1][0] = corner[0][0];
        corner[2][0] = min + ((step + 1) * inc);
        corner[3][0] = corner[2][0];
        break;
      }
      else {
        corner[0][1] = min + (step * inc);
        corner[1][1] = min + ((step + 1) * inc);
        corner[2][1] = corner[1][1];
        corner[3][1] = corner[0][1];
        break;
      }
    }
    step++;
    if(min + (step * inc) > max) {
      stat = 0;
      break;
    }
  }

  return stat;

}
