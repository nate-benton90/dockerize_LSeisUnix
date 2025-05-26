/* CPLOTLIB - callable plotting subroutines.

-------------------------------------------------------------------------------
For absolute coordinates in inches:	For user coordinates in user units:
move (x,y)				umove (xu,yu)
draw (x,y)				udraw (xu,yu)
plot (x,y,down)				uplot (xu,yu,down)
text (x,y,size,orient,string)		utext (xu,yu,size,orient,string)
Text (x,y,size,orient,string)		uText (xu,yu,size,orient,string)
window (xmin,ymin,xmax,ymax)		uwindow (xumin,yumin,xumax,yumax)
area (xp,yp,lp)				uarea (xp,yp,lp)
penup()
pendn(x,y)				upendn(x,y)

Absolute coordinates (x,y) in inches are computed internally
from user coordinates (xu,yu) in user units via:
	x = x0+(xu-xu0)*xscl
	y = y0+(yu-yu0)*yscl

Other subroutines:			To specify user units:		
setfn (filename)			set0 (x0,y0)
setfp (filepntr)			setu0 (xu0,yu0)
setcol (color)				setscl (xscl,yscl)
setovl (ovl)
setfat (fat)
setdash (dash1,gap1,dash2,gap2)
set_hard_dash (line_style)
where (&x,&y)
getscls (&xscl,&yscl)
erase ()
endplot ()
purge_plot ()
----------------------------------------------------------------------------
PARAMETER	DESCRIPTION					TYPE
----------------------------------------------------------------------------
x,y		absolute x and y coordinates 			float
xu,yu		user x and y coordinates			float
down		0 or 1 for pen up or down (move or draw)	int
size		character size in 1/20 inch (0 <= size <= 31)	int
orient		character orientation = 0, 1, 2, or 3		int
string		NULL terminated character string 		char pointer
xmin,ymin	minimum absolute x and y coordinates to plot	float
xmax,ymax 	maximum absolute x and y coordinates to plot	float
xumin,yumin	minimum user x and y coordinates to plot	float
xumax,yumax 	maximum user x and y coordinates to plot	float
xp,yp		x and y coordinates of polygon vertices		float pointer
lp		length of xp and yp arrays			int
xmask,ymask 	1/xmask, 1/ymask = fraction of rasters on	int
x0,y0		origin in inches for user units			float
xu0,yu0		coord. in user units corresponding to x0,y0	float
xscl,yscl	inches per user units				float
filename	output filename (NULL terminated string)	char pointer
filepntr	output file pointer				FILE pointer
color		0 (black) to 7 (white)				int
ovl		0 (...)  1 (...)  or 2 (...)			int
fat		0 for no fattening, 1 for slightly fat, etc.	int
dash1,gap1	lengths in inches of first dash and gap 	float
dash2,gap2	lengths in inches of second dash and gap 	float
		(dashes turned off if dash1 < 0.)
----------------------------------------------------------------------------

*/

/**************************************************************/

#include <stdio.h>
#define RPERIN 1000		/* logical number of rasters per inch */
#define MAX 50			/* maximum x or y coordinate in inches */

#define puth(s,fd) fwrite(&s,2,1,fd)	/* ------>>>>>>> Assembly */

static struct ptbl {
	float _x0, _y0;		/* origin in inches */
	float _xu0, _yu0;	/* value in user units corresponding to x0,y0 */
	float _xscl, _yscl;	/* scaling from user units to inches */
	float _xold, _yold;	/* old pen position (in inches) */
	float _dpos;		/* position in dash sequence (in inches) */
	float _ddef[4];		/* definition of current dashes (in inches) */
	int _fat;
	int _dashon;		/* =0 if not dashed line; =1 if dashed line */
	int _pendown;		/* =1 if pen is down, 0 if it is up */
	FILE *_pltout;		/* output file */
} pc = {
	0.0, 0.0,
	0.0, 0.0,
	1.0, 1.0,
	0.0, 0.0,
	0.0,
	0.0, 0.0, 0.0, 0.0,
	0,
	0,
	0,
	stdout
};

/*
 * Macro color set
 */
#define BLACK   0
#define WHITE   1
#define RED     2
#define GREEN	3
#define BLUE	4
#define SKYBLUE 5
#define PURPLE  6
#define YELLOW	7
#define ORANGE	8

black()		{setcol(BLACK);}
white()		{setcol(WHITE);}
red()		{setcol(RED);}
green()		{setcol(GREEN);}
blue()		{setcol(BLUE);}
skyblue()	{setcol(SKYBLUE);}
purple()	{setcol(PURPLE);}
yellow()	{setcol(YELLOW);}
orange()	{setcol(ORANGE);}

