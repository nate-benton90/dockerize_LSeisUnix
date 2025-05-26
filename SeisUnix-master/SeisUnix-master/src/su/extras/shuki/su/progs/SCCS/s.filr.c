h39350
s 00000/00000/00267
d D 1.4 88/11/15 14:03:39 shuki 4 3
c 
e
s 00036/00024/00231
d D 1.3 88/06/22 12:50:54 valer 3 2
c 
e
s 00052/00024/00203
d D 1.2 88/06/20 16:50:17 valer 2 1
c 
e
s 00227/00000/00000
d D 1.1 88/06/19 14:35:48 valer 1 0
c date and time created 88/06/19 14:35:48 by valer
e
u
U
f e 0
t
T
I 1
/*
 * recursive band-pass filtering                           
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
 
D 3
static char lSccsId[]="@(#)filr.c	1.6\t5/25/88\n";
E 3
I 3
static char lSccsId[]="@(#)filr.c	1.6\t6/25/88\n";
E 3


typedef struct {
	float a[5];	/* coeff. in transfer function */
	float b[5];	/* nominator & denominator */
} COEF;
typedef struct {
	int la;		/* number of coeff. a */
	int lb;		/* number of coeff. b */
} NCOEF;
static COEF *pcof;
static NCOEF *pncof;

static float f[4];
static float lev,del;
static int ord,phase;

static float dt;
static int ns,npol;

static char *lsdoc = 				
"RECURSIVE BAND-PASS FILTERING                  \n\
I 3
 on base of CHEBYSHEV polynomials		\n\
						\n\
E 3
 sufilr [options] parameters <stdin >stdout	\n\
						\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
		(by default: off)		\n\
						\n\
	-m	minimum-phase filtering		\n\
		(by default: zero-phase)	\n\
						\n\
PARAMETERS:					\n\
	fl0,fl,fh,fh0=frequencies(Hertz) of pass band	\n\
	            in the shape    |                   \n\
	                           1-       ****        \n\
			         lev-      |****|	\n\
	   (by default: fl=0        |     *|****|*	\n\
	         fh=fNyquist        |    **|****|**  	\n\
D 3
	          fl0=fl/1.5     del-  |***|****|***|   \n\
	          fh0=fh*1.5)       |--|***|****|***|-->\n\
E 3
I 3
	          fl0=fl/1.25    del-  |***|****|***|   \n\
	          fh0=fh*1.25)      |-*|***|****|***|*->\n\
E 3
				    0 fl0 fl    fh fh0	\n\
		Note! 0<=fl0<=fl<=fh<=fh0		\n\
							\n\
	lev=level at the passband edges (fl,fh)		\n\
		(by default: lev=0.9)			\n\
							\n\
	del=level at the stopband edges (fl0,fh0)	\n\
D 3
		(by default: del=0.1			\n\
		 i.e. for fh0=1.5fh slope=40db/oct)	\n\
E 3
I 3
		(by default: del=0.1)			\n\
		Note! del<lev				\n\
E 3
							\n\
D 3
	ord=order of recursive filter                   \n\
E 3
I 3
	ord=order of recursive Chebyshev filter         \n\
E 3
	        (by default: ord is automatic chosen)	\n\
							\n";

