h53106
s 00000/00000/00085
d D 1.2 88/11/15 14:07:28 shuki 2 1
c 
e
s 00085/00000/00000
d D 1.1 88/04/14 13:59:28 shuki 1 0
c date and time created 88/04/14 13:59:28 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * gpen - vplot command interpreter for GIGI
 *
 * By Shuki Ronen
 */

#include <stdio.h>
#include <math.h>
#define RPERIN 1000
#define GRPERIN 76
#define GYSIZE 479
#define GXSIZE 767
static int drawing;

pen_pldone()
{
	fprintf(stdout,"\\");
}

pen_init()
{
	fprintf(stdout,"Pp;\ns(e);\n");
}
pen_move(xy)
unsigned short xy[2];
{
	unsigned short x,y;
	x = xy[0]*GRPERIN/RPERIN;
	if(x>GXSIZE) x = GXSIZE;
	y = GYSIZE-xy[1]*GRPERIN/RPERIN;
	if(y>GYSIZE) y = GYSIZE;
	fprintf(stdout,"p[%d,%d];\n",x,y);
	pwait();
}
pen_draw(xy)
unsigned short xy[2];
{
	unsigned short x,y;
	x = xy[0]*GRPERIN/RPERIN;
	if(x>GXSIZE) x = GXSIZE;
	y = GYSIZE-xy[1]*GRPERIN/RPERIN;
	if(y>GYSIZE) y = GYSIZE;
	fprintf(stdout,"v[%d,%d];\n",x,y);
	pwait();
/* 	sleep(1); */
}
pwait()
{
	int n=1000;
	float x=1000.;
	while(n--) x = sqrt(x);
}
pen_col(col)
short int col;
{
	fprintf(stdout,"W(%d);\n",col);
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
	fprintf(stdout,"T(D%d)(S%d)(H%d)(D%d)'%s';\n",
		orient,size/4,size/4,orient,string);
	pwait();
}
pen_text(size,orient,string)
short int size,orient;
char *string;
{
	fprintf(stdout,"T(D%d)(S%d)(H%d)(D%d)'%s';\n",
		orient,size/4,size/4,orient,string);
	pwait();
}

pen_area(lp,ua,va)
short lp;
float *ua,*va;
{
/* 	fprintf(stdout,"p[%d,%d];\n",ua[0],va[0]); */
}
E 1
