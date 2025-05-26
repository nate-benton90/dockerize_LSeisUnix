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
char *sdoc="SDOC\n";
char *SccsId="@(#)susyn.c	1.6 11/15/88\n";
main(ac,av)
char **av;
{
	int	nt,nw,itr,ns,nf,is,jf,nmoflag,j,outfd;
	float	dt,sx,df,f,t,v,ov,ovv,ds,sina,cosa,alpha,*w,t0,ro;
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
	nmoflag = 0;	igetpar("nmoflag",&nmoflag);
	v = 1500.0;		fgetpar("v",&v);
	alpha = 60;		fgetpar("alpha",&alpha);
	ro = 0.5;		fgetpar("ro",&ro);
	nw = 16;		igetpar("nw",&nw);

	w = (float*) malloc(nw*sizeof(float));

	wvlet(w,nw,ro);

	alpha *= pi/180.0;
	sina = sin(alpha);
	cosa = cos(alpha);

	ov = 1.0/v;
	ovv = ov*ov;

/* 	tr.ns = nt; */
/* 	tr.dt = dt*1000000.0 + 0.5; */
	tr.data = (float*)calloc(nt,sizeof(float));

	/* ASCII HEADER */
	hispr(outfd,"LINE NAME: S%d\n",getpid());
	hispr(outfd,"AREA:      Holon\n");
/* 	hispr(outfd,"CLIENT:    %s\n",logname()); */
	hispr(outfd,"CLIENT:    %s\n",cuserid());
	j = time(0);
	hispr(outfd,"DATE:      %s",ctime(&j));

	/* BINARY HEADER */
/* 	bfill(0xff,&bh,sizeof(bh)); */
	bfill(0,&bh,sizeof(bh));
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

			puttr(outfd,&tr);

		}
	}
	exit(0);
}


wvlet(w,n,ro)
float *w;
int n;
float ro;
{
	int i;

	w[0] = 1.0;
	for(i=1;i<n;i++)
		w[i] = -ro*w[i-1];
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
