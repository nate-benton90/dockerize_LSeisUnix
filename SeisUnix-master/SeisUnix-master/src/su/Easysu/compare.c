#include <strings.h>
#include <ctype.h>
#include "lists.h"

/********************************************************
 *	Compare returns a 0 if str2 is the first	*
 *	part of str1.  It returns a 1 if any character	*
 *	doesn't match.					*
 ********************************************************/
compare(str1, str2)
char *str1, *str2;
{	

	while(*str2) {
		if ((*str1++) != (*str2++)) return(1);
	}

	return(0);
}

/*******************************************************
 *       Indent determines if str was indented         *
 *       in the file.  Indent returns 1 if str         *
 *       was indented.  0 otherwise	               *
 *******************************************************/
indent(str)
char *str;
{

	if(isspace(*str)) return(1);
	else return(0);

}

/******************************************************
 *      noequal decides if there is an = symbol       *
 *      in str, if there is return 0 else return 1    *
 ******************************************************/
noequal(str)
char *str;
{
	while (*str) {
		if ((*str++) == '=') return(0);	 
	}
	return(1);
}

/****************************************************
 *	strip removes the leading blanks from a     *
 *      string.  Also removes the string \n\        *
 *      from the end.                               *
 ****************************************************/
char *strip(str)
char *str;
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

/************************************************
 *	Secondline returns a 1 if the str	*
 *	is an extra line of description and	*
 *	a 0 is it is not.			*
 ************************************************/
secondline(str)
char *str;
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

/********************************************************
 *	Blankline returns a 0 if the line is only	*
 *	a \n\ and a one if there is more there.		*
 ********************************************************/
BlankLine(str)
char *str;
{	char temp[StringLength];
	strcpy(temp, str);
	if(compare(strip(temp), Blank)) return(1);
	else return(0);
}

/************************************************
 *       checks to see if str is a comment	*
 *       if it is then comment returns 1	*
 *	 otherwise return 0			*
 ************************************************/

comment(str)
char *str;
{
	char temp[80];

	strcpy(temp,str);
	strcpy(temp, strip(temp));

	if ((temp[0] == '.') && (temp[1] == '.') && (temp[2] == '.'))
		return (1);
	else return (0);
}


