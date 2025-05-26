#include "sub.h"
#include "x_tab.h"

static TPackage*	tempTP		= 0;
static int		nTemp		= -1;
int			nextTemp	= 0;    /* needs to be visible */
static int		tpInc		= 128;


#ifdef sun4
#include <memory.h>
#endif

static TPackage* getTemp(void)
{
    if(tempTP == 0) {
	tempTP = (TPackage*) emalloc(tpInc * sizeof(TPackage));
	nTemp = tpInc;
    }
    while(nextTemp >= nTemp) {
	nTemp += tpInc;
	tempTP = (TPackage*) realloc((char*) tempTP, nTemp * sizeof(TPackage));
    }
    return &tempTP[nextTemp++];
}

static void scratchTemp(TPackage *t)
{
    if(nextTemp <= 0)
	return;
    if(t == (tempTP + (nextTemp - 1)))
	--nextTemp;
}

/*
**	points to the last leaf of a variable-tree that was
**	pushed onto the value stack.  we need this to assign a
**	value to '$'.  if nothing is there, we use 0.
*/
static TPackage** lastP	= 0;
static int lastPlen	= 0;
static int lastPnexti	= 0;
static int lastPinc	= 32;
/*
**	push a TPackage* onto the lastP stack, extending it if
**	necessary.  (note that the stack never shrinks.)
*/
static void pushP(TPackage *tp)
{
    TPackage** newP;
    if(lastPnexti >= lastPlen) {
	newP = (TPackage**) malloc((lastPlen + lastPinc) * sizeof(TPackage*));
	if(lastP != 0) {
	    memcpy(newP, lastP, lastPlen * sizeof(TPackage*));
	    free(lastP);
	}
	lastP = newP;
	lastPlen += lastPinc;
    }
    lastP[lastPnexti++] = tp;
}
/*
**	replace the current lastP top-of-stack with a new pointer.
*/
static void subP(TPackage *tp)
{
    if(lastPnexti <= 0)
	execerror("subP attempted on empty lastP");
    lastP[lastPnexti - 1] = tp;
}
/*
**	pop a TPackage from the lastP stack.  popping an empty
**	stack is a fatal error because it can't happen.
*/
static void popP(void)
{
    if(lastPnexti-- <= 0)
	execerror("empty lastP stack popped.");
}
/*
**	stuff the value of the last index of the current, or most
**	recent, array onto the stack.
*/
void lastindex(void)
{
    TPackage *t;
    if(lastPnexti <= 0)
	execerror("$ (last index) used outside array index context");
    t = lastP[lastPnexti - 1];
    switch(t->type) {
      case TPSingleP:
	switch(t->u.tpa->type) {
	  case RBField:
	    pushI(getRBRange((RBlock*) t->u.tpa->u.chunk,
			     (RBFieldInfo*) t->u.tpa->pfi) - 1);
	    break;
	  default:
	    pushI(t->u.tpa->size - 1);
	    break;
	}
	break;
      case FloatP:
      case OFloatP:
      case SFloatP:
	pushI(t->size - 1);
	break;
      default:
	execerror("unanticipated argument type [%s] for lastindex()",
		  typeName(*t));
	break;
    }
}

void noop(void)
{
    return;
}

void datapush(void)
{
    TPackage v;
    v = ((Symbol *)*pc++)->spk;
    push(deepCopyTP(v));
}

