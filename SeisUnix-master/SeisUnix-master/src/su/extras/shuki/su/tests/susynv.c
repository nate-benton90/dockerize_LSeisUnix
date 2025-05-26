/* sysyn.c.shots */
#include <stdio.h>
#include <math.h>
#include "../include/su.h"
#define pi 3.14159265
/* #define MIN(a,b) a<b?a:b */
/* #define MAX(a,b) a>b?a:b */
int xargc; char **xargv;
char *SccsId;
bool verbose;
int alen;
/*	char *logname();	*/
char *sdoc="\n";
main(ac,av)

{
	int	nt,nw,itr,ns,nf,is,jf,nmoflag,j,outfd;
	int	lop,mid,shift;
	float	freq[4],*trout,*trin,ampl;
	float	dt,sx,df,f,t,v,ov,ovv,ds,sina,cosa,alpha,*w,t0;
	float 	rad,wletf,wletl;	/* min/max-wavelet parameters */ 
	int	PHASE;
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;

	outfd = output();

	bzero((char*)&tr,sizeof(tr));

	verbose = true;


	nt = 100;		igetpar("nt",&nt);
	nf=2;			igetpar("nf",&nf);
	ns=32;			igetpar("ns",&ns);
	dt = 0.016;		fgetpar("dt",&dt);
	df=800.0;		fgetpar("df",&df);
	ds=25.0;		fgetpar("ds",&ds);
	nmoflag = 0;		igetpar("nmoflag",&nmoflag);
	v = 1500.0;		fgetpar("v",&v);
	alpha = 60;		fgetpar("alpha",&alpha);
	ampl = 1.0;		fgetpar("ampl",&ampl); /* wavelet amplitude*/

	nw = 64;	igetpar("nw",&nw); /* number of samples in wavelet */
	freq[0] = 0.0;		fgetpar("fl0",&freq[0]);
	freq[1] = 0.0;		fgetpar("fl",&freq[1]);
	freq[2] = 1000.0;		fgetpar("fh",&freq[2]);
	freq[3] = 1000.0;		fgetpar("fh0",&freq[3]);

				PHASE = fgetpar("rad",&rad);
	if(PHASE){	/* input min/max-phase wavelet parameters */
		wletf = 30.0;	fgetpar("wletf",&wletf); /* hertz */
		wletl = 0.1;	fgetpar("wletl",&wletl); /* sec */
		nw = wletl/dt + 1.5;
	}

	w = (float*) malloc(nw*sizeof(float));

		/*  WAVELET FORMING                  */
	if(PHASE){
			/* min/max-phase wavelet */
		mmphase(rad,wletf,dt,wletl,w);
		shift = 0; lop = nw;
	}
	else{
			/* zerophase wavelet */
		operhan_(freq,&dt,&nw,w,&lop,&mid);
		shift = mid - 1;
	}

	for (j = 0; j<nw; j++) {
		*(w+j) *= ampl;
		}
	alpha *= pi/180.0;
	sina = sin(alpha);
	cosa = cos(alpha);

	ov = 1.0/v;
	ovv = ov*ov;

	tr.ns = nt;			/* <-- */
	tr.dt = dt*1000000.0 + 0.5;	/* <-- */
	trin = (float*)malloc(nt*sizeof(float));
	trout = (float*)malloc(nt*sizeof(float)); 

	/* ASCII HEADER */
	hispr(outfd,"LINE NAME: S%d\n",getpid());
	hispr(outfd,"AREA:      Holon\n");
/*	hispr(outfd,"CLIENT:    %s\n",logname());		*/
	j = time(0);
	hispr(outfd,"DATE:      %s",ctime(&j));

	/* BINARY HEADER */
	bfill(0xff,&bh,sizeof(bh));
	bh.ns = nt;
	bh.dt = dt*1000000.0 + 0.5;
	bh.esize = sizeof(float);
	sprintf(bh.name,"S%d",getpid());
	sprintf(bh.area,"Holon");
/*	sprintf(bh.client,logname());		*/
	putbh(outfd,&bh);
			
	/* TRACES */
	itr = 0;

	for(is=0,sx=ds;is<ns;is++) {

		sx += ds;
		tr.sx = sx;

		for(jf=0;jf<nf;jf++) {

			itr++;

			f = jf*df;

			tr.data = trin;
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
			/*putspk(tr.data,t/dt,nt);*/

/*			trin[(int)(t/dt+0.5)] = 1.0;*/

			/* FLAT REFLECTORS (ONE SECOND INTERVAL) */
			/*for(j=1;(float)j<nt*dt;j++) {	*/
				j=1;
			/*	t0 = dt*floor((double)j/dt);*/
				t0 = 0.25;
				t = sqrt(t0*t0+f*f*ovv);
				/*putspk(tr.data,t/dt,nt);*/

				trin[(int)(t/dt+0.5)] = 1.0;
			/*}	*/

/*			dotpow(-2.0,bh.ns,tr.data);*/

/* 			vprint(tr.data,bh.ns); */

/* 			bramp((char*)tr.data,bh.ns*sizeof(float)); */

			tr.tracl = itr;

			/* COVOLVE THE TRACE WITH A WAVELET */

   	   		convv_(&lop,&shift,w,&nt,tr.data,trout); 

			tr.data = trout;
			puttr(outfd,&tr);

		}
	}
	exit(0);
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
