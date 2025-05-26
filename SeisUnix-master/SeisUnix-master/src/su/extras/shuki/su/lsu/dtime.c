/*
 * @(#)dtime.c	1.2 11/15/88
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
