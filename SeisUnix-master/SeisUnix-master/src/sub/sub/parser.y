%{ 
#include "sub.h"

#ifdef ENABLE_DEBUGGING
#include <stdio.h>
#define YYDEBUG	1
#endif

#define code2(c1,c2)    code(c1); code(c2)
#define code3(c1,c2,c3) code(c1); code(c2); code(c3)

int verbose = 0;

static int autolevel = 0;
static int farglevel = 0;

%}
%union {
	Symbol*		sym;      /* symbol table pointer  */
	Inst*		inst;     /* machine instruction   */
	int     	narg;     /* number of arguments   */
	RBFieldInfo*	rbf;     /* info pointer      	   */
}
%token  <sym>   NUMBER STRING VAR FORMALARG AUTOVAR
%token	<rbf>	MEMBER
%token	<sym>	BLTIN
%token	<sym>	UNDEF WHILE_T IF_T ELSE_T AUTO
%token	<sym>	FOR_T
%token  <sym>   FUNCTION RETURN_T FUNC READ
%token	<sym>	ENDPROGRAM
%type	<inst>	tp_pointer
%type   <inst>  expr stmt asgn stmtlist index
%type	<inst>	subrange bexpr eexpr
%type   <inst>  while for if begin end xexpr
%type   <sym>   procname named_variable 
%type   <narg>  arglist defarglist autolist autodefns
%right	'='
%left	OR
%left	AND
%left	GT GE LT LE EQ NE
%left	'%'
%left	'+' '-'
%left	'*' '/'
%left	UNARYMINUS NOT
%left	PLUSPLUS MINUSMINUS
%right	'^'
%start	list
%%
list:     /* nothing */
	| list defn      { }
	| list ENDPROGRAM
			{  code(STOP); YYACCEPT;}
	| list error ';' { YYABORT; }
	;
named_variable:
    	  VAR
    	| AUTOVAR
    	| FORMALARG
    	;
procname: VAR
	| FUNCTION
	;
defn:	  FUNC procname		{$2->ttype=FUNCTION; define($2, progp);
				  beginlocal(); farglevel = 0; autolevel = 0;}
	  '(' defarglist ')'	{closelocal();}
	  '{'
	  autodefns	{ code((Inst)autolevel);}
	  stmtlist '}'  { $2->local = endlocal(); code(nothing);
			  code(funcret);}
    	;
defarglist:
    	  /* zip */	     { $$ = 0;}
    	| VAR		     { $$ = 1; $1->ttype = FORMALARG;
				 $1->loc = farglevel++; }
    	| defarglist ',' VAR { $$ = $1 + 1; $3->ttype = FORMALARG;
				$3->loc = farglevel++; }
    	;
autodefns:
          /* zip */	     {$$ = 0;}
	| autodefns AUTO {openlocal();} autolist ';'  { closelocal();}
    	;
autolist:
    	  /* zip */	     {$$ = 0;}
	| VAR		     {$1->ttype = AUTOVAR; $1->loc = autolevel++; }
	| autolist ',' VAR   {$3->ttype = AUTOVAR; $3->loc = autolevel++; }
	;
arglist:  /* nothing */         {$$ = 0;}
	| expr                  {$$ = 1;}
	| arglist ',' expr      {$$ = $1 + 1;}
	;
subrange: '[' bexpr ':' eexpr ']'	{$$ = $2;}
	;
bexpr:	  	{$$ = 0; code(zero);}
    	| expr	{$$ = 0;}
	;
eexpr:		{$$ = 0; code(nothing);}
	| expr {$$ = 0;}
	;
index:	  '[' expr ']'	{$$ = $2;}
    	;
tp_pointer:
    	  named_variable	{$$ = code(tpvarpush); code((Inst)$1);}
	| tp_pointer index	{code(tpindexpush); $$ = $1;}
	| tp_pointer subrange	{code(tpsubrangepush); $$ = $1;}
	| tp_pointer MEMBER	{code(tpmemberpush); code((Inst)$2); $$ = $1;}
	;
asgn:	tp_pointer '=' expr	{ code(tpassign); $$ = $1; }
	;
begin:    /* nothing */         {  $$ = progp; }
	;
