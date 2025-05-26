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
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <libgen.h>
#include "bhpio.h"

/* globals */
typedef struct {
  int nfiles;        /* number of files in archive (excluding TOC) */
  char **files;      /* archive file names */
  int *size;         /* sizes in bytes */
  } arch_def;

/*prototypes */
void get_filelist(char *archive, char *tmp, arch_def *arch, char *cwd, int verbose);
void find_hdr_file(char *file, char *string, arch_def *arch, int *nf, int *hdr1, int *list, int verbose);
void find_data_files(char *file, arch_def *arch, int *nf, int *data1, int *list, int verbose);
void update_part_list(char *filesys, int nparts, char *tmp, char *path, char *file, int verbose);
void close_files(char *archive, FILE *fp, char *offline, int verbose);

/*********************** self documentation **********************/
char *sdoc[] = {
"  BHPRESTORE extracts a BHPIO dataset from a tar archive.         ",
"                                                                  ",
"  Usage: bhprestore archive= filename= [optional parameters]      ",
"                                                                  ",
"  Required Parameters:                                            ",
"    archive=name            Name of tar archive containing the    ",
"                            dataset to be restored.               ",
"    filename=name           Name of BHPIO dataset to restore.     ",
"                            Filename must match the filename that ",
"                            was archived.                         ",
"  Optional Parameters:                                            ",
"    pathlist=name.dat       BHPIO pathlist - contains list of     ",
"                            directories where dataset partitions  ",
"                            are to be restored. A dataset does not",
"                            have to be restored to the same       ",
"                            directories from which it was archived.",
"    toc=no                  If toc=yes is specified, then the     ",
"                            Table of Contents is retrieved from the",
"                            archive and printed. No data is restored.",
"    offline=yes             Archive device will be taken offline  ",
"                            after restore unless offline=no is    ",
"                            specified. If offline=no is specified,",
"                            the device will be rewound, and       ",
"                            remain online for another restore operation.",
"    verbose=0               Use verbose=1 to see extra printout.  ",
"                                                                  ",
"  See BHPARCHIVE to create a tar archive of BHPIO data.           ",
"  When restoring data from an archive, bhprestore checks the available",
"  space on all the pathlist directories and uses the one with the ",
"  most space to load each partition.                              ",
"                                                                  ",
"  Error Conditions:                                               ",
"    Not enough disk space to load next partition.                 ",
"    Unable to read archive file or volume.                        ",
"    Multiple filename or pathlist entries specified. Only one     ",
"    dataset can be restored per job.                               ",
"    Specified filename cannot be found on archive.                ",
"    archive or filename unspecified.                              ",
"                                                                  ",
"  Examples:                                                       ",
"    bhprestore filename=stack archive=/dev/rmt0                   ",
"      stack.dat is assumed to exist in current directory.         ",
"    bhprestore toc=yes archive=/dev/rmt1                          ",
"      Table of contents is retrieved and listed.                  ",
"                                                                  ",
NULL};

