/* sysyn.c.shots */
#include <stdio.h>
#include <math.h>
#include "/src/su/include/su.h"
#define pi 3.14159265
/* #define MIN(a,b) a<b?a:b */
/* #define MAX(a,b) a>b?a:b */
int xargc; char **xargv;
bool verbose;
char *SccsId[]="@(#)susycdf.c     1.3\t5/25/88\n";

int alen;
/*	char *logname();	*/
char *sdoc="\n";
main(ac,av)
char **av;
{
	int	nt,nw,itr,mncdf,nrec,ncdf,irec,nmoflag,j,outfd;
	int	lop,mid,shift;
	float	freq[4],*trout,*trin,ampl;
	float	dt,xcdf,xdis,t,*v,*ov1,*ov,ds,sina,cosa,alpha,*w,t0;
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;

	outfd = output();

	bzero((char*)&tr,sizeof(tr));

	verbose = true;


	nt = 100;		igetpar("nt",&nt);
	nrec= 6;			igetpar("nrec",&nrec);
	mncdf=1;			igetpar("mncdf",&mncdf);
	dt = 0.016;		fgetpar("dt",&dt);
	ds=25.0;		fgetpar("ds",&ds);
	nmoflag = 0;		igetpar("nmoflag",&nmoflag);
	alpha = 60;		fgetpar("alpha",&alpha);
	ampl = 3.0;		fgetpar("ampl",&ampl); /* wavelet amplitude*/

	nw = 511;	igetpar("nw",&nw); /* number of samples in wavelet */
	v = (float*)malloc(mncdf*sizeof(float));
	ov = (float*)malloc(mncdf*sizeof(float));
	ov1 = (float*)malloc(mncdf*sizeof(float));
/*         v[0] = 500.;            fgetpar("v0",&v[0]); */
/*         v[1] = 600.;            fgetpar("v1",&v[1]); */
/*         v[2] = 700.;            fgetpar("v2",&v[2]); */
/*         v[3] = 800.;            fgetpar("v3",&v[3]); */
/*         v[4] = 900.;            fgetpar("v4",&v[4]); */
            fgetpar("v",v);
	freq[0] = 0.0;		fgetpar("fl0",&freq[0]);
	freq[1] = 0.0;		fgetpar("fl",&freq[1]);
	freq[2] = 1000.0;		fgetpar("fh",&freq[2]);
	freq[3] = 1000.0;		fgetpar("fh0",&freq[3]);

	w = (float*) malloc(nw*sizeof(float));

		/*  CONSTRUCTION OF FILTER OPERATOR  */
	operhan_(freq,&dt,&nw,w,&lop,&mid);

	for (j = 0; j<nw; j++) {
		*(w+j) *= ampl;
		}
	alpha *= pi/180.0;
	sina = sin(alpha);
	cosa = cos(alpha);
        for (j=0;j< mncdf; j++){
	ov[j] = 1.0/(v[j]*v[j]);
        ov1[j]= 1.0/((v[j]+v[j]*0.1)*(v[j]+v[j]*0.1));
               }
	bh.ns = nt;			/* <-- */
	bh.dt = dt*1000000.0 + 0.5;	/* <-- */
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

/* 	for(ncdf=0,xcdf=1000;ncdf<mncdf;ncdf++) { */
	for(ncdf=0,xcdf=1;ncdf<mncdf;ncdf++) {

		xcdf += 1.;
		tr.cdp = xcdf;

		for(irec=0;irec<nrec;irec++) {

			itr++;

/* 			xdis = irec*ds; */
			xdis = 50;

			tr.data = trin;
			tr.offset = xdis;
			tr.gx = irec*ds;
			tr.sx = xcdf-irec*ds;
			tr.tracl = irec;

			bzero((char*)tr.data,nt*sizeof(float));

			/* DIPPING REFLECTOR */
/* 			t = ov*sqrt(4.0*sx*sx*sina+4.0*sx*f*sina*sina+f+f); */
/* 			t = ov*sqrt(sx*sx+(sx+f)*(sx+f)-2.*sx*f*cos(2.*alpha)); */
/* 			t = 2.*tr.cdp*sina*ov; */
/* 			t = sqrt(t*t+(f*cosa*ov)*(f*cosa*ov)); */
/* 			if(verbose) fprintf(stderr,"sx=%f f=%f t=%f\n",sx,f,t); */
/* 			putspk(tr.data,t/dt,nt); */
  
			/* FLAT REFLECTORS (ONE SECOND INTERVAL) */
/* 			for(j=1;(float)j<nt*dt;j++) { */
/* 				t0 = dt*floor((double)j/dt); */
/* 				t = sqrt(t0*t0+xdis*xdis*ov1[ncdf]); */
		        if(irec==nrec/2)	{	t = 30*dt;
                             fprintf(stderr,"t=%f mncdf=%d irec=%d ncdf=%d itr=%d\n",t,mncdf,irec,ncdf,itr);
				putspk(tr.data,t/dt,nt);}
/* 			} */
/* 			for(j=1;(float)j<nt*dt;j++) { */
/* 				t0 = dt*floor((double)j/dt); */
/* 				t = sqrt(t0*t0+xdis*xdis*ov1[ncdf]); */
		        if(irec==nrec/2)	{	t = 60*dt;
				putspk(tr.data,t/dt,nt);}
/* 			} */

/*			dotpow(-2.0,bh.ns,tr.data);

/* 			vprint(tr.data,bh.ns); */

/* 			bramp((char*)tr.data,bh.ns*sizeof(float)); */

			tr.tracl = itr;

			/* COVOLVE THE TRACE WITH A WAVELET */

			shift = mid - 1;
   	   		convv_(&lop,&shift,w,&nt,tr.data,trout); 

			tr.data = trout;
			puttr(outfd,&tr);

		}
	}
	exit(0);
}


wvlet(w,n)
float *w;
int n;
{
	int i;
	w[0] = 1.0;
	for(i=1;i<n;i++)
		w[i] = -0.5*w[i-1];
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
