/* Four fft subroutines based on the algorithm used in fork (FGDP, p. 12) 
 * and modeled after Clayton's and Ottolini's subroutines
 *********************************************************************
 *								     *
 *	       lx-1					     *
 *	x(k) = sum  [x(j)*exp(isign*2*pi*sqrt(-1)*j*k/lx]    *
 *	       j=0					     *
 *								     *
 *********************************************************************
 *
 *		refft (x,lx,isign,mode)
 *		cefft (x,lx,isign)
 *		rvfft (x,lx,nx,isign,mode)
 *		cvfft (x,lx,nx,isign)
 *
 * See each subroutine below for a description of the arguments.
 */
/* #include <seplib.h> */
typedef struct {float re,im;} complex;
double pi=3.1415926535898;

/* 	rvfft	nx real vectors to nx/2 (or nx/2+1) complex vectors in core
 *
 *	x	input/output array
 *	lx	length of real vectors
 *	nx	number of vectors; must be a power of 2
 *	isign	sign of sqrt(-1)
 *	mode	 1 for nx real to nx/2 complex
 *		 2 for nx real to nx/2+1 complex
 *		-1 for nx/2 complex to nx real
 *		-2 for nx/2+1 complex to nx real
 */
rvfft (x,lx,nx,isign,mode)
/*register*/ complex *x;
int lx,nx,isign,mode;
{
	/*register*/ complex *xp,*xn,*xpplx;
	int i,j,k,ix,lx2m1;
	float real,imag,xsumre,xsumim,xdifre,xdifim,*xf,*xfplx;
	double aa,cn,sn,cd,sd,arg,sin();

	if(mode > 0) 				/* real to complex */
	  {
		for (ix=0; ix<nx; ix+=2)	/* weave (interleave) adjacent */
		  {				/* real vectors */
			for (i=1, lx2m1=lx*2-1, xf=(float*)x+ix*lx; i<lx2m1; i++)
			  {
				for (k=i>>1, j=k+lx*(i-(k<<1)); j<i; 
					k=j>>1, j=k+lx*(j-(k<<1)));
				real = xf[i]; xf[i] = xf[j]; xf[j] = real;
			  }
		  } 
		cvfft (x,lx,nx/2,isign);
		for (xp=x, xpplx=xp+lx, xn=x+nx/2*lx; xp<xpplx; xp++, xn++)
		  {
			real = xp->re+xp->im; imag = xp->re-xp->im;
			xp->re = real;
			if (mode == 1)		/* pack Nyquist */
				xp->im = imag;
			else			/* unpack Nyquist */
			  {
				xn->re = imag;
				xn->im = xp->im = 0.;
			  }
		  }
	  }
	cn = 1.; sn = 0.;
	arg = pi/nx;
	if (isign < 0) arg = -arg;
	aa = sin(arg);
	cd = 2.*aa*aa; sd = sin(arg+arg);
	for (ix=1, xp=x+lx; ix<=nx/4; ix++)
  	  {
		aa = cd*cn+sd*sn;
		sn += sd*cn-cd*sn;
		cn -= aa;
		for (xn=x+(nx/2-ix)*lx, xpplx=xp+lx; xp<xpplx; xp++, xn++)
		  {
			xsumre = 0.5*(xp->re+xn->re);
			xsumim = 0.5*(xp->im-xn->im);
			xdifre	= 0.5*(xp->re-xn->re);
			xdifim = 0.5*(xp->im+xn->im);
			real = sn*xdifre+cn*xdifim;
			imag = sn*xdifim-cn*xdifre;
			xp->re = xsumre+real;
			xp->im = imag+xsumim;
			xn->re = xsumre-real;
			xn->im = imag-xsumim;
	  	  }
	  }
	if(mode < 0) 				/* complex to real */
	  {
		if (mode == -2)			/* Nyquist not packed, */
		  {				/* so pack it */
			for (xp=x, xn=x+nx/2*lx, xpplx=xp+lx; xp<xpplx; xp++, xn++)
			  {
				xp->im = xn->re;	
				xn->re = 0.;
			  }
		  }
		for (xp=x, xpplx=xp+lx; xp<xpplx; xp++)
		  {
			real = 0.5*(xp->re+xp->im);
			xp->im = 0.5*(xp->im-xp->re);
			xp->re = real;
		  }
		cvfft (x,lx,nx/2,isign);
		for (ix=0; ix<nx; ix+=2)	/* unweave adjacent */
		  {				/* real vectors */
			xf=(float*)x+ix*lx;
			for (i=1, lx2m1=lx*2-1; i<lx2m1; i++)
			  {
				for (k=i/lx, j=k+((i-k*lx)<<1); j<i; 
					k=j/lx, j=k+((j-k*lx)<<1));
				real = xf[i]; xf[i] = xf[j]; xf[j] = real;
			  }
			for (xf+=lx, xfplx=xf+lx; xf<xfplx; xf++)
				*xf = -*xf;
		  } 
	   }
}
