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
/* globals */
/* grid structure used for bilinear interpolation */
typedef struct {
  float val;       /* X-value */
  int n;           /* Number of Ys, this X */
  float y1;        /* First Y, this X */
  float y2;        /* Last Y, this X */
  float *y;        /* Ys for this X */
  float *z;        /* Zs for this X */
  } grid_def;

/* NEW grid structure */
typedef struct {
  int nx;      /* number of Xs */
  float x1;    /* minimum X */
  float dx;    /* X incr */
  int ny;      /* number of Ys */
  float y1;    /* minimum Y */
  float dy;    /* Y incr */
  int nz;      /* number of Zs per XY */
  float ***z;  /* pointer to Zs */
  } xyz_def;

/* minmax */
typedef struct {
  float minx;
  float maxx;
  float incx;
  int nx;
  } minmax;

/* prototypes for interp_lib */
int find_non_null(float *x, int start, int stride, int end, float null, int verbose);
float *load_file_to_cube(int nkeys, char *file, int *cube_size, int npicks,
                         minmax **limits, float null, int verbose);
xyz_def *load_file_to_grid(char *file, float null, int nz, char *interp, int verbose);
int cube_lookup(int nkeys, float *x, minmax **limits, int bin, int verbose);
int check_limits(int nkeys, float *val, minmax **limits, int verbose);
int cube_extrap(int nkeys, float *val, minmax **limits, int bin, int verbose);
minmax *get_data_limits(int nr, float **x, int n, int verbose);
float *build_line(int pk1, int pk2, int sk1, int sk2, int xinc, int yinc, int *nlist, int verbose);
float *build_lines(char *keylist, int *narb, int inc1, int inc2, int verbose);
int interp_trace(grid_def *grid, FILE **fp, float xloc, float yloc, segy **traces,
                 int type, int nx, int my_endian, int file_endian, int nsamp, int verbose);
float bilin(grid_def *grid, int nx, int zoff, float x1, float y1,
            char *extrap, int *found, int verbose);
float extrapolate(grid_def *grid, int nx, int zoff, float x1, float y1,
                  int xout, int yout, int verbose);
grid_def *build_grid(int nr, int nz, float *x, float *y, float *z1, float *z2,
                     int *nx, int verbose);
grid_def *build_gridv(int **key_vector, int nx, int vlen, int verbose);
grid_def *load_grid(char *fpath, char *key1, char *key2, int n, int file_endian,
                    int my_endian, int verbose);
int get_nearest(float pkey, float skey, int **corner, int verbose); 
void compute_fractions(float pkey, float skey, int **corner, float *xfrac, float *yfrac, int verbose);
void bilin_interp_trace(segy **ctrace, segy *tr, float xfrac, float yfrac, float null, int verbose);
int get_corners(float pkey, float skey, int *min, int *inc, int *max, int **corner, int verbose);
xyz_def *build(int nr, int nz, float *x, float *y, float **z, float null, char *interp, int verbose);
void bilinear(float *z, xyz_def *xyz, float x1, float y1, char *extrap, char *interp,
              float null, int verbose);
float interp(float **z, int nx, int ny, int ix, int iy, float f1, float f2,
             float f3, float f4, float null);
