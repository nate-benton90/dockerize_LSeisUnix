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
#include <errno.h>
#include <sys/stat.h>
#include <libgen.h>
#include "bhpio.h"

/*prototypes */
void write_archive(float cur_size, char *archive, char *opt, char *tocfile,
                   FILE *pfp, int ncur, int verbose);
int check_type(char *archive, int verbose);
void open_next_archive(char *archive, int *narch, char *arpar, char *stacker,
                       char *tocfile, int verbose);

/* globals */
float gb=1024*1024*1024; /* 1 GB */
#define sleeptime 10      /* 10-second sleeptime */

/*********************** self documentation **********************/
char *sdoc[] = {
"  BHPARCHIVE creates a tar archive of one or more BHPIO datasets. ",
"                                                                  ",
"  Usage: bhparchive filename= archive= [optional parameters]      ",
"                                                                  ",
"  Required Parameters:                                            ",
"    filename=name           Name of one or more existing BHPIO    ",
"                            datasets, separated by commas.        ",
"                                                                  ",
"    archive=                File or device on which to write      ",
"                            archived data.                        ",
"  Optional Parameters:                                            ",
"    pathlist=name.dat       BHPIO pathlist - contains list of     ",
"                            directories where dataset partitions  ",
"                            are written. If unspecified, pathlist ",
"                            names are constructed as 'filename.dat',",
"                            and are assumed to be in the current  ",
"                            working directory.                    ",
"    size=                   Number of Gigabytes (1024*1024*1024)  ",
"                            at which to switch output volumes.    ",
"                            Individual files will NOT span archive",
"                            volumes. Therefore, size, if specified,",
"                            must be larger than the largest file  ",
"                            to be archived. If unspecified, all   ",
"                            data is archived to a single file or  ",
"                            volume.                               ",
"    stacker=no              Automatic stacker option. Default is to",
"                            issue an end-of-tape message and wait ",
"                            for user to respond when next tape is ",
"                            loaded. If stacker=yes is specified,  ",
"                            a 'rewoffl' command is issued to go to",
"                            the next tape automatically.          ",
"    verbose=0               Use verbose=1 to see extra printout.  ",
"                                                                  ",
"  BHPARCHIVE creates the archive using the UNIX tar command.      ",
"  The first file written to each archive is named 'Table_of_Contents'",
"  and contains a list of all files written to that archive volume.",
"  The filesytem names in the pathlist file are not written to the ",
"  archive, so that data can be restored to a set of directories   ",
"  that are different from where it was archived.                  ",
"  See BHPRESTORE for restoring archived BHPIO datasets.           ",
"                                                                  ",
"  Error Conditions:                                               ",
"    The following conditions will cause bhparchive to abort.      ",
"  Filename unspecified.                                           ",
"  Number of pathlist entries does not match number of filename entries.",
"  Pathlist file does not exist.                                   ",
"  Failure to find all of the .HDR files or data partitions for any",
"   filename.                                                      ",
"  Failure to create archive file or failure to access archive device.",
"                                                                  ",
"  Examples:                                                       ",
"    bhparchive filename=stack archive=/dev/rmt0                   ",
"      stack.dat is assumed to exist in the directory from which   ",
"      bhparchive was run. All HDR files and data partitions will be",
"      written to a single tape. If not all data will fit, then    ",
"      one or more files could be split across tapes.              ",
"    bhparchive filename=stack,model pathlist=/data/x.dat,/data/y.dat \\",
"     archive=/scratch/x.tar size=2                                ",
"      Two BHPIO datasets will be archived. If not all data will   ",
"      fit in 2GB, additional archive files will be created as     ",
"      /scratch/x.tar.1, /scratch/x.tar.2, etc.                    ",
"                                                                  ",
NULL};

