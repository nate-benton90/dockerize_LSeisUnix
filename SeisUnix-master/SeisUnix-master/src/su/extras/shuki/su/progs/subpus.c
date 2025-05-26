/*
 * subpus - Antialias bandpass and under sampling
 */

#include <stdio.h>
/* #include <math.h> */
#include "../include/su.h"

int xargc;				/* THIS EXTERNALS ARE ESSENTIAL	*/
char *sdoc,**xargv;			/* TO LINK THIS MODULE TO THE	*/
bool verbose;				/* SU LIBRARY			*/
char *SccsId[]="@(#)subpus.c	1.4\t11/15/88\n";


main(ac,av)
int ac; char **av;
{
	int infd,outfd,itr;
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;			/* INITIALIZATIONS	*/
	inits();

	infd = input();					/* OPEN FILES	*/
	outfd = output();

	apass(infd,outfd);			/* PASS ASCII HEADER	*/

	bpass2(infd,outfd,&bh);			/* PASS BINARY HEADER	*/

				/* DYNAMIC TRACE MEMORY ALLOCATION	*/
	tr.data = (float*) malloc(bh.ns*bh.esize);

	for(itr=0;gettr(infd,&tr)!=0;itr++) {		/* MAIN LOOP	*/

		if(trseq(itr,&tr,&bh)) puttr(outfd,&tr);
	}

	exit(0);
}

static char *lsdoc = 
"subpus [-v] <stdin >stdout                  \n\
                                             \n\
             {0 if t odd                     \n\
    out(t) = {                               \n\
             {(in(t-1)+2*in(t)+in(t+1))/4    \n\
                                             \n\
OPTIONS:                                     \n\
    -v turn verbose on                       \n\
                                             \n\
PARAMETERS:                                  \n\
                                             \n";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;

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
}

/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
static float buff[10000];	/* <----- */
trseq(itr,atr,abh)
int itr;
Sutrace *atr;
Subhed *abh;
{
/* 	static float *buff; */
	static int nb;

	if(itr==0) {
		nb = 2*abh->ns*abh->esize;
/* 		buff = (float*) malloc(nb); */
	}
	bcopy(atr->data,buff,nb);
	bpus(buff,atr->data,abh->ns);
	abh->ns /= 2;
	abh->dt *= 2;
	return(1);
}

bpus(p,q,ns)
float *p,*q;
int ns;
{
	int i;
	q[0] = 0.25*(3.0*p[0] + p[1]);
	q[ns-1] = 0.25*(3.0*p[2*ns-1]);
	for(i=1;i<ns-1;i++) {
		q[i] = 0.25*(2.0*p[2*i] + p[2*i-1] + p[2*i+1]);
	}
}

/*
 * bpass - pass binary header
 */
bpass2(infd,outfd,abh)
int infd,outfd;
Subhed *abh;
{
	getbh(infd,abh);
	hislog(outfd);
	abh->dt *= 2;
	abh->ns /= 2;
	putbh(outfd,abh);
}
