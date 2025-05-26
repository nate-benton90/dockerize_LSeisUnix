
# line 1 "parser.y"
 
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


# line 18 "parser.y"
typedef union  {
	Symbol*		sym;      /* symbol table pointer  */
	Inst*		inst;     /* machine instruction   */
	int     	narg;     /* number of arguments   */
	RBFieldInfo*	rbf;     /* info pointer      	   */
} YYSTYPE;
# define NUMBER 257
# define STRING 258
# define VAR 259
# define FORMALARG 260
# define AUTOVAR 261
# define MEMBER 262
# define BLTIN 263
# define UNDEF 264
# define WHILE_T 265
# define IF_T 266
# define ELSE_T 267
# define AUTO 268
# define FOR_T 269
# define FUNCTION 270
# define RETURN_T 271
# define FUNC 272
# define READ 273
# define ENDPROGRAM 274
# define OR 275
# define AND 276
# define GT 277
# define GE 278
# define LT 279
# define LE 280
# define EQ 281
# define NE 282
# define UNARYMINUS 283
# define NOT 284
# define PLUSPLUS 285
# define MINUSMINUS 286
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 194 "parser.y"

       /* end of grammar */
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 84
# define YYLAST 496
short yyact[]={

  34,   4,  56,  21,  42,   8, 144,  59,  57,  43,
  58,  82,  60, 120,  79,  54,   9,   5,  17,   3,
  50,  52,  51,  26,  13, 138,  18,  56,  61, 141,
  34, 127,  59,  57,  42,  58,  92,  60,  25,  43,
  59,  57, 139,  58,  82,  60,  49, 108, 122, 137,
   6,  91, 135,  26, 124,  86, 118,  59, 125,  61,
  34, 145,  60, 136,  42, 117, 135,  14, 126,  43,
  15, 123, 121,  85,  74,  73,  72,  34,  11,  23,
  20,  42,  22,  26,  61,  16,  43,  32,  10, 112,
   2,   1,  61,  19,  34,  53,  56,  87,  42,  12,
  70,  59,  57,  43,  58,   7,  60,  31,  56,  61,
  30,  29, 119,  59,  57,  75,  58,  32,  60,  24,
  56, 110, 111, 133, 114,  59,  57,  77,  58,  56,
  60,  35, 116,  76,  59,  57,  38,  58,  56,  60,
   0,   0, 107,  59,  57,   0,  58,  32,  60,  56,
 129,  55,   0,  61,  59,  57,   0,  58, 140,  60,
 130,   0, 132,   0,   0,  61,   0,   0,  83,  84,
 131,   0, 148, 149,   0,   0,   0,  61,   0,   0,
   0,   0,  78, 146, 147,   0,  61, 143,   0,   0,
   0,   0,   0,   0,   0,  61,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  61,   0,   0,   0,
   0,   0,   0,   0,   0,  78,   0,   0,   0,   0,
   0,  33,  45,  50,  52,  51,   0,  41,   0,  46,
  48,   0,   0,  47,  40,  28,   0,  39,  80,  81,
  69,  68,  62,  63,  64,  65,  66,  67,  44,  36,
  37,  33,  45,  50,  52,  51,   0,  41,   0,  46,
  48,   0,   0,  47,  40,  28,   0,  39,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  44,  36,
  37,  33,  45,  50,  52,  51,   0,  41,   0,  46,
  48,   0,   0,  47,  40,  28,   0,  39,  33,  45,
  50,  52,  51,   0,  41,   0,   0,   0,  44,  36,
  37,  40,   0,   0,  39,  33,  45,  50,  52,  51,
   0,  41,   0,   0,   0,  44,  36,  37,  40,   0,
   0,  39,   0,   0,  69,  68,  62,  63,  64,  65,
  66,  67,  44,  36,  37,   0,  69,  68,  62,  63,
  64,  65,  66,  67,   0,  27,   0,   0,  69,  68,
  62,  63,  64,  65,  66,  67,   0,  69,  68,  62,
  63,  64,  65,  66,  67,   0,   0,  68,  62,  63,
  64,  65,  66,  67,  71,   0,   0,   0,   0,  62,
  63,  64,  65,  66,  67,   0,   0,   0,  88,  89,
  90,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  93,  94,  95,  96,  97,  98,  99, 100,
 101, 102, 103, 104, 105, 106,   0,   0, 109, 109,
 109,   0,   0,   0,   0, 113,   0,   0, 115,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 128, 128,   0,   0,   0, 109,   0,
 134,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 142,   0,   0,   0, 109 };
