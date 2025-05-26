/*
**	home for more built-in functions
*/
#include "sub.h"

TPackage object_size(void)
{
    TPackage q;
    double s;

    q = nthArg(1);
    switch(q.type) {
      case RBlockP:
      case TPackageP:
      case FloatP:
      case OFloatP:
	s = q.size;
	break;
      case CharP:
	s = strlen(q.u.str);
	break;
      default:
	s = 1.0;
	break;
    }
    return dTPackage(s);
}

TPackage fArg(void)
{
    int i;

    if(nArgs() < 1)
	return dTPackage((double) prevVM->nargs);
    i = asDouble(nthArg(1));
    if(i > 0)
	return deepCopyTP(prevVM->argptr[i - 1]);
    if(i < 0)
	execerror("fArg: negative argument: %d", i);
    return cTPackage(prevVM->funcptr->name);
}

TPackage bltin_exit(void)
{
    exitprocandexit((int)(asDouble(nthArg(1)) + 0.5));
    return dTPackage(0.0);
}

TPackage clocktime(void)
{
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    return dTPackage((double)(tv.tv_sec + 1.0e-6 * tv.tv_usec));
}

static int perimeterlength(TPackage a, int *fptr)
{
    int len = 0;
    int i;

    if(a.type == Double)
	return 1;
    switch(a.type) {
      case CharP:
	*fptr = 0;
	len = 1;
	break;
      case TPackageP:
      case OTPackageP:
      case TPSingleP:
	for(i = 0; i < a.size; i++)
	    len += perimeterlength(a.u.tpa[i], fptr);
	break;
      case FloatP:
      case OFloatP:
	len += a.size;
	break;
      default:
	execerror("flatten: perimeterlength not smart enough for %s",
		  typeName(a));
    }
    return len;
}

static void perimeterelements(TPackage *rptr, int *nptr, TPackage a)
{
    int i;

    switch(a.type) {
      case Double:
	if(rptr->type == FloatP)
	    rptr->u.series[(*nptr)++] = a.u.val;
	else
	    rptr->u.tpa[(*nptr)++] = a;
	break;
      case FloatP:
      case OFloatP:
	for(i = 0; i < a.size; i++) {
	    if(rptr->type == FloatP)
		rptr->u.series[(*nptr)++] = a.u.series[i];
	    else
		rptr->u.tpa[(*nptr)++] = dTPackage(a.u.series[i]);
	}
	break;
      case CharP:
	rptr->u.tpa[(*nptr)++] = deepCopyTP(a);
	break;
      case TPackageP:
      case OTPackageP:
      case TPSingleP:
	for(i = 0; i < a.size; i++)
	    perimeterelements(rptr, nptr, a.u.tpa[i]);
	break;
      default:
	execerror("flatten: perimeterelements not smart enough for %s",
		  typeName(a));
    }
}

TPackage TPFlattened(void)
{
    int length;
    int allfloats;
    TPackage r;
    int i;

    length = 0;
    allfloats = 1;
    for(i = 1; i <= nArgs(); i++)
	length += perimeterlength(nthArg(i), &allfloats);
    if(length == 0)
	return newTP();

    if(allfloats != 0) {
	r.type = FloatP;
	r.size = length;
	r.u.series = (float*) emalloc(length * sizeof(float));
    } else {
	r. type = TPackageP;
	r.size = length;
	r.u.tpa = (TPackage*) emalloc(length * sizeof(TPackage));
    }

    length = 0;
    for(i = 1; i <= nArgs(); i++)
	perimeterelements(&r, &length, nthArg(i));
    return r;
}

TPackage TPSystem(void)
{
    char* s = asString(nthArg(1));
    int r;

    r = system(s);
    return dTPackage((double) r);
}

