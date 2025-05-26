#include "sub.h"
#include "x_tab.h"

/*
**	make sure the tpackage is free-standing (FloatP, not OFLoatP)
**	and is at least minlength long (zero-padding if necessary).
*/

extern void *malloc (size_t);

static TPackage newFMinSize(TPackage oldt, int minlength)
{
    int l;
    int i;

    l = oldt.size;
    if(l < minlength)
	l = minlength;
    if((l > oldt.size) || (oldt.type == OFloatP)) {
	TPackage q;
	q = oldt;
	oldt.type = FloatP;
	oldt.size = l;
	oldt.u.series = (float*) malloc(l * sizeof(float));
	memcpy(oldt.u.series, q.u.series, q.size * sizeof(float));
	if(q.size < l)
	    for(i = q.size; i < l; i++)
		oldt.u.series[i] = 0.0;
	deleteTP(q);
    }
    return oldt;
}
/*
**	true if both arguments are packed with numbers.
*/
static int bothNumeric(TPackage a, TPackage b)
{
    switch(a.type) {
      case Double:
      case FloatP:
      case OFloatP:
	break;
      default:
	return 0;
    }
    switch(b.type) {
      case Double:
      case FloatP:
      case OFloatP:
	break;
      default:
	return 0;
    }
    return 1;
}

void add(void)
{
    TPackage t, s, r;
    int i;

    t = pop();
    s = pop();
    if((t.type == CharP) && (s.type == CharP)) {
	push(addString(s, asString(t)));
	deleteTP(t);
	deleteTP(s);
	return;
    }
    if((t.type == Double) && (s.type == Double)) {
	pushD(asDouble(t) + asDouble(s));
	return;
    }
    if(t.type == Double) {
	r = s; s = t; t = r;
    }
    if(s.type == Double) { /* one scalar and its in s */
	switch(t.type) {
	  case OFloatP:
	  case FloatP:
	    t = newFMinSize(t, t.size);
	    for(i = 0; i < t.size; i++)
		t.u.series[i] += s.u.val;
	    push(t);
	    return;
	  default:
	    break;
	}
    } else if(bothNumeric(t, s)){
	t = newFMinSize(t, s.size);
	for(i = 0; i < s.size; i++)
	    t.u.series[i] += s.u.series[i];
	deleteTP(s);
	push(t);
	return;
    }
    execerror("add: sum of a %s and a %s is undefined.",
	      typeName(s), typeName(t));
}

void sub(void)
{
    TPackage t, s, r;
    int swapped;
    int i;

    s = pop();
    t = pop();

    if((t.type == Double) && (s.type == Double)) {
	pushD(asDouble(t) - asDouble(s));
	return;
    }
    swapped = 0;
    if(t.type == Double) {
	r = s; s = t; t = r;
	swapped = 1;
    }
    if(s.type == Double) { /* one scalar and its in s */
	switch(t.type) {
	  case OFloatP:
	  case FloatP:
	    t = newFMinSize(t, t.size);
	    if(swapped) {
		for(i = 0; i < t.size; i++)
		    t.u.series[i] = s.u.val - t.u.series[i];
	    } else {
		for(i = 0; i < t.size; i++)
		    t.u.series[i] = t.u.series[i] - s.u.val;
	    }
	    push(t);
	    return;
	  default:
	    break;
	}
    } else if(bothNumeric(t, s)){
	t = newFMinSize(t, s.size);
	for(i = 0; i < s.size; i++)
	    t.u.series[i] -= s.u.series[i];
	deleteTP(s);
	push(t);
	return;
    }
    execerror("sub:  a %s minus a %s is undefined.",
	      typeName(t), typeName(s));
}

