#include "sub.h"
#include "x_tab.h"

/*
**	The virtual machine's code space: we should replace
**	the array with a more flexible structure.
*/ 

#define DEFAULTPROG  20000

int		ProgramStackLength = DEFAULTPROG;
static Inst*	prog = 0;
static Inst*	progmax = 0;

Inst    		*progp;
Inst    		*pc;
VMRegisters 		VM;
VMRegisters*		prevVM;	/* used when calling bltins */

static int		depth	= 0;	/* calling depth		*/

int     		returning = 0;

void initcode(void) {
    prog = (Inst*) malloc(ProgramStackLength);
    progmax = prog + ProgramStackLength - 1;
    progp = prog;
    initstack();
    returning = 0;
    VM.callerVM = 0;
    depth = 0;
}

Symbol*	currentFunction(void)
{
    return VM.funcptr;
}

TPackage* ptr2auto(int i)
{
    return (VM.autoptr + i);
}

TPackage* ptr2arg(int i)
{
    return (VM.argptr + i);
}

void define(Symbol *sp, Inst *fstart)      /* put func/proc in symbol table */
           
             
{
	sp->spk.u.defn	= fstart;    /* start of code */
	sp->spk.type	= InstP;
}

/*
**	Pointer and stack handling which precedes function calls:
**		Don't screw with this function.
*/

void call(void)
{
    VMRegisters saved;
    TPackage t;
    TPackage retval;

    int i;

    saved = VM;
    VM.callerVM = (struct VMRegisters*) &saved;

    VM.funcptr = (Symbol*)pc[0];
    VM.nargs = (int)pc[1];
    VM.argptr = nextdata() - VM.nargs;
    VM.autoptr = nextdata();
    VM.nauto = (int)VM.funcptr->spk.u.defn[0];
    t = newTP();
    for(i = 0; i < VM.nauto; i++)
	push(t);

    t.type = InstP;
    t.u.defn = pc + 2;
    push(t);

    ++depth;
    execute(VM.funcptr->spk.u.defn + 1);
    --depth;
    returning = 0;

    retval = pop();
    pc = pop().u.defn;
    push(retval);
    while(VM.nauto-- > 0)
	freePop();
    while(VM.nargs-- > 0)
	freePop();
    VM = saved;
}

void funcret(void)
{
    returning = 1;
}

/*
**	for use by builtins: returns number of arguments
*/
int nArgs(void)
{
    return VM.nargs;
}

/*
**	for use by builtins: returns a shallow copy of the nth argument
**	The first argument is nthArg(1).  Numbering from 1 was a stupid
**	choice.
*/
TPackage nthArg(int n)
{
    if(depth < 1)
	execerror("nthArg: not in a function.");
    if((n < 1) || (n > VM.nargs))
	execerror("nthArg: bad argument index %d (%d args available)",
		  n, VM.nargs);
    return VM.argptr[n - 1];
}

void bltin(void)
{
    VMRegisters saved;
    TPackage r;

    saved = VM;
    VM.callerVM = (struct VMRegisters*) &saved;
    VM.nargs = (int)pc[1];
    VM.argptr = nextdata() - VM.nargs;
    prevVM = &saved;
    r = ((BltIn)(*pc))();

    while(VM.nargs-- > 0)
	freePop();
    push(r);
    pc += 2;
    VM = saved;
}

void varread(void)         /* read into variable */
{
	double x;
	extern FILE *fin;
	Symbol *var = (Symbol *) *pc++;
	switch (fscanf(fin, "%lf", &x)) {
	case EOF:
                x = 0.0;
		break;
        case 0:
		execerror("non-number read into %s", var->name);
		break;
        default:
		x = 1.0;
		break;
        }
	var->ttype = VAR;
	var->spk.u.val = x;
	var->spk.type = Double;
	pushD(x);
}

Inst *code(Inst f)  /* install one instruction or operand */
       
{
        Inst *oprogp = progp;
        if (progp >= progmax)
               execerror("program or function too big for code stack.");
        *progp++ = f;
        return oprogp;
}

void iexecute(Inst **p)	/* run the machine (indirectly) */
        
{
    execute(*p);
}

void execute(Inst *p)    /* run the machine */
        
{
          for (pc = p; *pc != STOP && !returning; )
                  (*(*pc++))();
}

void callSymbol(Symbol *sp, int numargs)
{
    Inst start[4];
    start[0] = (Inst) call;
    start[1] = (Inst) sp;
    start[2] = (Inst) 0;
    start[3] = STOP;
    execute(start);
    freePop();
}