void popStack(void)
{
    freePop();
}
/*
**	push a pointer to a variable's TPackage onto the D-stack.
**	Every access to a variable begins with this opcode.
*/
void tpvarpush(void)
{
    Symbol* v;
    TPackage q;

    v = (Symbol*) (*pc++);
    LastPushedVar = v->name;
    q.type = TPSingleP;
    switch(v->ttype) {
      case VAR:
	q.u.tpa = &(v->spk);
	break;
      case AUTOVAR:
	q.u.tpa = ptr2auto(v->loc);
	break;
      case FORMALARG:
	q.u.tpa = ptr2arg(v->loc);
	break;
      default:
	execerror("tried to evaluate non-variable: %s", v->name);
	break;
    }
    pushP(push(q));
    return;
}
/*
**	Replace the previous TPP with the TPP produced by indexing
**	into the previous item.  This action has to support a number
**	of special cases.
*/
void tpindexpush(void)
{
    TPackage who,howfar;
    int index;
    TPackage* r;
    int i, j;
    RBFieldInfo* rbfi;

    howfar = pop();
    index = (howfar.type == Double) ? howfar.u.val : -1;
    who = pop();
    if(who.type != TPSingleP)
	execerror("tpindexpush: stack contents not tpsinglep (%s).",
		  typeName(who));
    r = who.u.tpa;
/*
**	r must remain a pointer to the off-stack TPackage that
**	contains the object's value.  who is used to accumulate
**	the TP that will supersede the one r points to.
*/
    switch(r->type) {
      case SFloatP:
	execerror("tpindexpush: can't index type SFloatP.");
	break;
      case FloatP:
      case OFloatP:
	i = (index != -1) ? index : (r->size - 1);
	if(i < 0)
	    execerror("tpindexpush: *Float* index out of range: %d", i);
	if(i >= r->size) {
	    if(r->type == OFloatP)
		execerror("tpindexpush: can't extend type %s",
			  typeName(*r));
	    r->u.series =
		(float *) realloc(r->u.series, (i + 1) * sizeof(float));
	    for(j = r->size; j <= i; j++)
		r->u.series[j] = 0.0;
	    r->size = i + 1;
	}
	who.type = SFloatP;
	who.u.series = &r->u.series[i];
	who.size = 1;
	break;
/*
**	a field member's default index cases are handled in
**	private code.
**
**	NB: how will we do $ stuff?
*/
      case RBField:
	who = *r;
	rbfi = who.pfi;
	rbfi->rbiset.l = rbfi->rbiset.u = index;
	break;
      case TPackageP:
      case OTPackageP:
	i = (index != -1) ? index : (r->size - 1);
	if(i < 0)
	    execerror("tpindexpush: *TPackageP* index out of range: %d",
		      i);
	if((i >= r->size) && (r->type == OTPackageP))
		execerror("tpindexpush: can't extend type %s",
			  typeName(*r));
	who.size = 1;
	who.type = TPSingleP;
	who.u.tpa = accessArrayTP(who.u.tpa, i);
	break;
      default:
	if(index == -1)
	    execerror("tpindexpush: default case can't handle upper bound");
	i = index;
	if(i < 0)
	    execerror("tpindexpush: default case index was negative: %d", i);
	who.size = 1;
	who.type = TPSingleP;
	who.u.tpa = accessArrayTP(r, i);
	break;
    }
    scratchTemp(r);
    r = getTemp();
    *r = who;
    who.type = TPSingleP;
    who.size = 1;
    who.u.tpa = r;
    subP(push(who));
    return;
}
/*
**	Replace the previous TPP with the TPP produced by subranging
**	into the previous item.  This action has to support a number
**	of special cases.
*/
void tpsubrangepush(void)
{
    TPackage who,xto;
    int goodto;
    int from, to;
    TPackage* r;
    RBFieldInfo* rbfi;

    xto = pop();
    goodto = (xto.type == Double);
    to = goodto ? xto.u.val : -1;

    from = popD();
    who = pop();
    if(who.type != TPSingleP)
	execerror("tpsubrangepush: stack contents not tpsinglep (%s).",
		  typeName(who));
    r = who.u.tpa;
    switch(r->type) {
      case SFloatP:
	execerror("tpindexpush: can't subrange a %s", typeName(*r));
	break;
      case FloatP:
      case OFloatP:
	if(!goodto)
	    to = r->size - 1;
	if((from < 0) || (from >= r->size))
	    execerror("tpsubrangepush: -from- index out of range: %d", from);
	if((to < 0) || (to >= r->size))
	    execerror("tpsubrangepush: -to- index out of range: %d", to);
	who.type = OFloatP;
	who.u.series = &r->u.series[from];
	who.size = to - from + 1;
	break;
/*
**	a field member's default index cases are handled in
**	private code.
**
**	NB: how will we do $ stuff?
*/
      case RBField:
	who = *r;
	rbfi = who.pfi;
	rbfi->rbiset.l = from;
	rbfi->rbiset.u = to;
	break;
      default:
	if(!goodto)
	    to = r->size;
	accessArrayTP(who.u.tpa, to);
	who.u.tpa = accessArrayTP(who.u.tpa, from);
	who.size = to - from + 1;
	who.type = OTPackageP;
	break;
    }
    scratchTemp(r);
    r = getTemp();
    *r = who;
    who.type = TPSingleP;
    who.size = 1;
    who.u.tpa = r;
    subP(push(who));
    return;
}
/*
**	Replace the previous TPP with the TPP produced as a result
**	of accessing a member by name in the previous item.
**
**	Every access of the form xxx.yyy has to execute this code.
**	At present, the xxx part has to be a record block (RB).
**
**	When we get here, pc is pointing to a field info object
**	and the stack has a TPP leading to an rb chunk pointer.
*/
void tpmemberpush(void)
{
    TPackage		who;
    TPackage*		r;
    RBFieldInfo*	which;
    RBlock*		c;

    which = (RBFieldInfo*) (*pc++);
    who = pop();
/*
**	make sure we really have a record object down there somewhere
**	and dig out its address.
*/
    if((who.type != TPSingleP) || (who.u.tpa->type != RBlockP))
	execerror("tpmemberpush: target type %s not a record object",
		  typeName(*who.u.tpa));
    c = (RBlock*) who.u.tpa->u.chunk;
/*
**	clean up the intermediate TP (and then get a new one)
*/
    scratchTemp(who.u.tpa);
    r = getTemp();
    r->type = RBField;
    r->size = 1;
    r->u.chunk = c;
    r->pfi = which;
/*
**	build the new TPP and stick it on the stack.
*/
    who.type = TPSingleP;
    who.size = 1;
    who.u.tpa = r;
    subP(push(who));
    return;
}
/*
**	replace a variable's TPP with a copy of its value.
*/
static TPackage doEval(TPackage vp)
{
    TPackage	       	v;

    switch(vp.type) {
      case SFloatP:
	v = dTPackage(*(vp.u.series));
	break;
      case TPSingleP:
	v = deepCopyTP(*(vp.u.tpa));
	break;
      case RBField:
	v = getRBValue((RBlock*) vp.u.chunk, (RBFieldInfo*) vp.pfi);
	break;
      default:
	v = deepCopyTP(vp);
    }
    return v;
}

