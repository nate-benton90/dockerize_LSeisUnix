
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
* KEYWORDS:  $RCSfile: bhpio_lib.c,v $
*            $Revision: 1.21 $
*            $Date: 2003/12/09 22:17:52 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpio_lib.c,v $
* Revision 1.21  2003/12/09 22:17:52  ahmilb
* Key parsing - don't adjust upward if rule=match.
* Add get_pathlist_filename function.
*
* Revision 1.20  2003/05/14 16:41:03  ahmilb
* Set dt, delrt in subset_trace.
*
* Revision 1.19  2003/04/22 17:24:00  ahmilb
* Fix overrun in read_file_hdr when reading long path/file names.
*
* Revision 1.18  2003/04/01 22:05:56  ahmilb
* New properties allocation.
* Fix parser to handle vkey multiples without going past end of trace.
*
* Revision 1.17  2003/02/24 16:59:54  ahmilb
* Add shutdown code.
*
* Revision 1.15  2003/01/14 19:42:23  ahmilb
* Replace hard-coded lengths with NAMELEN, NFILES.
* Add isadir function.
*
* Revision 1.14  2002/11/08 15:17:59  ahmilb
* Separate stats for horizons.
*
* Revision 1.13  2002/10/03 13:53:27  ahmilb
* In trace_stats check for -999 and -999.25 as null values to avoid.
*
* Revision 1.12  2002/09/26 16:11:23  ahmilb
* Don't auto-adjust user specified keys when interpolating.
*
* Revision 1.11  2002/08/06 20:34:54  ahmilb
* Common file header.
* VKEY info as floats.
* In parse_key do auto-adjust of increment only if not interpolating.
* Add layernum to make property code simpler.
* Re-code set_stats.
* Add parse_sub_stringf for float keys.
*
* Revision 1.10  2002/05/08 14:32:46  ahmilb
* Fixed build_trace code for properties and horizons.
* Added code to build_trace to save maxdepth, mindepth if property.
* Added trace_stats, set_stats, and bhp_gettr fcns.
*
* Revision 1.9  2002/03/01 14:12:41  ahmilb
* fix key increment adjust if negative
*
* Revision 1.8  2002/02/28 14:54:41  ahmilb
* fix keylist adjustment for specified increment
*
* Revision 1.7  2002/02/27 15:45:02  ahmilb
* fix bug in keylist alignment calculation
*
* Revision 1.6  2002/02/25 18:20:51  ahmilb
* make sure key list min,max,inc are key incr multiples
*
* Revision 1.5  2002/01/29 16:08:25  ahmilb
* Add property:n syntax.
* Add subset_trace function to handle vkey access.
*
* Revision 1.4  2001/11/14 23:06:41  ahmilb
* Fix to keep wildcards in keylist from exceeding data limits.
*
* Revision 1.3  2001/10/30 18:44:40  ahmilb
* Re-structure byte-swapping code.
* Replace fputtr_endian with write_trace.
* Add check_stdout function.
* Move build_trace to lib.
*
* Revision 1.2  2001/09/14 18:34:15  ahmilb
* Change calling sequence for check_endian.
*
* Revision 1.1  2001/09/13 19:50:37  ahmilb
* Reduce source code by several hundred lines by implementing the following functions used by readseq/readcube:
*   get_bhpio_path, get_path1, get_keylist_subset, parse_sub_string, parse_key, parse_keylist,
*   get_endian, get_prop_horz, check_timeslice, save_timeslice, make_tfiles, close_tfiles,
*   fputtr_endian, check_endian.
*
******************************************************************/

#include <sys/stat.h>
#include <libgen.h>
#include "bhpio.h"

int get_bhpio_path(char *path, char *name, char *ext, char *fpath, int verbose)
{

  char command[256];

  /* See if file exists */
  strcpy(fpath,path);
  strcat(fpath,"/");
  strcat(fpath,name);
  strcat(fpath,ext);
  strcpy(command,"test -f ");
  strcat(command,fpath);
  if(verbose)
    fprintf(stderr,"Executing %s\n", command);
  if(!system(command))
    return 1;
  else
    return 0;

}

char *get_path1(char *filename, int verbose)
{

  char *pathlist;
  char *path1;

  FILE *fp1;

  /* Get pathlist */
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc(strlen(filename)+5,sizeof(char));
    strcpy(pathlist,filename);
    strcat(pathlist,".dat");
  }
  if(verbose)
    fprintf(stderr,"Opening %s\n", pathlist);
  if(!(fp1 = fopen(pathlist,"r")))
    err("pathlist specified, but cannot open %s\n", pathlist);
  else {
    /* Only need first path to get to partition zero */
    path1 = calloc(NAMELEN,sizeof(char));
    fscanf(fp1,"%s\n", path1);
    fclose(fp1);
  }
  return path1;

}

