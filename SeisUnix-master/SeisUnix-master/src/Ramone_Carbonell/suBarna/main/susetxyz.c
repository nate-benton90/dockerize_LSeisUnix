/* Copyright (c) 2001.*/
/* All rights reserved.                       */

/* SUSETGEOM: $Date: 2001/06/9 22:36:46 $		*/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									   ",
" SUSETGEOM - Incorporates to the headers the coordinates for the source   ",
"	 and the receiver station location.                                ",
" 									   ",
"   sushw <stdin > stdout     infile=ascii_file	                      	   ",
" 									   ",
" infile= 	ascii file of values 	with the following format	   ",
" 						                           ",
"                                                                          ",
" station number       x            y          z                           ",
"        1     647346.81250    4123017.25000  51.66930                     ",
"        2     647358.03750    4123050.00000  52.23436                     ",
NULL}; 

/* Credits:
 *	Institute of Earth Sciences Jaume Almera: Ramon Carbonell
 *      CSIC: 
 */
/**************** end self doc ****************************************/

segy tr;

int
main(int argc, char **argv)
{
	int n,*ist,i,icnt=0,iread=1,itmp;	
        float *x,*y,*z,tmpx,tmpy,tmpz;
	char *infile="";	/* name of input file of header values	*/
	FILE *infp=NULL;	/* pointer to input file		*/

	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);
        /* get name of infile */
        getparstring("infile",&infile);
        warn (" File -> %s", infile);
        if((infp=fopen(infile,"r"))==NULL) err("cannot open infile=%s\n",infile);
        while (iread > 0) {			
              iread=fscanf(infp,"%i %f %f %f\n",&itmp,&tmpx,&tmpy,&tmpz);
              icnt++;
        }
        rewind(infp);
        n=icnt;
        ist=(int*)malloc(n*sizeof(int));
        x=(float*)malloc(n*sizeof(float));
        y=(float*)malloc(n*sizeof(float));
        z=(float*)malloc(n*sizeof(float));
        for (i=0;i<n;i++) {
            iread=fscanf(infp,"%i %f %f %f\n",&ist[i],&x[i],&y[i],&z[i]);
        }
	/* loop over traces */
	while (gettr(&tr)) {
/*      
             warn("Source Station -> %i %i\n",tr.nstatsrc,n);
             warn("Receiver Station -> %i \n",tr.nstatrec);        */
              for (i=0;i<n;i++) {
                   if (tr.nstatsrc==ist[i]) {
                       tr.selev=z[i];
                       tr.sx=x[i];
                       tr.sy=y[i];
                   }
                   if (tr.nstatrec==ist[i]) {
                       tr.gelev=z[i];
                       tr.gx=x[i];
                       tr.gy=y[i];
                   }
              }
              puttr(&tr);
        }
       	return EXIT_SUCCESS; 
}
