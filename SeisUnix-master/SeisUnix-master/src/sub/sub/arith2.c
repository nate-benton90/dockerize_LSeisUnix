#include "sub.h"
#include "x_tab.h"

void incr(void)
{
    double d = popD();
    pushD(d + 1.0);
}

void decr(void)
{
    double d = popD();
    pushD(d - 1.0);
}

void mod(void)
{
	int a = popD();
	int b = popD();
        pushD((double) (b % a));
}

void gt(void)
{
	double a = popD();
	double b = popD();
	pushD((double)(b > a));
}

void lt(void)
{
	double a = popD();
	double b = popD();
	pushD((double)(b < a));
}

void ge(void)
{
	double a = popD();
	double b = popD();
	pushD((double)(b >= a));
}

void le(void)
{
	double a = popD();
	double b = popD();
	pushD((double)(b <= a));
}

void eq(void)
{
    TPackage ap, bp;
    ap = pop();
    bp = pop();

    if(isDouble(ap)) {
	pushD((double)(asDouble(ap) == asDouble(bp)));
    } else if(ap.type == Nothing) {
	pushD((double)(bp.type == Nothing));
    } else
    execerror("operator == used for types %s amd %s.",
	      typeName(ap), typeName(bp));
}

void ne(void)
{
	double a = popD();
	double b = popD();
	pushD((double)(b != a));
}

void and(void)
{ 
	double a = popD();
	double b = popD();
	pushD((double)(b != 0.0 && a != 0.0));
}

void or(void)
{
	double a = popD();
	double b = popD();
	pushD((double)(b != 0.0 || a != 0.0));
}
	
void not(void)
{
    pushD((double)(popD() == 0.0));
}

void zero(void)
{
    pushD(0.0);
}

void one(void)
{
    pushD(1.0);
}

void nothing(void)
{
    push(newTP());
}


