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
extern bool verbose,suout;
extern char *SccsId;
 
static char lSccsId[]="@(#)varin.c	1.12\t6/8/88\n";


static bool tbal;
static struct pixrect *screen;
static unsigned char red[256],green[256],blue[256];
static char *lsdoc = 
"suvarin [OPTIONS PARAMETERS] <stdin                      \n\
Variable intensity plot of su data                        \n\
                                                          \n\
OPTIONS:                                                  \n\
    -v         turn verbose on (default off)              \n\
    -b         trace balance (default on)                 \n\
                                                          \n\
PARAMETERS:                                               \n\
    zoomx=   (int) pixel replication in x (default 1)     \n\
    zoomt=   (int) pixel replication in t (default to fill the screen)  \n\
    #ntr=                                                 \n\
    #key=cdp                                              \n\
	clip=                                                 \n\
	qclip=98                                              \n\
	dclip=1                                               \n\
    color=bwr/gray/grayr/bbr/contr/contrn color table     \n\
                                                          \n";

static char *fontpath = "/usr/lib/fonts/fixedwidthfonts/cour.b.24";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
        SccsId = lSccsId;


	/* GET OPTIONS */
	verbose = false;
	tbal = true;
	suout = false;
	while( (c=getopt(xargc,xargv,"vb"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case 'b':
			tbal = false;
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
/* 	static char *key; */
/* 	static int lablsz,of,nttic,dttic,ntr,norm,n2; */
	static int nt,noz;
	static float clip,a,b;
	static int x0,t0,x,zoomx,zoomt;
	static unsigned char *buf;
	static struct pixrect *spr;
	static struct pr_prpos prtext;
	static struct pixfont *font;
	static int cmin,cmax,ccount;
	static struct pixrect *prcl;
	float getclip();
	int i,j,iz,k;
 
	if(itr==0) {

		/* OPEN SCREEN */
		screen = pr_open("/dev/fb");

		/* GET PARAMETERS */
		x0 = 20;	igetpar("x0",&x0);
		t0 = 0;		igetpar("t0",&t0);

		zoomx = 1;		igetpar("zoomx",&zoomx);
		if(zoomx<1) {
/* 			warn(__FILE__,__LINE__,"zoomx=%d corrected to 1",zoomx); */
			zoomx = 1;
		}
		zoomt = (screen->pr_size.y-2*t0)/abh->ns;	igetpar("zoomt",&zoomt);
		if(zoomt<1) {
/* 			warn(__FILE__,__LINE__,"zoomt=%d corrected to 1",zoomt); */
			zoomt = 1;
		}

		/* COLOR TABLE */
		cmin = 1;		igetpar("cmin",&cmin);
		ccount = 255;		igetpar("ccount",&ccount);
		cmax = cmin + ccount - 1;

		SetColorTable(red,green,blue);

/* 		key="cdp";          sgetpar("key",&key); */
		noz = 0;                    igetpar("noz",&noz);
		clip = 0.0;                 fgetpar("clip",&clip);
		if(clip!=0.0) a = 0.5*ccount/clip; else a = 0.0;
		b = 0.5*(ccount);

		/* SET STATIC CONSTANTS */
		nt = abh->ns;
		buf = (unsigned char*) malloc(abh->ns*zoomt*2);
		x = x0 + 256;

		/* CLEAR THE SCREEN */
		prcl = mem_create(screen->pr_size.x,screen->pr_size.y,screen->pr_depth);
		pr_rop(screen,0,0,screen->pr_size.x,screen->pr_size.y,PIX_SRC,prcl,0,0);

		/* TITLES */
		font = pf_open(fontpath);
		if(font==NULL)
			font = pf_open("/usr/lib/fonts/fixedwidthfonts/cour.b.24");

		if(font!=NULL) {

			prtext.pr = screen;

			prtext.pos.x = x0;
			prtext.pos.y = t0+20;
			pf_ttext(prtext,PIX_SET,font,"Line:");
			prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->name);

			prtext.pos.y += 20;
			prtext.pos.x = x0;
			pf_ttext(prtext,PIX_SET,font,"Client:");
			prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->client);

			prtext.pos.y += 20;
			prtext.pos.x = x0;
			pf_ttext(prtext,PIX_SET,font,"Area:");
			prtext.pos.x = x0+120; pf_ttext(prtext,PIX_SET,font,abh->area);

			pf_close(font);

		} else {
			warn(__FILE__,__LINE__,"Could not pf_open");
		}

		/* PUT COLOR TABLE */
		ColorBar(x0/2-1,prtext.pos.y+6);

		pr_putcolormap(screen,0,256,red,green,blue);

	}

	if(clip==0.0||tbal) {
		clip = getclip(atrin->data,nt);
		if(clip!=0.0) a = 0.5*ccount/clip; else a = 0.0;
		if(verbose) fprintf(stderr,"clip=%e\n",clip);
/* 		if(noz) continue; */
	}

	for(i=0;i<nt;i++) {

		j = (unsigned char)(a*atrin->data[i]+b);

/* if(atr->data[i]>clip) */
/* fprintf(stderr,"data=%f clip=%f j=%d (a=%f b=%f)\n",atr->data[i],clip,j,a,b); */

		if(j>cmax-1) j = cmax-1;
		if(j<cmin) j = cmin;

		for(iz=0;iz<zoomt;iz++) {
			k = i*zoomt+iz;			/* why do i need this? */
			buf[2*k] = j;
			buf[2*k+1] = j;
		}

	}

	spr = mem_point(1,nt*zoomt,8,buf);

	for(iz=0;iz<zoomx;iz++) {
		pr_rop(screen,x++,t0,1,nt*zoomt,PIX_SRC,spr,0,0);
	}

/* 	if(!((itr+1)%n2)) erase(); */

/* exit(0); */

	return(0);
}