int read_file_hdr(char *path, int verbose)
{

  char field1[512], field2[512];

  int i, j, k, nitems;
  int nparts=0,nprop=-1;

  /* Get file header */
  if(verbose)
    fprintf(stderr,"Opening %s\n", path);
  hdrfp = fopen(path,"r");
  if(!hdrfp) {
    fprintf(stderr,"Could not open %s\n", path);
    return 1;
  }

  /* Initialize file_hdr items to illegal values */
  file_hdr.npaths = 0;
  file_hdr.size = 0;
  file_hdr.nparts = 0;
  file_hdr.nsamp = 0;
  file_hdr.traces_per_part = 0;
  file_hdr.nkeys = 0;
  file_hdr.endian = -1;
  file_hdr.nprop = INT_MIN;
  if(filesys == NULL && verbose)
    fprintf(stderr,"First calloc for filesys\n");
  else if(filesys != NULL) {
    if(verbose)
      fprintf(stderr,"Free filesys\n");
    free(filesys);
  }
  filesys = calloc(NFILES*NAMELEN,sizeof(char));
  if(verbose)
    fprintf(stderr,"Allocated %d characters for filesys\n",NFILES*NAMELEN);
  /* set optional items to default values */
  file_hdr.units = -1;
  file_hdr.minval = FLT_MAX;
  file_hdr.maxval = -FLT_MAX;
  file_hdr.meanval = 0;
  file_hdr.rmsval = 0;
  file_hdr.data_type = 0;
  file_hdr.data_order = 0;
  strcpy(file_hdr.vkey,"");

  /* Scan file_hdr file */
  for(;;) {
    nitems = fscanf(hdrfp,"%s %*s %s", field1,field2);
    if(!nitems || nitems == EOF)
      break;
    /* get file_hdr members */
    if(!strcmp(field1,"NPATHS"))
      sscanf(field2,"%d",&file_hdr.npaths);
    else if(!strcmp(field1,"FILENAME"))
      strcpy(file_hdr.filename,field2);
    else if(!strcmp(field1,"PARTSIZE"))
      sscanf(field2,"%d",&file_hdr.size);
    else if(!strcmp(field1,"NPARTS"))
      sscanf(field2,"%d",&file_hdr.nparts);
    else if(!strcmp(field1,"NSAMP"))
      sscanf(field2,"%d",&file_hdr.nsamp);
    else if(!strcmp(field1,"TRACESPERPART"))
      sscanf(field2,"%d",&file_hdr.traces_per_part);
    else if(!strcmp(field1,"NKEYS"))
      sscanf(field2,"%d",&file_hdr.nkeys);
    else if(!strcmp(field1,"ENDIAN"))
      sscanf(field2,"%d",&file_hdr.endian);
    else if(!strcmp(field1,"NPROP")) {
      sscanf(field2,"%d",&file_hdr.nprop);
      /* allocate stats if model or horizon data */
      if(file_hdr.nprop != 0) {
        j = (file_hdr.nprop > 0) ? file_hdr.nprop : -file_hdr.nprop;
        properties = calloc(j,sizeof(char *));
        for(k=0; k<j; k++)
          properties[k] = calloc(NAMELEN,sizeof(char));
        file_hdr.prop_minval = calloc(j,sizeof(float));
        file_hdr.prop_maxval = calloc(j,sizeof(float));
        file_hdr.prop_meanval = calloc(j,sizeof(float));
        file_hdr.prop_rmsval = calloc(j,sizeof(float));
        for(i=0; i<j; i++) {
          file_hdr.prop_minval[i] = FLT_MAX;
          file_hdr.prop_maxval[i] = -FLT_MAX;
          file_hdr.prop_meanval[i] = 0;
          file_hdr.prop_rmsval[i] = 0;
        }
      }
    }
    else if(!strcmp(field1,"UNITS"))
      sscanf(field2,"%d",&file_hdr.units);
    else if(!strcmp(field1,"BIN"))
      sscanf(field2,"%d",&file_hdr.bin);
    else if(!strcmp(field1,"CUBE_SIZE"))
      sscanf(field2,"%d",&file_hdr.cube_size);
    /* single-value stats */
    else if(!strcmp(field1,"MIN"))
      sscanf(field2,"%f",&file_hdr.minval);
    else if(!strcmp(field1,"MAX"))
      sscanf(field2,"%f",&file_hdr.maxval);
    else if(!strcmp(field1,"MEAN"))
      sscanf(field2,"%f",&file_hdr.meanval);
    else if(!strcmp(field1,"RMS"))
      sscanf(field2,"%f",&file_hdr.rmsval);
    else if(!strcmp(field1,"TYPE"))
      sscanf(field2,"%d",&file_hdr.data_type);
    else if(!strcmp(field1,"ORDER"))
      sscanf(field2,"%d",&file_hdr.data_order);
    else if(!strcmp(field1,"VKEY"))
      sscanf(field2,"%s",file_hdr.vkey);
    else if(!strcmp(field1,"VMIN"))
      sscanf(field2,"%f",&file_hdr.vmin);
    else if(!strcmp(field1,"VMAX"))
      sscanf(field2,"%f",&file_hdr.vmax);
    else if(!strcmp(field1,"VINC"))
      sscanf(field2,"%f",&file_hdr.vinc);
    else if(!strcmp(field1,"PARTITION")) {
      sscanf(field2,"%s",&filesys[nparts*NAMELEN]);
      nparts++;
    }
    else if(!strcmp(field1,"PROPERTY") || !strcmp(field1,"HORIZON")) {
      nprop++;
      sscanf(field2,"%s",properties[nprop]);
    }
    else if(!strcmp(field1,"PROP_MIN"))
      sscanf(field2,"%f",&file_hdr.prop_minval[nprop]);
    else if(!strcmp(field1,"PROP_MAX"))
      sscanf(field2,"%f",&file_hdr.prop_maxval[nprop]);
    else if(!strcmp(field1,"PROP_MEAN"))
      sscanf(field2,"%f",&file_hdr.prop_meanval[nprop]);
    else if(!strcmp(field1,"PROP_RMS"))
      sscanf(field2,"%f",&file_hdr.prop_rmsval[nprop]);

  }
  fclose(hdrfp);
  return 0;
}
int write_file_hdr(char *path, int verbose)
{

  int i, j;

  if(verbose)
    fprintf(stderr,"Opening %s\n", path);
  hdrfp = fopen(path,"w");
  if(!hdrfp) {
    fprintf(stderr,"Could not open %s\n", path);
    return 1;
  }

  /* Write file header structure */
  fprintf(hdrfp,"NPATHS = %d\n", file_hdr.npaths);
  fprintf(hdrfp,"FILENAME = %s\n", file_hdr.filename);
  fprintf(hdrfp,"PARTSIZE = %d\n", file_hdr.size);
  fprintf(hdrfp,"NPARTS = %d\n", file_hdr.nparts);
  fprintf(hdrfp,"NSAMP = %d\n", file_hdr.nsamp);
  fprintf(hdrfp,"TRACESPERPART = %d\n", file_hdr.traces_per_part);
  fprintf(hdrfp,"NKEYS = %d\n", file_hdr.nkeys);
  fprintf(hdrfp,"ENDIAN = %d\n", file_hdr.endian);
  fprintf(hdrfp,"NPROP = %d\n", file_hdr.nprop);
  fprintf(hdrfp,"UNITS = %d\n", file_hdr.units);
  fprintf(hdrfp,"BIN = %d\n", file_hdr.bin);
  fprintf(hdrfp,"CUBE_SIZE = %d\n", file_hdr.cube_size);
  /* if data is seismic, only one set of stats */
  if(file_hdr.nprop == 0) {
    fprintf(hdrfp,"MIN = %f\n", file_hdr.minval);
    fprintf(hdrfp,"MAX = %f\n", file_hdr.maxval);
    fprintf(hdrfp,"MEAN = %f\n", file_hdr.meanval);
    fprintf(hdrfp,"RMS = %f\n", file_hdr.rmsval);
  }
  fprintf(hdrfp,"TYPE = %d\n", file_hdr.data_type);
  fprintf(hdrfp,"ORDER = %d\n", file_hdr.data_order);
  fprintf(hdrfp,"VKEY = %s\n", file_hdr.vkey);
  fprintf(hdrfp,"VMIN = %.3f\n", file_hdr.vmin);
  fprintf(hdrfp,"VMAX = %.3f\n", file_hdr.vmax);
  fprintf(hdrfp,"VINC = %.3f\n", file_hdr.vinc);
  for(i=0; i<file_hdr.nparts; i++)
    fprintf(hdrfp,"PARTITION = %s\n", &filesys[i*NAMELEN]);
  if(file_hdr.nprop != 0) {
    j = (file_hdr.nprop > 0) ? file_hdr.nprop : -file_hdr.nprop;
    for(i=0; i<j; i++) {
      if(file_hdr.nprop > 0)
        fprintf(hdrfp,"PROPERTY = %s\n", properties[i]);
      else
        fprintf(hdrfp,"HORIZON = %s\n", properties[i]);
      fprintf(hdrfp,"PROP_MIN = %f\n", file_hdr.prop_minval[i]);
      fprintf(hdrfp,"PROP_MAX = %f\n", file_hdr.prop_maxval[i]);
      fprintf(hdrfp,"PROP_MEAN = %f\n", file_hdr.prop_meanval[i]);
      fprintf(hdrfp,"PROP_RMS = %f\n", file_hdr.prop_rmsval[i]);
    }
  }

  fclose(hdrfp);

  return 0;

}
void get_keylist_subset(char *string, char *sub_string, int *loc)

{

  char *ptr1;               /* address of current sub-string */
  char *ptr2;               /* address of current sub-string */

  ptr1 = string;
  ptr2 = strstr(ptr1,":");

  if(ptr2 == NULL) {
    strcpy(sub_string,ptr1);
    *loc += strlen(sub_string) + 1;
  }
  else {
    strncpy(sub_string,ptr1,ptr2-ptr1);
    sub_string[ptr2-ptr1] = '\0';
    *loc += ptr2 - ptr1 + 1;
  }

}

