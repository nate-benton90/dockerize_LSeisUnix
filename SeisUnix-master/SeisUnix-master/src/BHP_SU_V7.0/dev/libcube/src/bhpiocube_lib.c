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
* KEYWORDS:  $RCSfile: bhpiocube_lib.c,v $
*            $Revision: 1.13 $
*            $Date: 2003/04/28 20:01:54 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpiocube_lib.c,v $
* Revision 1.13  2003/04/28 20:01:54  ahmilb
* Remove byte-swapping from read_cube_page and write_cube_page.
*
* Revision 1.12  2003/04/22 17:25:01  ahmilb
* Fix overrun in read_file_hdr when reading long path/file names.
*
* Revision 1.11  2003/02/24 16:58:48  ahmilb
* Add shutdown code.
*
* Revision 1.10  2003/01/28 23:39:33  ahmilb
* Add read_cube_page, write_cube_page functions
*
* Revision 1.9  2003/01/10 18:24:51  ahmilb
* Move lock_cube to main.
*
* Revision 1.8  2002/08/06 20:36:55  ahmilb
* Move read/write file header to bhpio_lib.
*
* Revision 1.7  2002/05/08 14:35:52  ahmilb
* Remove stats from file_hdr, deleted write_stats fcn.
*
* Revision 1.6  2002/03/01 14:13:31  ahmilb
* fix sample interval, start for all data-type,data-order combos in summary
*
* Revision 1.5  2002/02/13 19:15:18  ahmilb
* add write_stats function for request=summary
*
* Revision 1.4  2002/01/29 16:09:48  ahmilb
* Move get_trace to library.
* Scan file header for data-type, data-order.
*
* Revision 1.3  2001/10/12 20:17:24  ahmilb
* Add write_file_hdr function.
* Use cube_in_memory flag to access cube.
* Change flock structure reference for Linux make.
*
* Revision 1.2  2001/09/13 20:38:37  ahmilb
* Add HORIZONS code.
* Move get_bhpio_path to bhpio_lib
*
* Revision 1.1  2001/07/25 18:44:08  ahmilb
* Separate version of libraries for seq and cube.
*
* Revision 1.4  2001/06/19 14:50:29  ahmilb
* Re-structure locking code.
* Re-struture hypercube access and updating.
*
* Revision 1.3  2001/05/29 17:10:28  ahmilb
* Add file-locking to allow multiple, concurrent jobs to update same data.
*
* Revision 1.2  2001/05/21 17:28:31  ahmilb
* Save CUBE_SIZE in file header.
*
* Revision 1.1  2001/04/18 19:35:01  ahmilb
* New Hypercube BHPIO
*
* Revision 1.3  2001/02/09 00:16:16  ahglim
* updated to Bob's ahmilb (beta) directory
*
* Revision 1.2  2001/02/06 02:38:20  ahglim
* corrected comment problem
*
*
*
******************************************************************/

#include <fcntl.h>
#include <sys/stat.h>
#include "bhpio.h"
#include "bhpiocube.h"
#include <sys/time.h>

