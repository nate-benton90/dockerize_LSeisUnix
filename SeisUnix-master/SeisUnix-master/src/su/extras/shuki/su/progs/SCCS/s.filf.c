h15282
s 00000/00000/00148
d D 1.5 88/11/15 14:03:38 shuki 5 4
c 
e
s 00026/00017/00122
d D 1.4 88/06/22 12:50:12 valer 4 3
c 
e
s 00005/00000/00134
d D 1.3 88/05/25 14:54:30 shemer 3 2
c with SccsId[]
e
s 00016/00002/00118
d D 1.2 88/05/24 13:14:50 valer 2 1
c 
e
s 00120/00000/00000
d D 1.1 88/05/18 16:21:06 valer 1 0
c date and time created 88/05/18 16:21:06 by valer
e
u
U
t
T
I 1
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
I 3
extern char *SccsId; 
static char lSccsId[]="%W%\t%G%\n";
E 3

I 3

E 3
static float f[4];

static char *lsdoc = 				
D 4
"sufilf [options parameters] <stdin >stdout     \n\
E 4
I 4
"BAND-PASS FILTERING in FREQUENCY DOMAIN	\n\
E 4
                                                \n\
I 4
 sufilf [options parameters] <stdin >stdout     \n\
E 4
                                                \n\
OPTIONS:                                        \n\
        -v      turn verbose on                 \n\
		(by default: off) 		\n\
                                                \n\
PARAMETERS:                                     \n\
        fl0,fl,fh,fh0=frequencies(Hertz) of pass band   \n\
                        in the shape       ******       \n\
D 4
               (by default: fl0=fl=0      *|****|*      \n\
                    fh=fh0=fNyquist)     **|****|**     \n\
                                    ---|***|****|***|---\n\
                                      fl0 fl    fh fh0  \n\
E 4
I 4
               (by default: fl=0          *|****|*      \n\
                      fh=fNyquist        **|****|**     \n\
                      fl0=fl/1.25   ---|***|****|***|---\n\
                      fh0=fh*1.25)    fl0 fl    fh fh0  \n\
E 4
                Note! 0<=fl0<=fl<=fh<=fh0               \n\
D 4
                                                        \n\
E 4
                                                	\n";

I 4
/**********************************************************************/
E 4
/* INITIALIZE SELF DOCUMENTATION */
inits()
{
D 4
	int c,npar;
E 4
I 4
	int c,npar,ierr=0;
	float rel=1.25;
E 4

	sdoc = lsdoc;
I 3
        SccsId = lSccsId;
  
E 3

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

D 4
	/* GET PARAMETERS */
        f[0]=0.0;       npar = fgetpar("fl0",&f[0]);
        f[1]=0.0;       npar += fgetpar("fl",&f[1]);
E 4
I 4
        /* GET PARAMETERS */
        f[1]=0.0;       npar = fgetpar("fl",&f[1]);
        f[0]=f[1]/rel;         fgetpar("fl0",&f[0]);
E 4
        f[2]=1000.0;    npar += fgetpar("fh",&f[2]);
D 4
        f[3]=1000.0;    npar += fgetpar("fh0",&f[3]);
E 4
I 4
        f[3]=f[2]*rel;          fgetpar("fh0",&f[3]);
E 4
        if(npar==0) {
D 4
                        warn(__FILE__,__LINE__,"must specify frequency");
                        selfdoc();
E 4
I 4
           ierr++;
           warn(__FILE__,__LINE__,"must specify fl or fh at least");
E 4
        }
        for (c=0; c<=2; c++){
D 4
                if(f[c]>f[c+1]){
                        warn(__FILE__,__LINE__,"check frequencies");
                        selfdoc();
                }
E 4
I 4
           if(f[c]>f[c+1]){
              ierr++;
              warn(__FILE__,__LINE__,"fl0<=fl<=fh<=fh0 condition is violated");
           }
E 4
        }
I 4
        if (ierr>0) selfdoc();
E 4
}
I 4
/*********************************************************************/
E 4
I 2

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	/* OUTTRACE ON THE PLACE OF INTRACE */
      *aatrout = atrin;
}

I 4
/********************************************************************/
E 4
E 2
#ifdef HP
#define _WINHAN_  winhan
#else
#define _WINHAN_   winhan_
#endif

I 4
/*********************************************************************/
E 4
/* TRACE SEQUENTIAL FFT-PROCESSING */
D 2
trseq(itr,trin,abh,trout)
E 2
I 2
trseq(itr,trin,trout,abh)
E 2
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
D 2
		trout->data = (float*)realloc(trout->data,(ns2+2)*sizeof(float));
E 2
I 2
		/*trout->data = trin->data;*/
		/*trout->data = (float*)realloc(trout->data,(ns2+2)*sizeof(float));*/
E 2
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
I 2

I 4
/**********************************************************************/
E 4
postp(){}
I 4

/*********************************************************************/
E 4
E 2
E 1
