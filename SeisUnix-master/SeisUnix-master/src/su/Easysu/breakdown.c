#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include "lists.h"

/************************************************
 * 	RequBreakDown				*
 ************************************************/
RequBreakDown(str, template, docfile)
char *str;
FILE *template;
FILE *docfile;
{	char name[9], description[80];
	char *temp;
	int	i;

	i = 0;
	temp = str;
	while(!isspace(*str)) {
		name[i++] = *str++;
	}
	name[i++] = '\n';
	name[i] = '\0';

	while((*str++) != '=');
	while(isspace(*(++str)));

	strcpy(description, str);

	str = temp;

	fputs(name, template);
	fputs(description, template);

	fgets(str, 80, docfile);
	while(comment(str))
		fgets(str, 80, docfile);

	while ((BlankLine(str)) && (secondline(str))) {
		strcpy(str, strip(str));
		fputs(str, template);
		fgets(str, 80, docfile);
		while(comment(str))
			fgets(str, 80, docfile);
	}
	fputs("-\n", template);
}

/************************************************
 *	OptBreakDown				*
 ************************************************/
OptBreakDown(str,template,docfile)
char *str;
FILE *template;
FILE *docfile;
{	char name[9], description[80], value[40];
	char *temp;
	int	i;

	i = 0;
	temp = str;
	while(!isspace(*str)) {
		name[i++] = *str++;
	}
	name[i++] = '\n';
	name[i] = '\0';

	while((*str++) != '=');
	while(isspace(*(++str)));

	i = 0;
	while((*str != '\t') && ((*str != ' ') || (*(str+1) != ' '))) {
		value[i++] = *str++;
	}
	value[i++] = '\n';
	value[i] = '\0';

	while(isspace(*(++str)));
	strcpy(description, str);

	str = temp;

	fputs(name, template);
	fputs(value, template);
	fputs(description, template);

	fgets(str, 80, docfile);
	while(comment(str))
		fgets(str, 80, docfile);
	while ((BlankLine(str)) && (secondline(str))) {
		strcpy(str, strip(str));
		fputs(str, template);
		fgets(str, 80, docfile);
		while(comment(str))
			fgets(str, 80, docfile);
	}
	fputs("-\n", template);
}







