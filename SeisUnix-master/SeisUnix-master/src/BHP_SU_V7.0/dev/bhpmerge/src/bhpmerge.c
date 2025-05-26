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
#include <sys/types.h>
#include "bhpio.h"
#include "bhpioseq.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
" bhpmerge filenames=file1,file2...  [optional parameters]         ",
"                                                                  ",
" BHPMERGE merges the header index files of requested bhpio        ",
" datasets which were written by bhpwriteseq into a single index   ",
" file which can be read as 'bhpread filename='file1'_MERGED ...'  ", 
" A pathlist file for the merged dataset is also built.            ",
"                                                                  ",
" Required Parameters:                                             ",
"  filenames=file1,file2,... File names to be merged               ", 
"                                                                  ",
" Optional Parameters:                                             ",
"  pathlists='file1'.dat,'file2'.dat,...                           ",
"                              ASCII files containing lists of     ",
"                              paths used to create the files to be",
"                              merged                              ",
"                                                                  ",
NULL};

/* Globals */
bhp_hdr_limits *global_hdr_limits;     /* Merged hdr_limits */
bhp_file_hdr global_file_hdr;          /* Merged file-header */
char *global_filesys;                  /* List of all partitions */
int **vlen;              /* Length of key-vectors for each key in each partition */
int **vloc;              /* Location of key-vectors for each key in each partition */

/* Prototypes */
int write_hdr_limits(char *path, int verbose);
void quick_sort(int *sortv, int *index, int m);
int load_hdr_limits(char *lpath, int verbose);
int write_global_hdr_limits(char *path, int verbose);

