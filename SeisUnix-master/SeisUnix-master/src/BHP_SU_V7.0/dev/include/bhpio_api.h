
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
#include <assert.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "bhpio.h"
#include "bhpiocube.h"

/* Globals */
segy **traces;                 /* bin of traces */
segy **ctrace;                 /* traces for each corner needed for interpolation */
FILE *fp[NFILES];                 /* partitions */
int *key_min;                  /* Header min values from hdr_limits */
int *key_max;                  /* Header max values from hdr_limits */
int *key_incr;                 /* Header inc values from hdr_limits */
int *key_index;                /* Key indices in header */
int *cube_buffer;              /* buffer to hold trace addresses from cube */
int **corner;                  /* keys of corner traces for interpolation */
char interp_flag[8];           /* no=get match or nearest, yes=get match or interpolated */
char rule[8];                  /* binning rule - near or match */
int NSAMP;                     /* samples per trace from file_hdr */

/* Prototypes */
int read_file_hdr(char *path, int verbose);
int get_bhpio_path(char *path, char *name, char *ext, char *fpath, int verbose);
char *get_path1(char *filename, int verbose);
void check_endian(int endian_in, int endian, segy *tp, short ns, int verbose);
int read_hdr_limits(char *path, int verbose);
void read_cube(long offset, int *buff, int endian_in, int cube_in_memory, int verbose);
long cube_offset(int *vals, int *min, int *max, int *inc, int bin);
int get_trace(FILE **fp, int *vals, int *index, int *trace_addr, segy **traces, segy *tr,
              char *rule, int endian_in, int endian, int verbose);
int *open_bhpio_dataset(char *filename, char *pathlist, int *nkeys, int verbose);
int get_bhpio_header_limits(int *min, int *max, int *incr, int verbose);
int read_bhpio_trace(int *keyvals, segy *trace, int verbose);
void close_bhpio_dataset();
void set_bhpio_binning_rule(char *new_rule, int verbose);
void set_bhpio_interp_flag(char *new_flag, int verbose);
void compute_fractions(int pkey, int skey, int **corner, float *xfrac, float *yfrac, int verbose);
void bilin_interp_trace(segy **ctrace, segy *trace, float xfrac, float yfrac, float null, int verbose);
int get_corners(int pkey, int skey, int *min, int *inc, int *max, int **corner, int verbose);
