/* stackopt: residual statics estimation by stack optimization

	Input files
	===========
 *	standard input:file containing cmp gathers after NMO, with segy headers.

	Output files
	============
 *	sstat=sstat	output for final shots statics estimation.
 *	gstat=gstat	output for final geophones statics estimation.
 *	stacks=stacks	dump the stack after estimating shots and after geophones
 *	pows=pows	file containing the powers of the stacks.
 *	stats=stats	dump the statics estimation every time.
 *	xcf=no		dump the cross-correlation (no dump if not specified)

	Dimension parameters
	====================
 *	t1=0		beginning of the window
 *	nt=ntall-60	width of the window
 *	dtmax=15	max shift allowed every iteration.

	Iteration loop parameters
	=========================
 *	niter=4		no of iterations.
 *	refresh=4	reread the data every refresh iterations.

	Options
	=======
 *	taper=yes	taper the time window.
 *	rsmooth=no	smooth geophone statics when dg=1 ds=2.
 *	rmedian=no	running median filtering (every iteration)
				(for a second run if the first run had glitches)
 *	lmed=3		length of the running median
 *	jrmed=		running median in these iterations (up to 10)
				(e.g jrmed=5,10,15)
 *	verbose=no	outputs trace indexes to check the index table
 *	_unfrac=0	1 for nearest neighbor interpolation in refreshing data

	Sinc interpolation parameters
	=============================
 *	lsinc=10	length of the truncated interpolator

	Other parameters
	================
 *	aptime=30

*/
#define APLAST 65535
#define pi 3.14159265
#define MAXDS 20
#define NRM 10
#define SMALL 0.000001
#define BIG 1e10
#define pi 3.14159265
#include <stdio.h>
#include <math.h>
#include <sys/file.h>
#include <segy.h>
#include "stackopt.h"
char *sdoc = "stackopt <diskfile >stdout par=\n";
struct segy tr;
yhsg *itbl;
int xargc,
	nt,nh,ny,ns,ng,nf,ntr,ntsh,ntshp1,dtmax,maxdt,nc,ntnf2,nhm1,
	nt4, ntp,ntpp1,ntpm1, ntall,t1,t2,
	approf,apstack,apc,apcp1,apmx,apdd,aplast,
	gstatfd,sstatfd, stacksfd,statsfd,powsfd, xcfd, xcdump=0,
	n3, tblfd,
	jrmed[NRM],krm;
float *stack,*datap,*data,fid[4],*dd, *xc,
	*sstpdt,*gstpdt, *sst,*gst,
	*tp, fnc;

/**********************************/
/* #define NDATA 3600000 */
/* float data[NDATA]; */
/**********************************/

