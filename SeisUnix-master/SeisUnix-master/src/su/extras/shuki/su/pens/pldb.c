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
unsigned short *ua,*va;
{
	short i;
	fprintf(stdout,"a %d\n",lp);
	for(i=0;i<lp;i++)
		fprintf(stdout,"\t%u\t%u\n",ua[i],va[i]);
}
