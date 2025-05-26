/*
 * sbhw - Set Binary Header Word
 *
 * Example:
 *	sushw dt=4000 <infile >outfile
 *
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
 
static char lSccsId[]="@(#)sbhw.c	1.2\t11/15/88\n";

static int dt;


static char *lsdoc = 
"susbhw [OPTIONS PARAMETERS] <stdin >stdout    \n\
                                               \n\
Set binary header word                         \n\
                                               \n\
OPTIONS:                                       \n\
-v turn verbose on                             \n\
                                               \n\
PARAMETERS:                                    \n\
   dt=                                         \n\
                                               \n\
EXAMPLE:                                       \n\
    sushw dt=4000 <infile >outfile             \n\
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
	if(!igetpar("dt",&dt)) selfdoc();
}

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	abh->dt = dt;
    *aatrout = atrin;
}
 
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
Subhed *abh;
{
	return(1);
}
postp(){}