int read_hdr_limits(char *path, int verbose)
{

  char field1[512], field2[512];

  int i, nitems;

  /* Open header limits file */
  if(verbose)
    fprintf(stderr,"Opening %s\n", path);
  hdrfp = fopen(path,"r");
  if(!hdrfp) {
    fprintf(stderr,"Could not open %s\n", path);
    return 1;
  }

  /* Initialize limits to illegal values */
  for(i=0; i<file_hdr.nkeys; i++) {
    strcpy(hdr_limits[i].bhp_hdr_name,"");
    hdr_limits[i].bhp_hdr_min = INT_MAX;
    hdr_limits[i].bhp_hdr_max = INT_MIN;
    hdr_limits[i].bhp_hdr_inc = INT_MIN;
    hdr_limits[i].bhp_hdr_scalar = 0;
  }
  /* Scan hdr_limits file */
  for(;;) {
    nitems = fscanf(hdrfp,"%s %*s %s", field1,field2);
    if(!nitems || nitems == EOF)
      break;
    if(strstr(field1,"HDRNAME") != NULL) {
      for(i=0; i<file_hdr.nkeys; i++) {
        if(!strcmp(hdr_limits[i].bhp_hdr_name,"")) {
          strcpy(hdr_limits[i].bhp_hdr_name,field2);
          break;
        }
      }
    }
    else if(strstr(field1,"MIN") != NULL) {
      for(i=0; i<file_hdr.nkeys; i++) {
        if(hdr_limits[i].bhp_hdr_min == INT_MAX) {
          sscanf(field2,"%d",&hdr_limits[i].bhp_hdr_min);
          break;
        }
      }
    }
    else if(strstr(field1,"MAX") != NULL) {
      for(i=0; i<file_hdr.nkeys; i++) {
        if(hdr_limits[i].bhp_hdr_max == INT_MIN) {
          sscanf(field2,"%d",&hdr_limits[i].bhp_hdr_max);
          break;
        }
      }
    }
    else if(strstr(field1,"INCR") != NULL) {
      for(i=0; i<file_hdr.nkeys; i++) {
        if(hdr_limits[i].bhp_hdr_inc == INT_MIN) {
          sscanf(field2,"%d",&hdr_limits[i].bhp_hdr_inc);
          break;
        }
      }
    }
    else if(strstr(field1,"SCALAR") != NULL) {
      for(i=0; i<file_hdr.nkeys; i++) {
        if(!hdr_limits[i].bhp_hdr_scalar) {
          sscanf(field2,"%d",&hdr_limits[i].bhp_hdr_scalar);
          break;
        }
      }
    }
  }
  /* Verify all legal values */
  for(i=0; i<file_hdr.nkeys; i++) {
    if(!strcmp(hdr_limits[i].bhp_hdr_name,""))
      err("Header %d in header-limits file has illegal or missing name\n", i+1);
    if(hdr_limits[i].bhp_hdr_min == INT_MAX)
      err("Header %d in header-limits file has illegal or missing minimum\n", i+1);
    if(hdr_limits[i].bhp_hdr_max == INT_MIN)
      err("Header %d in header-limits file has illegal or missing maximum\n", i+1);
    if(hdr_limits[i].bhp_hdr_inc == INT_MIN)
      err("Header %d in header-limits file has illegal or missing increment\n", i+1);
    if(!hdr_limits[i].bhp_hdr_scalar)
      err("Header %d in header-limits file has illegal or missing scalar\n", i+1);
  }
  if(verbose)
    for(i=0; i<file_hdr.nkeys; i++)
      fprintf(stderr,"NAME: %s, MIN: %d, MAX: %d, INC: %d, SCALAR: %d\n",
              hdr_limits[i].bhp_hdr_name,hdr_limits[i].bhp_hdr_min,hdr_limits[i].bhp_hdr_max,
              hdr_limits[i].bhp_hdr_inc,hdr_limits[i].bhp_hdr_scalar);

  fclose(hdrfp);
  return 0;
}

long cube_offset(int *vals, int *min, int *max, int *inc, int bin)
{

  long offset=0;

  int i, j, term;

  /* Build up offset terms */
  for(i=0; i<file_hdr.nkeys; i++) {
    if(i == file_hdr.nkeys - 1)
      term = (vals[i] - min[i]) / inc[i];
    else {
      term = 1;
      for(j=i+1; j<file_hdr.nkeys; j++)
        term *= (max[j] - min[j] + inc[j]) / inc[j];
      term *= ((vals[i] - min[i]) / inc[i]);
    }
    offset += term;
  }

  offset *= bin;

  return offset;
}