void parse_sub_string(char *sub_string, int *n1, int *n2, int *n3, int verbose)
{

  char *p1, *p2, *p3, *p4, *p5, *p6;   /* p1->1st int, p2->hyphen, p3->2nd int, */
                                       /* p4->left paren, p5-> paren contents, p6->right paren */

  char string[32];

  /* Set pointers */
  p1 = sub_string;
  p2 = strstr(sub_string,"-");
  /* If p1 is negative */
  if(p2 == p1)
    p2 = strstr(p2+1,"-");
  if(p2)
    p3 = p2 + 1;
  else
    p3 = NULL;
  p4 = strstr(sub_string,"[");
  if(p4)
    p5 = p4 + 1;
  else
    p5 = NULL;
  p6 = strstr(sub_string,"]");

  /*fprintf(stderr,"Pointers\n");
  if(p1)
    fprintf(stderr,"p1 %s\n", p1);
  if(p2)
    fprintf(stderr,"p2 %s\n", p2);
  if(p3)
    fprintf(stderr,"p3 %s\n", p3);
  if(p4)
    fprintf(stderr,"p4 %s\n", p4);
  if(p5)
    fprintf(stderr,"p5 %s\n", p5);
  if(p6)
    fprintf(stderr,"p6 %s\n", p6);*/

  /* If p1 length is zero, empty sub-string */
  if(!strlen(sub_string))
    err("Empty sub-string\n");

  /* If p2, p4, p6 NULL => n1 */
  if(p2 == NULL && p4 == NULL && p6 == NULL && p1 != NULL) {
    *n1 = atoi(sub_string);
    /*if(!*n1)
      err("Sub-string %s contains zero or non-integer\n", sub_string);*/
  }

  /* If hyphen, no (), and there is something before and after hyphen => n1-n2 */
  else if(p2 != NULL && p4 == NULL && p6 == NULL && p2 > p1 && strlen(p3)) {
    *n1 = atoi(p1);
    *n2 = atoi(p3);
    /* Make sure n1 and n2 are int */
    /*if(!*n1 || !*n2)
      err("Sub-string %s contains zero or non-integer\n", p1);*/
  }

  /* Check for illegal combination of int and hyphen */
  else if((p2 != NULL && p2 == p1) || (p3 != NULL && strlen(p3) == 0)) {
    fprintf(stderr,"p1,p2,p3 %s %s %s\n", p1,p2,p3);
    err("Illegal combination of integer(s) and hyphen, or missing integers\n");
  }

  /* Therefore, must be => n-m(k) */
  /* If left paren, there must be right paren */
  else if(p4 != NULL && p6 == NULL)
    err("Sub-string has a left paren without a matching right paren\n");

  /* If right paren, there must be left paren */
  else if(p6 != NULL && p4 == NULL)
    err("Sub-string has a right paren without a matching left paren\n");

  /* If parens, they must be left, then right and there must be contents */
  else if(p6 < p4)
    err("Parentheses are backwards\n");

  /* Check contents */
  else if(p6 - p4 == 1)
    err("Empty parentheses\n");

  /* Get contents of () */
  else {
    strncpy(string,p5,p6-p4-1);
    string[p6-p4-1] = '\0';
    /* Contents must be integer */
    *n3 = atoi(string);
    if(!*n3)
      err("Parentheses contain zero or non-integer\n");

    /* Verify preceeding hyphen with something before -, and something between - and ( */
    if(!p2)
      err("Range, including hyphen, must preceed parentheses\n");

    /* Get range */
    if(p2 != NULL && p2 < p4 && p2 > p1 && p4 > p3) {
      *n1 = atoi(p1);
      *n2 = atoi(p3);
      /* Make sure n1 and n2 are int */
      if(*n1 && *n2) {
        if(verbose)
          fprintf(stderr,"n1-n2(n3) %d-%d(%d)\n", *n1,*n2,*n3);
      }
      /*else
        err("Sub-string contains zero or non-integer\n");*/
    }

    /* Hyphen out of place */
    else if(p2 > p4)
      err("Hyphen must preceed ()\n");
    else if(p2 == p1)
      err("Integer must preceed hyphen\n");
    else if(p4 <= p3)
      err("Integer must preceed (\n");
  }

}

void parse_key(char *string, int *keylist, int *nlist, int min, int max, int inc,
               int interp_type, int verbose)
{

/*
  A parameter string consists of 1 or more sub-strings, separated by commas
  A sub-string can contain up to 3 integers, one hyphen, and one pair of parentheses
  Valid sub-string Formats

  n            Item containing value n
  n-m          Items containing values n thru m, inclusive
  n-m(k)       Items containing every kth value, from item containing n thru item containing m

*/

  char sub_string[32];      /* current sub-string */
  char *ptr1;               /* address of current sub-string */
  char *ptr2;               /* address of next comma */

  /* n1-n2(n3) */
  int n1;                   /* First number in sub-string */
  int n2;                   /* Second number in sub-string */
  int n3;                   /* Third number in sub-string */
  int total=0;              /* Number of key values in expanded key string */
  int i, j, n;

  ptr1 = string;
  /* Get a sub-string */
  for(;;) {
    ptr2 = strstr(ptr1,",");
    if(ptr2 == NULL)
      strcpy(sub_string,ptr1);
    else {
      strncpy(sub_string,ptr1,ptr2-ptr1);
      sub_string[ptr2-ptr1] = '\0';
      ptr1 = ptr2 + 1;
    }
    n1 = n2 = n3 = 0;
    parse_sub_string(sub_string,&n1,&n2,&n3,verbose);
    /* Test for n1, n2 out of range or not multiple of inc */
    if(n1) {
      if(n1 > max)
        n1 = max;
      if(n1 < min)
        n1 = min;
      /* adjust inc if not interpolating */
      if(interp_type == 0)
        n1 = (n1 + inc - 1 - min) / inc * inc + min;
    }
    if(n2) {
      if(n2 > max)
        n2 = max;
      if(n2 < min)
        n2 = min;
      /* adjust inc if not interpolating */
      if(interp_type == 0)
        n2 = (n2 + inc - 1 - min) / inc * inc + min;
    }
    if(n3) {
      /* Check that sign of n3 is compatible with direction of range */
      if(n2 > n1 && n3 <= 0)
        err("Increment must be positive if range is ascending\n");
      if(n1 > n2 && n3 >= 0)
        err("Increment must be negative if range is descending\n");
      /* adjust inc if not interpolating */
      if(interp_type == 0) {
        if(n3 > 0)
          n3 = (n3 + inc - 1) / inc * inc;
        else
          n3 = (n3 - inc + 1) / inc * inc;
      }
      n = (n2 - n1 + n3 ) / n3;
      for(i=total,j=0; i<total+n; i++,j++)
        keylist[i] = n1 + j * n3;
      total += n;
    }
    else if(n2) {
      if(n2 > n1) {
        n = (n2 - n1 + inc) / inc;
        for(i=total,j=0; i<total+n; i++,j++)
          keylist[i] = n1 + j * inc;
      }
      else {
        n = (n2 - n1 - inc) / -(inc);
        for(i=total,j=0; i<total+n; i++,j++)
          keylist[i] = n1 - j * inc;
      }
      total += n;
    }
    else {
      keylist[total] = n1;
      total += 1;
    }
    if(ptr2 == NULL)
      break;
  }
  *nlist = total;

}

