/*
 * sutwind - time window
 *
 * Examples:
 *	sutwind <DATA tmin=0.5 tmax=2 | sugain ...
 *
*/
#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *sdoc;
extern char *SccsId;
 
static char lSccsId[]="@(#)twind.c	1.4\t11/15/88\n";


static char *lsdoc =
"									\n\
suwind <stdin >stdout [OPTIONS PARAMETERS]				\n\
									\n\
OPTIONS:								\n\
	-v	turn verbose on						\n\
									\n\
PARAMETERS:								\n\
	tmin=0.0	min time to pass				\n\
	tmax=		max time to pass (default= from header)		\n\
	itmin=0		min time sample to pass				\n\
	itmax=		max time sample to pass (default= from header)	\n\
	nt=		number of time samples to pass. (nt=itmax-itmin+1)\n\
									\n\
";
static float tmin,tmax,dt;
static int itmin,itmax,nt,ntorig;
static int nzeros;
static char *pzeros;

inits()
{
	int c;

	sdoc = lsdoc;
         SccsId = lSccsId;


	/* GET OPTIONS */
	verbose = false;
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


/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	ntorig = abh->ns;

	/* GET PARAMETERS */
	dt = 0.000001*abh->dt;
	itmin=0;		igetpar("itmin"   , &itmin);
	if(fgetpar("tmin",&tmin)) {
		itmin=tmin/dt;
	} else {
		tmin = itmin*dt;
	}

	itmax=abh->ns-1;		igetpar("itmax"   , &itmax);
	if(fgetpar("tmax",&tmax)) {
		itmax=tmax/dt-1;
	} else {
		tmax =itmax*dt;
	}

	if(igetpar("nt",&nt)) {
		itmax = itmin+nt-1;
		tmax =itmax*dt;
	} else {
		nt =itmax-itmin+1;
	}

	if(itmin<0 || itmin>itmax)
		err(__FILE__,__LINE__,"Illegal itmin=%d or itmax=%d",itmin,itmax);

	if(verbose) fprintf(stderr,"timewindow: itmin=%d itmax=%d nt=%d\n",itmin,itmax,nt);

	abh->ns = nt;

	nzeros = (nt-ntorig)*sizeof(float);
	if(nzeros>0) {
		pzeros = (char*)atrin->data;
		atrin->data = (float*) realloc(atrin->data,nt*sizeof(float));
		bcopy(pzeros,(char*)atrin->data,ntorig*abh->esize);
		pzeros = (char*) (atrin->data + ntorig - itmin);
	}

	*aatrout = atrin;

}
 
trseq(itr,atrin,atrout,abh)
int itr;
Sutrace *atrin,*atrout;
Subhed *abh;
{
	int i;

	if(itmin>0) {
		for(i=itmin;i<=itmax;i++) {
			atrout->data[i-itmin] = atrin->data[i];
		}
	}

	if(nzeros>0)
		bzero(pzeros,nzeros);

/* 	atrout->ns = nt; */

	return(1);
}

postp(){}
