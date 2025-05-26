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

cefft (x,n,sign)
register struct complex {float re, im;} *x;
int n,sign;
/*
 *	radix 2 complex <=> complex Fourier transform
 *	n is the number of complex points in x
 *	scaling on positive sign
 */
{
	register struct complex *xp, *yp;
	struct complex *end;
	double cn, sn, cd, sd, real, imag, scale, *psintab;
	float temp;
	int i, j, step;
	/* sines from pi/2 to pi/2097152 */
	static double sintab[] =
	{
		1.0000000000000000e-00,
		7.0710678118654747e-01,
		3.8268343236508974e-01,
		1.9509032201612825e-01,
		9.8017140329560596e-02,
		4.9067674327418010e-02,
		2.4541228522912286e-02,
		1.2271538285719925e-02,
		6.1358846491544749e-03,
		3.0679567629659760e-03,
		1.5339801862847655e-03,
		7.6699031874270447e-04,
		3.8349518757139556e-04,
		1.9174759731070329e-04,
		9.5873799095977337e-05,
		4.7936899603066881e-05,
		2.3968449808418217e-05,
		1.1984224905069705e-05,
		5.9921124526424274e-06,
		2.9960562263346605e-06
	
	};

	/* bit reverse address swapping */
	for (xp=x, end=x+n, i=0; xp<end; xp++, i+=j) {
		if (xp < (yp=x+i)) {
			temp = yp->re;
			yp->re = xp->re;
			xp->re = temp;
			temp = yp->im;
			yp->im = xp->im;
			xp->im = temp;
		}
		for (j=n>>1; j>=1 && i>=j;) {
			i -= j;
			j >>= 1;
		}
	}

	/* first butterfly */
	if (sign > 0) scale = 1. / n;
	for (xp=x, yp=x+1; xp<end; xp+=2, yp+=2) {
		if (sign > 0) {
			xp->re *= scale;
			xp->im *= scale;
			yp->re *= scale;
			yp->im *= scale;
		}
		temp = yp->re;
		yp->re = xp->re - temp;
		xp->re += temp;
		temp = yp->im;
		yp->im = xp->im - temp;
		xp->im += temp;
	}

	/* remaining butterflies */
	for (i=2, psintab=sintab; i<n; i=step) {
		step = i << 1;
		sd = *psintab++;
		temp = *psintab;
		cd = 2.0 * temp * temp;
		cn = 1.0;
		sn = 0.0;
		if (sign > 0) sd = -sd;
		for (j=0; j<i; j++) {
			for (xp=x+j; xp<end; xp+=step) {
				yp = xp + i;
				real = cn * yp->re - sn * yp->im;
				imag = sn * yp->re + cn * yp->im;
				yp->re = xp->re - real;
				yp->im = xp->im - imag;
				xp->re += real;
				xp->im += imag;
			}
			temp = cd * cn + sd * sn;
			sn += sd * cn - cd * sn;
			cn -=temp;
		}
	}
}
