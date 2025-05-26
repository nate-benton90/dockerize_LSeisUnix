/*
 * log - log stretch
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

static char *lsdoc = "sulog [-v] <stdin >stdout\n";

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
static int *itexp,ntau,nw;

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	int nt;
	int i;
	int tmax,tmin;
	float *texp;
	float eps,miu;
	float t1,tau1;
	float otau1;

	eps = 0.0;	fgetpar("eps",&eps); /* Minimum oversampling  */
	if(fabs(eps)>=0.999) warn("eps=%f -- may crash",eps);
	miu = 0.0;	fgetpar("miu",&miu); /* Zero padding */
	if(miu<0.0) warn("negative miu=%f -- may crash",miu);

	nt = abh->ns;

	tmax = nt;   lgetpar("tmax",&tmax); /* Maximum sample of interest */
	tmin = .1*nt; lgetpar("tmin",&tmin); /* Minimum sample of interest */
	nw = 2;
	
	sett1 (tmin ,tmax, miu, eps, &ntau, &tau1, &t1);
	if(verbose) {
		fprintf(stderr,"tau1=%f t1=%f ntau=%d nt=%d tmin=%d tmax=%d\n",
			tau1,t1,ntau,nt,tmin,tmax);
	}

	/* Allocate space for exp operations */
	texp  = (float *) malloc((unsigned) (ntau * sizeof (float)));
	itexp = (int *)   malloc((unsigned) (ntau * sizeof (int)));
	w     = (float *) malloc((unsigned) (ntau * nw * sizeof (float)));

	/* Calculate texp */
	otau1 = 1.0/tau1;
	for(i=0;i<ntau;i++) {
		texp[i] = t1*exp(otau1*i);
	}

	/* Calculate the interpolation coefficients */
	lintrp (texp, w, itexp, nt, ntau);

	abh->ns = ntau;
}

/* TRACE SEQUENTIAL PROCESSING */
trseq(itr,atrin,atrout,abh)
int itr;
Sutrace *atrin,*atrout;
Subhed *abh;
{
	bcopy((char*)atrin,(char*)atrout,sizeof(Sutrace)-sizeof(float*));

	/* Apply stretch */
	stretch (atrout->data,atrin->data,w,itexp,ntau,nw);

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

sett1 (tmin ,tmax, miu, eps, ntau, tau1, t1)
int *ntau,tmin,tmax;
float miu,eps,*tau1,*t1;
/*
 * Input:
 *	eps = minimum oversampling
 *	miu = zero padding
 *
 * Output:
 *	taumax = MAXIMUM VALUE OF TMIN<=T(TAU)<=TMAX
 *	ntau = NEXT POWER OF 2 OF (1+MIU)*TAUMAX+1
 *	tau1,t1 = CONSTANTS TO BE USED IN T/T1=EXP(TAU/TAU1)
 *
 * The following are required of t/t1=exp(tau/tau1)
 *	o  t[0] = tmin
 *	o  the slope dt/dtau is 1-eps at t(taumax)=tmax
 *	o  ntau is a power of 2
 *	o  ntau >= (1+miu)*taumax)
 */
{
	int nextpower();
	int taumax;

	*t1 = tmin;
	*tau1 = (float)tmax/(1.0-eps);	
	taumax = *tau1 * log((float)tmax/tmin);
	*ntau = nextpower(2,(int) ((1.0+miu) * taumax));
/* 	fprintf(stderr,"tmin tmax taumax = %d %d %d\n",tmin,tmax,taumax); */
/* 	fprintf(stderr,"ntau tau1 t1 = %d %f %f\n",*ntau,*tau1,*t1); */
}

nextpower(p,n)
int p,n;
{
	int nn;
	if(!n) return(0);
	for(nn=1;nn<n;nn *= p);
	return(nn);
}
