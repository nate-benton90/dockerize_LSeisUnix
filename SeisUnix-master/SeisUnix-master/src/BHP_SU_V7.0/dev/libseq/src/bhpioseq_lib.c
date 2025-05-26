
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
* KEYWORDS:  $RCSfile: bhpioseq_lib.c,v $
*            $Revision: 1.7 $
*            $Date: 2003/02/24 16:57:10 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpioseq_lib.c,v $
* Revision 1.7  2003/02/24 16:57:10  ahmilb
* Add shutdown function.
*
* Revision 1.6  2002/09/26 16:16:23  ahmilb
* Add new get_trace function to support interpolation.
*
* Revision 1.5  2002/08/06 20:41:02  ahmilb
* Move read/write file_header to bhpio_lib.
*
* Revision 1.4  2002/05/08 14:41:58  ahmilb
* Add units to file_hdr. Allow for optional items in file_hdr (backward compat).
*
* Revision 1.3  2001/10/12 20:18:47  ahmilb
* Add write_file_hdr function.
*
* Revision 1.2  2001/09/13 20:38:12  ahmilb
* Add HORIZONS code.
* Move get_bhpio_path to bhpio_lib
*
* Revision 1.1  2001/07/25 18:38:26  ahmilb
* Made separate libraries for seq and cube.
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

#include "bhpio.h"
#include "bhpioseq.h"

int read_hdr_limits(char *path, int index, char *key, int verbose)
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
    fscanf(hdrfp,"HDRNAME = %s\n", hdr_limits[index].bhp_hdr_name);
    fscanf(hdrfp,"MIN = %d\n", &hdr_limits[index].bhp_hdr_min);
    fscanf(hdrfp,"MAX = %d\n", &hdr_limits[index].bhp_hdr_max);
    fscanf(hdrfp,"INCR = %d\n", &hdr_limits[index].bhp_hdr_inc);
    fscanf(hdrfp,"SCALAR = %d\n", &hdr_limits[index].bhp_hdr_scalar);
    fscanf(hdrfp,"NUM = %d\n", &hdr_limits[index].bhp_hdr_num);
    fscanf(hdrfp,"ORDER = %d\n", &hdr_limits[index].bhp_hdr_order);
    fscanf(hdrfp,"TYPE = %d\n", &hdr_limits[index].bhp_hdr_type);
    fscanf(hdrfp,"DATATYPE = %s\n", hdr_limits[index].bhp_hdr_data);
    fscanf(hdrfp,"VLEN = %d\n", &hdr_limits[index].bhp_hdr_vlen);
    fscanf(hdrfp,"VLOC = %d\n", &hdr_limits[index].bhp_hdr_vloc);
    if(!strcmp(key,hdr_limits[index].bhp_hdr_name)) {
      if(verbose)
        fprintf(stderr,"Found %s in hdr-limits table \n",
                hdr_limits[index].bhp_hdr_name);
      stat = 1;
      break;
    }
  }
  return stat;
}

void get_trace(FILE **fp, segy *trace, int k1, int k2, int *v1, int vlen1, int *v2, int vlen2,
               int nsamp, int endian_in, int endian, int verbose)
{

  int i, j;
  int index;
  int index1, index2;
  int len1, len2;
  int addr;

  /* find k1 in v1 */
  index = 0;
  index1 = 0;
  for(;;) {
    if(v1[index] == k1) {
      index1 = index + 2;
      len1 = v1[index+1];
      break;
    }
    index += v1[index+1] + 2;
    if(index >= vlen1)
      break;
  }
  if(index1 == 0)
    err("Key-vector search failed\n");

  /* find k2 in v2 */
  index = 0;
  index2 = 0;
  for(;;) {
    if(v2[index] == k2) {
      index2 = index + 2;
      len2 = v2[index+1];
      break;
    }
    index += v2[index+1] + 2;
    if(index >= vlen2)
      break;
  }
  if(index2 == 0)
    err("Key-vector search failed\n");

  /* find matching trace address in v1, v2, starting at index1, index2 */
  index = 0;
  addr = -1;
  for(i=index1; i<index1+len1; i++) {
    for(j=index2; j<index2+len2; j++) {
      if(v2[j] == v1[i]) {
        addr = v2[j];
        break;
      }
    }
    if(addr != -1)
      break;
  }
  if(addr == -1)
    err("Matching address not found\n");

  efseek(fp[addr/MAX_TRACES],(addr%MAX_TRACES)*(nsamp+60)*sizeof(float),SEEK_SET);
  efread(trace,FSIZE,nsamp+(HDRBYTES/4),fp[addr/MAX_TRACES]);
  /* swap if necessary */
  check_endian(endian_in,endian,trace,(short)file_hdr.nsamp,verbose);

}

void shutdown(char *errmsg, char **names, int *vals, char *file, int count, FILE *fp,
              char *cpath, int verbose, int debug)
{

  char fpath[NAMELEN];

  printmsg(errmsg,names,vals,file,count);
  fclose(fp);
  /* calc stats */
  calc_stats(verbose,debug);
  /* write file_hdr */
  get_bhpio_path(cpath,file_hdr.filename,"_0000.HDR",fpath,verbose);
  if(write_file_hdr(fpath,verbose))
    err("Failed to write file header\n");

}
