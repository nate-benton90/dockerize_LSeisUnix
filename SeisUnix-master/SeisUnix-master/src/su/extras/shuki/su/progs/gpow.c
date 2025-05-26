/*
 * gpow - Gain by taking the trace at power gpow
 */
#include <stdio.h>
#include <math.h>
#include "../include/su.h"
extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
 
static char lSccsId[]="@(#)gpow.c	1.4\t11/15/88\n";

static float gpow;
static char *lsdoc = 
      "sugpow gpow= [-v] <stdin >stdout		\n\
	out(t) = in(t)**gpow            	\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
						\n\
PARAMETERS:					\n\
	gpow=					\n\
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

	/* GET PARAMETERS */
	if(!fgetpar("gpow",&gpow)) {
		warn(__FILE__,__LINE__,"Must specify gpow");
		selfdoc();
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
 
/*
 *    out(j)=inp(j)**gpow
 *
 *   
 *     itr -number of trace
 *     atr - structure segy_tr
 *     abh - structure segy_bh
 *     nsample  - number of samples
 *     ibuf      - data of the trace
 *     
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
Subhed *abh;
{
	dogpow(gpow,abh->ns,atr-> data);
	return(1);
}  

postp(){}
