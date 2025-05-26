/*
 * plot
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose,suout;
extern char *SccsId;
 
static char lSccsId[]="@(#)plot.c	1.5\t11/15/88\n";


static char *lsdoc = 
"suplot [OPTIONS PARAMETERS] <stdin | tube            \n\
	plot of segy data                                 \n\
                                                      \n\
OPTIONS:                                              \n\
	-v verbose on                                     \n\
                                                      \n\
PARAMETERS:                                           \n\
	wiggle=y wiggle plot                              \n\
	vararea=y variable area                           \n\
	dither=n dithering (turns off wiggle and vararea) \n\
 dx=0.1                                               \n\
	overlap=1.0                                       \n\
	x0=dx*overlap                                     \n\
	t0=2.0      	                                  \n\
	verbose=0                                         \n\
	black=0.0001                                      \n\
	tsize=5.0                                         \n\
	key=cdp                                           \n\
	title=binary header word name                     \n\
	ntr=1000000000                                    \n\
	n2=ntr                                            \n\
                                                      \n";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
        SccsId = lSccsId;


	/* GET OPTIONS */
	verbose = false;
	suout = false;
	while( (c=getopt(xargc,xargv,"v"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
}

prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}            
 
trseq(itr,atrin,atrout,abh)
int itr;
Sutrace *atrin,*atrout;
Subhed *abh;
{
	static char *key,*title;
	static int lablsz,i,of,nttic,dttic,titlesz,titlefat,ntr,norm,noz,n2;
	static float dt,clip,overlap,xsize,tsize,x0,t0,x,dx,black;
	float getclip();

	if(itr==0) {
		/* GET PARAMETERS */
		t0 = 2.0;			fgetpar("t0",&t0);
		dx = 0.1;			fgetpar("dx",&dx);
		overlap = 1.0;			fgetpar("overlap",&overlap);
		x0 = dx*overlap;		fgetpar("x0",&x0);
		tsize = 5.0;			fgetpar("tsize",&tsize);
		black = 0.0001;			fgetpar("black",&black);
		clip = 0.0;			fgetpar("clip",&clip);
		key="cdp";			sgetpar("key",&key);
		if(!sgetpar("title",&title)) 	title = gname(0);
		ntr = 1000000000;		lgetpar("ntr",&ntr);
		n2 = ntr;			lgetpar("n2",&n2);
		noz = 0;			lgetpar("noz",&noz);
		norm = 0;			lgetpar("norm",&norm);
		dt = tsize/(abh->ns-1);
	}

/* 	red(); */

	if(clip==0.0||norm) {
		clip = getclip(atrin->data,abh->ns);
		if(verbose) fprintf(stderr,"clip=%e\n",clip);
/* 		if(noz) continue; */
	}

	if(clip!=0.0||noz==0) {
		if(clip!=0.0) balclip(atrin->data,abh->ns,clip);
		x = x0 + (itr%n2)*dx;
		set0(x,t0+tsize);
		setscl(dx*overlap,-dt);
		wgl1(atrin->data,abh->ns,black);
	}

	if(!((itr+1)%n2)) erase();

	return(0);
}

xlabel(i,lablsz,dx)
float dx;
{
	char chari[50];

	if(i<0)	sprintf(chari,"%d",i);
	else 	sprintf(chari," %d",i);	/* space instead of the - */
 	
	utext(-0.2*lablsz,0.016*lablsz/dx,lablsz,2,chari);
}

wgl1(f,n,zblack)
float *f,zblack;
{
	int i,lp=0;
	static first=1;
	static float *xp,*yp;

	if(first) {
		xp = (float*) malloc(4*(n+2));
		yp = (float*) malloc(4*(n+2));
		first = 0;
	}
	if(zblack<=f[0] && zblack<f[1]) {
		yp[0] = 0.0;
		xp[0] = zblack;
		yp[1] = 0.0;
		xp[1] = f[0];
		lp = 2;
	}
	umove(f[0],0.0);
	for(i=1;i<n;i++)
	{
		udraw(f[i],(float)i);
		if(f[i]>zblack) {
			if(f[i-1]<=zblack) {
				yp[0] = (zblack-f[i])/(f[i]-f[i-1]) + i;
				xp[0] = zblack;
				lp = 1;
			}
			yp[lp] = i;
			xp[lp] = f[i];
			lp++;
		}
		if(f[i]<zblack&&f[i-1]>zblack) {
			yp[lp] = (zblack-f[i])/(f[i]-f[i-1]) + i;
			xp[lp] = zblack;
			lp++;
/* 			if(lp>2) uarea(xp,yp,lp,0,1,1); *//*<<<<<<<<<<<<<<<<*/
			lp = 0;
		} else if (i==n-1 && lp) {
			yp[lp] = i;
			xp[lp] = zblack;
			lp++;
/* 			if(lp>2) uarea(xp,yp,lp,0,1,1); *//*<<<<<<<<<<<<<<<<*/
		}
	}
}

postp(){}
