#include "sub.h"

extern void *malloc (size_t);

static double rMin(TPackage t)
{
    float*   s;
    double v, z;
    int n;

    switch(t.type) {
      case FloatP:
      case OFloatP:
	s = t.u.series;
	n = t.size;
	v = *s++;
	while(--n > 0) {
	    z = *s++;
	    if(v > z)
		v = z;
	}
	return v;
      case SFloatP:
	return *t.u.series;
      case Double:
	return t.u.val;
      case TPSingleP:
	return rMin(*t.u.tpa);
      case TPackageP:
	v = rMin(*t.u.tpa);
	n = 1;
	while(n < t.size) {
	    z = rMin(t.u.tpa[n]);
	    if(v > z)
		v = z;
	}
	return v;
      default:
	execerror("min invoked on argument of wrong type: %s",
		  typeName(t));
	break;
    }
    return 0.0;
}

/*
**	return the maximum value of the (flattened) arguments.
*/
TPackage TPMin(void)
{
    double v, z;
    int n;
    
    if(nArgs() < 1)
	execerror("min: no arguments.");
    v = rMin(nthArg(1));
    for(n = 2; n <= nArgs(); n++) {
	z = rMin(nthArg(n));
	if(z < v)
	    v = z;
    }
    return dTPackage(v);
}



static double rMax(TPackage t)
{
    float*   s;
    double v, z;
    int n;

    switch(t.type) {
      case FloatP:
      case OFloatP:
	s = t.u.series;
	n = t.size;
	v = *s++;
	while(--n > 0) {
	    z = *s++;
	    if(v < z)
		v = z;
	}
	return v;
      case SFloatP:
	return *t.u.series;
      case Double:
	return t.u.val;
      case TPSingleP:
	return rMax(*t.u.tpa);
      case TPackageP:
	v = rMax(*t.u.tpa);
	n = 1;
	while(n < t.size) {
	    z = rMax(t.u.tpa[n]);
	    if(v < z)
		v = z;
	}
	return v;
      default:
	execerror("max invoked on argument of wrong type: %s",
		  typeName(t));
	break;
    }
    return 0.0;
}

/*
**	return the maximum value of the (flattened) arguments.
*/
TPackage TPMax(void)
{
    double v, z;
    int n;
    
    if(nArgs() < 1)
	execerror("max: no arguments.");
    v = rMax(nthArg(1));
    for(n = 2; n <= nArgs(); n++) {
	z = rMax(nthArg(n));
	if(z > v)
	    v = z;
    }
    return dTPackage(v);
}

/*
**	sum the (flattened) contents of one or more arguments
*/
static double rSum(TPackage t)
{
    float*   s;
    double v;
    int n;

    switch(t.type) {
      case FloatP:
      case OFloatP:
	s = t.u.series;
	n = t.size;
	v = *s++;
	while(--n > 0)
	    v += *s++;
	return v;
      case SFloatP:
	return *t.u.series;
      case Double:
	return t.u.val;
      case TPSingleP:
	return rSum(*t.u.tpa);
      case TPackageP:
	v = rSum(*t.u.tpa);
	n = 1;
	while(n < t.size)
	    v += rMax(t.u.tpa[n]);
	return v;
      default:
	execerror("sum invoked on argument of bad type: %s",
		  typeName(t));
	break;
    }
    return 0.0;
}

TPackage TPSum(void)
{
    double v;
    int n;
    
    if(nArgs() < 1)
	execerror("sum: no arguments.");
    v = rSum(nthArg(1));
    for(n = 2; n <= nArgs(); n++)
	v += rSum(nthArg(n));
    return dTPackage(v);
}

TPackage fVector(void)
{
    TPackage t, init;
    int n;

    if(nArgs() < 1)
	execerror("fVector: called with no arguments.");
    if(nArgs() > 2)
	execerror("fVector: too many arguments (%d).", nArgs());
    t = nthArg(1);
    if(t.type != Double)
	execerror("fVector: first argument non-numeric type: %s",
		  typeName(t));
    if(t.u.val < 1.0)
	execerror("fVector: non-positive length requested %d", t.u.val);
    t.size = t.u.val;
    t.type = FloatP;
    t.u.series = (float*) malloc(t.size * sizeof(float));
    if(nArgs() < 2)
	init = dTPackage(0.0);
    else
	init = nthArg(2);
    if(init.type != Double)
	execerror("fVector: second argument not numeric: %s",
		  typeName(t));
    for(n = 0; n < t.size; n++)
	t.u.series[n] = init.u.val;
    return t;
}

TPackage TPSequence(void)
{
    TPackage t;
    double first, second, third;
    double dlen;
    int gotsecond;
    int length;
    int i;

    third = 1.0;
    gotsecond = 0;
    second = 0.0;

    if(nArgs() < 1)
	execerror("sequence: called with no arguments.");
    if(nArgs() > 3)
	execerror("sequence: too many arguments (%d).", nArgs());
    t = nthArg(1);
    if(t.type != Double)
	execerror("sequence: first argument non-numeric type: %s",
		  typeName(t));
    first = t.u.val;

    if(nArgs() > 1) {
	gotsecond = 1;
	t = nthArg(2);
	if(t.type != Double)
	    execerror("sequence: second argument non-numeric type: %s",
		      typeName(t));
	second = t.u.val;
    }
    if(nArgs() > 2) {
	t = nthArg(2);
	if(t.type != Double)
	    execerror("sequence: third argument non-numeric type: %s",
		      typeName(t));
	third = t.u.val;
    }

    if(gotsecond == 0) {
	second = first - 1;
	first = 0;
    }

    dlen = (second - first) / third;
    if(dlen < 1.0)
	execerror("sequence: implausible length %g", dlen);
    length = dlen;
    t.type = FloatP;
    t.size = length;
    t.u.series = (float*) emalloc(length * sizeof(float));
    dlen = first;
    for(i = 0; i < length; i++) {
	t.u.series[i] = dlen;
	dlen += third;
    }
    return t;
}
