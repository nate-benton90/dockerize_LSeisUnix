/*
*	loadgraph.h
*
*/

#include "hash.h"
#include "dsulib.h"

#define  MAXARGS 200
#define  ON     1
#define  OFF    0
#define  DOCOMMAND    1
#define  DOSTOP    0

/* 
	Types definitions 
*/

/* 
  Return types
	T_WORD	argument or filename
	T_BAR	Pipe symbol (|)
	T_AMP	Ampersand symbol (&)
	T_SEMI	semicolon symbol (;)
	T_GT	Greater than symbol (>)
	T_GTGT	Greater than symbol (>>)
	T_LT	Less than symbol (<)
	T_NL	Newline
	T_EOF	Eon of file
	T_DONE	Done
*/
/* 
  STATES
	NEUTRAL	Starting state
	GTGT	Next character migth be >
	INQUOTE	A quoted was was started
	INWORD	A word is in progress
*/

typedef	enum{	T_WORD, T_BAR, T_AMP, T_SEMI, T_GT, T_GTGT,
		T_LT, T_NL, T_EOF, T_ERROR, T_DONE} TOKEN;

typedef	enum{	C_NODE, C_ASSG} CMD_TYPE; 

/*
	Forward declarations
*/

static TOKEN GetToken(char *word, HASH *HashTable);
static TOKEN GetNode(TASK T, HASH *HashTable);
void	InsertArgv(HASH *HashTAble, char *RealArgv[], int RealArgc);
/* void ExecuteCmd(SEIS_NODE *head, HASH *HashTable); */
