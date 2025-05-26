/* Copyright (c) Colorado School of Mines, 1990.*/
/* All rights reserved.                       */

/* SMOOTH2 --- smooths uniformly sampled 2D arrays of data */

#include "par.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"									",
" SMOOTH2 --- SMOOTH a uniformly sampled 2d array of data, within a user-",
"		defined window, via a damped least squares technique	",
"									",
" smooth2 < stdin n1= n2= [optional parameters ] > stdout		",
"									",
" Required Parameters:							",
" n1=			number of samples in the 1st (fast) dimension	",
" n2=			number of samples in the 2nd (slow) dimension	",
"									",
" Optional Parameters:							",
" r1=0			smoothing parameter in the 1 direction		",
" r2=0			smoothing parameter in the 2 direction		",
" win=0,n1,0,n2		array for window range				",
" rw=0			smoothing parameter for window function		",
" errfile=verror		name of file containing relative error(x1)",
"									",
" Notes:								",
" Larger r1 and r2 result in a smoother data. Recommended ranges of r1 	", 
" and r2 are from 1 to 20.						",
"									",
" The file verror gives the relative error between the original velocity ",
" and the smoothed one, as a function of depth. If the error is		",
" between 0.01 and 0.1, the smoothing parameters are suitable. Otherwise,",
" consider increasing or decreasing the smoothing parameter values.	",
"									",
" Smoothing can be implemented in a selected window. The range of 1st   ",
" dimension for window is from win[0] to win[1]; the range of 2nd   	",
" dimension is from win[2] to win[3]. 					",
"									",
" Smoothing the window function (i.e. blurring the edges of the window)	",
" may be done by setting a nonzero value for rw, otherwise the edges	",
" of the window will be sharp.						",
" 									",
NULL};
/**************** end self doc *******************************************/

/*
	Credits: 
		CWP: Zhen-yue Liu,
			adapted for par/main by John Stockwell 1 Oct 92
		Windowing feature added by Zliu on 16 Nov 1992
*/


void tripd(float *d, float *e, float *b, int n);

main(int argc, char **argv)

