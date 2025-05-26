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
#include <stdlib.h>
#include <math.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include "bhpio.h"
#include "bhpiocube.h"
#include "cwp.h"
#include <libgen.h>

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
" BHPWRITECUBE creates a BHPIO cube dataset from SU trace data.    ",
"                                                                  ",
" Usage: bhpwritecube < stdin filename= [optional parameters]      ",
"                                                                  ",
" Required Parameters:                                             ",
"  filename=fname       User-supplied filename                     ",
"                                                                  ",
" Optional Parameters:                                             ",
"  init=no              The default action of bhpwrite is to accumulate",
"                       data and trace index information into an   ",
"                       existing BHPIO dataset. This allows you to run",
"                       multiple, concurrent jobs that all contributing",
"                       to the same data volume, once the first chunk",
"                       has been written. To create a new dataset, ",
"                       use init=yes.                              ",
"  pathlist=            ASCII file containing list of directories in which",
"  'filename'.dat       to write the dataset. BHPIO datasets are composed",
"                       of multiple partitions, which can be distributed across ",
"                       multiple UNIX files and filesystems.       ",
"                       Names for BHPIO datasets are composed of   ",
"                       a path, which comes from the pathlist file,",
"                       followed by the filename, a partition sequence",
"                       number, and the extension .su              ",
"                       For example, if filename=stack, and stack.dat contains",
"                       /data/D_170_001                            ",
"                       /data/D_170_004                            ",
"                       /data/D_170_006  ,                         ",
"                       then complete filenames are ",
"                        /data/D_170_001/stack_0001.su ,        ",
"                        /data/D_170_004/stack_0002.su ,        ",
"                        /data/D_170_006/stack_0003.su ,        ",
"                        /data/D_170_001/stack_0004.su ,        ",
"                       The size of each file is determined by the ",
"                       size parameter                             ",
"  key1=...",
"  key2=...",               
"  key3=...",
"  key4=...",
"  key5=...",
"                      Key parameters enable random reading of BHPIO",
"                      datasets. Up to 5 SU header keys may be specified",
"                      when a file is created. Each header used as a key",
"                      must be integer format. When you read the file",
"                      using BHPREAD, it can be read in any order defined",
"                      by combining the 5 keys which were used to ",
"                      create it. The syntax of the keyn parameter is",
"                      name,minimum-value,increment,number-of-bins, where",
"                      name is the SU header name,                  ",
"                      minimum-value is the minimum header value to write, ",
"                      increment is the increment between successive header",
"                      values, and number-of-bins is the number of bins",
"                      to allocate. The total number of traces which can",
"                      be written for a given key is defined as:",
"                      ((max-value - min-value + incr)/incr) * traces-per-bin",
"  bin=1               Maximum traces per bin",
"  action=w            Action to take if a key value is less than the",
"                      specified minimum or greater than the allowable",
"                      maximum, or if bin overflow occurs. Specify w, i, or a ",
"                      to discard the offending trace and issue a warning, or",
"                      to discard the offending trace without a warning, or",
"                      to abort the job, respectively.",
"  rule=keep           Binning rule. Choose from: ",
"                      keep - keep only the first bin=n traces in a bin,",
"                      replace - keep only the last trace to occupy a bin.",
"                      stack - sum traces into a bin, and update the nhs",
"                      trace header. The stacked trace is NOT normalized.",
"                      min - keep trace with smallest value in binhdr.",
"                      max - keep trace with largest value in binhdr.",
"                      Rules replace, stack, min and max work only if bin=1",
"  binhdr=offset       Binning header, used to decide which trace to keep",
"                      if rule=min or max",
"  maxens=0            Maximum ensemble size, in traces, in the input data.",
"                      To switch output partitions whenever the the end of",
"                      an input ensemble is reached, set maxens to the",
"                      maximum input ensemble size.",
"                      For this option to be effective key1 must correspond",
"                      to the major sort order of the input data.",
"  prealloc=no         Use prealloc=zero to cause output datasets to be",
"                      filled with zeros before writing any data.",
"                      Use prealloc=hdrs to pre-fill the dataset with trace headers",
"                      containing the correct key values, and traces consisting of zeros",
"  stripe=yes          Create multiple partitions of 'size' Megabytes each.",
"                      Use no to write a single partition. NOTE: there is an",
"                      arbitrary limit of 1 Million traces in a partition.",
"  size=1999           Size of each file partition, in megabytes.",
"  properties=...      List of model properties. BHPWRITECUBE can be used to",
"                      create layer-based models using input data that were",
"                      created by bhproffread or bhpmodel. Properties can",
"                      be any attribute such as velocity, density, etc  ",
"                      that can be represented as layered data.         ",
"  horizons=...        List of horizons, if input data were ",
"                      created by bhphorizon. If init=yes, new horizons",
"                      are created. If init=no, existing horizons are updated,",
"                      allowing for partial-trace updates",
"  endian=native       Omit endian parameter to write native byte-order",
"                      Specify endian=1 to force BIG_ENDIAN      ",
"                      Specify endian=0 to force LITTLE_ENDIAN   ",
"  stdin_endian=2      Default endianness for data from stdin is NATIVE,",
"                      where NATIVE is the endianness of the platform",
"                      on which bhpwritecube is running.",
"                      Specify stdin_endian=0 to read LITTLE_ENDIAN from stdin",
"                      Specify stdin_endian=1 to read BIG_ENDIAN from stdin",
"  transpose=no        Specify yes to write transposed data. Transposed data",
"                      is used to create time-slice or map-view datasets.",
"                      When writing transposed data, the last key is taken as the vertical",
"                      key. When writing non-transposed data, trace header tracl is automatically",
"                      used to store vertical key values, and is added to the file",
"                      header.                                            ",
"  vkey=tracl          The vertical key specification provides a way to access",
"                      vertical subsets of the data. The tracl header is not actually",
"                      used to store any infomation. After the data are written, use",
"                      the BHPIO utility to see how tracl is used. ",
"  units=              0=Seconds, 1=Feet, 2=Meters. If unspecified, the first ",
"                      input trace is checked. If dt/1000 > 8, units=2 is used.",
"                      If dt/1000 <= 8, units=0 is used. The units value is saved",
"                      in the header file for bhpread to use if required.",
"  verbose=0           For debug print, use verbose=1                        ",
"                                                                  ",
" Trace Header Usage:                                         ",
"   The trace header values associated with key1,...key5 are used to index",
"   data for future access.                                   ",
"   dt, ns and delrt are used to determine vertical extent and interval of",
"   the input data.                                           ",
"   Each trace after the first is checked for the same dt and ns value.",
"   If a different value is found, bhpwritecube will print an appropriate ",
"   message and perform an orderly shutdown. Data up to the point of failure",
"   is preserved.                                          ",
"   If any of the keys is sx, sy, gx, or gy, the coordinate scalar, scalco is applied.",
"   If rule=stack is specified, nhs is updated                    ",
"                                                                  ",
" BHPWRITECUBE saves trace header keys in an n-dimensional cube format,",
" where, n is the number of specified keys. This allows bhpreadcube",
" to quickly access traces randomly by doing a simple lookup operation.",
" Contrast with bhpwriteseq, which has an \"open-ended\" method of ",
" defining trace headers to save. bhpwriteseq is useful when the   ",
" limits of the data being processed are not well-defined. However,",
" bhpwritecube is significantly more efficient and uses less system ",
" resources than bhpwriteseq.                                      ",
"                                                                  ",
" Error Conditions:                                             ",
"  If any of the folowing error conditions occurs, bhpwritecube will attempt",
"  to perform and orderly shutdown. Once the error is resolved, you should",
"  be able to restart at the shutdown point without loss of data.    ",
"    Sample interval(dt) or trace length(ns) changes in input data.",
"    Out of disk space and unable to open next partition.",
"    Disk write returns an error status.               ",
"    Input data out of specified range, and action=a is specified.",
"    User does Ctrl-C or kill -s SIGINT pid                     ",
"                                                                  ",
"  EXAMPLES:                                                       ",
"    Write 10 lines, each with 101 CDPs of pre-stack data:  ",
"      bhpwritecube < line2031-2040.su filename=lines2031-2040 init=yes \\ ",
"         key1=fldr,2031,1,10 key2=cdp,1500,1,101 key3=offset,181,121,80 bin=2",
"      Use bin=2 to keep from losing any offsets.",
"    Same as previous job, but swap partitions on line boundaries.",
"      bhpwritecube < line2031-2040.su filename=lines2031-2040 init=yes \\ ",
"        maxens=80800 key1=fldr,2031,1,10 key2=cdp,1500,1,101 \\ ",
"        key3=offset,181,121,80 bin=2",
"      80800 = 10 lines times 101 CDPs times 80 offsets",
"    Initialize dataset with one line of data, then add lines 2-10",
"      bhpwritecube < line2031.su filename=lines2031-2040 init=yes \\ ",
"        key1=fldr,2031,1,10 key2=cdp,1500,1,101 \\ ",
"        key3=offset,181,121,80 bin=2",
"      bhpwritecube < line2032-2036.su filename=lines2031-2040",
"      bhpwritecube < line2037-2040.su filename=lines2031-2040",
"      Notice that the second 2 jobs specifiy only the filename.",
"      All other dataset information has already been saved.",
"      The second two jobs can be run serially or concurrently,",
"      but they cannot run until the first job completes.",
NULL};

