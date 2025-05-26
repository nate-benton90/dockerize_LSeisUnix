h46062
s 00004/00004/00536
d D 1.17 88/11/15 14:02:58 shuki 17 16
c 
e
s 00001/00001/00539
d D 1.16 88/06/26 14:47:53 shuki 16 15
c off zoomt=0 warn
e
s 00001/00001/00539
d D 1.15 88/06/26 12:11:02 shuki 15 14
c disabled warning on zero default zoomt
e
s 00082/00005/00458
d D 1.14 88/06/22 12:09:21 shemer 14 13
c 
e
s 00007/00001/00456
d D 1.13 88/06/21 10:17:28 shemer 13 12
c 
e
s 00085/00001/00372
d D 1.12 88/06/08 14:45:16 shemer 12 11
c with contour option
e
s 00006/00000/00367
d D 1.11 88/05/25 14:54:04 shemer 11 10
c with SccsId[]
e
s 00020/00008/00347
d D 1.10 88/05/24 07:03:08 shuki 10 9
c umainseq
e
s 00039/00007/00316
d D 1.9 88/05/19 09:34:09 shuki 9 8
c 
e
s 00048/00021/00275
d D 1.8 88/05/18 09:06:39 shuki 8 7
c 
e
s 00023/00029/00273
d D 1.7 88/05/17 10:30:17 shuki 7 6
c 
e
s 00061/00058/00241
d D 1.6 88/05/17 08:58:50 shuki 6 5
c Lots of fixes
e
s 00056/00031/00243
d D 1.5 88/05/17 05:58:21 shuki 5 4
c 
e
s 00077/00031/00197
d D 1.4 88/05/16 13:34:32 shuki 4 3
c 
e
s 00091/00019/00137
d D 1.3 88/05/15 13:43:18 shuki 3 2
c 
e
s 00008/00005/00148
d D 1.2 88/05/15 06:58:20 shuki 2 1
c 
e
s 00153/00000/00000
d D 1.1 88/05/12 11:36:40 shuki 1 0
c date and time created 88/05/12 11:36:40 by shuki
e
u
U
t
T
I 1
/*
 * varin
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

#include <pixrect/pixrect_hs.h>

extern char *sdoc;
extern int xargc;
extern char **xargv;
D 10
extern bool verbose;
E 10
I 10
extern bool verbose,suout;
I 11
extern char *SccsId;
 
D 13
static char lSccsId[]="%W%\t%G%\n";
E 13
I 13
static char lSccsId[]="@(#)varin.c	1.12\t6/8/88\n";
E 13
E 11

I 11

E 11
E 10
static bool tbal;
D 6

E 6
I 6
static struct pixrect *screen;
static unsigned char red[256],green[256],blue[256];
D 7
static char cbuf[80];
E 7
E 6
static char *lsdoc = 
D 3
"suvarin [OPTIONS PARAMETERS] <stdin          \n\
Variable intensity plot of su data            \n\
                                              \n\
OPTIONS:                                      \n\
    -v         verbose                        \n\
    -b         balance traces                 \n\
                                              \n\
PARAMETERS:                                   \n\
    zoomx=                                    \n\
D 2
    zoomy=                                    \n\
E 2
I 2
    zoomt=                                    \n\
E 2
    ntr=                                      \n\
    key=cdp                                   \n\
    title=binary header word name             \n\
                                              \n";
E 3
I 3
D 5
"suvarin [OPTIONS PARAMETERS] <stdin                                 \n\
Variable intensity plot of su data                                   \n\
                                                                     \n\
OPTIONS:                                                             \n\
    -v         verbose                                               \n\
    -b         trace balance by reselecting the clip for every trace \n\
                                                                     \n\
PARAMETERS:                                                          \n\
    zoomx=                                                           \n\
    zoomt=                                                           \n\
    ntr=                                                             \n\
    key=cdp                                                          \n\
    title=binary header word name                                    \n\
	clip=                                                            \n\
	qclip=98                                                         \n\
	dclip=1                                                          \n\
                                                                     \n";
E 5
I 5
"suvarin [OPTIONS PARAMETERS] <stdin                      \n\
Variable intensity plot of su data                        \n\
                                                          \n\
OPTIONS:                                                  \n\
    -v         turn verbose on (default off)              \n\
    -b         trace balance (default on)                 \n\
                                                          \n\
PARAMETERS:                                               \n\
D 6
    zoomx=                                                \n\
    zoomt=                                                \n\
    ntr=                                                  \n\
    key=cdp                                               \n\
    title=binary header word name                         \n\
E 6
I 6
D 10
    zoomx=   (int) pixel replication in x (default 1)             \n\
E 10
I 10
    zoomx=   (int) pixel replication in x (default 1)     \n\
E 10
    zoomt=   (int) pixel replication in t (default to fill the screen)  \n\
D 10
    #ntr=                                                  \n\
    #key=cdp                                               \n\
E 10
I 10
    #ntr=                                                 \n\
    #key=cdp                                              \n\
E 10
E 6
	clip=                                                 \n\
	qclip=98                                              \n\
	dclip=1                                               \n\
I 8
D 9
    color=bwr/gray/rgray/bbr color table                  \n\
E 9
I 9
D 12
    color=bwr/gray/grayr/bbr color table                  \n\
E 12
I 12
D 14
    color=bwr/gray/grayr/bbr/contr color table            \n\
E 14
I 14
    color=bwr/gray/grayr/bbr/contr/contrn color table     \n\
E 14
E 12
E 9
E 8
                                                          \n";
I 10

E 10
I 6
static char *fontpath = "/usr/lib/fonts/fixedwidthfonts/cour.b.24";
E 6
E 5
E 3

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
I 11
        SccsId = lSccsId;

E 11

	/* GET OPTIONS */
	verbose = false;
