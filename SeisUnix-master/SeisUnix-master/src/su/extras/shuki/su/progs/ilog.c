/*
 *  syilog - inverse log stretch
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

static char *lsdoc = "suilog <stdin >stdout [-v=0 tau1= t1= nt=]\n";

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

static float *w;
static int *itlog,nt,nw;
 
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	int ntau;
	int i;
	int tmax,tmin;
	float *tlog;
	float t1,tau1;
	float ot1;

	ntau = abh->ns;

	MUSTIGETPAR("nt",&nt);
	MUSTFGETPAR("t1",&t1);
	MUSTFGETPAR("tau1",&tau1);
	tmax = nt;	igetpar("tmax",&tmax);
	tmin = 0;	igetpar("tmin",&tmin);

	nw = 2;
	
	if(verbose) {
		fprintf(stderr,"tau1=%f t1=%f ntau=%d nt=%d tmin=%d tmax=%d\n",
			tau1,t1,ntau,nt,tmin,tmax);
	}

	/* Allocate space */
	tlog  = (float *) malloc((unsigned) (nt * sizeof (float)));
	itlog = (int *)   malloc((unsigned) (nt * sizeof (int)));
	w     = (float *) malloc((unsigned) (nt * nw * sizeof (float)));

	/* Calculate tlog */
	for(i=0;i<tmin;i++) {
		tlog[i] = -2*nw;
	}
	ot1 = 1.0/t1;
	for(i=tmin;i<nt;i++) {
		tlog[i] = tau1*log(ot1*i);
	}

	/* Calculate the interpolation coefficients */
	lintrp (tlog, w, itlog, ntau, nt);

	abh->ns = nt;
}

/* TRACE SEQUENTIAL PROCESSING */
trseq(itr,atrin,atrout,abh)
int itr;
Sutrace *atrin,*atrout;
Subhed *abh;
{
    bcopy((char*)atrin,(char*)atrout,sizeof(Sutrace)-sizeof(float*));
 
    /* Apply stretch */
    stretch (atrout->data,atrin->data,w,itlog,nt,nw);
 
    return(1);
}

postp(){}

stretch(q,p,w,it,lq,nw)
int lq,nw,*it;
float *p,*q,*w;
/*
 *  General coordinate stretch with predetermined coefficients
 *
 *         NW-1
 * Q(T) =  SUM W(T,J)*P(IT(T)), FOR T=0,LQ-1
 *         J=0
 */
{
	int j,i;

	for(i=0;i<lq;i++) {
		q[i] = 0.0;
		for(j=0;j<nw;j++) {
			q[i] += w[i*nw+j] * p[it[i]+j];
		}
	}
}

lintrp (q, w, it, lp, lq)
int *it,lp,lq;
float *q,*w;
{
	int i;
	float delta;

	for(i=0;i<lq;i++) {
/* 		fprintf(stderr,"q[%d]=%f\t",i,q[i]); */
		if(q[i]>=0.0&&q[i]<lp-1) {
			it[i] = q[i]; 
			delta = q[i] - it[i];
			w[i*2] = 1.0 - delta;
			w[i*2+1] = delta;
		} else {
			it[i] = 0;
			w[i*2] = 0.0;
			w[i*2+1] = 0.0;
		}
/* 		fprintf(stderr,"it=%d w=(%f,%f)\n",it[i],w[i*2],w[i*2+1]); */
	}
}
