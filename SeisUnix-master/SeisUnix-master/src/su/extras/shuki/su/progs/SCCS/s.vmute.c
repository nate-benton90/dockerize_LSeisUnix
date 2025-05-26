h16818
s 00000/00000/00086
d D 1.5 88/11/15 14:03:05 shuki 5 4
c 
e
s 00002/00002/00084
d D 1.4 88/06/06 13:12:22 shuki 4 3
c Cancel ns in trace headers
e
s 00005/00000/00081
d D 1.3 88/05/25 14:54:09 shemer 3 2
c with SccsId[]
e
s 00014/00003/00067
d D 1.2 88/05/25 06:53:06 shuki 2 1
c umainseq
e
s 00070/00000/00000
d D 1.1 88/04/14 13:52:51 shuki 1 0
c date and time created 88/04/14 13:52:51 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * suvmute - mute large offsets up shallow by velocity
 */

#include <stdio.h>
/* #include <math.h> */
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
"						\n\
suvmute v= [OPTIONS] <stdin >stdout 		\n\
						\n\
	Mutes where offset/v < time		\n\
						\n\
";

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
	static float v;		/* mute velocity			*/
	static float oavdt;	/* 1.0/(v*dt)				*/
	int nmute;		/* number of points muted		*/

	if(!itr) {
		/* GET PARAMETERS */
		if (!fgetpar("v",&v)) err(__FILE__,__LINE__,"must give v=");
		if (v <= 0.0) err(__FILE__,__LINE__,"v = %f must be positive", v);

		oavdt = 1000000.0 / (v*abh->dt);
	}

	nmute = oavdt * ABS(atr->offset);

D 4
	if (nmute > atr->ns) {
		nmute = atr->ns;
E 4
I 4
	if (nmute > abh->ns) {
		nmute = abh->ns;
E 4
		warn(__FILE__,__LINE__,"muting whole trace #%d", itr);
	}
	bzero(atr->data, nmute * sizeof(float));

	return(1);
}
I 2

postp(){}
E 2
E 1