void mul(void)
{
    TPackage t, s, r;
    int i;

    t = pop();
    s = pop();
    if((t.type == Double) && (s.type == Double)) {
	pushD(asDouble(t) * asDouble(s));
	return;
    }
    if(t.type == Double) {
	r = s; s = t; t = r;
    }
    if(s.type == Double) { /* one scalar and its in s */
	switch(t.type) {
	  case OFloatP:
	  case FloatP:
	    t = newFMinSize(t, t.size);
	    for(i = 0; i < t.size; i++)
		t.u.series[i] *= s.u.val;
	    push(t);
	    return;
	  default:
	    break;
	}
    } else if(bothNumeric(t, s)){
	t = newFMinSize(t, s.size);
	for(i = 0; i < s.size; i++)
	    t.u.series[i] *= s.u.series[i];
	deleteTP(s);
	push(t);
	return;
    }
    execerror("mul: a %s times a %s is undefined.",
	      typeName(s), typeName(t));
}

void divop(void)
{
    TPackage t, s;
    int i;

    t = pop();
    s = pop();
    if((t.type == Double) && (s.type == Double)) {
	if(t.u.val == 0.0)
		execerror("can't divide %g by zero", s.u.val);
	pushD(asDouble(s) / asDouble(t));
	return;
    }
    if(s.type == Double) {
	switch(t.type) {
	  case OFloatP:
	  case FloatP:
	    t = newFMinSize(t, t.size);
	    for(i = 0; i < t.size; i++) {
		if(t.u.series[i] == 0.0)
		    execerror("div: array element %d was zero", i);
		t.u.series[i] = s.u.val / t.u.series[i];
	    }
	    push(t);
	    return;
	  default:
	    execerror("div: bad type for divisor <%s>", typeName(t));
	}
    } else if(t.type == Double) {
	switch(s.type) {
	  case OFloatP:
	  case FloatP:
	    s = newFMinSize(s, s.size);
	    for(i = 0; i < s.size; i++) {
		s.u.series[i] = s.u.series[i] / t.u.val;
	    }
	    push(s);
	    return;
	  default:
	    execerror("div: bad type for dividend <%s>", typeName(t));
	}
    } else if(bothNumeric(t, s)){
	t = newFMinSize(t, s.size);
	for(i = 0; i < s.size; i++)
	    t.u.series[i] = s.u.series[i] / t.u.series[i];
	deleteTP(s);
	push(t);
	return;
    }
    execerror("div: a %s divided by a %s is undefined.",
	      typeName(s), typeName(t));
}

void power(void)
{
    TPackage t, s;
    int i;

    t = pop();
    s = pop();
    if((t.type == Double) && (s.type == Double)) {
	if(s.u.val == 0.0)
		execerror("can't exponentiate zero");
	pushD(pow(asDouble(s), asDouble(t)));
	return;
    }
    if(s.type == Double) {
	switch(t.type) {
	  case OFloatP:
	  case FloatP:
	    if(s.u.val == 0.0)
		execerror("can't exponentiate zero");
	    t = newFMinSize(t, t.size);
	    for(i = 0; i < t.size; i++) {
		t.u.series[i] = pow(s.u.val, t.u.series[i]);
	    }
	    push(t);
	    return;
	  default:
	    execerror("pow: bad type for divisor <%s>", typeName(t));
	}
    } else if(t.type == Double) {
	switch(s.type) {
	  case OFloatP:
	  case FloatP:
	    s = newFMinSize(s, s.size);
	    for(i = 0; i < s.size; i++) {
		s.u.series[i] = pow(s.u.series[i], t.u.val);
	    }
	    push(s);
	    return;
	  default:
	    execerror("pow: bad type for target <%>", typeName(s));
	}
    } else if(bothNumeric(t, s)){
	t = newFMinSize(t, s.size);
	for(i = 0; i < s.size; i++)
	    t.u.series[i] = pow(s.u.series[i], t.u.series[i]);
	deleteTP(s);
	push(t);
	return;
    }
    execerror("pow: a %s divided by a %s is undefined.",
	      typeName(s), typeName(t));
}

void negate(void)
{
	pushD(-popD());
}


