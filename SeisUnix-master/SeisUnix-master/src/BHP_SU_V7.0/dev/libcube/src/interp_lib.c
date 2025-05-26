
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
#include <math.h>
#include <assert.h>
#include "bhpio.h"
#include "bhp_interp.h"
#include "su.h"
#include "segy.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

float bilin(grid_def *grid, int nx, int zoff, float x1, float y1, char *extrap,
            int *found, int verbose)
{

  float zinterp;
  float xfrac, yfrac;

  int i, j;
  int line1, line2;
  int xout=0, yout=1;  /* initialize X inside and Y outside */

  /* Both points outside? -- take nearest corner */
  /* X inside and Y outside? -- take nearest Y and co-located or interpolated X */
  /* X outside and Y inside? -- take nearest X and co-located or interpolated Y */
  /* Both points in grid? -- take grid point if co-located, else interpolate */

  *found = 0;
  zinterp = 0.;

  /* Check X */
  if(x1 < grid[0].val || x1 > grid[nx-1].val)
    xout = 1;
  /* Y? */
  for(i=0; i<nx; i++) {
    if(y1 >= grid[i].y1 && y1 <= grid[i].y2) {
      yout = 0;
      break;
    }
  }

  /* If either point is outside and extrap=no, return not found */
  if((xout == 1 || yout == 1) && !strcmp(extrap,"no"))
    return zinterp;
  /* else extrapolate */
  else if((xout == 1 || yout == 1) && !strcmp(extrap,"yes")) {
    *found = 1;
    return zinterp = extrapolate(grid,nx,zoff,x1,y1,xout,yout,verbose);
  }

  /* Both points inside */
  line1 = line2 = -1;
  /* Find values bounding x */
  for(i=1; i<nx; i++) {
    if(x1 >= grid[i-1].val && x1 <= grid[i].val) {
      *found = 1;
      if(x1 == grid[i-1].val) {
        line1 = i - 1;
        line2 = i - 1;
      }
      else if(x1 == grid[i].val) {
        line1 = i;
        line2 = i;
      }
      else {
        line1 = i - 1;
        line2 = i;
      }
      break;
    }
  }
  /* Special case -- only 1 line */
  if(nx == 1) {
    if(x1 == grid[0].val) {
      *found = 1;
      line1 = line2 = 0;
    }
  }
  /* Check y's */
  if(!*found) {
    fprintf(stderr,"Error in rectangle search\n");
    fprintf(stderr,"x1: %f\n", x1);
    err("Error in rectangle search\n");
  }
  else {
    *found = 0;
    /* y is contained in both lines */
    if(y1 >= grid[line1].y1 && y1 <= grid[line1].y2 &&
       y1 >= grid[line2].y1 && y1 <= grid[line2].y2) {
      /* Find Ys bounding y1 in line1 */
      for(i=1; i<grid[line1].n; i++) {
        if(y1 >= grid[line1].y[i-1] && y1 <= grid[line1].y[i]) {
          /* Find same Ys in line2 */
          for(j=1; j<grid[line2].n; j++) {
            if(grid[line1].y[i-1] == grid[line2].y[j-1] &&
               grid[line1].y[i] == grid[line2].y[j]) {
              *found = 1;
              /* Interpolate */
              if(line1 == line2)
                xfrac = 0.0;
              else
                xfrac = (x1 - grid[line1].val) / (grid[line2].val - grid[line1].val);
              yfrac = (y1 - grid[line1].y[i-1]) / (grid[line1].y[i] - grid[line1].y[i-1]);
              zinterp = (1.0 - xfrac) * (1.0 - yfrac) * grid[line1].z[grid[line1].n*zoff+i-1] +
                        xfrac * (1.0 - yfrac) * grid[line2].z[grid[line2].n*zoff+j-1] +
                        xfrac * yfrac * grid[line2].z[grid[line2].n*zoff+j] +
                        (1.0 - xfrac) * yfrac * grid[line1].z[grid[line1].n*zoff+i];
              /*if(*found == 1 && verbose == 1) {
                fprintf(stderr,"key1,key2,interpolated z: %f, %f, %f\n", x1,y1,zinterp);
                fprintf(stderr,"Rectangle: xc1,yc1: %f, %f\n",grid[line1].val,grid[line1].y[i-1]);
                fprintf(stderr,"           xc2,yc2: %f, %f\n",grid[line1].val,grid[line1].y[i]); 
                fprintf(stderr,"           xc3,yc3: %f, %f\n",grid[line2].val,grid[line2].y[j]);
                fprintf(stderr,"           xc4,yc4: %f, %f\n",grid[line2].val,grid[line2].y[j-1]);
              }*/
              break;
            }
          }
        }
        if(*found)
          break;
      }
    }
    else if(!strcmp(extrap,"no"))
      return 0;
    /* else if y is not contained in both lines, use extrapolation */
    else {
      xout = 0;
      yout = 1;
      zinterp = extrapolate(grid,nx,zoff,x1,y1,xout,yout,verbose);
      *found = 1;
    }
  }

  return zinterp;
}