D 5
	tbal = false;
E 5
I 5
	tbal = true;
I 10
	suout = false;
E 10
E 5
D 3
	while( (c=getopt(xargc,xargv,"v"))!=EOF) {
E 3
I 3
	while( (c=getopt(xargc,xargv,"vb"))!=EOF) {
E 3
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case 'b':
D 5
			tbal = true;
E 5
I 5
			tbal = false;
E 5
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
}

D 6
static struct pixrect *screen;
I 5
static unsigned char red[256],green[256],blue[256];
E 5

E 6
D 10
trseqn(itr,atr,abh)
E 10
I 10

prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}
 
trseq(itr,atrin,atrout,abh)
E 10
int itr;
D 10
Sutrace *atr;
E 10
I 10
Sutrace *atrin,*atrout;
E 10
Subhed *abh;
{
D 6
	static char key[32],title[32];
/* 	static int lablsz,of,nttic,dttic,titlesz,titlefat,ntr,norm,n2; */
E 6
I 6
D 17
/* 	static char key[32]; */
E 17
I 17
/* 	static char *key; */
E 17
/* 	static int lablsz,of,nttic,dttic,ntr,norm,n2; */
E 6
	static int nt,noz;
D 4
	static float clip,normfac;
E 4
I 4
	static float clip,a,b;
E 4
D 2
	static int x0,t0,x=0,zoomx,zoomt;
E 2
I 2
	static int x0,t0,x,zoomx,zoomt;
E 2
D 5
	static unsigned char *buf, red[256],green[256],blue[256];
E 5
I 5
	static unsigned char *buf;
E 5
	static struct pixrect *spr;
I 5
	static struct pr_prpos prtext;
	static struct pixfont *font;
	static int cmin,cmax,ccount;
I 6
	static struct pixrect *prcl;
E 6
E 5
	float getclip();
D 4
	int i,j,iz;
E 4
I 4
D 5
	int i,j,iz,cmin,cmax,ccount;
E 5
I 5
D 6
	int i,j,iz;
E 6
I 6
	int i,j,iz,k;
E 6
E 5
E 4
 
	if(itr==0) {

I 4
		/* OPEN SCREEN */
		screen = pr_open("/dev/fb");

E 4
		/* GET PARAMETERS */
I 2
D 5
		x0 = 20;		igetpar("x0",&x0);
		t0 = 20;		igetpar("t0",&t0);
E 5
I 5
		x0 = 20;	igetpar("x0",&x0);
		t0 = 0;		igetpar("t0",&t0);

E 5
E 2
		zoomx = 1;		igetpar("zoomx",&zoomx);
		if(zoomx<1) {
D 15
			warn(__FILE__,__LINE__,"zoomx=%d corrected to 1",zoomx);
E 15
I 15
/* 			warn(__FILE__,__LINE__,"zoomx=%d corrected to 1",zoomx); */
E 15
			zoomx = 1;
		}
D 2
		zoomt = 1/*880/abh->ns*/;		igetpar("zoomt",&zoomt);
E 2
I 2
D 4
		zoomt = 2*(880-2*t0)/abh->ns;	igetpar("zoomt",&zoomt); /* WHY 2 ? */
E 4
I 4
D 6
		zoomt = 2*(screen->pr_size.y-2*t0)/abh->ns;	igetpar("zoomt",&zoomt);
																/* WHY 2 ? */
E 6
I 6
		zoomt = (screen->pr_size.y-2*t0)/abh->ns;	igetpar("zoomt",&zoomt);
E 6
E 4
E 2
		if(zoomt<1) {
D 16
			warn(__FILE__,__LINE__,"zoomt=%d corrected to 1",zoomt);
E 16
I 16
/* 			warn(__FILE__,__LINE__,"zoomt=%d corrected to 1",zoomt); */
E 16
			zoomt = 1;
		}
I 4

		/* COLOR TABLE */
D 5
		cmin = 2;		igetpar("cmin",&cmin);
		ccount = 254;		igetpar("ccount",&ccount);
E 5
I 5
		cmin = 1;		igetpar("cmin",&cmin);
		ccount = 255;		igetpar("ccount",&ccount);
E 5
		cmax = cmin + ccount - 1;

D 6
    	red[0]   = 0; /* Background */
E 6
I 6
D 8
    	red[0]   = 0; /* Background Color */
E 6
    	green[0] = 0;
    	blue[0]  = 0;
E 8
I 8
		SetColorTable(red,green,blue);
E 8

D 5
    	red[1]   = 0; /* Text */
    	green[1] = 1;
    	blue[1]  = 0;
E 5
I 5
D 6
    	red[255]   = 255;
E 6
I 6
D 8
    	red[255]   = 255; /* Text Color */
E 6
    	green[255] = 255;
    	blue[255]  = 0;
E 5

/* 		rgray(red,green,blue); */
		bwr(red,green,blue);
/* 		bbr(red,green,blue); */

E 8
D 6
		b = 0.5*(ccount);
E 4
		clip = 0.0;		fgetpar("clip",&clip);
E 6
I 6
D 17
/* 		strcpy(key,"cdp");          sgetpar("key",key); */
E 17
I 17
/* 		key="cdp";          sgetpar("key",&key); */
E 17
		noz = 0;                    igetpar("noz",&noz);
		clip = 0.0;                 fgetpar("clip",&clip);
E 6
D 3
		normfac = 0.0;
E 3
I 3
D 4
		if(clip!=0.0) normfac = 127.0/clip;
		else normfac = 0.0;
E 4
I 4
		if(clip!=0.0) a = 0.5*ccount/clip; else a = 0.0;
E 4
E 3
D 6
		strcpy(key,"cdp");		sgetpar("key",key);
		if(!sgetpar("title",title)) 	gname(0,title);
D 2
		x0 = 50;		igetpar("x0",&x0);
		t0 = 50;		igetpar("t0",&t0);
E 2
		noz = 0;		igetpar("noz",&noz);
E 6
I 6
		b = 0.5*(ccount);
E 6

		/* SET STATIC CONSTANTS */
		nt = abh->ns;
D 6
		buf = (unsigned char*) malloc(abh->ns*zoomt);
E 6
I 6
		buf = (unsigned char*) malloc(abh->ns*zoomt*2);
E 6
I 4
		x = x0 + 256;
E 4

D 4
		/* COLOR TABLE */
D 3
		gray(red,green,blue);
E 3
I 3
/* 		rgray(red,green,blue); */
		bwr(red,green,blue);
/* 		bbr(red,green,blue); */
E 4
I 4
D 6
		ClearPR(screen);
E 6
I 6
		/* CLEAR THE SCREEN */
		prcl = mem_create(screen->pr_size.x,screen->pr_size.y,screen->pr_depth);
		pr_rop(screen,0,0,screen->pr_size.x,screen->pr_size.y,PIX_SRC,prcl,0,0);
E 6
E 4
E 3

D 4
		/* OPEN SCREEN */
D 3
		system("/usr/bin/clear_colormap");
E 3
I 3
/* 		system("/usr/bin/clear_colormap"); */
E 3
		screen = pr_open("/dev/fb");
E 4
I 4
D 5
		ColorBar(x0/2,t0+200);
E 5
I 5
D 6
		/* Title */
/*
		font = pf_open("/usr/lib/fonts/fixedwidthfonts/cour.b.24");
E 6
I 6
D 7
		/* TITLE */
/* 		font = pf_open("/usr/lib/fonts/fixedwidthfonts/cour.b.24"); */
E 7
I 7
		/* TITLES */
E 7
		font = pf_open(fontpath);
I 7
		if(font==NULL)
			font = pf_open("/usr/lib/fonts/fixedwidthfonts/cour.b.24");
E 7
E 6
E 5
E 4

I 5
D 7
		prtext.pr = screen;
E 7
I 7
		if(font!=NULL) {
E 7

D 7
		prtext.pos.x = x0;
		prtext.pos.y = t0+20;
D 6
		sprintf(buf,"Line:   %s",abh->name);
		pf_ttext(prtext,PIX_SET,font,buf);
E 6
I 6
/* 		sprintf(cbuf,"Line:   %s",abh->name); */
/* fprintf(stderr,"cbuf=%s\n",cbuf); */
/* printf("%s\n",cbuf); */
/* 		pf_ttext(prtext,PIX_SET,font,cbuf); */
		pf_ttext(prtext,PIX_SET,font,"Line:");
		prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->name);
E 7
I 7
			prtext.pr = screen;
E 7
E 6

D 7
		prtext.pos.y += 20;
D 6
		sprintf(buf,"Area:   %s",abh->area);
		pf_ttext(prtext,PIX_SET,font,buf);
E 6
I 6
		prtext.pos.x = x0;
/* 		sprintf(cbuf,"Client: %s",abh->client); */
/* fprintf(stderr,"cbuf=%s\n",cbuf); */
/* printf("%s\n",cbuf); */
/* 		pf_ttext(prtext,PIX_SET,font,cbuf); */
		pf_ttext(prtext,PIX_SET,font,"Client:");
		prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->client);
E 7
I 7
			prtext.pos.x = x0;
			prtext.pos.y = t0+20;
			pf_ttext(prtext,PIX_SET,font,"Line:");
			prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->name);
