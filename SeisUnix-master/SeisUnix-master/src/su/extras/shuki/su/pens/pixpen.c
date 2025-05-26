/*
* pixpen
*/

#include <pixrect/pixrect_hs.h>
#include <stdio.h>
#include "pen.h" /*Device independed constsnts */

#define AXSIZE 1152.
#define AYSIZE 900.

#define NULLPR (struct pixrect *)NULL

extern int xargc;
extern char *xargv[];

static struct pixrect *screen;

static int xx0,yy0;   /* current position of the curser */
static float r[2];
unsigned char red[8],green[8],blue[8];
int op;

#define DEV "/dev/fb"

pen_init()
{
	int count;
	screen=pr_open(DEV);
	if(screen==NULL){
		fprintf(stderr,"%s (FATAL):pr_open returned %d\n",xargv[0],
                   screen);
		exit(-1);
	}
	r[0]=AXSIZE / 10000;
	r[1]=AYSIZE / 10000;

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
}

pen_move(xy)
unsigned short xy[2];
{
	xx0=r[0]*xy[0];
	yy0=r[1]*xy[1];
}
 

pen_draw(xy)
unsigned short xy[2];
{
	int xx,yy;
	xx=r[0]*xy[0];
	yy=r[1]*xy[1];
	pr_vector(screen,xx0,yy0,xx,yy,op,1);
	xx0=xx;
	yy0=yy;
}

pen_area(lp,ua,va)
short int lp;
unsigned short *ua,*va;
{
	struct pr_pos *vlist;
	int nbnds=1,npts[1],i;
	npts[0]=lp;

	vlist=(struct pr_pos *)malloc(lp*sizeof(struct pr_pos));
 
	for(i=0;i<lp;i++){
		vlist[i].x=r[0]*ua[i];
		vlist[i].y=r[1]*va[i];
	}

	pr_polygon_2(screen,0,0,nbnds,npts,vlist,op,0,0);
}


pen_pldone()
{
	pr_close(screen);
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
	struct pr_prpos where;
	struct pixfont *font;
	font=pf_open("/usr/lib/fonts/fixedwidthfonts/cour.r.24");
        where.pr=screen;
        where.pos.x=xx0;
        where.pos.y=yy0;
        pf_text(where,op,font,string);
        pf_close(font);
}

pen_Text(size,oriented,string)
short int size,oriented;
char *string;
{
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
}
