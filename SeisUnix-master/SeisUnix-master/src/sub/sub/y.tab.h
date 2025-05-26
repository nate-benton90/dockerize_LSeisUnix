
typedef union  {
	Symbol*		sym;      /* symbol table pointer  */
	Inst*		inst;     /* machine instruction   */
	int     	narg;     /* number of arguments   */
	RBFieldInfo*	rbf;     /* info pointer      	   */
} YYSTYPE;
extern YYSTYPE yylval;
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
