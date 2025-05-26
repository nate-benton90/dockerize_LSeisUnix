#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include "lists.h"

#define DEBUG 1

main(argv, argc)
char *argc[];
int argv;
{	char str[80], name[30], *temp, name2[30];
	char otherinfo[10][80];
	int i, next, count;
	struct list node;
	FILE *template, *docfile;
	
	if (argv !=2) {
		printf("syntax: generate <suprog> \n");
		exit(0);
	}	
	strcpy(name, argc[1]);
	strcat(name, Extension);
	if ((template = fopen(name, "r")) != NULL) {
		fclose(template);
		printf("Template file has already been created \n");
		exit(0);
	}		
	strcpy(name2, name);

	for(next=0; next < 10; next++) otherinfo[next][0] = '\0';
	next = 0;
	strcpy(name, PREFIX);
	strcat(name, argc[1]);
	strcat(name, ".c\0");
	if ((docfile = fopen(name, "r")) == NULL) {
		fclose(docfile);
		printf("Unable to find source code to produce template.  \n");
		exit(0);
	}
	template = fopen(name2, "w");

	for (i=0; argc[1][i]; i++) if (isdigit(argc[1][i])) 
					name[i] = argc[1][i];
				   else name[i] = toupper(argc[1][i]);
	name[i] = '\0';
	str[0] = '\0';

	while (compare(str, name)) fgets(str, 80, docfile);

if (DEBUG)
	fprintf(stderr, "got to here");

	strcpy(str, strip(str));
	fputs(str, template);		/* puts the header in */
	fgets(str, 80, docfile);
	while (BlankLine(str)) {
		strcpy(str, strip(str));
		fputs(str, template);
		fgets(str, 80, docfile);
if (DEBUG)
	fprintf(stderr, "got to here: %s.\n", str);
	}


	fputs("Required Parameters:\n", template);


	while ((compare(str, "Required\0")) && (compare(str,
		"Optional\0")) && (compare(str, "\";\0"))){
		 fgets(str, 80, docfile);
if (DEBUG)
	fprintf(stderr, "Looking for whatever. : %s.\n", str);
	}
	if (compare(str, "Required\0") == 0) {
		fgets(str, 80, docfile);
		while(comment(str))
			fgets(str, 80, docfile);
		while(compare(str, "Optional\0")) {
			strcpy(str, strip(str));
			if(compare(str, "none\0")) {
				if (noequal(str)) {
					strcpy(otherinfo[next++], str);
					fgets(str, 80, docfile);
					while(comment(str))
						fgets(str, 80, docfile);
				}
				else RequBreakDown(str, template, docfile);
			}
			else {
				fgets(str, 80, docfile);
				while(comment(str))
					fgets(str, 80, docfile);
				if (BlankLine(str) == 0) 
					strcpy(str, "none\0");
			}
			while(BlankLine(str) == 0) {
				fgets(str, 80, docfile);
				while(comment(str))
					fgets(str, 80, docfile);
			}	
		}	
	}


	fputs("Optional Parameters:\n", template);
	if (compare(str, "\";\0")) {
		fgets(str, 80, docfile);
		while(comment(str))
			fgets(str, 80, docfile);
	}
if (DEBUG)
	fprintf(stderr, "looking for notes1.  : %s.\n", str);

	while((compare(str,"NOTES:\0")) && (compare(str,"\";\0"))
		&& (indent(str))) {
		strcpy(str, strip(str));
		if(compare(str, "none\0")) {
			OptBreakDown(str, template, docfile);
		}
		else {
			strcpy(str, "NOTES:\0");
		}
if (DEBUG)
	fprintf(stderr, "looking for notes2.  : %s.\n", str);
		while(BlankLine(str) == 0){
			fgets(str, 80, docfile);
			while(comment(str))
				fgets(str, 80, docfile);
		}
	}

if (DEBUG)
	fprintf(stderr, "got to here : %s.\n",str);

	while (compare(str, "\";\0")) {
		if (compare(str, "NOTES:\0") == 0) {
			fgets(str, 80, docfile);
if (DEBUG)
	fprintf(stderr, "looking for end : %s.\n",str);
			while (BlankLine(str)) {
				strcpy(otherinfo[next++],strip(str));
				fgets(str,80,docfile);
if (DEBUG)
	fprintf(stderr, "looking for balnkline : %s.\n",str);
			}
		}
		fgets(str, 80, docfile);
	}

if (DEBUG)
	fprintf(stderr, "got to end string is: %s.\n", str);

	fputs("Other Information:\n", template);
	for(count=0; count < next; count++) fputs(otherinfo[count], template);

	fputs("END.\n", template);
	fclose(docfile);
	fclose(template);
}