int *parse_keylist(char *keylist_subset, int min, int max, int inc, int *nklist,
                   int interp_type, int verbose)
{

  int i;
  int *temp;
  int *klist;

  char string[16];

  /* If * is present, set to read all */
  if(strstr(keylist_subset,"*") != NULL) {
    sprintf(keylist_subset,"%d",min);
    sprintf(string,"-");
    strcat(keylist_subset,string);
    sprintf(string,"%d",max);
    strcat(keylist_subset,string);
  }
  temp = calloc(2*(max - min),sizeof(int));
  parse_key(keylist_subset,temp,nklist,min,max,inc,interp_type,verbose);
  klist = calloc(*nklist,sizeof(int));
  for(i=0; i<*nklist; i++)
    klist[i] = temp[i];

  free(temp);

  return klist;

}

int get_endian(int my_endian, int verbose)
{

  int endian;

  if(!getparint("endian",&endian)) {
    endian = my_endian;        /* native */
    if(verbose)
      fprintf(stderr,"Writing native format\n");
  }
  else if(endian == 0 || endian == 1) {
    if(verbose) {
      if(endian)
        fprintf(stderr,"Force BIG_ENDIAN format\n");
      else
        fprintf(stderr,"Force LITTLE_ENDIAN format\n");
    }
  }
  else {
    fprintf(stderr,"endian = %d\n", endian);
    err("If endian is specified, it must be 0 or 1\n");
  }

  return endian;

}

int *get_prop_horz(char **properties, int *nprop,  int *iz, int *nlayers,
                   int file_nprop, int nsamp, int *layernum,
                   int *layer_boundary, int verbose)
{

  char *p2;

  cwp_String prop[256];

  int *iprop;
  int i, j, k;

  /* check properties and horizons */
  if(getparstringarray("properties",prop) && getparstringarray("horizons",prop))
    err("Can't have both properties and horizons\n");

  /* Properties */
  *nprop = countparval("properties");
  if(*nprop > 0) {
    getparstringarray("properties",prop);
    /* Special cases: properties=name:n and properties=layer_boundary:n */
    if(*nprop == 1) {
      p2 = strstr(prop[0],":");
      if(p2 != NULL) {
        i = p2 - prop[0];
        prop[0][i] = '\0';
        p2++;
        *layernum = atoi(p2);
      }
    }
    if(verbose) {
      fprintf(stderr,"Requested PROPERTIES: %s ",prop[0]);
      if(*layernum != INT_MIN)
         fprintf(stderr," Layer Number: %d",*layernum);
      for(i=1; i<*nprop; i++)
        fprintf(stderr," %s ",prop[i]);
      fprintf(stderr,"\n");
    }
    /* Check requested vs available */
    for(i=0; i<*nprop; i++) {
      k = 0;
      for(j=0; j<file_nprop; j++) {
        if(!strcmp(prop[i],properties[j]) || !strcmp(prop[i],"layer_boundary")) {
          k = 1;
          break;
        }
      }
      if(!k)
        err("Property %s is not available\n", prop[i]);
    }
    /* Set indices of properties */
    iprop = calloc(*nprop,sizeof(int));
    /* Calc number of layers if not alread set */
    if(*nlayers == INT_MIN)
      *nlayers = (nsamp - 1) / (file_nprop + 1);
    /* Start of depths */
    *iz = *nlayers * file_nprop;
    /* Start of depths */
    for(i=0; i<*nprop; i++) {
      for(j=0; j<file_nprop; j++) {
        if(!strcmp(prop[i],properties[j])) {
          iprop[i] = j * (*nlayers);
          break;
        }
        if(!strcmp(prop[i],"layer_boundary")) {
          iprop[i] = *iz;
          *layer_boundary = 1;
          break;
        }
      }
    }
    if(verbose) {
      for(i=0; i<*nprop; i++)
        fprintf(stderr,"%s index %d\n", prop[i],iprop[i]);
      fprintf(stderr,"Depth Index: %d\n",*iz);
    } 
  }
  /* horizons */
  else if((*nprop = countparval("horizons")) > 0) {
    getparstringarray("horizons",prop);
    if(verbose) {
      fprintf(stderr,"Requested HORIZONS: %s ",prop[0]);
      for(i=1; i<*nprop; i++)
        fprintf(stderr," %s ",prop[i]);
      fprintf(stderr,"\n");
    }
    /* Check requested vs available */
    for(i=0; i<*nprop; i++) {
      k = 0;
      for(j=0; j<-file_nprop; j++) {
        if(!strcmp(prop[i],properties[j])) {
          k = 1;
          break;
        }
      }
      if(!k)
        err("Horizon %s is not available\n", prop[i]);
    }
    /* Set indices of horizons */
    iprop = calloc(*nprop,sizeof(int));
    for(i=0; i<*nprop; i++) {
      for(j=0; j<-file_nprop; j++) {
        if(!strcmp(prop[i],properties[j])) {
          iprop[i] = j + 1;
          break;
        }
      }
    }
    if(verbose)
      for(i=0; i<*nprop; i++)
        fprintf(stderr,"%s index %d\n", prop[i],iprop[i]);
  }

  /* if layernum is set, verify it is in range */
  if(*layernum != INT_MIN && (*layernum < 1 || *layernum > *nlayers + 1))
    err("Layer Number %d is out of range\n",*layernum);
 
  return iprop;

}

void check_timeslice(timeslice_def *timeslice, int nkeys, int verbose)
{

  char *output;
  char command[256];

  int i, j;
  int tparms[3]={0,0,0};

  /* Check timeslice parm */
  i = countparval("timeslice");
  if(i > 3) {
    fprintf(stderr,"Too many timeslice parameters\n");
    fprintf(stderr,"Specify timeslice as timeslice=start,end,incr\n");
    fprintf(stderr,"  , where times are in milliseconds, and end and incr are optional\n");
    err("Too many timeslice parameters\n");
  }
  if(i) {
    getparint("timeslice",tparms);
    /* Timeslice syntax check */
    /* Set flag to no errors */
    j = 0;
    if(i == 3 && tparms[2] <= 0) {
      fprintf(stderr,"Timeslice increment must be > 0\n");
      j++;
    }
    if(i >= 2 && tparms[1] <= tparms[0]) {
      fprintf(stderr,"Timeslice end must be > start\n");
      j++;
    }
    if(tparms[0] < 0 || (i >= 2 && tparms[1] < 0)) {
      fprintf(stderr,"Timeslice start and end must be >= zero\n");
      j++;
    }
    if(i == 2)
      tparms[2] = 1000;
    if(j) {
      fprintf(stderr,"timeslice=%d", tparms[0]);
      if(i >= 2)
        fprintf(stderr,",%d", tparms[1]);
      if(i == 3)
        fprintf(stderr,",%d", tparms[2]);
      fprintf(stderr,"\n");
      err("Timeslice syntax error\n");
    }
    /* Build times list, where each entry has a time at which to get a slice */
    if(i >= 2)
      timeslice->ntimes = (tparms[1] - tparms[0] + tparms[2]) / tparms[2];
    else
      timeslice->ntimes = 1;
    timeslice->times = calloc(timeslice->ntimes,sizeof(int));
    for(j=0; j<timeslice->ntimes; j++)
      timeslice->times[j] = tparms[0] + tparms[2] * j;
    if(verbose) {
      fprintf(stderr,"Saving timeslices at following times (ms)\n");
      for(j=0; j<timeslice->ntimes; j++)
        fprintf(stderr,"  %d", timeslice->times[j]);
      fprintf(stderr,"\n");
    }
    /* If not at least 2 keys, quit */
    if(nkeys < 2)
      err("At least 2 trace header keys must be specified to save timeslices\n");
  }

  /* Check output, display */
  if(timeslice->ntimes) {
    if(!(getparstring("output",&output))) {
      /* Make directory, if not already there */
      strcpy(command,"test -d timeslice");
      if(verbose)
        fprintf(stderr,"Executing %s\n", command);
      if((i = system(command))) {
        strcpy(command,"mkdir timeslice");
        if(verbose)
          fprintf(stderr,"Executing %s\n", command);
        i = system(command);
      }
      timeslice->opath = calloc(NAMELEN,sizeof(char));
      strcpy(timeslice->opath,"timeslice");
    }
    else
      strcpy(timeslice->opath,output);
    if(verbose)
      fprintf(stderr,"Writing timeslices to %s\n", timeslice->opath);
    if(!(getparstring("display",&timeslice->display)))
      timeslice->display = "yes";
  }

}

