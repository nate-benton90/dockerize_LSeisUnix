/* sudummy.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
char *sdoc[] = {" SUDUMMY - zero out traces or set trid flag if traces are all zero",
" sudummy < infile > outfile                             		",
" 									",
" Optional parameters          						",
"									",
" cdp=0		If set other than zero, it dumps the cdp # of live traces",
"               to stderr						",
" 									",
" zero=0        =1 it sets the trace values to zero if trid > 1		",
" 									",
" It set trace header word trid to 3 if all the values in the trace	",
" are zero.								",
" 									",
" 									",
" 									",
NULL};
   
/* Segy data constans */
segy tr;				/* SEGY trace */

int main( int argc, char *argv[] )
{
	/* Segy data constans */
	segy tr;				/* SEGY trace */
	int nt;                 /* number of time samples               */
        int ntr=0;                /* number of traces                     */
	int i,flag,lt=0;	/* counter */
	int cdp=0;
	int zero=0;

	initargs(argc, argv);
   	requestdoc(1);
	
        /* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
        nt = tr.ns;

	if(!getparint("cdp",&cdp)) cdp=0;
	if(!getparint("zero",&zero)) zero=0;
	
	do {
		ntr++;
		flag=0;
	
		if(zero) {
			if(tr.trid>1) for(i=0;i<tr.ns;i++) tr.data[i]=0.0;
		} else {
			for(i=0;i<nt;i++) {
				if(tr.data[i]!=0.0) {
					flag=1;
					lt++;
				}
				if(flag==1) break;
			}
			if(flag==0) {
				tr.trid=3;
			} else {
				if(cdp!=0) fprintf(stderr," %d\n",tr.cdp);
			}
		}
		puttr(&tr);
	} while(gettr(&tr));
	
	fprintf(stderr,"Number of live traces/total traces= %d/%d\n",lt,ntr); 	
   return EXIT_SUCCESS;
}
