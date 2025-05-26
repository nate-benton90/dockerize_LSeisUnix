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
* KEYWORDS:  $RCSfile: bhpiocube.c,v $
*            $Revision: 1.9 $
*            $Date: 2003/02/24 17:20:30 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpiocube.c,v $
* Revision 1.9  2003/02/24 17:20:30  ahmilb
* Updated self-doc.
*
* Revision 1.8  2003/01/28 23:19:45  ahmilb
* Accomodate PARTITION that is either full path/name or just path
*
* Revision 1.7  2003/01/14 19:18:00  ahmilb
* Replace hard-coded lengths with NAMELEN, NFILES.
* Add isadir function.
*
* Revision 1.6  2002/07/29 15:35:07  ahmilb
* Show vkey info as floats.
*
* Revision 1.5  2002/05/07 18:42:57  ahmilb
* Get units from file header, stats deleted from file-header, check dt and delrt if no vkey.
*
* Revision 1.4  2002/01/29 15:31:40  ahmilb
* Add data-type, data-order for map-view access.
*
* Revision 1.3  2001/10/12 20:03:47  ahmilb
* Allow for aliased dataset names.
*
* Revision 1.2  2001/09/13 18:26:16  ahmilb
* Add summary of HORIZONS data.
*
* Revision 1.1  2001/07/25 18:52:28  ahmilb
* bhpio utility for cube files.
*
* Revision 1.3  2001/06/19 14:54:20  ahmilb
* Add LOCK file.
*
* Revision 1.2  2001/05/21 17:31:32  ahmilb
* Remove code for deleting local header files.
*
* Revision 1.1  2001/04/18 20:00:19  ahmilb
* New Hypercube BHPIO
*
* Revision 1.3  2001/02/09 00:16:15  ahglim
* updated to Bob's ahmilb (beta) directory
*
* Revision 1.2  2001/02/06 03:38:18  ahglim
* corrected comment problem
*
*
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bhpio.h"
#include "bhpiocube.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
"  BHPIO is a utility program used to display a summary of a BHPIO ",
"  dataset, and optionally, to delete the dataset.                 ",
"                                                                  ",
" Usage: bhpio filename=      [optional parameters]                ",
"                                                                  ",
" Required Parameters:                                             ",
"  filename=               File name which was used to create the dataset",
"                                                                  ",
" Optional Parameters:                                             ",
"  pathlist='filename'.dat Pathlist file which was used to create  ",
"                          the dataset                             ",
"  verbose=0               Use verbose=1 for debug print           ",
"  delete=no               yes/no to delete an existing file       ",
"                                                                  ",
" Trace Header Usage:                                              ",
"  bhpio uses dt and delrt from the first trace in the dataset to  ",
"  print vertical key information.                                 ",
"                                                                  ",
" Example:                                                         ",
"  bhpio filename=newstack pathlist=/hou/data/D_169_002/ahmilb/test_data/int_datasets/newstack.dat",
"                                                                  ",
"  Output from example:                                            ",
"Opening newstack.dat",
"FILE: path=/hou/data/D_169_002/ahmilb/test_data/INT, filename=newstack exists",
"newstack is SEISMIC data, stored in CROSS-SECTION order",
"newstack is written in BIG_ENDIAN format",
"UNITS are seconds",
"Traces per Bin: 1",
"Partition Size: 1999 MB",
"Number of Partitions: 1",
"  Partition 1: /hou/data/D_169_005/ahmilb/test_data/INT/newstack_0001.su",
"Number of Header Keys: 3",
"  Name: ep, Min: 3014, Max: 3025, Incr: 1",
"  Name: cdp, Min: 1350, Max: 1450, Incr: 1",
"  Name: tracl, Min: 0.000, Max: 8000.000, Incr: 4.000",
NULL};

