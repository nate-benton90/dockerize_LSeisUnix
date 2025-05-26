
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
#include "bhpio_api.h"

/* local prototypes */
int nearest_corner(int *trace);

int *open_bhpio_dataset(char *filename, char *pathlist, int *nkeys, int verbose)
{

  char cindex[8];        /* For appending partition numbers */
  char path[NAMELEN];    /* File path */
  char *path1;           /* First path from pathlist */
  char fpath[NAMELEN];   /* File header path */

  int i;

  /* get path to first partition */
  path1 = get_path1(filename,verbose);

  /* alloc name in file_hdr */
  file_hdr.filename = calloc(strlen(filename)+1,sizeof(char));

  /* verify data is cube format by checking LOCK file */
  if(!get_bhpio_path(path1,filename,"_LOCK.HDR",fpath,verbose))
    err("%s is not a cube dataset\n",filename);

  /* Make sure file is there */
  if(!get_bhpio_path(path1,filename,"_0000.HDR",fpath,verbose))
    err("FILE: path=%s, filename=%s does not exist\n",path1,file_hdr.filename);
  /* Get file header */
  read_file_hdr(fpath,verbose);

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
    if(verbose)
      fprintf(stderr,"Opening %s\n", path);
    fp[i] = fopen(path,"rb");
    if(!fp[i])
      err("Could not open %s\n", path);
  }

  /* hdr_limits */
  key_index = calloc(file_hdr.nkeys,sizeof(int));
  key_min = calloc(file_hdr.nkeys,sizeof(int));
  key_max = calloc(file_hdr.nkeys,sizeof(int));
  key_incr = calloc(file_hdr.nkeys,sizeof(int));
  hdr_limits = calloc(file_hdr.nkeys,sizeof(bhp_hdr_limits));
  if(!get_bhpio_path(path1,file_hdr.filename,"_0000.0001.HDR",fpath,verbose))
    err("Could not access file: %s\n", fpath);
  /* Load hdr_limits */
  if(read_hdr_limits(fpath,verbose))
    err("Error reading %s\n", fpath);
  /* fill min, max, incr */
  for(i=0; i<file_hdr.nkeys; i++) {
    key_index[i] = getindex(hdr_limits[i].bhp_hdr_name);
    key_min[i] = hdr_limits[i].bhp_hdr_min;
    key_max[i] = hdr_limits[i].bhp_hdr_max;
    key_incr[i] = hdr_limits[i].bhp_hdr_inc;
  }
  if(verbose)
    for(i=0; i<file_hdr.nkeys; i++)
      fprintf(stderr,"KEY: %s, INDEX: %d, MIN: %d, MAX: %d, INC: %d\n",
              hdr_limits[i].bhp_hdr_name,key_index[i],key_min[i],key_max[i],key_incr[i]);

  /* alloc enough traces to hold a bin */
  traces = calloc(file_hdr.bin,sizeof(float *));
  for(i=0; i<file_hdr.bin; i++)
    traces[i] = calloc(file_hdr.nsamp+(HDRBYTES/4),sizeof(float));

  /* for interpolation allocate traces and coords for each corner */
  ctrace = calloc(4,sizeof(float *));
  corner = calloc(4,sizeof(int *));
  for(i=0; i<4; i++) {
    ctrace[i] = calloc(file_hdr.nsamp+(HDRBYTES/4),sizeof(float));
    corner[i] = calloc(2,sizeof(int));
  }

  /* Open cube */
  get_bhpio_path(path1,file_hdr.filename,"_0000.0002.HDR",fpath,verbose);
  if(verbose)
    fprintf(stderr,"Opening %s\n",fpath);
  cubefd = open(fpath,O_RDONLY);
  if(cubefd == -1)
    err("Failed to open cube %s\n", fpath);
  /* cube buff */
  cube_buffer = calloc(file_hdr.bin,sizeof(int));

  /* set defaults for binning and interp */
  strcpy(rule,"near");
  strcpy(interp_flag,"no");

  /* set nkeys */
  *nkeys = file_hdr.nkeys;

  /* nasmp */
  NSAMP = file_hdr.nsamp;

  return key_index;

}

int get_bhpio_header_limits(int *min, int *max, int *incr, int verbose)
{

  int i;
  
  for(i=0; i<file_hdr.nkeys; i++) {
    min[i] = key_min[i];
    max[i] = key_max[i];
    incr[i] = key_incr[i];
  }

  return 0;

}

