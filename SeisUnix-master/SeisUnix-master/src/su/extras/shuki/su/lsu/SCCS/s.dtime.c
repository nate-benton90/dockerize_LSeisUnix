h47128
s 00000/00000/00035
d D 1.2 88/11/15 14:02:01 shuki 2 1
c 
e
s 00035/00000/00000
d D 1.1 88/10/26 11:06:49 shuki 1 0
c date and time created 88/10/26 11:06:49 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * %W% %G%
 */

/* #include <stdio.h> */

#include <sys/types.h>
#include <sys/timeb.h>
#define MILI  0.001

float dtime()
{
	struct timeb tb;
	static struct timeb lasttb;
	static enum {false,true} first=true;
	float dt;
	int idt;

	ftime(&tb);

	if(first==true) {
		first = false;
		lasttb.time = 0;
		lasttb.millitm = 0;
	}

	idt = tb.time - lasttb.time;
	dt = idt;
	idt = tb.millitm - lasttb.millitm;
	dt += MILI*idt;

	ftime(&lasttb);

	return(dt);
}
E 1