int main(int argc, char **argv)
{

  char command[256];      /* unix commands */
  char *tmp;              /* temp file for TOC file */
  char *archive;          /* archive filename */
  char record[NAMELEN];   /* for scanning pathlist */
  char **paths;           /* list of paths for a given filename */
  char fpath[NAMELEN];    /* file_hdr /path/name */
  char *cwd;              /* current working dir */
  char *toc;              /* toc=yes --> print Table of Contents only */
  char *offline;          /* offline=yes, take device offline at end of restore, otherwise just rewind */
  char string[8];         /* scratch */

  cwp_String filename[NFILES];  /* list of filenames to restore */
  cwp_String pathlist[NFILES];  /* list of pathlists */

  int verbose;            /* debug printout */
  int i, j;               /* Loop counters */
  int npaths=0;           /* number of paths in current pathlist */
  int *hdrlist;           /* archive index of HDR files to load */
  int *datalist;          /* archive index of .su files to load */
  int nhf;                /* number of matching HDR files found in archive */
  int ndf;                /* number of matching .su files found in archive */
  int hdr1, data1;        /* location (1-based) in archive of 1st HDR file and first .su file */
  int gettoc;             /* =1 --> toc */

  float total;            /* total number of bytes to restore */

  FILE *fp;               /* restored partitions */
  FILE *pfp;              /* command pipe */

  arch_def *arch;         /* archive definition */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(0);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* toc only? */
  if(!getparstring("toc",&toc)) {
    toc = "no";
    gettoc = 0;
  }
  if(!strcmp(toc,"yes"))
    gettoc = 1;
  else
    gettoc = 0;

  /* offline after rewind? */
  if(!getparstring("offline",&offline))
    offline="yes";
  
  /* if not TOC-only */
  if(gettoc == 0) {
    i = countparval("filename");
    if(i == 0)
      err("filename parameter required\n");
    if(i > 1)
      err("Only 1 filename can be restored per job\n");
    getparstringarray("filename",filename);
    i = countparval("pathlist");
    if(i == 0) {
      for(i=0; i<1; i++) {
        pathlist[i] = calloc(strlen(filename[0])+5,sizeof(char));
        strcpy(pathlist[i],filename[i]);
        strcat(pathlist[i],".dat");
      }
    }
    else if(i > 1)
      err("number of pathlists must be same as number of filename\n");
    else
      getparstringarray("pathlist",pathlist);
  }

  if(!getparstring("archive",&tmp))
    err("archive file or device required\n");
  /* get full path to archive */
  if(!strncmp(tmp,"/",1)) {
    printf("archive is absolute\n");
    archive = calloc((int)strlen(tmp)+1,sizeof(char));
    strcpy(archive,tmp);
  }
  else {
    printf("archive is relative\n");
    cwd = getenv("PWD");
    if(verbose)
      printf("cwd=%s\n",cwd);
    i = (int)strlen(cwd);
    j = (int)strlen(tmp);
    archive = calloc(i+j+2,sizeof(char));
    strcpy(archive,cwd);
    strcat(archive,"/");
    strcat(archive,tmp);
  }
  if(gettoc == 1) {
    printf("Getting Table of Contents from %s\n",archive);
  }
  else {
    printf("Restoring from %s\n",archive);
    printf("Filenames and Pathlists:\n");
    for(i=0; i<1; i++)
      printf("  %s    %s\n",filename[i],pathlist[i]);
  }
  if(!strcmp(offline,"yes"))
    printf("Archive device will be taken offline after restore\n");
  else
    printf("Archive device will be rewound and left online after restore\n");

  /* get temp file to hold table of contents of restored volume */
  arch = calloc(1,sizeof(arch_def));
  tmp = calloc(L_tmpnam,sizeof(char));
  tmp = tmpnam(tmp);
  if(verbose)
    printf("tmp=%s\n",tmp);
  strcpy(command,"mkdir ");
  strcat(command,tmp);
  printf("Executing %s\n",command);
  pfp = popen(command,"w");
  pclose(pfp);
  get_filelist(archive,tmp,arch,cwd,verbose);
  for(i=0; i<arch->nfiles; i++)
    printf("Filename: %s, Number of Bytes: %d\n",arch->files[i],arch->size[i]);
  /* if TOC only, quit */
  if(gettoc == 1) {
    close_files(archive,NULL,offline,verbose);
    return EXIT_SUCCESS;
  }
  /* sum of all sizes */
  total = 0;
  for(i=0; i<arch->nfiles; i++)
    total += arch->size[i];
  printf("Total of %.0f bytes needed to restore all files on archive\n",total);
  hdrlist = calloc(arch->nfiles,sizeof(int));
  datalist = calloc(arch->nfiles,sizeof(int));
  /* get all available paths */
  fp = fopen(pathlist[0],"r");
  if(fp == NULL)
    err("Failed to open %s for reading\n",pathlist[0]);
  if(verbose)
    printf("Opened %s for reading\n",pathlist[0]);
  while(fgets(record,255,fp) != NULL)
    npaths++;
  rewind(fp);
  paths = calloc(npaths,sizeof(char *));
  for(i=0; i<npaths; i++)
    paths[i] = calloc(NAMELEN,sizeof(char));
  for(i=0; i<npaths; i++)
    fscanf(fp,"%s",paths[i]);
  printf("Available paths:\n");
  for(i=0; i<npaths; i++)
    printf("  %s\n",paths[i]);

  /* look for requested files in archive, starting with HDRs */
  nhf = 0;
  ndf = 0;
  find_hdr_file(filename[0],"_0000.HDR",arch,&nhf,&hdr1,hdrlist,verbose);
  find_hdr_file(filename[0],"_0000.0001.HDR",arch,&nhf,&hdr1,hdrlist,verbose);
  find_hdr_file(filename[0],"_0000.0002.HDR",arch,&nhf,&hdr1,hdrlist,verbose);
  find_hdr_file(filename[0],"_LOCK.HDR",arch,&nhf,&hdr1,hdrlist,verbose);
  /* find all data partitions for current filename */
  find_data_files(filename[0],arch,&ndf,&data1,datalist,verbose);
  printf("Found %d files\n",nhf+ndf); 
  printf("First Header File is at %d, and First Data File is at %d\n",hdr1,data1);
  for(i=0; i<arch->nfiles; i++) {
    if(hdrlist[i] != 0)
      printf("%s at file %d\n",arch->files[i],hdrlist[i]);
  }
  for(i=0; i<arch->nfiles; i++) {
    if(datalist[i] != 0)
      printf("%s at file %d\n",arch->files[i],datalist[i]);
  }

  /* if no files found, quit */
  if(nhf + ndf == 0) {
    printf("No matching files found on archive, quitting\n");
    err("No matching files found\n");
  }
  /* extract all HDR files to first partititon in list */
  strcpy(command,"cd ");
  strcat(command,paths[0]);
  strcat(command," ; mt -f ");
  strcat(command,archive);
  strcat(command," fsf ");
  sprintf(string,"%d",hdr1-1);
  strcat(command,string);
  for(i=0; i<arch->nfiles; i++) {
    if(hdrlist[i] != 0) {
      strcat(command," ; tar xvf ");
      strcat(command,archive);
    }
  }
  if(nhf > 0) {
    printf("Executing %s\n",command);
    pfp = popen(command,"w");
    pclose(pfp);
  }
  else
    printf("No HDR files on this archive\n");
    
  /* get file header */
  if(!get_bhpio_path(paths[0],filename[0],"_0000.HDR",fpath,verbose))
    err("Can't open file header, load HDR files first\n");
  file_hdr.filename = calloc(NAMELEN,sizeof(char));
  tmp = (char *)realloc(tmp,NAMELEN);
  read_file_hdr(fpath,verbose);
  /* set npaths in file_hdr to number in pathlist */
  file_hdr.npaths = npaths;
  strcpy(command,"mt -f ");
  strcat(command,archive);
  strcat(command," rewind ");
  strcat(command," ; mt -f ");
  strcat(command,archive);
  strcat(command," fsf ");
  sprintf(string,"%d",data1);
  strcat(command,string);     
  printf("Executing %s\n",command);
  pfp = popen(command,"w");
  pclose(pfp);
  /* extract each data file to path with most space */
  for(i=0; i<arch->nfiles; i++) {
    if(datalist[i] != 0) {
      j = get_path(paths,npaths,arch->size[i],verbose);
      printf("Extracting to %s\n",paths[j]);
      strcpy(command,"cd ");
      strcat(command,paths[j]);
      strcat(command," ; tar xvf ");
      strcat(command,archive);
      printf("Executing %s\n",command);
      pfp = popen(command,"w");
      pclose(pfp);
      update_part_list(filesys,file_hdr.nparts,tmp,paths[j],arch->files[i],verbose);
    }
  }

  /* save updated file header */
  write_file_hdr(fpath,verbose);

  close_files(archive,fp,offline,verbose);
  return EXIT_SUCCESS;

}

