#include <stdio.h>
#include <math.h>
#include "../include/su.h"
#define pi 3.14159265
/* #define MIN(a,b) a<b?a:b */
/* #define MAX(a,b) a>b?a:b */
int xargc; char **xargv;
bool verbose;
int alen;
/* char *logname(); */
char *sdoc = "suspike nt=100 ntr=1 dt=0.004 t0=nt/2 x0=0\n";
char *SccsId="@(#)susyn.c       1.5 7/4/88\n";

main(ac,av)
char **av;
{
	int nt,ntr,nt0,*t0,nx0,*x0,itr,nspike,ispike,j,dcdp;
	float dt,*buff;
	int outfd;
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;

	if(isatty(1)) selfdoc();

	verbose = true;

	outfd = output();

	nspike = maxgetpar();
	t0 = (int*)malloc(nspike*sizeof(int));
	x0 = (int*)malloc(nspike*sizeof(int));

	nt = 100;	igetpar("nt",&nt);		tr.ns = nt;
	dt = 0.004;	fgetpar("dt",&dt);		tr.dt = dt*1000000;
	dcdp = 25;	igetpar("dcdp",&dcdp);

	nt0=igetpar("t0",t0);
	if(nt0==0) {
		nt0=1;
		t0[0] = nt/2;
	}
	nx0 = igetpar("x0",x0);
	if(nx0==0) {
		nx0=1;
		x0[0] = 0;
	}
	ntr = nx0;	igetpar("ntr",&ntr);		tr.ntr = ntr;

	tr.ns = nt;			/* <-- */
	tr.dt = dt*1000000.0 + 0.5;	/* <-- */
	tr.data = (float*)malloc(nt*sizeof(float));
	tr.offset=800;

	/* ASCII HEADER */
	hispr(outfd,"LINE NAME: S%d\n",getpid());
	hispr(outfd,"AREA:      Holon (spikes)\n");
/* 	hispr(outfd,"CLIENT:    %s\n",logname()); */
	j = time(0);
	hispr(outfd,"DATE:      %s",ctime(&j));

	/* BINARY HEADER */
	bfill(0xff,&bh,sizeof(bh));
	bh.ns = nt;
	bh.dt = dt*1000000.0 + 0.5;
	bh.esize = sizeof(float);
	sprintf(bh.name,"S%d",getpid());
	sprintf(bh.area,"Holon");
/* 	sprintf(bh.client,logname()); */
	putbh(outfd,&bh);

	/* TRACES */
	buff = (float*)calloc(ntr*nt,sizeof(float));

	for(ispike=0;ispike<nspike;ispike++) {
		buff[x0[ispike]*nt+t0[ispike]] = 1.0;
	}

	for(itr=0,ispike=0;itr<ntr;itr++) {

		bcopy(buff+itr*nt,tr.data,nt*sizeof(float));

		tr.tracl = itr;
		tr.cdp = itr*dcdp;
		puttr(outfd,&tr);
	}

	exit(0);
}
