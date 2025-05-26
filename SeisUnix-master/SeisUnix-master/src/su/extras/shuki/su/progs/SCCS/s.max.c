h29948
s 00000/00000/00092
d D 1.4 88/11/15 14:02:37 shuki 4 3
c 
e
s 00006/00000/00086
d D 1.3 88/05/25 14:53:45 shemer 3 2
c with SccsId[]
e
s 00019/00009/00067
d D 1.2 88/05/24 07:31:57 shuki 2 1
c umainseq
e
s 00076/00000/00000
d D 1.1 88/04/14 13:52:32 shuki 1 0
c date and time created 88/04/14 13:52:32 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * sumax - get maximum value
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
D 2
extern bool verbose;
E 2
I 2
extern bool verbose,suout;
I 3
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";
E 3
E 2

D 2
float vmax(),vabsmax();
static float (*maxfun)();
E 2

I 3

E 3
static char *lsdoc = 
"sumax [-v -a] <stdin >stdout				\n\
							\n\
OPTIONS:						\n\
	-v verbose on to print maximum of every trace	\n\
	-a use absolute values				\n\
							\n";

I 2
static float (*maxfun)();
float vmax(),vabsmax();

E 2
/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
I 3
         SccsId = lSccsId;

E 3

	maxfun = vmax;
	verbose = false;
I 2
	suout = false;
E 2

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

D 2
/* TRACE SEQUENTIAL MAX FINDING */
trseqn(itr,atr,abh)
E 2
I 2
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}            
 
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
	float tmax;

D 2
	tmax = maxfun(atr->data,abh->ns);
E 2
I 2
	tmax = maxfun(atrin->data,abh->ns);
E 2

D 2
	if(verbose) printf("%d %e\n", itr, tmax);
E 2
I 2
	if(verbose) fprintf(stderr,"%d %f\n", itr, tmax);
/* 	fflush(stderr); */
E 2

	if(tmax > gmax) {
		gmax = tmax;
		gitr = itr;
	}
}

postp()
{
D 2
	printf("Global max=%e at trace %d\n",gmax,gitr);
E 2
I 2
	fprintf(stderr,"Global max=%e at trace %d\n",gmax,gitr);
E 2
}
E 1
