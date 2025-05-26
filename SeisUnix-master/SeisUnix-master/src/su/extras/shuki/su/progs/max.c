/*
 * sumax - get maximum value
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose,suout;
extern char *SccsId;
 
static char lSccsId[]="@(#)max.c	1.4\t11/15/88\n";



static char *lsdoc = 
"sumax [-v -a] <stdin >stdout				\n\
							\n\
OPTIONS:						\n\
	-v verbose on to print maximum of every trace	\n\
	-a use absolute values				\n\
							\n";

static float (*maxfun)();
float vmax(),vabsmax();

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
         SccsId = lSccsId;


	maxfun = vmax;
	verbose = false;
	suout = false;

	while( (c=getopt(xargc,xargv,"va"))!=EOF) {
		switch(c) {
		case 'a':
			maxfun = vabsmax;
			break;
		case 'v':
			verbose = true;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
}

static int gitr;		/* trace with global max */
#ifndef HUGE
#define HUGE 1e38
#endif
static float gmax= -1e38/*HUGE*/;	/* global absolute maximum */

prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}            
 
trseq(itr,atrin,atrout,abh)
int itr;
Sutrace *atrin,*atrout;
Subhed *abh;
{
	float tmax;

	tmax = maxfun(atrin->data,abh->ns);

	if(verbose) fprintf(stderr,"%d %f\n", itr, tmax);
/* 	fflush(stderr); */

	if(tmax > gmax) {
		gmax = tmax;
		gitr = itr;
	}
}

postp()
{
	fprintf(stderr,"Global max=%e at trace %d\n",gmax,gitr);
}
