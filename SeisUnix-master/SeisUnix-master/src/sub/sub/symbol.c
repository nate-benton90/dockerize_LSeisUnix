#include "sub.h" 
#include "x_tab.h"

static Symbol* global		= 0;
static Symbol* constants	= 0;
static Symbol* local		= 0;
static Symbol* anon		= 0;

static int inLocal = 0;


extern void free (void *);

void beginlocal(void)
{
    inLocal = 1;
    local = 0;
}

void openlocal(void)
{
    inLocal = 1;
}

void closelocal(void)
{
    inLocal = 0;
}

Symbol* endlocal(void)
{
    Symbol* sp;
    inLocal = 0;
    sp = local;
    local = 0;
    return sp;
}

static Symbol *reallookup(char *s, Symbol *b)
{
    Symbol *sp;
    for (sp = b; sp != (Symbol *) 0; sp = sp->next)
	if (strcmp(sp->name, s) == 0)
	    return sp;
    return 0;      /* 0 ==> not found */
}

Symbol *lookup(char *s)
{
    Symbol *sp;

    if(local != 0) {
	sp = reallookup(s, local);
	if(sp != 0)
	    return sp;
    }
    sp =  reallookup(s, global);
    if(sp == 0)
	sp = reallookup(s, constants);
    return sp;
}

static Symbol *realinstall(char *s, Symbol **b)
{
    Symbol *sp;
    char *emalloc(unsigned int n);
    
    sp = (Symbol *) emalloc(sizeof(Symbol));
    if(s != 0) {
	sp->name = emalloc(strlen(s)+1);
	strcpy(sp->name, s);
    } else
	sp->name = 0;
    sp->spk = newTP();
    sp->next = *b;
    sp->local = 0;
    *b = sp;
    return sp;
}

Symbol *install(char *s)  /* install s in symbol table */
        
{
    if(inLocal)
	return realinstall(s, &local);
    return realinstall(s, &global);
}

Symbol* installlc(char *s)
{
    Symbol* sp;
    char *emalloc(unsigned int n), *t;
    int i;
    
    t = emalloc(strlen(s)+1);
    for(i = 0; i <= strlen(s); i++)
	t[i] = tolower(s[i]);
    sp = install(t);
    free(t);
    return sp;
}

Symbol* installcons(char* s)
{
    Symbol* sp;
    char *emalloc(unsigned int n), *t;
    int i;
    
    t = emalloc(strlen(s)+1);
    for(i = 0; i <= strlen(s); i++)
	t[i] = tolower(s[i]);
    sp = realinstall(t, &constants);
    free(t);
    return sp;
}

Symbol* installanon()
{
    Symbol* sp;
    char *emalloc(unsigned int n), *t;
    static long anonv = 0L;

    t = emalloc(16);
    sprintf(t, "#%ld", anonv++);
    sp = realinstall(t, &anon);
    free(t);
    return sp;
}

static void dumpSymbolTable(FILE *out, Symbol *t, int showvals)
{
    Symbol* q;
    while(t != 0) {
	if((t->ttype != VAR) && (t->ttype != FUNCTION)) {
	    t = t->next;
	    continue;
	}
	fprintf(out, "   %-12s", t->name);
	if(t->ttype == FUNCTION)
	    fprintf(out, " (fcn)");
	if(showvals && (t->ttype == VAR)) {
	    if(strlen(stringForm(t->spk)) < 40)
		fprintf(out, " %s", stringForm(t->spk));
	    else
		fprintf(out, "  string of length %d",
			strlen(stringForm(t->spk)));
	}
	if((t->ttype == FUNCTION) && (t->local != 0)) {
	    fprintf(out, "\n");
	    q = t->local;
	    while(q != 0) {
		fprintf(out, "      %-12s (auto)\n", q->name);
		q = q->next;
	    }
	} else
	    fprintf(out, "\n");
	t = t->next;
    }
}

void dumpSymbols(FILE *out)
{
    fprintf(out, "%s::\n", "Globals and autos");
    dumpSymbolTable(out, global, 0);
    fprintf(out, "%s::\n", "Constants");
    dumpSymbolTable(out, constants, 1);
}
