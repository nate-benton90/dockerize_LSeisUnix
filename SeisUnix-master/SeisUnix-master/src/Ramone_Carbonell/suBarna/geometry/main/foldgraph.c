/* Copyright (c) 2001.*/
/* All rights reserved.                       */

/* MATFILL: $Date: 2001/06/9 22:36:46 $		*/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									   ",
" foldgraph - Fills a matrix with numbers acording to the mid point        ",
"             location  file                                               ",
" 									   ",
"   foldgraph n1=1000 n2=1000 > stdout  infile=midpoints-xy                ",
" 									   ",
" infile=  ascii file with the x, y coordinates of mid points              ",
" 						                           ",
"                                                                          ",
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
	int n1,n2,n,i,j,k=0,icnt=0,iread=1,itmp,jtmp;	
        float *z,d1,d2,xtmp,ytmp;
        float xmn=0,xmx=0,ymn=0,ymx=0;
	char *infile="";	/* name of input file of header values	*/
	FILE *infp=NULL;	/* pointer to input file		*/

	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);
        /* get name of infile */
        if (!getparint("n1", &n1))       n1=1000;
        if (!getparint("n2", &n2))       n2=1000;
					 
        getparstring("infile",&infile);
        warn (" File -> %s", infile);
        if((infp=fopen(infile,"r"))==NULL) err("cannot open infile=%s\n",infile);
        while (iread > 0) {			
              iread=fscanf(infp,"%i %i %f %f\n",&itmp,&jtmp,&xtmp,&ytmp);
	      if (!icnt) { 
		      xmn=xtmp;
		      xmx=xtmp;
		      ymn=ytmp;
		      ymx=ytmp;
              }
	      if (xtmp <= xmn) xmn=xtmp;
	      if (xtmp >= xmx) xmx=xtmp;
	      if (ytmp <= ymn) ymn=ytmp;
	      if (ytmp >= ymx) ymx=ytmp;
              icnt++;
        }
        rewind(infp);
        n=icnt-1;
        z=(float*)malloc(n1*n2*sizeof(float));
        for (j=0;j<n1*n2;j++) z[j]=0;
        d2=(xmx-xmn)/(n2-1);
        d1=(ymx-ymn)/(n1-1);
        for (i=0;i<n;i++) {
            iread=fscanf(infp,"%i  %i  %f  %f\n",&itmp,&jtmp,&xtmp,&ytmp);
            k=(int)(n1*((int)((xtmp-xmn)/d2)) + (int)((ytmp-ymn)/d1));
	    z[k]++;
        }
	fclose(infp);
        if (fwrite(z,sizeof(float),n1*n2,stdout) !=n1*n2) err("error writing output file!");
	warn("n1=%i  d1=%f  f1=%f ",n1,d1,ymn);
	warn("n2=%i  d2=%f  f2=%f ",n2,d2,xmn);
	warn("** Ximage Usage: ");
        if((infp=fopen("ximageCDPScatter.par","w"))==NULL) err("cannot open Parameter file\n");
	fprintf(infp,"%i %f %f  %i %f %f \n",n1,d1,ymn,n2,d2,xmn);
	fclose(infp);
	/* loop over cdps (ie. traces)  */
       	return EXIT_SUCCESS; 
}