short yypact[]={

-1000,-255,-1000,-1000,  -9,-254,-1000,-1000,-1000,-1000,
  38,-235,  26,-1000,-1000,-241, -97,-1000,-1000,-265,
-1000,-1000,  -6,-244,-1000,-1000,-1000,  92,  41,  36,
  35,  34,-1000,-1000,-1000, -47,-239,-239,-1000,  33,
-1000,-1000,  58,  58,  58,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,  -8,-1000,-1000,  58,  58,  58,  58,
  58,  58,  58,  58,  58,  58,  58,  58,  58,  58,
-1000,  83,  58,  58,  58, -36,-1000,-1000,-1000,  58,
-1000,-1000,  58, -80, -80,-239,  25,  16,  71, -66,
 -66,-1000,-246,  -2,  15,  15, -66, -66, -66, -10,
 -10, -10, -10, -10, -10, 112, 101,-1000,  31,  59,
 -11,  30,-1000,  59,  -4, -35,  27,  58,  58,-1000,
-1000,  24,  58,  24,  58,-1000,-1000,  22,  59,   8,
-1000, -17,-1000, -64,  59,  58,-1000,-1000,-1000,  58,
-261,-1000,  59,  20,  24,  24,-1000,-1000,-1000,-1000 };
short yypgo[]={

   0, 131, 355,  38, 136,  82, 133, 127, 124, 123,
 111, 110, 107,  55,  25,  47, 105,  46,  31,  99,
  95,  93,  91,  90,  88,  85,  80,  79 };
short yyr1[]={

   0,  22,  22,  22,  22,  17,  17,  17,  16,  16,
  24,  25,  26,  23,  19,  19,  19,  21,  27,  21,
  20,  20,  20,  18,  18,  18,   7,   8,   8,   9,
   9,   6,   1,   1,   1,   1,   4,  13,  14,   5,
   5,   3,   3,   3,   3,   3,   3,   3,   3,   3,
  15,  15,  11,  10,  12,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2 };
short yyr2[]={

   0,   0,   2,   2,   3,   1,   1,   1,   1,   1,
   0,   0,   0,  12,   0,   1,   3,   0,   0,   5,
   0,   1,   3,   0,   1,   3,   5,   0,   1,   0,
   1,   3,   1,   2,   2,   2,   3,   0,   0,   0,
   2,   1,   2,   2,   3,   6,  10,   6,   9,   3,
   0,   1,   1,   1,   1,   1,   1,   1,   2,   2,
   2,   2,   1,   4,   5,   5,   3,   3,   3,   3,
   3,   3,   3,   2,   3,   3,   3,   3,   3,   3,
   3,   3,   2,   1 };
short yychk[]={

-1000, -22, -23, 274, 256, 272,  59, -16, 259, 270,
 -24,  40, -19, 259,  41,  44, -25, 259, 123, -21,
 -26, 268,  -5, -27, 125,  -3,  59,  -2, 271, -10,
 -11, -12, 123, 257,  36,  -1, 285, 286,  -4, 273,
 270, 263,  40,  45, 284, 258, 265, 269, 266, -17,
 259, 261, 260, -20, 259,  59,  37,  43,  45,  42,
  47,  94, 277, 278, 279, 280, 281, 282, 276, 275,
  59,  -2,  40,  40,  40,  -5,  -6,  -7, 262,  61,
 285, 286,  91,  -1,  -1,  40, -13, -13,  -2,  -2,
  -2,  59,  44,  -2,  -2,  -2,  -2,  -2,  -2,  -2,
  -2,  -2,  -2,  -2,  -2,  -2,  -2,  59, -15,  -2,
 -15, -15, 125,  -2,  -8,  -2, -17,  40,  40,  41,
 259,  41,  59,  41,  58,  93,  41, -18,  -2, -18,
  -3, -15,  -3,  -9,  -2,  44,  41,  41, -14,  59,
 -14,  93,  -2, -15, 267,  41,  -3,  -3, -14, -14 };
short yydef[]={

   1,  -2,   2,   3,   0,   0,   4,  10,   8,   9,
   0,  14,   0,  15,  11,   0,   0,  16,  17,  12,
  39,  18,   0,  20,  13,  40,  41,   0,   0,   0,
   0,   0,  39,  55,  56,  57,   0,   0,  62,   0,
  37,  37,   0,   0,   0,  83,  53,  52,  54,  32,
   5,   6,   7,   0,  21,  42,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  43,   0,  50,  50,  50,   0,  33,  34,  35,   0,
  59,  61,  27,  58,  60,   0,   0,   0,   0,  73,
  82,  19,   0,  67,  68,  69,  70,  71,  72,  74,
  75,  76,  77,  78,  79,  80,  81,  44,   0,  51,
   0,   0,  49,  36,   0,  28,   0,  23,  23,  66,
  22,   0,  50,   0,  29,  31,  63,   0,  24,   0,
  38,   0,  38,   0,  30,   0,  64,  65,  45,  50,
  47,  26,  25,   0,   0,   0,  38,  38,  48,  46 };
