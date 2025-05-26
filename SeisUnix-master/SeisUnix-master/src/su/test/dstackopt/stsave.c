#include <math.h>
#include <stdio.h>
#define pi 3.14159265
#include <segy.h>
#include "stackopt.h"
#define MAX(a,b) a>b?a:b
struct stst {
	int station; 
	float shift;
};
char *sdoc = "stsave <stdin >stdout tbl=tbl shots=shots phones=phones\n";
int xargc; char **xargv;
main(ac,av)
int ac; char **av;
{
	int nt,ns,ng,j,fd,ntr,i,missing;
	float *sstat,*gstat,d,getst();
	struct segy tr;
	struct stst *sxsst,*gxgst;
	yhsg *tbl;
	char fname[100];

	xargc=ac; xargv=av;
	if(!gettr_(&tr)) err("cant read first trace\n");
	nt = tr.ns;
/* 	fprintf(stderr,"nt=%d\n",nt); */

	if(!getpar("tbl","s",fname)) strcpy(fname,"tbl");
	fd = open(fname,0);
	if(fd==-1) err("can't open tbl=%s\n",fname);
	ntr = fsize(fd);
	tbl = (yhsg*)alloc(ntr);
	read(fd,tbl,ntr);
	ntr /= sizeof(yhsg);
	close(fd);
	fprintf(stderr,"table of %d traces read\n",ntr);

	for(j=0,ns=0,ng=0;j<ntr;j++) {
		ns = MAX(tbl[j].s,ns);
		ng = MAX(tbl[j].g,ng);
	}
	ns++;
	ng++;
	fprintf(stderr,"ns=%d\tng=%d\n",ns,ng);

	sxsst = (struct stst*) alloc(ns*8);
	gxgst = (struct stst*) alloc(ng*8);

	for(i=0;i<ng;i++) {
		for(j=0,missing=1;j<ntr;j++) if(i==tbl[j].g) {
			gxgst[i].station = tbl[j].gx;
			gxgst[i].shift = tbl[j].gst;
			missing=0;
			continue;
		}
		if(missing) {
			gxgst[i].station = -777;
			gxgst[i].shift = -777;
			fprintf(stderr,"phone index %d in missing\n",i);
		}
/* 		fprintf(stderr,"gxgst[%d].station=%d\n",i,gxgst[i].station); */
	}
	for(i=0;i<ns;i++) {
		for(j=0,missing=1;j<ntr;j++) if(i==tbl[j].s) {
			sxsst[i].station = tbl[j].sx;
			sxsst[i].shift = tbl[j].sst;
			missing=0;
			continue;
		}
		if(missing) {
			sxsst[i].station = -777;
			sxsst[i].shift = -777;
			fprintf(stderr,"shot index %d in missing\n",i);
		}
/* 		fprintf(stderr,"sxsst[%d].station=%d\n",i,sxsst[i].station); */
	}

	if(!getpar("shots","s",fname)) strcpy(fname,"shots");
	fd = creat(fname,0644);
	if(fd>2) {
		sstat = (float*) alloc(ns*4);
		for(j=0;j<ns;j++)
			sstat[j] = sxsst[j].shift;
		write(fd,sstat,ns*4);
		close(fd);
	}
	else fprintf(stderr,"can't open shots=%s\n",fname);

	if(!getpar("phones","s",fname)) strcpy(fname,"phones");
	fd = creat(fname,0644);
	if(fd>2) {
		gstat = (float*) alloc(ng*4);
		for(j=0;j<ng;j++)
			gstat[j] = gxgst[j].shift;
		write(fd,gstat,ng*4);
		close(fd);
	}
	else fprintf(stderr,"can't open phones=%s\n",fname);

	do {
		d = getst(sxsst,tr.sx,ns);
		tr.srstat = d*tr.dt;
		d = getst(gxgst,tr.gx,ng);
		tr.grstat = d*tr.dt;
		puttr_(&tr);
	} while(gettr_(&tr));
	exit(0);
}

float getst(sxsst,sx,ns)
struct stst *sxsst;
{
	while(ns--) if(sx==sxsst[ns].station) return(sxsst[ns].shift);
	if(!ns) fprintf(stderr,"station %d not found\n",sx);
	return(0.0);
}