/* Globals */
cwp_String key_name[MAX_NKEYS];   /* Key names */
int *last_key_vals;     /* previous trace key values */
int trace_count;        /* Trace counter */
int cube_in_memory=0;   /* 1=in, 0=out */
char **path;            /* Paths from pathlist */
char *init;             /* init=yes to start over, else accumulate */
FILE *fp[NFILES];        /* file pointers for all created files */
char my_file[NAMELEN];   /* Complete path to current file */
int my_partition=0;      /* current partition, 1 thru n */
int file_status[NFILES]; /* 1=partition is open, 0=not open */
#define LOCK 1           /* Switch to lock lock_file */
#define UNLOCK 0         /* Switch to unlock lock_file */
int lockfd;              /* lock file descriptor */
struct timeval msec;
#define sleeptime 20000  /* sleep for 20 mills */
#define NPAGES 10        /* number of pages in cube for paging and locking */
#define MINPAGESIZE 1000 /* lower-limit on size of a cube page in ints */
int verbose;            /* debug printout */
int debug=0;            /* trace code */


/* Set default endian flag */
#ifdef CWP_BIG_ENDIAN
  int my_endian = 1;
#else
  int my_endian = 0;
#endif

/* use doubles for mean, rms calc */
double meanval;
double rmsval;

/* Prototypes */
void minmax(int cube_buffer, int index, segy *tr, segy *tr1, char *rule,
            int my_endian, char *path, char *name, struct flock *hdrlock, int *nlocks,
            int *nwaits, int *waittime, int verbose, int debug);
void convert_addr(int trace_addr, int *prevpart, long *prevpos);
size_t write_trc(int *trace_count, char *init, int my_endian, segy *tr,
                 int nlayers, int nvklist, int *iprop, int verbose);
int lock_cube(struct flock *cublock, int lock, int start, int end, int *nlocks, int *nwaits,
              int *waittime, int verbose, int debug);
int lock_file_hdr(int verbose, struct flock *hdrlock, int lock, int *nlocks, int *nwaits, int *waittime,
                  int debug);
int bhp_open_write(int npaths, char **path, int verbose, char *prealloc, char **files, char *init);
int bhp_next_path(char **path, int npaths, int verbose);
int write_hdr_limits(char *path, int verbose);
int write_file_hdr(char *path, int verbose);
char **preallocate(int n, int npaths, char **path, int verbose);
void check_file_status(int part, int *file_status, char *path, char *name, struct flock *hdrlock, 
                       int *nlocks, int *nwaits, int *waittime, int verbose, int debug);
void fill_cube(int *key_min, int *key_max, int *key_incr, int *key_index, int cube_in_memory,
               int my_endian, segy *savetr, int trace_count, int nlayers, int nvklist, int *iprop,
               int npaths, char **path, char **files, struct flock *hdrlock, int *nlocks,
               int *nwaits, int *waittime, int verbose, int debug);

