/*
**	type conversions
*/
#include "sub.h"

double	strtod(const char *, char **);
char*	emalloc(unsigned int n);

TPackage TPStrToNum(void)
{
    TPackage r;
    TPackage s;
    char* e;
    s = nthArg(1);
    r = dTPackage( strtod(asString(s), &e) );
    if(e == asString(s))
	execerror("String -> double conversion failed.");
    return r;
}

TPackage TPDNumToStr(void)
{
    TPackage v;
    TPackage f;
    char b[128];

    v = nthArg(1);
    f = nthArg(2);
    sprintf(b, asString(f), asDouble(v));
    return cTPackage(b);
}

TPackage TPINumToStr(void)
{
    TPackage v;
    TPackage f;
    char b[128];

    v = nthArg(1);
    f = nthArg(2);
    sprintf(b, asString(f), (int)(asDouble(v)));
    return cTPackage(b);
}

TPackage TPisDoubleP(void)
{
    TPackage t;

    if(nArgs() != 1)
	return dTPackage(0);
    t = nthArg(1);
    return dTPackage( (t.type == Double) ? 1.0 : 0.0);
}

TPackage TPisStringP(void)
{
    TPackage t;

    if(nArgs() != 1)
	return dTPackage(0);
    t = nthArg(1);
    return dTPackage( (t.type == CharP) ? 1.0 : 0.0);
}

TPackage TPisNothing(void)
{
    TPackage t;

    if(nArgs() != 1)
	return dTPackage(0);
    t = nthArg(1);
    return dTPackage( (t.type == Nothing) ? 1.0 : 0.0);
}

TPackage TPisArrayP(void)
{
    TPackage t;

    if(nArgs() != 1)
	return dTPackage(0);
    t = nthArg(1);
    return dTPackage( (t.type == TPackageP) ? 1.0 : 0.0);
}

TPackage TPisfVectorP(void)
{
    TPackage t;

    if(nArgs() != 1)
	return dTPackage(0);
    t = nthArg(1);
    return dTPackage( (t.type == FloatP) ? 1.0 : 0.0);
}

