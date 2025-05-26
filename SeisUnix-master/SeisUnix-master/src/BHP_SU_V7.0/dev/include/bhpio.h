
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
#include <string.h>
#include <signal.h>
#include "par.h"
#include "su.h"
#include "segy.h"
#include "header.h"
#include "cwp.h"

/* structures */
/* File header */
typedef struct {
  /* following items are mandatory for all datasets */
  int npaths;            /* Number of paths used */
  char *filename;        /* User file name */
  int size;              /* Partition size */
  int nparts;            /* Number of partitions */
  int nsamp;             /* Trace length */
  int traces_per_part;   /* Traces per partition */
  int nkeys;             /* Number of keys saved when file was written */
  int endian;            /* 1=file was written on BIG_ENDIAN platform, else LITTLE_ENDIAN */
  int nprop;             /* number of properties; zero for seismic, >0 for models, <0 for events */
  /* above items are mandatory for all datasets */
  /* units is optional, not in all datasets */
  int units;             /* 0=seconds, 1=feet, 2=meters */
  /* bin and cube_size are mandatory for cube datasets, N/A for sequential */
  int bin;               /* Traces per bin */
  int cube_size;         /* Number of ints in hypercube */
  /* single-value stats are used for seismic data */
  float minval;           /* minimum value in data */
  float maxval;           /* maximum value in data */
  float meanval;          /* average value in data */
  float rmsval;           /* rms of data */
  /* following items are optional - not in all datasets */
  int data_type;          /* 0=unspecified,1=seismic, 2=horizon, 3=property */
  int data_order;         /* 0=unspecifid,1=cross-section, 2=map-view(transposed) */
  char vkey[8];           /* vertical key */
  float vmin;             /* vkey minimum value */
  float vmax;             /* vkey maximum value */
  float vinc;             /* vkey increment value */
  /* above items are optional - not in older datasets */
  /* following items are mandatory for all datasets */
  /* NOTES: the words event and horizon are used interchangeably,
     and refer to times or depths that have been picked or exported from a model.
     The partition_list, properties and prop_stats pointers are for showing how
     the file_hdr is stored on disk. The actual lists are held in the globals
     filesys and properties */
  char *partition_list;  /* partition names or paths */
  /* above items are mandatory for all datasets */
  /* properties exist if nprop != 0 */
  char **properties;     /* property or horizon names */
  /* prop_stats are used for model properties and events, one set per property/event */
  float *prop_minval;    /* min for each property if nprop!=0 */
  float *prop_maxval;    /* max for each property if nprop!=0 */
  float *prop_meanval;   /* mean for each property if nprop!=0 */
  float *prop_rmsval;    /* rms for each property if nprop!=0 */
  } bhp_file_hdr;

typedef struct {        /* timeslice structure */
  int ntimes;           /* number of times */
  int *times;           /* timeslices */
  char *opath;          /* directory */
  char *display;        /* yes or no */
} timeslice_def;

/* stats structure */
typedef struct {
  int nsamp;                /* samples per output trace */
  int ntr;                  /* number of output traces */
  float n1;                 /* sampling interval or maxdepth
                               seismic x-sect: seconds or ft/meters,
                               event x-sect: 1,
                               property x-sect: max depth or time,
                               seismic/event/property map-view: vkey inc */
  float n2;                 /* start time/depth or mindepth
                               seismic x-sect: delrt in secs or ft/meters
                               event x-sect: 0,
                               property x-sect: min depth or time,
                               seismic/event/property map-view: first vkey value */
  int *nvals;               /* Number of non-null data values for mean,rms calc
                               Can hvae multiple enrires for properties */
  } data_stats;

