#include "sub.h"

extern   int     errno;

static TPackage errcheck(TPackage d, char *s)
{
    if (errno == EDOM) {
	errno = 0;
	execerror("%s: argument out of domain", s);
    } else if (errno == ERANGE) {
	errno = 0;
	execerror("%s: result out of range", s);
    }
    return d;
}

static TPackage arrayApply(double (*f) (/* ??? */), TPackage r, char *s)
{
    int i;

    switch(r.type) {
      case Double:
	r.u.val = f(r.u.val);
	break;
      case FloatP:
	for(i = 0; i < r.size; i++)
	    r.u.series[i] = f(r.u.series[i]);
	break;
      case TPackageP:
	for(i = 0; i < r.size; i++)
	    r.u.tpa[i] = arrayApply(f, r.u.tpa[i], s);
	break;
      default:
	execerror("%s: bad argument type (%s)", s, typeName(r));
	break;
    }
    return r;
}

typedef double (*double2double)(double);

static TPackage scall(double2double f, char *s)
{
    TPackage r;

    if(nArgs() != 1)
	execerror("%s: bad argument count (%d)", s, nArgs());
    r = nthArg(1);
    if(r.type == Double)
	return errcheck(dTPackage(f(asDouble(nthArg(1)))), s);
    r = deepCopyTP(r);
    return errcheck(arrayApply(f, r, s), s);
}

TPackage TPSin(void)
{
    return scall(sin, "sin");
}

TPackage TPCos(void)
{
    return scall(cos, "cos");
}

TPackage TPAtan(void)
{
    return scall(atan, "atan");
}

TPackage TPLog(void)
{
    return scall(log, "log");
}

TPackage TPLog10(void)
{
    return scall(log10, "log10");
}

TPackage TPExp(void)
{
    return scall(exp, "exp");
}

TPackage TPSqrt(void)
{
    return scall(sqrt, "sqrt");
}

static double dint(double d)
{
    return (double) ((int) d);
}

TPackage TPInteger(void)
{
    return scall(dint, "int");
}

static double dnint(double d)
{
    return ((d >= 0) ? (floor((d)+.5)) : (ceil((d)-.5)));
}

TPackage TPNInteger(void)
{
    return scall(dnint, "nint");
}

static double dblabs(double d)
{
    return (d < 0) ? (- d) : d;
}

TPackage TPAbs(void)
{
    return scall(dblabs, "abs");
}

/*
**	return a random double in [0,1).  Stolen from TclX and calls
**	a Berkeley version of random (included in bkrandom.c).
*/
#define RANDOM_RANGE 0x7fffffffL

static double ReallyRandom ()
{
    static unsigned rrseed = 0;
    static double scale = 1.0/RANDOM_RANGE;

    if(rrseed == 0) {
	rrseed = (unsigned) (getpid() + time((time_t *)NULL));
        subSrandom (rrseed);
    }
    return (subRandom() * scale);
}

TPackage urandom(void)
{
    int n;
    TPackage t;
    int i;

    if(nArgs() < 1)
	return dTPackage(ReallyRandom());
    t = nthArg(1);
    if(t.type != Double)
	execerror("random: optional argument a number");
    n = (int) t.u.val;
    if(n < 1)
	execerror("random: optional argument must be >= 1");
    t = newTP();
    t.type = FloatP;
    t.size = n;
    t.u.series = (float *) emalloc(n * sizeof(float));
    for(i = 0; i < n; i++)
	t.u.series[i] = ReallyRandom();
    return t;
}