float extrapolate(grid_def *grid, int nx, int zoff, float x1, float y1,
                  int xout, int yout, int verbose)
{

  float zval;
  float frac;

  int i, j;
  int line, trace;

  /* Both points outside? -- take nearest corner */
  if(xout == 1 && yout ==1) {
    if(x1 < grid[0].val)
      line = 0;
    else
      line = nx - 1;
    if(y1 < grid[0].y1)
      trace = 0;
    else
      trace = grid[0].n - 1;
    return zval = grid[line].z[grid[line].n*zoff+trace];
  }
  /* X outside and Y inside? -- take nearest X and co-located or interpolated Y */
  else if(xout == 1) {
    if(x1 < grid[0].val)
      line = 0;
    else
      line = nx - 1;
    /* Find a line which contains y value */
    for(i=0; i<nx; i++) {
      if(y1 >= grid[i].y1 && y1 <= grid[i].y2) {
        for(j=1; j<grid[i].n; j++) {
          if(y1 >= grid[i].y[j-1] && y1 <= grid[i].y[j]) {
            frac = (y1 - grid[i].y[j-1]) / (grid[i].y[j] - grid[i].y[j-1]);
            return zval = grid[line].z[grid[i].n*zoff+j-1] + frac *
                          (grid[line].z[grid[i].n*zoff+j] - grid[line].z[grid[i].n*zoff+j-1]); 
            break;
          }
        }
        break;
      }
    }
  }
  /* X inside and Y outside? -- take nearest Y and co-located or interpolated X */
  else if(yout == 1) {
    /* find bounds for X */
    for(i=1; i<nx; i++) {
      if(x1 >= grid[i-1].val && x1 <= grid[i].val) {
        line = i - 1;
        frac = (x1 - grid[i-1].val) / (grid[i].val - grid[i-1].val);
        break;
      }
    }
    /* Find line end point nearer Y */
    if(y1 <= grid[line].y1)
      trace = 0;
    else if(y1 >= grid[line].y2)
      trace = grid[line].n - 1;
    return zval = grid[line].z[grid[i].n*zoff+trace] + frac *
                  (grid[line+1].z[grid[i].n*zoff+trace] - grid[line].z[grid[i].n*zoff+trace]);
  }

}

grid_def *build_gridv(int **key_vector, int nx, int vlen, int verbose)
{

  grid_def *grid;

  float *yy, *zz;

  int i, j, k, m;
  int found;
  int min, max;
  int *ii;

  /* Alloc grid  and populate */
  grid = calloc(nx,sizeof(grid_def));
  /* Load Xs and number of Ys */
  for(i=0,j=0; i<nx; i++,j+=key_vector[0][j+1]+2) {
    grid[i].val = key_vector[0][j];
    grid[i].n = key_vector[0][j+1];
    grid[i].y = calloc(grid[i].n,sizeof(float));
    grid[i].z = calloc(grid[i].n,sizeof(float));
  }
  /* Load Zs */
  for(i=0,j=0; i<nx; i++,j+=key_vector[0][j+1]+2) {
    for(k=0; k<grid[i].n; k++) {
      grid[i].z[k] = key_vector[0][k+j+2];
    }
  }
  /* Load Ys */
  for(i=0; i<vlen; i+=key_vector[1][i+1]+2) {
    for(j=0; j<key_vector[1][i+1]; j++) {
      found = 0;
      for(k=0; k<nx; k++) {
        for(m=0; m<grid[k].n; m++) {
          if((float)key_vector[1][i+j+2] == grid[k].z[m]) {
            grid[k].y[m] = key_vector[1][i];
            found = 1;
            break;
          }
        }
        if(found)
          break;
      }
    }
  }
  /* Find max nyz */
  max = INT_MIN;
  for(i=0; i<nx; i++)
    max = (max > grid[i].n) ? max : grid[i].n; 
  ii = calloc(max,sizeof(int));
  yy = calloc(max,sizeof(int));
  zz = calloc(max,sizeof(int));
  /* Sort Ys  and Zs in each X */
  for(i=0; i<nx; i++) {
    for(j=0; j<grid[i].n; j++) {
      ii[j] = j;
      yy[j] = grid[i].y[j];
      zz[j] = grid[i].z[j];
    }
    qkisort(grid[i].n,yy,ii);
    grid[i].y1 = yy[ii[0]];
    grid[i].y2 = yy[ii[grid[i].n-1]];
    for(j=0; j<grid[i].n; j++) {
      grid[i].y[j] = yy[ii[j]];
      grid[i].z[j] = zz[ii[j]];
    }
  }
  /* Set min, max Y for each X */
  for(i=0; i<nx; i++) {
    min = INT_MAX;
    max = INT_MIN;
    for(j=0; j<grid[i].n; j++) {
      if(grid[i].y[j] < min)
        min = grid[i].y[j];
      if(grid[i].y[j] > max)
        max = grid[i].y[j];
    }
    grid[i].y1 = min;
    grid[i].y2 = max;
  }
  return grid;
}

grid_def *build_grid(int nr, int nz, float *x, float *y, float *z1, float *z2, int *nx, int verbose)
{

  grid_def *grid;

  int i, j;
  int *ii;
  int ny;
  int zoff;

  float *xx, *yy, *zz;

  xx = calloc(nr,sizeof(float));
  yy = calloc(nr,sizeof(float));
  zz = calloc(nr,sizeof(float));
  ii = calloc(nr,sizeof(int));

  /* Sort by X */
  for(i=0; i<nr; i++) {
    xx[i] = x[i];
    yy[i] = y[i];
    zz[i] = z1[i];
    ii[i] = i;
  }
  qkisort(nr,xx,ii);
  for(i=0; i<nr; i++) {
    x[i] = xx[ii[i]];
    y[i] = yy[ii[i]];
    z1[i] = zz[ii[i]];
  }
  if(nz == 2) {
    for(i=0; i<nr; i++)
      zz[i] = z2[i];
    for(i=0; i<nr; i++)
      z2[i] = zz[ii[i]];
  }
  /* Count number of Xs */
  *nx = 1;
  for(i=1; i<nr; i++) {
    if(x[i] != x[i-1])
      (*nx)++;
  }
  if(verbose)
    fprintf(stderr,"Number of Xs: %d\n", *nx);

  /* Alloc grid  and populate */
  grid = calloc(*nx,sizeof(grid_def));
  grid[0].val = x[0];
  ny = 1;
  for(i=1,j=0; i<nr; i++) {
    if(x[i] != x[i-1]) {
      grid[j].n = ny;
      grid[j].y = calloc(ny,sizeof(float));
      if(nz == 1)
        grid[j].z = calloc(ny,sizeof(float));
      else if(nz == 2)
        grid[j].z = calloc(2*ny,sizeof(float));
      ny = 1;
      j++;
      grid[j].val = x[i];
    }
    else
      ny++;
  }
  grid[j].n = ny;
  grid[j].y = calloc(ny,sizeof(float));
  if(nz == 1)
    grid[j].z = calloc(ny,sizeof(float));
  else if(nz == 2)
    grid[j].z = calloc(2*ny,sizeof(float));
  /* Load Ys and Zs */
  zoff = 0;
  for(i=0; i<*nx; i++) {
    for(j=0; j<grid[i].n; j++) {
      grid[i].y[j] = y[zoff+j];
      grid[i].z[j] = z1[zoff+j];
      if(nz == 2)
        grid[i].z[grid[i].n+j] = z2[zoff+j];
    }
    grid[i].y1 = grid[i].y[0];
    grid[i].y2 = grid[i].y[grid[i].n-1];
    zoff += grid[i].n;
  }
  /* Sort Ys  and Zs in each X */
  for(i=0; i<*nx; i++) {
    for(j=0; j<grid[i].n; j++) {
      ii[j] = j;
      yy[j] = grid[i].y[j];
      zz[j] = grid[i].z[j];
    }
    qkisort(grid[i].n,yy,ii);
    grid[i].y1 = yy[ii[0]];
    grid[i].y2 = yy[ii[grid[i].n-1]];
    for(j=0; j<grid[i].n; j++) {
      grid[i].y[j] = yy[ii[j]];
      grid[i].z[j] = zz[ii[j]];
    }
    if(nz == 2) {
      for(j=0; j<grid[i].n; j++)
        zz[j] = grid[i].z[grid[i].n+j];
      for(j=0; j<grid[i].n; j++)
        grid[i].z[grid[i].n+j] = zz[ii[j]];
    }
  }
  return grid;
}