/*************************************************************/

move_ (x,y)
float *x,*y;
{
	move(*x,*y);
}
move (x,y)
float x,y;
{
	plot (x,y,0);
}

draw_ (x,y)
float *x,*y;
{
	draw(*x,*y);
}
draw (x,y)
float x,y;
{
	plot (x,y,1);
}

penup_()
{
	penup();
}
penup()
{
	pc._pendown = 0;
}

pendn_(x,y)	/* go to location (x,y) and then put the pen down */
float *x,*y;
{
	pendn(*x,*y);
}
pendn(x,y)
float x,y;
{
	plot(x,y,pc._pendown);
	pc._pendown = 1;
}

plot_ (x,y,down)
float *x,*y;
int *down;
{
	plot(*x,*y,*down);
}
plot (x,y,down)
float x,y;
int down;
{
	float dx,dy,dist,dpos,xp,yp,tonext,cosine,sine;
	int i;
	double p_fmod(),sqrt();

	if (!down || !pc._dashon)		/* if move or no dashes */
	{
		p_pout(x,y,down); 	/* output a move or draw */
		pc._xold = x; pc._yold = y;	/* save old x and y */
		pc._dpos = 0.0;			/* reset position in dashes */
		return;
	}
	dx = x-pc._xold; dy = y-pc._yold;	/* change in x and y */
	dist = sqrt(dx*dx+dy*dy);		/* distance */
	if (dist <= 0.) return;			/* return if no change */
	cosine = dx/dist;  sine = dy/dist;
	dpos = pc._dpos;			/* current position in dashes */
	pc._dpos = p_fmod(dpos+dist,pc._ddef[3]); /* next position in dashes */
	for (i = 0; i < 4 && pc._ddef[i] <= dpos; i++); /* index to dash def */
	xp = pc._xold; yp = pc._yold;		/* initialize xp and yp */
	while (dist > 0.0)
	{
		tonext = pc._ddef[i]-dpos;	/* dist to next gap or dash */
		if (tonext > dist) tonext = dist;
		xp += tonext*cosine; yp += tonext*sine;
		p_pout(xp,yp,!(i%2));
		dpos = pc._ddef[i];		/* new position */
		i = (i+1)%4;			/* i = 0,1,2, or 3 */
		if (i == 0) dpos = 0.0;		/* back to start of dashes */
		dist -= tonext;
	}
	pc._xold = xp; pc._yold = yp;
}

vpause(nsec)
short nsec;
{
	putc ('s',pc._pltout);
	puth (nsec,pc._pltout);
}


text_(x,y,size,orient,string)
float *x,*y;
int *size,*orient;
char *string;
{
	text (*x,*y,*size,*orient,string);
}

text(x,y,size,orient,string)
float x,y;
int size,orient;
char *string;
{

	short sh_size,sh_orient;

	sh_size = size;
	sh_orient = orient;

	if (sh_size == 0) return;
	p_pout (x,y,0);
	putc ('t',pc._pltout);
	puth (sh_size,pc._pltout);
	puth (sh_orient,pc._pltout);
	do { putc (*string,pc._pltout); } while (*string++);
}

utext(x,y,size,orient,string)
float x,y;
int size,orient;
char *string;
{
	float xp,yp;
	xp = pc._x0+(x-pc._xu0)*pc._xscl;
	yp = pc._y0+(y-pc._yu0)*pc._yscl;
	text (xp,yp,size,orient,string);
}

Text_(x,y,size,orient,string)
float *x,*y;
int *size,*orient;
char *string;
{
	Text (*x,*y,*size,*orient,string);
}

Text(x,y,size,orient,string)
float x,y;
int size,orient;
char *string;
{
	short sh_size,sh_orient;

	sh_size = size;
	sh_orient = orient;

	if (sh_size == 0) return;
	p_pout (x,y,0);
	putc ('T',pc._pltout);
	puth (sh_size,pc._pltout);
	puth (sh_orient,pc._pltout);
	do { putc (*string,pc._pltout); } while (*string++);
}

uText(x,y,size,orient,string)
float x,y;
int size,orient;
char *string;
{
	float xp,yp;
	xp = pc._x0+(x-pc._xu0)*pc._xscl;
	yp = pc._y0+(y-pc._yu0)*pc._yscl;
	Text (xp,yp,size,orient,string);
}