E 7
E 6

D 7
		prtext.pos.y += 20;
D 6
		sprintf(buf,"Client: %s",abh->client);
		pf_ttext(prtext,PIX_SET,font,buf);
E 6
I 6
		prtext.pos.x = x0;
/* 		sprintf(cbuf,"Area:   %s",abh->area); */
/* fprintf(stderr,"cbuf=%s\n",cbuf); */
/* printf("%s\n",cbuf); */
/* 		pf_ttext(prtext,PIX_SET,font,cbuf); */
		pf_ttext(prtext,PIX_SET,font,"Area:");
		prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->area);
E 7
I 7
			prtext.pos.y += 20;
			prtext.pos.x = x0;
			pf_ttext(prtext,PIX_SET,font,"Client:");
			prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->client);
E 7
E 6

D 7
		pf_close(font);
E 7
I 7
			prtext.pos.y += 20;
			prtext.pos.x = x0;
			pf_ttext(prtext,PIX_SET,font,"Area:");
			prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->area);

			pf_close(font);

		} else {
			warn(__FILE__,__LINE__,"Could not pf_open");
		}
E 7
D 6
*/
E 6

E 5
		/* PUT COLOR TABLE */
I 5
D 8
		ColorBar(x0/2,prtext.pos.y+5);