grid_def *load_grid(char *fpath, char *key1, char *key2, int n, int file_endian,
                    int my_endian, int verbose)
{

  grid_def *grid;

  FILE *vfp;

  char gkey1[8], gkey2[8];

  float *y, *z;

  int i;
  int nx;
  int nyz=0;

  size_t nitems;

  if(verbose)
    fprintf(stderr,"Opening %s\n", fpath);
  vfp = fopen(fpath,"r");
  if(!vfp)
    err("Failed to open grid file\n");
  efread(gkey1,sizeof(char),8,vfp);
  efread(gkey2,sizeof(char),8,vfp);
  efread(&nx,sizeof(int),1,vfp);
  if(file_endian != my_endian)
    swap_int_4(&nx);
  if(verbose) {
    fprintf(stderr,"Keys from grid file: %s, %s\n", gkey1,gkey2);
    fprintf(stderr,"Keys: %s, %s\n",key1,key2);
    fprintf(stderr,"Number of Xs from grid file: %d\n", nx);
    fprintf(stderr,"Number of Xs: %d\n", n);
  }
  /* verify nx, keys match, else return NULL */
  if(nx != n || strcmp(key1,gkey1) || strcmp(key2,gkey2))
    return grid = NULL;
  /* load grid */
  else {
    grid = calloc(nx,sizeof(grid_def));
    nitems = fread(grid,sizeof(grid_def),nx,vfp);
    if(nitems != nx)
      err("Requested %d grid cells, got %d\n",nx,nitems);
    if(file_endian != my_endian) {
      for(i=0; i<nx; i++) {
        swap_float_4(&grid[i].val);
        swap_int_4(&grid[i].n);
        swap_float_4(&grid[i].y1);
        swap_float_4(&grid[i].y2);
      }
    }
    /* count number of Ys and Zs to load */
    for(i=0; i<nx; i++)
      nyz += grid[i].n;
    y = calloc(nyz,sizeof(float));
    z = calloc(nyz,sizeof(float));
    nitems = fread(y,sizeof(float),nyz,vfp);
    if(nitems != nyz)
      err("Requested %d Ys, get %d\n",nyz,nitems);
    nitems = fread(z,sizeof(float),nyz,vfp);
    if(nitems != nyz)
      err("Requested %d Zs, get %d\n",nyz,nitems);
    if(file_endian != my_endian) {
      for(i=0; i<nyz; i++) {
        swap_float_4(&y[i]);
        swap_float_4(&z[i]);
      }
    }
    /* set pointers */
    grid[0].y = y;
    grid[0].z = z;
    for(i=1; i<nx; i++) {
      grid[i].y = grid[i-1].y + grid[i-1].n; 
      grid[i].z = grid[i-1].z + grid[i-1].n; 
    }
  }
  return grid;

}

float *build_line(int pk1, int pk2, int sk1, int sk2, int pinc, int sinc, int *nlist, int verbose)
{

  int n, i;

  float incr;
  float xd,yd,dist;
  float xinc, yinc;
  float *list;

  xd = pk2 - pk1;
  yd = sk2 - sk1;
  dist = sqrt(xd * xd + yd * yd);
  incr = sqrt(pinc * pinc + sinc * sinc);
  n = NINT((dist + incr) / incr);
  xinc = xd / dist * incr;
  yinc = yd / dist * incr;

  if(verbose) {
    fprintf(stderr,"pk1=%d,sk1=%d,pk2=%d,sk2=%d,pinc=%d,sinc=%d\n", pk1,sk1,pk2,sk2,pinc,sinc);
    fprintf(stderr,"dist=%f,incr=%f,n=%d,xd=%f,yd=%f,xinc=%f,yinc=%f\n",dist,incr,n,xd,yd,xinc,yinc);
  }

  list = calloc(2*n,sizeof(float));

  /* build list - use floats for keys */
  list[0] = pk1;
  list[1] = sk1;
  for(i=2; i<2*n; i+=2) {
    list[i] = list[i-2] + xinc;
    list[i+1] = list[i-1] + yinc;
  }

  if(verbose) {
    fprintf(stderr,"Arbitrary List\n");
    for(i=0; i<2*n; i+=2)
      fprintf(stderr,"%f,%f ",list[i],list[i+1]);
    fprintf(stderr,"\n");
  }

  *nlist = n;
  return list;

}

