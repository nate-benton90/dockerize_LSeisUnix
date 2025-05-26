h46969
s 00000/00000/00085
d D 1.4 88/11/15 14:02:26 shuki 4 3
c 
e
s 00005/00000/00080
d D 1.3 88/05/25 14:53:38 shemer 3 2
c with SccsId[]
e
s 00015/00003/00065
d D 1.2 88/05/25 06:52:53 shuki 2 1
c umainseq
e
s 00068/00000/00000
d D 1.1 88/04/14 13:52:25 shuki 1 0
c date and time created 88/04/14 13:52:25 by shuki
e
u
U
f e 0
t
T
I 1
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
I 3
extern char *SccsId;
static char lSccsId[]="%W%\t%G%\n";
E 3

I 3

E 3
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
I 2

postp(){}
E 2
E 1
