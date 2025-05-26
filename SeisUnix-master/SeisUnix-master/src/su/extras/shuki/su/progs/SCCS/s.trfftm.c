h52423
s 00001/00001/00147
d D 1.3 88/11/15 14:02:37 shuki 3 2
c 
e
s 00001/00001/00147
d D 1.2 88/06/06 13:13:01 shuki 2 1
c Cancel ns in trace headers
e
s 00148/00000/00000
d D 1.1 88/05/29 14:59:44 root 1 0
c date and time created 88/05/29 14:59:44 by root
e
u
U
f e 0
t
T
I 1
/*
 * FFT of trace
 */

#include <stdio.h>
#include <math.h>
D 3
#include "/src/su/include/su.h"
E 3
I 3
#include "../include/su.h"
E 3

double fabs();
extern char *SccsId;

/*static char lSccsId[]="@(#)trfft.c      1.8\t5/25/88\n";*/
static char lSccsId[]="                               ";


extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
static int phase;
static int ns,nsout;
static float dt;
static float *trig,*work;
static int nfft,ifax[15];

static char *lsdoc = 				
"sutrfftm [-v][-p]   				 	\n\
	out(f) = FFT(in(t))			\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
	-p	evaluate only phase spectrum	\n\
		(by default only amplitude	\n\
				 spectrum)	\n\
						\n\
PARAMETERS:  no parameters			\n\
						\n";

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

#ifdef HP
#define _NEXTFFT_ nextfft
#define _FFTPREP_ fftprep
#define _RFFTM_   rfftm
#else
#define _NEXTFFT_ nextfft_
#define _FFTPREP_ fftprep_
#define _RFFTM_   rfftm_
#endif

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{                    
	int ier,i1;
      /**aatrout = atrin;*/ /* OUTTRACE ON THE PLACE OF INTRACE */
	dt = abh->dt/1000000.0;
	ns = abh->ns;

	/* frequency quantity (nfft/2+1) evaluation */
	_NEXTFFT_(&ns,&nfft);

	trig = (float*) malloc(2*nfft*sizeof(float));
	work = (float*) malloc(nfft*sizeof(float));

	/* TRIGonometric functions preparation */
	i1 = 1;
	_FFTPREP_(&nfft,ifax,trig,&i1,&ier);
	if(ier!=0){
		warn(__FILE__,__LINE__,"nfft=%d cannot be factorized",nfft);
		selfdoc();
	}

	if (nfft > ns)
		atrin->data = (float*)realloc(atrin->data,nfft*sizeof(float));
	nsout = nfft/2 + 2;	/* frequency quantity + 1 */
	abh->ns = nsout;
}

/* TRACE SEQUENTIAL FFT-PROCESSING */
trseq(itr,trin,trout,abh)
int itr;
Sutrace *trin,*trout;
Subhed *abh;
{
	int i,j,k,i1,i2;	
	double atan2(),sqrt(),af,bf;

        /* copy of trace header excluding data address */
        bcopy((char*)trin,(char*)trout,sizeof(Sutrace)-sizeof(float*));
D 2
	trout->ns = nsout;
E 2
I 2
/* 	trout->ns = nsout; */
E 2

	/*	ADD ZERO's TO THE TAIL OF TRACE		*/
	if (nfft > ns)
		bzero((char*)(trin->data+ns),(nfft-ns)*sizeof(float));

	*(trout->data+nsout-1) = 0.0;	/* "value" at NYQfreq + df */ 
	i1 = 1; i2 = -1;
	_RFFTM_(trin->data,work,trig,ifax,&i1,&i1,&nfft,&i1,&i2);

	if (phase==0){
		/* AMPLITUDE at ZEROfreq and NYQfreq */
		*(trout->data) = fabs(*(trin->data));
		*(trout->data+nsout-2) = fabs(*(trin->data+nfft-1));
	}
	else
		/* ZEROphase at ZEROfreq and NYQfreq */
		*(trout->data) = *(trout->data+nsout-2) = 0.0;

	for (i=1,j=1; i<=nfft-3; i+=2,j++) {
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
E 1
