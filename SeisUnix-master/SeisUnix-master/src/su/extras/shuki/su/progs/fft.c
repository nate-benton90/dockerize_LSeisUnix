/*
 * sufft -- forward fft driver using refft
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;

static char lSccsId[]="%W% %G%\n";

static char *lsdoc = "sufft [-v] <stdin >stdout\n";

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

	/* GET PARAMETERS */
}

static int nt,sign,mode,ntin,idin;


/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;

	/* Pad to next power of 2 */
	ntin = abh->ns;
	idin = abh->id;
	for(nt=1;nt<abh->ns;nt *= 2);
	atrin->data = (float*)realloc(atrin->data,nt);
	if(atrin->data==NULL) err("can't realloc");

	abh->ns = nt;

	/* Set direction to forward and mode to real ==> complex */
	switch(idin) {

		case 10:
			sign = -1;
			mode =  0;
			abh->id = 13;
			break;

		case 11:
			sign = -1;
			mode = -2;
			abh->id = 1;
			break;

		case 12:
			sign = -1;
			mode = -1;
			abh->id = 1;
			break;

		case 13:
			sign =  1;
			mode =  0;
			abh->id = 10;
			break;

		default:
			sign = 1;
			mode = 1;
			abh->id = 12;
	}

	igetpar("sign",&sign);
	igetpar("mode",&mode);

}

/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atr,atrout,abh)
int itr;
Sutrace *atr,*atrout;
Subhed *abh;
{
 	if(nt>abh->ns) bzero(atr->data+ntin,(nt-ntin)*sizeof(float));

	if(mode) {
		refft (atr->data,nt,sign,mode);	
	} else {
		cefft(atr->data,nt,sign);
	}

	return(1);
}

postp(){}