int main(int argc, char **argv)
{

  char fpath[NAMELEN];    /* Partiiton zero path name */
  char lpath[NAMELEN];    /* Header limits path */
  char vpath[NAMELEN];    /* Key vector path */
  char cindex[8];         /* Use to append file index to path */
  char **pathlists;       /* Pathlist files  */
  char **filenames;       /* Filenames */
  char **path1;           /* Path to first partition of each file */
  char command[256];      /* For ln -s command */

  int nfiles;             /* Number of files to be merged */
  int verbose;            /* debug printout */
  int i, j, k, m;         /* Loop counters*/
  int loc;                /* Current loc in global-key-vector */
  int numg;               /* Number of key occurences - global */
  int *slot;              /* 1=this partition contributed to current pass of global build */
  int *vndx;              /* Current location in each key vector */
  int gndx;               /* Current location in global key vector */
  int *index;             /* Sort work array for global vector */
  int gvlen;              /* Length of global vector */
  int npaths;             /* Number of user-specified paths */
  int **key_vector;       /* Sorted keys, disk addrs - per file */
  int *tempv;             /* Array for building global vectors */
  int *sortv;             /* Sort work array for global vector */
  int min;                /* For building global vector */
  int sumlen;             /* Total length of all key-vectors for a key */
  int *nparts;            /* Number of partitions, each file */
  int *bias;              /* Amount to add to trace numbers in each file's key-vector */

  FILE *vfp;          /* Pointer to vector file */
  FILE *vfpg;         /* Pointer to global vector file */
  FILE *hfp;          /* Use for file headers */
  FILE *pfile;        /* Command pipe */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  i = countparval("pathlists");
  nfiles = countparval("filenames");
  if(nfiles < 2)
    err("Must have at least 2 filenames");
  /* if pathlists is used, number of entries must equal number of filename entries */
  if(verbose) {
    fprintf(stderr,"Counted %d pathlists\n", i);
    fprintf(stderr,"Counted %d filenames\n", nfiles);
  }
  if(i == 1)
    err("Must have at least 2 pathlists");
  if(i >0 && nfiles != i)
    err("Number of pathlists and filenames must be equal\n");
  filenames = calloc(nfiles,sizeof(char *));
  getparstringarray("filenames",filenames);
  if(i > 0)
    getparstringarray("pathlists",pathlists);
  else {
    pathlists = calloc(nfiles,sizeof(char *));
    for(i=0; i<nfiles; i++) {
      pathlists[i] = calloc(strlen(filenames[i])+5,sizeof(char));
      strcpy(pathlists[i],filenames[i]);
      strcat(pathlists[i],".dat");
    }
  }
  if(verbose) {
    fprintf(stderr,"pathlists\n");
    for(i=0; i<nfiles; i++)
      fprintf(stderr,"   %s\n", pathlists[i]);
    fprintf(stderr,"\n");
    fprintf(stderr,"filenames\n");
    for(i=0; i<nfiles; i++)
      fprintf(stderr,"   %s\n", filenames[i]);
  }

  /* Verify first partition of each file */
  path1 = calloc(nfiles,sizeof(char *));
  for(i=0; i<nfiles; i++) {
    path1[i] = calloc(NAMELEN,sizeof(char));
    fprintf(stderr,"Opening %s\n", pathlists[i]);
    if(!(hfp = fopen(pathlists[i],"r")))
      err("Cannot open %s\n", pathlists[i]);
    else {
      fscanf(hfp,"%s\n", path1[i]);
      fclose(hfp);
    }
    /* Make sure file is there */
    if(!get_bhpio_path(path1[i],filenames[i],"_0000.HDR",fpath,verbose))
      err("FILE: path=%s, filename=%s does not exist\n",path1[i],filenames[i]);
  }

  /* Open each file header */
  file_hdr.filename = calloc(NAMELEN,sizeof(char));
  /* Initialize merged file header */
  global_file_hdr.npaths = 1;
  global_file_hdr.size = 0;
  global_file_hdr.nparts = 0;
  global_file_hdr.traces_per_part = 0;
  /* Index into global_filesys */
  k = 0;
  nparts = calloc(nfiles,sizeof(int));
  bias = calloc(nfiles,sizeof(int));
  /* Count total number of partitions, so filesys array can be allocated */
  for(i=0; i<nfiles; i++) {
    if(!get_bhpio_path(path1[i],filenames[i],"_0000.HDR",fpath,verbose))
      err("FILE: path=%s, filename=%s does not exist\n",path1[i],filenames[i]);
    read_file_hdr(fpath,verbose);
    /* Running total of all partitions */
    global_file_hdr.nparts += file_hdr.nparts;
    nparts[i] = file_hdr.nparts;
  }
  fprintf(stderr,"Total number of partitions: %d\n", global_file_hdr.nparts);
  global_filesys = calloc(global_file_hdr.nparts*NAMELEN,sizeof(char));
 fprintf(stderr,"Allocated %d chars for global_filesys\n", global_file_hdr.nparts*NAMELEN);
  for(i=0; i<nfiles; i++) {
    if(!get_bhpio_path(path1[i],filenames[i],"_0000.HDR",fpath,verbose))
      err("FILE: path=%s, filename=%s does not exist\n",path1[i],filenames[i]);
    read_file_hdr(fpath,verbose);
    if(verbose) {
      fprintf(stderr,"File Name from File Header %s\n", file_hdr.filename);
      fprintf(stderr,"Partition Size from File Header %d\n", file_hdr.size);
      fprintf(stderr,"Number of Partitions from File Header %d\n", file_hdr.nparts);
      fprintf(stderr,"Samples per Trace from File Header %d\n", file_hdr.nsamp);
      fprintf(stderr,"Traces per Partition from File Header %d\n", file_hdr.traces_per_part);
      fprintf(stderr,"Number of Keys from File Header %d\n", file_hdr.nkeys);
    }
    if(i == 0) {
      /* Use name_MERGED, nsamp, nkeys from first file */
      global_file_hdr.filename = calloc(strlen(filenames[0])+8,sizeof(char));
      strcpy(global_file_hdr.filename,filenames[0]);
      strcat(global_file_hdr.filename,"_MERGED");
      global_file_hdr.nsamp = file_hdr.nsamp;
      global_file_hdr.nkeys = file_hdr.nkeys;
      global_file_hdr.endian = file_hdr.endian;
    }
    else {
      /* Verify nsamp, nkeys are same for all files */
      if(file_hdr.nsamp != global_file_hdr.nsamp) {
        fprintf(stderr,"Number of samples per trace must be the same for all files being merged\n");
        fprintf(stderr,"First file is %d, File %d is %d\n", global_file_hdr.nsamp,i+1,file_hdr.nsamp);
        err("Number of samples per trace must be the same for all files being merged\n");
      }
      if(file_hdr.nkeys != global_file_hdr.nkeys) {
        fprintf(stderr,"Number of header keys must be the same for all files being merged\n");
        fprintf(stderr,"First file is %d, File %d is %d\n", global_file_hdr.nkeys,i+1,file_hdr.nkeys);
        err("Number of header keys must be the same for all files being merged\n");
      }
    }
    /* Add partition names and filename from current file to global list */
    for(j=0; j<file_hdr.nparts; j++) {
      strcpy(&global_filesys[k*NAMELEN+(j*NAMELEN)],&filesys[j*NAMELEN]);
      /* if path is a dir, append filename, else filename is already appended */
      if(isadir(&global_filesys[k*NAMELEN+(j*NAMELEN)]) == 1) {
        if(verbose)
          fprintf(stderr,"%s is a directory, appending filename\n",&global_filesys[k*NAMELEN+(j*NAMELEN)]);
        strcat(&global_filesys[k*NAMELEN+(j*NAMELEN)],"/");
        strcat(&global_filesys[k*NAMELEN+(j*NAMELEN)],file_hdr.filename);
        strcat(&global_filesys[k*NAMELEN+(j*NAMELEN)],"_");
        sprintf(cindex,"%04i\0",j+1);
        strcat(&global_filesys[k*NAMELEN+(j*NAMELEN)],cindex);
        strcat(&global_filesys[k*NAMELEN+(j*NAMELEN)],".su");
      }
      else {
        if(verbose)
          fprintf(stderr,"%s is a file, not appending filename\n",&global_filesys[k*NAMELEN+(j*NAMELEN)]);
      }
    }
    k += file_hdr.nparts;
    free(filesys);
  }
  fprintf(stderr,"Global Partition List\n");
  for(i=0; i<global_file_hdr.nparts; i++)
    fprintf(stderr,"  %s\n", &global_filesys[i*NAMELEN]);

  /* Write merged header */
  get_bhpio_path(path1[0],global_file_hdr.filename,"_0000.HDR",fpath,verbose);
  if(write_global_file_hdr(fpath,verbose))
    err("Failed to write file header\n");

  /* Verify that header limits of all files have the same keys */
  vlen = calloc(global_file_hdr.nkeys,sizeof(int *));
  vloc = calloc(global_file_hdr.nkeys,sizeof(int *));
  for(i=0; i<global_file_hdr.nkeys; i++) {
    vlen[i] = calloc(nfiles,sizeof(int));
    vloc[i] = calloc(nfiles,sizeof(int));
  }
  hdr_limits = calloc(global_file_hdr.nkeys,sizeof(bhp_hdr_limits));
  global_hdr_limits = calloc(global_file_hdr.nkeys,sizeof(bhp_hdr_limits));
  for(i=0; i<nfiles; i++) {
    if(!get_bhpio_path(path1[i],filenames[i],"_0000.0001.HDR",lpath,verbose))
      err("Could not access file: %s\n", lpath);
    if(load_hdr_limits(lpath,verbose))
      err("Error loading hdr_limits for %s\n", filenames[i]);
    for(j=0; j<global_file_hdr.nkeys; j++) {
      /* If this is first file, use it to initialize global_hdr_limits */
      if(i == 0) {
        strcpy(global_hdr_limits[j].bhp_hdr_name,hdr_limits[j].bhp_hdr_name);
        global_hdr_limits[j].bhp_hdr_min = hdr_limits[j].bhp_hdr_min;
        global_hdr_limits[j].bhp_hdr_max = hdr_limits[j].bhp_hdr_max;
        global_hdr_limits[j].bhp_hdr_inc = hdr_limits[j].bhp_hdr_inc;
        global_hdr_limits[j].bhp_hdr_scalar = hdr_limits[j].bhp_hdr_scalar;
        global_hdr_limits[j].bhp_hdr_num = hdr_limits[j].bhp_hdr_num;
        global_hdr_limits[j].bhp_hdr_order = hdr_limits[j].bhp_hdr_order;
        global_hdr_limits[j].bhp_hdr_type = hdr_limits[j].bhp_hdr_type;
        strcpy(global_hdr_limits[j].bhp_hdr_data,hdr_limits[j].bhp_hdr_data);
        global_hdr_limits[j].bhp_hdr_vlen = hdr_limits[j].bhp_hdr_vlen;
        global_hdr_limits[j].bhp_hdr_vloc = hdr_limits[j].bhp_hdr_vloc;
      }
      /* Otherwise, check hdr_name */
      else {
        if(strcmp(global_hdr_limits[j].bhp_hdr_name,hdr_limits[j].bhp_hdr_name)) {
          fprintf(stderr,"Header name %s, in file %s, does not match %s in %s\n",
                  hdr_limits[j].bhp_hdr_name,filenames[i],global_hdr_limits[j].bhp_hdr_name,filenames[0]);
          err("Header names mis-match\n");
        }
      }
      /* Update min, max */
      if(hdr_limits[j].bhp_hdr_min < global_hdr_limits[j].bhp_hdr_min)
        global_hdr_limits[j].bhp_hdr_min = hdr_limits[j].bhp_hdr_min;
      if(hdr_limits[j].bhp_hdr_max > global_hdr_limits[j].bhp_hdr_max)
        global_hdr_limits[j].bhp_hdr_max = hdr_limits[j].bhp_hdr_max;
      /* Capture vlen, vloc */
      vlen[j][i] = hdr_limits[j].bhp_hdr_vlen;
      vloc[j][i] = hdr_limits[j].bhp_hdr_vloc;
    }
  }
  
  if(verbose) {
    fprintf(stderr,"Global Header Limits\n");
    for(i=0; i<global_file_hdr.nkeys; i++)
      fprintf(stderr,"Name: %s, Min: %d, Max: %d, Inc: %d, Order: %d, Type: %d\n",
              global_hdr_limits[i].bhp_hdr_name,global_hdr_limits[i].bhp_hdr_min,
              global_hdr_limits[i].bhp_hdr_max,global_hdr_limits[i].bhp_hdr_inc,
              global_hdr_limits[i].bhp_hdr_order,global_hdr_limits[i].bhp_hdr_type);
  }
  /* Write merged hdr-limits */
  get_bhpio_path(path1[0],global_file_hdr.filename,"_0000.0001.HDR",lpath,verbose);
  if(write_global_hdr_limits(lpath,verbose))
    err("Error writing global_hdr_limits\n");
  fprintf(stderr,"Wrote %s\n", lpath);
  
  if(verbose) {
    for(i=0; i<global_file_hdr.nkeys; i++) {
      fprintf(stderr,"vlen for %s\n", global_hdr_limits[i].bhp_hdr_name);
      for(j=0; j<nfiles; j++)
        fprintf(stderr,"%d  ", vlen[i][j]);
      fprintf(stderr,"\n");
    }
  }

  /* Merge each file's global-key-vector into new global vector for all files */
  vndx = calloc(nfiles,sizeof(int));
  slot = calloc(nfiles,sizeof(int));
  loc = 0;

  key_vector = calloc(nfiles,sizeof(int *));

  /* Compute each file's bias */
  for(i=1; i<nfiles; i++)
    bias[i] = bias[i-1] + 1000000 * nparts[i-1];
  if(verbose) {
    fprintf(stderr,"Bias for each file\n");
    for(i=0; i<nfiles; i++)
      fprintf(stderr,"%d  ",bias[i]);
    fprintf(stderr,"\n");
  }

  /* Open global key vector file */
  get_bhpio_path(path1[0],global_file_hdr.filename,"_0000.0002.HDR",vpath,verbose);
  fprintf(stderr,"Opening %s\n", vpath);
  vfpg = fopen(vpath,"w");
  if(!vfpg)
    err("Failed to open key vectors %s\n", vpath);

  for(i=0; i<global_file_hdr.nkeys; i++) {
    /* Load all vectors for current header key */
    for(j=0; j<nfiles; j++) {
      key_vector[j] = calloc(vlen[i][j],sizeof(int));
      if(verbose)
        fprintf(stderr,"Allocated %d ints for key-vector %d\n", vlen[i][j],i+1);
      strcpy(vpath,path1[j]);
      strcat(vpath,"/");
      strcat(vpath,filenames[j]);
      strcat(vpath,"_0000.0002.HDR");
      fprintf(stderr,"Opening %s\n", vpath);
      vfp = fopen(vpath,"r");
      if(!vfp)
        err("Failed to open key vectors %s\n", vpath);
      if(verbose)
        fprintf(stderr,"Seek to %d to read %d ints for next key-vector\n", vloc[i][j],vlen[i][j]);
      efseek(vfp,vloc[i][j],SEEK_SET);
      efread(key_vector[j],sizeof(int),vlen[i][j],vfp);
      efclose(vfp);
      /* Add each file's bias to all it's trace numbers */
      for(k=0; k<vlen[i][j]; k+=key_vector[j][k+1]+2)
        for(m=0; m<key_vector[j][k+1]; m++)
          key_vector[j][k+2+m] += bias[j];
    }

    /* Build global vector for current key */
    numg = 0;
    sumlen = 0;
    for(j=0; j<nfiles; j++) {
      if(verbose)
        fprintf(stderr,"Key-vector length for file %d is %d\n", j+1,vlen[i][j]);
      sumlen += vlen[i][j];
    }
    /* Final len will be <= sumlen */
    tempv = calloc(sumlen,sizeof(int));
    gndx = 0;
    sortv = calloc(sumlen,sizeof(int));
    index = calloc(sumlen,sizeof(int));
    if(verbose)
      fprintf(stderr,"Allocated %d ints for tempv\n", sumlen);
    gvlen = 0;
    /* Set location in each vector to zero */
    for(j=0; j<nfiles; j++)
      vndx[j] = 0;
    for(;;) {
      /* Initialize indices for sort */
      for(j=0; j<sumlen; j++)
        index[j] = j;
      /* Find next lowest key */
      min = INT_MAX;
      /* Set all slots as not containing min */
      for(j=0; j<nfiles; j++)
        slot[j] = -1;
      /* Find minimum remaining key value */
      for(j=0; j<nfiles; j++)
        if(vndx[j] < vlen[i][j] && key_vector[j][vndx[j]] < min)
          min = key_vector[j][vndx[j]];
      /* Set slot for all parts which contain min */
      for(j=0; j<nfiles; j++)
        if(vndx[j] < vlen[i][j] && key_vector[j][vndx[j]] == min)
          slot[j] = 1;
      k = 0;
      for(j=0; j<nfiles; j++) {
        if(slot[j] >= 0) {
          k++;
          break;
        }
      }
      if(!k)
          err("Build global vector failed - MIN not found\n");
      /* Put key value in next slot in global vector and bump number of key values */
      tempv[gndx] = min;
      numg++;
      /* Count all nums for min */
      m = 0;
      for(j=0; j<nfiles; j++)
        if(vndx[j] < vlen[i][j] && key_vector[j][vndx[j]]  == min)
          m += key_vector[j][vndx[j]+1];
      if(m == 0)
        err("Build global vector failed - NUMG=0\n");
      /* Store number of occurences in next global slot and update global vector length */
      tempv[gndx+1] = m;
      gvlen += 2 + m;
      m = 0;
      /* Sort all addresses for current slot */
      for(j=0; j<nfiles; j++) {
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
      for(j=0; j<nfiles; j++) {
        if(slot[j] == 1) {
          if(vndx[j] < vlen[i][j])
            vndx[j] += 2 + key_vector[j][vndx[j]+1];
        }
      }
      /* See if at least one vector still has data */
      m = 0;
      for(j=0; j<nfiles; j++)
        if(vndx[j] < vlen[i][j])
          m++;
      /* All are at end, go to next key */
      if(!m)
        break;
/*
      for(j=0; j<gndx; j+=8) {
        for(m=0; m<8; m++)
          fprintf(stderr,"%d ", tempv[j+m]);
        fprintf(stderr,"\n");
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
    efwrite(tempv,sizeof(int),global_hdr_limits[i].bhp_hdr_vlen,vfpg);
    if(verbose) {
      fprintf(stderr,"Finished key vector for %s\n", global_hdr_limits[i].bhp_hdr_name);
      fprintf(stderr,"Seek to byte %d to write %d ints\n", global_hdr_limits[i].bhp_hdr_vloc,
                      global_hdr_limits[i].bhp_hdr_vlen);
    }
    /* release buffers */
    free(tempv);
    free(sortv);
    free(index);
    for(j=0; j<nfiles; j++)
      free(key_vector[j]);
  }
  efclose(vfpg);
  if(verbose)
    fprintf(stderr,"Finished global key-vector\n");

  /* Get path and write global hdr_limits */
  get_bhpio_path(path1[0],global_file_hdr.filename,"_0000.0001.HDR",fpath,verbose);
  /* Copy global_hdr_limits */
  if(write_global_hdr_limits(fpath,verbose))
    err("Failed to write header limits\n");

  /* Release buffers */
  free(hdr_limits);
  free(global_hdr_limits);
  free(filesys);
  for(i=0; i<file_hdr.nkeys; i++) {
    free(vlen[i]);
    free(vloc[i]);
  }
  free(key_vector);
  free(vndx);
  free(slot);
  free(vlen);
  free(vloc);

  /* Make a link to first pathlist file for MERGED dataset */
  strcpy(command,"ln -s ");
  strcat(command,filenames[0]);
  strcat(command,".dat ");
  strcat(command,filenames[0]);
  strcat(command,"_MERGED.dat");
  fprintf(stderr,"Executing %s\n", command);
  pfile = popen(command,"w");
  pclose(pfile);

  return EXIT_SUCCESS;
}
 
int write_global_file_hdr(char *path, int verbose)
{

  int i, j;

  /* Copy global file header to file_hdr */
  file_hdr.npaths = global_file_hdr.npaths;
  strcpy(file_hdr.filename,global_file_hdr.filename);
  file_hdr.nparts = global_file_hdr.nparts;
  file_hdr.cube_size = 0;
  if(filesys != NULL)
    filesys = calloc(global_file_hdr.nparts*NAMELEN,sizeof(char));
  for(i=0; i<global_file_hdr.nparts; i++)
    strcpy(&filesys[i*NAMELEN],&global_filesys[i*NAMELEN]);

  /* write new file header */
  write_file_hdr(path,verbose);

  return 0;

}

int load_hdr_limits(char *path, int verbose)
{

    int i, stat;

  /* Open header limits file */
  if(verbose)
    fprintf(stderr,"Opening %s\n", path);
  hdrfp = fopen(path,"r");
  if(!hdrfp) {
    fprintf(stderr,"Could not open %s\n", path);
    return 1;
  }

  /* Loop through header limits info */
  stat = 0;
  for(i=0; i<file_hdr.nkeys; i++) {
    /* Match, load limits info */
    fscanf(hdrfp,"HDRNAME = %s\n", hdr_limits[i].bhp_hdr_name);
    fscanf(hdrfp,"MIN = %d\n", &hdr_limits[i].bhp_hdr_min);
    fscanf(hdrfp,"MAX = %d\n", &hdr_limits[i].bhp_hdr_max);
    fscanf(hdrfp,"INCR = %d\n", &hdr_limits[i].bhp_hdr_inc);
    fscanf(hdrfp,"SCALAR = %d\n", &hdr_limits[i].bhp_hdr_scalar);
    fscanf(hdrfp,"NUM = %d\n", &hdr_limits[i].bhp_hdr_num);
    fscanf(hdrfp,"ORDER = %d\n", &hdr_limits[i].bhp_hdr_order);
    fscanf(hdrfp,"TYPE = %d\n", &hdr_limits[i].bhp_hdr_type);
    fscanf(hdrfp,"DATATYPE = %s\n", &hdr_limits[i].bhp_hdr_data);
    fscanf(hdrfp,"VLEN = %d\n", &hdr_limits[i].bhp_hdr_vlen);
    fscanf(hdrfp,"VLOC = %d\n", &hdr_limits[i].bhp_hdr_vloc);
  }
  return stat;
}

int write_global_hdr_limits(char *path, int verbose)
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
    fprintf(lfp,"HDRNAME = %s\n", global_hdr_limits[i].bhp_hdr_name);
    fprintf(lfp,"MIN = %d\n", global_hdr_limits[i].bhp_hdr_min);
    fprintf(lfp,"MAX = %d\n", global_hdr_limits[i].bhp_hdr_max);
    fprintf(lfp,"INCR = %d\n", global_hdr_limits[i].bhp_hdr_inc);
    fprintf(lfp,"SCALAR = %d\n", global_hdr_limits[i].bhp_hdr_scalar);
    fprintf(lfp,"NUM = %d\n", global_hdr_limits[i].bhp_hdr_num);
    fprintf(lfp,"ORDER = %d\n", global_hdr_limits[i].bhp_hdr_order);
    fprintf(lfp,"TYPE = %d\n", global_hdr_limits[i].bhp_hdr_type);
    fprintf(lfp,"DATATYPE = %s\n", global_hdr_limits[i].bhp_hdr_data);
    fprintf(lfp,"VLEN = %d\n", global_hdr_limits[i].bhp_hdr_vlen);
    fprintf(lfp,"VLOC = %d\n", global_hdr_limits[i].bhp_hdr_vloc);
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