# line 1 "/usr/lib/yaccpar"
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif not lint

# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps>= &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 2:
# line 50 "parser.y"
{ } break;
case 3:
# line 52 "parser.y"
{  code(STOP); YYACCEPT;} break;
case 4:
# line 53 "parser.y"
{ YYABORT; } break;
case 10:
# line 63 "parser.y"
{yypvt[-0].sym->ttype=FUNCTION; define(yypvt[-0].sym, progp);
				  beginlocal(); farglevel = 0; autolevel = 0;} break;
case 11:
# line 65 "parser.y"
{closelocal();} break;
case 12:
# line 67 "parser.y"
{ code((Inst)autolevel);} break;
case 13:
# line 68 "parser.y"
{ yypvt[-10].sym->local = endlocal(); code(nothing);
			  code(funcret);} break;
case 14:
# line 72 "parser.y"
{ yyval.narg = 0;} break;
case 15:
# line 73 "parser.y"
{ yyval.narg = 1; yypvt[-0].sym->ttype = FORMALARG;
				 yypvt[-0].sym->loc = farglevel++; } break;
case 16:
# line 75 "parser.y"
{ yyval.narg = yypvt[-2].narg + 1; yypvt[-0].sym->ttype = FORMALARG;
				yypvt[-0].sym->loc = farglevel++; } break;
case 17:
# line 79 "parser.y"
{yyval.narg = 0;} break;
case 18:
# line 80 "parser.y"
{openlocal();} break;
case 19:
# line 80 "parser.y"
{ closelocal();} break;
case 20:
# line 83 "parser.y"
{yyval.narg = 0;} break;
case 21:
# line 84 "parser.y"
{yypvt[-0].sym->ttype = AUTOVAR; yypvt[-0].sym->loc = autolevel++; } break;
case 22:
# line 85 "parser.y"
{yypvt[-0].sym->ttype = AUTOVAR; yypvt[-0].sym->loc = autolevel++; } break;
case 23:
# line 87 "parser.y"
{yyval.narg = 0;} break;
case 24:
# line 88 "parser.y"
{yyval.narg = 1;} break;
case 25:
# line 89 "parser.y"
{yyval.narg = yypvt[-2].narg + 1;} break;
case 26:
# line 91 "parser.y"
{yyval.inst = yypvt[-3].inst;} break;
case 27:
# line 93 "parser.y"
{yyval.inst = 0; code(zero);} break;
case 28:
# line 94 "parser.y"
{yyval.inst = 0;} break;
case 29:
# line 96 "parser.y"
{yyval.inst = 0; code(nothing);} break;
case 30:
# line 97 "parser.y"
{yyval.inst = 0;} break;
case 31:
# line 99 "parser.y"
{yyval.inst = yypvt[-1].inst;} break;
case 32:
# line 102 "parser.y"
{yyval.inst = code(tpvarpush); code((Inst)yypvt[-0].sym);} break;
case 33:
# line 103 "parser.y"
{code(tpindexpush); yyval.inst = yypvt[-1].inst;} break;
case 34:
# line 104 "parser.y"
{code(tpsubrangepush); yyval.inst = yypvt[-1].inst;} break;
case 35:
# line 105 "parser.y"
{code(tpmemberpush); code((Inst)yypvt[-0].rbf); yyval.inst = yypvt[-1].inst;} break;
case 36:
# line 107 "parser.y"
{ code(tpassign); yyval.inst = yypvt[-2].inst; } break;
case 37:
# line 109 "parser.y"
{  yyval.inst = progp; } break;
case 38:
# line 111 "parser.y"
{  code(STOP); yyval.inst = progp; } break;
case 39:
# line 113 "parser.y"
{ yyval.inst = progp; } break;
case 41:
# line 116 "parser.y"
{  yyval.inst = code(noop); } break;
case 42:
# line 117 "parser.y"
{ code(popStack);} break;
case 43:
# line 118 "parser.y"
{ yyval.inst = code(nothing); code(funcret);} break;
case 44:
# line 120 "parser.y"
{ yyval.inst=yypvt[-1].inst; code(funcret); } break;
case 45:
# line 121 "parser.y"
{
		(yypvt[-5].inst)[1] = (Inst)yypvt[-1].inst;    /* body of loop */
		(yypvt[-5].inst)[2] = (Inst)yypvt[-0].inst; } break;
case 46:
# line 124 "parser.y"
{
    		(yypvt[-9].inst)[1] = (Inst) yypvt[-1].inst;	/* body */
		(yypvt[-9].inst)[2] = (Inst) yypvt[-0].inst;	/* end */
		(yypvt[-9].inst)[3] = (Inst) yypvt[-7].inst;	/* initialization */
		(yypvt[-9].inst)[4] = (Inst) yypvt[-5].inst;	/* condition */
		(yypvt[-9].inst)[5] = (Inst) yypvt[-3].inst;	/* iteration */
		} break;