char **xargv,verbose[10];
main(ac,av)
int ac; char **av;
{
	int is,ig,j, refresh, pad,lsinc,iter,niter,direc,lmed,lh,_unfrac,fix;
	float *buf, *smoo, *p,*q,*ppad,*qpad, *wmed,shift;
	char fname[100],taper[10],rsmooth[10],rmedian[10],rlin[10],dc[10];
	int seed,amp,biasr,fd,i0s,ies,i0g,ieg,lambda;
	FILE *fopen(),*Hm;

	xargc = ac; xargv = av;
	doc("/usr/src/segy/programs/dstackopt/stackopt.c");

	/* creat output files */
	if(!getpar("sstat","s",fname)) strcpy(fname,"sstat");
	sstatfd = creat(fname,0644);
	if(sstatfd == -1) err("can't open sstat=%s\n",fname);
	if(!getpar("gstat","s",fname)) strcpy(fname,"gstat");
	gstatfd = creat(fname,0644);
	if(gstatfd == -1) err("can't open gstat=%s\n",fname);

	/* formatted output */
	if(!getpar("tbl","s",fname)) strcpy(fname,"tbl");
	tblfd = creat(fname,0644);
	if(tblfd == -1) err("can't create tbl=%s\n",fname);

	/* creat file for stacks (for monitoring) */
	if(!getpar("stacks","s",fname)) strcpy(fname,"stacks");
	stacksfd = creat(fname,0644);
	if(stacksfd == -1) err("can't creat stacks=%s\n",fname);
	if(!getpar("Hstacks","s",fname)) strcpy(fname,"Hstacks");
	Hm = fopen(fname,"w");
	if(Hm == NULL) err("can't creat Hstacks=%s\n",fname);

	/* creat file for statics (for restart) */
	if(!getpar("stats","s",fname)) strcpy(fname,"stats");
	statsfd = creat(fname,0644);
	if(statsfd == -1) err("can't creat stats=%s\n",fname);

	/* creat file for stack power (for monitoring) */
	if(!getpar("pows","s",fname)) strcpy(fname,"pows");
	powsfd = creat(fname,0644);
	if(powsfd == -1) err("can't creat pows=%s\n",fname);

	/* cross correlation dump if desired */
	if(getpar("xcf","s",fname)) {
		xcfd = creat(fname,0644);
		xcdump = 1;
	}
	/* get parameters */
	if(getpar("_tbl","s",fname)) {
		fd = open(fname,0);
		if (fd==-1) err("can't open itbl=%s\n",fname);
		if(!getpar("ntr","d",&ntr)) err("nead ntr=\n");
		if(!getpar("ns","d",&ns)) err("nead ns=\n");
		if(!getpar("ng","d",&ng)) err("nead ng=\n");
		if(!getpar("ny","d",&ny)) err("nead ny=\n");
		if(!getpar("ntall","d",&ntall)) err("nead ntall=\n");
		if(!getpar("nf","d",&nf)) err("nead nf=\n");
		itbl = (yhsg*) alloc(ntr*sizeof(yhsg));
		if(read(fd,itbl,ntr*sizeof(yhsg))!=ntr*sizeof(yhsg))
			err("error in reading from _tbl=%s\n",fname);
		close(fd);
		fprintf(stderr,
		    "read itbl:ntr=%d ntall=%d ny=%d nh=%d ns=%d ng=%d nf=%d\n",
			ntr,ntall,ny,nh,ns,ng,nf);
	}
	else {
		getitbl();
		fprintf(stderr,
		    "got itbl: ntr=%d ntall=%d ny=%d nh=%d ns=%d ng=%d nf=%d\n",
			ntr,ntall,ny,nh,ns,ng,nf);
	}
	write(tblfd,itbl,ntr*sizeof(yhsg));

	if(!getpar("nt","d",&nt)) nt = ntall-60;
	sprintf(taper,"yes");	getpar("taper","s",taper);
	sprintf(rsmooth,"no");	getpar("rsmooth","s",rsmooth);
	sprintf(rmedian,"no");	getpar("rmedian","s",rmedian);
	sprintf(rlin,"no");	getpar("rlin","s",rlin);
	sprintf(dc,"yes");	getpar("dc","s",dc);
	for(krm=0;krm<NRM;krm++)
		jrmed[krm] = -1;
	krm = 0;
	getpar("jrmed","d",jrmed);
	sprintf(verbose,"no");	getpar("verbose","s",verbose);
	dtmax = 15;	getpar("dtmax","d",&dtmax);
	maxdt = 2*dtmax;getpar("maxdt","d",&maxdt);
	niter = 4;	getpar("niter","d",&niter);
	refresh = 4;	getpar("refresh","d",&refresh);
	lsinc = 10;	getpar("lsinc","d",&lsinc);
	t1 = 0;		getpar("t1","d",&t1);
	_unfrac = 0;	getpar("_unfrac","d",&_unfrac);
	if(t1<0) err("t1=%d negative not allowed\n",t1);
	t1 += 60;
	t2 = t1+nt;
	fprintf(stderr,"ntall=%d nt=%d t1-60=%d t2-60=%d maxdt=%d\n",
		ntall,nt,t1-60,t2-60,maxdt);
	if(t2 > ntall) err("t1+nt=%d > ntall=%d\n",t2,ntall);
	nt4 = nt*4; ntnf2 = nt*nf*2;

	/* running median parameters */
	if(!getpar("lmed","d",&lmed)) lmed = 3;
	if(!(lmed%2)) {
		fprintf(stderr,"stackopt: setting lmed=%d to odd %d\n",lmed,++lmed);
	}
	lh = lmed/2;

	nc = 2*dtmax + 1;		/* cross-correlation length */
	ntsh = nt - nc + 1;
	ntshp1 = ntsh + 1;

	gname(stacksfd,fname);
	fprintf(Hm,"in=%s n1=%d n2=%d\n",fname,ntsh,ny); fflush(Hm);

	fprintf(stderr,"lmed=%d\n",lmed);
	fprintf(stderr,"nt=%d\tntsh=%d\tnc=%d\tntr=%d\n",nt,ntsh,nc,ntr);
	if(ntsh < nc) err("dtmax too big: ntsh=%d\tdtmax=%d\n",ntsh,dtmax);
	fnc = nc;
	ntp = 2*dtmax; ntpp1 = ntp + 1; ntpm1 = ntp - 1; nhm1 = nh - 1;
	/* allocate space */
	data  = (float*) alloc(nt*ntr*4); 		/* ny cmp gathers   */
	datap = data + dtmax;
	stack = (float*) alloc((ntsh*ny+1)*4);		/* ny short traces  */
				/* +1, because of the power apget */
	sst   = (float*) calloc(ns,4);			/* shot statics     */
	gst   = (float*) calloc(ng,4);			/* geophone statics */
	sstpdt= (float*) alloc(ns*4);			/* shot statics     */
	gstpdt= (float*) alloc(ng*4);			/* geophone statics */
	smoo  = (float*) alloc(ng*4);			/* buffer           */
	xc    = (float*) alloc(nc*4);
	tp    = (float*) alloc(ntp*4);
	wmed  = (float*) alloc(lmed*4);
	dd    = fid + 1;

	/* allocate space for the interpolation */
	pad = nt + 2*(lsinc+nt) + 2;
	ppad = (float*) alloc(pad*4); p = ppad + lsinc + nt + 1;
	qpad = (float*) alloc(pad*4); q = qpad + lsinc + nt + 1;
	buf  = (float*) alloc(2*lsinc*4);

	settp();

	/* initialize statics */
	fname[0] = 'n';	getpar("restart","s",fname);
	if( fname[0] == 'y') {
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
		}
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
	for(is=0;is<ns;is++)
		sstpdt[is] = sst[is] + dtmax;
	for(ig=0;ig<ng;ig++)
		gstpdt[ig] = gst[ig] + dtmax;

	write(statsfd,gst,ng*4);
	write(statsfd,sst,ns*4);

	/* AP memory */
	apstack = 0;
	approf  = apstack + nf*nt;
	apc     = approf  + nf*nt;
	apcp1   = apc + 1;
	apmx    = apc + nc;
	apdd    = apmx + 2;
	aplast  = apdd + 3; fprintf(stderr,"aplast=%d\n",aplast);
	if(aplast > APLAST)
		err("too big for the AP, reduce nt\n");

	direc=1;	fix = getpar("direc","d",&direc);
	getpar("fix","d",&fix);
	for(iter=0;iter<niter;iter++)
	{
		fprintf(stderr,"\niteration %d (out of %d (-1))\n",iter,niter);
		if( !(iter%refresh) || (iter==niter-1) )
		{
			if(_unfrac) {
				unfrac(gst,ng);
				unfrac(sst,ns);
			}
			dataread(p,q,buf,lsinc);
			if(taper[0] == 'y') tap();
			apclr_();
			stackit();
		}

		gest(direc);			/* estimate receivers statics */
		if( (rmedian[0] == 'y') || (iter == jrmed[krm]) ) {
			write(statsfd,gstpdt,ng*4);
			rmed(gstpdt,ng,lmed,lh,wmed,smoo);
			write(statsfd,gstpdt,ng*4);
			}
		if (rsmooth[0] == 'y')		/* filter receivers statics */
			nyqout(gstpdt,ng,smoo);	
		if(rlin[0]=='y') rlinout(gstpdt,ns);
		if(  dc[0]=='y') dcout(gstpdt,ns);
		for(ig=0;ig<ng;ig++)	/* check if gst is within range */
			if( fabs(gst[ig]+gstpdt[ig]-dtmax) > maxdt)
				gstpdt[ig] = dtmax;
		for(ig=0;ig<ng;ig++)		/* save the estimated statics */
			gst[ig] += gstpdt[ig] - dtmax;
		write(statsfd,gst,ng*4);
		for(j=0;j<ntr;j++) itbl[j].gst = gst[itbl[j].g];
		if(iter == niter-1) write(gstatfd,gst,ng*4);
		gcorrect();			/* correct the data */
		stackit();			/* restack */

		/* shots */
		if(fix) sest(direc);		/* estimate shots statics */
		else sest(-direc);		/* estimate shots statics */
		if( (rmedian[0] == 'y') || (iter == jrmed[krm]) ) {
			krm++;
			write(statsfd,sstpdt,ns*4);
			rmed(sstpdt,ns,lmed,lh,wmed,smoo);
			write(statsfd,sstpdt,ns*4);
			}
		if(rlin[0]=='y') rlinout(sstpdt,ns);
		if(  dc[0]=='y') dcout(sstpdt,ns);
		for(is=0;is<ns;is++)
			if( fabs(sst[is]+sstpdt[is]-dtmax) > maxdt)
				sstpdt[is] = dtmax;
		for(is=0;is<ns;is++)		/* save the estimated statics */
			sst[is] += sstpdt[is] - dtmax;
		write(statsfd,sst,ns*4);
		for(j=0;j<ntr;j++) itbl[j].sst = sst[itbl[j].s];
		if(iter == niter-1) write(sstatfd,sst,ns*4);
		scorrect();			/* correct the data */
		stackit();			/* restack */

		/* receivers */
		fprintf(Hm,"n3=%d\n",n3); fflush(Hm);
		lseek(tblfd,0,0); write(tblfd,itbl,ntr*sizeof(yhsg));
		if(!fix) direc *= -1;
	}
	fprintf(stderr,"statics estimated\n");
	lseek(0,0,0);
	for(j=0;j<ntr;j++) {
		shift = itbl[j].sst + itbl[j].gst;
		gettr_(&tr);
/* 		copy((char*)(tr.data),(char*)p,tr.ns*4); */
/* 		tsinc(p,q,tr.ns,shift,lsinc,buf); */
/* 		copy((char*)q,(char*)(tr.data),tr.ns*4); */
		tr.cdpt = shift*tr.dt;
		puttr_(&tr);
	}
	fprintf(stderr,"statics applied\n");
	exit(0);
}