int interp_trace(grid_def *grid, FILE **fp, float xloc, float yloc, segy **traces,
                 int type, int nx, int my_endian, int file_endian, int nsamp, int verbose)
{

  int i, j;
  int found=0;
  int line1, line2;
  int t[4];
  int trace;
  int index[4]={0,1,2,3};
  static int prev_trace=-1;

  float xfrac, yfrac;
  float dist[4];
  float xsq;

  /* Check loc vs grid limits */
  if(xloc < grid[0].val || xloc > grid[nx-1].val)
    return 0;
  for(i=0; i<nx; i++) {
    if(yloc >= grid[i].y1 && yloc <= grid[i].y2) {
      found = 1;
      break;
    }
  }
  if(!found)
    return 0;
  found = 0;

  /* Find Xs bounding xval */
  for(i=1; i<nx; i++) {
    if(xloc >= grid[i-1].val && xloc <= grid[i].val) {
      found = 1;
      if(xloc == grid[i-1].val) {
        line1 = i - 1;
        line2 = i - 1;
      }
      else if(xloc == grid[i].val) {
        line1 = i;
        line2 = i;
      }
      else {
        line1 = i - 1;
        line2 = i;
      }
      break;
    }
  }
  if(!found)
    err("Error locating %d in grid\n", xloc);
  /* Make sure yloc is contained in both lines */
  if(yloc < grid[line1].y1 || yloc > grid[line1].y2 ||
     yloc < grid[line2].y1 || yloc > grid[line2].y2)
    return 0;

  found = 0;
  /* Find Ys bounding yloc */
  for(i=1; i<grid[line1].n; i++) {
    if(yloc >= grid[line1].y[i-1] && yloc <= grid[line1].y[i]) {
      for(j=1; j<grid[line2].n; j++) {
        if(grid[line1].y[i-1] == grid[line2].y[j-1] &&
           grid[line1].y[i] == grid[line2].y[j]) {
          found = 1;
          t[0] = grid[line1].z[i-1];
          t[1] = grid[line1].z[i];
          t[2] = grid[line2].z[j-1];
          t[3] = grid[line2].z[j];
          /* If bilin, compute fractions, else compute distance to each corner */
          if(type == 2) {
            if(line1 == line2)
              xfrac = 0.0;
            else if(xloc == grid[line2].val)
              xfrac = 1.0;
            else
              xfrac = ((float)xloc - (float)grid[line1].val) /
                      ((float)grid[line2].val - (float)grid[line1].val);
            if(yloc == grid[line1].y[i-1])
              yfrac = 0.0;
            else if(yloc == grid[line1].y[i])
              yfrac = 1.0;
            else
              yfrac = (yloc - grid[line1].y[i-1]) /
                      (grid[line1].y[i] - grid[line1].y[i-1]);
          }
          else {           /*interpolate=nearest */
            xsq = (xloc - grid[line1].val) * (xloc - grid[line1].val);
            dist[0] = sqrt(xsq + ((yloc - grid[line1].y[i-1]) * (yloc - grid[line1].y[i-1])));
            dist[1] = sqrt(xsq + ((yloc - grid[line1].y[i]) * (yloc - grid[line1].y[i])));
            xsq = (xloc - grid[line2].val) * (xloc - grid[line2].val);
            dist[2] = sqrt(xsq + ((yloc - grid[line2].y[j-1]) * (yloc - grid[line2].y[j-1])));
            dist[3] = sqrt(xsq + ((yloc - grid[line2].y[j]) * (yloc - grid[line2].y[j])));
            qkisort(4,dist,index);
            trace = t[index[0]];
          }
          break;
        }
      }
    }
  }
  if(!found)
    err("Error locating %d in grid\n", yloc);

  /* If nearest trace is same as previous return not found */
  if(trace == prev_trace)
    return 0;
  prev_trace = trace;

  if(verbose) {
    if(type == 2) {
      fprintf(stderr,"Trace addresses %d, %d, %d, %d\n", t[0],t[1],t[2],t[3]);
      fprintf(stderr,"xfrac,yfrac %f, %f\n", xfrac,yfrac);
    }
    else
      fprintf(stderr,"Nearest trace address %d\n", trace);
  }

  /* Load traces */
  if(type == 2) {
    for(i=0; i<4; i++) {
      efseek(fp[t[i]/1000000],
             (t[i] % 1000000)*(nsamp+60)*sizeof(float),SEEK_SET);
      efread(traces[i],sizeof(float),nsamp+(HDRBYTES/4),fp[t[i]/1000000]);
      check_endian(file_endian,my_endian,traces[i],(short)nsamp,verbose);
    }
    memcpy((void *)traces[4],(const void *)traces[0],HDRBYTES);
    /* Interpolate */
    for(i=0; i<nsamp; i++)
      traces[4]->data[i] = (1.0 - xfrac) * (1.0 - yfrac) * traces[0]->data[i] +
                     xfrac * (1.0 - yfrac) * traces[3]->data[i] +
                     xfrac * yfrac * traces[2]->data[i] +
                     (1.0 - xfrac) * yfrac * traces[1]->data[i];
  }
  else {
    efseek(fp[trace/1000000],
           (trace % 1000000)*(nsamp+60)*sizeof(float),SEEK_SET);
    efread(traces[4],sizeof(float),nsamp+(HDRBYTES/4),fp[trace/1000000]);
    check_endian(file_endian,my_endian,traces[4],(short)nsamp,verbose);
  }

  return 1;

}