/*
**	replace a variable's TPP with a copy of its value.
*/
void tpeval(void)
{
    TPackage		vp;
    TPackage*		r;

    vp = pop();
    if(vp.type != TPSingleP)
	execerror("tpeval: stack not tpsinglep (%s).", typeName(vp));
    r = vp.u.tpa;
    vp = *r;
    scratchTemp(r);
    popP();
    push(doEval(vp));
}

static void assignIntoTPVector(TPackage *target, TPackage *value)
{
    int i;

    switch(value->type) {
      case SymbolP:
	execerror("assignIntoTPVector: can't assign from %s",
		  typeName(*value));
	break;
      case FloatP:
      case OFloatP:
	if(value->size > target->size)
	    execerror("assignIntoTPVector: float source (%d) > dest (%d)",
		      value->size, target->size);
	for(i = 0; i < value->size; i++)
	    target->u.tpa[i] = dTPackage(value->u.series[i]);
	for(i = value->size; i < target->size; i++)
	    target->u.tpa[i] = newTP();
	break;
      case TPackageP:
      case OTPackageP:
	if(value->size > target->size)
	    execerror("assignIntoTPVector: tp source (%d) > dest (%d)",
		      value->size, target->size);
	for(i = 0; i < value->size; i++)
	    target->u.tpa[i] = value->u.tpa[i];
	for(i = value->size; i < target->size; i++)
	    target->u.tpa[i] = newTP();
	break;
      default:
	for(i = 0; i < target->size; i++)
	    target->u.tpa[i] = deepCopyTP(*value);
	deleteTP(*value);
	break;
    }
}