int main(int argc, char **argv)
{

  char *stripe;           /* yes or no */
  char hpath[NAMELEN];    /* Complete path to file_hdr */
  char lpath[NAMELEN];    /* Complete path to hdr_limits */
  char cpath[NAMELEN];    /* Complete path to cube */
  char kpath[NAMELEN];    /* Complete path to lock_file */
  char file[NAMELEN];     /* Re-usable filename */
  char string1[8];        /* scratch */
  char string2[8];        /* scratch */
  char *pathlist;         /* pathlist file */
  char *action;           /* error action if hdr outside min/max */
  char *rule;             /* binning - keep,replace,stack,min,max */
  char *binhdr;           /* header used to decide min/max rule */
  char *bintype;          /* binhdr type */
  char *prealloc;         /* pre-allocate output space */
  char **files;           /* pre-allocated filenames */
  char *aname;            /* aliased filename, e.g. 'filename_as_events' */
  char *transpose;        /* yes=writing transposed data, default=no */
  char *vkey;             /* vertical key parameter */
  char errmsg[128];       /* error messages */
  char *prop;             /* horizon to be updated */

  float factor;           /* Scale factor for coordinates */

  int bin_index;          /* header index of binhdr */
  int i, j, stat;         /* counters, status */
  int bytes_per_trace;    /* Bytes per trace+hdr */
  int key_index[MAX_NKEYS];/* Header key indices */
  int striping;           /* 1=yes, 0=no */
  int npaths;             /* Number of user-specified paths */
  int key_min[MAX_NKEYS]; /* key minima */
  int key_max[MAX_NKEYS]; /* key maxima */
  int key_incr[MAX_NKEYS];/* key increments */
  int *key_vals;          /* Header key values */
  int flush;              /* Flag to discard out of range trace */
  int *cube_buffer;       /* Cube read/write buffer */
  int page_in_memory=-1;  /* cube page in memory */
  int num_to_write;       /* number-to-write from keyn list */
  int trace_addr;         /* Relative trace address to write */
  int maxens;             /* Max number of traces per ensemble */
  int prev_key;           /* key1 value of previous trace */
  int prevpart;           /* partition number where partial stack is written */
  int partial=0;          /* 1=partial trace updating enabled */
  int ihorz;              /* index into trace of horizon being updated */
  int stdin_endian;       /* endianness of stdin */
  int nlayers;            /* layers per property for model data */
  int nvklist=0;          /* nvklist needed for trace_stats */
  int *iprop=NULL;        /* sample index of each property in model */
  int nwaits=0;           /* number of times this process has to wait for a lock */
  int nlocks=0;           /* number of times this process acquired a lock */
  int waittime=0;         /* total time to acquire locks, in milliseconds */
  int pagesize;           /* size of each cube page, for locking */
  int lastpage;           /* size of last cube page */
  int *pagestart;         /* starting int offset of each page */
  int *pageend;           /* ending int offset of each page */
  int pagenum=0 ;         /* page number to lock */
  int pagelock=-1;        /* page number that is currently locked, -1=none locked */
  int offset_in_page;     /* offset into loaded page */
  int npages;             /* hidden parameter - used to override NPAGES */

  short dt;               /* dt from first trace */

  long curpos;            /* current position in output data */
  long prevpos;           /* position in output data where partial stack is written */
  long offset;            /* offset into cube */

  cwp_String key_type[MAX_NKEYS];   /* Key types */ 
  cwp_String key_string[MAX_NKEYS]; /* Temporary key holder */

  Value hval;      /* Header values */

  struct flock cublock; /* Cube file lock structure */
  struct flock hdrlock; /* file_hdr lock structure */

  segy stack;      /* stacked trace for rule=stack */
  segy savetr;     /* saved trace for fill_cube */

  size_t nitems;   /* return from write_trc */

  /* initialize lock */
  cublock.l_whence = (short)SEEK_SET;
  hdrlock.l_whence = (short)SEEK_SET;

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  getparint("debug",&debug);
  if(debug == 1) {
    gettimeofday(&msec,NULL);
    fprintf(stderr,"%d started at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
    fprintf(stderr,"%d running on %u\n",(int)getpid(),(uint)gethostid());
  }

  /* init */
  if(!getparstring("init",&init))
    init = "no";
  if(strcmp(init,"no") && strcmp(init,"yes"))
    err("init is %s, must be yes or no\n",init);

  /* initialize filesys */
  filesys = NULL;

  /* Filename is required */
  if(!getparstring("filename",&file_hdr.filename))
    err("Filename is required\n");
  /* save filename  */
  i = strlen(file_hdr.filename);
  aname = calloc(i+1,sizeof(char));
  strcpy(aname,file_hdr.filename);

  /* check for alias */
  if(!strcmp(&file_hdr.filename[i-10],"_as_events")) {
    /* trim "_as_events" */
    file_hdr.filename[i-10] = '\0';
    if(verbose) {
      fprintf(stderr,"Aliased filename %s\n",aname);
      fprintf(stderr,"Trimmed filename %s\n",file_hdr.filename);
    }
  }

  /* pathlist */
  npaths = 0;
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc(strlen(file_hdr.filename)+5,sizeof(char));
    strcpy(pathlist,file_hdr.filename);
    strcat(pathlist,".dat");
  }
  if(verbose)
    fprintf(stderr,"Opening %s\n", pathlist);
  /* Load paths */
  if(!(fp[0] = fopen(pathlist,"r")))
    err("Cannot open pathlist: %s\n", pathlist);
  else {
    while(fgets(file,NAMELEN-1,fp[0]) != NULL)
      npaths++;
    rewind(fp[0]);
    path = calloc(npaths,sizeof(char *));
    for(i=0; i<npaths; i++)
      path[i] = calloc(NAMELEN,sizeof(char));
    for(i=0; i<npaths; i++) {
      fscanf(fp[0],"%s\n", file);
      if(!strcmp(file,"."))
        path[i] = getenv("PWD");
      else if(!strcmp(file,".."))
        err(".. in pathlist is illegal; use explicit path\n");
      else
        strcpy(path[i],file);
    }
    fclose(fp[0]);
  }
  if(!npaths)
    err("File %s is empty\n", pathlist);
  if(verbose)
    fprintf(stderr,"%d path(s) in pathlist\n", npaths);

  /* If init=no, load file header */
  if(!strcmp(init,"no")) {
    if(!get_bhpio_path(path[0],file_hdr.filename,"_LOCK.HDR",kpath,verbose))
      err("Cannot access file %s\n", kpath);
    if((lockfd = open(kpath,O_RDWR)) == -1)
      err("Cannot open lock_file\n");
    if(debug) {
      gettimeofday(&msec,NULL);
      fprintf(stderr,"%d opened_lock_file at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
    }
    if(lock_file_hdr(verbose,&hdrlock,LOCK,&nlocks,&nwaits,&waittime,debug))
      err("Could not lock lock_file1\n");
    if(!get_bhpio_path(path[0],aname,"_0000.HDR",hpath,verbose))
      err("Cannot access file %s\n", hpath);
    if((read_file_hdr(hpath,verbose)))
      err("Cannot open %s\n", hpath);
    if(lock_file_hdr(verbose,&hdrlock,UNLOCK,&nlocks,&nwaits,&waittime,debug))
      err("Could not unlock lock_file1\n");
    /* initialize file_status */
    for(i=0; i<NFILES; i++)
      file_status[i] = 0;
    /* Set striping */
    if(file_hdr.size)
      striping = 1;
    else
      striping = 0;
    /* no prealloc */
    prealloc = "no";
  }
  /* init=yes, initialize file-hdr */
  else {
    /* Allocate space for filenames */
    filesys = calloc(NFILES*NAMELEN,sizeof(char));
    /* Striping ? */
    if(!getparstring("stripe",&stripe))
      striping = 1;
    else {
      if(!strcmp(stripe,"yes"))
        striping = 1;
      else
        striping = 0;
    }
    /* If striping, get size parameter */
    if(striping) {
      /* Set size to default if unspecified  and striping=yes */
      if(!getparint("size",&file_hdr.size))
        file_hdr.size = BHP_SIZE;
      else
        getparint("size",&file_hdr.size);
      if(file_hdr.size <= 0)
        err("size = %d is illegal\n", file_hdr.size);
    }
    else
      file_hdr.size =0;
    /* pre-allocate */
    if(!getparstring("prealloc",&prealloc))
      prealloc = "no";
    if(strcmp(prealloc,"no") && strcmp(prealloc,"zero") && strcmp(prealloc,"hdrs"))
      err("prealloc=%s, should be no, zero, or hdrs\n", prealloc);
    if(!strcmp(prealloc,"zero"))
      if(verbose)
        fprintf(stderr,"Output space will be pre-allocated by writing all zeros\n");
    if(!strcmp(prealloc,"hdrs"))
      if(verbose)
        fprintf(stderr,"Empty slots in dataset will be filled with zeroed traces\n");

    /* data_order */
    if(!getparstring("transpose",&transpose))
      transpose = "no";
    if(!strcmp(transpose,"yes"))
      file_hdr.data_order = 2; 
    else
      file_hdr.data_order = 1;

    if(!getparstring("vkey",&vkey))
      vkey = "tracl";

    /* Check keys */
    file_hdr.nkeys = 0;
    for(i=0; i<MAX_NKEYS; i++) {
      strcpy(string1,"key");
      sprintf(string2,"%1i", i+1);
      strcat(string1,string2);
      if(getparstringarray(string1,key_string) && (j = countparval(string1)) != 4)
        err("%s should be name,min,incr,num\n",string1);
      else if(getparstringarray(string1,key_string)) {
        if(file_hdr.nkeys != i)
          err("%s cannot be used unless all lower numbered keys are also used\n", string1);
        key_name[i] = key_string[0];
        if(strlen(key_name[i]) > 7)
          err("Keynames are limited to 7 characters\n");
        key_min[i] = atoi(key_string[1]);
        key_incr[i] = atoi(key_string[2]);
        num_to_write = atoi(key_string[3]);
        key_max[i] = key_min[i] + key_incr[i] * num_to_write - key_incr[i];
        file_hdr.nkeys++;
      }
    }
    /* if data_order is transposed, last key is vkey, not a "real" key */
    if(file_hdr.data_order == 2) {
      file_hdr.nkeys--;
      strcpy(file_hdr.vkey,key_name[file_hdr.nkeys]);
      file_hdr.vmin = key_min[file_hdr.nkeys];
      file_hdr.vmax = key_max[file_hdr.nkeys];
      file_hdr.vinc = key_incr[file_hdr.nkeys];
    }
      
    if(verbose)
      for(i=0; i<file_hdr.nkeys; i++)
        fprintf(stderr,"KEY=%s, MIN=%d, MAX=%d, INC=%d\n",
                key_name[i],key_min[i],key_max[i],key_incr[i]);

    /* Save npaths in file_hdr */
    file_hdr.npaths = npaths;
    /* Need at least 1 key */
    if(!file_hdr.nkeys)
      err("At least one key required\n");
    /* bin size */
    if(!getparint("bin",&file_hdr.bin))
      file_hdr.bin = 1;
    if(file_hdr.bin <= 0)
      err("bin=%d is illegal, bin must be >= 1\n", file_hdr.bin);
    if(verbose)
      fprintf(stderr,"Traces per bin = %d\n", file_hdr.bin);
    /* Calculate cube size */
    file_hdr.cube_size = ((key_max[0] - key_min[0] + key_incr[0]) / key_incr[0]);
    for(i=1; i<file_hdr.nkeys; i++)
      file_hdr.cube_size *= ((key_max[i] - key_min[i] + key_incr[i]) / key_incr[i]);
    file_hdr.cube_size *= file_hdr.bin;
  } /* end init is yes or no */

  /* error action */
  if(!getparstring("action",&action))
    action = "w";
  if(strcmp(action,"w") && strcmp(action,"i") && strcmp(action,"a"))
    err("action=%s is illegal, action must be a, or, w, or i\n", action);
  if(verbose)
    fprintf(stderr,"Action=%s\n", action);

  /* binning rule */
  if(!getparstring("rule",&rule))
    rule = "keep";
  if(verbose)
    fprintf(stderr,"rule=%s\n", rule);
  if(strcmp(rule,"keep") && strcmp(rule,"replace") && strcmp(rule,"stack") &&
     strcmp(rule,"min") && strcmp(rule,"max"))
    err("%s is illegal value for rule\n", rule);
  if(file_hdr.bin > 1 && strcmp(rule,"keep"))
    err("rule=%s works only for bin=1\n", rule);
 
  /* binhdr */
  if(!strcmp(rule,"min") || !strcmp(rule,"max")) {
    if(!getparstring("binhdr",&binhdr))
      binhdr = "offset";
    if(verbose)
      fprintf(stderr,"Using %s for min/max binning\n", binhdr);
    bin_index = getindex(binhdr);
    bintype = hdtype(binhdr);
  }

  /* Alloc read/write buffer big enough for bin size */
  cube_buffer = calloc(file_hdr.bin,sizeof(int));
  /* If cube will fit in memory and init=yes, allocate, otherwise read/write cube file */
  if(file_hdr.cube_size <= MAX_CUBE_SIZE && !strcmp(init,"yes")) {
    if(verbose)
      fprintf(stderr,"Allocating %d ints for cube\n", file_hdr.cube_size);
    cube = calloc(file_hdr.cube_size,sizeof(int));
    cube_in_memory = 1;
  }
  /* Cube too big to keep in memory, or will fit and init=no */
  else {
    get_bhpio_path(path[0],file_hdr.filename,"_0000.0002.HDR",cpath,verbose);
    /* init=yes */
    if(!strcmp(init,"yes"))
      cubefd = open(cpath,O_RDWR | O_TRUNC | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    else
      cubefd = open(cpath,O_RDWR);
    if(cubefd == -1)
      err("Failed to open cube %s\n", cpath);
    if(debug) {
      gettimeofday(&msec,NULL);
      fprintf(stderr,"%d opened_cube at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
    }
    /* Initialize if init=yes */
    if(!strcmp(init,"yes")) {
      /* write 32K chunk at a time, then remainder */
      for(i=0; i<SU_NFLTS; i++)
        tr.data[i] = 0;
      /* number of 32K chunks */
      j = file_hdr.cube_size / SU_NFLTS;
      /* remainder */
      stat = file_hdr.cube_size - (j * SU_NFLTS);
      for(i=0; i<j; i++) {
        write(cubefd,&tr.data,sizeof(float)*SU_NFLTS);
        if(verbose > 0)
          fprintf(stderr,"Initialized %d cube slots\n",SU_NFLTS);
      }
      write(cubefd,&tr.data,sizeof(float)*stat);
      if(verbose > 0)
        fprintf(stderr,"Initialized %d cube slots\n",stat);
      lseek(cubefd,0L,SEEK_SET);
      if(verbose)
        fprintf(stderr,"Initialized %d ints in cube file\n", file_hdr.cube_size);
    }
  }

  /* cube not in memory, set page boundaries */
  if(cube_in_memory == 0) {
    if(!getparint("npages",&npages))
      npages = NPAGES;
    pagesize = file_hdr.cube_size / npages;
    if(pagesize <= MINPAGESIZE) {
      pagesize = file_hdr.cube_size;
      npages = 1;
    }
    pagestart = calloc(npages,sizeof(int));
    pageend = calloc(npages,sizeof(int));
    for(i=0; i<npages; i++) {
      pagestart[i] = i * pagesize;
      if(i < npages - 1)
        pageend[i] = pagestart[i] + pagesize - 1;
      else {
        lastpage = file_hdr.cube_size - pagestart[i];
        pageend[i] = pagestart[i] + lastpage - 1;
      }
    }
    /* alloc cube big enough for max page size */
    if(pagesize >= lastpage) {
      cube = calloc(pagesize,sizeof(int));
      if(verbose)
        fprintf(stderr,"Allocated %d ints for cube page\n",pagesize);
    }
    else {
      cube = calloc(lastpage,sizeof(int));
      if(verbose)
        fprintf(stderr,"Allocated %d ints for cube page\n",lastpage);
    }
    if(verbose) {
      fprintf(stderr,"npages=%d,pagesize=%d\n",npages,pagesize);
        for(i=0; i<npages; i++)
          fprintf(stderr,"start=%d  end=%d\n",pagestart[i],pageend[i]);
    }
  }
      
  /* Set rw_flag for writing */
  rw_flag = 1;

  /* Alloc hdr-limits */
  if(file_hdr.nkeys) {
    hdr_limits = calloc(file_hdr.nkeys,sizeof(bhp_hdr_limits));
    key_vals = calloc(file_hdr.nkeys,sizeof(int));
    last_key_vals = calloc(file_hdr.nkeys,sizeof(int));
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

  if(!strcmp(init,"yes")) {
    /* endian */
    if(!getparint("endian",&file_hdr.endian)) {
      file_hdr.endian = my_endian;
      if(verbose)
        fprintf(stderr,"Writing native format\n");
    }
    else if(file_hdr.endian == 0 || file_hdr.endian == 1) {
      if(verbose) {
        if(file_hdr.endian)
          fprintf(stderr,"Force BIG_ENDIAN format\n");
        else
          fprintf(stderr,"Force LITTLE_ENDIAN format\n");
      }
    }
    else {
      fprintf(stderr,"endian = %d\n", file_hdr.endian);
      err("If endian is specified, it must be 0 or 1\n");
    }
    
    /* can't have properties AND horizons */
    if(countparval("properties") > 0 && countparval("horizons") > 0)
      err("Can't have both properties and horizons\n");

    /* load properties or horizons, set data_type */
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

    /* Fill in header limits , key_index, key_type */
    for(i=0; i<file_hdr.nkeys; i++) {
      strcpy(hdr_limits[i].bhp_hdr_name,key_name[i]);
      hdr_limits[i].bhp_hdr_min = key_min[i];
      hdr_limits[i].bhp_hdr_max = key_max[i];
      hdr_limits[i].bhp_hdr_inc = key_incr[i];
      hdr_limits[i].bhp_hdr_scalar = 1;
      key_index[i] = getindex(hdr_limits[i].bhp_hdr_name);
      key_type[i] = hdtype(hdr_limits[i].bhp_hdr_name);
    }
  }
  /* init=no - get hdr-limits */
  else {
    if(!get_bhpio_path(path[0],file_hdr.filename,"_0000.0001.HDR",lpath,verbose))
      err("Could not access file: %s\n", lpath);
    /* Load hdr_limits */
    if(read_hdr_limits(lpath,verbose))
      err("Error reading %s\n", lpath);
    for(i=0; i<file_hdr.nkeys; i++) {
      key_name[i] = hdr_limits[i].bhp_hdr_name;
      key_index[i] = getindex(hdr_limits[i].bhp_hdr_name);
      key_type[i] = hdtype(hdr_limits[i].bhp_hdr_name);
      key_min[i] = hdr_limits[i].bhp_hdr_min;
      key_max[i] = hdr_limits[i].bhp_hdr_max;
      key_incr[i] = hdr_limits[i].bhp_hdr_inc;
      factor = hdr_limits[i].bhp_hdr_scalar;
      if(factor < 0)
        factor = -1. / factor;
      hdr_limits[i].bhp_hdr_min *= factor;
      hdr_limits[i].bhp_hdr_max *= factor;
      if(verbose) {
        fprintf(stderr,"Key: %s, Min: %d, Max: %d, Incr: %d\n",
                hdr_limits[i].bhp_hdr_name,hdr_limits[i].bhp_hdr_min,hdr_limits[i].bhp_hdr_max,
                hdr_limits[i].bhp_hdr_inc);
      }
    }
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
    if(strcmp(init,"yes"))
      fprintf(stderr,"Data will be merged with existing data\n");
    else
      fprintf(stderr,"Writing new dataset\n");
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
    
  /* If init=yes, see if file exists */
  if(!strcmp(init,"yes")) {
    if(get_bhpio_path(path[0],file_hdr.filename,"_0000.HDR",hpath,verbose)) {
      fprintf(stderr,"FILE: path=%s, filename=%s exists\n",path[0],file_hdr.filename);
      fprintf(stderr,"run 'bhpio pathlist=%s filename=%s delete=yes' to delete it\n",
              pathlist,file_hdr.filename);
      return(EXIT_FAILURE);
    }
  }
  
  /* if init=no, and horizons are specified, verify they exist, and get indices */
  if(!strcmp(init,"no")) {
    i = countparval("horizons");
    if(i > 1)
      err("Cannot update more than 1 horizon at a time\n");
    if(getparstring("horizons",&prop)) {
      /* verify requested horizon exists, and get index */ 
      stat = 0;
      for(j=0; j<-file_hdr.nprop; j++) {
        if(!strcmp(prop,properties[j])) {
          stat = 1;
          ihorz = j;
          break;
        }
      }
        if(!stat)
          err("Horizon %s does not exist\n",prop);
      if(verbose)
        fprintf(stderr,"%s is being updated, sample index is %d\n",prop,ihorz);
      partial = 1;
      /* set rule to partial */
      rule = "partial";
      if(verbose)
        fprintf(stderr,"Setting rule=partial\n");
    }
  }

  /* setup signal handler */
  signal(SIGINT,(void (*)(int))catch);
  signal(SIGHUP,(void (*)(int))catch);
  signal(SIGQUIT,(void (*)(int))catch);
  signal(SIGTERM,(void (*)(int))catch);
  
  /* Get first trace */
  if(!bhp_gettr(&tr,my_endian,stdin_endian))
    err("can't get first trace");
  dt = tr.dt;

  /* if prealloc=hdrs, save tr for later use to avoid possible double-swapping */
  if(!strcmp(prealloc,"hdrs"))
    memcpy((void *)&savetr,(const void *)&tr,HDRBYTES+4*file_hdr.nsamp);

  if(!strcmp(init,"yes")) {
    /* nlayers, nsamp for stats */
    file_hdr.nsamp = tr.ns;
    if(file_hdr.nprop <= 0) {
      nlayers = 0;
      stats.nsamp = file_hdr.nsamp;
    }
    else {
      /* cross-section model */
      if(file_hdr.data_order == 1) {
        nlayers = (file_hdr.nsamp - 1) / (file_hdr.nprop + 1);
        stats.nsamp = file_hdr.nsamp - (nlayers + 1);
      }
      /* map-view model */
      else if(file_hdr.data_order == 2) {
        /* nlayers is based on primay key max value for transposed data */
        nlayers = hdr_limits[0].bhp_hdr_max / file_hdr.nprop;
        stats.nsamp = file_hdr.nsamp;
      }
    }
    /* initialize stats */
    file_hdr.minval = FLT_MAX;
    file_hdr.maxval = -FLT_MAX;
    file_hdr.meanval = 0;
    file_hdr.rmsval = 0;
    /* single nvals if not prop or horz */
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
    /* if not transposed, save vkey info */
    /* seismic */
    if(file_hdr.data_order == 1 && file_hdr.nprop == 0) {
      strcpy(file_hdr.vkey,vkey);
      file_hdr.vmin = (float)tr.delrt;
      file_hdr.vinc = (float)tr.dt * 0.001;
      file_hdr.vmax = (float)tr.delrt + file_hdr.vinc * (tr.ns - 1);
    }
    /* model */
    else if(file_hdr.data_order == 1 && file_hdr.nprop > 0) {
      strcpy(file_hdr.vkey,vkey);
      file_hdr.vmin = 1;
      file_hdr.vinc = 1;
      file_hdr.vmax = nlayers;
    }
    /* events */
    else if(file_hdr.data_order == 1 && file_hdr.nprop < 0) {
      strcpy(file_hdr.vkey,vkey);
      file_hdr.vmin = 1;
      file_hdr.vinc = 1;
      file_hdr.vmax = tr.ns;
    }

    /* set units if unspecified */
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
  }
  if(verbose)
    fprintf(stderr,"VERTICAL Key=%s, MIN=%8.3f, MAX=%8.3f, INC=%8.3f\n",file_hdr.vkey,
            file_hdr.vmin,file_hdr.vmax,file_hdr.vinc);

  trace_count = 0;

  /* If any key is scalable save scalco */
  for(i=0; i<file_hdr.nkeys; i++)
    if(!strcmp(hdr_limits[i].bhp_hdr_name,"sx") ||
       !strcmp(hdr_limits[i].bhp_hdr_name,"sy") ||
       !strcmp(hdr_limits[i].bhp_hdr_name,"gx") ||
       !strcmp(hdr_limits[i].bhp_hdr_name,"gy"))
      hdr_limits[i].bhp_hdr_scalar = tr.scalco;

  /* If init=yes, set lpath and write hdr_limits */
  if(!strcmp(init,"yes")) {
    strcpy(lpath,path[0]);
    strcat(lpath,"/");
    strcat(lpath,file_hdr.filename);
    strcat(lpath,"_0000");
    strcat(lpath,".0001.HDR");
    if(write_hdr_limits(lpath,verbose))
      err("Failed to write header limits\n");
    if(verbose)
      fprintf(stderr,"Saved hdr_limits to %s\n", lpath);
  }

  /* Max ensemble size */
  if(!getparint("maxens",&maxens))
    maxens = 0;
  else {
    gethval(&tr,key_index[0],&hval);
    prev_key = hval.i;
    if(verbose)
      fprintf(stderr,"Maximum Ensemble Size, in Traces: %d\n", maxens);
  }
  
  /* Calculate traces per partition */
  bytes_per_trace = file_hdr.nsamp * sizeof(float) + HDRBYTES;
  file_hdr.traces_per_part = (file_hdr.size * 1024 * 1024) / bytes_per_trace;
  if(file_hdr.traces_per_part < maxens)
    file_hdr.traces_per_part = maxens;
  if(file_hdr.traces_per_part > MAX_TRACES) {
    file_hdr.traces_per_part = MAX_TRACES;
    if(verbose)
      fprintf(stderr,"Traces per partition limited to %d\n",MAX_TRACES);
  }
  if(verbose) {
    if(striping)
      fprintf(stderr,"bytes_per_trace: %d, traces_per_part: %d\n", bytes_per_trace,
              file_hdr.traces_per_part);
    else
      fprintf(stderr,"bytes_per_trace: %d, traces_per_part: All traces in data\n",
              bytes_per_trace);
  }

  /* If pre-allocating */
  if(strcmp(prealloc,"no") && !strcmp(init,"yes"))
    files = preallocate(file_hdr.cube_size/file_hdr.bin,npaths,path,verbose);
  else
    files = NULL;

  /* if init=yes, initialize counters */
  if(!strcmp(init,"yes"))
    file_hdr.nparts = 1;
  /* init=no, lock and read file_hdr */
  else {
    if(lock_file_hdr(verbose,&hdrlock,LOCK,&nlocks,&nwaits,&waittime,debug))
      err("Could not lock lock_file2\n");
    read_file_hdr(hpath,verbose);
    file_hdr.nparts++;
  }
  /* Open file */
  stat = bhp_open_write(npaths,path,verbose,prealloc,files,init);
  /* Quit if open failed */
  if(stat < 0) {
    fprintf(stderr," Could not open %s\n",my_file);
    return EXIT_FAILURE;
  }
  my_partition = get_part(my_file,verbose);
  /* If init=no, write updated file_hdr */
  if(!strcmp(init,"no")) {
    if(write_file_hdr(hpath,verbose))
      err("Failed to write file header\n");
    if(lock_file_hdr(verbose,&hdrlock,UNLOCK,&nlocks,&nwaits,&waittime,debug))
      err("Could not unlock lock_file2\n");
  }

  /* Loop over traces */
  for(;;) {
    /* verify consistency of ns, dt */
    if(tr.dt != dt) {
      sprintf(errmsg,"dt from first trace = %d, dt from current trace is %d\n",dt,tr.dt);
      shutdown(errmsg,key_name,last_key_vals,my_file,trace_count,fp,file_status,
               cube_in_memory,path[0],cube,init,lockfd,&msec,verbose,debug);
      break;
    }
    if((int)tr.ns != file_hdr.nsamp && partial != 1) {
      sprintf(errmsg,"ns from file header = %d, ns from current trace is %d\n",file_hdr.nsamp,tr.ns);
      shutdown(errmsg,key_name,last_key_vals,my_file,trace_count,fp,file_status,
               cube_in_memory,path[0],cube,init,lockfd,&msec,verbose,debug);
      break;
    }
    /* Check end of partition */
    gethval(&tr,key_index[0],&hval);
    if((maxens == 0 && trace_count == file_hdr.traces_per_part) || 
       (maxens != 0 && hval.i != prev_key &&
        trace_count + maxens > file_hdr.traces_per_part)) {
      fprintf(stderr,"End of Current Partition: Wrote %d traces to file %s\n",
              trace_count,my_file);
      efclose(fp[my_partition-1]);
      file_status[my_partition-1] = 0;
      /* if init=no, read file header */
      if(!strcmp(init,"no")) {
        if(lock_file_hdr(verbose,&hdrlock,LOCK,&nlocks,&nwaits,&waittime,debug))
          err("Could not lock lock_file3\n");
        read_file_hdr(hpath,verbose);
      }
      file_hdr.nparts++;
      /* Open next partition */
      stat = bhp_open_write(npaths,path,verbose,prealloc,files,init);
      /* prepare for shutdown if open failed */
      if(stat < 0) {
        /* open failed, probably out of space, decr nparts, shutdown */
        file_hdr.nparts--;
        sprintf(errmsg,"Could not open new partition\n");
        shutdown(errmsg,key_name,last_key_vals,my_file,trace_count,fp,file_status,
                 cube_in_memory,path[0],cube,init,lockfd,&msec,verbose,debug);
      }
      my_partition = get_part(my_file,verbose);
      trace_count = 0;
      /* If init=no write and unlock file_hdr */
      if(!strcmp(init,"no")) {
        if(write_file_hdr(hpath,verbose))
          err("Failed to write file header\n");
        if(lock_file_hdr(verbose,&hdrlock,UNLOCK,&nlocks,&nwaits,&waittime,debug))
          err("Could not unlock lock_file3\n");
      }
      /* if open failed, quit */
      if(stat < 0)
        err("Shutdown due to above error\n");
    }
    /* Verify trace is in cube range */
    flush = 0;
    /* Header keys for current trace */
    for(i=0; i<file_hdr.nkeys; i++) {
      gethval(&tr,key_index[i],&hval);
      key_vals[i] = vtoi(key_type[i],hval);
      /* Check range */
      if(key_vals[i] < key_min[i] || key_vals[i] > key_max[i]) {
        flush = 1;
        if(!strcmp(action,"w"))
          fprintf(stderr,"Discarding trace with %s = %d\n", key_name[i],key_vals[i]);
        if(!strcmp(action,"a")) {
          sprintf(errmsg,"Aborting, trace with %s = %d is out of range\n", key_name[i],key_vals[i]);
          shutdown(errmsg,key_name,last_key_vals,my_file,trace_count,fp,file_status,
                   cube_in_memory,path[0],cube,init,lockfd,&msec,verbose,debug);
        }
      } 
    }
    if(flush == 1 && !strcmp(action,"a"))
      break;
    if(!flush) {
      offset = cube_offset(key_vals,key_min,key_max,key_incr,file_hdr.bin);
      assert(offset >= 0 && offset < file_hdr.cube_size);
      trace_addr = MAX_TRACES * my_partition + trace_count;
      /* if cube not in memory and new page needed, write current page */
      if(cube_in_memory == 0) {
        pagenum = ((offset / pagesize) > npages - 1) ? npages - 1 : offset / pagesize;
        if(page_in_memory != pagenum && page_in_memory != -1) {
          if(page_in_memory == npages - 1)
            write_cube_page((long)(page_in_memory*pagesize*sizeof(int)),lastpage,my_endian,verbose);
          else
            write_cube_page((long)(page_in_memory*pagesize*sizeof(int)),pagesize,my_endian,verbose);
          page_in_memory = -1;
        }
      }
      /* if init=no, lock the page to be changed, if not already locked */
      if(!strcmp(init,"no")) {
        if(pagenum != pagelock) {
          /* unlock locked page first */
          if(pagelock != -1) {
            if((i = lock_cube(&cublock,UNLOCK,pagestart[pagelock],pageend[pagelock],
                              &nlocks,&nwaits,&waittime,verbose,debug)) == -1) {
              perror("lock_cube");
              err("unlock_cube failed at offset=%d, len=%d\n", offset,file_hdr.bin);
            }
          }
          if((i = lock_cube(&cublock,LOCK,pagestart[pagenum],pageend[pagenum],
                            &nlocks,&nwaits,&waittime,verbose,debug)) == -1) {
            perror("lock_cube");
            err("lock_cube failed at offset=%d, len=%d\n", offset,file_hdr.bin);
          }
          pagelock = pagenum;
        }
      }
      /* if cube not in memory, make sure cube page is loaded */
      if(cube_in_memory == 0 && (page_in_memory != pagenum)) {
        if(pagenum == npages - 1)
          read_cube_page((long)(pagenum*pagesize*sizeof(int)),lastpage,my_endian,verbose);
        else
          read_cube_page((long)(pagenum*pagesize*sizeof(int)),pagesize,my_endian,verbose);
        page_in_memory = pagenum;
        offset_in_page = (page_in_memory == 0) ? offset : offset % (page_in_memory * pagesize);
      }
      /* get the bin status */
      if(cube_in_memory == 1)
        read_cube(offset,cube_buffer,my_endian,1,verbose);
      else {
        offset_in_page = (page_in_memory == 0) ? offset : offset % (page_in_memory * pagesize);
        read_cube(offset_in_page,cube_buffer,my_endian,1,verbose);
      }
      /* rule=keep - if bin is full, flush trace */
      if(!strcmp(rule,"keep")) {
        /* look for slot */
        stat = 1;
        for(i=0; i<file_hdr.bin; i++) {
          if(!cube_buffer[i]) {
            stat = 0;
            cube_buffer[i] = trace_addr;
            break;
          }
        }
        /* write trace if stat is zero, else flush */
        if(!stat) {
          nitems = write_trc(&trace_count,init,my_endian,&tr,nlayers,nvklist,iprop,verbose);
          /* shutdown if write_trc returns error */
          if(nitems != HDRBYTES + file_hdr.nsamp * sizeof(float)) {
            sprintf(errmsg,"Aborting: write trace returned %d bytes written, should be %d\n",
                    nitems,HDRBYTES+file_hdr.nsamp*sizeof(float));
            shutdown(errmsg,key_name,last_key_vals,my_file,trace_count,fp,file_status,
                     cube_in_memory,path[0],cube,init,lockfd,&msec,verbose,debug);
            break;
          }
        }
        else {
          flush = 1;
          if(!strcmp(action,"w") || !strcmp(action,"a")) {
            fprintf(stderr,"Discarding trace with ");
            for(i=0; i<file_hdr.nkeys; i++)
              fprintf(stderr,"%s=%d ",key_name[i],key_vals[i]);
            fprintf(stderr,"because of bin overflow\n");
          }
          if(!strcmp(action,"a")) {
            sprintf(errmsg,"Aborting: bin overflow\n");
            shutdown(errmsg,key_name,last_key_vals,my_file,trace_count,fp,file_status,
                     cube_in_memory,path[0],cube,init,lockfd,&msec,verbose,debug);
            break;
          }
        }
      }
      /* min,max - keep trace with min or max binhdr value */
      else if(!strcmp(rule,"min") || !strcmp(rule,"max")) {
        assert(file_hdr.bin == 1);
        /* if bin is empty, just write the trace */
        if(!cube_buffer[0]) {
          cube_buffer[0] = trace_addr;
          nitems = write_trc(&trace_count,init,my_endian,&tr,nlayers,nvklist,iprop,verbose);
        }
        /* if occupied, compare current binhdrs and possibly write new trace */
        else
          minmax(cube_buffer[0],bin_index,&tr,&stack,rule,my_endian,path[0],aname,&hdrlock,
                 &nlocks,&nwaits,&waittime,verbose,debug);
      }
      /* replace,stack, or partial - if bin is occupied, overwrite the previously-written trace */
      else if(!strcmp(rule,"replace") || !strcmp(rule,"stack") || !strcmp(rule,"partial")) {
        assert(file_hdr.bin == 1);
        /* if slot is empty, just write the trace */
        if(!cube_buffer[0]) {
          /* can't update empty bin */
          assert(strcmp(rule,"partial"));
          cube_buffer[0] = trace_addr;
          nitems = write_trc(&trace_count,init,my_endian,&tr,nlayers,nvklist,iprop,verbose);
        }
        /* if occupied, overwrite the previous address */
        else {
          /* Save current location in output */
          curpos = ftell(fp[my_partition-1]);
          trace_addr = cube_buffer[0];
          /* Convert trace address to partition number + byte offset */
          convert_addr(trace_addr,&prevpart,&prevpos);
          /* if prevpart different from my_partition, make sure file is open */
          if(prevpart != my_partition) {
            if(verbose)
              fprintf(stderr,"Checking status for part %d, my part is %d\n",prevpart,my_partition);
            check_file_status(prevpart,file_status,path[0],aname,&hdrlock,&nlocks,&nwaits,&waittime,
                              verbose,debug);
          }
          /* Seek to where previous trace was written */
          efseek(fp[prevpart-1],prevpos,SEEK_SET);
          /* if rule=stack or rule=partial, read partial stack or original trace */
          if(!strcmp(rule,"stack") || !strcmp(rule,"partial")) {
            efread(&stack,FSIZE,file_hdr.nsamp+(HDRBYTES/4),fp[prevpart-1]);
            if(verbose && !strcmp(rule,"stack")) {
              fprintf(stderr,"Stacking trace with ");
              for(i=0; i<file_hdr.nkeys; i++)
                fprintf(stderr,"%s=%d ",key_name[i],key_vals[i]);
              fprintf(stderr,"\n");
            }
            /* Back up to beginning of trace to write updated stack or updated trace */
            efseek(fp[prevpart-1],prevpos,SEEK_SET);
            /* Swap bytes if necessary */
            check_endian(my_endian,file_hdr.endian,&stack,(short)file_hdr.nsamp,verbose);
            if(!strcmp(rule,"stack")) {
              for(i=0; i<file_hdr.nsamp; i++)
                tr.data[i] += stack.data[i];
              tr.nhs += stack.nhs;
            }
            else if(!strcmp(rule,"partial")) {
              stack.data[ihorz] = tr.data[0];
              tr.ns = stack.ns;
              for(i=0; i<file_hdr.nsamp; i++)
                tr.data[i] = stack.data[i];
            }
          }
          /* Swap bytes if necessary */
          check_endian(my_endian,file_hdr.endian,&tr,(short)file_hdr.nsamp,verbose);
          /* write to previous address */
          efwrite(&tr,HDRBYTES,1,fp[prevpart-1]);
          efwrite(tr.data,FSIZE,file_hdr.nsamp,fp[prevpart-1]);
          i = fflush(fp[prevpart-1]);
          if(i) {
            perror("bhpwritecube:fflush1");
            err("fflush returned %d\n", i);
          }
          /* move file pointer to next output address */
          efseek(fp[my_partition-1],curpos,SEEK_SET);
          if(verbose == 1 && !strcmp(rule,"replace")) {
            fprintf(stderr,"Replacing trace with ");
            for(i=0; i<file_hdr.nkeys; i++)
              fprintf(stderr,"%s=%d ",key_name[i],key_vals[i]);
            fprintf(stderr,"\n");
          }
        }
      }
      /* save keys in case error on next trace */
      for(i=0; i<file_hdr.nkeys; i++)
        last_key_vals[i] = key_vals[i];
    }
    /* write updated bin */
    if(!flush) {
      if(cube_in_memory == 1)
        write_cube(offset,cube_buffer,my_endian,cube_in_memory,verbose);
      else {
        offset_in_page = (page_in_memory == 0) ? offset : offset % (page_in_memory * pagesize);
        write_cube(offset_in_page,cube_buffer,my_endian,1,verbose);
      }
    }
    /* Save ensemble key for checking next trace */
    gethval(&tr,key_index[0],&hval);
    prev_key = hval.i;
    /* End of input? */
    if(!bhp_gettr(&tr,my_endian,stdin_endian)) {
      fprintf(stderr,"End of Input Data: %d wrote %d traces to file %s\n",(int)getpid(),trace_count,my_file);
      efclose(fp[my_partition-1]);
      file_status[my_partition-1] = 0;
      /* If no new traces were written, remove my_partition and decr nparts */
      if(trace_count == 0) {
        if(!strcmp(init,"no")) {
          if(lock_file_hdr(verbose,&hdrlock,LOCK,&nlocks,&nwaits,&waittime,debug))
            err("Could not lock lock_file4\n");
        }
        read_file_hdr(hpath,verbose);
        /* find file in partition list and remove it; 
           it may not be the last one in the list */
        j = -1;
        for(i=0; i<file_hdr.nparts; i++) {
          if(!strcmp(my_file,&filesys[i*NAMELEN])) {
            j = i;
            break;
          }
        }
        if(j >= 0) {
          eremove(my_file);
          file_hdr.nparts--;
          for(i=j; i<file_hdr.nparts; i++)
            strcpy(&filesys[i*NAMELEN],&filesys[(i+1)*NAMELEN]);
          if(verbose)
            fprintf(stderr,"Removed empty file, %s, and decremented nparts to %d\n",
                    my_file,file_hdr.nparts);
          if(write_file_hdr(hpath,verbose))
            err("Failed to write file header\n");
        }
        if(!strcmp(init,"no")) {
          if(lock_file_hdr(verbose,&hdrlock,UNLOCK,&nlocks,&nwaits,&waittime,debug))
            err("Could not unlock lock_file4\n");
        }
      }
      break;
    }
  }
  
  /* if cube not in memory, write current page */
  if(cube_in_memory == 0 && page_in_memory != -1) {
    if(page_in_memory == npages - 1)
      write_cube_page((long)(page_in_memory*pagesize*sizeof(int)),lastpage,my_endian,verbose);
    else
      write_cube_page((long)(page_in_memory*pagesize*sizeof(int)),pagesize,my_endian,verbose);
  }

  /* unlock locked page */
  if(!strcmp(init,"no") && pagelock != -1) {
    if((i = lock_cube(&cublock,UNLOCK,pagestart[pagelock],pageend[pagelock],
                      &nlocks,&nwaits,&waittime,verbose,debug)) == -1) {
      perror("lock_cube");
      err("unlock_cube failed at offset=%d, len=%d\n", offset,file_hdr.bin);
    }
  }

  /* Write cube if in memory */
  if(cube_in_memory)
    save_cube(path[0],cube,verbose);

  /* if init=yes, calc stats and write file header */
  if(!strcmp(init,"yes")) {
    calc_stats(verbose,debug);
    get_bhpio_path(path[0],file_hdr.filename,"_0000.HDR",hpath,verbose);
    if(write_file_hdr(hpath,verbose))
      err("Failed to write file header\n");
    /* Create lock file */
    get_bhpio_path(path[0],file_hdr.filename,"_LOCK.HDR",kpath,verbose);
    if(create_lock_file(kpath,lockfd,&msec,verbose,debug))
      err("Failed to create lock file\n");
  }

  /* if prealloc=hdrs, fill in missing slots */
  if(!strcmp(prealloc,"hdrs"))
    fill_cube(key_min,key_max,key_incr,key_index,cube_in_memory,my_endian,
              &savetr,trace_count,nlayers,nvklist,iprop,npaths,path,files,&hdrlock,
              &nlocks,&nwaits,&waittime,verbose,debug);

  /* Release buffers */
  free(hdr_limits);
  free(filesys);
  free(cube);
  for(i=0; i<npaths; i++)
    free(path[i]);
  free(path);

  if(debug) {
    gettimeofday(&msec,NULL);
    fprintf(stderr,"%d finished at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
  }
  if(verbose)
    fprintf(stderr,"%d acquired %d locks, and waited %u times, for a total of %u milliseconds\n",
            (int)getpid(),nlocks,nwaits,waittime/1000);

  return EXIT_SUCCESS;

}

int bhp_open_write(int npaths, char **path, int verbose, char *prealloc,
                   char **files, char *init)
{

  char cindex[8];

  int next_filesys;

  /* Get path with most space unless preallocated */
  if(!strcmp(prealloc,"no")) {
    next_filesys = bhp_next_path(path,npaths,verbose);
    if(next_filesys < 0)
      return -1;
    /* Build complete file name unless pre-allocated */
    strcpy(my_file,path[next_filesys]);
    strcat(my_file,"/");
    strcat(my_file,file_hdr.filename);
    strcat(my_file,"_");
    /* get last part from list */
    if(file_hdr.nparts > 1) {
      my_partition = get_part(&filesys[(file_hdr.nparts-2)*NAMELEN],verbose)+1;
      sprintf(cindex,"%04i",my_partition);
    }
    else {
      my_partition = 1;
      sprintf(cindex,"%04i",my_partition);
    }
    strcat(my_file,cindex);
    strcat(my_file,".su");
    /* Save file in list */
    strcpy(&filesys[(file_hdr.nparts-1)*NAMELEN],my_file);
  }
  else {
    strcpy(&filesys[(file_hdr.nparts-1)*NAMELEN],files[file_hdr.nparts-1]);
    strcpy(my_file,files[file_hdr.nparts-1]);
  }

  /* Truncate if (init=yes and not preallocated) or init=no */
  if((!strcmp(init,"yes") && !strcmp(prealloc,"no")) || !strcmp(init,"no")) {
    fp[my_partition-1] = fopen(my_file,"w+");
    rewind(fp[my_partition-1]);
    if(verbose)
      fprintf(stderr,"Creating %s\n", my_file);
    file_status[my_partition-1] = 1;
  }
  /* init=yes and pre-allocated */
  else if(!strcmp(init,"yes") && (!strcmp(prealloc,"zero") || !strcmp(prealloc,"hdrs"))) {
    fp[my_partition-1] = fopen(my_file,"r+");
    if(fp[my_partition-1] == NULL)
      err("Failed to open pre-allocated file=%s\n",my_file);
    if(verbose)
      fprintf(stderr,"Opening %s\n", my_file);
    file_status[my_partition-1] = 1;
  }

  if(verbose > 0) {
    fprintf(stderr,"Opened partition %d, using fp %d\n",my_partition,my_partition-1);
    fprintf(stderr,"Number of active parts is %d\n",file_hdr.nparts);
  }

  return 0;

}

int bhp_next_path(char **path, int npaths, int verbose)
{

  int i, stat;
  int index;
  float mb_avail;
  float most;

  struct statvfs buf;

  most = 0;

  for(i=0; i<npaths; i++) {
    stat = statvfs(path[i],&buf);
    if(verbose)
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
  if(most < file_hdr.size) {
    fprintf(stderr,"Not enough space to hold next partition\n");
    index = -1;
  }
  else if(verbose)
    fprintf(stderr,"Allocate next partition in %s, with %fMB available\n", path[index],most);

  return index;

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
  }
  fclose(lfp);

  return 0;

}

char **preallocate(int n, int npaths, char **path, int verbose)
{

  char string[8];
  char **files;

  int i, j, k;
  int ntraces, nbytes, nparts;
  int *trace;
  int size=10000000;
  int size1;
  int next_filesys;

  /* If no keys specified, return error */
  if(!file_hdr.nkeys)
    err("Can't pre-allocate unless keys are specified\n");

  /* Calculate partition size in bytes */
  ntraces = file_hdr.traces_per_part;
  nbytes = ntraces * (file_hdr.nsamp * sizeof(float) + HDRBYTES);
  if(ntraces != file_hdr.traces_per_part)
    file_hdr.traces_per_part = ntraces;

  /* Calculate number of partitions to preallocate */
  nparts = n / ntraces;
  if(n%ntraces)
    nparts++;

  /* Reserve space for pre-allocated filenames */
  files = calloc(nparts,sizeof(char *));
  for(i=0; i<nparts; i++)
    files[i] = calloc(NAMELEN,sizeof(char));

  /* allocate a 10MB trace */
  trace = calloc(size,sizeof(char));

  if(verbose) {
    fprintf(stderr,"Pre-allocating %d traces per partition\n", ntraces);
    fprintf(stderr,"Pre-allocating %d bytes per partition\n", nbytes);
    fprintf(stderr,"Pre-allocating %d partitions\n", nparts);
  }

  /* Loop over partitions */
  for(i=0; i<nparts; i++) {
    /* Get path with most space */
    next_filesys = bhp_next_path(path,npaths,verbose);
    strcpy(files[i],path[next_filesys]);
    strcat(files[i],"/");
    strcat(files[i],file_hdr.filename);
    strcat(files[i],"_");
    sprintf(string,"%04i",i+1);
    strcat(files[i],string);
    strcat(files[i],".su");
    fprintf(stderr,"Opening file %s\n", files[i]);
    fp[i] = fopen(files[i],"w");
    /* If last partition is not full, don't waste any space */
    if(verbose)
      fprintf(stderr,"i=%d,n=%d,nparts=%d,ntraces=%d\n",i,n,nparts,ntraces);
    if(i == nparts - 1 && n%ntraces) {
      ntraces = n - ntraces * (nparts - 1);
      nbytes = ntraces * (file_hdr.nsamp * sizeof(float) + HDRBYTES);
      if(verbose)
        fprintf(stderr,"Last partition has %d traces, %d bytes\n", ntraces,nbytes);
    }
    /* calculate number of 10MB chunks to write */
    k = nbytes / size + 1;
    if(verbose)
      fprintf(stderr,"k=%d,nbytes=%d\n",k,nbytes);
    /* write zeros */
    for(j=0; j<k; j++)
      if(j == k - 1) {
        size1 = nbytes - (k - 1) * size;
        efwrite(trace,sizeof(char),size1,fp[i]);
        if(verbose)
          fprintf(stderr,"Wrote %d bytes\n",size1);
      }
      else {
        efwrite(trace,sizeof(char),size,fp[i]);
        if(verbose)
          fprintf(stderr,"Wrote %d bytes\n",size);
      }
    efclose(fp[i]);
  }

  free(trace);

  return files;

}

int lock_file_hdr(int verbose, struct flock *hdrlock, int lock, int *nlocks, int *nwaits, int *waittime,
                  int debug)
{

  int i;
  unsigned int req, acq;

  if(lock) {
    hdrlock->l_type = F_WRLCK;
    hdrlock->l_start = 0L;
    hdrlock->l_len = 0L;
    gettimeofday(&msec,NULL);
    req = million * msec.tv_sec + msec.tv_usec;
    if(debug)
      fprintf(stderr,"%d request_lock_file at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
    /* loop until lock acquired */
    while(1) {
      i = fcntl(lockfd,F_SETLK,hdrlock);
      if(i != -1)
        break;
      else {
        (*nwaits)++;
        if(debug) {
          gettimeofday(&msec,NULL);
          fprintf(stderr,"%d sleeping at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
        }
        i = usleep(sleeptime);
      }
    }
    gettimeofday(&msec,NULL);
    (*nlocks)++;
    acq = million * msec.tv_sec + msec.tv_usec;
    (*waittime) += (acq - req);
    if(i != -1 && debug)
      fprintf(stderr,"%d acquire_lock_file at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
  }
  else {
    hdrlock->l_type = F_UNLCK;
    i = fcntl(lockfd,F_SETLK,hdrlock);
    gettimeofday(&msec,NULL);
    if(i != -1 && debug)
      fprintf(stderr,"%d unlock_lock_file at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
  }

  return i;

}

int lock_cube(struct flock *cublock, int lock, int start, int end, int *nlocks, int *nwaits,
              int *waittime, int verbose, int debug)
{

  int i;
  unsigned int req, acq;

  /* lock required page in cube */
  if(lock) {
    cublock->l_type = F_WRLCK;
    cublock->l_start = start * sizeof(int);
    cublock->l_len = (long)((end - start + 1) * sizeof(int));
    gettimeofday(&msec,NULL);
    req = million * msec.tv_sec + msec.tv_usec;
    if(debug)
      fprintf(stderr,"%d request_cube at %lu\n",(int)getpid(),million*msec.tv_sec+msec.tv_usec);
    /* loop until lock acquired */
    while(1) {
      i = fcntl(cubefd,F_SETLK,cublock);
      if(i != -1)
        break;
      else {
        (*nwaits)++;
        if(debug) {
          gettimeofday(&msec,NULL);
          fprintf(stderr,"%d sleeping at %lu\n",(int)(int)getpid(),million*msec.tv_sec+msec.tv_usec);
        }
        i = usleep(sleeptime);
      }
    }
    gettimeofday(&msec,NULL);
    (*nlocks)++;
    acq = million * msec.tv_sec + msec.tv_usec;
    (*waittime) += (acq - req);
    if(i != -1 && debug)
      fprintf(stderr,"%d acquire_cube st=%d, len=%d, at %u\n",(int)getpid(),(int)cublock->l_start,
              (int)cublock->l_len,(uint)(million*msec.tv_sec+msec.tv_usec));
  }
  /* unlock */
  else {
    cublock->l_type = F_UNLCK;
    i = fcntl(cubefd,F_SETLK,cublock);
    gettimeofday(&msec,NULL);
    if(i != -1 && debug)
      fprintf(stderr,"%d unlock_cube st=%d, len=%d, at %u\n",(int)getpid(),(int)cublock->l_start,
              (int)cublock->l_len,(uint)(million*msec.tv_sec+msec.tv_usec));
  }

  return i;

}

size_t write_trc(int *trace_count, char *init, int my_endian, segy *tr,
               int nlayers, int nvklist, int *iprop, int verbose)
{

  int i;
  static int first=1;  /* first time flag */
  static int index;    /* primary key index */
  int jprop;           /* property index if map-view model */

  Value hval;

  size_t nitems;       /* return from fwrite */

  /* accumulate statistics if init=yes */
  if(!strcmp(init,"yes")) {
    /* if data is transposed model or horizons, pass 1 for nprop, current index in iprop */
    if(file_hdr.data_order == 2 && file_hdr.data_type > 1) {
      if(first == 1) {
        first = 0;
        index = getindex(hdr_limits[0].bhp_hdr_name);
      }
      gethval(tr,index,&hval);
      /* for model, iprop index is property index grouped by nlayers */
      if(file_hdr.data_type == 3)
        jprop = ((hval.i - 1) / nlayers) * nlayers;
      /* for events, use event index */
      else if(file_hdr.data_type == 2)
        jprop = hval.i;
      trace_stats(tr,nlayers,nvklist,1,&jprop,0,verbose);
    }
    else
      trace_stats(tr,nlayers,nvklist,file_hdr.nprop,iprop,0,verbose);
  }

  /* Swap bytes if necessary */
  check_endian(my_endian,file_hdr.endian,tr,(short)file_hdr.nsamp,verbose);

  nitems = fwrite(tr,sizeof(char),HDRBYTES+sizeof(float)*file_hdr.nsamp,fp[my_partition-1]);
  /* if error, back up to where write was attempted and return error */
  if(nitems != HDRBYTES+sizeof(float)*file_hdr.nsamp) {
    fprintf(stderr,"fwrite returned %d bytes written\n",nitems);
    return nitems;
  }
  i = fflush(fp[my_partition-1]);
  if(i) {
    perror("bhpwritecube:fflush2");
    err("fflush returned %d\n", i);
  }

  (*trace_count)++;
  return nitems;

}
void minmax(int cube_buffer, int index, segy *tr, segy *tr1, char *rule,
            int my_endian, char *path, char *name, struct flock *hdrlock,
            int *nlocks, int *nwaits, int *waittime, int verbose, int debug)
{

  int i;
  int prevpart;
  int replace=0;   /* replace flag */

  long prevpos;
  long curpos;

  Value hval;
  Value hval1;

  /* read trace and compare hdrs */
  convert_addr(cube_buffer,&prevpart,&prevpos);
  /* save current file position */
  curpos = ftell(fp[my_partition-1]);
  /* if prevpart different from my_partition, make sure file is open */
  if(prevpart != my_partition)
    check_file_status(prevpart,file_status,path,name,hdrlock,nlocks,nwaits,waittime,verbose,debug);
  /* read previous trace */
  efseek(fp[prevpart-1],prevpos,SEEK_SET);
  efread(tr1,FSIZE,file_hdr.nsamp+(HDRBYTES/4),fp[prevpart-1]);
  /* Swap bytes if necessary */
  check_endian(my_endian,file_hdr.endian,tr1,(short)file_hdr.nsamp,verbose);
  /* get binhdrs */
  gethval(tr,index,&hval);
  gethval(tr1,index,&hval1);
  if(!strcmp(rule,"min")) {
    if(hval1.i <= hval.i) {
      if(verbose)
        fprintf(stderr,"Keeping %d, rejecting %d\n", hval1.i,hval.i);
    }
    else {
      replace = 1;
      if(verbose)
        fprintf(stderr,"Replacing %d with %d\n", hval1.i,hval.i);
    }
  }
  if(!strcmp(rule,"max")) {
    if(hval1.i >= hval.i) {
      if(verbose)
        fprintf(stderr,"Keeping %d, rejecting %d\n", hval1.i,hval.i);
    }
    else {
      replace = 1;
      if(verbose)
        fprintf(stderr,"Replacing %d with %d\n", hval1.i,hval.i);
    }
  }
  /* if replace set, new trace is winner */
  if(replace == 1) {
    /* Back up to beginning of trace to write new trace */
    efseek(fp[prevpart-1],prevpos,SEEK_SET);
    /* Swap bytes if necessary */
    check_endian(my_endian,file_hdr.endian,tr,(short)file_hdr.nsamp,verbose);
    /* write to previous address */
    efwrite(tr,sizeof(char),HDRBYTES+sizeof(float)*file_hdr.nsamp,fp[prevpart-1]);
    i = fflush(fp[prevpart-1]);
    if(i) {
      perror("bhpwritecube:fflush3");
      err("fflush returned %d\n", i);
    }
  }
  /* move file pointer back to next output address */
  efseek(fp[my_partition-1],curpos,SEEK_SET);

}

void convert_addr(int trace_addr, int *prevpart, long *prevpos)
{

  *prevpart = trace_addr / MAX_TRACES;
  *prevpos = trace_addr - (MAX_TRACES * (*prevpart));
  *prevpos *= (HDRBYTES + (file_hdr.nsamp * sizeof(float)));

}

void check_file_status(int part, int *file_status, char *path, char *name, struct flock *hdrlock,
                       int *nlocks, int *nwaits, int *waittime, int verbose, int debug)
{

  char cindex[8];         /* Use to append file index to path */
  char hpath[NAMELEN];    /* file_hdr filename */
  char file[NAMELEN];     /* filename */

  int i, j;

  /* open file if not open already */
  if(file_status[part-1] == 0) {
    /* get list of open parts */
    if(!get_bhpio_path(path,name,"_0000.HDR",hpath,verbose))
      err("Cannot access file %s\n", hpath);
    if(lock_file_hdr(verbose,hdrlock,LOCK,nlocks,nwaits,waittime,debug))
      err("Could not lock lock_file5\n");
    if((read_file_hdr(hpath,verbose)))
      err("Cannot open %s\n", hpath);
    if(lock_file_hdr(verbose,hdrlock,UNLOCK,nlocks,nwaits,waittime,debug))
      err("Could not unlock lock_file5\n");
    /* search filesys list for correct part */
    for(i=0; i<file_hdr.nparts; i++) {
      strcpy(file,&filesys[i*NAMELEN]);
      j = (int)strlen(file);
      strncpy(cindex,&file[j-7],4);
      cindex[4] = '\0';
      j = atoi(cindex);
      if(j == part) {
        j = -1;
        break;
      }
    }
    if(j != -1)
      err("Could not find part %d\n",part);
    /* if file is a dir, append filename, else filename is already appended */
    if(isadir(file) == 1) {
      if(verbose)
        fprintf(stderr,"%s is a directory, appending filename\n",file);
      strcpy(file,&filesys[(part-1)*NAMELEN]);
      strcat(file,"/");
      strcat(file,file_hdr.filename);
      strcat(file,"_");
      sprintf(cindex,"%04i\0",part);
      strcat(file,cindex);
      strcat(file,".su");
    }
    else {
      if(verbose)
        fprintf(stderr,"%s is a file, not appending filename\n",file);
    }
    fp[part-1] = fopen(file,"r+");
    if(!fp[part-1])
      err("Could not open %s\n", file);
    else if(verbose)
      fprintf(stderr,"%d opened %s\n",(int)getpid(),file);
    /* mark open */
    file_status[part-1] = 1;
  }

}

void fill_cube(int *key_min, int *key_max, int *key_incr, int *key_index, int cube_in_memory,
               int my_endian, segy *savetr, int trace_count, int nlayers, int nvklist, int *iprop,
               int npaths, char **path, char **files, struct flock *hdrlock,
               int *nlocks, int *nwaits, int *waittime, int verbose, int debug)
{

  int i, ntraces, j;
  int *keys;           /* key values to use */
  int cube_buffer[1];  /* check single trace per bin */
  int offset;
  int trace_addr;

  Value hval;

  segy trace;          /* scratch trace for swapping */

  size_t nitems;       /* return from write_trc */

  /* calculate total number of traces */
  ntraces = 1;
  for(i=0; i<file_hdr.nkeys; i++)
    ntraces *= ((key_max[i] - key_min[i] + key_incr[i])
               / key_incr[i]);
  if(verbose)
    fprintf(stderr,"Filling %d traces\n",ntraces);

  /* alloc keys */
  keys = calloc(file_hdr.nkeys,sizeof(int));

  /* initialize key values */
  for(i=0; i<file_hdr.nkeys; i++)
    keys[i] = key_min[i];

  /* zero-fill trace */
  for(i=0; i<file_hdr.nsamp; i++)
    savetr->data[i] = 0;
  if(verbose)
    fprintf(stderr,"Zero-filled %d samples\n",file_hdr.nsamp);

  /* make sure my_partition is open */
  check_file_status(my_partition,file_status,path[0],file_hdr.filename,hdrlock,nlocks,nwaits,waittime,
                    verbose,debug);
  /* move to byte past last trace written in my_part */
  if(verbose)
    fprintf(stderr,"Move pointer for part=%d to byte %d\n",my_partition,
            trace_count*(HDRBYTES+(file_hdr.nsamp*sizeof(float))));
  fseek(fp[my_partition-1],(long)(trace_count*(HDRBYTES+(file_hdr.nsamp*sizeof(float)))),SEEK_SET);

  /* loop over ntraces, write header and zero-filled trace for all empty cube slots */
  for(i=0; i<ntraces; i++) {
    offset = cube_offset(keys,key_min,key_max,key_incr,1);
    assert(offset >= 0 && offset < file_hdr.cube_size);
    read_cube(offset,cube_buffer,my_endian,cube_in_memory,verbose);
    /* write hdr, trace */
    if(cube_buffer[0] == 0) {
      /* copy input trace */
      memcpy((void *)&trace,(const void *)savetr,HDRBYTES+4*file_hdr.nsamp);
      for(j=0; j<file_hdr.nkeys; j++) {
        hval.i = keys[j];
        puthval(&trace,key_index[j],&hval);
      }
      if(trace_count == file_hdr.traces_per_part) {
        fprintf(stderr,"End of Current Partition: Wrote %d traces to file %s\n",
                trace_count,my_file);
        efclose(fp[my_partition-1]);
        trace_count = 0;
        file_hdr.nparts++;
        /* open next part */
        j = bhp_open_write(npaths,path,verbose,"hdrs",files,"yes");
        my_partition = get_part(my_file,verbose);
      }
      trace_addr = MAX_TRACES * my_partition + trace_count;
      cube_buffer[0] = trace_addr;
      write_cube(offset,cube_buffer,my_endian,cube_in_memory,verbose);
      nitems = write_trc(&trace_count,"yes",my_endian,&trace,nlayers,nvklist,iprop,verbose);
    }
    /* Check key wrap around */
    keys[file_hdr.nkeys-1] += hdr_limits[file_hdr.nkeys-1].bhp_hdr_inc;
    for(j=file_hdr.nkeys-1; j>=0; j--) {
      if(keys[j] > hdr_limits[j].bhp_hdr_max) {
        keys[j] = hdr_limits[j].bhp_hdr_min;
        keys[j-1]++;
      }
    }
  }

  /* close all open parts */
  for(i=0; i<file_hdr.nparts; i++) {
    if(file_status[i] == 1) {
      if(verbose)
        fprintf(stderr,"Closing part %d\n",i+1);
      fclose(fp[i]);
    }
  }

}

void *catch(void)
{

  char errmsg[128];       /* error messages */

  /* try to shutdown if any of the following occur: SIGINT, SIGHUP, SIGTERM, or SIGQUIT */
  sprintf(errmsg,"Shutting down due to external termination\n");
  shutdown(errmsg,key_name,last_key_vals,my_file,trace_count,fp,file_status,
           cube_in_memory,path[0],cube,init,lockfd,&msec,verbose,debug);
  exit(EXIT_FAILURE);


}
  
