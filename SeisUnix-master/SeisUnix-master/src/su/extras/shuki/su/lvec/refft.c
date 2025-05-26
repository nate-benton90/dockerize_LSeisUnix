#include <math.h>
/*
 *	fft subroutine package
 *
 *	cefft (x,ne,sign)   complex element fft 
 *	refft (x,ne,sign,mode)
 *	incvfft (x,nv,ne,sign)	 in core complex vector fft
 *	inrvfft (x,nv,ne,sign,mode,buf)
 *	outcvfft (file,nv,ne,sign,verbose,buf,bufsize)	 out of core complex vector fft 
 *	outrvfft (file,nv,ne,sign,mode,verbose,buf,bufsize)
 *
 *	The first two ffts are between elements in a vector,
 *	the last four are between vectors in an array.
 *	The first two vector ffts are completely in core,
 *	the last two are partially out of core.
 *	There are three complex <=> complex ffts,
 *	and three real <=> complex ffts.
 *	The arguments are as follows:
 *		x	input and output space
 *		ne	number of elements in a vector,
 *			must be a power of two for the effts
 *		nv	number of vectors in an array, must be a power of two
 *		sign	postive for forward, negative for reverse,
 *			EE convention, sign is that of exponential argument
 *		mode	mode for real-complex ffts-
 *				1 forward, dont unpack,
 *				-1 reverse, dont pack,
 *				2 forward, unpack Nyquist,
 *				-2 reverse, pack Nyquist,
 *		verbose	 print pass completion messages for out of core ffts
 *		buf	buffer space, aligned on double boundary
 *			for inrvfft must be ne*4 bytes,
 *			for outcvfft, outrvfft as close as possible to
 *			ne*(nv+4)*4
 *		bufsize	 size of buffer in bytes
 */
/*
 * Keyword: fft fourier-transform subroutine
 */

refft (x,n,sign,mode)
register struct complex {float re, im;} *x;
int n, sign, mode;
/*
 *	radix 2 real <=> complex fast Fourier transform
 *	x is the in place vector, n is the power of two transform length
 *	mode:
 *		1	real to complex; real Nyquist part in imaginary DC
 *		-1	complex to real; real Nyquist part expected in imaginary DC
 *		2	real to complex; unpacked
 *		-2	complex to real; unpacked
 *	scaling for postive modes
 */
{
	register struct complex *xp, *yp;
	int n2;
	double cn, sn, cd, sd, arg;
	double aa, bb, ab, ba, real, imag;
	double sin(), atan2();

	n2 = n / 2;
	if (mode > 0) cefft (x,n2,sign);

	/* pack */
	/* rfft butterfly */
	if (mode == -2) x->im = x[n/2].re;

	sn = 0.0;
	cn = 1.0;
	arg = 2. * atan2 (1.,0.) / n;
	aa = sin (arg);
	cd = 2.0 * aa * aa;
	sd = sin (arg+arg);
	if (sign > 0) sd = -sd;
	aa = x->re;
	bb = x->im;
	if (sign > 0)
		{
		x->re = aa + bb;
		x->im = aa - bb;
		}
	else
		{
		x->re = (aa + bb) * .5;
		x->im = (bb - aa) * .5;
		}
	for (xp=x+1, yp=x+n/2-1; xp<=yp; xp++, yp--)
		{
		aa = cd * cn + sd * sn;
		sn += sd * cn - cd * sn;
		cn -= aa;
		aa = (xp->re + yp->re) * .5;
		ab = (xp->re - yp->re) * .5;
		ba = (xp->im + yp->im) * .5;
		bb = (xp->im - yp->im) * .5;
		real = cn * ba + sn * ab;
		imag = sn * ba - cn * ab;
		yp->im = imag - bb;
		xp->im = imag + bb;
		yp->re = aa - real;
		xp->re = aa + real;
		}

	if (0 > mode)
		{
		cefft (x,n2,sign);
		for (xp=x, yp=x+n/2; xp<yp; xp++) xp->im *= -1.;
		}

	/* unpack */
	if (mode == 2)
		{
		x[n/2].re = x[0].im;
		x[0].im = x[n/2].im = 0.;
		}
}
