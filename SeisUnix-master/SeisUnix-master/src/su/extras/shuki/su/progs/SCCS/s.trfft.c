h05234
s 00000/00000/00108
d D 1.11 88/11/15 14:03:10 shuki 11 10
c 
e
s 00010/00012/00098
d D 1.10 88/06/22 14:58:03 shuki 10 9
c cosmetics
e
s 00001/00001/00109
d D 1.9 88/06/06 13:12:16 shuki 9 8
c Cancel ns in trace headers
e
s 00006/00000/00104
d D 1.8 88/05/25 14:54:14 shemer 8 7
c with SccsId[]
e
s 00026/00007/00078
d D 1.7 88/05/24 16:41:01 valer 7 6
c 
e
s 00000/00000/00085
d D 1.6 88/05/08 17:19:55 valer 6 5
c 
e
s 00001/00001/00084
d D 1.5 88/04/28 09:47:21 valer 5 4
c 
e
s 00002/00002/00083
d D 1.4 88/04/24 14:25:45 valer 4 3
c 
e
s 00001/00001/00084
d D 1.3 88/04/24 12:19:55 valer 3 2
c without bzero(outrace)
e
s 00000/00000/00085
d D 1.2 88/04/24 11:35:21 valer 2 1
c test
e
s 00085/00000/00000
d D 1.1 88/04/20 12:46:33 valer 1 0
c date and time created 88/04/20 12:46:33 by valer
e
u
U
f e 0
t
T
I 1
/*
 * FFT of trace
 */

#include <stdio.h>
#include <math.h>
D 5
#include "/src/su/include/su.h"
E 5
I 5
#include "../include/su.h"
E 5

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
static int phase;
I 7
static int ns,ns2,nsout;
static float dt;
I 8
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";
E 8
E 7

I 8

E 8
D 10
static char *lsdoc = 				
"sufft [-v][-p]   				 	\n\
	out(f) = FFT(in(t))			\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
	-p	evaluate only phase spectrum	\n\
		(by default only amplitude	\n\
				 spectrum)	\n\
						\n\
PARAMETERS:  no parameters			\n\
						\n";
E 10
I 10
static char *lsdoc =
"sufft [-v][-p]\n\
    out(f) = FFT(in(t))                                               \n\
                                                                      \n\
OPTIONS:                                                              \n\
    -v turn verbose on                                                \n\
    -p evaluate only phase spectrum (default only amplitude spectrum) \n\
                                                                      \n\
PARAMETERS:  no parameters                                            \n";
E 10

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
I 8
        SccsId = lSccsId;

E 8

	/* GET OPTIONS */
	phase=0;
	while( (c=getopt(xargc,xargv,"v p"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case 'p':
			phase=1;
			break;
		case '?':
			warn("getopt returned '?'");
		}
	}

	/* GET PARAMETERS */
}
I 7

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{                    
        /* OUTTRACE ON THE PLACE OF INTRACE */
      /*	*aatrout = atrin;	*/
	dt = abh->dt/1000000.0;
	ns = abh->ns;
	for (ns2=1; ns2<ns; ns2 *=2);
	nsout = ns2/2 + 2;
	abh->ns = nsout;
}

E 7
/* TRACE SEQUENTIAL FFT-PROCESSING */
D 7
trseq(itr,trin,abh,trout)
E 7
I 7
trseq(itr,trin,trout,abh)
E 7
int itr;
Sutrace *trin,*trout;
Subhed *abh;
{
D 7
	static int ns,ns2;	
	static float dt;
E 7
	int i,j,k;	
	double atan2(),sqrt(),af,bf;

	/* FIRST TRACE	*/
	if(itr==0) {
D 7
		dt = abh->dt/1000000.0;
	   	ns = abh->ns;
		for (ns2=1; ns2<ns; ns2 *=2);
E 7
D 3
		bzero((char*)trout->data,ns*sizeof(float));
E 3
I 3
D 4
/*		bzero((char*)trout->data,ns*sizeof(float));	*/
E 4
I 4
		trin->data = (float*)realloc(trin->data,(ns2+2)*sizeof(float));
D 7
		bzero((char*)trout->data,ns*sizeof(float));	
E 7
I 7
		bzero((char*)trout->data,nsout*sizeof(float));
E 7
E 4
E 3
	}

I 7
        /* copy of trace header excluding data address */
        bcopy((char*)trin,(char*)trout,sizeof(Sutrace)-sizeof(float*));
D 9
	trout->ns = nsout;
E 9
I 9
/* 	trout->ns = nsout; */
E 9

E 7
	/*	ADD ZERO's TO THE TAIL OF TRACE		*/
D 4
	trin->data = (float*)realloc(trin->data,(ns2+2)*sizeof(float));
E 4
	bzero((char*)(trin->data+ns),(ns2+2-ns)*sizeof(float));

	refft(trin->data,ns2,1,2);
I 10

E 10
	for (i=0,j=0; i<=ns2; i+=2,j++) {
		af = *(trin->data+i);
		bf = *(trin->data+i+1);
		if (phase==0)
			*(trout->data+j) = sqrt(af*af + bf*bf); 
		else
			*(trout->data+j) = atan2(bf,af);       
	}
	return(1);
}
I 7
 
postp(){}
E 7
E 1
