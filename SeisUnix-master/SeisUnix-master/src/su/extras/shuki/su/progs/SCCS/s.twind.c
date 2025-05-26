h14482
s 00000/00000/00134
d D 1.4 88/11/15 14:03:41 shuki 4 3
c 
e
s 00001/00001/00133
d D 1.3 88/06/06 13:12:36 shuki 3 2
c Cancel ns in trace headers
e
s 00006/00000/00128
d D 1.2 88/05/25 14:54:31 shemer 2 1
c with SccsId[]
e
s 00128/00000/00000
d D 1.1 88/05/24 05:34:30 shuki 1 0
c date and time created 88/05/24 05:34:30 by shuki
e
u
U
f e 0
t
T
I 1
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
I 2
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";
E 2

I 2

E 2
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
I 2
         SccsId = lSccsId;

E 2

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

D 3
	atrout->ns = nt;
E 3
I 3
/* 	atrout->ns = nt; */
E 3

	return(1);
}

postp(){}
E 1
