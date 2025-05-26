#include <par.h>

/* travel-time interpolation from rays to grids */
/* author:	zhiming li		2-8-93		       */
/*
void ray2grd(float *xray, float *zray, float *tray, int *npts, 
	int nray, int maxp,
	float sx, float sz, float treplace, 
	float x0, float z0, float dx, float dz, int nx, int nz,
	float *tgrid, float maxfac); 
input:
	xray(maxp,nray) 	---	x positions of rays
	zray(maxp,nray) 	---	z positions of rays
	tray(maxp,nray) 	---	travel times of rays
	npts(nray)		---	number of points per ray
	nray			---	number of rays
	maxp			---	maximum number of points per ray
	sx			---	source x location
	sz			---	source z location
	treplace		---	replacement time at zones outside
                                        the ray coverage 
	x0			---	minimum x of grids
	z0			---	minimum z of grids
	dx			---	x increment of grids
	dz			---	z increment of grids
	nx			---	number of x of grids
	nz			---	number of z of grids
	maxfac			---	maximum factor to determine whether
					a ray's arrival time at constant depth
					will be used. This is the maximum ratio
					allowed between |dt/dx| of a ray and
					the average |dt/dx| of all the rays
					at the current depth 
					(when maxfac=0., no checking)
	
output:
	tgrid(nz,nx)		---	travel times at grids
*/
	

void ray2grd(float *xray, float *zray, float *tray, int *npts, 
	int nray, int maxp,
	float sx, float sz, float treplace, 
	float x0, float z0, float dx, float dz, int nx, int nz,
	float *tgrid, float maxfac) { 

	float *xc, *tc, *xwork, *twork;
	int *index;

	float z1, z2, x1, x2, t1, t2;
	float deltaz, deltax, deltat;
	int iz, ix, ir, jp, nc, ic, jc; 
	int one=1, itmp, iw;
	float t, x, z, s, tmp;
	int jz;

	xc = (float*) malloc(maxp*nray*sizeof(float));
	tc = (float*) malloc(maxp*nray*sizeof(float));
	xwork = (float*) malloc(maxp*nray*sizeof(float));
	twork = (float*) malloc(maxp*nray*sizeof(float));
	index = (int*) malloc(maxp*nray*sizeof(int));

	jz = 0;

	for(iz=0;iz<nz;iz++) {
		z = z0 + iz*dz;
		/* find cross points of rays at z */ 
		nc = 0; 
		for(ir=0;ir<nray;ir++) {
			for(jp=1;jp<npts[ir];jp++) {
				z2=zray[ir*maxp+jp];
				z1=zray[ir*maxp+jp-1];
				x2=xray[ir*maxp+jp];
				x1=xray[ir*maxp+jp-1];
				t2=tray[ir*maxp+jp];
				t1=tray[ir*maxp+jp-1];
				if(z1==z2 && x1!=x2 && z==z1) {
					xc[nc] = x1; 
					tc[nc] = t1; 
					nc = nc + 1;
					xc[nc] = x2; 
					tc[nc] = t2; 
					nc = nc + 1;
				} else if(z==z1 ) {
					xc[nc] = x1; 
					tc[nc] = t1; 
					nc = nc + 1;
				} else if(z==z2 ) {
					xc[nc] = x2; 
					tc[nc] = t2; 
					nc = nc + 1;
				} else if(z>z1 && z<z2) {
					deltaz = z2 - z1;
					deltax = x2 - x1;
					deltat = t2 - t1;
					xc[nc] = x1+(z-z1)*deltax/deltaz;
					tc[nc] = t1+(z-z1)*deltat/deltaz;
					nc = nc + 1;
				}
			}
		}


		/* sort xc into ascending order */
		if(nc>1) {
			for(ic=0;ic<nc;ic++) {
				index[ic] = ic;
				xwork[ic] = xc[ic];
				twork[ic] = tc[ic];
			}
			qkisort(nc,xwork,index);
			/* delete data points at same xc */
			jc = 0;
			xc[0] = xwork[index[0]];
			tc[0] = twork[index[0]];
			for(ic=1;ic<nc;ic++) {
				iw = index[ic];
				if( xwork[iw]!=xc[jc] ) { 
					jc = jc + 1;
					xc[jc] = xwork[iw];
					tc[jc] = twork[iw];
				} else if( xwork[iw]==xc[jc] && 
				     	   twork[iw]<tc[jc] ) {
					xc[jc] = xwork[iw];
					tc[jc] = twork[iw];
				}
			}
			nc = jc + 1;
		}

		/* filtering out large changes of arrival times */
		if(maxfac>0. && nc>1 ) {
			for(ix=1;ix<nc;ix++) {
				twork[ix] = (tc[ix]-tc[ix-1])/(xc[ix]-xc[ix-1]);
			}
			twork[0] = twork[1];
			for(ix=0;ix<nc;ix++) {
				if(twork[ix]<0.) twork[ix] = 0.;
			}
			tmp = 0.;
			for(ix=0;ix<nc;ix++) {
				tmp = tmp + twork[ix];
			}
			tmp = tmp/nc;
			tmp = tmp * maxfac;
			for(ix=0;ix<nc;ix++) {
				if(twork[ix]>tmp) {
					index[ix] = 0;
				} else {
					index[ix] = 1;
				}
			}
			itmp = 0;
			for(ix=0;ix<nc;ix++) {
				if(index[ix]==1) {
					tc[itmp] = tc[ix];
					xc[itmp] = xc[ix];
					itmp  = itmp + 1;
				}
			}
			nc = itmp;
		} 

		/* now interpolate along x */
		if(nc>1) {
			for(ix=0;ix<nx;ix++) {
				x = x0 + ix*dx;
				if(x<xc[0]) {
					t = treplace;
				} else if (x>xc[nc-1]) {
					t = treplace;
				} else if (x==xc[nc-1]) {
					t = tc[nc-1];
				} else {
					lin1d_(xc,tc,&nc,&x,&t,&one,&itmp);
				}
				tgrid[nz*ix+iz] = t;
			}
			jz = jz + 1;
		} else if(z==sz && nc==1) {
			for(ix=0;ix<nx;ix++) {
				x = x0 + ix*dx;
				if(x==sx) {
					tgrid[nz*ix+iz] = 0.;
				} else {
					tgrid[nz*ix+iz] = treplace;
				}
					
			}
		} else {
			for(ix=0;ix<nx;ix++) {
				tgrid[nz*ix+iz] = treplace;
			}
		}
	}

	free(xc);
	free(tc);
	free(xwork);
	free(twork);
	free(index);
}
