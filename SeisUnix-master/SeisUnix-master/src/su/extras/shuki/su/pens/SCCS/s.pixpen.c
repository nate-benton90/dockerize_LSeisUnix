h29337
s 00000/00000/00170
d D 1.4 88/11/15 14:07:41 shuki 4 3
c 
e
s 00020/00002/00150
d D 1.3 88/08/23 09:50:47 rafi 3 2
c edit text and Text
e
s 00087/00012/00065
d D 1.2 88/08/17 10:23:22 rafi 2 1
c color area
e
s 00077/00000/00000
d D 1.1 88/08/03 15:05:21 rafi 1 0
c date and time created 88/08/03 15:05:21 by rafi
e
u
U
t
T
I 1
/*
* pixpen
*/

#include <pixrect/pixrect_hs.h>
#include <stdio.h>
D 2
/*#include "pen.h" *//*Device independed constsnts */
E 2
I 2
#include "pen.h" /*Device independed constsnts */
E 2

D 2
#define AXSIZE 1152
#define AYSIZE 900
E 2
I 2
#define AXSIZE 1152.
#define AYSIZE 900.
E 2

#define NULLPR (struct pixrect *)NULL

extern int xargc;
extern char *xargv[];

static struct pixrect *screen;

static int xx0,yy0;   /* current position of the curser */
D 2
static int r[2];
E 2
I 2
static float r[2];
unsigned char red[8],green[8],blue[8];
int op;
E 2

D 2
#define DEV dev/fb
E 2
I 2
#define DEV "/dev/fb"
E 2

pen_init()
{
D 2
	screen=pr_open("DEV");
E 2
I 2
	int count;
	screen=pr_open(DEV);
E 2
	if(screen==NULL){
		fprintf(stderr,"%s (FATAL):pr_open returned %d\n",xargv[0],
                   screen);
		exit(-1);
	}
	r[0]=AXSIZE / 10000;
	r[1]=AYSIZE / 10000;
I 2

  /* Set color table of 8 color  */
	red[BLACK]=0;	 green[BLACK]=0;     blue[BLACK]=0;
	red[WHITE]=255;  green[WHITE]=255;   blue[WHITE]=255;
	red[RED]=255;	 green[RED]=0;       blue[RED]=0;
	red[GREEN]=0;    green[GREEN]=255;   blue[GREEN]=0;
	red[BLUE]=0;     green[BLUE]=0;      blue[BLUE]=255;
	red[SKYBLUE]=0;  green[SKYBLUE]=255; blue[SKYBLUE]=255;
	red[PURPLE]=255; green[PURPLE]=0;    blue[PURPLE]=255;
	red[YELLOW]=255; green[YELLOW]=255;  blue[YELLOW]=0;

	pr_putcolormap(screen,0,8,red,green,blue);

	/* Erase the screen */
	pen_col(BLACK);
	pen_erase();

	/* Default color */
	pen_col(RED);
E 2
}

pen_move(xy)
unsigned short xy[2];
{
	xx0=r[0]*xy[0];
	yy0=r[1]*xy[1];
}
 

D 2
pen_drow(xy)
E 2
I 2
pen_draw(xy)
E 2
unsigned short xy[2];
{
	int xx,yy;
	xx=r[0]*xy[0];
	yy=r[1]*xy[1];
D 2
	pr_vector(screen,xx0,yy0,xx,yy,PIX_SET,1);
E 2
I 2
	pr_vector(screen,xx0,yy0,xx,yy,op,1);
E 2
	xx0=xx;
	yy0=yy;
}

pen_area(lp,ua,va)
short int lp;
D 2
float *ua,*va;
E 2
I 2
unsigned short *ua,*va;
E 2
{
	struct pr_pos *vlist;
	int nbnds=1,npts[1],i;
	npts[0]=lp;

	vlist=(struct pr_pos *)malloc(lp*sizeof(struct pr_pos));
 
	for(i=0;i<lp;i++){
D 2
		vlist[i].x=ua[i];
		vlist[i].y=va[i];
E 2
I 2
		vlist[i].x=r[0]*ua[i];
		vlist[i].y=r[1]*va[i];
E 2
	}

D 2
	pr_polygon_2(screen,0,0,nbnds,npts,vlist,PIX_SET,0,0);
E 2
I 2
	pr_polygon_2(screen,0,0,nbnds,npts,vlist,op,0,0);
E 2
}


pen_pldone()
{
	pr_close(screen);
I 2
}
 

pen_vpause(i)
int i;
{
	sleep(i);
}


pen_erase()
{
	unsigned short x[4],y[4];
/*
	x[0] = 0;	y[0] = 0;
	x[1] = AXSIZE;	y[1] = 0;
	x[2] = AXSIZE;	y[2] = AYSIZE;
	x[3] = 0;	y[3] = AYSIZE;
*/
	x[0] = 0;	y[0] = 0;
	x[1] = 10000;	y[1] = 0;
	x[2] = 10000;	y[2] = 10000;
	x[3] = 0;	y[3] = 10000;
	pen_area(4,x,y);
}


pen_col(col)
short int col;
{
	op=PIX_SRC|PIX_COLOR(col);
}


pen_fat(fat)
short int fat;
{
	/* NOT READY */
}


pen_text(size,oriented,string)
short int size,oriented;
char *string;
{
D 3
	/* NOT READY */
E 3
I 3
	struct pr_prpos where;
	struct pixfont *font;
	font=pf_open("/usr/lib/fonts/fixedwidthfonts/cour.r.24");
        where.pr=screen;
        where.pos.x=xx0;
        where.pos.y=yy0;
        pf_text(where,op,font,string);
        pf_close(font);
E 3
}

pen_Text(size,oriented,string)
short int size,oriented;
char *string;
{
D 3
	/* NOT READY */
E 3
I 3
	struct pr_prpos where;
	struct pixfont *font;
	struct pr_size pf_textwidth(),width;
	int length;
	length=strlen(string);
	font=pf_open("/usr/lib/fonts/fixedwidthfonts/cour.r.24");
	width=pf_textwidth(length,font,string);
        where.pr=screen;
	where.pos.x=xx0-(width.x/2);
        where.pos.y=yy0;
        pf_text(where,op,font,string);
        pf_close(font);
E 3
E 2
}
E 1
