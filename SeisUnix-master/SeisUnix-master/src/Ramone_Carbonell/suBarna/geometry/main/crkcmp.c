/* Copyright (c) Colorado School of Mines, 2001.*/
/* All rights reserved.                       */

/* CRKCMP: $Revision: 1.13 $ ; $Date: 1996/09/18 19:04:39 $	*/

#include "su.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 									",
" CRKCMP - Comput coordinates of CMP for crooked line processing        ",
" 									",
" crkcmp pckcmp.asc rmp.asc dcmp=17.5 cmp0=100 cmpfl=CMPxy.asc >stdout 	",
" 									",
" Required parameters:							",
" 	pckcmp.asc  File containing the y,x coordinates of the crooked  ",
"                   line CMP's                                          ",
" 	rmp.asc  File containing the coordinats the real cmp points     ",
"       dcmp=17.5  distance between cdp's 				",
"       cmp0=100   Starting cmp number   				",
" 	cmpfl=CMPxy.asc File containing the y,x coordinates of the      ",
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
   FILE *fp0;		/* file pointer for first file		*/
   int n0,iread=1,cmp0;	/* number of sample points on traces	*/
   int icnt=0,i,j,*np,ncmp;  /* number of trace being processed	*/
   int itmp,*ffid,*itr,*icmp;	/* number of trace being processed	*/
   float ftmp,*x0,*y0,dcmp;     /* number of trace being processed      */
   float *cmprx,*cmpry,d,xmn=0;	/* number of trace being processed	*/
   float *cmpx,*cmpy,*d0,*cita;	/* number of trace being processed	*/
   float xtmp,ytmp,dtot,tmp;	/* number of trace being processed	*/
   char *cmpfl="CMPxy.asc";     /* name of input file of header values  */

   /* Initialize */
    initargs(argc, argv);
    requestdoc(2); /* two file args required */
    if (!getparfloat("dcmp" , &dcmp)) dcmp=17.5;
    if (!getparint("cmp0" , &cmp0)) cmp0=100;
    if (!getparstring("cmpfl",&cmpfl)) cmpfl="CMPxy.asc";

   /* Open two files given as arguments for reading */
    fp0 = fopen(argv[1], "r");
    while (iread > 0) {
           iread=fscanf(fp0," %f  %f \n",&ftmp,&ftmp);
           icnt++;
    }
    n0=icnt-1;
    rewind(fp0);
    x0=(float*)malloc(n0*sizeof(float));
    y0=(float*)malloc(n0*sizeof(float));
    d0=(float*)malloc(n0*sizeof(float));
    cita=(float*)malloc(n0*sizeof(float));
    np=(int*)malloc(n0*sizeof(int));
    iread=1;
    dtot=0;
    for (i=0;i<n0;i++) iread=fscanf(fp0," %f   %f \n",&x0[i],&y0[i]);
    for (i=0;i<n0-1;i++) {
         xtmp=x0[i+1]-x0[i];
         ytmp=y0[i+1]-y0[i];
         d0[i]=sqrt(xtmp*xtmp+ytmp*ytmp);
         cita[i]=atan2((double)(y0[i+1]-y0[i]),(double)(x0[i+1]-x0[i]));
         dtot=dtot+d0[i]; 
    }
    ncmp=dtot/dcmp;
    cmpx=(float*)malloc(ncmp*sizeof(float));
    cmpy=(float*)malloc(ncmp*sizeof(float));
    cmpx[0]=x0[0];
    cmpy[0]=y0[0];
    j=1;
    i=0;
    d=d0[0];
    do {
        xtmp=dcmp*cos(cita[i]);
        ytmp=dcmp*sin(cita[i]);
        cmpx[j]=cmpx[j-1]+xtmp;
        cmpy[j]=cmpy[j-1]+ytmp;
        d=d-sqrt(xtmp*xtmp+ytmp*ytmp); 
        if (d < 0) {
            cmpx[j]=cmpx[j-1]+(dcmp+d)*cos(cita[i+1])+((fabs)(d))*cos(cita[i]);
            cmpy[j]=cmpy[j-1]+(dcmp+d)*sin(cita[i+1])+((fabs)(d))*sin(cita[i]);
            i++;
            d=d0[i];
        }    
        j++;
    } while (j<ncmp);
   fclose(fp0);
   if((fp0=efopen(cmpfl,"w"))==NULL) err("cannot open cmpfl=%s\n",cmpfl);
   for (i=0;i<ncmp;i++) {
        fprintf(fp0,"%i  %f  %f\n",100+i,cmpx[i],cmpy[i]);
   }
   fclose(fp0);
   iread=1;
   fp0 = fopen(argv[2], "r");
   icnt=0;
   while (iread > 0) {
          iread=fscanf(fp0," %i %i %f %f \n",&itmp,&itmp,&ftmp,&ftmp);
           icnt++;
   }
   n0=icnt-1;
   rewind(fp0);
   ffid=(int*)malloc(n0*sizeof(int));
   itr=(int*)malloc(n0*sizeof(int));
   icmp=(int*)malloc(n0*sizeof(int));
   cmprx=(float*)malloc(n0*sizeof(float));
   cmpry=(float*)malloc(n0*sizeof(float));
   for (i=0;i<n0;i++) {
        iread=fscanf(fp0," %i %i %f %f \n",&ffid[i],&itr[i],&cmpry[i],&cmprx[i]);
        for (j=0;j<ncmp; j++) {
             xtmp=cmpx[j]-cmprx[i]; 
             ytmp=cmpy[j]-cmpry[i]; 
             tmp=sqrt(xtmp*xtmp+ytmp*ytmp);
             if (!j) xmn=tmp;
             if ( j) {
                   if (tmp < xmn) {
                       xmn=tmp;
                       icmp[i]=j;
                   }
             }
       }
   }
   fclose(fp0);
   for (i=0;i<n0;i++) {
        fprintf(stdout," %i  %i  %f  %f  %i  %f  %f\n",ffid[i],itr[i],cmpry[i],cmprx[i],100+icmp[i],cmpy[icmp[i]],cmpx[icmp[i]]);
   }
   return EXIT_SUCCESS;
}
