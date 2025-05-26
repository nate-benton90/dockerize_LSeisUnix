/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include <math.h>
#include <stdio.h>

typedef struct _RKState {	/* Runga-Kutta state */
	int n;
	void *aux;
	void (*fdydx)(float x, void *aux, float y[], float dydx[]);
	float *w1,*w2,*w3,*w4,*w5,*w6;
} RKState;


RKState *rkcreate (int n, void *aux,
	void (*fdydx)(float x, void *aux, float y[], float dydx[]))
/*****************************************************************************
Creates and returns a pointer to the state of a Runga-Kutta stepper.
******************************************************************************
Input:
n		number of equations  
aux		pointer to auxiliary parameters to be passed to fdydx
fdydx		pointer to a function that calculates derivatives
******************************************************************************
Input to the user-supplied function fdydx:
x		the independent variable
aux		pointer to auxiliary variables required by fdydx.
y[]		array of dependent variables y(x)
******************************************************************************
Output from the user-supplied function fdydx:
dydx[]		array of derivatives y'(x)
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 10/10/89
******************************************************************************/
{
	RKState *rk;
	
	rk = (RKState*)malloc(sizeof(RKState));
	rk->n = n;
	rk->aux = aux;
	rk->fdydx = fdydx;
	rk->w1 = (float*)malloc(n*sizeof(float));
	rk->w2 = (float*)malloc(n*sizeof(float));
	rk->w3 = (float*)malloc(n*sizeof(float));
	rk->w4 = (float*)malloc(n*sizeof(float));
	rk->w5 = (float*)malloc(n*sizeof(float));
	rk->w6 = (float*)malloc(n*sizeof(float));
	
	return rk;
}


void rkdestroy (RKState *rk)
/*****************************************************************************
Destroys state of a Runga-Kutta stepper.
******************************************************************************
Input:
rk		pointer to Runga-Kutta state  
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 10/10/89
******************************************************************************/
{
	free(rk->w1);
	free(rk->w2);
	free(rk->w3);
	free(rk->w4);
	free(rk->w5);
	free(rk->w6);
	free(rk);
}


void rkstep (RKState *rk,
	float h, float x, float y[], float dydx[], float ynew[])
/*****************************************************************************
Performs one Runga-Kutta (4th order) step
******************************************************************************
Input:
rk		pointer to Runga-Kutta state
h		step size
x		independent variable x
y[]		array of dependent variables y(x)
dydx[]		array of derivatives y'(x)
******************************************************************************
Output:
ynew[]		array of y(x+h) (may be equivalenced to y[])
******************************************************************************
Notes:
Adapted from function rk4 in Numerical Recipes in C, 
by Press, Flannery, Teukolsky, and Vetterling, 1988.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 10/10/89
******************************************************************************/
{
	int i;
	float hhalf,hsixth,xhalf;
	int n=rk->n;
	float *dym=rk->w1,*dyt=rk->w2,*yt=rk->w3;
	void *aux=rk->aux;
	
	/* constants */
	hhalf = h/2.0;
	hsixth = h/6.0;
	xhalf = x+hhalf;

	/* 1st step */
	for (i=0; i<n; i++)
		yt[i] = y[i]+hhalf*dydx[i];
		
	/* 2nd step */
	rk->fdydx(xhalf,aux,yt,dyt);
	for (i=0; i<n; i++)
		yt[i] = y[i]+hhalf*dyt[i];
	
	/* 3rd step */
	rk->fdydx(xhalf,aux,yt,dym);
	for (i=0; i<n; i++) {
		yt[i] = y[i]+h*dym[i];
		dym[i] += dyt[i];
	}
	
	/* 4th step; accumulate weighted results from prior steps */
	rk->fdydx(x+h,aux,yt,dyt);
	for (i=0; i<n; i++)
		ynew[i] = y[i]+hsixth*(dydx[i]+dyt[i]+2.0*dym[i]);
}


#define PGROW -0.20		/* power for increasing step size */
#define PSHRINK -0.25		/* power for decreasing step size */
#define FCORR 0.066666666	/* fifth order correction factor = 1/15 */
#define SAFETY 0.9		/* factor for step size adjustment */
#define ERRCON 6.0e-4		/* error constant = (4/SAFETY)^(1/PGROW) */

void rkastep (RKState *rk, 
	float htry, float *hdid, float *hnext, float eps, float yscl[],
	float x, float y[], float dydx[], float ynew[])
