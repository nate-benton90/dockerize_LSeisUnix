#include "../include/su.h"
#include "../include/hdrs.h"

char *sdoc = "suahed nt= dt= < input > output \n\
 syahed puts a segy header on each trace in inputfile\n\
 dt is sample interval in seconds ";

int xargc;
char **xargv;
bool verbose;
char *SccsId[]="@(#)suahed.c	1.5\t11/15/88\n";


main(ac,av)
int ac; char **av;
{
	int infd,outfd,nb;
	float dt;
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;
	verbose = true;

	infd = input();
	outfd = output();

	if(!igetpar("nt",&(bh.ns))) err(__FILE__,__LINE__,"need nt=");
	bh.esize = sizeof(float);

	tr.data = (float*) malloc(bh.ns*bh.esize);

/* 	tr.ns = bh.ns; */

	if(!fgetpar("dt",&dt)) err(__FILE__,__LINE__,"need dt=");
	if(dt>0.1) warn(__FILE__,__LINE__,"dt=%f seconds",dt);
	bh.dt = dt*1000000;
/* 	tr.dt = bh.dt; */

/* 	nb = HDRBYTES + tr.ns*sizeof(float); */
	nb = bh.ns*sizeof(float);

	hislog(outfd);

	putbh(outfd,&bh);

	tr.tracl = 0;
	while(pread(infd,tr.data,nb)) {
		puttr(outfd,&tr) ;
		tr.tracl++;
	}
	exit(0);
/* 	hlog(""); */
}