void read_cube_page(long seekto, int size, int endian_in, int verbose)
{

  lseek(cubefd,seekto,SEEK_SET);
  read(cubefd,cube,size*sizeof(int));

}
void write_cube_page(long seekto, int size, int endian_in, int verbose)
{

  lseek(cubefd,seekto,SEEK_SET);
  write(cubefd,cube,size*sizeof(int));

}
void read_cube(long offset, int *buff, int endian_in, int cube_in_memory, int verbose)
{

  int i;
 
  /* See if cube is in mem or on disk */
  if(cube_in_memory) {
    for(i=0; i<file_hdr.bin; i++)
      buff[i] = cube[offset+i];
  }
  else {
    lseek(cubefd,offset*sizeof(int),SEEK_SET);
    read(cubefd,buff,file_hdr.bin*sizeof(int));
  }

  /* byte-swap? */
  if(endian_in != file_hdr.endian)
    for(i=0; i<file_hdr.bin; i++)
      swap_int_4(&buff[i]);

}

void write_cube(long offset, int *buff, int endian_in, int cube_in_memory, int verbose)
{

  int i;

  /* byte-swap? */
  if(endian_in != file_hdr.endian)
    for(i=0; i<file_hdr.bin; i++)
      swap_int_4(&buff[i]);

  /* in memory? */
  if(cube_in_memory) {
    for(i=0; i<file_hdr.bin; i++)
      cube[offset+i] = buff[i];
  }
  /* update if cube on disk */
  else {
    lseek(cubefd,offset*sizeof(int),SEEK_SET);
    write(cubefd,buff,file_hdr.bin*sizeof(int));
  }
}

int get_trace(FILE **fp, int *vals, int *index, int *trace_addr, segy **traces, segy *tr,
              char *rule, int endian_in, int endian, int verbose)
{

  int dist;
  int i, j;
  int found=-1;
  int count=0;
  int fpnum;
  int partnum;

  segy fold;

  Value hval;

  /* Read all traces whose trace_addr is non-zero */
  for(i=0; i<file_hdr.bin; i++) {
    if(trace_addr[i]) {
      partnum = trace_addr[i] / MAX_TRACES;
      fpnum = -1;
      for(j=0; j<file_hdr.nparts; j++) {
        if(partnum == partition_map[j]) {
          fpnum = j;
          break;
        }
      }
      if(fpnum == -1)
        err("Could not find file for partnum=%d\n",partnum);
      efseek(fp[fpnum],(trace_addr[i]%MAX_TRACES)*(file_hdr.nsamp+60)*sizeof(float),SEEK_SET);
      efread(traces[count],FSIZE,file_hdr.nsamp+(HDRBYTES/4),fp[fpnum]);
      /* swap if necessary */
      check_endian(endian_in,endian,traces[count],(short)file_hdr.nsamp,verbose);
      count++;
    }
  }
  /* if bin is empty, return dead trace */
  if(count == 0) {
    count = 1;
    for(i=0; i<file_hdr.nsamp; i++)
      traces[0]->data[i] = 0;
    traces[0]->trid = 2;
    traces[0]->ns = file_hdr.nsamp;
    for(i=0; i<file_hdr.nkeys; i++) {
      hval.i = vals[i];
      puthval(traces[0],index[i],&hval);
    }
    return count;
  }
  /* rule=all */
  if(!strcmp(rule,"all"))
    return count;
  /* rule=match */
  if(!strcmp(rule,"match")) {
    for(i=0; i<count; i++) {
      found = i;
      for(j=0; j<file_hdr.nkeys; j++) {
        gethval(traces[i],index[j],&hval);
        if(hval.i != vals[j])
          found = -1;
      }
      if(found == i)
        break;
    }
    count = 0;
    if(found >= 0) {
      if(found > 0)
        memcpy((void *)traces[0],(const void *)traces[found],
               HDRBYTES+sizeof(float)*file_hdr.nsamp);
      count = 1;
    }
  }
  /* rule=near */
  else if(!strcmp(rule,"near")) {
    if(count) {
      found = 0;
      gethval(traces[0],index[file_hdr.nkeys-1],&hval);
      dist = ABS(vals[file_hdr.nkeys-1] - hval.i);
      for(i=1; i<count; i++) {
        gethval(traces[i],index[file_hdr.nkeys-1],&hval);
        if(ABS(vals[file_hdr.nkeys-1] - hval.i) < dist) {
          dist = ABS(vals[file_hdr.nkeys-1] - hval.i);
          found = i;
        }
      }
      if(found > 0)
        memcpy((void *)traces[0],(const void *)traces[found],
               HDRBYTES+sizeof(float)*file_hdr.nsamp);
      count = 1;
    }
  }
  /* rule=stack */
  else if(!strcmp(rule,"stack")) {
    if(count) {
      for(i=0; i<file_hdr.nsamp; i++)
        if(traces[0]->data[i])
          fold.data[i] = 1;
      for(i=1; i<file_hdr.bin; i++) {
        if(trace_addr[i]) {
          for(j=0; j<file_hdr.nsamp; j++) {
            traces[0]->data[j] += traces[i]->data[j];
            if(traces[i]->data[j])
              fold.data[j]++;
          }
        }
      }
      for(j=0; j<file_hdr.nsamp; j++)
        if(fold.data[j])
          traces[0]->data[j] /= fold.data[j];
      traces[0]->nhs = count;
      count = 1;
    }
  }
    
  return count;

}

