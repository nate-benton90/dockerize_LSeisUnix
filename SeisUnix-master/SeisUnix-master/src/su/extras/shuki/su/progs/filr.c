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
 
static char lSccsId[]="@(#)filr.c	1.6\t6/25/88\n";


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
 on base of CHEBYSHEV polynomials		\n\
						\n\
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
	          fl0=fl/1.25    del-  |***|****|***|   \n\
	          fh0=fh*1.25)      |-*|***|****|***|*->\n\
				    0 fl0 fl    fh fh0	\n\
		Note! 0<=fl0<=fl<=fh<=fh0		\n\
							\n\
	lev=level at the passband edges (fl,fh)		\n\
		(by default: lev=0.9)			\n\
							\n\
	del=level at the stopband edges (fl0,fh0)	\n\
		(by default: del=0.1)			\n\
		Note! del<lev				\n\
							\n\
	ord=order of recursive Chebyshev filter         \n\
	        (by default: ord is automatic chosen)	\n\
							\n";

/*******************************************************************/
/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c,npar,ordi,ierr=0;
	float deli,levi,rel=1.25;
	char *phstr;

	sdoc = lsdoc;
         SccsId = lSccsId;


	phstr = "zero";
	phase=0;
	/* GET OPTIONS */
	while( (c=getopt(xargc,xargv,"v m"))!=EOF) {
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

	levi=0.9;	fgetpar("lev",&levi);
	deli=0.1;	fgetpar("del",&deli);
	if ((deli<0.0)||(deli>levi)||(levi>1.0)) {
	   ierr++;
	   warn(__FILE__,__LINE__,"0<del<lev<1 condition is violated");
	}

	ordi=0;   	igetpar("ord",&ordi);
	lev=levi;	del=deli;	ord=ordi;
	if (phase==0) {
		lev=sqrt(levi); del=sqrt(deli); ord=ordi*0.5+0.6;
	}	

	if (verbose){

	/* OPTIONS&PARAMETERS PRINT */

		fprintf(stderr,"\t\tOPTIONS:\n");
		fprintf(stderr,"%s-phase filter\n\n",phstr);
		
		fprintf(stderr,"\t\tPARAMETERS:\n");
		fprintf(stderr,"bandpass frequencies\n");
		fprintf(stderr,"fl0=%-5.1f fl=%-5.1f fh=%-5.1f fh0=%-5.1f\n",f[0],f[1],f[2],f[3]);
		fprintf(stderr,"passband level  lev=%4.2f\n",levi);
		fprintf(stderr,"stopband level  del=%4.2f\n",deli);
		fprintf(stderr,"filter order ord=%d\n\n",ordi);
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
	float wl,wh,fnyq;
	float *poles;

/*	MEMORY ALOCATION for OUTTRACE in MAINSEQ	*/
/*        *aatrout = atrin;	*/

           dt = abh->dt*0.000001;   /* from microsec to sec */
           ns = abh->ns;           /* number of trace samples */


	fnyq = 0.5/dt;
	if (f[1]>fnyq) {
		warn(__FILE__,__LINE__,"fl<fnyquist condition is violated");
		selfdoc();
	}
	if (f[2]>fnyq) f[2] = fnyq;
	if (f[3]>fnyq) f[3] = fnyq;


	/*  FREQuency CORRECTION and ORDER n EVALUATION
                        for Chebyshev recursive filter */
	_PRECHEB_(f,&dt,&lev,&del,&ord,&npol,&wl,&wh);

	if (ord==0) {
		warn(__FILE__,__LINE__,"ERROR ! ZERO filter order !");
		selfdoc();
	}
	if (verbose) {
		fprintf(stderr,"\nRECURSIVE FILTER of %d ORDER\n\n",ord);
	}

	/* COPMUTATION of POLES LOCATION
                        for Chebyshev recursive filter */
        poles = (float*) malloc(4*npol*sizeof(float));

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
	static float *primout;
	float *bufin,*bufout,*work;
	int ipol,dir=1;
	NCOEF *lalb;
	COEF *ab;

	if (itr==0) primout = trout->data;

	bufin = primout;
	bufout = trin->data;

	for (ipol=0; ipol<npol; ipol++) {
		lalb = pncof + ipol;
		ab = pcof + ipol;
		work = bufin;
		bufin = bufout;
		bufout = work;
		_FILREC_(&(lalb->la),ab->a,&(lalb->lb),ab->b,
			 &ns,bufin,&dir,bufout);
	}

	if (phase==1) {
		trout->data = bufout;
		return(1);
	}

	dir = -1;
         /* REVERSE filtering for ZERO-phase recursive operator */
	for (ipol=0; ipol<npol; ipol++) {
		lalb = pncof + ipol;
		ab = pcof + ipol;
		work = bufin;
		bufin = bufout;
		bufout = work;
		_FILREC_(&(lalb->la),ab->a,&(lalb->lb),ab->b,
			 &ns,bufin,&dir,bufout);
	}

	trout->data = bufout;
	return(1);
}

/*******************************************************************/
postp(){}

/*******************************************************************/