void save_timeslice(segy **traces, int nt, int ntimes, int *times, FILE **ofp)
{

  int i, j;

  segy tr;

  /* Save header from first trace  and set ns=nt */
  memcpy((void *)&tr,(const void *)traces[0],HDRBYTES);
  tr.ns = nt;

  for(i=0; i<ntimes; i++) {
    /* Pick off samples from each trace at current time */
    for(j=0; j<nt; j++)
      tr.data[j] = traces[j]->data[times[i]/(tr.dt/1000)];
    efwrite(&tr,HDRBYTES,1,ofp[i]);
    efwrite(tr.data,FSIZE,nt,ofp[i]);
  }
}

FILE **make_tfiles(int ntimes, int *times, char *opath, int verbose)
{

  FILE **ofp;

  char ofile[256];
  char cindex[8];

  int i;

  ofp = calloc(ntimes,sizeof(FILE *));
  for(i=0; i<ntimes; i++) {
    strcpy(ofile,opath);
    strcat(ofile,"/");
    sprintf(cindex,"%i",times[i]);
    strcat(ofile,cindex);
    strcat(ofile,"MS.su");
    if(verbose)
      fprintf(stderr,"Opening %s\n", ofile);
    ofp[i] = fopen(ofile,"w");
    if(!ofp[i])
      err("Could not open %s\n", ofile);
  }

  return ofp;
}

void close_tfiles(timeslice_def *timeslice, FILE **ofp, int verbose)
{

  int i, j;

  char command[256];
  char ofile[256];
  char cindex[8];

  /* Close timeslices */
  for(i=0; i<timeslice->ntimes; i++)
    efclose(ofp[i]);
  /* start suxiamge, if requested */
  if(!(strcmp(timeslice->display,"yes"))) {
    for(i=0; i<timeslice->ntimes; i++) {
      strcpy(command,"suximage title=");
      strcpy(ofile,timeslice->opath);
      strcat(ofile,"/");
      sprintf(cindex,"%i",timeslice->times[i]);
      strcat(ofile,cindex);
      strcat(ofile,"MS.su");
      strcat(command,ofile);
      strcat(command," < ");
      strcat(command,ofile);
      strcat(command," &");
      if(verbose)
        fprintf(stderr,"Executing %s\n", command);
      j = system(command);
    }
  }
  free(timeslice->times);

}

void check_endian(int endian_in, int endian, segy *tp, short ns, int verbose)
{
/* endian_in = input data
   endian = output data
*/

  int i;
  
  /* if output is different from input, swap */
  if(endian != endian_in) {
    for(i=0; i<SU_NKEYS; i++)
      swaphval(tp,i);
    for(i=0; i<ns; i++)
      swap_float_4(&tp->data[i]);
  }

}

void check_stdout(void)
{

  FileType ftype;

  ftype = filestat(fileno(stdout));
  if(ftype == DIRECTORY)
    err("Output cannot be a directory\n");
  else if(ftype == TTY)
    err("Output cannot be terminal\n");

}

void write_trace(segy *tr, short ns)
{

  size_t n;

  if((n = fwrite(tr,sizeof(char),HDRBYTES,stdout)) != HDRBYTES)
    err("Error writing trace header\n");
  if((n = fwrite(&tr->data,sizeof(float),ns,stdout)) != ns)
    err("Error writing trace\n");

}

void build_trace(segy *trace, int file_nprop, int nprop, int *iprop, int iz,
                 int nl, int layer_boundary, float *maxd, float *mind,
                 int nvklist, float *vklist, data_stats *stats, int verbose)
{

  segy work;     /* Scratch trace */

  int i, j, k;
  int m;

  /* Copy input */
  for(i=0; i<trace->ns; i++) {
    work.data[i] = trace->data[i];
    trace->data[i] = 0;
  }

  /* If properties, copy properties and depths */
  if(file_nprop > 0) {
    for(i=0; i<nprop; i++) {
      /* if nvklist > 0, get vertical subset */
      if(nvklist > 0)
        for(k=i*nvklist,m=0; k<i*nvklist+nvklist; k++,m++)
          trace->data[k] = work.data[iprop[i]+(int)vklist[m]];
      else {
        for(j=iprop[i],k=i*nl; j<iprop[i]+nl; j++,k++)
          trace->data[k] = work.data[j];
      }
    }
    /* layer_boundary special case */
    if(layer_boundary == 1) {
      trace->ns = 1;
      j = 0;
      k = 1;
    }
    /* Copy tops, bottom */
    else if(nvklist > 0) {
      for(k=nprop*nvklist,m=0; m<nvklist; k++,m++)
        trace->data[k] = work.data[iz+(int)vklist[m]];
      trace->data[nprop*nvklist+nvklist] = work.data[iz+(int)vklist[nvklist-1]+1];
      trace->ns = nvklist * nprop + nvklist + 1;
      j = nprop * nvklist;
      k = j + nvklist + 1;
    }
    else {
      for(j=iz,k=nprop*nl; j<iz+nl+1; j++,k++)
        trace->data[k] = work.data[j];
      trace->ns = nl * nprop + nl + 1;
      j = nprop * nl;
      k = j + nl + 1;
    }
  }
  /* if horizons, copy requested horizons */
  else if(file_nprop < 0) {
    for(i=0; i<nprop; i++)
      trace->data[i] = work.data[iprop[i] - 1];
    trace->ns = nprop;
    j = 0;
    k = nprop;
  }

  /* set trace length in stats structure */
  stats->nsamp = k;

  /* Find min, max depths */
  for(i=j; i<k; i++) {
    if(trace->data[i] < *mind && trace->data[i] >= 0)
      *mind = trace->data[i];
    if(trace->data[i] > *maxd)
      *maxd = trace->data[i];
  }
}

void subset_trace(segy *trout, int nsamp, int nvklist, float *vklist, float vdt,
                  float vdelrt, int verbose)
{

  segy trace;  /* scratch */
 
  int i;

  for(i=0; i<nsamp; i++)
    trace.data[i] = trout->data[i];

  for(i=0; i<nvklist; i++)
    trout->data[i] = trace.data[(int)vklist[i]];

  trout->ns = nvklist;
  trout->dt = (int)vdt * 1000;
  trout->delrt = (int)vdelrt;

}