dataread(p,q,buf,lsinc)			/* read shifted data */
float *p,*q,*buf; int lsinc;
{
	int i,j,t1pj,t2pj;
	long sk;
	float shift;

	fprintf(stderr,"dataread: ");
	for(i=0;i<ntr;i++) {
		shift = sst[itbl[i].s] + gst[itbl[i].g];
		j = shift;
		if(shift<0.) j--;
		t1pj = t1 + j;
		t2pj = t2 + j;
		if( (t1pj < 0) || (t2pj > ntall)) /* off the trace */
		{
/* 			fprintf(stderr,"datared: off trace reading (i=%d)\n",i); */
			sk = 4*(i*ntall+t1);
			lseek(0,sk,0);
			if( read(0,p,nt4) != nt4)
				err("read error in trace i=%d\n",i);
		}
		else			/* within the trace */
		{
			shift -= j;	/* only the fraction done by tsinc */
			sk = 4*(i*ntall+t1pj);
			lseek(0,sk,0);
			if( read(0,p,nt4) != nt4)
				err("read error in trace i=%d\n",i);
		}
		tsinc(p,q,nt,shift,lsinc,buf);
		copy((char*)q,(char*)(data+i*nt),nt*4);		/* <----- */
		}
	fprintf(stderr,"done\n");
}
		
