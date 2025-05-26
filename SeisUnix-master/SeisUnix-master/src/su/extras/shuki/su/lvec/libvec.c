/* vecpkge - a package of vector operations
 *
 *	x,y,z	vectors
 *	a	scalar
 *	n	integer vector length
 *
 * For floats:
 * void vsadd, vsmul(&x, a, n)				x <- x + a
 * void vsadd2, vsmul2(&x, &y, a, n)			x <- y + a
 * void vadd, vsub, vmul, vdiv(&x, &y, n)		x <- x + y
 * void vadd2, vsub2, vmul2, vdiv2(&x, &y, &z, n)	x <- y + z
 * void vsqrt(), vabs(&x)				x <- abs(x)
 * void cvabs(&x, &z, n) [z is complex]			x <- cabs(z)
 * float vmax, vmin, vabsmax(&x, n); returns max, min, max(|max|, |min|)
 * float vsum, vprod(&x, n); returns sum, prod
 * float vl1, vl2(&x, n); returns sum of abs values, sums of squares
 *
 * For doubles:
 * dvsadd, etc.
 *
 * For ints:
 * ivsadd, etc.
 *
 * Credits:
 *	CWP: Shuki, Jack
 *
 * Caveat:
 *	FHUGE, DHUGE, IHUGE picked off the wall for now.  If math.h does
 *	something reasonable, we should accept it, if not, we should
 *	think it through.
 *
 *	A vector length that is zero or negative, causes an immediate
 *	return with no action performed.  Perhaps negative lengths
 *	should be a fatal error.
 *
 *	A case could be made for n to be declared as long, likewise
 *	imax, imin, iabsmax.
 *
 * Note:
 *	The attractive form:
 *		while (n--) operation;
 *	goes into an infinite loop if n (erroneously)is set <  0.
 *	The form:
 *	do operation; while(--n);
 *	is worse, failing also for n = 0.
 *
*/


/* #include "../include/cwp.h" */

#define SGN(x)		( (x) < 0 ? -1.0 : 1.0)
#define ABS(x)		( (x) < 0 ? -(x) : (x) )
#define	MAX(x,y)	( (x) > (y) ? (x) : (y) )
#define	MIN(x,y)	( (x) < (y) ? (x) : (y) )

typedef struct {float re, im;} complex;

#define	FHUGE	(float) 1.0e36
#define	DHUGE	(double) 1.0e36
#define	IHUGE	(int) 1000000000


void vsadd(x, a, n)
register float *x, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ += a;
}


void vsmul(x, a, n)
register float *x, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ *= a;
}


void vsadd2(x, y, a, n)
register float *x, *y, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ = *y++ + a;
}


void vsmul2(x, y, a, n)
register float *x, *y, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ = *y++ * a;
}


void vadd(x, y, n)
register float *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ += *y++;
}


void vsub(x, y, n)
register float *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ -= *y++;
}


void vmul(x, y, n)
register float *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ *= *y++;
}


void vdiv(x, y, n)
register float *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		if (*y != 0)
			*x++ /= *y++;
		else
			err(__FILE__,__LINE__,"vdiv: divide by zero");
}


void vadd2(x, y, z, n)
register float *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y + *z;
		y++; z++;
	}
}


void vsub2(x, y, z, n)
register float *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y - *z;
		y++; z++;
	}
}


void vmul2(x, y, z, n)
register float *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y * *z;
		y++; z++;
	}
}


void vdiv2(x, y, z, n)
register float *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--)
		if (*z != 0)
			*x++ = *y / *z;
		else
			err(__FILE__,__LINE__,"vdiv2: divide by zero");
		
		y++; z++;
}


void vsqrt(x, n)
register float *x;
register int n;
{
	for ( ; n > 0; n--,x++) {
		*x = sqrt(*x);
	}
}


void vabs(x, n)
register float *x;
register int n;
{
	for ( ; n > 0; n--,x++)
		*x = ABS(*x);
}