/*******************************************************************/
/* INITIALIZE SELF DOCUMENTATION */
inits()
{
D 3
	int c,npar,ierr=0;
D 2
	float rel=1.5;
E 2
I 2
	float deli,levi,ordi,rel=1.5;
E 3
I 3
	int c,npar,ordi,ierr=0;
	float deli,levi,rel=1.25;
E 3
E 2
	char *phstr;

	sdoc = lsdoc;
         SccsId = lSccsId;


	phstr = "zero";
	phase=0;
	/* GET OPTIONS */
D 2
	while( (c=getopt(xargc,xargv,"v"))!=EOF) {
E 2
I 2
	while( (c=getopt(xargc,xargv,"v m"))!=EOF) {
E 2
		switch(c) {
		case 'v':
			verbose = true;
			break;
                case 'm':
                        phase=1;
			phstr="minimum";
                        break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}

	/* GET PARAMETERS */
	f[1]=0.0;	npar = fgetpar("fl",&f[1]); 	
	f[0]=f[1]/rel;	       fgetpar("fl0",&f[0]);
	f[2]=1000.0;	npar += fgetpar("fh",&f[2]);
	f[3]=f[2]*rel;	        fgetpar("fh0",&f[3]);
	if(npar==0) {
	   ierr++;
	   warn(__FILE__,__LINE__,"must specify fl or fh at least");
	}
	for (c=0; c<=2; c++){
	   if(f[c]>f[c+1]){
	      ierr++;
	      warn(__FILE__,__LINE__,"fl0<=fl<=fh<=fh0 condition is violated");
	   }
	}

D 2
	lev=0.9;	fgetpar("lev",&lev);
	del=0.1;	fgetpar("del",&del);
	if ((del<0.0)||(del>lev)||(lev>1.0)) {
E 2
I 2
	levi=0.9;	fgetpar("lev",&levi);
	deli=0.1;	fgetpar("del",&deli);
	if ((deli<0.0)||(deli>levi)||(levi>1.0)) {
E 2
	   ierr++;
	   warn(__FILE__,__LINE__,"0<del<lev<1 condition is violated");
	}

D 2
	ord=0;   	igetpar("ord",&ord);
E 2
I 2
	ordi=0;   	igetpar("ord",&ordi);
	lev=levi;	del=deli;	ord=ordi;
E 2
	if (phase==0) {
D 2
		lev=sqrt(&lev); del=sqrt(&del); ord=ord*0.5+0.6;
E 2
I 2
		lev=sqrt(levi); del=sqrt(deli); ord=ordi*0.5+0.6;
E 2
	}	

	if (verbose){

	/* OPTIONS&PARAMETERS PRINT */

D 3
		fprintf(stderr,"\t\tOPTIONS:\n\n");
D 2
		fprintf(stderr,"%s-phase filter\n",*phstr);
E 2
I 2
		fprintf(stderr,"%s-phase filter\n",phstr);
E 3
I 3
		fprintf(stderr,"\t\tOPTIONS:\n");
		fprintf(stderr,"%s-phase filter\n\n",phstr);
E 3
E 2
		
D 3
		fprintf(stderr,"\t\tPARAMETERS:\n\n");
E 3
I 3
		fprintf(stderr,"\t\tPARAMETERS:\n");
E 3
D 2
		fprintf(stderr,"bandpass frequencies");
		fprintf(stderr,"fl0=%f fl=%f fh=%f fh0=%f\n",f[0],f[1],f[2],f[3]);
		fprintf(stderr,"passband level  lev=%f\n",lev);
		fprintf(stderr,"stopband level  del=%f\n",del);
		fprintf(stderr,"filter order ord=%d\n",ord);
E 2
I 2
		fprintf(stderr,"bandpass frequencies\n");
		fprintf(stderr,"fl0=%-5.1f fl=%-5.1f fh=%-5.1f fh0=%-5.1f\n",f[0],f[1],f[2],f[3]);
		fprintf(stderr,"passband level  lev=%4.2f\n",levi);
		fprintf(stderr,"stopband level  del=%4.2f\n",deli);
D 3
		fprintf(stderr,"filter order ord=%d\n",ordi);
E 3
I 3
		fprintf(stderr,"filter order ord=%d\n\n",ordi);
E 3
E 2
	}

	if (ierr>0) selfdoc();
}

/*******************************************************************/
#ifdef HP
#define _PRECHEB_ precheb
#define _POLCHEB_ polcheb
#define _RECFORM_ recform
#define _FILREC_ filrec
#else
#define _PRECHEB_ precheb_
#define _POLCHEB_ polcheb_
#define _RECFORM_ recform_
#define _FILREC_ filrec_
#endif

/*******************************************************************/
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
D 2
	float wl,wh;
E 2
I 2
	float wl,wh,fnyq;
E 2
	float *poles;
D 2
/*      *aatrout = atrin; */
E 2

D 2
/*	MEMORY ALLOCATION for OUTTRACE in MAINSEQ */
        dt = abh->dt/1000000.0;
        ns = abh->ns;
E 2
I 2
/*	MEMORY ALOCATION for OUTTRACE in MAINSEQ	*/
/*        *aatrout = atrin;	*/
E 2

I 2
           dt = abh->dt*0.000001;   /* from microsec to sec */
           ns = abh->ns;           /* number of trace samples */


	fnyq = 0.5/dt;
	if (f[1]>fnyq) {
		warn(__FILE__,__LINE__,"fl<fnyquist condition is violated");
		selfdoc();
	}
	if (f[2]>fnyq) f[2] = fnyq;
	if (f[3]>fnyq) f[3] = fnyq;


E 2
	/*  FREQuency CORRECTION and ORDER n EVALUATION
                        for Chebyshev recursive filter */
	_PRECHEB_(f,&dt,&lev,&del,&ord,&npol,&wl,&wh);

I 2
	if (ord==0) {
		warn(__FILE__,__LINE__,"ERROR ! ZERO filter order !");
		selfdoc();
	}
	if (verbose) {
D 3
		fprintf(stderr,"RECURSIVE FILTER of %d ORDER\n",ord);
E 3
I 3
		fprintf(stderr,"\nRECURSIVE FILTER of %d ORDER\n\n",ord);
E 3
	}

E 2
	/* COPMUTATION of POLES LOCATION
D 2
	poles = (float*) malloc(2*npol*sizeof(float));
E 2
                        for Chebyshev recursive filter */
I 2
        poles = (float*) malloc(4*npol*sizeof(float));

E 2
	_POLCHEB_(&lev,&ord,&npol,poles);

	/* MEMORY ALLOCATION for RECURSIVE OPERATOR pcof */
	pncof = (NCOEF*) malloc(npol*sizeof(NCOEF));
	pcof = (COEF*) malloc(npol*sizeof(COEF));

	/* RECURSIVE OPERATOR FORMING
			in the cascade form */
	_RECFORM_(f,&dt,&wl,&wh,&ord,&npol,poles,pncof,pcof);
}

/*******************************************************************/
/* TRACE SEQUENTIAL FILTERING PROCESSING */
trseq(itr,trin,trout,abh)
int itr;
Sutrace *trin,*trout;
Subhed *abh;
{
I 2
D 3
	static float *buftr;
E 3
I 3
	static float *primout;
	float *bufin,*bufout,*work;
E 3
E 2
	int ipol,dir=1;
	NCOEF *lalb;
	COEF *ab;

I 2
D 3
	if (itr==0) {
	/*	MEMORY ALLOCATION for BUFFERTRACE	*/
	   if (phase==0) buftr = (float*) malloc(abh->ns * abh->esize);
	   else buftr = trout->data;
	}
E 3
I 3
	if (itr==0) primout = trout->data;
E 3

I 3
	bufin = primout;
	bufout = trin->data;

E 3
E 2
	for (ipol=0; ipol<npol; ipol++) {
		lalb = pncof + ipol;
		ab = pcof + ipol;
D 2
		_FILREC_(lalb->la,ab->a,lalb->lb,ab->b,
			 &ns,trin->data,&dir,trout->data);
E 2
I 2
D 3
		_FILREC_(&(lalb->la),&(ab->a),&(lalb->lb),&(ab->b),
			 &ns,trin->data,&dir,buftr);
E 3
I 3
		work = bufin;
		bufin = bufout;
		bufout = work;
		_FILREC_(&(lalb->la),ab->a,&(lalb->lb),ab->b,
			 &ns,bufin,&dir,bufout);
E 3
E 2
	}

D 3
	if (phase==1) return(1);
E 3
I 3
	if (phase==1) {
		trout->data = bufout;
		return(1);
	}
E 3

D 2
	dir=-1;
E 2
I 2
	dir = -1;
E 2
         /* REVERSE filtering for ZERO-phase recursive operator */
	for (ipol=0; ipol<npol; ipol++) {
		lalb = pncof + ipol;
		ab = pcof + ipol;
D 2
		_FILREC_(lalb->la,ab->a,lalb->lb,ab->b,
			 &ns,trin->data,&dir,trout->data);
E 2
I 2
D 3
		_FILREC_(&(lalb->la),&(ab->a),&(lalb->lb),&(ab->b),
			 &ns,buftr,&dir,trout->data);
E 3
I 3
		work = bufin;
		bufin = bufout;
		bufout = work;
		_FILREC_(&(lalb->la),ab->a,&(lalb->lb),ab->b,
			 &ns,bufin,&dir,bufout);
E 3
E 2
	}

I 3
	trout->data = bufout;
E 3
	return(1);
}

/*******************************************************************/
postp(){}

/*******************************************************************/
E 1
