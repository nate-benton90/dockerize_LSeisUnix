h51416
s 00000/00000/00080
d D 1.2 88/11/15 14:03:01 shuki 2 1
c 
e
s 00080/00000/00000
d D 1.1 88/06/06 13:31:25 shuki 1 0
c date and time created 88/06/06 13:31:25 by shuki
e
u
U
t
T
I 1
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
 
static char lSccsId[]="%W%\t%G%\n";

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
E 1