gcorrect()
{
	int ig,j,n,id,new0,len,old0,arg1,arg2;
	float tmax;

	fprintf(stderr,"gcorrect: ");
	arg2=2;
	for(ig=0;ig<ng;ig++)
	{
		/* put the relevant stuff in the ap */
		for(j=0,n=0;j<ntr;j++) if(itbl[j].g == ig) {
			arg1=approf+(n++)*nt;
			apput_(data+j*nt,&arg1,&nt,&arg2);
			}

		tmax = gstpdt[ig] -dtmax;
		if (tmax >= 0.) {
			id = (int)tmax;
			new0 = apstack;
			len = nt - id - 1;
			old0 = approf + id;
			}
		else {
			id = (int)tmax - 1;
			new0 = apstack - id;
			len = nt + id;
			old0 = approf;
			}
		dd[0] = tmax - id;
		dd[1] = 1. - dd[0];
		apput_(dd,&apc,&arg2,&arg2);
		apwd_();
		arg1=nt*n; apcrct_(&new0,&old0,&apc,&apcp1,&nt,&len,&arg1,&n);
		apwr_();
		for(j=0,n=0;j<ntr;j++) if(itbl[j].g == ig) {
			arg1=apstack+(n++)*nt;
			apget_(data+j*nt,&arg1,&nt,&arg2);
			}
		apwd_();
	}
	fprintf(stderr,"done\n");
}

