
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
/* structures */
/* Header limits */
typedef struct {
  char bhp_hdr_name[8];  /* Header name */
  int bhp_hdr_min;       /* Minimum key value for a header */
  int bhp_hdr_max;       /* Maximum key value for a header */
  int bhp_hdr_inc;       /* Key value increment(R) or nominal increment(I) */
  int bhp_hdr_scalar;    /* Scalar which needs to be applied to get actual values */
  } bhp_hdr_limits;

/* Globals */
int *partition_map;            /* partition number associated with each file
                                  e.g. if there are 3 parts: 0001, 0003, 0005
                                  partition_map[0]=1,partition_map[1]=3,partition_map[2]=5 */
bhp_hdr_limits *hdr_limits;    /* Header-limits table */
int cubefd;                    /* cube file descriptor */
int *cube;                     /* Header cube (if in memory) */
int rw_flag;                   /* Cube Read/Write flag */
#define MAX_CUBE_SIZE 10000000 /* Maximum cube size (10 million ints) to hold in memory */
/* #define MAX_CUBE_SIZE 1000 *//* Test MAX_CUBE_SIZE */

/* Prototypes */
int get_part(char *file, int verbose);
int read_hdr_limits(char *path, int verbose);
void read_cube_page(long seekto, int size, int endian_in, int verbose);
void write_cube_page(long seekto, int size, int endian_in, int verbose);
void read_cube(long offset, int *buff, int endian_in, int cube_in_memory, int verbose);
void write_cube(long offset, int *buff, int endian_in, int cube_in_memory, int verbose);
long cube_offset(int *vals, int *min, int *max, int *inc, int bin);
int get_trace(FILE **fp, int *vals, int *index, int *trace_addr, segy **traces, segy *tr,
              char *rule, int endian_in, int endian, int verbose);
void close_files(FILE **fp, int *file_status);
void save_cube(char *path, int *cube, int verbose);
void shutdown(char *errmsg, char **names, int *vals, char *file, int count, FILE **fp,
              int *file_status, int cube_in_memory, char *cpath, int *cube,
              char *init, int lockfd, struct timeval *msec, int verbose, int debug);
int create_lock_file(char *fpath, int lockfd, struct timeval *msec, int verbose, int debug);
