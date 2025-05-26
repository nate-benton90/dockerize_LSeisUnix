h13258
s 00000/00000/00178
d D 1.8 88/11/15 14:03:15 shuki 8 7
c 
e
s 00009/00001/00169
d D 1.7 88/10/24 10:01:38 shuki 7 6
c 
e
s 00006/00000/00164
d D 1.6 88/05/25 14:54:19 shemer 6 5
c with SccsId[]
e
s 00024/00010/00140
d D 1.5 88/05/23 10:19:44 shuki 5 4
c umainseq
e
s 00002/00002/00148
d D 1.4 88/05/15 16:57:54 valer 4 3
c 
e
s 00009/00007/00141
d D 1.3 88/05/15 14:35:50 valer 3 2
c 
e
s 00061/00012/00087
d D 1.2 88/05/10 09:30:52 valer 2 1
c 
e
s 00099/00000/00000
d D 1.1 88/05/08 17:18:45 valer 1 0
c date and time created 88/05/08 17:18:45 by valer
e
u
U
t
T
I 1
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
I 6
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";
E 6

I 6

E 6
static float plag,tol,pwn,sat,eat;
D 2
static int iplag,isat,ieat;
E 2
I 2
static int iplag,ieat;
E 2

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
D 4
                 (by default: 200)				\n\
E 4
I 4
                 (by default: 120)				\n\
E 4
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
I 2
	int c;

E 2
	sdoc = lsdoc;
I 6
        SccsId = lSccsId;

E 6

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
D 4
	tol=200.0;	        fgetpar("tol",&tol);
E 4
I 4
	tol=120.0;	        fgetpar("tol",&tol);
E 4
	pwn=0.1;		fgetpar("pwn",&pwn);
D 2
			isat = fgetpar("sat",&sat);
E 2
I 2
	sat=0.0;		fgetpar("sat",&sat);
E 2
			ieat = fgetpar("eat",&eat);
}

I 5
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
/* 	*aatrout = atrin; */
}

E 5
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
D 5
trseq(itr,trin,abh,trout)
E 5
I 5
trseq(itr,trin,trout,abh)
E 5
int itr;
Sutrace *trin,*trout;
Subhed *abh;
{
D 2
	static int lop,mid,shift,ns;	
E 2
I 2
	static int ns,ns1;
	static int ltol,lplag,linv,shift;
	static int lat;
E 2
	static float dt;
D 2
	static float *oper;
E 2
I 2
	static float *toper,*invop,*autc,*cros,*rab1,*rab2;
	static float *psat;
E 2

I 2
	int nsat;
	int i,j;

I 7
						int jj;

E 7
E 2
	/* FIRST TRACE	*/
	if(itr==0) {
D 2
		dt = abh->dt/1000000.0;
	   	ns = abh->ns;
		oper = (float*) malloc(mlop*sizeof(float));
		/*  CONSTRUCTION OF FILTER OPERATOR  */
		_OPERHAN_(f,&dt,&mlop,oper,&lop,&mid);
		shift = mid - 1;
E 2
I 2
		/* INITIALIZATION */
		dt = abh->dt/1000.0;   /* from microsec to msec */
		if(iplag==0) plag=dt;  /* default spike decon */
	   	ns = abh->ns;		/* number of trace samples */
		if(ieat==0) eat=(ns-1)*dt; /* default end autcorr.time */
D 3
		nsat = sat/dt;		/* start autcorr.time sample */
		lat = (eat - sat)/dt + 1;  /* length of autcorr.time
E 3
I 3
		nsat = sat/dt+0.5;	/* start autcorr.time sample */
		lat = (eat - sat)/dt + 1.5;  /* length of autcorr.time
E 3
						window in samples */

		if(tol > (eat-sat)) tol=eat-sat;
D 3
		ltol = tol/dt;		/* tol-length in samples */
		lplag = plag/dt;	/* pred.lag in samples */ 
E 3
I 3
		ltol = tol/dt+0.5;	/* tol-length in samples */
		lplag = plag/dt+0.5;	/* pred.lag in samples */ 
E 3
		linv = ltol - lplag;	/* inv.oper.length in samples */
D 3
		shift = 0;		/* convolution with toper */
E 3
I 3
		shift = 0;		/* for convolution with toper */
E 3
					   
D 3
		psat = trin->data + nsat; /* pointer to sat-sample */
E 3
I 3
		psat = trin->data;
		psat = psat + nsat;	/* pointer to sat-sample */
E 3
		ns1 = ns - nsat;	/* trace length from sat-sample
D 3
						in samples */
E 3
I 3
					   to trace end in samples */
E 3
		
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

E 2
	}
D 2
	/*  CONVOLUTION  */
   	   _CONVV_(&lop,&shift,oper,&ns,trin->data,trout->data); 
E 2
I 2
	
D 5
		/* autocorrelation computation */
	   _CROSCOR_(&ns1,psat,psat,&lat,&ltol,autc);
E 5
I 5
D 7
    bcopy((char*)trin,(char*)trout,sizeof(Sutrace)-sizeof(float*));
E 7
I 7
		jj = sizeof(Sutrace);
		jj = sizeof(float*);
		jj = sizeof(Sutrace)-sizeof(float*);
    bcopy((char*)trin,(char*)trout,jj);


/*     bcopy((char*)trin,(char*)trout,sizeof(Sutrace)-sizeof(float*)); */
E 7
E 5

I 3
D 5
	   *autc *=1.0+pwn*0.01;	/* white noise adding */
E 3
		/* inverse filter computation */
	   _TOEPL_(&linv,autc,cros,rab1,rab2,invop); 
E 5
I 5
	/* autocorrelation computation */
	_CROSCOR_(&ns1,psat,psat,&lat,&ltol,autc);
E 5

D 5
		/*  pred.error operator forming */
	   for(i=lplag, j=0; i<ltol; i++,j++) toper[i] = -invop[j];
E 5
I 5
	autc[0] *=1.0+pwn*0.01;	/* white noise adding */
	/* inverse filter computation */
	_TOEPL_(&linv,autc,cros,rab1,rab2,invop); 
E 5

D 5
		/*  CONVOLUTION of trace with pred.error operator */
   	   _CONVV_(&ltol,&shift,toper,&ns,trin->data,trout->data); 
E 5
I 5
	/*  pred.error operator forming */
	for(i=lplag, j=0; i<ltol; i++,j++) toper[i] = -invop[j];

	/*  CONVOLUTION of trace with pred.error operator */
	_CONVV_(&ltol,&shift,toper,&ns,trin->data,trout->data); 

E 5
E 2
	return(1);
}
I 5

postp(){}
E 5
E 1
