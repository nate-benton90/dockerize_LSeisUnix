/*
 * sunpen
 */

#include <cgidefs.h>
#include <stdio.h>
#include "pen.h" /* Device independent constants */

static Ccoor ends[2];
static Ccoorlist polycoors;
static Cint name;
static Cvwsurf device;

/* Device dependent constants */
/* #define ARPERIN	78 */
/* #define AYSIZE	910 */
/* #define AXSIZE	1152 */

extern int xargc;
extern char *xargv[];

static int fd;
static int r;
static FILE *tty;

pen_pldone()
{
	tty = fopen("/dev/tty","r");
	if(tty==NULL) err("tty=NULL");
	getc(tty);
	close_vws(name);
	close_cgi();
}

pen_vpause(i)
int i;
{
	sleep(i);
}

pen_init()
{
	polycoors.n = 2;
	polycoors.ptlist = ends;
	NORMAL_VWSURF(device,PIXWINDD);
	open_cgi();
	open_vws(&name,&device);
/* 	r = 1.0/(10.0*RPERIN); */
	r = 3;
}

pen_erase()
{
/* 	mgiclearpln(2,-1,0); */
}

pen_move(xy)
unsigned short xy[2];
{
	int xx,yy;

	xx = r*xy[0];
	yy = r*xy[1];

	ends[0].x = xx;
	ends[0].y = yy;
}

pen_draw(xy)
unsigned short xy[2];
{
	int xx,yy;

	xx = r*xy[0];
	yy = r*xy[1];

	ends[1].x = xx;
	ends[1].y = yy;

	polyline(&polycoors);

	ends[0].x = xx;
	ends[0].y = yy;
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
int *ua,*va;
{
	/* NOT READY */
}
