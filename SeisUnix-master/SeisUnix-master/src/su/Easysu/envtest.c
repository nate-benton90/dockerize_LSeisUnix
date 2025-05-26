#include <stdio.h>
#include <ctype.h>

char *getenv();

TermGet(c, l)
int *c, *l;
{	char *env, var[4];
	char *pnt, var2[4];
	int i;

	env = getenv("TERMCAP");


	pnt = env;
	while((*pnt != 'c') || (*(pnt+1) != 'o') || (*(pnt+2) != '#'))
		pnt++;
	for(i=0; i< 3; i++) pnt++;
	i = 0;
	while(isdigit(*pnt))
		var[i++] = *pnt++;
	var[i] = '\0';
	pnt = env;
	while((*pnt != 'l') || (*(pnt+1) != 'i') || (*(pnt+2) != '#'))
		pnt++;
	for(i=0; i< 3; i++) pnt++;
	i = 0;
	while(isdigit(*pnt))
		var2[i++] = *pnt++;
	var2[i] = '\0';

	*c = atoi(var);
	*l = atoi(var2);
}
