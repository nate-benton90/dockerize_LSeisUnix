/*
 * hpen300 - vplot command interpreter for HP300 high resolution
 */

#include <starbase.c.h>
/* #include <math.h> */
#include <stdio.h>

/* Device independent constants */
#include "pen.h"

/* Device dependent constants */
/* #define ARPERIN	78 */
/* #define AYSIZE	910 */
/* #define AXSIZE	1152 */

extern int xargc;
extern char *xargv[];

static int fd;
static float r;

pen_pldone()
{
	gclose(fd);
}

pen_vpause(i)
int i;
{
	sleep(i);
}

#ifdef HP300H
#define DEV	"/dev/crt"
#define TERM	"hp300h"
#endif

#ifdef HPTERM
#define DEV	"/dev/tty"
#define TERM	"hpterm"
#endif
pen_init()
{
/* 	fd = gopen("/dev/crt",OUTDEV,"hp300h",INIT); */	/* hpen300 */
/* 	fd = gopen("/dev/tty",OUTDEV,"hpterm",INIT); */	/* hpppen */
	fd = gopen(DEV,OUTDEV,TERM,INIT);
	if(fd<0) {
		fprintf(stderr,"%s (FATAL): gopen returned %d\n",xargv[0],fd);
		exit(-1);
	}

/* 	r = 1.5*(float)ARPERIN / RPERIN; */
	r = 1.0/(10.0*RPERIN);
}

pen_erase()
{
/* 	mgiclearpln(2,-1,0); */
}

pen_move(xy)
unsigned short xy[2];
{
	float xx,yy;
	xx = r*xy[0];
	yy = r*xy[1];
	move2d(fd, xx, yy);
}

pen_draw(xy)
unsigned short xy[2];
{
	float xx,yy;
	xx = r*xy[0];
	yy = r*xy[1];
	draw2d(fd, xx, yy);
}

pen_col(col)
short int col;
{
/* 	fprintf(stderr,"pen_col NOT READY\n"); */
/*
	switch(col) {
		case RED:	mgicm(1,0xff0000);
		case GREEN:	mgicm(1,0x0000ff);
		case BLUE:	mgicm(1,0x00ff00);
		case WHITE:	mgicm(1,0xffffff);
		case BLACK:	mgicm(1,0x000000);
		case SKYBLUE:	mgicm(1,0x0fff0f);
		case YELLOW:	mgicm(1,0xffff00);
		case ORANGE:	mgicm(1,0xfffe00);
		case PURPLE:	mgicm(1,0xffff0f);
	}
*/
}

pen_fat(fat)
short int fat;
{
/* 	fprintf(stderr,"pen_fat NOT READY\n"); */
}

pen_Text(size,orient,string)
short int size,orient;
char *string;
{
	/* NOT READY */
}

pen_text(size,orient,string)
short int size,orient;
char *string;
{
}

pen_area(lp,ua,va)
short lp;
float *ua,*va;
{
	/* NOT READY */
}
