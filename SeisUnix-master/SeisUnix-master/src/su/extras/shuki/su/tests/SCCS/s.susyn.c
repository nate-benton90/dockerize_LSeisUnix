h51411
s 00002/00001/00195
d D 1.6 88/11/15 14:19:32 shuki 6 5
c 
e
s 00001/00001/00195
d D 1.5 88/07/04 06:53:02 shuki 5 4
c 
e
s 00001/00001/00195
d D 1.4 88/07/04 06:28:53 shuki 4 3
c calloc
e
s 00011/00019/00185
d D 1.3 88/06/27 13:46:49 shuki 3 2
c decon
e
s 00002/00001/00202
d D 1.2 88/05/29 07:28:31 shuki 2 1
c SccsId
e
s 00203/00000/00000
d D 1.1 88/05/05 07:25:27 shuki 1 0
c date and time created 88/05/05 07:25:27 by shuki
e
u
U
f e 0
t
T
I 1
/* sysyn.c.shots */
#include <stdio.h>
#include <math.h>
#include "../include/su.h"
#define pi 3.14159265
/* #define MIN(a,b) a<b?a:b */
/* #define MAX(a,b) a>b?a:b */
#ifdef HP
#define _CONVOL_ convol
#else
#define _CONVOL_ convol_
#endif
int xargc; char **xargv;
bool verbose;
int alen;
/* char *logname(); */
D 2
char *sdoc="\n";
E 2
I 2
char *sdoc="SDOC\n";
char *SccsId="%W% %G%\n";
E 2
main(ac,av)
char **av;
{
	int	nt,nw,itr,ns,nf,is,jf,nmoflag,j,outfd;
D 3
	int	wave;
	float	hcut;
	float	dt,sx,df,f,t,v,ov,ovv,ds,sina,cosa,alpha,*w,t0;
E 3
I 3
	float	dt,sx,df,f,t,v,ov,ovv,ds,sina,cosa,alpha,*w,t0,ro;
E 3
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;

	outfd = output();

	bzero((char*)&tr,sizeof(tr));

	verbose = true;

	/*
	nf = 12;
	df = 100.0;
	ns = 8;
	ds = 100.;

	nf = 2;
	df = 800.0;
	ns = 32;
	ds = 25.;

	nf = 14;
	df = 100.0;
	ns = 6;
	ds = 125.;
	*/

	nt = 100;		igetpar("nt",&nt);
	nf=2;			igetpar("nf",&nf);
	ns=32;			igetpar("ns",&ns);
	dt = 0.016;		fgetpar("dt",&dt);
	df=800.0;		fgetpar("df",&df);
	ds=25.0;		fgetpar("ds",&ds);
D 3
	nmoflag = 0;		igetpar("nmoflag",&nmoflag);
E 3
I 3
	nmoflag = 0;	igetpar("nmoflag",&nmoflag);
E 3
	v = 1500.0;		fgetpar("v",&v);
	alpha = 60;		fgetpar("alpha",&alpha);
D 3
	wave = 3;		fgetpar("wave",&wave); /* 1 - Gaussian;
							  2 - Rickerian;
							  3 - minphase */
	hcut = 60.0;		fgetpar("hcut",&hcut); /* highcut of
							wavelet(Hz) */
E 3
I 3
D 5
	ro = 0.8;		fgetpar("ro",&ro);
E 5
I 5
	ro = 0.5;		fgetpar("ro",&ro);
E 5
	nw = 16;		igetpar("nw",&nw);
E 3

D 3

	nw = 8;		igetpar("nw",&nw);

E 3
	w = (float*) malloc(nw*sizeof(float));

D 3
	wvlet(w,nw);
E 3
I 3
	wvlet(w,nw,ro);
E 3

	alpha *= pi/180.0;
	sina = sin(alpha);
	cosa = cos(alpha);

	ov = 1.0/v;
	ovv = ov*ov;

D 3
	tr.ns = nt;			/* <-- */
	tr.dt = dt*1000000.0 + 0.5;	/* <-- */
E 3
I 3
/* 	tr.ns = nt; */
/* 	tr.dt = dt*1000000.0 + 0.5; */
E 3
D 4
	tr.data = (float*)malloc(nt*sizeof(float));
E 4
I 4
	tr.data = (float*)calloc(nt,sizeof(float));
E 4

	/* ASCII HEADER */
	hispr(outfd,"LINE NAME: S%d\n",getpid());
	hispr(outfd,"AREA:      Holon\n");
/* 	hispr(outfd,"CLIENT:    %s\n",logname()); */
	hispr(outfd,"CLIENT:    %s\n",cuserid());
	j = time(0);
	hispr(outfd,"DATE:      %s",ctime(&j));

	/* BINARY HEADER */
D 6
	bfill(0xff,&bh,sizeof(bh));
E 6
I 6
/* 	bfill(0xff,&bh,sizeof(bh)); */
	bfill(0,&bh,sizeof(bh));
E 6
	bh.ns = nt;
	bh.dt = dt*1000000.0 + 0.5;
	bh.esize = sizeof(float);
	sprintf(bh.name,"S%d",getpid());
	sprintf(bh.area,"Holon");
/* 	sprintf(bh.client,logname()); */
	sprintf(bh.client,cuserid());
	putbh(outfd,&bh);
			
	/* TRACES */
	itr = 0;

	for(is=0,sx=ds;is<ns;is++) {

		sx += ds;
		tr.sx = sx;

		for(jf=0;jf<nf;jf++) {

			itr++;

			f = jf*df;

			tr.offset = f;
			tr.gx = sx + f;
			tr.cdp = 0.5*(tr.sx+tr.gx);
			tr.tracl = itr;

			bzero((char*)tr.data,nt*sizeof(float));

			/* DIPPING REFLECTOR */
/* 			t = ov*sqrt(4.0*sx*sx*sina+4.0*sx*f*sina*sina+f+f); */
/* 			t = ov*sqrt(sx*sx+(sx+f)*(sx+f)-2.*sx*f*cos(2.*alpha)); */
			t = 2.*tr.cdp*sina*ov;
			t = sqrt(t*t+(f*cosa*ov)*(f*cosa*ov));
/* 			if(verbose) fprintf(stderr,"sx=%f f=%f t=%f\n",sx,f,t); */
			putspk(tr.data,t/dt,nt);
  
			/* FLAT REFLECTORS (ONE SECOND INTERVAL) */
			for(j=1;(float)j<nt*dt;j++) {
				t0 = dt*floor((double)j/dt);
				t = sqrt(t0*t0+f*f*ovv);
				putspk(tr.data,t/dt,nt);
			}

			dotpow(-2.0,bh.ns,tr.data);

/* 			vprint(tr.data,bh.ns); */

/* 			bramp((char*)tr.data,bh.ns*sizeof(float)); */

			tr.tracl = itr;

			/* COVOLVE THE TRACE WITH A WAVELET */
			_CONVOL_(tr.data,w,&nt,&nw);
D 3
/* 			conv(tr.data,&nt,&dt,&wave,&hcut); */
E 3

D 3

E 3
			puttr(outfd,&tr);

		}
	}
	exit(0);
}


D 3
wvlet(w,n)
E 3
I 3
wvlet(w,n,ro)
E 3
float *w;
int n;
I 3
float ro;
E 3
{
	int i;
I 3

E 3
	w[0] = 1.0;
	for(i=1;i<n;i++)
D 3
		w[i] = -0.5*w[i-1];
E 3
I 3
		w[i] = -ro*w[i-1];
E 3
}
		
/* JUNK YARD

bramp(r,n)
char *r;
int n;
{
	int i;
	for(i=0;i<n;i++)
		r[i] = i;
}

vprint(v,n)
int n;
float *v;
{
	int i;

	for(i=0;i<n;i++)
		fprintf(stderr,"[%d]\t%f\n",i,v[i]);
}

float tnmo(t,xx)
float t,xx;
{
	t = t*t-xx;

	if(t>0.0) t = sqrt(t);
	else t = -1.0;

	return(t);
}
*/
E 1
