h60325
s 00000/00000/00282
d D 1.4 88/11/15 14:02:50 shuki 4 3
c 
e
s 00000/00000/00282
d D 1.3 88/05/25 14:53:55 shemer 3 2
c with SccsId[]
e
s 00001/00000/00281
d D 1.2 88/05/25 12:54:11 shuki 2 1
c 
e
s 00281/00000/00000
d D 1.1 88/04/14 13:52:41 shuki 1 0
c date and time created 88/04/14 13:52:41 by shuki
e
u
U
f e 0
t
T
I 1
/*
 *	sudmo -  Dip move out 
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

char *sdoc;
int xargc;
char **xargv;
bool verbose;
I 2
char *SccsId[]="%W%\t%G%\n";
E 2
/* bool hisout,bhout; */

#define pi 3.14159265

typedef struct {float re,im;} complex;

char *sdoc = "							\n\
	sydmo <stdin >stdout []					\n\
								\n\
	Input is sorted to offset sections, with cdp order	\n\
	monotonous (increasing or decreasing) but with possible	\n\
	gaps. The first common offset section must contain more	\n\
	than one trace. If there is a gap between the first and	\n\
	the second trace, the cdp interval dcdp has to specified\n\
	by the user.						\n\
								\n";
int xargc; char **xargv;
int nalloc,dalloc;
float *data;
main(ac,av)
int ac; char **av;
{
	Sutrace tr;
	Subhed bh;
	int nt,nx,offset,newoffset,cdp0,dcdp,nxpad;
	int infd,outfd;
	int c;
	float dx,h;
	Section coff;

	xargc = ac; xargv = av;
	infd = input();
	outfd = output();

	/* GET OPTIONS */
	verbose = false;
	while( (c=getopt(xargc,xargv,"v"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}

	/* PASS ASCII HEADER */
	apass(infd,outfd);

	/* READ BINARY HEADER */
	getbh(infd,&bh);
	nt = bh.ns;
	tr.data = (float*)malloc(bh.ns*bh.esize);

	/* ADD HISTORY LOG */
	hislog(outfd);
/* 	hisclose(outfd); */

	/* WRITE BINARY HEADER */
	putbh(outfd,&bh);

	/* TRIG TABLE */
	mktrigtbl(1000);

/* 	while(getco(infd,nt,&nx,&offset,&newoffset,&cdp0,&dcdp)) { */

	coff.n1 = nt;

	while(getco(infd,&coff,&newoffset)) {

		nx = coff.n2;
		offset = coff.val.i;
/* 		cdp0 = coff.o2; */
		dcdp = coff.d2;
		data = coff.data;

		if(verbose) fprintf(stderr,"Read offset=%d nt=%d nx=%d\n",
								offset,nt,nx);

		/* ZERO PADDING */
		zpad2(&coff);
		nxpad = coff.n2;

		/* NORMALIZE HALF OFFSET */
		dx = fabs((float)dcdp);
		h = 0.5*offset;
		h /= dx;

		if(verbose) fprintf(stderr,"\tdx=%f Normalized h=%f\n",dx,h);

		/* FFT OVER MIDPOINT */
		if(verbose) fprintf(stderr,"sudmo: fft over midpoint ... ");
		rvfft((complex*)data,nt,nxpad,1,2);
		if(verbose) fprintf(stderr,"fft done\n");

		/* DMO */
		if(verbose) fprintf(stderr,"sudmo: dmo ... ");
		dmo((complex*)data,nt,nxpad,h);
		if(verbose) fprintf(stderr,"dmo done\n");

		/* INVERSE FFT OVER MIDPOINT */
		if(verbose) fprintf(stderr,"sudmo: inverse fft over midpoint ... ");
		rvfft((complex*)data,nt,nxpad,-1,-2);
		if(verbose) fprintf(stderr,"inverse fft done\n");

		/* FFT SCALE */
		vsmul(data,2.0/nxpad,nxpad*nt);

		/* WRITE THE CO SECTION */
/* 		putco(outfd,nt,nxpad,offset,cdp0,dcdp,&tr); */
		putco(outfd,&coff);

		offset = newoffset;
	}

	exit(0);
}

dmo(cdata,nt,nxpad,h)
complex *cdata;
int nt,nxpad;
float h;
{
	int ntpad,iw,nw,it,ik,nk,ntpad8,nt8,tmin,tmax,wmin,wmax;
	float dw,dk,camp,k,hhkk,wt,arg,pha,amp,iop,rop,*w;
	register float rdata,idata;
	complex *cbuf;

/* 	if(h==0.0) {if(verbose) fprintf(stderr,"sudmo: h=%f ... returning\n",h); return;} */

	if(verbose) fprintf(stderr,"sudmo: dmo: h=%f\n",h);

	for(ntpad=1;ntpad<nt;ntpad*=2);

	nw = ntpad/2;
	nk = nxpad/2+1;
	ntpad8 = ntpad*sizeof(complex);
	nt8 = nt*sizeof(complex);

	/* MEMORY ALLOCATION */
	w = (float*) malloc ( ntpad/2*sizeof(float));
	cbuf = (complex*) malloc ( ntpad*sizeof(complex) );

	dw = 1.0/ntpad;
	dk = 1.0/nxpad;

	for(iw=0;iw<nw;iw++)
		w[iw] = dw*iw;

/* 	camp = 1.0/sqrt((float)ntpad); */
	camp = 1.0;

	tmin = 1; tmax = nt-1;
	wmin = 1; wmax = ntpad/2-1;
	for(ik=0;ik<nk;ik++) {			/* K LOOP */

		if(verbose) fprintf(stderr,"ik=%d out of %d\n",ik,nk-1);

		k = dk*ik;
		hhkk = h*h*k*k;

		bzero(cbuf,ntpad8);

		for(iw=wmax;iw>=wmin;iw--) {		/* W LOOP */

			for(it=tmax;it>=tmin;it--) {		/* T LOOP */

/* 				if(verbose) fprintf(stderr,"\tiw=%d it=%d\n",iw,it); */

				wt = dw*iw*it;
				arg = wt*wt + hhkk;


				/* RECURSIVE SQRT (TO SAVE TIME) */
				if(it==nt-1) {
					pha = sqrt(arg);
				} else {
					pha = 0.5*(pha+arg/pha);
					pha = 0.5*(pha+arg/pha);
				}
/* 				if(arg!=0.0) if(verbose) fprintf(stderr,"it=%d Error in sqrt = %e\n",it,pha/sqrt(arg)-1.0); */

				/* DMO OPERATOR */
				sinecos(pha,&iop,&rop);		/* (rop,iop)=cexp(2*pi*pha) */
				amp = camp/**fabs(wt/pha)*/;	/* Jacobian */
				iop *= amp;
				rop *= amp;

				/* PUT THE DATA INTO REGISTERS */
				rdata = cdata[ik*nt+it].re;
				idata = cdata[ik*nt+it].im;

				/* POSITIVE FREQUENCY */
				cbuf[iw].re += rop*rdata - iop*idata;
				cbuf[iw].im += rop*idata + iop*rdata;

				/* NEGATIVE FREQUENCY */
				cbuf[ntpad-iw].re += rop*rdata + iop*idata;
				cbuf[ntpad-iw].im += rop*idata - iop*rdata;

			}				/* END OF T LOOP */

		}				/* END OF W LOOP */

/* 		cprntnonz(cbuf,ntpad,1,"cbuf (before ifft)"); */

		/* INVERSE ELEMENT FFT */
		cefft(cbuf,ntpad,1);

/* 		cprntnonz(cbuf,ntpad,1,"cbuf (after ifft)"); */

		/* FFT SCALE */
/* 		vsmul((float*)cbuf,sqrt(ntpad/2.0),2*ntpad); */

		/* COPY BACK */
		bcopy(cbuf,cdata+ik*nt,nt8);

/* 		cprntnonz(cdata+ik*nt,nt,1,"cdata[ik] (after)"); */

	}				/* END OF K LOOP */

	if(verbose) fprintf(stderr,"sudmo: dmo done\n");

}

/*
cprntnonz(x,n1,n2,name)
char *name;
complex *x;
{
	int i1,i2,zeroflag=1;
	float r,i;

	fprintf(stderr,"\nCOMPLEX ARRAY: %s (%d,%d)\n",name,n1,n2);

	for(i2=0;i2<n2;i2++) {
		for(i1=0;i1<n1;i1++) {
			r = x[i2*n1+i1].re;
			i = x[i2*n1+i1].im;
			if( (r!=0.0) || (i!=0.0) ) {
				fprintf(stderr,"\t%s( %d, %d ) = ( %f, %f )\n",
					name,i1,i2,r,i);
				zeroflag = 0;
			}
		}
	}
	if(zeroflag) fprintf(stderr,"\t *** ALL ZERO ***\n");
}

prntnonz(x,n1,n2,name)
char *name;
float *x;
{
	int i1,i2,zeroflag=1;

	fprintf(stderr,"\nARRAY: %s(%d,%d)\n",name,n1,n2);

	for(i2=0;i2<n2;i2++) {
		for(i1=0;i1<n1;i1++) {
			if(x[i2*n1+i1]!=0.0) {
				fprintf(stderr,"\telement(%d,%d) = %6.3f\n",
					i1,i2,x[i2*n1+i1]);
				zeroflag = 0;
			}
		}
	}
	if(zeroflag) fprintf(stderr,"\t *** ALL ZERO ***\n");
}
*/
E 1
