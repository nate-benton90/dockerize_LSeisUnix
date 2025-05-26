h18764
s 00000/00000/00063
d D 1.2 88/11/15 14:07:33 shuki 2 1
c 
e
s 00063/00000/00000
d D 1.1 88/04/14 13:59:30 shuki 1 0
c date and time created 88/04/14 13:59:30 by shuki
e
u
U
f e 0
t
T
I 1
#include <stdio.h>

/*
 * tpen - vplot command interpreter for tektronix
 *
 * By Brian Sumner
 */

pen_erase() {}

pen_pldone()
{
	fflush(stdout);
	pldone();
}

pen_init()
{
	plinit();
	pwind(0.,9.6-0.015,0.,7.2-0.015);
	uwind(0.,9.6-0.015,0.,7.2-0.015);
	csize(0.2);
}
pen_move(xy)
unsigned short xy[2];
{
	pmove((float)xy[0]/1000.0, (float)xy[1]/1000.0);
}
pen_draw(xy)
unsigned short xy[2];
{
	pdraw((float) xy[0]/1000.0, (float)xy[1]/1000.0);
}
pen_col(col)
short int col;
{
	setpen((int) col);
}
pen_fat(fat)
short int fat;
{
	/* NOT READY */
}
pen_Text(size,orient,string)
short int size,orient;
char *string;
{
	csize(0.02*(float)size);
	center(string,(float)orient,strlen(string));
}
pen_text(size,orient,string)
short int size,orient;
char *string;
{
	csize(0.02*(float)size);
	text(string,(float)orient,strlen(string));
}
pen_area(lp,ua,va)
short lp;
float *ua,*va;
{
	fill(ua,va,(int) lp);
}
E 1
