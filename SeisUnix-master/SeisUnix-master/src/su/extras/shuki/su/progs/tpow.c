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
extern char *SccsId;

static char lSccsId[]="@(#)tpow.c	1.8\t11/15/88\n";
static float tpow;

static char *lsdoc = 
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

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
	SccsId = lSccsId;
/* 	sprintf(SccsId,"@(#)tpow.c	1.8\t11/15/88\n"); */

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

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atrin,atrout,abh)
int itr;
Sutrace *atrin,*atrout;
Subhed *abh;
{
	dotpow( tpow,  abh->ns, atrin->data); /* DO TPOW */
	return(1);
}

postp(){}