window_(xmin,ymin,xmax,ymax)
float *xmin,*ymin,*xmax,*ymax;
{
	window (*xmin,*ymin,*xmax,*ymax);
}
window (xmin,ymin,xmax,ymax)
float xmin,ymin,xmax,ymax;
{
	short ix,iy;

	putc ('w',pc._pltout);
	ix = xmin*RPERIN; puth (ix,pc._pltout);
	iy = ymin*RPERIN; puth (iy,pc._pltout);
	ix = xmax*RPERIN; puth (ix,pc._pltout);
	iy = ymax*RPERIN; puth (iy,pc._pltout);
}

area_(xp,yp,lp)
float *xp,*yp;
int *lp;
{
	area (xp,yp,*lp);
}

area (xp,yp,lp)
float *xp,*yp;
int lp;
{
	int i;
	short ix,iy,sh_lp;

	sh_lp = lp;

	putc('a',pc._pltout);
	puth(sh_lp,pc._pltout);
	for(i = 0; i < lp; i++) {
		ix = (*xp++)*RPERIN;
		iy = (*yp++)*RPERIN;
		puth (ix,pc._pltout);
		puth (iy,pc._pltout);
	}
}

setfn_(filename)
char *filename;
{
	setfn(filename);
}
setfn (filename)
char *filename;
{
	FILE *fopen();

	if((pc._pltout=fopen(filename,"w")) == NULL) {
		fprintf(stderr,"Cplotlib: cannot create %s\n",filename);
		exit(-1);
	}
}

setfp_(filepntr)
int  *filepntr;
	{ extern FILE *fdopen(); setfp (fdopen(*filepntr,"w")); }
setfp (filepntr)
FILE *filepntr;
{
	pc._pltout = filepntr;
}

endplt_()
{
	endplot ();
}
endplot ()
{
	if (fclose(pc._pltout) == EOF)
	  {
		fprintf (stderr,"Cplotlib: cannot close plotfile\n");
		exit(-1);
	  }
}

purge_plot_()
{
	purge_plot ();
}
purge_plot ()
{
	putc ('p',pc._pltout);
	if(EOF == fflush (pc._pltout)) {
		fprintf(stderr,"unable to purge plot\n"); 
		exit(-1);
		}
}

set_hard_dash_(style)
int *style;
{
	set_hard_dash (*style);
}
set_hard_dash (style)
int style;
{
	short sty;
	sty = style;
	putc ('s',pc._pltout);
	puth (sty,pc._pltout);
}

setovl_(ovl)
int *ovl;
{
	setovl (*ovl);
}
setovl (ovl)
int ovl;
{
	short sovl;

	sovl = ovl;
	putc ('v',pc._pltout);
	puth (sovl,pc._pltout);
}

setcol_(color)
int *color;
{
	setcol (*color);
}
setcol (color)
int color;
{
	short col;

	col = color;
	putc ('c',pc._pltout);
	puth (col,pc._pltout);
}

setfat_(fat)
int *fat;
{
	setfat (*fat);
}
setfat (fat)
int fat;
{
	short fatness;

	fatness = fat;
	pc._fat = fatness;
	putc ('f',pc._pltout);
	puth (fatness,pc._pltout);
}

setdash_ (dash1,gap1,dash2,gap2)
float *dash1,*gap1,*dash2,*gap2;
{
	setdash (*dash1,*gap1,*dash2,*gap2);
}
setdash (dash1,gap1,dash2,gap2)
float dash1,gap1,dash2,gap2;
{
	if (dash1 < 0. || gap1 < 0. || dash2 < 0. || gap2 < 0.) {
		pc._dashon = 0;
		return;
	}
	pc._ddef[0] = dash1;
	pc._ddef[1] = pc._ddef[0]+gap1;
	pc._ddef[2] = pc._ddef[1]+dash2;
	pc._ddef[3] = pc._ddef[2]+gap2;
	if (pc._ddef[3] <= 0.) {
		pc._dashon = 0;
		return;
	}
	pc._dashon = 1;
	pc._dpos = 0.0;
	return;
}

erase_()
	{ erase (); }
erase ()
{
	putc ('e',pc._pltout);
}

set0_(x0,y0)
float *x0,*y0;
	{ set0 (*x0,*y0); }
set0 (x0,y0)
float x0,y0;
{
	pc._x0 = x0;
	pc._y0 = y0;
}

setu0_(xu0,yu0)
float *xu0,*yu0;
	{ setu0 (*xu0,*yu0);} 
setu0 (xu0,yu0)
float xu0,yu0;
{
	pc._xu0 = xu0;
	pc._yu0 = yu0;
}

