/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include <math.h>

float zeronrb (int maxiter, float xeps, float x1, float x2, void *aux,
	void (*ydydx)(float x, void *aux, float *y, float *dydx), int *found)
/*****************************************************************************
Finds and returns a zero of y(x) via Newton-Raphson and bisection
******************************************************************************
Input:
maxiter		maximum number of iterations
xeps		smallest significant difference between x values
x1		an x value that, together with x2, brackets a zero of y(x)
x2		an x value that, together with x1, brackets a zero of y(x)   
aux		pointer to auxiliary parameters to be passed to ydydx
ydydx		pointer to function to evaluate y(x) and y'(x)
******************************************************************************
Output:
found		number of iterations required to find root; 0 if not found 
******************************************************************************
Input to the user-supplied function ydydx:
x		the independent variable
aux		pointer to auxiliary variables required by fdydx.
******************************************************************************
Output from the user-supplied function ydydx:
y		y(x)
dydx		y'(x)
******************************************************************************
Notes:
Adapted from function rtsafe in Numerical Recipes in C, 
by Press, Flannery, Teukolsky, and Vetterling, 1988.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 10/10/89
******************************************************************************/
{
	float y,dydx,xl,xh,xr,xrold,dx,dxold;
	
	/* orient search so that y(xl) < 0 */
	(*ydydx)(x1,aux,&y,&dydx);
	if (y<0.0) {
		xl = x1;
		xh = x2;
	} else {
		xl = x2;
		xh = x1;
	}
	
	/* initialize the root, the step before last, and the last step */
	xr = 0.5*(xl+xh);
	dxold = fabs(xh-xl);
	dx = dxold;
	
	/* initialize the function and its derivative */
	(*ydydx)(xr,aux,&y,&dydx);
	
	/* loop over iterations */
	for ((*found)=1; (*found)<=maxiter; (*found)++) {
	
		/* if Newton-Raphson is out of range or not converging */
		if ( ((xr-xh)*dydx-y)*((xr-xl)*dydx-y)>=0.0 ||
		     fabs(2.0*y)>fabs(dxold*dydx) ) {
			
			/* use bisection */
			dxold = dx;
			dx = 0.5*(xh-xl);
			xr = xl+dx;
			
			/* if change in root is insignificant, we're done */
			if (xl==xr) return xr;
		
		/* else, if Newton-Raphson step is acceptable */
		} else {
		
			/* take the step */
			dxold = dx;
			dx = y/dydx;
			xrold = xr;
			xr -= dx;
			
			/* if change in root is insignificant, we're done */
			if (xrold==xr) return xr;
			
		}
		
		/* if converged, we're done */
		if (fabs(dx)<xeps) return xr;
		
		/* evaluate function and derivative at root */
		(*ydydx)(xr,aux,&y,&dydx);
		
		/* keep root bracketed */
		if (y<0.0)
			xl = xr;
		else
			xh = xr;
	}
	
	/* failed to find root in maxiter iterations */
	*found = 0;
}	



/* simple test */

typedef struct _Parms {
	float a,b;
} Parms;

void ydydx (float x, Parms *parms, float *y, float *dydx)
{
	float a=parms->a,b=parms->b;
	*y = a*sin(x-b);
	*dydx = a*cos(x-b);
}

main()
{	
	int found;
	float xeps=0.001,a,b,x1,x2,xroot;
	Parms p;
	char s[256];
		
	while(1) {
		printf("Enter a b x1 x2\n");
		sscanf(gets(s),"%f %f %f %f",&p.a,&p.b,&x1,&x2);
		xroot = zeronrb(100,xeps,x1,x2,&p,
			(void(*)(float,void*,float*,float*))ydydx,&found);
		if (found)
			printf("After %d iterations, root is %g\n\n",
				found,xroot);
		else
			printf("Root not found!\n\n");
	}
}
