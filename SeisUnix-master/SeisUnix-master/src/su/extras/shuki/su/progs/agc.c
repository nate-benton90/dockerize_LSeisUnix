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
extern char *SccsId;
static char lSccsId[]="@(#)agc.c	1.9\t11/15/88\n";


static float win;
static float *aa;
static int rms=0;

static char *lsdoc = 
"suagc win= [-vr] <stdin >stdout		\n\
						\n\
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
        SccsId=lSccsId;

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

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

#ifdef HP
#define _AGC_ agc
#else
#define _AGC_ agc_
#endif
 
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
Subhed *abh;
{
	static float dt;
	static float wmx;

	if(itr==0) {
		dt = abh->dt/1000.0;
		wmx = dt*(abh->ns-1);
		if(wmx<win) {
			warn(__FILE__,__LINE__,"Agc window too big (%g msec) ... corrected to %g msec",win,wmx);
			win=wmx;
		}
		aa=(float*)malloc((unsigned)(abh->ns*sizeof(float)));
	}

	_AGC_(atr->data,aa,&abh->ns,&win,&dt,&rms);

	return(1);
}

postp(){}