int get_corners(float pkey, float skey, int *min, int *inc, int *max, int **corner, int verbose)
{

  int step;
  int stat;

  step = 0;
  stat = 1;

  for(;;) {
    if(min[0] + (step * inc[0]) <= pkey && min[0] + ((step + 1) * inc[0]) >= pkey) {
      corner[0][0] = min[0] + (step * inc[0]);
      corner[1][0] = corner[0][0];
      corner[2][0] = min[0] + ((step + 1) * inc[0]);
      corner[3][0] = corner[2][0];
      break;
    }
    step++;
    if(min[0] + (step * inc[0]) > max[0]) {
      stat = 0;
      break;
    }
  }
  if(stat == 1) {
    step = 0;
    for(;;) {
      if(min[1] + (step * inc[1]) <= skey && min[1] + ((step + 1) * inc[1]) >= skey) {
        corner[0][1] = min[1] + (step * inc[1]);
        corner[1][1] = min[1] + ((step + 1) * inc[1]);
        corner[2][1] = corner[1][1];
        corner[3][1] = corner[0][1];
        break;
      }
      step++;
      if(min[1] + (step * inc[1]) > max[1]) {
        stat = 0;
        break;
      }
    }
  }

  return stat;

}

void compute_fractions(float pkey, float skey, int **corner, float *xfrac,
                       float *yfrac, int verbose)
{

  *xfrac = (pkey - corner[0][0]) / (corner[3][0] - corner[0][0]);
  *yfrac = (skey - corner[0][1]) / (corner[1][1] - corner[0][1]);

}

void bilin_interp_trace(segy **ctrace, segy *trace, float xfrac, float yfrac, float null, int verbose)
{

  int i;
  int inull;

  inull = null;
  for(i=0; i<trace->ns; i++) {
    if((int)ctrace[0]->data[i] == inull || (int)ctrace[1]->data[i] == inull ||
       (int)ctrace[2]->data[i] == inull || (int)ctrace[3]->data[i] == inull )
      trace->data[i] = null;
    else
    trace->data[i] = (1.0 - xfrac) * (1.0 - yfrac) * ctrace[0]->data[i] +
                     xfrac * (1.0 - yfrac) * ctrace[3]->data[i] +
                     xfrac * yfrac * ctrace[2]->data[i] +
                     (1.0 - xfrac) * yfrac * ctrace[1]->data[i];
}
    
}

int get_nearest(float pkey, float skey, int **corner, int verbose)
{

  float dist[4];
  float x2, y2;
  float min;

  int i;
  int nearest;

  for(i=0; i<4; i++) {
    x2 = (pkey - (float)corner[i][0]) * (pkey - (float)corner[i][0]);    
    y2 = (skey - (float)corner[i][1]) * (skey - (float)corner[i][1]);    
    dist[i] = sqrt(x2 + y2);
  }

  min = FLT_MAX;
  for(i=0; i<4; i++) {
    if(dist[i] < min) {
      nearest = i;
      min = dist[i];
    }
  }

  return nearest;

}