int compare(char *str1, char *str2)
/********************************************************
 *	Compare returns a 0 if str2 is the first	*
 *	part of str1.  It returns a 1 if any character	*
 *	doesn't match.					*
 ********************************************************/
{	

	while(*str2) {
		if ((*str1++) != (*str2++)) return(1);
	}

	return(0);
}

int indent(char *str)
/*******************************************************
 *       Indent determines if str was indented         *
 *       in the file.  Indent returns 1 if str         *
 *       was indented.  0 otherwise	               *
 *******************************************************/
{

	if(isspace(*str)) return(1);
	else return(0);

}

int noequal(char *str)
/******************************************************
 *      noequal decides if there is an = symbol       *
 *      in str, if there is return 0 else return 1    *
 ******************************************************/
{
	while (*str) {
		if ((*str++) == '=') return(0);	 
	}
	return(1);
}

char *strip(char *str)
/****************************************************
 *	strip removes the leading blanks from a     *
 *      string.  Also removes the string \n\        *
 *      from the end.                               *
 ****************************************************/
{
	char *temp;
	int len;

	
	while ((isspace(*str++))){};
	*str--;
	len = strlen(str);
	temp = str+len-4;
	while (isspace(*(--temp)));
	temp++;
	*temp++ = '\n';
	*temp = '\0';

	return(str);

}

int secondline(char *str)
/************************************************
 *	Secondline returns a 1 if the str	*
 *	is an extra line of description and	*
 *	a 0 is it is not.			*
 ************************************************/
{	int count;

	count = 0;
	while((isspace(*str)) && (count < 15)) {
		switch (*str) {
			case ' ' : count++;
				break;
			case '\t' : count += 8;
				    count -= count%8;
				break;
			default : return(0);
		}
		str++;
	}
	if (count >= 15) return(1);
	return(0);
}

int BlankLine(char *str)
/********************************************************
 *	Blankline returns a 0 if the line is only	*
 *	a \n\ and a one if there is more there.		*
 ********************************************************/
{	char temp[StringLength];
	strcpy(temp, str);
	if(compare(strip(temp), Blank)) return(1);
	else return(0);
}


int comment(char *str)
/************************************************
 *       checks to see if str is a comment	*
 *       if it is then comment returns 1	*
 *	 otherwise return 0			*
 ************************************************/
{
	char temp[80];

	strcpy(temp,str);
	strcpy(temp, strip(temp));

	if ((temp[0] == '.') && (temp[1] == '.') && (temp[2] == '.'))
		return (1);
	else return (0);
}


void RequBreakDown(char *str, FILE *template, FILE *docfile)
/************************************************
 * 	RequBreakDown				*
 ************************************************/
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

void OptBreakDown(char *str, FILE *template, FILE *docfile)
/************************************************
 *	OptBreakDown				*
 ************************************************/
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

	fprintf(stderr,"%s", *str);
	exit(0);

	while((*str++) != '=');
	while(isspace(*(++str)));

	i = 0;
	str = temp;
	while((*str != '\t') && ((*str != ' ') || (*(str+1) != ' '))) {
		value[i++] = *(++str);
	}

	while(*str != '\t') {
		value[i++] = *(++str);
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
