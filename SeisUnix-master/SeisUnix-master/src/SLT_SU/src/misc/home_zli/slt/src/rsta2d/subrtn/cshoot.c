#include "cshoot.h"
#include "par.h"

/* common shot ray shooting of direct waves */ 
/* author:	zhiming li	2-9-93	      	*/
/*
void cshoot(float *xint, float *zint, float *vint, int maxspl, 
	int nint, int *nspls, 
	double *a0, double *a1, double *a2, double *a3, 
	float *sign, int *norder, int flag,   
	float xs, float angmin, float dang, int nang, float *v, 
	float xstart, float xend, float dx,
	float *xray, float *zray, float *tray, int *npts);
input:
	xint(nint,maxspl) 	---	x positions of interfaces
	zint(nint,maxspl) 	---	z positions of interfaces
	vint(nint,maxspl) 	---	velocity at positions of interfaces
	maxspl			---	maximum number of points per interface
	nint			---	number of interfaces (including the
					surface)
	nspls(nint)		---	number of points per interface
	a0(nint,maxspl)		---	spline coefficients of interfaces 
	a1(nint,maxspl)		---	spline coefficients of interfaces 
	a2(nint,maxspl)		---	spline coefficients of interfaces 
	a3(nint,maxspl)		---	spline coefficients of interfaces 
	sign(nint)		---	sign of ray directions at interfaces
					(-1. --- down;  1 --- up)
	norder(nint)		---	order of ray intersections with
					interfaces
	flag			---	whether a0,a1,a2,a3,sign,norder should
					computed (0=no 1=yes)	
	xs			---	surface source x location
	angmin			---	minimum angle (in degrees) of rays
					(measured from vertical ranged from
					-90 to +90)
	dang			---	angle increment (in degrees) of rays
	nang			---	number of angles to shoot
	v(nint)			---	velocities below interfaces
	xstart			---	minimum x position of model	
	xend			---	maximum x position of model	
	dx 			---	lateral trace spacing
output:
	xray(nint,nang)		---	x positions of rays 	
	zray(nint,nang)		---	z positions of rays 	
	tray(nint,nang)		---	travel times of rays 	
	npts(nang)		---	number of points per ray
					when npts=0 --- ray tracing failed at 
					this angle 
*/
void cshoot(float *xint, float *zint, float *vint, int maxspl, 
	int nint, int *nspls, 
	double *a0, double *a1, double *a2, double *a3, 
	float *sign, int *norder, int flag,   
	float xs, float angmin, float dang, int nang, float *v, 
	float xstart, float xend, float dx, 
	float *xray, float *zray, float *tray, int *npts) {


	double *c, *d2, *e, *b, *cv; 
	float *x, *z, *d;
	double *dz,*ddz,*deltax,*deltaz;
	float tmp, pi;
	double dtmp;
	int spfail;
	int icross, maxint, maxn, maxnp1;
	float recdpt, ang;

	int nl, mxspm1;
	int ia, i, j,  n;


	nl = nint - 1;
	mxspm1 = maxspl - 1;
	maxint = nl;
	maxn = nint;
	maxnp1 = maxn + 1;

	/* compute spline coefficients, sign, norder */  
	if(flag==1) {
		/* spline coefficients */
		c = (double*) malloc(maxspl*sizeof(double));
		d2 = (double*) malloc(maxspl*sizeof(double));
		e = (double*) malloc(maxspl*sizeof(double));
		b = (double*) malloc(maxspl*sizeof(double));
		cv = (double*) malloc(maxspl*sizeof(double));
		bzero(a0, maxspl*nint*sizeof(double));
		bzero(a1, maxspl*nint*sizeof(double));
		bzero(a2, maxspl*nint*sizeof(double));
		bzero(a3, maxspl*nint*sizeof(double));
		cuspln_(&nl,xint,zint,nspls,a0,a1,a2,a3,&spfail,
			&nl,&mxspm1,c,d2,e,b,cv);
		free(c);
		free(d2);
		free(e);
		free(b);
		free(cv);
		for(i=0;i<nint;i++) {
			sign[i] = -1.0;
			norder[i] = i+1;
		}
	}

	/* array allocations */
	x = (float*) malloc(maxnp1*sizeof(float));
	z = (float*) malloc(maxnp1*sizeof(float));
	dz = (double*) malloc(maxnp1*sizeof(double));
	ddz = (double*) malloc(maxnp1*sizeof(double));
	deltax = (double*) malloc(maxnp1*sizeof(double));
	deltaz = (double*) malloc(maxnp1*sizeof(double));
	d = (float*) malloc(maxnp1*sizeof(float));

	/* shoot rays */
	pi = 3.141592654;
	recdpt = 0.;

	bzero(npts,nang*sizeof(int));

	for(ia=0;ia<nang;ia++) {
		ang = angmin + ia * dang;
		
		x[0] = xs;
		z[0] = 0.;
		if(abs(ang+90.)<=0.001) {
			x[1] = xstart;
			z[1] = 0.;
			n = 2;
		} else if(abs(ang-90.)<=0.001) {
			x[1] = xend;
			z[1] = 0.;
			n = 2;
		} else { 
			n = nint - 1;

			tshoot_(x,z,&icross,&ang,&pi,&dx,&xstart,&xend,&recdpt,
				xint,zint,a0,a1,a2,a3,sign,nspls,&nl,norder,
				dz,ddz,deltax,deltaz,d,v,&n,
				&maxint,&maxspl,&maxn,&maxnp1,&mxspm1,vint);

			n = icross;
		}

		/*
		if(n<=1) {
			if(ang<0.) {
				x[1] = xstart; 
				z[1] = (x[1]-x[0])/tan(ang*pi/180.); 
				n = 2;
			} else if (ang>0.) {
				x[1] = xend;
				z[1] = (x[1]-x[0])/tan(ang*pi/180.); 
				n = 2;
			}
		}
		*/

		/* compute travel times */
		xray[ia*nint] = x[0];
		zray[ia*nint] = z[0];
		tray[ia*nint] = 0.;

		for(i=1;i<n;i++) {
			xray[ia*nint+i] = x[i];
			zray[ia*nint+i] = z[i];
			tmp = (x[i]-x[i-1])*(x[i]-x[i-1]) + 
			      (z[i]-z[i-1])*(z[i]-z[i-1]);
			dtmp = tmp;
			dtmp = sqrt(dtmp);
			tmp = dtmp;
			tray[ia*nint+i] = tray[ia*nint+i-1]+tmp/v[i-1]; 
		}
		npts[ia] = n;
	}
	
	free(x);
	free(z);
	free(dz);
	free(ddz);
	free(deltax);
	free(deltaz);
	free(d);
}
