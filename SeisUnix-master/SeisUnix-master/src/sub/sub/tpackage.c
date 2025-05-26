#include "sub.h"
#include "x_tab.h"

/*
**	phony constructors, etc
*/


extern void free (void *);
extern void *realloc (void *, size_t);
extern void *malloc (size_t);

TPackage newTP(void)
{
    TPackage n;
    n.type = Nothing;
    n.size = 0;
    n.u.val = 0.0;
    return n;
}

void deleteTP(TPackage t)
{
    int i;
    RBlock* s;

    switch(t.type) {
      case CharP:
	free(t.u.str);
	break;
      case FloatP:
	free(t.u.series);
	break;
      case RBlockP:
	s = (RBlock*) t.u.chunk;
	free(s->rbptr);
	free(s);
	break;
      case TPackageP:
	for(i = 0; i < t.size; i++)
	    deleteTP(t.u.tpa[i]);
	free(t.u.tpa);
	break;
    default:
	break;
    }
}

TPackage deepCopyTP(TPackage t)
{
    TPackage r;
    int i;
    RBlock* s;
    RBlock* os;

    r = t;
    switch(r.type) {
      case CharP:
	r.size = strlen(t.u.str) + 1;
	r.u.str = emalloc(r.size);
	memcpy(r.u.str, t.u.str, r.size);
	break;
      case SFloatP:
	r.type = Double;
	r.u.val = *t.u.series;
	break;
      case FloatP:
      case OFloatP:
	r.type = FloatP;
	r.u.series = (float*) emalloc(r.size * sizeof(float));
	memcpy(r.u.series, t.u.series, r.size * sizeof(float));
	break;
      case RBlockP:
	os = (RBlock*) t.u.chunk;
	s = (RBlock*) emalloc(sizeof(RBlock));
	s->rbptr = (void*) emalloc(os->rblen);
	memcpy(s, os, sizeof(RBlock));
	memcpy(s->rbptr, os->rbptr, sizeof(RBlock));
	r.u.chunk = (void*) s;
	break;
      case TPackageP:
      case OTPackageP:
	r.type = TPackageP;
	r.u.tpa = (TPackage*) emalloc(t.size * sizeof(TPackage));
	for(i = 0; i < r.size; i++)
	    r.u.tpa[i] = deepCopyTP(t.u.tpa[i]);
	break;
      default:
	break;
    }
    return r;
}

/*
**	return a pointer to a[index].  make it if need be.
**	This function cannot handle FloatP and OFloatP types.
*/
TPackage* accessArrayTP(TPackage *array, int index)
{
    int i;

    if(index < 0)
	execerror("accessArrayTP: negative index %d", index);
    switch(array->type) {
      case OTPackageP:
      case TPackageP:
	if(array->size <= index) {
	    if(array->type == OTPackageP)
		execerror("accessArrayTP: can't extend type <%s>",
			  typeName(*array));
	    array->u.tpa =
		(TPackage*) realloc((char*) array->u.tpa,
				    (index + 1) * sizeof(TPackage));
	    if(array->u.tpa == NULL)
		execerror("accessArrayTP: no memory");
	    for(i = array->size; i <= index; i++)
		array->u.tpa[i] = newTP();
	    array->size = index + 1;
	}
	break;
      case SFloatP:
      case FloatP:
      case OFloatP:
	execerror("accessArrayTP: can't handle *Float* types %s",
		  typeName(*array));
	break;
      default:
	deleteTP(*array);
	*array = newTP();
	array->size = index + 1;
	array->u.tpa = (TPackage*) malloc(array->size * sizeof(TPackage));
	array->type = TPackageP;
	for(i = 0; i < array->size; i++)
	    array->u.tpa[i] = newTP();
	break;
    }
	return array->u.tpa + index;
}

