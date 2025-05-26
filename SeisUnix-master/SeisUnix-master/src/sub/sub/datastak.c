#include "sub.h"
#include "x_tab.h"

#define NSTACK 		10000

static TPackage		stack[NSTACK];  	/* the stack */
static TPackage		*stackp = stack;        /* next free spot on stack */

void initstack(void)
{
    stackp = stack;
}

TPackage* push(TPackage d)
{
    if (stackp >= &stack[NSTACK])
	execerror("push: data stack overflow");
#ifdef DEBUG_DATA
    fprintf(stderr, "--Pushing %s\n", stringForm(d));
#endif
    *stackp = d;
    return stackp++;
}

TPackage pop(void)
{
    if (stackp == stack)
	execerror("pop: data stack underflow");
#ifdef DEBUG_DATA
    d = *--stackp;
    fprintf(stderr, "--Popping %s\n", stringForm(d));
    return d;
#else
    return *--stackp;
#endif
}

void freePop(void)
{
    TPackage t;

    if (stackp == stack)
	execerror("freePop: data stack underflow");
    t = *--stackp;
#ifdef DEBUG_DATA
    fprintf(stderr, "--freePopping %s\n", stringForm(t));
#endif
    deleteTP(t);
}

TPackage* nextdata(void)
{
    return stackp;
}

/*
**	double handlers
*/

double popD(void)
{
    return asDouble(pop());
}

void pushD(double x)
{
    push(dTPackage(x));
}

void pushI(int x)
{
    push(dTPackage((double)x));
}

/*
**	string (char*) handlers
*/

char* popC(void)
{
    return asString(pop());
}