void trace_stats(segy *trace, int nlayers, int nvklist, int nprop,
                 int *iprop, int layer_boundary, int verbose)
{

  int i, j, k;
  int n;
  int np;

  /* if seismic data, use single-value stats */
  if(file_hdr.nprop == 0 || (file_hdr.nprop != 0 && nprop == 0) || layer_boundary == 1) {
    for(i=0; i<stats.nsamp; i++) {
      if(trace->data[i] != -999. && trace->data[i] != -999.25) {
        if(trace->data[i] < file_hdr.minval)
          file_hdr.minval = trace->data[i];
        if(trace->data[i] > file_hdr.maxval)
          file_hdr.maxval = trace->data[i];
        file_hdr.meanval += trace->data[i];
        file_hdr.rmsval += trace->data[i] * trace->data[i];
        stats.nvals[0]++;
      }
    }
  }
  /* properties and horizons need separate stats */
  else {
    np = (nprop > 0) ? nprop : -nprop;
    for(i=0; i<np; i++) {
      /* vkey? use nvklist, else X-sect? use nlayers, else map-view? use nsamp */
      if(nvklist != 0)
        n = nvklist;
      else if(file_hdr.data_order <= 1) {
        if(file_hdr.nprop > 0)
          n = nlayers;
        else
          n = 1;
      }
      else if(file_hdr.data_order == 2)
        n = stats.nsamp;
      if(file_hdr.nprop > 0)
        k = iprop[i] / nlayers;
      else
        k = iprop[i] - 1;
      for(j=i*n; j<i*n+n; j++) {
        if(trace->data[j] != -999. && trace->data[j] != -999.25) {
          if(trace->data[j] < file_hdr.prop_minval[k])
            file_hdr.prop_minval[k] = trace->data[j];
          if(trace->data[j] > file_hdr.prop_maxval[k])
            file_hdr.prop_maxval[k] = trace->data[j];
          file_hdr.prop_meanval[k] += trace->data[j];
          file_hdr.prop_rmsval[k] += trace->data[j] * trace->data[j];
          stats.nvals[k]++;
        }
      }
    }
  }
}

void set_stats(data_stats *stats, char *data_type, char *data_order, int vflag,
               int nsamp, float dt, float delrt, int nvklist, float vdt,
               float vdelrt, float vmin, float vinc, int units, int verbose)
{

  /* stats.nsamp */
  if(vflag == 1)
    stats->nsamp = nvklist;
  else if(vflag == 0)
    stats->nsamp = nsamp;

  /* if vkey is used n1=vdt, and n2=vdelrt */
  if(vflag == 1) {
    stats->n1 = vdt;
    stats->n2 = vdelrt;
    /* if seismic, x-sect, time, scale to seconds */
/*
    if(!strcmp(data_type,"SEISMIC") && !strcmp(data_order,"CROSS-SECTION") && units == 0) {
      stats->n1 *= 0.001;
      stats->n2 *= 0.001;
    }
*/
  }
  /* no vkey used */
  else {
    /* map-view is vinc, vmin regardless of data-type */
    if(!strcmp(data_order,"MAP-VIEW")) {
      stats->n1 = vinc;
      stats->n2 = vmin;
    }
    /* x-sect, no vkey */
    else {
      /* seismic x-sect assume dt and delrt are already scaled correctly */
      if(!strcmp(data_type,"SEISMIC") && !strcmp(data_order,"CROSS-SECTION") && units == 0) {
        stats->n1 = dt;
        stats->n2 = delrt;
      }     
      /* seismic x-sect dt and delrt are m/ft */
      else if(!strcmp(data_type,"SEISMIC") && !strcmp(data_order,"CROSS-SECTION") && units != 0) {
        stats->n1 = dt;
        stats->n2 = delrt;
      }     
      /* events x-sect 1 and 0 */
      else if(!strcmp(data_type,"HORIZON") && !strcmp(data_order,"CROSS-SECTION")) {
        stats->n1 = 1.0;
        stats->n2 = 0.;
      }
      /* model x-sect build_trace will find maxdepth, mindepth */
      else if(!strcmp(data_type,"PROPERTY") && !strcmp(data_order,"CROSS-SECTION")) {
        stats->n1 = 0;
        stats->n2 = 0;
      }
    }
  }
}

/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */

/* Modified version of SU fgettr function to allow native or foreign endian via stdin */

/*********************** self documentation **********************/
/****************************************************************************
BHP_GETTR - Routines to get an SU trace from a file

bhp_fgettr   get a fixed-length segy trace from a file by file pointer
bhp_gettr    macro using fgettr to get a trace from stdin

*****************************************************************************
Function Prototype:
int bhp_fgettr(FILE *fp, segy *tp, int my_endian, int data_endian);

my_endian = caller's endianness, 1=BIG, 0=LITTLE
data_endian = input data endianness, 1=BIG, 0=LITTLE

*****************************************************************************
Returns: bhp_fgettr: int: number of bytes read on current trace (0 after last trace)

Macros
#define bhp_gettr(x,my_endian,data_endian)  bhp_fgettr(stdin,(x),my_endian,data_endian)

Usage example:
   segy tr;
   ...
   while (gettr(&tr,my_endian,data_endian)) {
     tr.offset = abs(tr.offset);
     puttr(&tr);
   }
   ...

*****************************************************************************
Authors: SEP: Einar Kjartansson, Stew Levin CWP: Shuki Ronen, Jack Cohen
****************************************************************************/
/*
 * Revised: 7/2/95 Stewart A. Levin   Mobil
 *     Major rewrite:  Use xdr library for portable su output file
 *     format.   Merge fgettr and fgettra into same source file.
 *     Make input from multiple streams work (at long last!).
 * Revised: 11/22/95 Stewart A. Levin  Mobil
 *     Always set ntr for DISK input.  This fixes susort failure.
 * Revised: 1/9/96  jkc CWP
 *     Set lastfp on nread <=0 return, too.
 */
/**************** end self doc ********************************/

#include "su.h"
#include "segy.h"
#include "header.h"

static struct insegyinfo {
  FILE *infp;                  /* FILE * ptr for search   */
  struct insegyinfo *nextinfo; /* linked list pointer      */
  unsigned long itr;       /* number of traces read   */
  int nsfirst;         /* samples from 1st header */
  unsigned short bytesper;     /* bytes per datum     */
  int nsegy;          /* segy bytes from nsfirst */
  int ntr;                     /* traces in input,if known */
  FileType ftype;         /* file type of input *fp */
} *insegylist = (struct insegyinfo *) NULL;

static FILE *lastfp = (FILE *) NULL;
static struct insegyinfo *infoptr, **oldinfoptr;

static
void searchlist(FILE *fp)
{
  oldinfoptr = &insegylist;
  for(infoptr = insegylist; infoptr != ((struct insegyinfo *) NULL);
      infoptr = infoptr->nextinfo) {
    if(fp == infoptr->infp) break;
    oldinfoptr = &infoptr->nextinfo;
  }
}

static
int dataread(segy *tp, struct insegyinfo *iptr, cwp_Bool fixed_length)
{
  unsigned int nsread = fixed_length?iptr->nsfirst:tp->ns;
  unsigned int databytes = infoptr->bytesper*nsread;
  int nread = (int) efread((char *) (&((tp->data)[0])),1, databytes, iptr->infp);

  if(nread > 0 && nread != databytes)
    fprintf(stderr,"%s: on trace #%ld, tried to read %d bytes, "
        "read %d bytes ",
        __FILE__, (infoptr->itr)+1, databytes, nread);

  return(nread);
}

