h05726
s 00000/00000/00083
d D 1.4 88/11/15 14:02:30 shuki 4 3
c 
e
s 00006/00000/00077
d D 1.3 88/05/25 14:53:42 shemer 3 2
c with SccsId[]
e
s 00019/00007/00058
d D 1.2 88/05/25 06:52:57 shuki 2 1
c umainseq
e
s 00065/00000/00000
d D 1.1 88/04/14 13:52:29 shuki 1 0
c date and time created 88/04/14 13:52:29 by shuki
e
u
U
f e 0
t
T
I 1
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
I 3
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";

E 3
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
I 3
         SccsId = lSccsId;

E 3

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
I 2
 
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}
 
E 2
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
D 2
trseq(itr,atr,abh)
int itr;
Sutrace *atr;
E 2
I 2
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
E 2
Subhed *abh;
D 2
    { 
       dogpow(gpow,abh->ns,atr-> data);
       return(1);
    }  
E 2
I 2
{
	dogpow(gpow,abh->ns,atr-> data);
	return(1);
}  

postp(){}
E 2
E 1
