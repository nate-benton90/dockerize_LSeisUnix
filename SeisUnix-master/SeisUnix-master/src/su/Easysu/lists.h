#ifndef EASYSU_LISTS_H
#define EASYSU_LISTS_H

#include <curses.h>

#define StringLength 80 
#define Extension ".tpl\0   "
#define PREFIX "/usr/local/cwp/src/su/Easysu/\0"
#define Blank "\\n\\\0"

char movement();

struct info {
	char stuff[StringLength];
	struct info *next;
};

/*	stores the description of variables	*/
struct list {
	char variable[9];
	char type;
	char value[StringLength];
	char changed;
	struct info *description;
	struct list *next, *prev;
};

/*	One node for each parameter	*/
struct info2 {
	char stuff[2*StringLength];
	struct info2 *next;
};

/*	store header description	*/
FILE *GetJob();
char *strcat();
char *strip();
WINDOW *MakeWindow();
int col, lin;

/* function prototypes */

int compare(char *str1, char *str2);
int indent(char *str);
int noequal(char *str);
char *strip(char *str);
int secondline(char *str);
int BlankLine(char *str);
int comment(char *str);
void RequBreakDown(char *str, FILE *template, FILE *docfile);
void OptBreakDown(char *str, FILE *template, FILE *docfile);

#endif