int main(int argc, char **argv)
{

  char fpath[NAMELEN];    /* /path/file for a partition */
  char **hpath;           /* /path/file for all HDR files */
  char command[256];      /* unix commands */
  char cindex[8];         /* Use to append file index to path */
  char *path;             /* First path from pathlist */
  char *aname;            /* aliased filename */
  char *tmp;              /* temp dir for TOC file */
  char *record;           /* record from toc */
  char *string;           /* scratch */
  char *arpar;            /* archive parameter */
  char *archive;          /* archive filename */
  char *file;             /* basename from /path../file */
  char *dir;              /* dirname from /path../file */
  char tar_opt[8];        /* cvf or rvf */
  char *tocfile;          /* tmp/Table_of_Contents */
  char *stacker;          /* no=wait for next tape */

  cwp_String filename[512];  /* list of filenames to save */
  cwp_String pathlist[512];  /* list of pathlists */

  float size;             /* size of output */
  float cur_size;         /* size of current fileset */
  float temp;             /* cur_size in GB */

  int verbose;            /* debug printout */
  int i, j, k;            /* Loop counters */
  int nfiles;             /* number of files to save */
  int ncur;               /* number of files in current set */
  int nhdr;               /* number of HDR files */
  int *hdr_size;          /* size of HDR files */
/* tar_open flag is OBSELETE ?? */
  int tar_open=0;         /* 1=tarfile in progress, else 0 */
  int narch;              /* number of archive volumes written */
  int filetype;           /* 0=disk, use append for 2nd thru last files */

  FILE *fp;               /* first partition */
  FILE *tocfp;            /* table of contents for each archive volume */
  FILE *pfp;              /* command pipe */
  
  struct stat info;

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  nfiles = countparval("filename");
  if(nfiles == 0)
    err("filename parameter required\n");
  getparstringarray("filename",filename);
  i = countparval("pathlist");
  if(i == 0) {
    for(i=0; i<nfiles; i++) {
      pathlist[i] = calloc(strlen(filename[i])+5,sizeof(char));
      strcpy(pathlist[i],filename[i]);
      strcat(pathlist[i],".dat");
    }
  }
  else if(i != nfiles) {
    printf("%d filenames, %d pathlists\n",nfiles,i);
    err("number of pathlists must be same as number of filename\n");
  }
  else
    getparstringarray("pathlist",pathlist);
  printf("Filenames and Pathlists:\n");
  for(i=0; i<nfiles; i++)
    printf("  %s    %s\n",filename[i],pathlist[i]);
  file_hdr.filename = calloc(NAMELEN,sizeof(char));

  /* archive required */
  if(!getparstring("archive",&arpar))
    err("archive is required\n");
  archive = calloc((int)strlen(arpar)+10,sizeof(char));
  strcpy(archive,arpar);
  printf("archive=%s\n",archive);

  /* size of output */
  if(!getparfloat("size",&size)) {
    printf("Not checking size of archive\n");
    size = FLT_MAX;
  }
  else
    printf("Size of Output Volume: %.4f gigabytes\n",size);

  /* stacker */ 
  if(!getparstring("stacker",&stacker))
    stacker = "no";
  if(!strcmp(stacker,"yes"))
    printf("Tape changes will be done automatically\n");
  else
    printf("Tape changes will be done interactively\n");

  /* tape or disk archive? */
  filetype = check_type(archive,verbose);
  if(filetype == 0)
    printf("Achiving to disk file, will use append option for 2nd thru last files\n");
  else
    printf("Achiving to tape\n");

  /* HDR file paths */
  hdr_size = calloc(NFILES,sizeof(int));
  hpath = calloc(NFILES,sizeof(char *));
  for(i=0; i<NFILES; i++)
    hpath[i] = calloc(NAMELEN,sizeof(char));
  /* get temp file to hold table of contents for each output volume */
  tmp = calloc(L_tmpnam,sizeof(char));
  tmp = tmpnam(tmp);
  printf("tmp=%s\n",tmp);
  strcpy(command,"mkdir ");
  strcat(command,tmp);
  pfp = popen(command,"w");
  pclose(pfp);
  tocfile = calloc((int)strlen(tmp)+18,sizeof(char));
  strcpy(tocfile,tmp);
  strcat(tocfile,"/");
  strcat(tocfile,"Table_of_Contents");
  tocfp = fopen(tocfile,"w");
  printf("Opened %s for writing\n",tocfile);
  record = calloc(256,sizeof(char));
  string = calloc(256,sizeof(char));
  narch = 0;
  /* loop over files, get sizes */
  cur_size = 0;
  ncur = 0;
  for(i=0; i<nfiles; i++) {
    /* check for alias */
    j = (int)strlen(filename[i]);
    if(!strcmp(&filename[i][j-10],"_as_events")) {
      /* trim */
      filename[i][j-10] = '\0';
      printf("Trimmed Filename: %s\n",filename[i]);
    }
    printf("Opening %s\n", pathlist[i]);
    if(!(hdrfp = fopen(pathlist[i],"r")))
      err("pathlist specified, but cannot open %s\n",pathlist[i]);
    else {
      /* Only need first path to get to partition zero */
      path = calloc(NAMELEN,sizeof(char));
      fscanf(hdrfp,"%s\n",path);
      fclose(hdrfp);
    }
    /* Make sure file exists */
    if(get_bhpio_path(path,filename[i],"_0000.HDR",fpath,0))
      printf("FILE: path=%s, filename=%s exists\n",path,filename[i]);
    else {
      printf("FILE: path=%s, filename=%s does not exist\n",path,filename[i]);
      return EXIT_FAILURE;
    }

    /* Get file header */
    read_file_hdr(fpath,0);
    /* check partitions, make sure they are /path/file format */
    for(j=0; j<file_hdr.nparts; j++) {
      if(isadir(&filesys[j*NAMELEN]) == 1) {
        strcat(&filesys[j*NAMELEN],"/");
        strcat(&filesys[j*NAMELEN],file_hdr.filename);
        strcat(&filesys[j*NAMELEN],"_");
        sprintf(cindex,"%04i\0",j+1);
        strcat(&filesys[j*NAMELEN],cindex);
        strcat(&filesys[j*NAMELEN],".su");
      }
    }
    write_file_hdr(fpath,0);
    /* Get sizes of all HDR files */
    nhdr = 0;
    if(get_bhpio_path(path,file_hdr.filename,"_0000.HDR",hpath[nhdr],0)) {
      j = stat(hpath[nhdr],&info);
      hdr_size[nhdr] = info.st_size;
      nhdr++;
    }
    else
      err("Could not open %s\n",fpath);
    if(get_bhpio_path(path,file_hdr.filename,"_0000.0001.HDR",hpath[nhdr],0)) {
      j = stat(hpath[nhdr],&info);
      hdr_size[nhdr] = info.st_size;
      nhdr++;
    }
    else
      err("Could not open %s\n",fpath);
    if(get_bhpio_path(path,file_hdr.filename,"_0000.0002.HDR",hpath[nhdr],0)) {
      j = stat(hpath[nhdr],&info);
      hdr_size[nhdr] = info.st_size;
      nhdr++;
    }
    else
      err("Could not open %s\n",fpath);
    if(get_bhpio_path(path,file_hdr.filename,"_LOCK.HDR",hpath[nhdr],0)) {
      j = stat(hpath[nhdr],&info);
      hdr_size[nhdr] = info.st_size;
      nhdr++;
    }
    /* put hdr files on list */
    for(j=0; j<nhdr; j++)
      fprintf(tocfp,"%s contains %d bytes\n",hpath[j],hdr_size[j]);
    ncur += nhdr;
    for(j=0; j<nhdr; j++)
      cur_size += hdr_size[j];
    nhdr = 0;
    /* check cur_size + hdr_sizes vs size */
    temp = cur_size;
    for(j=0; j<nhdr; j++)
      temp += hdr_size[j];
    temp /= gb;
    if(temp > size) {
      fclose(tocfp);
      if(tar_open == 0) {
        tar_open = 1;
        strcpy(tar_opt,"cvf");
      }
      else if(tar_open == 1 && filetype == 0)
        strcpy(tar_opt,"rvf");
      write_archive(cur_size,archive,tar_opt,tocfile,pfp,ncur,verbose);
      open_next_archive(archive,&narch,arpar,stacker,tocfile,verbose);
      ncur = 0;
      cur_size = 0;
      tocfp = fopen(tocfile,"w");
      tar_open = 0;
    }
    /* loop through file_hdr partition list */
    printf("Number of Partitions: %d\n", file_hdr.nparts);
    for(j=0; j<file_hdr.nparts; j++) {
      strcpy(fpath,&filesys[j*NAMELEN]);
      k = stat(fpath,&info);
      if(info.st_size / gb > size)
        err("File %s will not fit on archive, re-run with larger size\n",fpath);
      /* check cur_size vs size */
      temp = (cur_size + info.st_size) / gb;
      if(temp > size) {
        fclose(tocfp);
        if(tar_open == 0) {
          tar_open = 1;
          strcpy(tar_opt,"cvf");
        }
        else if(tar_open == 1 && filetype == 0)
          strcpy(tar_opt,"rvf");
        write_archive(cur_size,archive,tar_opt,tocfile,pfp,ncur,verbose);
        open_next_archive(archive,&narch,arpar,stacker,tocfile,verbose);
        cur_size = 0;
        ncur = 0;
        tocfp = fopen(tocfile,"w");
        tar_open = 0;
      }
      ncur++;
      cur_size += info.st_size;
      fprintf(tocfp,"%s contains %d bytes\n",fpath,info.st_size);
    }
  }

  /* archive remaining files */
  if(ncur > 0) {
    fclose(tocfp);
    if(tar_open == 0) {
      tar_open = 1;
      strcpy(tar_opt,"cvf");
    }
    else if(tar_open == 1 && filetype == 0)
      strcpy(tar_opt,"rvf");
    write_archive(cur_size,archive,tar_opt,tocfile,pfp,ncur,verbose);
    /* if tape, take offline */
    if(filetype != 0) {
      strcpy(command,"mt -f ");
      strcat(command,arpar);
      /* for testing, if verbose=2, don't take offline */
      if(verbose == 2)
        strcat(command," rewind");
      else
        strcat(command," rewoffl");
      printf("Executing %s\n",command);
      pfp = popen(command,"w");
      pclose(pfp);
    }
  }

  return EXIT_SUCCESS;

}

