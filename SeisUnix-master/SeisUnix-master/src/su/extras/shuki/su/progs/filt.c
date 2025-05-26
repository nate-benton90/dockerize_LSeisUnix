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
extern char *SccsId;
 
static char lSccsId[]="@(#)filt.c	1.8\t11/15/88\n";


static float f[4];
static int mlop;

static char *lsdoc = 				
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
	lop=max.length of operator(in samples)	\n\
		(by default lop=64)		\n\
						\n";
/*******************************************************************/

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c,npar,ierr=0;
	float rel=1.25;

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
	mlop=64; 	igetpar("lop",&mlop);
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
/*      *aatrout = atrin; */
}

#ifdef HP
#define _OPERHAN_ operhan
#define _CONVV_   convv
#else
#define _OPERHAN_ operhan_
#define _CONVV_   convv_
#endif

/* TRACE SEQUENTIAL FILTERING PROCESSING */
trseq(itr,trin,trout,abh)
int itr;
Sutrace *trin,*trout;
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

	/* copy of trace header excluding data address */
	bcopy((char*)trin,(char*)trout,sizeof(Sutrace)-sizeof(float*));

	/*  CONVOLUTION  */
   	   _CONVV_(&lop,&shift,oper,&ns,trin->data,trout->data); 
	return(1);
}

postp(){}
