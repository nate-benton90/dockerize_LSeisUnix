#include <stdio.h>
#include "stackopt.h"
#include <segy.h>
#define SMALL 1
#define MAX(a,b) a>b?a:b
#define MIN(a,b) a<b?a:b

/* yhsg *itbl; */
/* struct segy tr; */
/* char *sdoc = "getitbl <diskfile"; */
/* int itr,ntr,ntall,ny,nh,ns,ng,nf; */
/* main() */
/* { */
/* 	int j; */
/* 	getitbl(); */
/* 	fprintf(stderr,"ntr=%d ntall=%d ny=%d nh=%d ns=%d ng=%d nf=%d\n", */
/* 		ntr,ntall,ny,nh,ns,ng,nf); */
/* 	for(j=0;j<ntr;j++) */
/* 		fprintf(stderr,"j=%d y=%d h=%d s=%d g=%d\n", */
/* 			j,itbl[j].y,itbl[j].h,itbl[j].s,itbl[j].g); */
/* 	exit(0); */
/* } */

getitbl()
{
	extern struct segy tr;
	extern yhsg *itbl;
	extern ntr,ntall,ny,nh,ns,ng,nf/* ,tblfd */;
	int itr,j,nb,i,nf_, fd;
	int *buff,d_;
	char fname[100];

	fprintf(stderr,"getitbl: ");
	itr = 0;
	ntr = gettra_(&tr,&itr);
	fprintf(stderr,"ntr=%d ",ntr);
	nb = 240 + tr.ns*4;
	ntall =  60 + tr.ns;
	itbl = (yhsg*) alloc (ntr*sizeof(yhsg));
	buff = (int*) alloc (ntr*4);

	j=0;
	do {
		itbl[j].y = tr.cdp;
		itbl[j].h = tr.offset;
		itbl[j].sx = tr.sx;
		itbl[j].gx = tr.gx;
		lseek(0,nb*(++j),0);
	} while (read(0,&tr,240)==240);
	fprintf(stderr,"segy headers scanned\n");

	/* cdps */
		fprintf(stderr,"cdps - ");
	for(j=0;j<ntr;j++)
		buff[j] = itbl[j].y;
	uniq(buff,ntr,&ny);
		fprintf(stderr,"(uniqed, ny=%d) ",ny);
	ibubbles(buff,ny);
		fprintf(stderr,"(sorted)\n");
	findd(buff,ny,&d_);
	if(d_<SMALL) err("dy=%f\n",d_);
	for(j=0;j<ntr;j++)
		itbl[j].y = (itbl[j].y-buff[0])/d_;

	/* offsets */
	/*
		fprintf(stderr,"offsets - ");
	for(j=0;j<ntr;j++)
		buff[j] = itbl[j].h;
	uniq(buff,ntr,&nh);
		fprintf(stderr,"(uniqed, nh=%d) ",nh);
	ibubbles(buff,nh);
	fprintf(stderr,"(sorted)\n");
	findd(buff,nh,&d_);
	if(d_<SMALL) err("dh=%f\n",d_);
	for(j=0;j<ntr;j++)
		itbl[j].h = (itbl[j].h-buff[0])/d_;
	*/

	/* shots */
	fprintf(stderr,"shots - ");
	for(j=0;j<ntr;j++)
		buff[j] = itbl[j].sx;
	uniq(buff,ntr,&ns);
	fprintf(stderr,"(uniqed, ns=%d) ",ns);
	ibubbles(buff,ns);
	fprintf(stderr,"(sorted)\n");
	findd(buff,ns,&d_);
	if(d_<SMALL) err("ds=%f\n",d_);
	if(!getpar("sxs","s",fname)) strcpy(fname,"sxs");
	fd = creat(fname,0644);
	for(j=0;j<ntr;j++) {
		write(fd,&(itbl[j].sx),4);
		itbl[j].s = (itbl[j].sx-buff[0])/d_;
		write(fd,&(itbl[j].s),4);
	}
	close(fd);

	/* geophones */
	fprintf(stderr,"geophones - ");
	for(j=0;j<ntr;j++)
		buff[j] = itbl[j].gx;
	uniq(buff,ntr,&ng);
	fprintf(stderr,"(uniqed, ng=%d) ",ng);
	ibubbles(buff,ng);
	fprintf(stderr,"(sorted)\n");
	findd(buff,ng,&d_);
	if(d_<SMALL) err("dg=%f\n",d_);
	if(!getpar("gxg","s",fname)) strcpy(fname,"gxg");
	fd = creat(fname,0644);
	for(j=0;j<ntr;j++) {
		write(fd,&(itbl[j].gx),4);
		itbl[j].g = (itbl[j].gx-buff[0])/d_;
		write(fd,&(itbl[j].g),4);
	}
	close(fd);

	free( (char*) buff );

/* 	if(!getpar("tbl","s",fname)) strcpy(fname,"tbl"); */
/* 	tblfd = creat(fname,0644); */
/* 	write(tblfd,itbl,ntr*sizeof(yhsg)); */

	for(i=0,nf=0;i<ng;i++) {
		for(j=0,nf_=0;j<ntr;j++) if(itbl[j].g==i) nf_++;
		nf = MAX(nf,nf_);
	}

	for(i=0;i<ns;i++) {
		for(j=0,nf_=0;j<ntr;j++) if(itbl[j].s==i) nf_++;
		nf = MAX(nf,nf_);
	}
	fprintf(stderr,"done\n");
}

ibubbles(f,nt)
int *f; int nt;
{
	int i,swaps;
	while(1) {
		for(i=1;i<nt;i++) {		/* the bubble floats */
			if(f[i-1]>f[i]) {
				iswap(f+i,f+i-1);
				}
			}
		for(i=nt-2,swaps=0;i>0;i--) {	/* the weight drawns */
			if(f[i-1]>f[i]) {
				iswap(f+i,f+i-1);
				swaps++;
				}
			}
		if(swaps == 0) return;
		}
}

iswap(p,q)
int *p,*q;
{
	register int temp;
	temp = *p;
	*p = *q;
	*q = temp;
}

uniq(b,n,nu)
int n,*nu,*b;
{
	int i,ii,j,u;

	for(j=1,i=1;i<n;i++) {
		for(u=1,ii=i-1;ii>=0;ii--) if (b[ii]==b[i]) {
			u = 0;
			break;
		}
		if(u) {
			b[j] = b[i];
			j++;
		}
	}
	*nu = j;
}

findd(b,n,d)
int *b,n,*d;
{
	int dd,j;
	*d = b[1] - b[0];
	for(j=2;j<n;j++) {
		dd = b[j]-b[j-1];
		*d = MIN(*d,dd);
	}
}