void write_archive(float cur_size, char *archive, char *opt, char *tocfile,
                   FILE *pfp, int ncur, int verbose)
{

  char command[256];
  char record[256];
  char string[256];
  char *file, *dir;

  int i;
  int filetype;

  FILE *tocfp;

  cur_size /= gb;
  printf("Writing current fileset, containing %d files, and %.6f GB, to archive\n",ncur,cur_size);
  strcpy(string,tocfile);
  file = basename(string);
  dir = dirname(string);
  printf("Archiving %s from %s\n",file,dir);
  strcpy(command,"tar cvf");
  strcat(command," ");
  strcat(command,archive);
  strcat(command," ");
  strcat(command,"-C ");
  strcat(command,dir);
  strcat(command," ");
  strcat(command,file);
  printf("Executing %s\n",command);
  pfp = popen(command,"w");
  pclose(pfp);
  tocfp = fopen(tocfile,"r");
  printf("Opened %s for reading\n",tocfile);
  for(i=0; i<ncur; i++) {
    fgets(record,127,tocfp);
    sscanf(record,"%s",string);
    file = basename(string);
    dir = dirname(string);
    printf("Archiving %s from %s\n",file,dir);
    strcpy(command,"tar ");
    filetype = check_type(archive,verbose);
    if(filetype == 0)
      strcat(command,"rvf ");
    else
      strcat(command,"cvf ");
    strcat(command,archive);
    strcat(command," ");
    strcat(command,"-C ");
    strcat(command,dir);
    strcat(command," ");
    strcat(command,file);
    printf("Executing %s\n",command);
    pfp = popen(command,"w");
    pclose(pfp);
  }
  fclose(tocfp);

}
int check_type(char *archive, int verbose)
{

  int type;

  struct stat statbuf;

  type = 0;

  if(stat(archive,&statbuf) == -1) {
    if(errno == ENOENT)
      printf("%s does not exist\n",archive);
    else
      err("Could not stat %s\n",archive);
  }
  else if(statbuf.st_mode & S_IFCHR || statbuf.st_mode & S_IFBLK) {
    printf("%s is character special or block special\n",archive);
    type = 1;
  }
  else if(statbuf.st_mode & S_IFREG)
    printf("%s is a regular file\n",archive);

  return type;

}  
void open_next_archive(char *archive, int *narch, char *arpar, char *stacker,
                       char *tocfile, int verbose)
{

  char tmp[8];
  char command[128];
  char string[128];

  FILE *pfp;
  FILE *tocfp;

  int type;

  FILE *tty;              /* user input */

  type = check_type(archive,verbose);
  *narch++;
  /* disk file */
  if(type == 0) {
    strcpy(archive,arpar);
    strcat(archive,".");
    sprintf(tmp,"%d",*narch);
    strcat(archive,tmp);
  }
  /* tape */
  else {
    strcpy(command,"mt -f ");
    strcat(command,arpar);
    /* for testing, if verbose=2, don't take offline */
    if(verbose == 2)
      strcat(command," rewind");
    else
      strcat(command," rewoffl");
    printf("Executing %s\n",command);
    pfp = popen(command,"w");
    pclose(pfp);
    if(!strcmp(stacker,"no")) {
      printf("Tape full, load next tape and hit any key to continue\n");
      printf("Use Ctrl-C to quit\n");
      tty = fopen("/dev/tty","r");
      getc(tty);
      fclose(tty);
    }
    /* wait for tape to load */
    for(;;) {
      printf("Sleep for %d seconds\n",sleeptime);
      sleep(sleeptime);
      /* check tape status */
      strcpy(command,"mt -f ");
      strcat(command,archive);
      strcat(command," status > ");
      strcat(command,tocfile);
      printf("Executing %s\n",command);
      pfp = popen(command,"w");
      pclose(pfp);
      tocfp = fopen(tocfile,"r");
      fscanf(tocfp,"%s",string);
      fclose(tocfp);
      printf("status=%s\n",string);
      if(!strcmp(string,"tape"))
        break;
    }
  }
}
