#ifndef SUPPORT_H
#define SUPPORT_H

typedef enum {
    Idle,
    Compiling,
    Executing
} UfhStateType;

extern UfhStateType UfhState;

char*	emalloc(unsigned int n);
void	execerror(...);
void	yyerror(char *s);
int	strcasecmp(const char* a, const char* b);

extern char*	progname;
extern int	lineno;
extern char*	infile;
extern char*	LastPushedVar;

#endif