static void assignIntoFVector(TPackage *target, TPackage *value)
{
    int i;
    double d;

    switch(value->type) {
      case Double:
	d = value->u.val;
	for(i = 0; i < target->size; i++)
	    target->u.series[i] = d;
	break;
      case FloatP:
      case OFloatP:
	if(value->size > target->size)
	    execerror("assignIntoFVector: source (%d) >  dest (%d)",
		      value->size, target->size);
	memcpy(target->u.series, value->u.series, value->size*sizeof(float));
	for(i = value->size; i < target->size; i++)
	    target->u.series[i] = 0.0;
	break;
      case TPackageP:
      case OTPackageP:
	if(value->size > target->size)
	    execerror("assignIntoFVector: source (%d) >  dest (%d)",
		      value->size, target->size);
	for(i = 0; i < value->size; i++) {
	    if(value->u.tpa[i].type != Double)
		execerror("assignIntoFVector: bad source type: %s",
			  typeName(value->u.tpa[i]));
	    target->u.series[i] = value->u.tpa[i].u.val;
	}
	for(i = value->size; i < target->size; i++)
	    target->u.series[i] = 0.0;
	break;
      default:
	execerror("assignIntoFVector: can't assign from %s", typeName(*value));
	break;
    }
}

void tpassign(void)
{
    TPackage		vp;
    TPackage   		newvalue;
    TPackage*		r;

    newvalue = pop();
    vp = pop();
    if(vp.type != TPSingleP)
	execerror("tpassign: stack not tpsinglep (%s).", typeName(vp));
    r = vp.u.tpa;
    switch(r->type) {
      case OFloatP:
	assignIntoFVector(r, &newvalue);
	break;
      case OTPackageP:
	assignIntoTPVector(r, &newvalue);
	break;
      case SFloatP:
	*(r->u.series) = newvalue.u.val;
	break;
      case RBField:
	putRBValue((RBlock*) r->u.chunk, (RBFieldInfo *) r->pfi, &newvalue);
	break;
      case TPSingleP:
	deleteTP(*(r->u.tpa));
	*(r->u.tpa) = deepCopyTP(newvalue);
	break;
      case TPackageP:
      case FloatP:
      default:
	deleteTP(*r);
	*r = deepCopyTP(newvalue);
	break;
    }
    scratchTemp(r);
    popP();
    push(newvalue);
}

/*
**   	various operations
*/
/*
**	dupTOSeval should only be used for duping a variable's TPP.
**	On return TOS has the value and the next cell has the original
**	reference.
*/
void dupTOSeval(void)
{
    TPackage t;
    t = pop();
/*
**	MLS/NER:: was:
		push(t);
		pushP(t);
       but changed it to the following.  Not sure what's going on here.
*/
    pushP(push(t));
    push(doEval(t));
}

void swapTOS(void)
{
    TPackage t, n;
    t = pop();
    n = pop();
    push(t);
    push(n);
}

void forcode(void)
{
	Inst *savepc = pc;    

	ic_execute(savepc + 2);	/* initialization */
	pop();
	ic_execute(savepc + 3);     /* condition */
	while (popD() != 0.0) {
		ic_execute(savepc);  /* body */
		if (returning)
			break;
		ic_execute(savepc + 4);	/* iteration */
		pop();
                ic_execute(savepc + 3);	/* condition */ 
	}
	if (!returning)
        	pc = *((Inst **)(savepc+1));  /* next statement */
}

void whilecode(void)
{
	Inst *savepc = pc;    
        
	execute(savepc+2);     /* condition */
	while (popD() != 0.0) {
		ic_execute(savepc);  /* body */
		if (returning)
			break;
                execute(savepc+2);      /* condition */ 
	}
	if (!returning)
        	pc = *((Inst **)(savepc+1));  /* next statement */
}

void ifcode(void)
{
	Inst *savepc= pc;     /* then part */

	execute(savepc+3);    /* condition */
	if (popD())
		ic_execute(savepc);
        else if (*((Inst **)(savepc+1))) /* else part? */
		ic_execute(savepc+1);
        if (!returning)
        	pc = *((Inst **)(savepc+2));     /* next stmt */
}