int main(int argc, char **argv)
{

  char fpath[NAMELEN];    /* Partition zero path name */
  char cindex[8];         /* Use to append file index to path */
  char command[256];      /* For UNIX command */
  char *delete;           /* Delete parameter */
  char *summary;          /* Summary parameter */
  char *pathlist;         /* File containing user-specified path-list for forming filenames */
  char *path;             /* First path from pathlist */
  char *aname;            /* aliased filename */
  char data_type[16];     /* seismic, horizon, property */
  char data_order[16];    /* cross-section, map-view */

  int verbose;            /* debug printout */
  int i, j;               /* Loop counters */
  int npaths;             /* Number of user-specified paths */
#ifdef CWP_BIG_ENDIAN
  int my_endian=1;
#else
  int my_endian=0;
#endif

  float factor;           /* Scaling factor */

  FILE *fp;               /* first partition */
  
  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  /* summary=yes is always set */
  summary = "yes";

  if(!getparint("verbose",&verbose))
    verbose = 0;

  if(!getparstring("filename",&file_hdr.filename))
    err("Filename is required\n");
  /* save filename */
  i = strlen(file_hdr.filename);
  aname = calloc(i+1,sizeof(char));
  strcpy(aname,file_hdr.filename);

  /* check for alias */
  if(!strcmp(&file_hdr.filename[i-10],"_as_events")) {
    /* trim */
    file_hdr.filename[i-10] = '\0';
    if(verbose) {
      fprintf(stderr,"Aliased filename %s\n",aname);
      fprintf(stderr,"Trimmed filename %s\n",file_hdr.filename);
    }
  }

   npaths = 0;
  /* Get pathlist */
  if(!getparstring("pathlist",&pathlist)) {
    pathlist = calloc(strlen(file_hdr.filename)+5,sizeof(char));
    strcpy(pathlist,file_hdr.filename);
    strcat(pathlist,".dat");
  }
  printf("Opening %s\n", pathlist);
  if(!(hdrfp = fopen(pathlist,"r")))
    err("pathlist specified, but cannot open %s\n", pathlist);
  else {
    /* Only need first path to get to partition zero */
    npaths = 1;
    path = calloc(NFILES,sizeof(char));
    fscanf(hdrfp,"%s\n", path);
    fclose(hdrfp);
  }

  /* Delete? */
  if(!getparstring("delete",&delete))
    delete = "no";

  if(verbose) {
    printf("Path: %s,  Filename: %s\n",path,file_hdr.filename);
    if(!strcmp(delete,"yes"))
      printf("File is being deleted\n");
    if(!strcmp(summary,"yes"))
      printf("Print Summary\n");
  }

  /* Make sure file exists */
  if(get_bhpio_path(path,aname,"_0000.HDR",fpath,verbose))
    printf("FILE: path=%s, filename=%s exists\n",path,file_hdr.filename);
  else {
    printf("FILE: path=%s, filename=%s does not exist\n",path,file_hdr.filename);
    return EXIT_SUCCESS;
  }

  /* Get file header */
  read_file_hdr(fpath,verbose);
 for(i=0; i<file_hdr.nparts; i++)

  /* file type: seismic, horizon, property */
  if(file_hdr.data_type == 1 || (file_hdr.data_type == 0 && file_hdr.nprop == 0))
    strcpy(data_type,"SEISMIC");
  else if(file_hdr.data_type == 2 || (file_hdr.data_type == 0 && file_hdr.nprop < 0))
    strcpy(data_type,"HORIZON");
  else if(file_hdr.data_type == 3 || (file_hdr.data_type == 0 && file_hdr.nprop > 0))
    strcpy(data_type,"PROPERTY");

  /* data order: cross-section, map-view  */
  if(file_hdr.data_order <= 1)
    strcpy(data_order,"CROSS-SECTION");
  else if(file_hdr.data_order == 2)
    strcpy(data_order,"MAP-VIEW");

  /* data type and order */
  printf("%s is %s data, stored in %s order\n",file_hdr.filename,data_type,data_order);

  /* Print endianess */
  if(file_hdr.endian)
    printf("%s is written in BIG_ENDIAN format\n", file_hdr.filename);
  else
    printf("%s is written in LITTLE_ENDIAN format\n", file_hdr.filename);
  
  /* units */
  if(file_hdr.units == 0)
    printf("UNITS are seconds\n");
  else if(file_hdr.units == 1)
    printf("UNITS are feet\n");
  else if(file_hdr.units == 2)
    printf("UNITS are meters\n");
  else
    printf("UNITS are unspecified\n");

  printf("Traces per Bin: %d\n", file_hdr.bin);

  /* Summarize - loop through file_hdr partition list */
  if(!strcmp(summary,"yes")) {
    printf("Partition Size: %d MB\n", file_hdr.size);
    printf("Number of Partitions: %d\n", file_hdr.nparts);
    for(i=0; i<file_hdr.nparts; i++) {
      strcpy(fpath,&filesys[i*NAMELEN]);
      /* if path is a dir, append filename, else filename is already appended */
      if(isadir(fpath) == 1) {
        if(verbose)
          fprintf(stderr,"%s is a directory, appending filename\n",fpath);
        strcat(fpath,"/");
        strcat(fpath,file_hdr.filename);
        strcat(fpath,"_");
        sprintf(cindex,"%04i\0",i+1);
        strcat(fpath,cindex);
        strcat(fpath,".su");
      }
      /* if first part, read first trace header for dt,delrt */
      if(i == 0) {
        if((fp = fopen(fpath,"r")) == NULL)
          err("Cannot open %s\n",fpath);
        fread(&tr,sizeof(int),HDRBYTES/4,fp);
        fclose(fp);
        /* check endianness */
        if(my_endian != file_hdr.endian) {
          swap_short_2(&tr.dt);
          swap_short_2(&tr.delrt);
        }
      }
      printf("  Partition %d: %s\n", i+1,fpath);
    }
    /* header limits */
    hdr_limits = calloc(file_hdr.nkeys,sizeof(bhp_hdr_limits));
    if(!(get_bhpio_path(path,file_hdr.filename,"_0000.0001.HDR",fpath,verbose)))
      err("Cannot access header-limits: %s\n", fpath);
    /* Load hdr_limits */
    if(read_hdr_limits(fpath,verbose))
      err("Error reading %s\n", fpath);
    /* number of horizons or properties and layers */
    if(file_hdr.nprop < 0)
      printf("Number of Horizons: %d\n",-file_hdr.nprop);
    else if(file_hdr.nprop > 0) {
      printf("Number of Properties: %d\n",file_hdr.nprop);
      if(file_hdr.data_order <= 1)
        printf("Number of Layers: %d\n",(file_hdr.nsamp-1)/(file_hdr.nprop+1));
      else if(file_hdr.data_order == 2)
        printf("Number of Layers: %d\n",hdr_limits[0].bhp_hdr_max/file_hdr.nprop);
    }
    printf("Number of Header Keys: %d\n", file_hdr.nkeys+1);
    for(i=0; i<file_hdr.nkeys; i++) {
      factor = hdr_limits[i].bhp_hdr_scalar;
      if(factor < 0)
        factor = -1. / factor;
      hdr_limits[i].bhp_hdr_min *= factor;
      hdr_limits[i].bhp_hdr_max *= factor;
      if(!strcmp(data_type,"PROPERTY") && file_hdr.data_order == 2 && i == 0)
        printf("  Name: %s, Min: %d, Max: %d, Incr: %d\n",
               hdr_limits[i].bhp_hdr_name,hdr_limits[i].bhp_hdr_min,
               hdr_limits[i].bhp_hdr_max/file_hdr.nprop,hdr_limits[i].bhp_hdr_inc);
      else
        printf("  Name: %s, Min: %d, Max: %d, Incr: %d\n",
               hdr_limits[i].bhp_hdr_name,hdr_limits[i].bhp_hdr_min,
               hdr_limits[i].bhp_hdr_max,hdr_limits[i].bhp_hdr_inc);
    }
    /* if vkey is empty use tracl */
    if(!strcmp(file_hdr.vkey,"")) {
      strcpy(file_hdr.vkey,"tracl");
      if(!strcmp(data_type,"PROPERTY") || !strcmp(data_type,"HORIZON")) {
        file_hdr.vmin = 1;
        if(!strcmp(data_type,"PROPERTY"))
          file_hdr.vmax = (file_hdr.nsamp-1)/(file_hdr.nprop+1);
        else
          file_hdr.vmax = file_hdr.nsamp;
        file_hdr.vinc = 1;
      }
      else {
        file_hdr.vmin = tr.delrt;
        file_hdr.vinc = tr.dt * 0.001;
        file_hdr.vmax = tr.dt * 0.001 * (file_hdr.nsamp - 1);
      }
    }
    /* vkey for cross-section model data is 1 thru nlayers */
    if(file_hdr.data_order <= 1 && file_hdr.nprop > 0)
      printf("  Name: tracl, Min: 1, Max: %d, Incr: 1\n",(file_hdr.nsamp-1)/(file_hdr.nprop+1));
    /* vkey for cross-section event data is 1 thru nsamp */
    else if(file_hdr.data_order <= 1 && file_hdr.nprop < 0)
      printf("  Name: tracl, Min: 1, Max: %d, Incr: 1\n",file_hdr.nsamp);
    else
      printf("  Name: %s, Min: %.3f, Max: %.3f, Incr: %.3f\n", file_hdr.vkey,
             file_hdr.vmin,file_hdr.vmax,file_hdr.vinc);
    if(file_hdr.nprop > 0) {
      printf("PROPERTIES: %s ", properties[0]);
      for(i=1; i<file_hdr.nprop; i++)
        printf(" %s ", properties[i]);
      printf("\n");
    }
    else if(file_hdr.nprop < 0) {
      printf("HORIZONS: %s ", properties[0]);
      for(i=1; i<-file_hdr.nprop; i++)
        printf(" %s ", properties[i]);
      printf("\n");
    }
    if(file_hdr.bin > 1) {
      printf("Bin Limits: \n");
      for(j=0,i=hdr_limits[file_hdr.nkeys-1].bhp_hdr_min; i<hdr_limits[file_hdr.nkeys-1].bhp_hdr_max;
          i+=hdr_limits[file_hdr.nkeys-1].bhp_hdr_inc,j++) {
        printf("%3d: %5d --> %5d  ", j+1,i,i+hdr_limits[file_hdr.nkeys-1].bhp_hdr_inc-1);
        if(!((j+1)%4))
          printf("\n");
      }
      printf("\n");
    }
  }

  /* Remove - loop through file_hdr partition list */
  if(!strcmp(delete,"yes")) {
    for(i=0; i<file_hdr.nparts; i++) {
      printf("Partition %d: %s\n", i+1,&filesys[i*NAMELEN]);

      /* Data */
      strcpy(fpath,&filesys[i*NAMELEN]);
      /* if path is a dir, append filename, else filename is already appended */
      if(isadir(fpath) == 1) {
        if(verbose)
          fprintf(stderr,"%s is a directory, appending filename\n",fpath);
        strcat(fpath,"/");
        strcat(fpath,file_hdr.filename);
        strcat(fpath,"_");
        sprintf(cindex,"%04i\0",i+1);
        strcat(fpath,cindex);
        strcat(fpath,".su");
      }
      else {
        if(verbose)
        fprintf(stderr,"%s is a file, not appending filename\n",fpath);
      }
      strcpy(command,"rm ");
      strcat(command,fpath);
      printf("Executing %s\n", command);
      if(system(command))
        printf("%s failed\n", command);

      /* Global lock_file, header limits, cube, file header */
      if(i == 0) {
        /* lock_file */
        if(get_bhpio_path(path,file_hdr.filename,"_LOCK.HDR",fpath,verbose)) {
          strcpy(command,"rm ");
          strcat(command,fpath);
          printf("Executing %s\n", command);
          if(system(command))
            printf("%s failed\n", command);
        }
        else
          err("Cannot open header-limits: %s\n", fpath);
        /* Header limits */
        if(get_bhpio_path(path,file_hdr.filename,"_0000.0001.HDR",fpath,verbose)) {
          strcpy(command,"rm ");
          strcat(command,fpath);
          printf("Executing %s\n", command);
          if(system(command))
            printf("%s failed\n", command);
        }
        else
          err("Cannot open header-limits: %s\n", fpath);
        /* cube */
        if(get_bhpio_path(path,file_hdr.filename,"_0000.0002.HDR",fpath,verbose)) {
          strcpy(command,"rm ");
          strcat(command,fpath);
          printf("Executing %s\n", command);
          if(system(command))
            printf("%s failed\n", command);
        }
        else
          err("Cannot open index cube: %s\n", fpath);
        /* grid - optional */
        if(get_bhpio_path(path,file_hdr.filename,"_0000.0003.HDR",fpath,verbose)) {
          strcpy(command,"rm ");
          strcat(command,fpath);
          printf("Executing %s\n", command);
          if(system(command))
            printf("%s failed\n", command);
        }
        /* File header */
        if(get_bhpio_path(path,file_hdr.filename,"_0000.HDR",fpath,verbose)) {
          strcpy(command,"rm ");
          strcat(command,fpath);
          printf("Executing %s\n", command);
          if(system(command))
            printf("%s failed\n", command);
        }
        else
          err("Cannot open file-header: %s\n", fpath);
      }
    }
  }

  return EXIT_SUCCESS;

}
