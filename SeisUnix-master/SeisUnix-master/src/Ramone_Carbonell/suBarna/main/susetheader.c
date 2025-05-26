/* Copyright (c) 2001.*/
/* All rights reserved.                       */

/* SUSETHEADER: $Date: 2001/06/9 22:36:46 $		*/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									       ",
" SUSETHEADER - sets the value of a headerword  (key2) if the headers key1 and ",
"               key2 correspods to the specified in the file                   ",
" 									       ",
"  susetheader <stdin >stdout key0=fldr key1=tracf key2=cdp infile=ascfl       ",
" 									       ",
" infile= 	ascii file of header  values 	                               ",
" 						                               ",
"  hey0  key1       key2                                                       ",
"   21     22       130                                                        ",
"   22     23       140                                                        ",
"   24     25       140                                                        ",
"  Notes:   This programs works like:                                          ",
"           If the incoming trace hase the header valuess of key0 and key1     ",
"           equal  to the ones specified in the infile sets key2 to the value  ",
"           specified in the file                                              ",
NULL}; 

/* Credits:
 *	Institute of Earth Sciences Jaume Almera: Ramon Carbonell
 *      CSIC: 
 */
/**************** end self doc ****************************************/

segy tr;
void setval(cwp_String type, Value *valp, double a);

int
main(int argc, char **argv)
{
        int n,i,icnt=0,iread=1,itmp,isht,ichn;	
        int *ffid,*itrc,*ihd;
	double a;
        cwp_String key0[SU_NKEYS];      /* array of keywords */
        cwp_String key1[SU_NKEYS];      /* array of keywords */
        cwp_String key2[SU_NKEYS];      /* array of keywords */
        cwp_String typ0[SU_NKEYS];     /* array of keywords */
        cwp_String typ1[SU_NKEYS];     /* array of keywords */
        cwp_String typ2[SU_NKEYS];     /* array of keywords */
        int indx0[SU_NKEYS];           /* name of type of getparred key */
        int indx1[SU_NKEYS];           /* name of type of getparred key */
        int indx2[SU_NKEYS];           /* name of type of getparred key */
        Value val0,val1,val;                     /* value of key field */
        char *infile="";	/* name of input file of header values	*/
        FILE *infp=NULL;	/* pointer to input file		*/

        /* Initialize */
        initargs(argc, argv);
        requestdoc(1);
        /* get name of infile */  
        if (!getparstringarray("key0",key0)) key0[0]="fldr";
        if (!getparstringarray("key1",key1)) key1[0]="tracf";
        if (!getparstringarray("key2",key2)) key2[0]="cdp";
        getparstring("infile",&infile);
        warn (" File -> %s", infile);
        if (!fgettr(stdin, &tr)) err("can't read first trace");
        if ((infp=fopen(infile,"r"))==NULL) err("cannot open infile=%s",infile);
        while (iread > 0) {			
               iread=fscanf(infp," %i  %i  %i \n",&itmp,&itmp,&itmp);
               icnt++;
        }
        rewind(infp);
        n=icnt-1;
        ffid=(int*)malloc(n*sizeof(int));
        itrc=(int*)malloc(n*sizeof(int));
        ihd=(int*)malloc(n*sizeof(int));
        typ0[0]=hdtype(key0[0]);
        indx0[0]=getindex(key0[0]);              
        typ1[0]=hdtype(key1[0]);
        indx1[0]=getindex(key1[0]);
        typ2[0]=hdtype(key2[0]);
        indx2[0]=getindex(key2[0]);
	iread=1;
        for (i=0;i<n;i++) iread=fscanf(infp," %i   %i   %i \n",&ffid[i],&itrc[i],&ihd[i]);
        fclose(infp);
        /* loop over traces */
        do {              
              gethval(&tr, indx0[0], &val0);
              gethval(&tr, indx1[0], &val1);
              isht  = (int) vtof(hdtype(key0[0]), val0);
              ichn  = (int) vtof(hdtype(key1[0]), val1); 
              for (i=0; i<n; i++) {
                   if ((isht == ffid[i] ) && (ichn == itrc[i]))  {
		        a=(double) ihd[i];	  
                        setval(typ2[0],&val,a);
                        puthval(&tr,indx2[0],&val);
                   }
              }
              puttr(&tr);
        } while (gettr(&tr)); 
        return EXIT_SUCCESS;
}

void setval( cwp_String type, Value *valp, double a)
{
   switch (*type) {
   case 's':
         err("can't set char header word");
   break;
   case 'h':
         valp->h = a;
   break;
   case 'u':
         valp->u = a;
   break;
   case 'l':
         valp->l = (long) (a);
   break;
   case 'v':
         valp->v = (unsigned long) (a);
   break;
   case 'i':
         valp->i = a;
   break;
   case 'p':
         valp->p = a;
   break;
   case 'f':
         valp->f = a;
   break;
   case 'd':
         valp->d = a;
   default:
         err("unknown type %s", type);
   break;
   }
   return;
}