end:      /* nothing */         {  code(STOP); $$ = progp; }
	;
stmtlist: /* nothing */         { $$ = progp; }
	| stmtlist stmt
	;
stmt:	';'	{  $$ = code(noop); }
	| expr ';'   { code(popStack);}
	| RETURN_T ';' { $$ = code(nothing); code(funcret);}
	| RETURN_T expr ';'
		{ $$=$2; code(funcret); }
	| while '(' xexpr ')' stmt end {
		($1)[1] = (Inst)$5;    /* body of loop */
		($1)[2] = (Inst)$6; }  /* end, if cond fails */
	| for '(' xexpr ';' xexpr ';' xexpr ')' stmt end {
    		($1)[1] = (Inst) $9;	/* body */
		($1)[2] = (Inst) $10;	/* end */
		($1)[3] = (Inst) $3;	/* initialization */
		($1)[4] = (Inst) $5;	/* condition */
		($1)[5] = (Inst) $7;	/* iteration */
		}
	| if '(' xexpr ')' stmt end {     /* else-less if */
		($1) [1] = (Inst)$5;    /* thenpart */
		($1) [3] = (Inst)$6; }  /* end, if cond fails */
	| if '(' xexpr ')' stmt end ELSE_T stmt end {      /* if with else */
		($1) [1] = (Inst)$5;    /* thenpart */
		($1) [2] = (Inst)$8;    /* elsepart */
		($1) [3] = (Inst)$9; }  /* end, if cond fails */
	| '{' stmtlist '}'       { $$ = $2; }
	;
xexpr:	/* nothing */	{ $$ = code(one); code(STOP); }
    	| expr		{ $$ = $1; code(STOP); }
	;
for:	FOR_T	{ $$ = code(forcode); code2(STOP,STOP);
			 code3(STOP,STOP,STOP); }
	;
while:	WHILE_T { $$ = code3(whilecode,STOP,STOP); }
	;
if:	IF_T    { $$ = code(ifcode); code3(STOP,STOP,STOP); }
	;
expr:
	  NUMBER	{ $$ = code2(datapush, (Inst)$1);}
	| '$'		{ $$ = code(lastindex);}
	| tp_pointer   	{ code(tpeval); $$ = $1; }
	| PLUSPLUS tp_pointer
			{ code(dupTOSeval); code(incr);
				code(tpassign); $$ = $2; }
	| tp_pointer PLUSPLUS
			{ code(dupTOSeval); code(swapTOS);
			  code(dupTOSeval); code(incr);
			  code(tpassign); code(popStack); $$ = $1;}
	| MINUSMINUS tp_pointer
			{ code(dupTOSeval); code(decr);
				code(tpassign); $$ = $2; }
	| tp_pointer MINUSMINUS
			{ code(dupTOSeval); code(swapTOS);
			  code(dupTOSeval); code(decr);
			  code(tpassign); code(popStack); $$ = $1;}
	| asgn 
	| READ '(' named_variable ')' { $$ = code2(varread, (Inst)$3);}
	| FUNCTION begin '(' arglist ')'
		{ $$ = $2; code(call);  code((Inst)$1); code((Inst)$4);}
	| BLTIN begin '(' arglist ')'
		{ $$ = $2; code(bltin); code((Inst)$1->spk.u.ptr);
			code((Inst)$4); }
	| '(' expr ')'  { $$ = $2; }
	| expr '%' expr { code(mod); }
	| expr '+' expr { code(add); }
	| expr '-' expr { code(sub); }
	| expr '*' expr { code(mul); }
	| expr '/' expr { code(divop); }
	| expr '^' expr { code(power); }
	| '-' expr   %prec UNARYMINUS  { $$=$2; code(negate); }
	| expr GT expr  { code(gt); }
	| expr GE expr  { code(ge); }
	| expr LT expr  { code(lt); }
	| expr LE expr  { code(le); }
	| expr EQ expr  { code(eq); }
	| expr NE expr  { code(ne); }
	| expr AND expr { code(and); }
	| expr OR expr  { code(or); } 
	| NOT expr      { $$ = $2; code(not); }
	| STRING        { $$ = code2(datapush, (Inst)$1); }
	;
%%
       /* end of grammar */