setscl_(xscl,yscl)
float *xscl,*yscl;
	{ setscl (*xscl,*yscl); }
setscl (xscl,yscl)
float xscl,yscl;
{
	pc._xscl = xscl;
	pc._yscl = yscl;
}

umove_(x,y)
float *x,*y;
	{ umove (*x,*y); }
umove (x,y)
float x,y;
{
	uplot (x,y,0);
}

udraw_(x,y)
float *x,*y;
	{ udraw (*x,*y); }
udraw (x,y)
float x,y;
{
	uplot (x,y,1);
}

upendn_(x,y)	/* go to location (x,y) and then put the pen down */
float *x,*y;
	{ upendn(*x,*y); }
upendn(x,y)
float x,y;
	{
	uplot(x,y,pc._pendown);
	pc._pendown = 1;
	}

uplot_(x,y,down)
float *x,*y;
int *down;
	{ uplot (*x,*y,*down); }
uplot (x,y,down)
float x,y;
int down;
{
	float xp,yp;
	xp = pc._x0+(x-pc._xu0)*pc._xscl;
	yp = pc._y0+(y-pc._yu0)*pc._yscl;
	plot (xp,yp,down);
}

uwindo_(xmin,ymin,xmax,ymax)
float *xmin,*ymin,*xmax,*ymax;
	{ uwindow (*xmin,*ymin,*xmax,*ymax); }
uwindow (xmin,ymin,xmax,ymax)
float xmin,ymin,xmax,ymax;
{
	float xpmin,ypmin,xpmax,ypmax;
	xpmin = pc._x0+(xmin-pc._xu0)*pc._xscl;
	ypmin = pc._y0+(ymin-pc._yu0)*pc._yscl;
	xpmax = pc._x0+(xmax-pc._xu0)*pc._xscl;
	ypmax = pc._y0+(ymax-pc._yu0)*pc._yscl;
	window (xpmin,ypmin,xpmax,ypmax);
}

uarea_(xp,yp,lp)
float *xp,*yp; int *lp;
	{ uarea (xp,yp,*lp); }
uarea (xp,yp,lp)
float *xp,*yp; int lp;
{
	int i;
	short ix,iy,sh_lp;

	sh_lp = lp;

	putc ('a',pc._pltout);
	puth (sh_lp,pc._pltout);
	for(i = 0; i < lp; i++)
	{
		ix = (pc._x0+((*xp++)-pc._xu0)*pc._xscl)*RPERIN;
		iy = (pc._y0+((*yp++)-pc._yu0)*pc._yscl)*RPERIN;
		puth (ix,pc._pltout);
		puth (iy,pc._pltout);
	}
}

getscls_(xscl,yscl)
float *xscl,*yscl;
	{ getscls (xscl,yscl); }
getscls (xscl,yscl)
float *xscl,*yscl;
{
	*xscl = pc._xscl;
	*yscl = pc._yscl;
}

where_(x,y)
float *x,*y;
	{ where (x,y); }
where (x,y)
float *x,*y;
{
	*x = pc._xold;
	*y = pc._yold;
}

static p_pout (xp,yp,down)
float xp,yp;
int down;
{
	short ix,iy;

	xp = (xp > MAX) ? MAX : xp;
	xp = (xp < -MAX) ? -MAX : xp;
	yp = (yp > MAX) ? MAX : yp;
	yp = (yp < -MAX) ? -MAX : yp;
	ix= xp*RPERIN; iy= yp*RPERIN;
	putc ((down?'d':'m'),pc._pltout);
	puth (ix, pc._pltout); puth (iy,pc._pltout);
}

static double p_fmod(x,y)
float x,y;
{
	double floor();
	return(x-floor(x/y)*y);
}

#define UX(a) (pc._x0+(a-pc._xu0)*pc._xscl)*RPERIN
#define UY(a) (pc._y0+(a-pc._yu0)*pc._yscl)*RPERIN

uraster (n,fat,z,xstart,xend,v)
float *v,z,xstart,xend;
int n,fat;
{
	short s;
				putc('r',pc._pltout);
	s = n;			puth(s,pc._pltout);
	s = fat;		puth(s,pc._pltout);
	s = UX(z);		puth(s,pc._pltout);
	s = UY(xstart);		puth(s,pc._pltout);
	s = UY(xend);		puth(s,pc._pltout);
	while (n--) {
	   s = UX(*v++);	puth(s,pc._pltout);
	}
	setfat(pc._fat);
}
