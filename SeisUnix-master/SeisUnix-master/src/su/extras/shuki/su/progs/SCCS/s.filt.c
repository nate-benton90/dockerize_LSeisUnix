h18384
s 00000/00000/00130
d D 1.8 88/11/15 14:02:28 shuki 8 7
c 
e
s 00039/00035/00091
d D 1.7 88/06/22 12:50:40 valer 7 6
c 
e
s 00006/00000/00120
d D 1.6 88/05/25 14:53:40 shemer 6 5
c with SccsId[]
e
s 00016/00001/00104
d D 1.5 88/05/24 13:14:47 valer 5 4
c 
e
s 00003/00003/00102
d D 1.4 88/05/15 15:53:21 valer 4 3
c 
e
s 00000/00000/00105
d D 1.3 88/05/08 17:18:12 valer 3 2
c 
e
s 00003/00004/00102
d D 1.2 88/04/28 17:24:28 valer 2 1
c outtrace independent
e
s 00106/00000/00000
d D 1.1 88/04/14 13:52:27 shuki 1 0
c date and time created 88/04/14 13:52:27 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * convolution of trace with operator of band pass filter
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
static float f[4];
static int mlop;

static char *lsdoc = 				
D 7
"sufilt [-v fl0=0 fl=0 fh=fN fh0=fN ]   \\ 	\n\
D 4
	 [lop=511] <stdin >stdout		\n\
E 4
I 4
	 [lop=64] <stdin >stdout		\n\
E 4
						\n\
	out(t) = conv(in(t),op(t))		\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
						\n\
PARAMETERS:					\n\
	fl0,fl,fh,fh0=frequencies(Hertz) of pass band	\n\
			in the shape       ******	\n\
	       (by default fl0=fl=0       *|****|*	\n\
		    fh=fh0=fNyquist)     **|****|**  	\n\
				    ---|***|****|***|---\n\
				      fl0 fl    fh fh0	\n\
		Note! 0<=fl0<=fl<=fh<=fh0		\n\
							\n\
E 7
I 7
"BAND-PASS FILTERING in TIME DOMAIN		\n\
                                                \n\
 sufilt [options parameters] <stdin >stdout     \n\
                                                \n\
OPTIONS:                                        \n\
        -v      turn verbose on                 \n\
                (by default: off)               \n\
                                                \n\
PARAMETERS:                                     \n\
        fl0,fl,fh,fh0=frequencies(Hertz) of pass band   \n\
                        in the shape       ******       \n\
               (by default: fl=0          *|****|*      \n\
                      fh=fNyquist        **|****|**     \n\
                      fl0=fl/1.25   ---|***|****|***|---\n\
                      fh0=fh*1.25)    fl0 fl    fh fh0  \n\
                Note! 0<=fl0<=fl<=fh<=fh0               \n\
                                                        \n\
E 7
	lop=max.length of operator(in samples)	\n\
D 4
		(by default lop=511)		\n\
E 4
I 4
		(by default lop=64)		\n\
E 4
						\n";
I 7
/*******************************************************************/
E 7

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
D 7
	int c,npar;
E 7
I 7
	int c,npar,ierr=0;
	float rel=1.25;
E 7

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

D 7
	/* GET PARAMETERS */
	f[0]=0.0;	npar = fgetpar("fl0",&f[0]); 	
	f[1]=0.0;	npar += fgetpar("fl",&f[1]);
	f[2]=1000.0;	npar += fgetpar("fh",&f[2]);
	f[3]=1000.0;	npar += fgetpar("fh0",&f[3]);
	if(npar==0) {
			warn(__FILE__,__LINE__,"must specify frequency");
			selfdoc();	
	}
	for (c=0; c<=2; c++){
		if(f[c]>f[c+1]){
			warn(__FILE__,__LINE__,"check frequencies");
			selfdoc();
		}
	}
E 7
I 7
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
E 7
D 4
	mlop=511; 	igetpar("lop",&mlop);
E 4
I 4
	mlop=64; 	igetpar("lop",&mlop);
E 4
D 7
}

E 7
I 7
        if (ierr>0) selfdoc();
}        
/*********************************************************************/
E 7
I 5
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
I 7
        /* OUTTRACE ON THE PLACE OF INTRACE */
E 7
/*      *aatrout = atrin; */
}

E 5
#ifdef HP
#define _OPERHAN_ operhan
#define _CONVV_   convv
#else
#define _OPERHAN_ operhan_
#define _CONVV_   convv_
#endif

/* TRACE SEQUENTIAL FILTERING PROCESSING */
D 2
trseq(itr,atr,abh,trout)
E 2
I 2
D 5
trseq(itr,trin,abh,trout)
E 5
I 5
trseq(itr,trin,trout,abh)
E 5
E 2
int itr;
D 2
float *trout;
Sutrace *atr;
E 2
I 2
Sutrace *trin,*trout;
E 2
Subhed *abh;
{
	static int lop,mid,shift,ns;	
	static float dt;
	static float *oper;

	/* FIRST TRACE	*/
	if(itr==0) {
		dt = abh->dt/1000000.0;
	   	ns = abh->ns;
		oper = (float*) malloc(mlop*sizeof(float));
		/*  CONSTRUCTION OF FILTER OPERATOR  */
		_OPERHAN_(f,&dt,&mlop,oper,&lop,&mid);
		shift = mid - 1;
	}
I 5

	/* copy of trace header excluding data address */
	bcopy((char*)trin,(char*)trout,sizeof(Sutrace)-sizeof(float*));

E 5
	/*  CONVOLUTION  */
D 2
   	   _CONVV_(&lop,&shift,oper,&ns,atr->data,trout); 
E 2
I 2
   	   _CONVV_(&lop,&shift,oper,&ns,trin->data,trout->data); 
E 2
	return(1);
}
I 5

postp(){}
E 5
E 1