scorrect() /* correct the data */
{
	int is,j,n,id,new0,len,old0,arg1,arg2;
	float tmax;

	fprintf(stderr,"scorrect: ");
	arg2=2;
	for(is=0;is<ns;is++)
	{
		/* put the relevant stuff in the ap */
		for(j=0,n=0;j<ntr;j++) if(itbl[j].s == is) {
			arg1=approf+(n++)*nt;
			apput_(data+j*nt,&arg1,&nt,&arg2);
			}

		tmax = sstpdt[is] - dtmax;
		if (tmax >= 0.) {
			id = (int)tmax;
			new0 = apstack;
			len = nt - id - 1;
			old0 = approf + id;
			}
		else {
			id = (int)tmax - 1;
			new0 = apstack - id;
			len = nt + id;
			old0 = approf;
			}
		dd[0] = tmax - id;
		dd[1] = 1. - dd[0];
		apput_(dd,&apc,&arg2,&arg2);
		apwd_();
		arg1=nt*n;
		apcrct_(&new0,&old0,&apc,&apcp1,&nt,&len,&arg1,&n);
		apwr_();
		for(j=0,n=0;j<ntr;j++) if(itbl[j].s == is) {
			arg1=apstack+(n++)*nt;
			apget_(data+j*nt,&arg1,&nt,&arg2);
			}
		apwd_();
	}
	fprintf(stderr,"done\n");
}

gest(direc)
int direc;
{
	int ig,j,n,id,arg1,arg2,arg3,arg4;
	float tmax,fmaxv();

	fprintf(stderr,"gest: direc=%d\t",direc);
	if(direc==1) ig=0;
	else if(direc == -1) ig=ng-1;
	else err("direc=%d\n",direc);
	arg1=1; arg2=2;
	for(;(ig<ng) && (ig>=0);ig += direc)
	{
		/* clear the ap (for the gaps between traces in the stack) */
		vclr_(&apstack,&arg1,&ntnf2);
		apwr_();

		/* put the relevant stuff in the ap */
		for(j=0,n=0;j<ntr;j++) if(itbl[j].g == ig)
		{
			arg3=apstack+n*nt;
			apput_(stack+itbl[j].y*ntsh,&arg3,&ntsh,&arg2);
			arg3=approf+n*nt;
			apput_(data+j*nt,&arg3,&nt,&arg2);
			n++;
		}

		if(n>1) {
		/* x-c the profile and stack */
		apwd_();
		arg3=approf+dtmax; arg4=nt*n;
		apxcmx_(&apstack,&approf,&apc,&apmx,&apdd,&arg3,&nt,&ntsh,&arg4,&nc,&n);

		/* get the x-c */
		apwr_(); apget_(xc,&apc,&nc,&arg2); apwd_();

		/* dump xc */
		if(xcdump) write(xcfd,xc,nc*4);

		/* find the maximum */
		tmax = fmaxv(xc,nc);

		/* enter shot receivers estimate */
		gstpdt[ig] = dtmax + tmax;

		/* correct the stack */
		id = gstpdt[ig];
		dd[0] = gstpdt[ig] - id;		/* gstpdt[]-id */
		dd[1] = 1. - dd[0];		/* 1-dd */
		apput_(dd,&apc,&arg2,&arg2);
		apwd_();
		arg3=approf+id;
		aprstk_(&apstack,&apc,&arg3,&nt,&ntsh,&n);
		apwr_();
		for(j=0,n=0;j<ntr;j++) if(itbl[j].g == ig) {
			arg3=apstack+(n++)*nt;
			apget_(stack+itbl[j].y*ntsh,&arg3,&ntsh,&arg2);
			}
		apwd_();
		}
	}
	fprintf(stderr,"done\n");
	apwr_(); apwd_();
}

