h00152
s 00003/00003/00178
d D 1.5 88/11/15 14:02:41 shuki 5 4
c 
e
s 00005/00006/00176
d D 1.4 88/06/06 13:12:17 shuki 4 3
c Cancel ns in trace headers
e
s 00006/00000/00176
d D 1.3 88/05/25 14:53:48 shemer 3 2
c with SccsId[]
e
s 00038/00029/00138
d D 1.2 88/05/24 07:11:51 shuki 2 1
c umainseq
e
s 00167/00000/00000
d D 1.1 88/04/14 13:52:35 shuki 1 0
c date and time created 88/04/14 13:52:35 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * plot
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
D 2
extern bool verbose;
E 2
I 2
extern bool verbose,suout;
I 3
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";
E 3
E 2

I 3

E 3
static char *lsdoc = 
D 2
"suplot [OPTIONS PARAMETERS] <stdin | tube			\n\
	plot of segy data					\n\
								\n\
OPTIONS:							\n\
	-v verbose on						\n\
								\n\
PARAMETERS:							\n\
	wiggle=y	wiggle plot				\n\
	vararea=y	variable area				\n\
	dither=n	dithering (turns off wiggle and vararea)\n\
	dx=0.1							\n\
	overlap=1.0						\n\
	x0=dx*overlap						\n\
	t0=2.0							\n\
	verbose=0						\n\
	black=0.0001						\n\
	tsize=5.0						\n\
	key=cdp							\n\
	title=binary header word name				\n\
	ntr=1000000000						\n\
	n2=ntr							\n\
								\n";
E 2
I 2
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
E 2

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
I 3
        SccsId = lSccsId;

E 3

	/* GET OPTIONS */
	verbose = false;
I 2
	suout = false;
E 2
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

D 2
trseqn(itr,atr,abh)
E 2
I 2
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}            
 
trseq(itr,atrin,atrout,abh)
E 2
int itr;
D 2
Sutrace *atr;
E 2
I 2
Sutrace *atrin,*atrout;
E 2
Subhed *abh;
{
D 5
	static char key[32],title[32];
E 5
I 5
	static char *key,*title;
E 5
D 4
	static int nt,lablsz,i,of,nttic,dttic,titlesz,titlefat,ntr,norm,noz,n2;
E 4
I 4
	static int lablsz,i,of,nttic,dttic,titlesz,titlefat,ntr,norm,noz,n2;
E 4
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
D 5
		strcpy(key,"cdp");		sgetpar("key",key);
		if(!sgetpar("title",title)) 	gname(0,title);
E 5
I 5
		key="cdp";			sgetpar("key",&key);
		if(!sgetpar("title",&title)) 	title = gname(0);
E 5
		ntr = 1000000000;		lgetpar("ntr",&ntr);
		n2 = ntr;			lgetpar("n2",&n2);
		noz = 0;			lgetpar("noz",&noz);
		norm = 0;			lgetpar("norm",&norm);
D 2
		nt = atr->ns;
E 2
I 2
D 4
		nt = atrin->ns;
E 2
		dt = tsize/(nt-1);
E 4
I 4
		dt = tsize/(abh->ns-1);
E 4
	}

/* 	red(); */

	if(clip==0.0||norm) {
D 2
		clip = getclip(atr->data,nt);
E 2
I 2
D 4
		clip = getclip(atrin->data,nt);
E 4
I 4
		clip = getclip(atrin->data,abh->ns);
E 4
E 2
		if(verbose) fprintf(stderr,"clip=%e\n",clip);
/* 		if(noz) continue; */
	}

	if(clip!=0.0||noz==0) {
D 2
		if(clip!=0.0) balclip(atr->data,nt,clip);
E 2
I 2
D 4
		if(clip!=0.0) balclip(atrin->data,nt,clip);
E 4
I 4
		if(clip!=0.0) balclip(atrin->data,abh->ns,clip);
E 4
E 2
		x = x0 + (itr%n2)*dx;
		set0(x,t0+tsize);
		setscl(dx*overlap,-dt);
D 2
		wgl1(atr->data,nt,black);
E 2
I 2
D 4
		wgl1(atrin->data,nt,black);
E 4
I 4
		wgl1(atrin->data,abh->ns,black);
E 4
E 2
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
E 1