void cvabs(x, z, n)
register float *x;
register complex *z;
register int n;
{
	register i;

	for (i = 0; i < n; i++) {
		x[i] = z[i].re*z[i].re + z[i].im*z[i].im;
	}
	vsqrt(x, n);
}


float vmax(x, n)
register float *x;
register int n;
{
	register float m = -FHUGE;

	for ( ; n > 0; n--) {
		m = MAX(*x, m);
		x++;
	}
	return(m);
}


float vmin(x, n)
register float *x;
register int n;
{
	register float m = FHUGE;

	for ( ; n > 0; n--) {
		m = MIN(*x, m);
		x++;
	}
	return(m);
}



float vabsmax(x, n)
register float *x;
register int n;
{
	register float m = 0.0;
	register float absp;

	for ( ; n > 0; n--) {
		absp = ABS(*x);
		m = MAX(absp, m);
		x++;
	}
	return(m);
}


float vsum(x, n)
register float *x;
register int n;
{
	register float sum;

	sum = 0.0;
	for ( ; n> 0; n--)
		sum += *x++;
	return(sum);
}


float vprod(x, n)
register float *x;
register int n;
{
	register float prod;

	prod = 1.0;
	for ( ; n> 0; n--)
		prod *= *x++;
	return(prod);
}


float vl1(x, n)
register float *x;
register int n;
{
	register float l1;

	l1 = 0.0;
	for ( ; n> 0; n--) {
		l1 += ABS(*x);
		x++;
	}
	return(l1);
}


float vl2(x, n)
register float *x;
register int n;
{
	register float l2;

	l2 = 0.0;
	for ( ; n> 0; n--) {
		l2 += *x * *x;
		x++;
	}
	return(l2);
}


void dvsadd(x, a, n)
register double *x, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ += a;
}


void dvsmul(x, a, n)
register double *x, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ *= a;
}


void dvsadd2(x, y, a, n)
register double *x, *y, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ = *y++ + a;
}


void dvsmul2(x, y, a, n)
register double *x, *y, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ = *y++ * a;
}


void dvadd(x, y, n)
register double *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ += *y++;
}


void dvsub(x, y, n)
register double *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ -= *y++;
}


void dvmul(x, y, n)
register double *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ *= *y++;
}


void dvdiv(x, y, n)
register double *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		if (*y != 0)
			*x++ /= *y++;
		else
			err(__FILE__,__LINE__,"dvdiv: divide by zero");
}


void dvsqrt(x, n)
register double *x;
register int n;
{
	for ( ; n > 0; n--,x++) {
		*x = sqrt(*x);
	}
}


void dvabs(x, n)
register double *x;
register int n;
{
	for ( ; n > 0; n--,x++)
		*x = ABS(*x);
}


double dvmax(x, n)
register double *x;
register int n;
{
	register double m = -DHUGE;

	for ( ; n > 0; n--) {
		m = MAX(*x, m);
		x++;
	}
	return(m);
}


double dvmin(x, n)
register double *x;
register int n;
{
	register double m = DHUGE;

	for ( ; n > 0; n--) {
		m = MIN(*x, m);
		x++;
	}
	return(m);
}


double dvabsmax(x, n)
register double *x;
register int n;
{
	register double m = 0.0;
	register double absp;

	for ( ; n > 0; n--) {
		absp = ABS(*x);
		m = MAX(absp, m);
		x++;
	}
	return(m);
}


double dvsum(x, n)
register double *x;
register int n;
{
	register double sum;

	sum = 0.0;
	for ( ; n> 0; n--)
		sum += *x++;
	return(sum);
}


double dvprod(x, n)
register double *x;
register int n;
{
	register double prod;

	prod = 1.0;
	for ( ; n> 0; n--)
		prod *= *x++;
	return(prod);
}


double dvl1(x, n)
register double *x;
register int n;
{
	register double l1;

	l1 = 0.0;
	for ( ; n> 0; n--) {
		l1 += ABS(*x);
		x++;
	}
	return(l1);
}