{
	int n1;		/* number of points in x1 (fast) dimension */
	int n2;		/* number of points in x1 (fast) dimension */
	int nmax;	/* max of n1 and n2 */
	int ix, iz;	/* counters */
	int *win;	/* 1d array defining the corners of smoothing window */
	float **v0;	/* array of input velocities */
	float **v;	/* array of output velocities */
	float **w;	/* intermediate array */
	float *errz;	/* array of error estimates as a function of x1 */
	float *d, *e;	/* input arrays for subroutine tripd */
	float *f;	/* intermediate array */
	float r1;	/* smoothing parameter for x1 direction */
	float r2;	/* smoothing parameter for x2 direction */
	float rw;	/* smoothing parameter for window */
	float err0;	/* error variable */
	float vrms;	/* rms velocity */
	FILE *infp=stdin;	/* input file pointer */
	FILE *outfp=stdout;	/* output file pointer */
	FILE *errorfp;		/* error file pointer */
	char *errfile="";	/* name of error file */
	
	/* hook up getpar */
	initargs(argc, argv);
	requestdoc(0);
	
	/* get parameters */
	if (!getparint("n1",&n1)) err("must specify n1!\n");
	if (!getparint("n2",&n2)) err("must specify n2!\n");
	if (!getparfloat("r1",&r1)) r1 = 0.;
	if (!getparfloat("r2",&r2)) r2 = 0.;
	if (!getparstring("errfile",&errfile)) errfile = "verror";
	
	/* scale the smoothing parameter */
	r1 = r1*r1*0.25;
	r2 = r2*r2*0.25;

	/* allocate space */
	nmax = (n1<n2)?n2:n1;
	win = alloc1int(4);
	v = alloc2float(n1,n2);
	v0 = alloc2float(n1,n2);
	w = alloc2float(n1,n2);
	errz = alloc1float(nmax);
	d = alloc1float(nmax);
	e = alloc1float(nmax);
	f = alloc1float(nmax);

	/* read velocities */
	fread(v[0],sizeof(float),n2*n1,infp);
 
	/* save the original velocity */
        for(ix=0; ix<n2; ++ix)
	 	for(iz=0; iz<n1; ++iz)
			v0[ix][iz]=v[ix][iz];

	/* get parameters for window function */
	if (!getparint("win",win)) {
		win[0] = 0;
		win[1] = n1;
		win[2] = 0;
		win[3] = n2;
		}
	if (!getparfloat("rw",&rw)) rw = 0.;
	rw = rw*rw*0.25;
 
	/* define the window function */
	for(ix=0; ix<n2; ++ix)
	 	for(iz=0; iz<n1; ++iz)
			w[ix][iz] = 0;	
	for(ix=win[2]; ix<win[3]; ++ix)
	 	for(iz=win[0]; iz<win[1]; ++iz)
			w[ix][iz] = 1;	

	if(win[0]>0 || win[1]<n1 || win[2]>0 || win[3]<n2){
	/*	smooth the window function */
         	for(iz=0; iz<n1; ++iz){
	 		for(ix=0; ix<n2; ++ix){
				d[ix] = 1.0+2.0*rw;
				e[ix] = -rw;
				f[ix] = w[ix][iz];
			}
        		d[0] -= rw;
         		d[n2-1] -= rw;
         		tripd(d,e,f,n2);
	 		for(ix=0; ix<n2; ++ix)
				w[ix][iz] = f[ix];
		}
         	for(ix=0; ix<n2; ++ix){
	 		for(iz=0; iz<n1; ++iz){
				d[iz] = 1.0+2.0*rw;
				e[iz] = -rw;
				f[iz] = w[ix][iz];
		}
        		d[0] -= rw;
         		d[n1-1] -= rw;
         		tripd(d,e,f,n1);
	 		for(iz=0; iz<n1; ++iz)
				w[ix][iz] = f[iz];
		}
	}

	/*      solving for the smoothing velocity */
        for(iz=0; iz<n1; ++iz){
	 	for(ix=0; ix<n2-1; ++ix){
			d[ix] = 1.0+r2*(w[ix][iz]+w[ix+1][iz]);
			e[ix] = -r2*w[ix+1][iz];
			f[ix] = v[ix][iz];
		}
        	d[0] -= r2*w[0][iz];
         	d[n2-1] = 1.0+r2*w[n2-1][iz];
		f[n2-1] = v[n2-1][iz];
         	tripd(d,e,f,n2);
	 	for(ix=0; ix<n2; ++ix)
			v[ix][iz] = f[ix];
	}
         for(ix=0; ix<n2; ++ix){
	 	for(iz=0; iz<n1-2; ++iz){
			d[iz] = 1.0+r1*(w[ix][iz+1]+w[ix][iz+2]);
			e[iz] = -r1*w[ix][iz+2];
			f[iz] = v[ix][iz+1];
		}
		f[0] += r1*w[ix][1]*v[ix][0];
         	d[n1-2] = 1.0+r1*w[ix][n1-1];
		f[n1-2] = v[ix][n1-1];
         	tripd(d,e,f,n1-1);
	 	for(iz=0; iz<n1-1; ++iz)
			v[ix][iz+1] = f[iz];
	}
	/* write smoothed data */
	fwrite(v[0],sizeof(float),n1*n2,outfp);

	/*	calculate the RMS error of velocity */
	errorfp = fopen(errfile,"w");
	err0 = 0.;
	vrms = 0;
        for(iz=0; iz<n1; ++iz){
	 	for(ix=0; ix<n2; ++ix){
		    err0 += (v0[ix][iz]-v[ix][iz])*(v0[ix][iz]-v[ix][iz]);
		    vrms += v0[ix][iz]*v0[ix][iz];
		}
	errz[iz] = sqrt(err0/vrms);
	}
	fwrite(errz,sizeof(float),n1,errorfp);
	fclose(errorfp);

}
        
	void tripd(float *d, float *e, float *b, int n)
/*****************************************************************************
Given an n-by-n symmetric, tridiagonal, positive definite matrix A and
 n-vector b, following algorithm overwrites b with the solution to Ax = b

  d() the diagonal of A 
  e() the superdiagonal of A
*****************************************************************************/
{
	int k; 
	float temp;
	
	/* decomposition */
	for(k=1; k<n; ++k){
           temp = e[k-1];
           e[k-1] = temp/d[k-1];
           d[k] -= temp*e[k-1];
	}

	/* substitution	*/
        for(k=1; k<n; ++k)  b[k] -= e[k-1]*b[k-1];
	
        b[n-1] /=d[n-1];
        for(k=n-1; k>0; --k)  b[k-1] = b[k-1]/d[k-1] - e[k-1]*b[k]; 
	
 }