void close_files(FILE **fp, int *file_status)
{

  int i;

  /* close open partitions */
  for(i=0; i<file_hdr.nparts; i++) {
    if(file_status[i] == 1) {
      fclose(fp[i]);
      fprintf(stderr,"Closed part %d\n",i+1);
    }
  }

} 

void shutdown(char *errmsg, char **names, int *vals, char *file, int count, FILE **fp,
              int *file_status, int cube_in_memory, char *cpath, int *cube,
              char *init, int lockfd, struct timeval *msec, int verbose, int debug)
{

  char fpath[NAMELEN];

  printmsg(errmsg,names,vals,file,count);
  close_files(fp,file_status);
  /* save cube if loaded */
  if(cube_in_memory)
    save_cube(cpath,cube,verbose);
  /* If init=yes, calc stats and create lock file */
  if(!strcmp(init,"yes")) {
    calc_stats(verbose,debug);
    get_bhpio_path(cpath,file_hdr.filename,"_LOCK.HDR",fpath,verbose);
    if(create_lock_file(fpath,lockfd,msec,verbose,debug))
      err("Failed to create lock file\n");
  }
  /* write file_hdr */
  get_bhpio_path(cpath,file_hdr.filename,"_0000.HDR",fpath,verbose);
  if(write_file_hdr(fpath,verbose))
    err("Failed to write file header\n");

}

void save_cube(char *path, int *cube, int verbose)
{

  char cpath[NAMELEN];

  int cubefd;

  get_bhpio_path(path,file_hdr.filename,"_0000.0002.HDR",cpath,verbose);
  cubefd = open(cpath,O_WRONLY | O_TRUNC | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if(cubefd == -1)
    err("Failed to open cube %s\n", cpath);
  write(cubefd,cube,file_hdr.cube_size*sizeof(int));
  close(cubefd);

}

int create_lock_file(char *fpath, int lockfd, struct timeval *msec, int verbose, int debug)
{

  lockfd = open(fpath,O_RDWR | O_TRUNC | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if(lockfd == -1)
    err("Failed to open lock file %s\n", fpath);
  close(lockfd);
  if(verbose)
    fprintf(stderr,"Created %s\n",fpath);
  if(debug) {
    gettimeofday(msec,NULL);
    fprintf(stderr,"%d created_lock_file at %lu\n",(int)getpid(),million*msec->tv_sec+msec->tv_usec);
  }

  return 0;

}

int get_part(char *file, int verbose)
{

  int len;
  int part;

  char temp[8];

  len = (int)strlen(file);
  strncpy(temp,&file[len-7],4);
  temp[4] = '\0';
  if(verbose > 0)
    fprintf(stderr,"file=%s, part=%d\n",file,atoi(temp));

  return part = atoi(temp);

}
