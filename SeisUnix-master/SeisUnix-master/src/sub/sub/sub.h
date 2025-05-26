#ifndef SUB_H
#define SUB_H

#include "subinc.h"
#include "support.h"

/*
**	TPackage - typed data package
*/

typedef struct TPackage (*BltIn)();
typedef void   (*Inst)();

typedef enum {
    Nothing,
    SFloatP,	  	/* ptr to single float value 		*/
    FloatP,		/* ptr to array of floats		*/
    OFloatP,		/* ptr to other's float array		*/
    Double,		/* value				*/
    SymbolP,	 	/* points to symbol table entry 	*/
    StackP, 		/* points to a stack location		*/
    InstP,	 	/* points to Inst	      		*/
    CharP, 		/* points to char			*/
    FileStar,	 	/* stdio channel pointer 		*/
    TPSingleP,	 	/* pointer to one TPackage 		*/
    TPackageP, 		/* pointer to an array of TP's		*/
    OTPackageP,		/* pointer to other's array of TP's	*/
    RBFieldInfoP, 	/* info about an RB field		*/
    RBlockP, 		/* an rb chunk				*/
    RBField,		/* a particular member of a chunk	*/
    LastType		/* always the last entry		*/
} PackageType;

typedef union PValue {
	double			val;
	struct Symbol*		sym;
	BltIn			ptr;
	struct TPackage*	stackp;
	Inst*			defn;
	char*       		str;
	struct TPackage*	tpa;
	float*			series;
	FILE*			fstar;
	void*			chunk;
	void*		        info;
} PValue;

/*
**	TPackage.size holds:
**		- allocated length for a string (strlen() + 1).
**		- allocated length for an SISChunk.
**		- number of entries in a TPackage array
**		- number of values in a FloatP array.
**
**	Aggregate types (currently only data records) require two
**	values: a pointer to the aggregate (u) and a pointer to a
**	field info structure (pfi, so we can get at the value).
*/
typedef struct TPackage {
    PackageType type;
    int 	size;
    PValue	u;
    void*	pfi;
} TPackage;
/*
**	internal type that can hold the most general type of
**	indexing that makes sense for this data stream.  (I don't
**	yet know if extending this to higher dimensionality
**	will work smoothly.)
**
**	for SEGY it is two integers: an upper index and a lower index.
*/
typedef struct {
    int l;
    int u;
} iSet;

#include "brecord.h"

/*
**	symbol table entry
*/

typedef struct Symbol { /* symbol table entry */
	char    	*name;
	int		ttype;
	int		loc;
	int		flags;
	TPackage	spk;
	struct Symbol*  next;
	struct Symbol*	local;
} Symbol;

/*
**	virtual machine state
*/

typedef struct {
    TPackage*		argptr;		/* first argument slot	*/
    int			nargs;
    TPackage*		autoptr;	/* first auto slot 	*/
    int			nauto;
    Symbol*		funcptr;	/* current function	*/
    struct VMRegisters*	callerVM;	/* 0 at end of chain	*/
} VMRegisters;

/*
**	TPackage support
*/

TPackage	newTP(void);
void		deleteTP(TPackage t);
TPackage	deepCopyTP(TPackage t);
TPackage	addString(TPackage t, char *s);
TPackage*	accessArrayTP(TPackage *array, int index);

TPackage dTPackage(double x);	/* wrap a double	*/
TPackage cTPackage(char *s);	/* wrap a char*		*/
TPackage sTPackage(Symbol *s);	/* wrap a Symbol*	*/

TPackage	TPOutputRBlock(void);

int	isDouble(TPackage d);
int	isSym(TPackage d);
int	isString(TPackage d);
int	isArray(TPackage d);
int	isRBlock(TPackage d);

char*	stringForm(TPackage d);
char*	typeName(TPackage d);

double		asDouble(TPackage d);
char*		asString(TPackage d);
Symbol*		asSymbol(TPackage d);
FILE*		asFileStar(TPackage d);

/*
**	symbol table support
*/
void	beginlocal(void);
void	openlocal(void);
void 	closelocal(void);
Symbol*	endlocal(void);
Symbol*	lookup(char *s);
Symbol*	install(char *s);
Symbol*	installlc(char *s);
Symbol*	installcons(char* s);
Symbol*	installanon();
void	dumpSymbols(FILE *out);

/*
**	data stack support
*/

void		initstack(void);
TPackage*      	push(TPackage d);
TPackage	pop(void);
void		freePop(void);
TPackage*	nextdata(void);
void		deleteTP(TPackage t);

double	popD(void);
void	pushD(double x);
void	pushI(int x);

char* popC(void);

/*
**	virtual machine support
*/

#define STOP     (Inst) 0

extern VMRegisters	VM;
extern VMRegisters*	prevVM;
extern Inst*		progp;
extern Inst*		pc;
extern int		returning;

TPackage*	ptr2auto(int i);
TPackage*	ptr2arg(int i);

void		skip_to_eol(void);
extern char*	SourceText;

Inst	*code(Inst f);

TPackage	TPOpPow(void);

int		nArgs(void);
TPackage	nthArg(int n);

void init(void);
void initcode(void);
Symbol* currentFunction(void);
void tpvarpush(void);
void tpindexpush(void);
void tpsubrangepush(void);
void tpmemberpush(void);

void tpeval(void);

void tpassign(void);

void noop(void);
void popStack(void);
void datapush(void);
void dupTOSeval(void);
void swapTOS(void);
void incr(void);
void decr(void);
void mod(void);
void add(void);
void sub(void);
void mul(void);
void divop(void);
void negate(void);
void gt(void);
void lt(void);
void ge(void);
void eq(void);
void ne(void);
void and(void);
void or(void);
void not(void);
void power(void);
void zero(void);
void one(void);
void nothing(void);
void lastindex(void);
void le(void);
void whilecode(void);
void forcode(void);
void ifcode(void);

void define(Symbol *sp, Inst *fstart);
void call(void);
void funcret(void);
void bltin(void);
void prexpr();
void prstr();
void varread(void);

void execute(Inst *p);
void iexecute(Inst **p);
int run(void);
void callSymbol(Symbol *sp, int numargs);
extern int verbose;

void exitproconly(void);
void exitprocandexit(int exitarg);

void subSrandom(unsigned int x);
long subRandom();

#define ic_execute(x)	iexecute((Inst**)(x))

#endif
