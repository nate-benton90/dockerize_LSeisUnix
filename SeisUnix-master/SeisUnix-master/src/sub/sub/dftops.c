#include "functions.h"
#include "dft.h"

typedef struct {
    int n;
    complex* c;
} carray;

/*
**	return the value of the nth member of an array of numeric values
*/
static double nthValue(TPackage t, const int i)
{
    TPackage tmp;

    switch(t.type) {
      case FloatP:
      case OFloatP:
	if(i < t.size)
	    return t.u.series[i];
	execerror("nthValue: index %d out-of-range (%d)", i, t.size);
	break;
      case TPackageP:
      case OTPackageP:
	if(i >= t.size)
	    execerror("nthValue: index %d out-of-range (%d)", i, t.size);
	tmp = t.u.tpa[i];
	if(tmp.type != Double)
	    execerror("nthValue: bad element type <%s> at index %d",
		      i, typeName(tmp));
	return tmp.u.val;
	break;
      default:
	execerror("nthValue: not a numeric value: <%s>",
		  typeName(t));
    }
    return 0.0;
}
/*
**	if the argument appears to be a numeric array, return
**	its length.  otherwise return -1.
*/
static int numericArraySize(TPackage t)
{
    switch(t.type) {
      case FloatP:
      case OFloatP:
      case TPackageP:
      case OTPackageP:
	return t.size;
      default:
	return -1;
    }
}
/*
**	if the argument appears to be a complex array (an array of two
**	numeric arrays), return the length of the (first) numeric
**	array.  otherwise return -1.
*/
static int complexArraySize(TPackage t)
{
    switch(t.type) {
      case TPackageP:
      case OTPackageP:
	return t.u.tpa[0].size;
      default:
	return -1;
    }
}
/*
**	return the complex array corresponding to the only
**	argument, or break.  return value is to malloc'd space.
*/
static carray complicate(TPackage t)
{
    carray c;
    int n;
    int i;
    TPackage a;
    int na;
    TPackage b;
    int nb;

    n = complexArraySize(t);
    if(n >= 0) {
	c.c = (complex*) emalloc(n * sizeof(complex));
	c.n = n;
	a = t.u.tpa[0];
	na = a.size;
	b = t.u.tpa[1];
	nb = b.size;
	for(i = 0; i < n; i++) {
	    if(i < na)
		c.c[i].r = nthValue(a, i);
	    else
		c.c[i].r = 0.0;
	    if(i < nb)
		c.c[i].i = nthValue(b, i);
	    else
		c.c[i].i = 0.0;
	}
	return c;
    }
    n = numericArraySize(t);
    if(n >= 0) {
	c.c = (complex*) emalloc(n * sizeof(complex));
	c.n = n;
	for(i = 0; i < n; i++) {
	    c.c[i].r = nthValue(t, i);
	    c.c[i].i = 0.0;
	}
	return c;
    }
    execerror("complicate: not a legal argument");
    return c;
}
/*
**	return a complex array (a pair of fVectors) corresponding
**	the the carray c.
*/
static TPackage uncomplicate(carray c)
{
    TPackage t;
    int i;

    t = newTP();
    t.type = TPackageP;
    t.size = 2;
    t.u.tpa = (TPackage *) emalloc(2 * sizeof(TPackage));
    for(i = 0; i < 2; i++) {
	t.u.tpa[i].type = FloatP;
	t.u.tpa[i].size = c.n;
	t.u.tpa[i].u.series = (float *) emalloc(c.n * sizeof(float));
    }
    for(i = 0; i < c.n; i++) {
	t.u.tpa[0].u.series[i] = c.c[i].r;
	t.u.tpa[1].u.series[i] = c.c[i].i;
    }
    return t;
}
/*
**	pfnext(n)
*/
TPackage TPpfnext(void)
{
    TPackage t;
    int n;

    if(nArgs() < 1)
	execerror("pfnext: need an argument");
    t = nthArg(1);
    if(t.type != Double)
	execerror("pfnext: need a numeric argument");
    n = t.u.val;
    return dTPackage((double)npfa(n));
}
/*
**	pfbest(n [, nmax])
*/
TPackage TPpfbest(void)
{
    TPackage t;
    int n;
    int nmax;

    if(nArgs() < 1)
	execerror("pfbest: need an argument");
    t = nthArg(1);
    if(t.type != Double)
	execerror("pfbest: need a numeric argument");
    n = t.u.val;
    nmax = 720720;
    if(nArgs() > 1) {
	t = nthArg(2);
	if(t.type != Double)
	    execerror("pfbest: optional second argument must be numeric");
	nmax = t.u.val;
    }
    return dTPackage((double)npfao(n, nmax));
}
/*
**	extend(y, n[, val)
*/
TPackage TPextend(void)
{
    TPackage y;
    TPackage t;
    int newn;
    double exval;
    int n;
    int i;

    if(nArgs() < 2)
	execerror("pfextend: need at least two arguments");
    y = nthArg(1);
    t = nthArg(2);
    if(t.type != Double)
	execerror("pfextend: 2nd argument must be numeric");
    newn = (int) t.u.val;
    if(newn < 1)
	execerror("pfextend: new length must be positive");
    exval = 0.0;
    if(nArgs() > 2) {
	t = nthArg(3);
	if(t.type != Double)
	    execerror("pfextend: optional 3rd argument must be numeric");
	exval = t.u.val;
    }
    t = newTP();
    t.type = FloatP;
    t.size = newn;
    t.u.series = (float *) emalloc(sizeof(float) * newn);
    n = y.size;
    if(n > newn)
	n = newn;
    for(i = 0; i < n; i++)
	t.u.series[i] = nthValue(y, i);
    for(i = n; i < newn; i++)
	t.u.series[i] = exval;
    return t;
}

/*
**	fdft(y)	- y is either a numeric array or an array of two numeric
**		arrays
**		- returns an array of two numeric arrays
*/
TPackage TPfdft(void)
{
    TPackage t;
    carray c;

    if(nArgs() < 1)
	execerror("fdft: requires an argument");
    t = nthArg(1);
    c = complicate(t);
    pfacc(fwdsign, c.n, c.c);
    return uncomplicate(c);
}
/*
**	idft(y)	- y is either a numeric array or an array of two numeric
**		arrays
**		- returns an array of two numeric arrays
*/
TPackage TPidft(void)
{
    TPackage t;
    carray c;
    int i;
    double enn;

    if(nArgs() < 1)
	execerror("idft: requires an argument");
    t = nthArg(1);
    c = complicate(t);
    pfacc(invsign, c.n, c.c);
    enn = c.n;
    for(i = 0; i < c.n; i++) {
	c.c[i].r /= enn;
	c.c[i].i /= enn;
    }
    return uncomplicate(c);
}