/*****************************************************************************
Performs one Runga-Kutta (5th order) step, with a step size that is
adjusted to satisfy a user-specified error tolerance.
The step size is adjusted until either (1) the error in all y[i]
(i = 0 to n-1) is less than eps*yscl[i], where eps is an overall
error tolerance and yscl[i] is a user-supplied scale factor, or
(2) the step size becomes insignificant.
******************************************************************************
Input:
rk		pointer to Runga-Kutta state
htry		step size to try
eps		overall error tolerance
yscl		array of scale factors used to normalize errors
x		independent variable x
y[]		array of dependent variables y(x)
dydx[]		array of derivatives y'(x)
******************************************************************************
Output:
hdid		step size actually taken (0 if step unsuccessful)
hnext		step size recommended for next step (0 if step unsuccessful)
ynew[]		array of y(x+hdid) (may be equivalenced to y[])
******************************************************************************
Notes:
Adapted from function rkqc in Numerical Recipes in C, 
by Press, Flannery, Teukolsky, and Vetterling, 1988.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 10/10/89
******************************************************************************/
{
	int i;
	float h,hhalf,temp,errmax;
	int n=rk->n;
	float *yt=rk->w4,*dydxt=rk->w5,*ys=rk->w6;
	void *aux=rk->aux;
	
	/* save current y */
	for (i=0; i<n; i++)
		ys[i] = y[i];
	
	/* loop over trial step sizes (while trial step is significant) */
	for (h=htry,*hdid=0.0,*hnext=0.0; x+h!=x;) {
		
		/* take two half steps */
		hhalf = h/2.0;
		rkstep(rk,hhalf,x,ys,dydx,yt);
		rk->fdydx(x+hhalf,aux,yt,dydxt);
		rkstep(rk,hhalf,x+hhalf,yt,dydxt,ynew);
		
		/* take one large step */
		rkstep(rk,h,x,ys,dydx,yt);
		
		/* determine maximum error scaled by required accuracy */
		for (i=0,errmax=0.0; i<n; i++) {
			yt[i] = ynew[i]-yt[i];
			temp = fabs(yt[i]/yscl[i]);
			if (errmax<temp) errmax = temp;
		}
		errmax /= eps;
		
		/* if normalized maximum error is small enough */
		if (errmax<1.0) {
		
			/* compute size of next step */
			*hdid = h;
			*hnext = (errmax>ERRCON ?
				SAFETY*h*pow(errmax,PGROW) :
				4.0*h);

			/* compensate for 5th order truncation error */
			for (i=0; i<n; i++)
				ynew[i] += yt[i]*FCORR;
			
			/* we're done! */
			return;
		
		/* else if normalized maximum error is too big */
		} else {
		
			/* adjust step size for another try */
			h = SAFETY*h*pow(errmax,PSHRINK);
		}
	}
	
	/* step was unsuccessful, so set new y equal to input y */
	for (i=0; i<n; i++)
		ynew[i] = ys[i];
}



/* simple test */

typedef struct _Parms {
	float freq,phase;
} Parms;

void func (float x, Parms *parms, float y[])
{
	float freq=parms->freq,phase=parms->phase;
	y[0] = sin(freq*x+phase);
	y[1] = cos(freq*x+phase);
}
void dfunc (float x, Parms *parms, float y[], float dydx[])
{
	float freq=parms->freq,phase=parms->phase;
	dydx[0] = freq*cos(freq*x+phase);
	dydx[1] = -freq*sin(freq*x+phase);
}

#define N 2
#define NX 20
main()
{	
	int n=N,nx=NX,ix;
	float x,h=0.5,htry,hdid,hnext,eps=0.1;
	float y[N],ye[N],dydx[N],yscl[N];
	Parms p;
	RKState *rk;
	
	p.freq = 1.0;
	p.phase = 3.0;
	
	yscl[0] = 1.0;
	yscl[1] = 1.0;
	
	rk = rkcreate(n,&p,(void(*)(float,void*,float[],float[]))dfunc);	
	
	printf("\nEuler's method\n");
	func(0.0,&p,y);
	for (ix=0,x=0.0; ix<nx-1; ix++,x+=h) {
		func(x,&p,ye);
		printf("x = %8.3f  err[0] = %10.5f  err[1] = %10.5f\n",
			x,y[0]-ye[0],y[1]-ye[1]);
		dfunc(x,&p,y,dydx);
		y[0] += h*dydx[0];
		y[1] += h*dydx[1];
	}
	
	printf("\nRunga-Kutta with constant step size\n");
	func(0.0,&p,y);
	for (ix=0,x=0.0; ix<nx-1; ix++,x+=h) {
		func(x,&p,ye);
		printf("x = %8.3f  err[0] = %10.5f  err[1] = %10.5f\n",
			x,y[0]-ye[0],y[1]-ye[1]);
		dfunc(x,&p,y,dydx);
		rkstep(rk,h,x,y,dydx,y);
	}
	
	printf("\nRunga-Kutta with adaptive step size\n");
	func(0.0,&p,y);
	for (ix=0,x=0.0,htry=h; ix<nx-1; ix++,x+=hdid,htry=hnext) {
		func(x,&p,ye);
		printf("x = %8.3f  err[0] = %10.5f  err[1] = %10.5f\n",
			x,y[0]-ye[0],y[1]-ye[1]);
		dfunc(x,&p,y,dydx);
		rkastep(rk,htry,&hdid,&hnext,eps,yscl,x,y,dydx,y);
		if (hdid==0.0) break;
	}
}