/* Globals */
bhp_file_hdr file_hdr;         /* File header from partition zero */
char *filesys;                 /* paths/files open, len=NAMELEN*NFILES */ 
FILE *hdrfp;                   /* Pointer to file header */
segy tr;                       /* SU trace and header */
char **properties;             /* model property names, initially allocated as 2048 by NAMELEN */
data_stats stats;              /* Structure to hold stats info not contained in file_hdr */
#define NAMELEN 256            /* maximum length of BHPIO path/filename */
#define NFILES 1024            /* maximum number of partitions allowed */
#define NULLVAL -999.25        /* property, event null values */
#define MAX_TRACES 1000000     /* Max traces per partition */
#define MAX_NKEYS 5            /* Maximum number of keys supported */
#define BHP_SIZE 1999          /* Megabytes per disk partition */
#define bhp_gettr(x,y,z)  bhp_fgettr(stdin,(x),(y),(z))
#define UNITS 1000             /* divisor to get units for request=summary  */
                               /* 1000 means dt/1000 which yields milliseconds for time, */
                               /* and meters or feet for depth (assumes dt is in millimeters/ft) */
#define million 1000000        /* multiplier for seconds to micro-seconds */

/* Prototypes */
int read_file_hdr(char *path, int verbose);
int write_file_hdr(char *path, int verbose);
int get_bhpio_path(char *path, char *name, char *ext, char *fpath, int verbose);
char *get_filename_pathlist(int verbose);
char *get_path1(char *filename, int verbose);
void get_keylist_subset(char *string, char *sub_string, int *loc);
int *parse_keylist(char *keylist_subset, int min, int max, int inc, int *nklist,
                   int interp_type, int verbose);
void parse_key(char *string, int *keylist, int *nlist, int min, int max, int inc,
               int interp_type, int verbose);
void parse_sub_string(char *sub_string, int *n1, int *n2, int *n3, int verbose);
float *parse_keylistf(char *keylist_subset, float min, float max, float inc, int *nklist,
                      int interp_type, char *rule, int verbose);
void parse_keyf(char *string, float *keylist, int *nlist, float min, float max, float inc,
                int interp_type, char *rule, int verbose);
void parse_sub_stringf(char *sub_string, float *n1, float *n2, float *n3, int verbose);
int get_endian(int my_endian, int verbose);
int *get_prop_horz(char **properties, int *nprop,  int *iz, int *nlayers,
                   int file_nprop, int nsamp, int *layernum,
                   int *layer_boundary, int verbose);
void check_timeslice(timeslice_def *timeslice, int nkeys, int verbose);
FILE **make_tfiles(int ntimes, int *times, char *opath, int verbose);
void save_timeslice(segy **traces, int nt, int ntimes, int *times, FILE **ofp);
void close_tfiles(timeslice_def *timeslice, FILE **ofp, int verbose);
void check_endian(int endian_in, int endian, segy *tp, short ns, int verbose);
void check_stdout(void);
void write_trace(segy *tr, short ns);
void subset_trace(segy *trout, int nsamp, int nvklist, float *vklist, float vdt,
                  float vdelrt, int verbose);
void build_trace(segy *tr, int file_nprop, int nprop, int *iprop, int iz,
                 int nl, int layer_boundary, float *maxd, float *mind,
                 int nvklist, float *vklist, data_stats *stats, int verbose);
void trace_stats(segy *tr, int nlayers, int nvklist, int nprop, int *iprop,
                 int layer_boundary, int verbose);
void write_stats(char *data_type, char *data_order, int vflag, int nvklist, int iz,
                 int nprop, int nlayers, float maxdepth, float mindepth, int ntr,
                 int dt, int vdt, float vdelrt, float delrt, int units, int verbose);
void set_stats(data_stats *stats, char *data_type, char *data_order, int vflag,
               int nsamp, float dt, float delrt, int nvklist, float vdt,
               float vdelrt, float vmin, float vinc, int units, int verbose);
void printmsg(char *msg, char **keyname, int *key_vals, char *part, int count);
void calc_stats(int verbose, int debug);
void *catch(void);
int *alloc_fold(int fkeys, int *nklist, int *nfold, int verbose);
void check_fold(segy *trout, segy *tr, int *fold, int *ftable, int nfold,
                float **klist, int *nklist, int *index, int nkeys, int last,
                char *request, int *is_in_cube, int my_endian, int endian, int verbose);
void write_fold(FILE *ffp, int *ftable, int nfold, int nkeys, int *nklist,
                float **klist, int verbose);
int isadir(char *path);
void adjust_keyf(float *key, float min, float max, float inc, char *rule, int interp_type, int verbose);