TPackage addString(TPackage t, char *s)
{
    int l;
    TPackage r;
    char* z;
    if(t.type != CharP)
	execerror("tried to add characters to a non-string: ", typeName(t));
    l = strlen(t.u.str) + strlen(s) + 1;
    z = emalloc(l);
    strcpy(z, t.u.str);
    strcat(z, s);
    r.type = CharP;
    r.u.str = z;
    r.size = l;
    return r;
}

/*
**	value type interrogations
*/

int isDouble(TPackage d)
{
    return (d.type == Double);
}

int isSym(TPackage d)
{
    return (d.type == SymbolP);
}

int isRBlock(TPackage d)
{
    return(d.type == RBlockP);
}

int isString(TPackage d)
{
    return (d.type == CharP);
}

int isArray(TPackage d)
{
    return (d.type == TPackageP);
}

char* stringForm(TPackage d)
{
    static char z[128];
    switch(d.type) {
      case Nothing:
	return "<Nothing>";
      case Double:
	sprintf(z, "%g", d.u.val);
	return z;
      case SymbolP:
	return d.u.sym->name;
      case StackP:
	return "<StackP>";
      case InstP:
	return "<InstP>";
      case CharP:
	return d.u.str;
      case RBFieldInfoP:
	sprintf(z, "RBFieldInfo:\"%s\"",
		((RBFieldInfo*)d.u.info)->rbfieldname);
	return z;
      case RBlockP:
	sprintf(z, "RBlock of length %d", ((RBlock*)d.u.chunk)->rblen);
	return z;
      case SFloatP:
	sprintf(z, "SFloatP: %g", *(d.u.series));
	return z;
      case TPackageP:
	return "<TPackageP>";
      case FloatP:
	return "<FloatVector>";
      case OFloatP:
	return "<OFloatVector>";
      case FileStar:
	return "<FILE*>";
      case TPSingleP:
	return "<TPSingleP>";
      default:
	execerror("bad stack TPackage type %s", typeName(d));
	exitprocandexit(1);
    }
    return 0;
}

char* typeName(TPackage d)
{
    switch(d.type) {
      case Nothing:
	return "Nothing";
      case SFloatP:
	return "SFloatP";
      case FloatP:
	return "FloatP";
      case OFloatP:
	return "OFloatP";
      case Double:
	return "Double";
      case SymbolP:
	return "SymbolP";
      case RBFieldInfoP:
	return "RBFieldInfoP";
      case RBField:
	return "RBField";
      case RBlockP:
	return "RBlockP";
      case StackP:
	return "StackP";
      case InstP:
	return "InstP";
      case CharP:
	return "CharP";
      case FileStar:
	return "FileStar";
      case TPSingleP:
	return "TPSingleP";
      case TPackageP:
	return "TPackageP";
      default:
	return 0;
    }
}

TPackage dTPackage(double x)
{
    TPackage d;
    d.type = Double;
    d.size = 1;
    d.u.val = x;
    return d;
}

TPackage cTPackage(char *s)
{
    TPackage d;
    d.type = CharP;
    d.size = strlen(s) + 1;
    d.u.str = emalloc(d.size);
    memcpy(d.u.str, s, d.size);
    return d;
}

TPackage sTPackage(Symbol *s)
{
    TPackage d;
    d.type = SymbolP;
    d.size = 1;
    d.u.sym = s;
    return d;
}


double asDouble(TPackage d)
{
    if(d.type == Double)
	return d.u.val;
    execerror("asDouble: data non-numeric type: %s", typeName(d));
    return 0.0;
}

char* asString(TPackage d)
{
    if(d.type == CharP)
	return d.u.str;
    execerror("asString: data non-string type: %s", typeName(d));
    return (char*) 0;
}

Symbol* asSymbol(TPackage d)
{
    if(d.type == SymbolP)
	return d.u.sym;
    execerror("asSymbol: data not a Symbol*: %s", typeName(d));
    return (Symbol*) 0;
}

FILE* asFileStar(TPackage d)
{
    if(d.type == FileStar)
	return d.u.fstar;
    execerror("asFileStar: data not a FILE*", typeName(d));
    return (FILE*) 0;
}

