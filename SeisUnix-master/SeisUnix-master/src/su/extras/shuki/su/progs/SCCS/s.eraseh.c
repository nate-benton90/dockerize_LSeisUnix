h65233
s 00000/00000/00116
d D 1.3 88/11/15 14:03:37 shuki 3 2
c 
e
s 00044/00030/00072
d D 1.2 88/07/17 14:50:14 moshe 2 1
c 
e
s 00102/00000/00000
d D 1.1 88/07/17 08:07:43 moshe 1 0
c date and time created 88/07/17 08:07:43 by moshe
e
u
U
t
T
I 1
/*
D 2
 * agc - Automatic gain control
E 2
I 2
 * eraseh - Erase SU header and write data in trace order (no headers)
E 2
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
static char lSccsId[]="@(#)agc.c	1.8\t6/15/88\n";


D 2
static float win;
static float *aa;
static int rms=0;
E 2

static char *lsdoc = 
"suagc win= [-vr] <stdin >stdout		\n\
						\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
D 2
	-r	use rms for envelope calculation\n\
			(default absolute value)\n\
E 2
						\n\
D 2
PARAMETERS:					\n\
	win=	window length (msec)		\n\
E 2
						\n";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
        SccsId=lSccsId;

	/* GET OPTIONS */
	while( (c=getopt(xargc,xargv,"vr"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
D 2
		case 'r':
			rms = 1;
			break;
E 2
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}

	/* GET PARAMETERS */
D 2
	if(!fgetpar("win",&win)) {
		warn(__FILE__,__LINE__,"Must specify win");
		selfdoc();
	}
E 2
}

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

D 2
#ifdef HP
#define _AGC_ agc
#else
#define _AGC_ agc_
#endif
E 2
 
D 2
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
E 2
I 2
/* TRACE SEQUENTIAL PROCESSING */
E 2
trseq(itr,atr,atrout,abh)
D 2
int itr;    
E 2
I 2
int *itr;    
E 2
Sutrace *atr,*atrout;
Subhed *abh;
{
	static float dt;
D 2
	static float wmx;
E 2
I 2
        static int iff,nbyt;
int ier;    
E 2

	if(itr==0) {
		dt = abh->dt/1000.0;
D 2
		wmx = dt*(abh->ns-1);
		if(wmx<win) {
			warn(__FILE__,__LINE__,"Agc window too big (%g msec) ... corrected to %g msec",win,wmx);
			win=wmx;
		}
		aa=(float*)malloc((unsigned)(abh->ns*sizeof(float)));
E 2
I 2
                strt(&iff,&ier);
                nbyt=abh->ns*sizeof(float);
E 2
	}

D 2
	_AGC_(atr->data,aa,&abh->ns,&win,&dt,&rms);
E 2
I 2
	wrt(&iff,&nbyt,&ier,atr->data);
E 2

	return(1);
}

postp(){}
I 2

strt(iff,ier)
int  *iff, *ier;
{
	static char *aa = "/scr/moshe/FFF";
	static int fd;

	fd =  open(aa, 2);
	*iff = fd;

	if(fd > 0)
 	   *ier=0;
	if(fd < 0 )
	   *ier= -1;

}

endtio_(iff, ier)
int *iff, *ier;
{
	int iret;

	iret = close(*iff);
	*ier = iret;
}

wrt(iff, nbyte, ier, aa)
int *iff, *nbyte, *ier;
float *aa;
{
	int nwrite;

	nwrite = write(*iff, aa, *nbyte);
	*ier = nwrite;

}
E 2
E 1
