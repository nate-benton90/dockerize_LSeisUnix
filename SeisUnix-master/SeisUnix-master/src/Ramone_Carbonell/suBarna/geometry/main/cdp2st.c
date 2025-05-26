/* Copyright (c) Colorado School of Mines, 2001.*/
/* All rights reserved.                       */

/* CRKCMP: $Revision: 1.13 $ ; $Date: 1996/09/18 19:04:39 $	*/

#include "su.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 									",
" CDP2ST - Calculate the station number that is the closest to the CDP  ",
" 									",
" crkcmp cmp.asc st.asc >stdout                                  	",
" 									",
" Required parameters:							",
" 	cmpfl.asc  File containing the y,x coordinates of the crooked   ",
"                   line CMP's                                          ",
" 	stfl.asc File containing the coordinats station location        ",
"                   crooked line CMP's                                  ",
" 									",
NULL};

/* Credits:
 *	CSIC-ICTJA: Ramon  Carbonell
 *
 */
/**************** end self doc ***********************************/

int
main(int argc, char **argv)
{
   FILE *fp0;		            /* file pointer for first file	*/
   int nst,ncmp,iread=1,*icmp,*ist; /* number of sample points on traces*/
   int icnt=0,i,j,itmp,*icmpst;     /* number of trace being processed	*/
   float *cmpx,*cmpy,d,xmn;	    /* number of trace being processed	*/
   float *stx,*sty,ftmp,xtmp,ytmp;  /* number of trace being processed	*/

   /* Initialize */
    initargs(argc, argv);
    requestdoc(2); /* two file args required */

   /* Open two files given as arguments for reading */
    fp0 = fopen(argv[1], "r");
    while (iread > 0) {
           iread=fscanf(fp0," %i  %f  %f\n",&itmp,&ftmp,&ftmp);
           icnt++;
    }
    nst=icnt-1;
    rewind(fp0);
    ist=  (int*)malloc(nst*sizeof(int));
    stx=(float*)malloc(nst*sizeof(float));
    sty=(float*)malloc(nst*sizeof(float));
    iread=1;
    for (i=0;i<nst;i++) iread=fscanf(fp0," %i  %f  %f \n",&ist[i],&stx[i],&sty[i]);
    fclose(fp0);
    fp0 = fopen(argv[2], "r");
    while (iread > 0) {
           iread=fscanf(fp0," %i  %f  %f \n",&itmp,&ftmp,&ftmp);
           icnt++;
    }
    ncmp=icnt-1;
    rewind(fp0);
    icmpst=(int*)malloc(ncmp*sizeof(int));
    icmp=(int*)malloc(ncmp*sizeof(int));
    cmpx=(float*)malloc(ncmp*sizeof(float));
    cmpy=(float*)malloc(ncmp*sizeof(float));
    iread=1;
    for (i=0;i<ncmp;i++) iread=fscanf(fp0," %i  %f  %f\n",&icmp[i],&cmpx[i],&cmpy[i]);
    fclose(fp0);
    for (i=0;i<ncmp;i++) {
	 xtmp=stx[0]-cmpx[i];
	 ytmp=sty[0]-cmpy[i];
	 xmn=sqrt(xtmp*xtmp+ytmp*ytmp);
         for (j=0;i<nst;i++) {
	      xtmp=stx[j]-cmpx[i];
	      ytmp=sty[j]-cmpy[i];
	      d=sqrt(xtmp*xtmp+ytmp*ytmp);
	      if (d < xmn ) {
		 xmn=d;
                 icmpst[i]=ist[j];
              }
         }
    }
   for (i=0;i<ncmp;i++) {
        fprintf(stdout,"%i  %i\n",icmp[i],icmpst[i]);
   }
   fclose(fp0);
   return EXIT_SUCCESS;
}
