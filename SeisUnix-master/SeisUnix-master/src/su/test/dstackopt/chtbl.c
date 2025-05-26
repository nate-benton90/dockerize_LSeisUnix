#include <stdio.h>
#include <math.h>
#include <sys/file.h>
#include <segy.h>
#include "stackopt.h"
#define pi 3.14159265
char *sdoc = "schtbl <old_table >new_table insstat=insstat ingstat=ingstat\n";
yhsg *itbl;
int xargc;
char **xargv,verbose[10];
main(ac,av)
int ac; char **av;
{
	int is,ig,j,ny,ns,ng,ntr;
	char fname[100];
	int seed,amp,biasr,fd,i0s,ies,i0g,ieg,lambda;
	float *sst,*gst;

	xargc = ac; xargv = av;

	/* get parameters */
	if(!getpar("ntr","d",&ntr)) err("nead ntr=\n");
	if(!getpar("ns","d",&ns)) err("nead ns=\n");
	if(!getpar("ng","d",&ng)) err("nead ng=\n");
	if(!getpar("ny","d",&ny)) err("nead ny=\n");
	itbl = (yhsg*) alloc(ntr*sizeof(yhsg));
	if(read(0,itbl,ntr*sizeof(yhsg))!=ntr*sizeof(yhsg))
		err("error in reading\n");
	fprintf(stderr,
	    "read itbl:ntr=%d ny=%d ns=%d ng=%d\n",
		ntr,ny,ns,ng);

	/* allocate space */
	sst   = (float*) calloc(ns,4);			/* shot statics     */
	gst   = (float*) calloc(ng,4);			/* geophone statics */

	/* initialize statics */
	/* restart shots */
	if(!getpar("insstat","s",fname)) strcpy(fname,"insstat");
	fd = open(fname,0);
	if(fd == -1) err("can't open insstat=%s\n",fname);
	read(fd,sst,ns*4);
	close(fd);
	/* restart geophones */
	if(!getpar("ingstat","s",fname)) strcpy(fname,"ingstat");
	fd = open(fname,0);
	if(fd == -1) err("can't open ingstat=%s\n",fname);
	read(fd,gst,ng*4);
	close(fd);

	if(getpar("lambda","d",&lambda)) {
		amp = 4;	getpar("amp","d",&amp);
		i0g = 0;	getpar("i0g","d",&i0g);
		i0s = 0;	getpar("i0s","d",&i0s);
		biasr = 0;	getpar("bias","d",&biasr);
		fprintf(stderr,"lambda=%d\tamp=%d\ti0g=%d\ti0s=%d\n",
			lambda,amp,i0g,i0s);
		for(ig=0;ig<ng;ig++)
		   gst[ig] += biasr+(float)amp*cos(2.*pi*(ig-i0g)/lambda);
		for(is=0;is<ns;is++)
		   sst[is] += biasr+(float)amp*cos(2.*pi*(is-i0s)/lambda);
		}
	if(getpar("seed","d",&seed)) {
		amp = 4;	getpar("ampr","d",&amp);
		biasr = amp/2;	getpar("biasr","d",&biasr);
		fprintf(stderr,"seed=%d\tamp=%d\tbiasr=%d\n",seed,amp,biasr);
		srand(seed);

		i0g = 0;	getpar("ri0g","d",&i0g);
		ieg = ng;	getpar("rieg","d",&ieg);
		fprintf(stderr,"i0g=%d ieg=%d\n",i0g,ieg);
		for(ig=i0g;ig<ieg;ig++)
			gst[ig] += rand()%(amp+1) - biasr;

		i0s = 0; getpar("i0s","d",&i0s);
		ies = ns; getpar("ies","d",&ies);
		fprintf(stderr,"i0s=%d ies=%d\n",i0s,ies);
		for(is=i0s;is<ies;is++)
			sst[is] += rand()%(amp+1) - biasr;
		}

	for(j=0;j<ntr;j++) itbl[j].gst = gst[itbl[j].g];
	for(j=0;j<ntr;j++) itbl[j].sst = sst[itbl[j].s];

	write(1,itbl,ntr*sizeof(yhsg));

	exit(0);
}
