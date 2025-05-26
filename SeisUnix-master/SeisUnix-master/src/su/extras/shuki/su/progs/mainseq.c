/*
 * mainseq - Generic main for trace-sequential application programs
 */

#include <stdio.h>
#include "../include/su.h"

int xargc;				/* THIS EXTERNALS ARE ESSENTIAL	*/
char *sdoc,**xargv;			/* TO LINK THIS MODULE TO THE	*/
bool verbose;				/* SU LIBRARY			*/
bool suout=true;
char *SccsId;

main(ac,av)
int ac; char **av;
{
	int infd,outfd,itr,nr,nbpt;
	Sutrace *atrin,*atrout;
	Subhed bh;

	/* INITIALIZATIONS	*/
	xargc = ac; xargv = av;
	inits();

	/* OPEN FILES	*/
	infd = input();	
	if(suout) outfd = output();
	else outfd = -1;

	/* ASCII HEADER	*/
	apass(infd,outfd);
	hislog(outfd);

	/* GET BINARY HEADER	*/
	getbh(infd,&bh);

	/* MEMORY ALLOCATION */
	atrin  = (Sutrace*) malloc(sizeof(Sutrace));
	if(atrin==NULL) err(__FILE__,__LINE__,"Can't malloc atrin");
	atrin->data  = (float*) malloc(bh.ns*bh.esize);
	if(atrin->data==NULL) err(__FILE__,__LINE__,"Can't malloc atrin.data");

	atrout = (Sutrace*)NULL;

	/* FIRST GETTR */
	nr = gettr(infd,atrin);
	nbpt = NTRHB+bh.ns*sizeof(float);
	if(nr!=nbpt)
	  err(__FILE__,__LINE__,"First Gettr() returned %d, should be %d",nr,nbpt);

	/* PRE PROCESSING */
	prep(&bh,atrin,&atrout,infd,outfd);
	if(atrout==NULL) {
		atrout = (Sutrace*) malloc(sizeof(Sutrace));
		if(atrout==NULL) err(__FILE__,__LINE__,"Can't malloc atrout");
		atrout->data = (float*) malloc(bh.ns*bh.esize);
		if(atrout->data==NULL) err(__FILE__,__LINE__,"Can't malloc atrout.data");
	}

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

	exit(0);
}
