/*
 * band pass filtering in frequency domain
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId; 
static char lSccsId[]="@(#)filf.c	1.5\t11/15/88\n";


static float f[4];

static char *lsdoc = 				
"BAND-PASS FILTERING in FREQUENCY DOMAIN	\n\
                                                \n\
 sufilf [options parameters] <stdin >stdout     \n\
                                                \n\
OPTIONS:                                        \n\
        -v      turn verbose on                 \n\
		(by default: off) 		\n\
                                                \n\
PARAMETERS:                                     \n\
        fl0,fl,fh,fh0=frequencies(Hertz) of pass band   \n\
                        in the shape       ******       \n\
               (by default: fl=0          *|****|*      \n\
                      fh=fNyquist        **|****|**     \n\
                      fl0=fl/1.25   ---|***|****|***|---\n\
                      fh0=fh*1.25)    fl0 fl    fh fh0  \n\
                Note! 0<=fl0<=fl<=fh<=fh0               \n\
                                                	\n";

/**********************************************************************/
/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c,npar,ierr=0;
	float rel=1.25;

	sdoc = lsdoc;
        SccsId = lSccsId;
  

	/* GET OPTIONS */
	while( (c=getopt(xargc,xargv,"v p"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case '?':
			warn("getopt returned '?'");
		}
	}

        /* GET PARAMETERS */
        f[1]=0.0;       npar = fgetpar("fl",&f[1]);
        f[0]=f[1]/rel;         fgetpar("fl0",&f[0]);
        f[2]=1000.0;    npar += fgetpar("fh",&f[2]);
        f[3]=f[2]*rel;          fgetpar("fh0",&f[3]);
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
        if (ierr>0) selfdoc();
}
/*********************************************************************/

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	/* OUTTRACE ON THE PLACE OF INTRACE */
      *aatrout = atrin;
}

/********************************************************************/
#ifdef HP
#define _WINHAN_  winhan
#else
#define _WINHAN_   winhan_
#endif

/*********************************************************************/
/* TRACE SEQUENTIAL FFT-PROCESSING */
trseq(itr,trin,trout,abh)
int itr;
Sutrace *trin,*trout;
Subhed *abh;
{
	static int ns,ns2,nf;
	static float *spechan;
	static float dt,df;
	int i,j;	

	/* FIRST TRACE	*/
	if(itr==0) {
	   	ns = abh->ns;
		dt = abh->dt*0.000001;   /* from microsec to sec */
		for (ns2=1; ns2<ns; ns2 *=2);
		nf = ns2/2 + 1;		/* number of frequencies */
		df = 0.5/dt/(nf-1);	/* frequency sampling interval */
		/*abh->ns = ns2 + 2;*/

                /* DYNAMIC MEMORY ALLOCATION        */
		trin->data = (float*)realloc(trin->data,(ns2+2)*sizeof(float));
		/*trout->data = trin->data;*/
		/*trout->data = (float*)realloc(trout->data,(ns2+2)*sizeof(float));*/
		/*	for HANNING window */
		spechan = (float*)malloc(nf*sizeof(float));


		/*	HANNING window generating	*/
		_WINHAN_(f,&df,&nf,spechan);
	}

	/*	ADD ZERO's TO THE TRACE TAIL 		*/
	bzero((char*)(trin->data+ns),(ns2+2-ns)*sizeof(float));

	refft(trin->data,ns2,1,2);

	/*	AMPLITUDE SPECTRA MULTIPLICATION	*/
	for (i=0,j=0; i<=ns2; i+=2,j++) {
		*(trout->data+i) = (*(trin->data+i))*(*(spechan+j));
		*(trout->data+i+1) = (*(trin->data+i+1))*(*(spechan+j));
	}

	/*	INVERSE FFT	*/
	refft(trout->data,ns2,-1,-2);
	return(1);
}

/**********************************************************************/
postp(){}

/*********************************************************************/