xyz_def *build(int nr, int nz, float *x, float *y, float **z, float null, char *interp, int verbose)
{

  xyz_def *xyz;

  int i, j, k, m;
  int *ii;
  int *ny;
  int nx;

  float *xx, *yy, **zz;
  float *y1, *y2;
  float yy2;
  float x2;

  /* xyz_def structure */
  xyz = calloc(1,sizeof(xyz_def));

  /* temp arrays for sorting */
  xx = calloc(nr,sizeof(float));
  yy = calloc(nr,sizeof(float));
  zz = calloc(nz,sizeof(float *));
  for(i=0; i<nz; i++)
    zz[i] = calloc(nr,sizeof(float));
  ii = calloc(nr,sizeof(int));

  /* number of Ys per X */
  ny = calloc(nr,sizeof(int));

  /* Sort by X */
  for(i=0; i<nr; i++) {
    xx[i] = x[i];
    yy[i] = y[i];
    for(j=0; j<nz; j++)
      zz[j][i] = z[j][i];
    ii[i] = i;
  }
  qkisort(nr,xx,ii);
  for(i=0; i<nr; i++) {
    x[i] = xx[ii[i]];
    y[i] = yy[ii[i]];
    for(j=0; j<nz; j++)
      z[j][i] = zz[j][ii[i]];
  }

  /* Count number of Xs and Ys in each X, find min, max X, min dx */
  xyz->x1 = x[0];
  x2 = x[0];
  xyz->dx = 1;
  nx = 1;
  j = 1;
  k = 0;
  for(i=1; i<nr; i++) {
    if(x[i] != x[i-1]) {
      ny[k] = j;
      nx++;
      j = 1;
      k++;
      if(x[i] < xyz->x1)
        xyz->x1 = x[i];
      if(x[i] > x2)
        x2 = x[i];
      if(x[i] - x[i-1] < xyz->dx)
        xyz->dx = x[i] - x[i-1];
    }
    else
      j++;
  }
  ny[k] = j;
  /*if(verbose) {
    fprintf(stderr,"Number of Xs in data: %d, MINX: %f, MAXX: %f\n", nx,xyz->x1,x2);
    fprintf(stderr,"Ys per each X\n");
    for(i=0; i<nx; i++) {
      fprintf(stderr,"%d ",ny[i]);
      if(((i+1)%10) == 0)
        fprintf(stderr,"\n");
    }
    fprintf(stderr,"\n");
  }*/

  /* Find min, max Y, in each X, min dy */
  y1 = calloc(nx,sizeof(float));
  y2 = calloc(nx,sizeof(float));
  xyz->dy = FLT_MAX;
  for(i=0,k=0; i<nx; i++,k+=ny[i-1]) {
    y1[i] = FLT_MAX;
    y2[i] = FLT_MIN;
    for(j=0; j<ny[i]; j++) {
      if(y[k+j] < y1[i])
        y1[i] = y[k+j];
      if(y[k+j] > y2[i])
        y2[i] = y[k+j];
    }
    for(j=1; j<ny[i]; j++) {
      if(ABS(y[k+j] - y[k+j-1]) < xyz->dy)
        xyz->dy = ABS(y[k+j] - y[k+j-1]); 
    }
  }
  /*if(verbose) {
    fprintf(stderr,"Min/Max Y per X\n");
    for(i=0; i<nx; i++) {
      fprintf(stderr," %f %f ",y1[i],y2[i]);
      if(((i+1)%10) == 0)
        fprintf(stderr,"\n");
    }
    fprintf(stderr,"\n");
    fprintf(stderr,"DY: %f\n",xyz->dy);
  }*/

  /* find global y1,y2, max ny */
  xyz->y1 = FLT_MAX;
  yy2 = FLT_MIN;
  for(i=0; i<nx; i++) {
    if(y1[i] < xyz->y1)
      xyz->y1 = y1[i];
    if(y2[i] > yy2)
      yy2 = y2[i];
  }
  if(verbose)
    fprintf(stderr,"Min, Max Ys %f %f\n", xyz->y1,yy2);

  xyz->nx = (x2 - xyz->x1 + xyz->dx) / xyz->dx;
  xyz->ny = (yy2 - xyz->y1 + xyz->dy) / xyz->dy;

  /* alloc Z */
  xyz->nz = nz;
  xyz->z = calloc(xyz->nz,sizeof(float **));
  for(i=0; i<xyz->nz; i++) {
    xyz->z[i] = calloc(xyz->nx,sizeof(float *));
    for(j=0; j<xyz->nx; j++)
      xyz->z[i][j] = calloc(xyz->ny,sizeof(float));
  }

  /* fill with nulls */
  for(i=0; i<xyz->nz; i++) {
    for(j=0; j<xyz->nx; j++) {
      for(k=0; k<xyz->ny; k++)
        xyz->z[i][j][k] = null;
    }
  }

  if(verbose) {
    fprintf(stderr,"XYZ: \n");
    fprintf(stderr,"NX: %d, X1: %f, DX: %f\n",xyz->nx,xyz->x1,xyz->dx);
    fprintf(stderr,"NY: %d, Y1: %f, DY: %f\n",xyz->ny,xyz->y1,xyz->dy);
  }

  /* fill from input data */
  for(i=0; i<nr; i++) {
    for(j=0; j<xyz->nz; j++) {
      k = (x[i] - xyz->x1) / xyz->dx;
      m = (y[i] - xyz->y1) / xyz->dy;
      xyz->z[j][k][m] = z[j][i];
    }
  }

  /* if interp=yes, infill interpolate inline direction, i.e. replace single null surrounded by non-nulls */
  if(!strcmp(interp,"yes")) {
    for(i=0; i<xyz->nx; i++) {
      for(j=1; j<xyz->ny-1; j++) {
        for(k=0; k<xyz->nz; k++) {
          if(xyz->z[k][i][j] == null && xyz->z[k][i][j-1] != null && xyz->z[k][i][j+1] != null)
            xyz->z[k][i][j] = 0.5 * xyz->z[k][i][j-1] + 0.5 * xyz->z[k][i][j+1];
        }
      }
    }
  }

  /* if interp=yes, infill interpolate xline direction, i.e. replace single null surrounded by non-nulls */
  if(!strcmp(interp,"yes")) {
    for(i=0; i<xyz->ny; i++) {
      for(j=1; j<xyz->nx-1; j++) {
        for(k=0; k<xyz->nz; k++) {
          if(xyz->z[k][j][i] == null && xyz->z[k][j-1][i] != null && xyz->z[k][j+1][i] != null)
            xyz->z[k][j][i] = 0.5 * xyz->z[k][j-1][i] + 0.5 * xyz->z[k][j+1][i];
        }
      }
    }
  }

  if(verbose) {
    fprintf(stderr,"Zs\n");
    for(i=0; i<xyz->nx; i++) {
      for(j=0; j<xyz->ny; j++) {
        for(k=0; k<xyz->nz; k++) {
          fprintf(stderr," %f ",xyz->z[k][i][j]);
          if(((j+1)%10) == 0)
            fprintf(stderr,"\n");
        }
      }
      fprintf(stderr,"\n");
    }
  }

  /* release uneeded buffers */
  free(xx);
  free(yy);
  free(zz);
  free(ii);
  free(ny);
  free(y1);
  free(y2);

  return xyz;

}

void bilinear(float *z, xyz_def *xyz, float x1, float y1, char *extrap, char *interpolate,
              float null, int verbose)
{

  float xi, yi, sx, sy;
  float f1, f2, f3, f4;

  int ix, iy;
  int i;

  xi = (x1 - xyz->x1) / xyz->dx;
  yi = (y1 - xyz->y1) / xyz->dy;

  /* check extrap option */
  if(!strcmp(extrap,"no")) {
    if(xi > xyz->nx - 1 || xi < 0 || yi > xyz->ny - 1 || yi < 0) {
      for(i=0; i<xyz->nz; i++)
        z[i] = null;
      return;
    }
  }

  if(xi > xyz->nx - 1)
    xi = xyz->nx - 1;
  if(xi < 0)
    xi = 0;
  if(yi > xyz->ny - 1)
    yi = xyz->ny - 1;
  if(yi < 0)
    yi = 0;
  ix = xi;
  iy = yi;
  sx = xi - ix;
  sy = yi - iy;
  f1 = (1. - sx) * (1. - sy);
  f2 = sx  * (1. - sy);
  f3 = sx * sy;
  f4 = (1. - sx) * sy;

  /* if interpolate=no and f1 != 1, return null */
  if(!strcmp(interpolate,"no") && f1 != 1.) {
    for(i=0; i<xyz->nz; i++)
      z[i] = null;
  }
  else {
    for(i=0; i<xyz->nz; i++)
     z[i] = interp(xyz->z[i],xyz->nx,xyz->ny,ix,iy,f1,f2,f3,f4,null);
  }

  if(verbose) {
    fprintf(stderr,"xi=%f,yi=%f,ix=%d,iy=%d,sx=%f,sy=%f,f1=%f,f2=%f,f3=%f,f4=%f",
            xi,yi,ix,iy,sx,sy,f1,f2,f3,f4);
    fprintf(stderr," ZVALS: ");
    for(i=0; i<xyz->nz; i++)
      fprintf(stderr,"%f ",z[i]);
    fprintf(stderr,"\n");
  }
}