int read_bhpio_trace(int *keyvals, segy *trace, int verbose)
{

#ifdef CWP_BIG_ENDIAN    /* Set default endian */
  int my_endian = 1;
#else
  int my_endian = 0;
#endif
  int trcount;
  int ntraces=0;        /* should be 4 for interp */
  int i, k;
  int vals[2];

  float xfrac, yfrac;

  long offset;

  Value hval;

  if(!strcmp(interp_flag,"no")) {
    offset = cube_offset(keyvals,key_min,key_max,key_incr,file_hdr.bin);
    assert(offset >= 0 && offset < file_hdr.cube_size);
    read_cube(offset,cube_buffer,my_endian,0,verbose);
    trcount = get_trace(fp,keyvals,key_index,cube_buffer,traces,
                        trace,"match",file_hdr.endian,my_endian,verbose);
    memcpy((void *)trace,(const void *)traces[0],HDRBYTES+4*file_hdr.nsamp);
  }
  /* bilinear interp */
  else {
    if((get_corners(keyvals[0],keyvals[1],key_min,key_incr,key_max,corner,verbose)) == 0)
      err("%d, %d is outside grid\n",keyvals[0],keyvals[1]);
    /*if(verbose)
      fprintf(stderr,"C1: %d, %d, C2: %d, %d, C3: %d, %d. C4: %d, %d\n",
              corner[0][0],corner[0][1],corner[1][0],corner[1][1],
              corner[2][0],corner[2][1],corner[3][0],corner[3][1]);*/
    compute_fractions(keyvals[0],keyvals[1],corner,&xfrac,&yfrac,verbose);
    for(k=0; k<4; k++) {
      vals[0] = corner[k][0];
      vals[1] = corner[k][1];
      offset = cube_offset(vals,key_min,key_max,key_incr,file_hdr.bin);
      assert(offset >= 0 && offset < file_hdr.cube_size);
      read_cube(offset,cube_buffer,my_endian,0,verbose);
      trcount = get_trace(fp,vals,key_index,cube_buffer,traces,
                          ctrace[k],"match",file_hdr.endian,my_endian,verbose);
      if(trcount != 0)
        ntraces++;
      /* zero corner coords if no trace */
      else {
        corner[k][0] = 0;
        corner[k][1] = 0;
      }
      memcpy((void *)ctrace[k],(const void *)traces[0],HDRBYTES+4*file_hdr.nsamp);
    }
    /* initialize output hdr */
    memcpy((void *)trace,(const void *)ctrace[0],HDRBYTES);
    /* if all corners not present, just use nearest */
    if(ntraces != 4) {
      i = nearest_corner(keyvals);
      if(i < 0)
        return trcount = 0;
      memcpy((void *)trace,(const void *)ctrace[i],HDRBYTES+4*file_hdr.nsamp);
    }
    /* interp if we have 4 corners */
    else
      bilin_interp_trace(ctrace,trace,xfrac,yfrac,NULLVAL,verbose);
    hval.i = keyvals[0];
    puthval(trace,key_index[0],&hval);
    hval.i = keyvals[1];
    puthval(trace,key_index[1],&hval);
    trcount = 1;
  }

  return trcount;

}

void close_bhpio_dataset()
{

  int i;

  for(i=0; i<file_hdr.nparts; i++)
    fclose(fp[i]);

  close(cubefd);

}

void set_bhpio_binning_rule(char *new_rule, int verbose)
{

  if(!strcmp(new_rule,"match") || !strcmp(new_rule,"near"))
    strcpy(rule,new_rule);
  else
    err("%s is not a legal binning rule\n",new_rule);

  if(verbose)
    fprintf(stderr,"BHPIO binning rule = %s\n",new_rule);

}

void set_bhpio_interp_flag(char *new_flag, int verbose)
{

  if(!strcmp(new_flag,"no") || !strcmp(new_flag,"yes"))
    strcpy(interp_flag,new_flag);
  else
    err("%s is not a legal interp value\n",new_flag);

  if(verbose)
    fprintf(stderr,"BHPIO interp flag = %s\n",new_flag);

}
 
int nearest_corner(int *keyvals)
{

  float dist[4];
  float min;

  int idist;
  int i;

  /* calc distance from keyvals to each non-zero corner point */
  for(i=0; i<4; i++) {
    dist[i] = FLT_MAX;
    if(corner[i][0] != 0 && corner[i][1] != 0)
      dist[i] = sqrt((keyvals[0] - corner[i][0]) * (keyvals[0] - corner[i][0]) +
                     (keyvals[1] - corner[i][1]) * (keyvals[1] - corner[i][1]));
  }
  /* find shortest dist */
  idist = -1;
  min = FLT_MAX;
  for(i=0; i<4; i++) {
    if(dist[i] < min) {
      min = dist[i];
      idist = i;
    }
  }

  return idist;

}
