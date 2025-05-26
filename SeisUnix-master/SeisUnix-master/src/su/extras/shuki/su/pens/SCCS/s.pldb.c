h29936
s 00000/00000/00066
d D 1.3 88/11/15 14:07:32 shuki 3 2
c 
e
s 00002/00002/00064
d D 1.2 88/08/17 10:20:08 rafi 2 1
c area text color
e
s 00066/00000/00000
d D 1.1 88/04/14 13:59:30 shuki 1 0
c date and time created 88/04/14 13:59:30 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * pldb - vplot debugger
 */

#include <stdio.h>
#include <math.h>

pen_erase() {}

pen_pldone() { }

pen_init() { }

pen_vpause(i)
int i;
{
	fprintf(stdout,"s %d \n",i);
}

pen_move(xy)
unsigned short xy[2];
{
	fprintf(stdout,"m %d %d\n",xy[0],xy[1]);
}

pen_draw(xy)
unsigned short xy[2];
{
	fprintf(stdout,"d %d %d\n",xy[0],xy[1]);
}

pen_col(col)
short int col;
{
	fprintf(stdout,"c %d\n",col);
}

pen_fat(fat)
short int fat;
{
	fprintf(stdout,"f %d\n",fat);
}

pen_Text(size,orient,string)
short int size,orient;
char *string;
{
	fprintf(stdout,"T %d %d %s\n",size,orient,string);
}

pen_text(size,orient,string)
short int size,orient;
char *string;
{
	fprintf(stdout,"t %d %d %s\n",size,orient,string);
}

pen_area(lp,ua,va)
short lp;
D 2
float *ua,*va;
E 2
I 2
unsigned short *ua,*va;
E 2
{
	short i;
	fprintf(stdout,"a %d\n",lp);
	for(i=0;i<lp;i++)
D 2
		fprintf(stdout,"\t%f\t%f\n",ua[i],va[i]);
E 2
I 2
		fprintf(stdout,"\t%u\t%u\n",ua[i],va[i]);
E 2
}
E 1