static
int bhp_fgettr_internal(FILE *fp, segy *tp, cwp_Bool fixed_length,
                        int my_endian, int data_endian)
{
 
  int i;
  int nread;  /* bytes seen by fread calls  */

  /* search linked list for possible alternative */
  if(fp != lastfp)
    searchlist(fp);

  if (infoptr == ((struct insegyinfo *) NULL)) {
    /* initialize new segy input stream */
    unsigned int databytes;  /* bytes from nsfirst*/

    /* allocate new segy input information table */
    *oldinfoptr = (struct insegyinfo *)
     malloc(sizeof(struct insegyinfo));
    infoptr = *oldinfoptr;
    infoptr->nextinfo = (struct insegyinfo *) NULL;
    infoptr->infp = fp;  /* save FILE * ptr */
    infoptr->itr = 0;
    infoptr->ntr = -1;

    switch (infoptr->ftype = filestat(fileno(fp))) {
      case DIRECTORY:
        err("%s: segy input can't be a directory", __FILE__);
      case TTY:
        err("%s: segy input can't be tty", __FILE__);
      default:
        /* all others are ok */
      break;
    }
    
    /* Get the header */
    switch (nread = (int) efread(tp, 1, HDRBYTES, infoptr->infp)) {
      case 0:
        return 0; /* no traces; trap in mains */
      default:
        if (nread != HDRBYTES)
          err("%s: bad first header", __FILE__);
    }

    /* swap headers if necessary */
    if(my_endian != data_endian)
      for(i=0; i<SU_NKEYS; i++)
        swaphval(tp,i);

    /* Have the header, now for the data */
    infoptr->nsfirst = tp->ns;
    if (infoptr->nsfirst > SU_NFLTS)
      err("%s: unable to handle %d > %d samples per trace",
          __FILE__, infoptr->nsfirst, SU_NFLTS);

    switch (tp->trid) {
      case CHARPACK:
        infoptr->bytesper = sizeof(char); break;
      case SHORTPACK:
        infoptr->bytesper = 2*sizeof(char); break;
      default:
        infoptr->bytesper = sizeof(float); break;
    }

    databytes = infoptr->bytesper * tp->ns;

    infoptr->nsegy = HDRBYTES + databytes;

    /* Inconvenient to bump nread here; do it in the switch */
    nread = dataread(tp, infoptr, fixed_length);

    switch (nread) {
      case 0:
        err("%s: no data on first trace", __FILE__);
      default:
        if (nread != databytes)
          err("%s: first trace: read only %d bytes of %u",__FILE__, nread, databytes);
        else nread += HDRBYTES;
    }

    if (infoptr->ftype == DISK) { /* compute ntr */
      efseek(fp,0L,SEEK_END);
      infoptr->ntr = (int)(eftell(fp)/infoptr->nsegy);
      efseek(fp,infoptr->nsegy,SEEK_SET); /* reset fp */
    }

    /* swap data if necessary */
    if(my_endian != data_endian)
      for(i=0; i<tp->ns; i++)
        swap_float_4(&tp->data[i]);

  }
  else {    /* Not first entry */
    switch (nread = (int) efread(tp, 1, HDRBYTES, infoptr->infp)) {
      case 0:
        lastfp = infoptr->infp;
        return 0; /* finished */
    default:
      if (nread != HDRBYTES) {
        fprintf(stderr,"%s: on trace #%ld read %d bytes expected %d bytes\n",
            __FILE__,(infoptr->itr)+1,nread,HDRBYTES);
        return 0;
      }
    }

    /* swap headers if necessary */
    if(my_endian != data_endian)
      for(i=0; i<SU_NKEYS; i++)
        swaphval(tp,i);

    nread += dataread(tp, infoptr, fixed_length);

    if (fixed_length && (tp->ns != infoptr->nsfirst))
      fprintf(stderr,"%s: on trace #%ld, "
          "number of samples in header (%d) "
          "differs from number for first trace (%d)",
          __FILE__, (infoptr->itr)+1, tp->ns,
          infoptr->nsfirst);

    /* swap data if necessary */
    if(my_endian != data_endian)
      for(i=0; i<tp->ns; i++)
        swap_float_4(&tp->data[i]);

  }

  ++(infoptr->itr);
  lastfp = infoptr->infp;
  return (nread);
}

int bhp_fgettr(FILE *fp, segy *tp, int my_endian, int data_endian)
{
 return(bhp_fgettr_internal(fp,tp,cwp_true,my_endian,data_endian));
}

void parse_sub_stringf(char *sub_string, float *n1, float *n2, float *n3, int verbose)
{

  char *p1, *p2, *p3, *p4, *p5, *p6;   /* p1->1st int, p2->hyphen, p3->2nd int, */
                                       /* p4->left paren, p5-> paren contents, p6->right paren */

  char string[32];

  /* Set pointers */
  p1 = sub_string;
  p2 = strstr(sub_string,"-");
  /* If p1 is negative */
  if(p2 == p1)
    p2 = strstr(p2+1,"-");
  if(p2)
    p3 = p2 + 1;
  else
    p3 = NULL;
  p4 = strstr(sub_string,"[");
  if(p4)
    p5 = p4 + 1;
  else
    p5 = NULL;
  p6 = strstr(sub_string,"]");
/*
  fprintf(stderr,"Pointers\n");
  if(p1)
    fprintf(stderr,"p1 %s\n", p1);
  if(p2)
    fprintf(stderr,"p2 %s\n", p2);
  if(p3)
    fprintf(stderr,"p3 %s\n", p3);
  if(p4)
    fprintf(stderr,"p4 %s\n", p4);
  if(p5)
    fprintf(stderr,"p5 %s\n", p5);
  if(p6)
    fprintf(stderr,"p6 %s\n", p6);
*/
  /* If p1 length is zero, empty sub-string */
  if(!strlen(sub_string))
    err("Empty sub-string\n");

  /* If p2, p4, p6 NULL => n1 */
  if(p2 == NULL && p4 == NULL && p6 == NULL && p1 != NULL)
    *n1 = atof(sub_string);

  /* If hyphen, no (), and there is something before and after hyphen => n1-n2 */
  else if(p2 != NULL && p4 == NULL && p6 == NULL && p2 > p1 && strlen(p3)) {
    *n1 = atof(p1);
    *n2 = atof(p3);
  }

  /* Check for illegal combination of float and hyphen */
  else if((p2 != NULL && p2 == p1) || (p3 != NULL && strlen(p3) == 0)) {
    fprintf(stderr,"p1,p2,p3 %s %s %s\n", p1,p2,p3);
    err("Illegal combination of value(s) and hyphen, or missing integers\n");
  }

  /* Therefore, must be => n-m(k) */
  /* If left paren, there must be right paren */
  else if(p4 != NULL && p6 == NULL)
    err("Sub-string has a left paren without a matching right paren\n");

  /* If right paren, there must be left paren */
  else if(p6 != NULL && p4 == NULL)
    err("Sub-string has a right paren without a matching left paren\n");

  /* If parens, they must be left, then right and there must be contents */
  else if(p6 < p4)
    err("Parentheses are backwards\n");

  /* Check contents */
  else if(p6 - p4 == 1)
    err("Empty parentheses\n");

  /* Get contents of () */
  else {
    strncpy(string,p5,p6-p4-1);
    string[p6-p4-1] = '\0';
    /* Contents must be float */
    *n3 = atof(string);
    if(!*n3)
      err("Parentheses contain zero or non-integer\n");

    /* Verify preceeding hyphen with something before -, and something between - and ( */
    if(!p2)
      err("Range, including hyphen, must preceed parentheses\n");

    /* Get range */
    if(p2 != NULL && p2 < p4 && p2 > p1 && p4 > p3) {
      *n1 = atof(p1);
      *n2 = atof(p3);
      /* Make sure n1 and n2 are float */
      if(*n1 && *n2) {
        if(verbose)
          fprintf(stderr,"n1-n2(n3) %f-%f(%f)\n", *n1,*n2,*n3);
      }
      /*else
        err("Sub-string contains zero or non-integer\n");*/
    }

    /* Hyphen out of place */
    else if(p2 > p4)
      err("Hyphen must preceed ()\n");
    else if(p2 == p1)
      err("Integer must preceed hyphen\n");
    else if(p4 <= p3)
      err("Integer must preceed (\n");
  }

}

