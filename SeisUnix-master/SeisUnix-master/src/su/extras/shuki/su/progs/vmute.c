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
extern char *SccsId;
 
static char lSccsId[]="@(#)vmute.c	1.5\t11/15/88\n";


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
        SccsId = lSccsId;

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

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}
 
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
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

	if (nmute > abh->ns) {
		nmute = abh->ns;
		warn(__FILE__,__LINE__,"muting whole trace #%d", itr);
	}
	bzero(atr->data, nmute * sizeof(float));

	return(1);
}

postp(){}