void get_filelist(char *archive, char *tmp, arch_def *arch, char *cwd, int verbose)
{

  char command[256];
  char toc[256];
  char record[256];
  char s1[16],s2[16];

  FILE *pfp;
  FILE *tfp;
 
  int i;

  /* extract toc and count files */
  strcpy(command," cd ");
  strcat(command,tmp);
  strcat(command," ; ");
  strcat(command,"tar xvf ");
  strcat(command,archive);
  strcat(command," Table_of_Contents");
  printf("Executing %s\n",command);
  pfp = popen(command,"w");
  pclose(pfp);
  strcpy(command,tmp);
  strcat(command,"/Table_of_Contents");
  tfp = fopen(command,"r");
  if(tfp == NULL)
    err("Failed to open %s for reading\n",command);
  printf("Opened %s for reading\n",command);
  while(fgets(record,255,tfp) != NULL)
    arch->nfiles++;
  printf("Found %d files on archive\n",arch->nfiles);
  rewind(tfp);
  arch->files = calloc(arch->nfiles,sizeof(char *));
  arch->size = calloc(arch->nfiles,sizeof(int));
  for(i=0; i<arch->nfiles; i++)
    arch->files[i] = calloc(NAMELEN,sizeof(char));
  for(i=0; i<arch->nfiles; i++) {
    fscanf(tfp,"%s %s %d %s\n",arch->files[i],s1,&arch->size[i],s2);
    arch->files[i] = basename(arch->files[i]);
  }

}
void find_hdr_file(char *file, char *string, arch_def *arch, int *nf, int *hdr1, int *list, int verbose)
{

  int i;
  
  char name[256];

  strcpy(name,file);
  strcat(name,string);
  for(i=0; i<arch->nfiles; i++) {
    if(!strcmp(name,arch->files[i])) {
      list[i] = i + 1;
      if(*nf == 0)
        *hdr1 = i + 1;
      (*nf)++;
    }
  }

}
void find_data_files(char *file, arch_def *arch, int *nf, int *data1, int *list, int verbose)
{

  char name[NAMELEN];

  int i, n;
  int len;

  /* loop over loclist, check each file not already on list to see if it is 'filename_nnnn.su' */
  for(i=0; i<arch->nfiles; i++) {
    if(list[i] == 0) {
      /* num chars is filename+8 */
      len = (int)strlen(file);
      if((int)strlen(arch->files[i]) == len + 8) {
        /*if(verbose)
          printf("Length match at %d\n",i);*/
        strncpy(name,arch->files[i],len);
        name[len] = '\0';
        /* name part matches */
        if(!strcmp(file,name)) {
          /*if(verbose)
            printf("name match at %d\n",i);*/
          /* next char is '_' */
          if(arch->files[i][len] == '_') {
            /*if(verbose)
              printf("name match plus '_' at %d\n",i);*/
            /* last 3 chars are '.su' */
            len = (int)strlen(arch->files[i]) - 3;
            strncpy(name,&arch->files[i][len],3);
            name[3] = '\0';
            if(!strcmp(name,".su")) {
              /*if(verbose)
                printf("last 3 chars = %s at %d\n",name,i);*/
              /* remaining chars are 'nnnn' */
              strncpy(name,&arch->files[i][len-4],4);
              name[4] = '\0';
              n = atoi(name);
              if(n >= 1 && n <= 9999) {
                /*if(verbose)
                  printf("nnnn=%d at %d\n",n,i);*/
                list[i] = i + 1;
                if(*nf == 0)
                  *data1 = i + 1;
                (*nf)++;
              }
            }
          }
        }
      }
    }
  }

}
int get_path(char **paths, int npaths, int size, int verbose)
{

  float mb_avail;
  float most;

  int index;
  int i;
  int stat;

  struct statvfs buf;

  for(i=0; i<npaths; i++) {
    if(verbose)
      printf("Checking %s\n",paths[i]);
    stat = statvfs(paths[i],&buf);
    if(stat != 0)
      err("Could not get status for %s\n",paths[i]);
    mb_avail = ((float)buf.f_frsize * (float)buf.f_bavail) / (1024. * 1024.);
    if(mb_avail > most) {
      most = mb_avail;
      index = i;
    }
  }

  if(most < ((float)size / 1024. / 1024.))
    err("Not enough space for next partition\n");

  if(verbose)
    printf("Using %s with %f MB available\n",paths[index],most);

  return index;

}
void update_part_list(char *filesys, int nparts, char *tmp, char *path, char *file, int verbose)
{

  int i;

  /* find file in list */
  for(i=0; i<nparts; i++) {
    tmp = basename(&filesys[i*NAMELEN]);
    if(!strcmp(file,tmp)) {
      if(verbose)
        printf("Found %s at part %d\n",file,i+1);
      strcpy(&filesys[i*NAMELEN],path);
      strcat(&filesys[i*NAMELEN],"/");
      strcat(&filesys[i*NAMELEN],file);
    }  
  }
}
void close_files(char *archive, FILE *fp, char *offline, int verbose)
{

  char command[256];      /* unix commands */

  FILE *pfp;              /* command pipe */
  
  struct stat statbuf;

  /* close pathlist file if open */
  if(fp != NULL)
    fclose(fp);

  /* rewind or rewoffl if archive is tape */
  if(stat(archive,&statbuf) != 0) {
    /* rewind or rewind and offline */
    strcpy(command,"mt -f ");
    strcat(command,archive);
    if(!strcmp(offline,"yes"))
      strcat(command," rewoffl");
    else
      strcat(command," rewind");
    printf("Executing %s\n",command);
    pfp = popen(command,"w");
    pclose(pfp);
  }

}
