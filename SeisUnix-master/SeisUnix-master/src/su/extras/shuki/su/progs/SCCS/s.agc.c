h16556
s 00000/00000/00102
d D 1.9 88/11/15 14:03:34 shuki 9 8
c 
e
s 00000/00000/00102
d D 1.8 88/06/15 12:26:45 moshe 8 7
c 
e
s 00001/00000/00101
d D 1.7 88/06/01 08:04:53 shuki 7 6
c 
e
s 00004/00001/00097
d D 1.6 88/05/25 14:54:27 shemer 6 5
c with SccsId[]
e
s 00023/00015/00075
d D 1.5 88/05/25 06:53:10 shuki 5 4
c umainseq
e
s 00001/00001/00089
d D 1.4 88/05/17 10:48:37 moshe 4 3
c 
e
s 00012/00001/00078
d D 1.3 88/05/17 10:24:19 moshe 3 2
c 
e
s 00000/00001/00079
d D 1.2 88/05/15 12:08:22 moshe 2 1
c 
e
s 00080/00000/00000
d D 1.1 88/05/15 11:26:24 moshe 1 0
c date and time created 88/05/15 11:26:24 by moshe
e
u
U
f e 0
t
T
I 1
/*
 * agc - Automatic gain control
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
static char lSccsId[]="%W%\t%G%\n";
E 6

I 6

E 6
static float win;
I 3
static float *aa;
E 3
static int rms=0;

static char *lsdoc = 
"suagc win= [-vr] <stdin >stdout		\n\
						\n\
D 2
	out(t) = t**tpow*in(t)			\n\
E 2
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
	-r	use rms for envelope calculation\n\
			(default absolute value)\n\
						\n\
PARAMETERS:					\n\
	win=	window length (msec)		\n\
						\n";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
I 6
        SccsId=lSccsId;
E 6

	/* GET OPTIONS */
	while( (c=getopt(xargc,xargv,"vr"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case 'r':
			rms = 1;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}

	/* GET PARAMETERS */
	if(!fgetpar("win",&win)) {
		warn(__FILE__,__LINE__,"Must specify win");
		selfdoc();
	}
}

I 5
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

E 5
#ifdef HP
#define _AGC_ agc
#else
#define _AGC_ agc_
#endif
D 5

/* TRACE SEQUENTIAL AUTOMATIC GAIN PROCESSING */
trseq(itr,atr,abh)
int itr;
Sutrace *atr;
E 5
I 5
 
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
E 5
Subhed *abh;
{
	static float dt;
I 3
	static float wmx;
D 5
        int num;
E 5
E 3

	if(itr==0) {
		dt = abh->dt/1000.0;
I 3
D 5
                num = abh->ns;
                wmx = dt*num;
         if(wmx<win)
	{
D 4
            worn(__FILE__,__LINE__,"Agc window too big (%f) ... corrected to %f",win,wmx);
E 4
I 4
            warn(__FILE__,__LINE__,"Agc window too big (%f) ... corrected to %f",win,wmx);
E 4
            win=wmx;
        }
         aa=(float *)malloc((unsigned)(num*sizeof(float)));
E 5
I 5
		wmx = dt*(abh->ns-1);
		if(wmx<win) {
			warn(__FILE__,__LINE__,"Agc window too big (%g msec) ... corrected to %g msec",win,wmx);
			win=wmx;
		}
		aa=(float*)malloc((unsigned)(abh->ns*sizeof(float)));
E 5
E 3
	}

D 3
        _AGC_(atr->data,&abh->ns,&win,&dt,&rms);
E 3
I 3
D 5
        _AGC_(atr->data,aa,&abh->ns,&win,&dt,&rms);
E 5
I 5
	_AGC_(atr->data,aa,&abh->ns,&win,&dt,&rms);
E 5
E 3

	return(1);
}
I 5

I 7
postp(){}
E 7
D 6
postp(){}
E 6
E 5
E 1