E 8
I 8
		ColorBar(x0/2-1,prtext.pos.y+6);
E 8
I 6

E 6
E 5
D 4
		pr_putcolormap(screen,0,255,red,green,blue);
E 4
I 4
		pr_putcolormap(screen,0,256,red,green,blue);
E 4
I 2

D 4
		x = x0;
I 3

/* 		pr_rop(screen,0,0,1152,880,PIX_SRC,NULL,0,0); */

		buf = (unsigned char*) calloc(1152*880,1);
		spr = mem_point(1152,880,8,buf);
		pr_rop(screen,0,0,1152,880,PIX_SRC,spr,0,0);

E 4
E 3
E 2
	}

	if(clip==0.0||tbal) {
D 10
		clip = getclip(atr->data,nt);
E 10
I 10
		clip = getclip(atrin->data,nt);
E 10
D 4
		if(clip!=0.0) normfac = 127.0/clip;
		else normfac = 0.0;
E 4
I 4
		if(clip!=0.0) a = 0.5*ccount/clip; else a = 0.0;
E 4
		if(verbose) fprintf(stderr,"clip=%e\n",clip);
/* 		if(noz) continue; */
	}

	for(i=0;i<nt;i++) {
I 6

E 6
D 4
		j = (unsigned char)(normfac*atr->data[i]+127.0);
E 4
I 4
D 10
		j = (unsigned char)(a*atr->data[i]+b);
E 10
I 10
		j = (unsigned char)(a*atrin->data[i]+b);
E 10
E 4
I 3

/* if(atr->data[i]>clip) */
D 4
/* fprintf(stderr,"data=%f clip=%f j=%d\n",atr->data[i],clip,j); */
E 4
I 4
/* fprintf(stderr,"data=%f clip=%f j=%d (a=%f b=%f)\n",atr->data[i],clip,j,a,b); */
E 4

E 3
D 4
		if(j>255) j= 255;
		if(j<0) j = 0;
E 4
I 4
		if(j>cmax-1) j = cmax-1;
		if(j<cmin) j = cmin;
E 4
I 2

E 2
		for(iz=0;iz<zoomt;iz++) {
D 6
			buf[i*zoomt+iz] = j;
E 6
I 6
			k = i*zoomt+iz;			/* why do i need this? */
			buf[2*k] = j;
			buf[2*k+1] = j;
E 6
		}
I 6

E 6
	}

	spr = mem_point(1,nt*zoomt,8,buf);
