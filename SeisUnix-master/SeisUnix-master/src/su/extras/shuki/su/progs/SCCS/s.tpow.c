h29916
s 00000/00000/00076
d D 1.8 88/11/15 14:03:04 shuki 8 7
c 
e
s 00000/00000/00076
d D 1.7 88/05/25 14:54:07 shemer 7 6
c with SccsId[]
e
s 00004/00002/00072
d D 1.6 88/05/25 12:51:49 shuki 6 5
c 
e
s 00001/00000/00073
d D 1.5 88/05/25 12:22:19 shuki 5 4
c 
e
s 00001/00001/00072
d D 1.4 88/05/25 12:20:23 shuki 4 3
c 
e
s 00011/00010/00062
d D 1.3 88/05/25 12:19:02 shuki 3 2
c SccsId
e
s 00014/00003/00058
d D 1.2 88/05/23 10:19:43 shuki 2 1
c umainseq
e
s 00061/00000/00000
d D 1.1 88/04/14 13:52:48 shuki 1 0
c date and time created 88/04/14 13:52:48 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * tpow - Gain by time to the power of tpow
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
I 6
extern char *SccsId;
E 6

I 3
D 4
static char SccsId[]="%W\t%G%";
E 4
I 4
D 6
static char SccsId[]="%W%\t%G%";
E 6
I 6
static char lSccsId[]="%W%\t%G%\n";
E 6
E 4
E 3
static float tpow;

static char *lsdoc = 
D 3
"sutpow tpow= [-v] <stdin >stdout		\n\
						\n\
	out(t) = t**tpow*in(t)			\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
						\n\
PARAMETERS:					\n\
	tpow=					\n\
						\n";
E 3
I 3
"sutpow tpow= [-v] <stdin >stdout  \n\
      \n\
 out(t) = t**tpow*in(t)   \n\
      \n\
OPTIONS:     \n\
 -v turn verbose on   \n\
      \n\
PARAMETERS:     \n\
 tpow=     \n\
      \n";
E 3

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
I 6
	SccsId = lSccsId;
/* 	sprintf(SccsId,"%W%\t%G%\n"); */
E 6

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
	if(!fgetpar("tpow",&tpow)) {
		warn(__FILE__,__LINE__,"Must specify tpow");
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
I 5
D 6
	hispr(outfd,SccsId);
E 6
E 5
}

E 2
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
D 2
trseq(itr,atr,abh)
E 2
I 2
trseq(itr,atrin,atrout,abh)
E 2
int itr;
D 2
Sutrace *atr;
E 2
I 2
Sutrace *atrin,*atrout;
E 2
Subhed *abh;
{
D 2
	dotpow( tpow,  abh->ns, atr->data); /* DO TPOW */
E 2
I 2
	dotpow( tpow,  abh->ns, atrin->data); /* DO TPOW */
E 2
	return(1);
}
I 2

postp(){}
E 2
E 1