double dvl2(x, n)
register double *x;
register int n;
{
	register double l2;

	l2 = 0.0;
	for ( ; n> 0; n--) {
		l2 += *x * *x;
		x++;
	}
	return(l2);
}


void dvadd2(x, y, z, n)
register double *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y + *z;
		y++; z++;
	}
}


void dvsub2(x, y, z, n)
register double *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y - *z;
		y++; z++;
	}
}


void dvmul2(x, y, z, n)
register double *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y * *z;
		y++; z++;
	}
}


void dvdiv2(x, y, z, n)
register double *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--)
		if (*z != 0)
			*x++ = *y / *z;
		else
			err(__FILE__,__LINE__,"dvdiv2: divide by zero");
		
		y++; z++;
}


void ivsadd(x, a, n)
register int *x, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ += a;
}


void ivsmul(x, a, n)
register int *x, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ *= a;
}


void ivsadd2(x, y, a, n)
register int *x, *y, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ = *y++ + a;
}


void ivsmul2(x, y, a, n)
register int *x, *y, a;
register int n;
{
	for ( ; n > 0; n--)
		*x++ = *y++ * a;
}


void ivadd(x, y, n)
register int *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ += *y++;
}


void ivsub(x, y, n)
register int *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ -= *y++;
}


void ivmul(x, y, n)
register int *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		*x++ *= *y++;
}


void ivdiv(x, y, n)
register int *x, *y;
register int n;
{
	for ( ; n > 0; n--)
		if (*y != 0)
			*x++ /= *y++;
		else
			err(__FILE__,__LINE__,"ivdiv: divide by zero");
}


void ivsqrt(x, n)
register int *x;
register int n;
{
	for ( ; n > 0; n--,x++) {
		*x = (int) sqrt((double)*x);
	}
}


void ivabs(x, n)
register int *x;
register int n;
{
	for ( ; n > 0; n--,x++)
		*x = ABS(*x);
}


int ivmax(x, n)
register int *x;
register int n;
{
	register int m = -IHUGE;

	for ( ; n > 0; n--) {
		m = MAX(*x, m);
		x++;
	}
	return(m);
}


int ivmin(x, n)
register int *x;
register int n;
{
	register int m = IHUGE;

	for ( ; n > 0; n--) {
		m = MIN(*x, m);
		x++;
	}
	return(m);
}


int ivabsmax(x, n)
register int *x;
register int n;
{
	register int m = 0.0;
	register int absp;

	for ( ; n > 0; n--) {
		absp = ABS(*x);
		m = MAX(absp, m);
		x++;
	}
	return(m);
}


int ivsum(x, n)
register int *x;
register int n;
{
	register int sum;

	sum = 0.0;
	for ( ; n> 0; n--)
		sum += *x++;
	return(sum);
}


int ivprod(x, n)
register int *x;
register int n;
{
	register int prod;

	prod = 1.0;
	for ( ; n> 0; n--)
		prod *= *x++;
	return(prod);
}


int ivl1(x, n)
register int *x;
register int n;
{
	register int l1;

	l1 = 0.0;
	for ( ; n> 0; n--) {
		l1 += ABS(*x);
		x++;
	}
	return(l1);
}


int ivl2(x, n)
register int *x;
register int n;
{
	register int l2;

	l2 = 0.0;
	for ( ; n> 0; n--) {
		l2 += *x * *x;
		x++;
	}
	return(l2);
}


void ivadd2(x, y, z, n)
register int *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y + *z;
		y++; z++;
	}
}


void ivsub2(x, y, z, n)
register int *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y - *z;
		y++; z++;
	}
}


void ivmul2(x, y, z, n)
register int *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--) {
		*x++ = *y * *z;
		y++; z++;
	}
}


void ivdiv2(x, y, z, n)
register int *x, *y, *z;
register int n;
{
	for ( ; n > 0; n--)
		if (*z != 0)
			*x++ = *y / *z;
		else
			err(__FILE__,__LINE__,"ivdiv2: divide by zero");
		
		y++; z++;
}
