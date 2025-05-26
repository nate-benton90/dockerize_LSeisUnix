/*
 * eraseh - Erase SU header and write data in trace order (no headers)
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



static char *lsdoc = 
"suagc win= [-vr] <stdin >stdout		\n\
						\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
						\n\
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
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}

	/* GET PARAMETERS */
}

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

 
/* TRACE SEQUENTIAL PROCESSING */
trseq(itr,atr,atrout,abh)
int *itr;    
Sutrace *atr,*atrout;
Subhed *abh;
{
	static float dt;
        static int iff,nbyt;
int ier;    

	if(itr==0) {
		dt = abh->dt/1000.0;
                strt(&iff,&ier);
                nbyt=abh->ns*sizeof(float);
	}

	wrt(&iff,&nbyt,&ier,atr->data);

	return(1);
}

postp(){}

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
