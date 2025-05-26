/*
 * FFT of trace
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
static int phase;
static int ns,ns2,nsout;
static float dt;
extern char *SccsId;
 
static char lSccsId[]="@(#)trfft.c	1.11\t11/15/88\n";


static char *lsdoc =
"sufft [-v][-p]\n\
    out(f) = FFT(in(t))                                               \n\
                                                                      \n\
OPTIONS:                                                              \n\
    -v turn verbose on                                                \n\
    -p evaluate only phase spectrum (default only amplitude spectrum) \n\
                                                                      \n\
PARAMETERS:  no parameters                                            \n";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
        SccsId = lSccsId;


	/* GET OPTIONS */
	phase=0;
	while( (c=getopt(xargc,xargv,"v p"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case 'p':
			phase=1;
			break;
		case '?':
			warn("getopt returned '?'");
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
        /* OUTTRACE ON THE PLACE OF INTRACE */
      /*	*aatrout = atrin;	*/
	dt = abh->dt/1000000.0;
	ns = abh->ns;
	for (ns2=1; ns2<ns; ns2 *=2);
	nsout = ns2/2 + 2;
	abh->ns = nsout;
}

/* TRACE SEQUENTIAL FFT-PROCESSING */
trseq(itr,trin,trout,abh)
int itr;
Sutrace *trin,*trout;
Subhed *abh;
{
	int i,j,k;	
	double atan2(),sqrt(),af,bf;

	/* FIRST TRACE	*/
	if(itr==0) {
		trin->data = (float*)realloc(trin->data,(ns2+2)*sizeof(float));
		bzero((char*)trout->data,nsout*sizeof(float));
	}

        /* copy of trace header excluding data address */
        bcopy((char*)trin,(char*)trout,sizeof(Sutrace)-sizeof(float*));
/* 	trout->ns = nsout; */

	/*	ADD ZERO's TO THE TAIL OF TRACE		*/
	bzero((char*)(trin->data+ns),(ns2+2-ns)*sizeof(float));

	refft(trin->data,ns2,1,2);

	for (i=0,j=0; i<=ns2; i+=2,j++) {
		af = *(trin->data+i);
		bf = *(trin->data+i+1);
		if (phase==0)
			*(trout->data+j) = sqrt(af*af + bf*bf); 
		else
			*(trout->data+j) = atan2(bf,af);       
	}
	return(1);
}
 
postp(){}