sest(direc)
int direc;
{
	int is,j,n,id,arg1,arg2,arg3,arg4;
	float tmax,fmaxv();

	fprintf(stderr,"sest: direc=%d\t",direc);
	if(direc==1) is=0;
	else if(direc == -1) is=ns-1;
	else err("direc=%d\n",direc);
	arg1=1; arg2=2;
	for(;(is<ns) && (is>=0);is += direc)
	{
		/* clear the ap (for the gaps between traces in the stack) */
		vclr_(&apstack,&arg1,&ntnf2);
		apwr_();

		/* put the relevant traces in the ap */
		for(j=0,n=0;j<ntr;j++) if(itbl[j].s == is)
		{
			arg3=apstack+n*nt;
			apput_(stack+itbl[j].y*ntsh,&arg3,&ntsh,&arg2);
			arg3=approf+n*nt;
			apput_(data+j*nt,&arg3,&nt,&arg2);
			n++;
		}

		if(n>1) {
		/* x-c the profile and stack */
		apwd_(); arg3=approf+dtmax; arg4=nt*n;
		apxcmx_(&apstack,&approf,&apc,&apmx,&apdd,&arg3,&nt,&ntsh,&arg4,&nc,&n);

		/* get the x-c */
		apwr_(); apget_(xc,&apc,&nc,&arg2); apwd_();

		/* dump xc */
		if(xcdump) write(xcfd,xc,nc*4);

		/* find the maximum */
		tmax = fmaxv(xc,nc);

		/* enter shot static estimate */
		sstpdt[is] = dtmax + tmax;

		/* correct the stack */
		id = sstpdt[is];
		dd[0] = sstpdt[is] - id;		/* sstpdt[]-id */
		dd[1] = 1. - dd[0];		/* 1-dd */
		apput_(dd,&apc,&arg2,&arg2);
		apwd_(); arg3=approf+id;
		aprstk_(&apstack,&apc,&arg3,&nt,&ntsh,&n);	/* restack */
		apwr_();
		for(j=0,n=0;j<ntr;j++) if(itbl[j].s == is) {	/* put it back */
			arg3=apstack+(n++)*nt;
			apget_(stack+itbl[j].y*ntsh,&arg3,&ntsh,&arg2);
			}
		apwd_();
		}
	}
	fprintf(stderr,"done\n");
	apwr_(); apwd_();
}

stackit()
{
	int iy,i,j,arg0,arg2,arg3;
	float pow;

	fprintf(stderr,"stackit: ");
	arg0=0; arg2=2;
	/* loop over midpoints */
	for(iy=0,pow=0.;iy<ny;iy++)
	{
		/* the rest of the cmp gather (nt>ntsh!) */
		for(j=0,i=0;j<ntr;j++) if(itbl[j].y==iy) {
			arg3=i*ntsh;
			apput_(datap+j*nt,&arg3,&ntsh,&arg2);
			i++;
		}
		i--;

		apwd_();
		if(i) apstk_(&i,&ntsh);
		apwr_();
		apget_(stack+iy*ntsh,&arg0,&ntshp1,&arg2);
		apwd_();

		pow += stack[iy*ntsh+ntsh];
	}
	write(stacksfd,stack,ntsh*ny*4); n3++;
	write(powsfd,&pow,4);
	fprintf(stderr,"pow=%e ",pow);
	fprintf(stderr,"done\n");
}