ColorBar(x0,t0)
int x0,t0;
{
	struct pixrect *spr;
	unsigned char *buf;
	int i,j;
	int w=256,h=10;

	buf = (unsigned char*) malloc((w+2)*(h+2));
	if(buf==NULL) err("ColorBar: can't malloc");

	for(i=0;i<(w+2)*(h+2);i++)
		buf[i] = 255;
	spr = mem_point(w+2,h+2,8,buf);
	pr_rop(screen,x0,t0,w+2,h+2,PIX_SRC,spr,0,0);

	for(j=0;j<h;j++) {
		for(i=0;i<w;i++) {
			buf[j*w+i] = (unsigned char) i;
		}
	}
	spr = mem_point(256,10,8,buf);
	pr_rop(screen,x0+1,t0+1,256,10,PIX_SRC,spr,0,0);
	pr_destroy(spr);
}

SetColorTable(red,green,blue)
unsigned char *red,*green,*blue;
{
	static char *color;

    red[0]   = 0; /* Background Color */
    green[0] = 0;
    blue[0]  = 0;

    red[255]   = 255; /* Text Color */
    green[255] = 255;
    blue[255]  = 0;

	if(!sgetpar("color",&color)) color="bgrayr";

	if(!strcmp(color,"grayr")) {
		grayr(red,green,blue);
	} else if(!strcmp(color,"ggrayr")) {
		ggrayr(red,green,blue);
	} else if(!strcmp(color,"bgrayr")) {
		bgrayr(red,green,blue);
	} else if(!strcmp(color,"gray")) {
		gray(red,green,blue);
	} else if(!strcmp(color,"bbr")) {
		bbr(red,green,blue);
	} else if(!strcmp(color,"bwr")) {
		bwr(red,green,blue);
	} else if(!strcmp(color,"contr")) {
		contr(red,green,blue);
	} else if(!strcmp(color,"contrn")) {
		contrn(red,green,blue);
	} else {
		warn(__FILE__,__LINE__,"Unknown color %s, using default color %s",color,"bgrayr");
		bgrayr(red,green,blue);
	}
}

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
unsigned char red[256],green[256],blue[256];
{
	gray(red,green,blue);
	red[254]   = 255;
	green[254] = 0;
	blue[254]  = 0;
}

gray(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
    int i;
 
    for(i=1;i<255;i++) {
        red[i]   = i;
        green[i] = i;
        blue[i]  = i;
    }
        green[127]=130;
        green[128]=130;
        red[127]=130;
        red[128]=130;
        blue[127]=130;
        blue[128]=130;
}

bbr(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
    int i;
 
    for(i=1;i<128;i++) {
        red[i]   = 0;
        green[i] = 0;
        blue[i]  = 255-i*2;
    }

	i=128;
    red[i]   = 0;
    green[i] = 0;
    blue[i]  = 0;

    for(;i<255;i++) {
        red[i]   = i*2;
        green[i] = 0;
        blue[i]  = 0;
    }
}

bwr(red,green,blue)
unsigned char red[256],green[256],blue[256];
{
    int i;
 
    for(i=1;i<128;i++) {
        red[i]   = i*2;
        green[i] = i*2;
        blue[i]  = 255;
    }

    red[128]   = 255;
    green[128] = 255;
    blue[128]  = 255;

    for(129;i<255;i++) {
        red[i]   = 255;
        green[i] = 255-i*2;
        blue[i]  = 255-i*2;
    }
}

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
   red[j]= 255;
   green[j]=255;
   blue[j]= 255;
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
   red[j]= 255;
   green[j]=255;
   blue[j]= 255;
  }
   break;

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

 } /*and switch*/
  } /*end for */
 } /*end function*/

postp()
{
	/* CLOSE SCREEN */
	pr_close(screen);
}
