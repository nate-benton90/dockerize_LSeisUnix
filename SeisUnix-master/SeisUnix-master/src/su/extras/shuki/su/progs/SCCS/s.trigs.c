h62055
s 00000/00000/00040
d D 1.3 88/11/15 14:03:05 shuki 3 2
c 
e
s 00000/00000/00040
d D 1.2 88/05/25 14:54:08 shemer 2 1
c with SccsId[]
e
s 00040/00000/00000
d D 1.1 88/04/14 13:52:49 shuki 1 0
c date and time created 88/04/14 13:52:49 by shuki
e
u
U
f e 0
t
T
I 1
/******* TRIG TABLE ROUTINES ********/

#include <stdio.h>
#include <math.h>
#define pi 3.14159265
typedef struct {float re,im;} complex;

static int trigodx;
static float *sintbl,*costbl;

mktrigtbl(n)
int n;
{
	int i;
	float arg,dx;

	sintbl = (float*) malloc(n*4);
	costbl = (float*) malloc(n*4);

	dx = 1.0/n;
	trigodx = 1.0/dx;
	dx *= 2.0*pi;
	for(i=0;i<n;i++) {
		arg = dx*i;
		sintbl[i] = sin(arg);
		costbl[i] = cos(arg);
	}
}

sinecos(arg,s,c)
float arg,*s,*c;
{
	int i;
/* 	*s = sin(2.0*pi*arg); */
/* 	*c = cos(2.0*pi*arg); */
	arg -= floor(arg);
	i = trigodx*arg + 0.499999;
	*s = sintbl[i];
	*c = costbl[i];
}
E 1