float interp(float **z, int nx, int ny, int ix, int iy, float f1, float f2,
             float f3, float f4, float null)
{

  float zz;

  /* if any contributing corner point is null, return null */
  if(z[ix][iy] == null  && f1 != 0)
    return null;
  else
    zz = f1 * z[ix][iy];
  if(ix < nx - 1 && z[ix+1][iy] == null && f2 != 0)
    return null;
  else if(ix < nx - 1)
   zz += f2 * z[ix+1][iy];
  if(ix < nx - 1 && iy < ny - 1 && z[ix+1][iy+1] == null && f3 != 0)
    return null;
  else if(ix < nx - 1 && iy < ny - 1)
    zz += f3 * z[ix+1][iy+1];
  if(iy < ny - 1 && z[ix][iy+1] == null && f4 != 0)
    return null;
  else if(iy < ny - 1)
    zz += f4 * z[ix][iy+1];

  return zz;

}

minmax *get_data_limits(int nr, float **x, int n, int verbose)
{

  int i;
  int *ii;

  float *xx;

  minmax *mm;

  mm = calloc(1,sizeof(minmax));
  xx = calloc(nr,sizeof(float));
  ii = calloc(nr,sizeof(int));

  for(i=0; i<nr; i++) {
    xx[i] = x[i][n];
    ii[i] = i;
  }

  qkisort(nr,xx,ii);

  mm->minx = xx[ii[0]];
  mm->maxx = xx[ii[nr-1]];
  mm->incx = FLT_MAX;
  for(i=1; i<nr; i++) {
    if(ABS(xx[ii[i]] - xx[ii[i-1]]) < mm->incx && xx[ii[i]] != xx[ii[i-1]])
      mm->incx = ABS(xx[ii[i]] - xx[ii[i-1]]);
  }
  mm->nx = (mm->maxx - mm->minx + mm->incx) / mm->incx;

  return mm;

}

float *load_file_to_cube(int nkeys, char *file, int *cube_size, int npicks,
                         minmax **limits, float null, int verbose)
{

  char buffer[1024];
  char *s;

  FILE *infp;

  int i, j, nr;
  int k;

  float **x, **y;
  float *cube;

  /* open file */
  if((infp=fopen(file,"r"))==NULL)
    err("Cannot open %s\n",file);

  /* count records */
  nr = 0;
  while(fgets(buffer,1024,infp) != NULL)
    nr++;
  rewind(infp);
  if(verbose)
    fprintf(stderr,"%d records in %s\n", nr,file);

  /* alloc arrays for all data */
  x = calloc(nr,sizeof(float *));
  y = calloc(nr,sizeof(float *));
  for(i=0; i<nr; i++) {
    x[i] = calloc(nkeys,sizeof(float));
    y[i] = calloc(npicks,sizeof(float));
  }

  /* Load file */
  for(i=0; i<nr; i++) {
    fgets(buffer,1024,infp);
    s = strtok(buffer," ");
    x[i][0] = atof(s);
    for(j=1; j<nkeys; j++) {
      s = strtok(NULL," ");
      x[i][j] = atof(s);
    }
    for(j=0; j<npicks; j++) {
      s = strtok(NULL," ");
      y[i][j] = atof(s);
    }
    /*if(verbose) {
      fprintf(stderr,"keys=");
      for(j=0; j<nkeys; j++)
        fprintf(stderr,"%.3f ",x[i][j]);
      fprintf(stderr,"horizons=");
      for(j=0; j<npicks; j++)
        fprintf(stderr,"%.3f ",y[i][j]);
    }*/
  }
  efclose(infp);
  
  /* set data limits */
  for(i=0; i<nkeys; i++)
    limits[i] = get_data_limits(nr,x,i,verbose);

  if(verbose) {
    for(i=0; i<nkeys; i++)
      fprintf(stderr,"MIN: %.3f, MAX: %.3f, INCR: %.3f; N: %d\n",
              limits[i]->minx,limits[i]->maxx,limits[i]->incx,limits[i]->nx);
  }

  /* Calculate cube size */
  *cube_size = limits[0]->nx;
  for(i=1; i<nkeys; i++)
    *cube_size *= limits[i]->nx;
  /* multiply by npicks per location */
  *cube_size *= npicks;
  cube = calloc(*cube_size,sizeof(float));
  if(verbose)
    fprintf(stderr,"Allocated %d floats for cube\n",*cube_size);

  /* initialize cube with nulls */
  for(i=0; i<*cube_size; i++)
    cube[i] = null;

  /* populate cube */
  for(i=0; i<nr; i++) {
    j = cube_lookup(nkeys,x[i],limits,npicks,verbose);
    assert(j >= 0 && j < *cube_size);
    for(k=0; k<npicks; k++)
      cube[j+k] = y[i][k];
  }

  return cube;
}

xyz_def *load_file_to_grid(char *file, float null, int nz, char *interp, int verbose)
{

  char buffer[1024];
  char *s;

  FILE *infp;

  int i, nr;
  int j;

  float *x, *y;
  float **z;

  xyz_def *grid;

  /* open file */
  if((infp=fopen(file,"r"))==NULL)
    err("Cannot open %s\n",file);

  /* allocate space for file */
  nr = 0;
  while(fgets(buffer,1024,infp) != NULL)
    nr++;
  rewind(infp);
  if(verbose)
    fprintf(stderr,"%d records in %s\n", nr,file);
  x = calloc(nr,sizeof(float *));
  y = calloc(nr,sizeof(float));
  z = calloc(nz,sizeof(float *));
  for(i=0; i<nz; i++)
    z[i] = calloc(nr,sizeof(float));

  /* Load file */
  for(i=0; i<nr; i++) {
    fgets(buffer,1024,infp);
    s = strtok(buffer," ");
    x[i] = atof(s);
    s = strtok(NULL," ");
    y[i] = atof(s);
    for(j=0; j<nz; j++) {
      s = strtok(NULL," ");
      z[j][i] = atof(s);
    }
    /*if(verbose) {
      fprintf(stderr,"keys=%f,%f  horz=%f ",x[i],y[i],z[0][i]);
      for(j=1; j<nz; j++)
        fprintf(stderr,"%f ",z[j][i]);
      fprintf(stderr,"\n");
    }*/
  }
  efclose(infp);

  /* make grid */
  return grid = build(nr,nz,x,y,z,null,interp,verbose);

}

