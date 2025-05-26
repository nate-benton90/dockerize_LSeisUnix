/*
 * bal - Trace balance
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
static char lSccsId[]="@(#)bal.c	1.4\t11/15/88\n";


static char *lsdoc = 
"subal [-v] <stdin >stdout		\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
						\n\
						\n";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
        SccsId = lSccsId;


	/* GET OPTIONS */
	while( (c=getopt(xargc,xargv,"v"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
}

 
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}
 
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
Subhed *abh;
{
	dobal( abh->ns, atr->data);
	return(1);
}

dobal(n,x)
int n;
float *x;
{
	int j;
	float max;

	for(j=0,max=0.0;j<n;j++) {
		max = MAX(max,x[j]);
		max = MAX(max,-x[j]);
	}

	if(max!=0.0) max = 1.0/max;

	for(j=0;j<n;j++) {
		x[j] *= max;
	}
}

postp(){}