case 47:
# line 131 "parser.y"
{     /* else-less if */
		(yypvt[-5].inst) [1] = (Inst)yypvt[-1].inst;    /* thenpart */
		(yypvt[-5].inst) [3] = (Inst)yypvt[-0].inst; } break;
case 48:
# line 134 "parser.y"
{      /* if with else */
		(yypvt[-8].inst) [1] = (Inst)yypvt[-4].inst;    /* thenpart */
		(yypvt[-8].inst) [2] = (Inst)yypvt[-1].inst;    /* elsepart */
		(yypvt[-8].inst) [3] = (Inst)yypvt[-0].inst; } break;
case 49:
# line 138 "parser.y"
{ yyval.inst = yypvt[-1].inst; } break;
case 50:
# line 140 "parser.y"
{ yyval.inst = code(one); code(STOP); } break;
case 51:
# line 141 "parser.y"
{ yyval.inst = yypvt[-0].inst; code(STOP); } break;
case 52:
# line 143 "parser.y"
{ yyval.inst = code(forcode); code2(STOP,STOP);
			 code3(STOP,STOP,STOP); } break;
case 53:
# line 146 "parser.y"
{ yyval.inst = code3(whilecode,STOP,STOP); } break;
case 54:
# line 148 "parser.y"
{ yyval.inst = code(ifcode); code3(STOP,STOP,STOP); } break;
case 55:
# line 151 "parser.y"
{ yyval.inst = code2(datapush, (Inst)yypvt[-0].sym);} break;
case 56:
# line 152 "parser.y"
{ yyval.inst = code(lastindex);} break;
case 57:
# line 153 "parser.y"
{ code(tpeval); yyval.inst = yypvt[-0].inst; } break;
case 58:
# line 155 "parser.y"
{ code(dupTOSeval); code(incr);
				code(tpassign); yyval.inst = yypvt[-0].inst; } break;
case 59:
# line 158 "parser.y"
{ code(dupTOSeval); code(swapTOS);
			  code(dupTOSeval); code(incr);
			  code(tpassign); code(popStack); yyval.inst = yypvt[-1].inst;} break;
case 60:
# line 162 "parser.y"
{ code(dupTOSeval); code(decr);
				code(tpassign); yyval.inst = yypvt[-0].inst; } break;
case 61:
# line 165 "parser.y"
{ code(dupTOSeval); code(swapTOS);
			  code(dupTOSeval); code(decr);
			  code(tpassign); code(popStack); yyval.inst = yypvt[-1].inst;} break;
case 63:
# line 169 "parser.y"
{ yyval.inst = code2(varread, (Inst)yypvt[-1].sym);} break;
case 64:
# line 171 "parser.y"
{ yyval.inst = yypvt[-3].inst; code(call);  code((Inst)yypvt[-4].sym); code((Inst)yypvt[-1].narg);} break;
case 65:
# line 173 "parser.y"
{ yyval.inst = yypvt[-3].inst; code(bltin); code((Inst)yypvt[-4].sym->spk.u.ptr);
			code((Inst)yypvt[-1].narg); } break;
case 66:
# line 175 "parser.y"
{ yyval.inst = yypvt[-1].inst; } break;
case 67:
# line 176 "parser.y"
{ code(mod); } break;
case 68:
# line 177 "parser.y"
{ code(add); } break;
case 69:
# line 178 "parser.y"
{ code(sub); } break;
case 70:
# line 179 "parser.y"
{ code(mul); } break;
case 71:
# line 180 "parser.y"
{ code(divop); } break;
case 72:
# line 181 "parser.y"
{ code(power); } break;
case 73:
# line 182 "parser.y"
{ yyval.inst=yypvt[-0].inst; code(negate); } break;
case 74:
# line 183 "parser.y"
{ code(gt); } break;
case 75:
# line 184 "parser.y"
{ code(ge); } break;
case 76:
# line 185 "parser.y"
{ code(lt); } break;
case 77:
# line 186 "parser.y"
{ code(le); } break;
case 78:
# line 187 "parser.y"
{ code(eq); } break;
case 79:
# line 188 "parser.y"
{ code(ne); } break;
case 80:
# line 189 "parser.y"
{ code(and); } break;
case 81:
# line 190 "parser.y"
{ code(or); } break;
case 82:
# line 191 "parser.y"
{ yyval.inst = yypvt[-0].inst; code(not); } break;
case 83:
# line 192 "parser.y"
{ yyval.inst = code2(datapush, (Inst)yypvt[-0].sym); } break;
# line 148 "/usr/lib/yaccpar"

		}
		goto yystack;  /* stack new state and value */

	}
