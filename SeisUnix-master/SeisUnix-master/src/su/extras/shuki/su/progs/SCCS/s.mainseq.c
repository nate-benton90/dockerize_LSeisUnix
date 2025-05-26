h60222
s 00000/00000/00075
d D 1.6 88/11/15 14:02:31 shuki 6 5
c 
e
s 00002/00000/00073
d D 1.5 88/10/24 10:01:37 shuki 5 4
c 
e
s 00000/00000/00073
d D 1.4 88/05/25 14:53:43 shemer 4 3
c with SccsId[]
e
s 00001/00000/00072
d D 1.3 88/05/25 13:02:17 shuki 3 2
c SccsId
e
s 00047/00012/00025
d D 1.2 88/05/25 06:55:19 shuki 2 1
c umainseq
e
s 00037/00000/00000
d D 1.1 88/04/14 13:52:29 shuki 1 0
c date and time created 88/04/14 13:52:29 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * mainseq - Generic main for trace-sequential application programs
 */

I 2
#include <stdio.h>
E 2
#include "../include/su.h"

int xargc;				/* THIS EXTERNALS ARE ESSENTIAL	*/
char *sdoc,**xargv;			/* TO LINK THIS MODULE TO THE	*/
bool verbose;				/* SU LIBRARY			*/
I 2
bool suout=true;
I 3
char *SccsId;
E 3
E 2

main(ac,av)
int ac; char **av;
{
D 2
	int infd,outfd,itr;
	Sutrace tr;
E 2
I 2
	int infd,outfd,itr,nr,nbpt;
	Sutrace *atrin,*atrout;
E 2
	Subhed bh;

D 2
	xargc = ac; xargv = av;			/* INITIALIZATIONS	*/
	inits(ac,av);
E 2
I 2
	/* INITIALIZATIONS	*/
	xargc = ac; xargv = av;
	inits();
E 2

D 2
	infd = input();					/* OPEN FILES	*/
	outfd = output();
E 2
I 2
	/* OPEN FILES	*/
	infd = input();	
	if(suout) outfd = output();
	else outfd = -1;
E 2

D 2
	apass(infd,outfd);			/* PASS ASCII HEADER	*/
E 2
I 2
	/* ASCII HEADER	*/
	apass(infd,outfd);
	hislog(outfd);
E 2

D 2
	bpass(infd,outfd,&bh);			/* PASS BINARY HEADER	*/
E 2
I 2
	/* GET BINARY HEADER	*/
	getbh(infd,&bh);
E 2

D 2
				/* DYNAMIC TRACE MEMORY ALLOCATION	*/
	tr.data = (float*) malloc(bh.ns*bh.esize);
E 2
I 2
	/* MEMORY ALLOCATION */
	atrin  = (Sutrace*) malloc(sizeof(Sutrace));
	if(atrin==NULL) err(__FILE__,__LINE__,"Can't malloc atrin");
	atrin->data  = (float*) malloc(bh.ns*bh.esize);
	if(atrin->data==NULL) err(__FILE__,__LINE__,"Can't malloc atrin.data");
E 2

I 5
	atrout = (Sutrace*)NULL;

E 5
D 2
	for(itr=0;gettr(infd,&tr)!=0;itr++) {		/* MAIN LOOP	*/
E 2
I 2
	/* FIRST GETTR */
	nr = gettr(infd,atrin);
	nbpt = NTRHB+bh.ns*sizeof(float);
	if(nr!=nbpt)
	  err(__FILE__,__LINE__,"First Gettr() returned %d, should be %d",nr,nbpt);
E 2

D 2
		if(trseq(itr,&tr,&bh)) puttr(outfd,&tr);
E 2
I 2
	/* PRE PROCESSING */
	prep(&bh,atrin,&atrout,infd,outfd);
	if(atrout==NULL) {
		atrout = (Sutrace*) malloc(sizeof(Sutrace));
		if(atrout==NULL) err(__FILE__,__LINE__,"Can't malloc atrout");
		atrout->data = (float*) malloc(bh.ns*bh.esize);
		if(atrout->data==NULL) err(__FILE__,__LINE__,"Can't malloc atrout.data");
E 2
	}
I 2

	/* PUT BINARY HEADER */
	if(suout) putbh(outfd,&bh);

	/* MAIN PROCESSING LOOP */
	itr = 0;
	do {
		if(trseq(itr,atrin,atrout,&bh))
			puttr(outfd,atrout);
		itr++;
	} while( gettr(infd,atrin) );

	/* POST PROCESSING */
	postp();
E 2

	exit(0);
}
E 1
