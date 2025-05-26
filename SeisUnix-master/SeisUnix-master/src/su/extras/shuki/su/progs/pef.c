/*
 * predictive error filtering
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
 
static char lSccsId[]="@(#)pef.c	1.8\t11/15/88\n";


static float plag,tol,pwn,sat,eat;
static int iplag,ieat;

static char *lsdoc = 				
"supef [options parameters] <stdin >stdout			\n\
								\n\
OPTIONS								\n\
     -v          turn verbose on				\n\
                 (by default: off)				\n\
								\n\
PARAMETERS							\n\
     plag=       predictive lag (msec)				\n\
                 (by default: one sampling interval -		\n\
                              - spiking deconvolution)		\n\
								\n\
     tol=        total operator length (msec)			\n\
                 of predictive error filter			\n\
                 (by default: 120)				\n\
								\n\
     pwn=        white noise percent (%)			\n\
                 (by default: 0.1)				\n\
								\n\
     sat=  eat=  start and end time (msec) of the trace window	\n\
                 for autocorrelation computation		\n\
                 (by default: trace start and trace end time)	\n\
								\n";

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
			iplag = fgetpar("plag",&plag); 	
	tol=120.0;	        fgetpar("tol",&tol);
	pwn=0.1;		fgetpar("pwn",&pwn);
	sat=0.0;		fgetpar("sat",&sat);
			ieat = fgetpar("eat",&eat);
}

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
/* 	*aatrout = atrin; */
}

#ifdef HP
#define _CROSCOR_   croscor
#define _TOEPL_   toepl
#define _CONVV_   convv
#define _BLEND_   blend
#else
#define _CROSCOR_   croscor_
#define _TOEPL_   toepl_
#define _CONVV_   convv_
#define _BLEND_   blend_
#endif

/* TRACE SEQUENTIAL FILTERING PROCESSING */
trseq(itr,trin,trout,abh)
int itr;
Sutrace *trin,*trout;
Subhed *abh;
{
	static int ns,ns1;
	static int ltol,lplag,linv,shift;
	static int lat;
	static float dt;
	static float *toper,*invop,*autc,*cros,*rab1,*rab2;
	static float *psat;

	int nsat;
	int i,j;

						int jj;

	/* FIRST TRACE	*/
	if(itr==0) {
		/* INITIALIZATION */
		dt = abh->dt/1000.0;   /* from microsec to msec */
		if(iplag==0) plag=dt;  /* default spike decon */
	   	ns = abh->ns;		/* number of trace samples */
		if(ieat==0) eat=(ns-1)*dt; /* default end autcorr.time */
		nsat = sat/dt+0.5;	/* start autcorr.time sample */
		lat = (eat - sat)/dt + 1.5;  /* length of autcorr.time
						window in samples */

		if(tol > (eat-sat)) tol=eat-sat;
		ltol = tol/dt+0.5;	/* tol-length in samples */
		lplag = plag/dt+0.5;	/* pred.lag in samples */ 
		linv = ltol - lplag;	/* inv.oper.length in samples */
		shift = 0;		/* for convolution with toper */
					   
		psat = trin->data;
		psat = psat + nsat;	/* pointer to sat-sample */
		ns1 = ns - nsat;	/* trace length from sat-sample
					   to trace end in samples */
		
		/* DYNAMIC MEMORY ALLOCATION        */
			/* total predict.error operator */
		toper = (float*) malloc(ltol*sizeof(float));
			/* pred.lag coefficients */
		toper[0] = 1.0;
		for(i=1; i<lplag; i++) toper[i] = 0.0;

			/*  inverse operator */
		invop = (float*) malloc(linv*sizeof(float));

			/* autocorrelation */
		autc = (float*) malloc(ltol*sizeof(float));
		cros = autc + lplag; 	/* pointer to 'cross'corr */

			/* working arrays */
		rab1 = (float*) malloc(linv*sizeof(float));
		rab2 = (float*) malloc(linv*sizeof(float));

	}
	
		jj = sizeof(Sutrace);
		jj = sizeof(float*);
		jj = sizeof(Sutrace)-sizeof(float*);
    bcopy((char*)trin,(char*)trout,jj);


/*     bcopy((char*)trin,(char*)trout,sizeof(Sutrace)-sizeof(float*)); */

	/* autocorrelation computation */
	_CROSCOR_(&ns1,psat,psat,&lat,&ltol,autc);

	autc[0] *=1.0+pwn*0.01;	/* white noise adding */
	/* inverse filter computation */
	_TOEPL_(&linv,autc,cros,rab1,rab2,invop); 

	/*  pred.error operator forming */
	for(i=lplag, j=0; i<ltol; i++,j++) toper[i] = -invop[j];

	/*  CONVOLUTION of trace with pred.error operator */
	_CONVV_(&ltol,&shift,toper,&ns,trin->data,trout->data); 

	return(1);
}

postp(){}