I 6

E 6
	for(iz=0;iz<zoomx;iz++) {
		pr_rop(screen,x++,t0,1,nt*zoomt,PIX_SRC,spr,0,0);
I 3
D 4
/* 		pr_rop(screen,x++,t0,1,nt*zoomt,PIX_SET,spr,0,0); */
E 4
E 3
	}

/* 	if(!((itr+1)%n2)) erase(); */

I 4
/* exit(0); */

E 4
	return(0);
}

I 4
D 6
ClearPR(target)
struct pixrect *target;
{
	unsigned char *buf;
	struct pixrect *spr;

	buf = (unsigned char*) calloc(target->pr_size.x*target->pr_size.y,1);
	spr = mem_point(target->pr_size.x,target->pr_size.y,8,buf);
	pr_rop(target,0,0,target->pr_size.x,target->pr_size.y,PIX_SRC,spr,0,0);
	pr_destroy(spr);
	free(buf);
}

E 6
ColorBar(x0,t0)
int x0,t0;
{
	struct pixrect *spr;
D 8
	unsigned char buf[2560];
E 8
I 8
	unsigned char *buf;
E 8
	int i,j;
I 8
	int w=256,h=10;
E 8

D 8
	for(j=0;j<10;j++) {
		for(i=0;i<256;i++) {
			buf[j*256+i] = (unsigned char) i;
E 8
I 8
	buf = (unsigned char*) malloc((w+2)*(h+2));
	if(buf==NULL) err("ColorBar: can't malloc");

	for(i=0;i<(w+2)*(h+2);i++)
		buf[i] = 255;
	spr = mem_point(w+2,h+2,8,buf);
	pr_rop(screen,x0,t0,w+2,h+2,PIX_SRC,spr,0,0);

	for(j=0;j<h;j++) {
		for(i=0;i<w;i++) {
			buf[j*w+i] = (unsigned char) i;
E 8
		}
	}
	spr = mem_point(256,10,8,buf);
D 8
	pr_rop(screen,x0,t0,256,10,PIX_SRC,spr,0,0);
E 8
I 8
	pr_rop(screen,x0+1,t0+1,256,10,PIX_SRC,spr,0,0);
E 8
	pr_destroy(spr);
}

I 8
SetColorTable(red,green,blue)
unsigned char *red,*green,*blue;
{
D 17
	static char color[32];
E 17
I 17
	static char *color;
E 17

    red[0]   = 0; /* Background Color */
    green[0] = 0;
    blue[0]  = 0;

    red[255]   = 255; /* Text Color */
    green[255] = 255;
    blue[255]  = 0;

D 9
	if(!sgetpar("color",color)) strcpy(color,"bwr");
E 9
I 9
D 17
	if(!sgetpar("color",color)) strcpy(color,"bgrayr");
E 17
I 17
	if(!sgetpar("color",&color)) color="bgrayr";
E 17
E 9

D 9
	if(!strcmp(color,"rgray")) {
		rgray(red,green,blue);
E 9
I 9
	if(!strcmp(color,"grayr")) {
		grayr(red,green,blue);
	} else if(!strcmp(color,"ggrayr")) {
		ggrayr(red,green,blue);
	} else if(!strcmp(color,"bgrayr")) {
		bgrayr(red,green,blue);
E 9
	} else if(!strcmp(color,"gray")) {
		gray(red,green,blue);
	} else if(!strcmp(color,"bbr")) {
		bbr(red,green,blue);
	} else if(!strcmp(color,"bwr")) {
		bwr(red,green,blue);
I 12
	} else if(!strcmp(color,"contr")) {
		contr(red,green,blue);
I 14
	} else if(!strcmp(color,"contrn")) {
		contrn(red,green,blue);
E 14
E 12
	} else {
D 9
		warn(__FILE__,__LINE__,"Unknown color %s, using default color %s",color,"bwr");
		bwr(red,green,blue);
E 9
I 9
		warn(__FILE__,__LINE__,"Unknown color %s, using default color %s",color,"bgrayr");
		bgrayr(red,green,blue);
E 9
	}
}

E 8
E 4
I 3
D 9
rgray(red,green,blue)
E 9
I 9
bgrayr(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
	gray(red,green,blue);

	red[1]   = 0;
	green[1] = 0;
	blue[1]  = 255;

	red[254]   = 255;
	green[254] = 0;
	blue[254]  = 0;
}

ggrayr(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
	gray(red,green,blue);

	red[1]   = 0;
	green[1] = 255;
	blue[1]  = 0;

	red[254]   = 255;
	green[254] = 0;
	blue[254]  = 0;
}

grayr(red,green,blue)
E 9
unsigned char red[256],green[256],blue[256];
{
	gray(red,green,blue);
I 8
	red[254]   = 255;
E 8
D 6
	green[255] = 0;
	blue[255]  = 0;
E 6
I 6
	green[254] = 0;
	blue[254]  = 0;
E 6
}

E 3
gray(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
    int i;
 
D 3
    for(i=0;i<255;i++) {
E 3
I 3
D 4
    for(i=0;i<256;i++) {
E 4
I 4
D 8
    red[0]   = 0;
    green[0] = 0;
    blue[0]  = 0;
    for(i=1;i<256;i++) {
E 8
I 8
    for(i=1;i<255;i++) {
E 8
E 4
E 3
        red[i]   = i;
        green[i] = i;
        blue[i]  = i;
I 3
    }
I 13
        green[127]=130;
        green[128]=130;
        red[127]=130;
        red[128]=130;
        blue[127]=130;
        blue[128]=130;
E 13
}

bbr(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
    int i;
 
D 4
    for(i=0;i<128;i++) {
E 4
I 4
D 6
    red[0]   = 0;
    green[0] = 0;
    blue[0]  = 0;
E 6
    for(i=1;i<128;i++) {
E 4
        red[i]   = 0;
        green[i] = 0;
        blue[i]  = 255-i*2;
    }

	i=128;
    red[i]   = 0;
    green[i] = 0;
    blue[i]  = 0;

D 6
    for(;i<256;i++) {
E 6
I 6
    for(;i<255;i++) {
E 6
        red[i]   = i*2;
        green[i] = 0;
        blue[i]  = 0;
    }
}

bwr(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
    int i;
 
D 4
    for(i=0;i<128;i++) {
E 4
I 4
D 5
    for(i=2;i<128;i++) {
E 5
I 5
    for(i=1;i<128;i++) {
E 5
E 4
        red[i]   = i*2;
        green[i] = i*2;
        blue[i]  = 255;
    }

D 6
	i=128;
    red[i]   = 255;
    green[i] = 255;
    blue[i]  = 255;
D 5

E 5
    for(;i<256;i++) {
E 6
I 6
    red[128]   = 255;
    green[128] = 255;
    blue[128]  = 255;

    for(129;i<255;i++) {
E 6
        red[i]   = 255;
        green[i] = 255-i*2;
        blue[i]  = 255-i*2;
E 3
    }
}
I 12

contr(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
  int i,j;
   for(i=1;i< 128;i++)
   { 
    red[i] = 100;
    green[i]=100;
    blue[i] =100;
   }

   for(i=0;i<8;i++){
   switch(i){
   case 0:/* black*/
   for(j=128;j<145;j++){
D 14
   red[j]= 0;
   green[j]=0;
   blue[j]= 0;
E 14
I 14
   red[j]= 255;
   green[j]=255;
   blue[j]= 255;
E 14
  }
   break;

  case 1: /*violet*/
  for(j;j<=16*(i+1)+128;j++){
   red[j]= 255;
   green[j]=0;
   blue[j]= 255;
  }
   break;

  case 2: /*blue*/
  for(j;j<=16*(i+1)+128;j++){
   red[j]= 0;
   green[j]=0;
   blue[j]= 255;
  }
   break;

  case 3: /*sky blue*/
  for(j;j<=16*(i+1)+128;j++){
   red[j]= 0;
   green[j]=255;
   blue[j]= 255;
  }
   break;

  case 4: /*green   */
  for(j;j<=16*(i+1)+128;j++){
   red[j]= 0;
   green[j]=255;
   blue[j]= 0;
  }
   break;

  case 5: /*yellow   */
  for(j;j<=16*(i+1)+128;j++){
   red[j]= 255;
   green[j]=255;
   blue[j]= 0;
  }
   break;

  case 6: /*red   */
  for(j;j<=16*(i+1)+128;j++){
   red[j]= 255;
   green[j]=0;
   blue[j]= 0;
  }
   break;

  case 7: /*white   */
  for(j;j<255;j++){
I 14
   red[j]= 0;
   green[j]=0;
   blue[j]= 0;
  }
   break;

 } /*and switch*/
  } /*end for */
 } /*end function*/


contrn(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
  int i,j;

   for(i=0;i<8;i++){
   switch(i){
   case 0:/* black*/
   for(j=1;j<33;j++){
E 14
   red[j]= 255;
   green[j]=255;
   blue[j]= 255;
  }
   break;

I 14
  case 1: /*violet*/
  for(j;j<=32*(i+1);j++){
   red[j]= 255;
   green[j]=0;
   blue[j]= 255;
  }
   break;

  case 2: /*blue*/
  for(j;j<=32*(i+1);j++){
   red[j]= 0;
   green[j]=0;
   blue[j]= 255;
  }
   break;

  case 3: /*sky blue*/
  for(j;j<=32*(i+1);j++){
   red[j]= 0;
   green[j]=255;
   blue[j]= 255;
  }
   break;

  case 4: /*green   */
  for(j;j<=32*(i+1);j++){
   red[j]= 0;
   green[j]=255;
   blue[j]= 0;
  }
   break;

  case 5: /*yellow   */
  for(j;j<=32*(i+1);j++){
   red[j]= 255;
   green[j]=255;
   blue[j]= 0;
  }
   break;

  case 6: /*red   */
  for(j;j<=32*(i+1);j++){
   red[j]= 255;
   green[j]=0;
   blue[j]= 0;
  }
   break;

  case 7: /*white   */
  for(j;j<255;j++){
   red[j]= 0;
   green[j]=0;
   blue[j]= 0;
  }
   break;

E 14
 } /*and switch*/
  } /*end for */
 } /*end function*/
D 14

E 14
E 12

postp()
{
	/* CLOSE SCREEN */
	pr_close(screen);
}
E 1
