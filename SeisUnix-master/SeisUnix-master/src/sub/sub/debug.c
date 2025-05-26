#include "sub.h"
#include "x_tab.h"

/*
**	debugging stuff
*/

TPackage traceback(void)
{
    VMRegisters* r;
    int i;
    int printItem(FILE *fs, TPackage tp);
    FILE* tb;

    tb = stderr;
    r = (VMRegisters*) VM.callerVM;
    fprintf(tb, "Execution stack traceback:\n");
    while(r->callerVM != 0) {
	fprintf(tb, "  %s(", r->funcptr->name);
	for(i = 0; i < r->nargs; i++) {
	    printItem(tb, r->argptr[i]);
	    if(i < (r->nargs - 1))
		fprintf(tb, ", ");
	}
	fprintf(tb, ")\n");
	r = (VMRegisters*) r->callerVM;
    }
    return newTP();
}

static TPackage memorysize(void)
{
    long p = (long) sbrk(0);
    return dTPackage((double) p);
}

TPackage internals(void)
{
    extern int nextTemp;

    if(nArgs() != 1)
	execerror("internals: bad argument count (%d)", nArgs());
    switch((int)asDouble(nthArg(1))) {
      case 0:
	return memorysize();
      case 1:
	return dTPackage((double)nextTemp);
      default:
	execerror("internals: unknown request selector value (%g)",
		  asDouble(nthArg(1)));
    }
    return memorysize();
}
