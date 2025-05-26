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