void parse_keyf(char *string, float *keylist, int *nlist, float min, float max, float inc,
               int interp_type, char *rule, int verbose)
{

/*
  A parameter string consists of 1 or more sub-strings, separated by commas
  A sub-string can contain up to 3 floats, one hyphen, and one pair of parentheses
  Valid sub-string Formats

  n            Item containing value n
  n-m          Items containing values n thru m, inclusive
  n-m(k)       Items containing every kth value, from item containing n thru item containing m

*/

  char sub_string[32];      /* current sub-string */
  char *ptr1;               /* address of current sub-string */
  char *ptr2;               /* address of next comma */

  /* n1-n2(n3) */
  float n1;                 /* First number in sub-string */
  float n2;                 /* Second number in sub-string */
  float n3;                 /* Third number in sub-string */
  int total=0;              /* Number of key values in expanded key string */
  int i, j, n;

  ptr1 = string;
  /* Get a sub-string */
  for(;;) {
    ptr2 = strstr(ptr1,",");
    if(ptr2 == NULL)
      strcpy(sub_string,ptr1);
    else {
      strncpy(sub_string,ptr1,ptr2-ptr1);
      sub_string[ptr2-ptr1] = '\0';
      ptr1 = ptr2 + 1;
    }
    n1 = n2 = n3 = 0;
    parse_sub_stringf(sub_string,&n1,&n2,&n3,verbose);
    /* Test for n1, n2 out of range or not multiple of inc */
    if(n1)
      adjust_keyf(&n1,min,max,inc,rule,interp_type,verbose);
    if(n2)
      adjust_keyf(&n2,min,max,inc,rule,interp_type,verbose);
    if(n3) {
      /* Check that sign of n3 is compatible with direction of range */
      if(n2 > n1 && n3 <= 0)
        err("Increment must be positive if range is ascending\n");
      if(n1 > n2 && n3 >= 0)
        err("Increment must be negative if range is descending\n");
      i = NINT(n3 / inc);
      n3 = (float)i * inc;
      n = NINT((n2 - n1 + n3 ) / n3);
      for(i=total,j=0; i<total+n; i++,j++) {
        if((n1 + j * n3) > n2) {
          n = j;
          break;
        }
        keylist[i] = n1 + j * n3;
      }
      total += n;
    }
    else if(n2) {
      if(n2 > n1) {
        n = NINT((n2 - n1 + inc) / inc);
        for(i=total,j=0; i<total+n; i++,j++)
          keylist[i] = n1 + j * inc;
      }
      else {
        n = NINT((n2 - n1 - inc) / -(inc));
        for(i=total,j=0; i<total+n; i++,j++)
          keylist[i] = n1 - j * inc;
      }
      total += n;
    }
    else {
      keylist[total] = n1;
      total += 1;
    }
    if(ptr2 == NULL)
      break;
  }
  *nlist = total;

}

float *parse_keylistf(char *keylist_subset, float min, float max, float inc, int *nklist,
                      int interp_type, char *rule, int verbose)
{

  int i;

  float *klist;
  float *temp;

  char string[16];

  /* If * is present, set to read all */
  if(strstr(keylist_subset,"*") != NULL) {
    sprintf(keylist_subset,"%f",min);
    sprintf(string,"-");
    strcat(keylist_subset,string);
    sprintf(string,"%f",max);
    strcat(keylist_subset,string);
  }
  temp = calloc(2 * ((max - min + inc) / inc),sizeof(float));
  parse_keyf(keylist_subset,temp,nklist,min,max,inc,interp_type,rule,verbose);
  klist = calloc(*nklist,sizeof(float));
  for(i=0; i<*nklist; i++)
    klist[i] = temp[i];

  free(temp);

  return klist;

}
 
int isadir(char *path)
{

  int dir=0;

  struct stat statbuf;

  stat(path,&statbuf);
  if(statbuf.st_mode & S_IFDIR)
    dir = 1;

  return dir;

}

void printmsg(char *msg, char **key_name, int *key_vals, char *part, int count)
{

  int i;

  fprintf(stderr,"\n");
  fprintf(stderr,"Shutting down due to following error condition: \n");
  fprintf(stderr,"  %s",msg);
  fprintf(stderr,"Last Good Trace was: \n");
  for(i=0; i<file_hdr.nkeys; i++)
    fprintf(stderr,"  %s=%d",key_name[i],key_vals[i]);
  fprintf(stderr,"\n");
  fprintf(stderr,"Current Partition: %s, Trace Number: %d\n",part,count);
  fprintf(stderr,"\n");

}
 
void calc_stats(int verbose, int debug)
{

  int i, j;

  /* calculate mean, rms */
    if(file_hdr.nprop == 0) {
      if(stats.nvals[0] > 0) {
        file_hdr.meanval /= stats.nvals[0];
        file_hdr.rmsval = sqrt(file_hdr.rmsval / stats.nvals[0]);
      }
    }
    else {
      j = (file_hdr.nprop > 0) ? file_hdr.nprop : -file_hdr.nprop;
      for(i=0; i<j; i++) {
        if(stats.nvals[i] > 0) {
          file_hdr.prop_meanval[i] /= stats.nvals[i];
          file_hdr.prop_rmsval[i] = sqrt(file_hdr.prop_rmsval[i] / stats.nvals[i]);
        }
      }
    }

}

char *get_filename_pathlist(int verbose) {

  char *filename;
  char *pathlist;

  int i, j;

  i = getparstring("filename",&filename);
  j = getparstring("pathlist",&pathlist);

  /* either filename or pathlist must be specified */
  if(i == 0 && j == 0)
    err("either filename or pathlist is required\n");

  if(verbose) {
    if(i == 1 && j == 1)
      fprintf(stderr,"Filename and pathlist both specified\n");
    else if(i == 1 && j == 0)
      fprintf(stderr,"Filename specified, building pathlist\n");
    else if(i == 0 && j == 1)
      fprintf(stderr,"Pathlist specified, building filename\n");
  }

  /* pathlist only, build filename */
  if(i == 0 && j == 1) {
    filename = calloc((int)strlen(pathlist)+1,sizeof(char));
    strcpy(filename,pathlist);
    /* strip off path if present */
    filename = basename(filename);
    /* strip off last 4 chars, assumed to be .dat */
    filename[(int)strlen(filename)-4] = '\0';
    if(verbose)
      fprintf(stderr,"Constructed FILENAME: %s\n",filename);
  }

  return filename;

}

void adjust_keyf(float *key, float min, float max, float inc, char *rule, int interp_type, int verbose)
{

  float k1, k2;
  float d1, d2;

  int m1, m2;

  if(*key >= max)
    *key = max;
  else if(*key <= min)
    *key = min;
  else {
    /* adjust to nearest multiple of inc if not interpolating and rule not match */
    if(interp_type == 0 && strcmp(rule,"match")) {
      m1 = (*key - min) / inc;
      m2 = (*key + inc - min) / inc;
      k1 = min + m1 * inc;
      k2 = min + m2 * inc;
      d1 = *key - k1;
      d2 = k2 - *key;
      if(d1 < d2)
        *key = k1;
      else
        *key = k2; 
    }
  }
}
