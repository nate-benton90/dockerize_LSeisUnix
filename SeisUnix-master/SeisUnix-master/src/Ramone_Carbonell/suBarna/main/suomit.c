/* Copyright (c) 2001.*/
/* All rights reserved.                       */

/* SUSETGEOM: $Date: 2001/06/9 22:36:46 $		*/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									       ",
" SUOMIT - omit traces from the file                                           ",
" 									       ",
" suomit <stdin >stdout key0=fldr key1=tracf iomit=0 infile=ascii_file         ",
" 									       ",
" iomit=0       0=omit traces  1=pass traces                                   ",
" infile= 	ascii file of values 	with the following format	       ",
" 						                               ",
"  hey1  key2  num traces                                    ",
"   21     22       130                             ",
"   22     23       140                                       ",
"   24     25       140                                      ",
NULL}; 

/* Credits:
 *	Institute of Earth Sciences Jaume Almera: Ramon Carbonell
 *      CSIC: 
 */
/**************** end self doc ****************************************/

segy tr;

void 
main(int argc, char **argv)
{
        int n,i,j,icnt=0,iread=1,itmp,isht,ichn;	
        int fl,*ffid,*itrc,*ntrc,iomit=0;
        cwp_String key0[SU_NKEYS];      /* array of keywords */
        cwp_String key1[SU_NKEYS];      /* array of keywords */
        cwp_String typ0[SU_NKEYS];     /* array of keywords */
        cwp_String typ1[SU_NKEYS];     /* array of keywords */
        int indx0[SU_NKEYS];           /* name of type of getparred key */
        int indx1[SU_NKEYS];           /* name of type of getparred key */
        Value val0,val1;                     /* value of key field */
        char *infile="";	/* name of input file of header values	*/
        FILE *infp=NULL;	/* pointer to input file		*/

        /* Initialize */
        initargs(argc, argv);
        requestdoc(1);
        /* get name of infile */  
        if (!getparstringarray("key0",key0)) key0[0]="fldr";
        if (!getparstringarray("key1",key1)) key1[0]="tracf";
        if (!getparint("iomit",&iomit)) iomit=0;
        getparstring("infile",&infile);
        warn (" File -> %s", infile);
        if (!fgettr(stdin, &tr)) err("can't read first trace");
        if((infp=fopen(infile,"r"))==NULL) {
           warn("cannot open infile=%s",infile);
           warn("No trace edits required");
           do {              
               puttr(&tr);
           } while (gettr(&tr)); 
         }
         else {
           while (iread > 0) {			
                 iread=fscanf(infp," %i  %i  %i \n",&itmp,&itmp,&itmp);
                 icnt++;
           }
           rewind(infp);
           n=icnt-1;
           ffid=(int*)malloc(n*sizeof(int));
           itrc=(int*)malloc(n*sizeof(int));
           ntrc=(int*)malloc(n*sizeof(int));
           typ0[0]=hdtype(key0[0]);
           indx0[0]=getindex(key0[0]);              
           typ1[0]=hdtype(key1[0]);
           indx1[0]=getindex(key1[0]);
           for (i=0;i<n;i++) iread=fscanf(infp," %i   %i   %i \n",&ffid[i],&itrc[i],&ntrc[i]);
           fclose(infp);
           /* loop over traces */
	   if (!iomit) { 
              do {              
                    gethval(&tr, indx0[0], &val0);
                    gethval(&tr, indx1[0], &val1);
                    isht = (int) vtof(hdtype(key0[0]), val0);
                    ichn = (int) vtof(hdtype(key1[0]), val1); 
                    fl=0;
                    for (i=0; i<n; i++) {
                         if (isht == ffid[i] ) {
                              for (j=0;j<ntrc[i];j++) {
                                  if (ichn == itrc[i]+j)  fl=1;
                              }
                          }
                    }
                    if (!fl) puttr(&tr);
              } while (gettr(&tr)); 
           }
	   if (iomit) { 
              do {              
                    gethval(&tr, indx0[0], &val0);
                    gethval(&tr, indx1[0], &val1);
                    isht = (int) vtof(hdtype(key0[0]), val0);
                    ichn = (int) vtof(hdtype(key1[0]), val1); 
                    fl=1;
                    for (i=0; i<n; i++) {
                         if (isht == ffid[i] ) {
                              for (j=0;j<ntrc[i];j++) {
                                  if (ichn == itrc[i]+j)  fl=0;
                              }
                          }
                    }
                    if (!fl) puttr(&tr);
              } while (gettr(&tr)); 
           }
      }
}