int cube_lookup(int nkeys, float *x, minmax **limits, int bin, int verbose)
{

  int offset=0;

  int i, j, term;

  for(i=0; i<nkeys; i++) {
    if(i == nkeys - 1)
      term = (x[i] - limits[i]->minx) / limits[i]->incx;
    else {
      term = 1;
      for(j=i+1; j<nkeys; j++)
        term *= (limits[j]->maxx - limits[j]->minx + limits[j]->incx) / limits[j]->incx;
      term *= ((x[i] - limits[i]->minx) / limits[i]->incx);
    }
    offset += term;
  }

  offset *= bin;

  return offset;

}

int check_limits(int nkeys, float *val, minmax **limits, int verbose)
{

  int i;

  int stat=0;

  /* if val is within limits, return 0 */
  /* if val is outside limits, return 1 */
  for(i=0; i<nkeys; i++) {
    if(val[i] < limits[i]->minx || val[i] > limits[i]->maxx) {
      stat = 1;
      break;
    }
  }

  return stat;

}

int cube_extrap(int nkeys, float *val, minmax **limits, int bin, int verbose)
{

  int i;

  for(i=0; i<nkeys; i++) {
    if(val[i] < limits[i]->minx)
      val[i] = limits[i]->minx;
    if(val[i] > limits[i]->maxx)
      val[i] = limits[i]->maxx;
  }

  i = cube_lookup(nkeys,val,limits,bin,verbose);

  return i;

}
 
int find_non_null(float *x, int start, int stride, int end, float null, int verbose)
{

  int i;

  if(end >= start) {
    for(i=start; i<end; i+=stride) {
      if(x[i] != null)
        return i;
    }
  }
  else {
    for(i=start; i>=end; i-=stride) {
      if(x[i] != null)
        return i;
    }
  }

  return -1;

}

float *build_lines(char *keylist, int *narb, int inc1, int inc2, int verbose)
{

  char *string;

  int i, j, k, n;
  int nseg;
  int pk1, sk1, pk2, sk2;
  int nlist;

  float xd, yd, dist, incr;
  float xinc, yinc;
  float *arblist;

  /* allocate scratch space for strtok */
  string = calloc((int)strlen(keylist)+1,sizeof(char));

  /* change ^ to , */
  nseg = 0;
  for(i=0; i<(int)strlen(keylist); i++) {
    if(keylist[i] == '^') {
      keylist[i] = ',';
      nseg++;
    }
  }

  /* verify number of entries is even and >=4 */
  for(i=0,j=0; i<(int)strlen(keylist); i++) {
    if(keylist[i] == ',') {
      j++;
    }
  }
  /* assume key after last comma */
  j++;
  /* number of keys must be even and at least 4 */
  if((j % 2) != 0)
    err("For arbitrary traverse, number of primary and secondary keys must be equal\n");
  if(j < 4)
    err("For arbitrary traverse, there must be at least 2 primary and 2 secondary values\n");

  /* count total number of points in all segments, allocate list */
  strcpy(string,keylist);
  nlist = 0;
  for(i=0; i<nseg; i++) {
    if(i > 0) {
      pk1 = pk2;
      sk1 = sk2;
    }
    else {
      pk1 = atoi(strtok(string,","));
      sk1 = atoi(strtok(NULL,","));
    }
    pk2 = atoi(strtok(NULL,","));
    sk2 = atoi(strtok(NULL,","));
    xd = pk2 - pk1;
    yd = sk2 - sk1;
    dist = sqrt(xd * xd + yd * yd);
    incr = sqrt(inc1 * inc1 + inc2 * inc2);
    nlist += NINT((dist + incr) / incr);
  }
  arblist = calloc(2*nlist,sizeof(float));
  if(verbose)
    fprintf(stderr,"%d segments, total arblist length is %d\n",nseg,nlist);
  
  /* loop over list, build each line segment from starting and ending keys */
  j = 0;
  strcpy(string,keylist);
  for(i=0; i<nseg; i++) {
    if(i > 0) {
      pk1 = pk2;
      sk1 = sk2;
    }
    else {
      pk1 = atoi(strtok(string,","));
      sk1 = atoi(strtok(NULL,","));
    }
    pk2 = atoi(strtok(NULL,","));
    sk2 = atoi(strtok(NULL,","));
    xd = pk2 - pk1;
    yd = sk2 - sk1;
    dist = sqrt(xd * xd + yd * yd);
    incr = sqrt(inc1 * inc1 + inc2 * inc2);
    n = NINT((dist + incr) / incr);
    xinc = xd / dist * incr;
    yinc = yd / dist * incr;
    if(verbose) {
      fprintf(stderr,"pk1=%d,sk1=%d,pk2=%d,sk2=%d,inc1=%d,inc2=%d\n", pk1,sk1,pk2,sk2,inc1,inc2);
      fprintf(stderr,"dist=%f,incr=%f,n=%d,xd=%f,yd=%f,xinc=%f,yinc=%f\n",dist,incr,n,xd,yd,xinc,yinc);
    }
    /* build segment list - use floats for keys */
    arblist[j] = pk1;
    arblist[j+1] = sk1;
    for(k=j+2; k<j+2*n; k+=2) {
      arblist[k] = arblist[k-2] + xinc;
      arblist[k+1] = arblist[k-1] + yinc;
    }
    j += 2 * n;
  }

  *narb = nlist;
  return arblist;

}
