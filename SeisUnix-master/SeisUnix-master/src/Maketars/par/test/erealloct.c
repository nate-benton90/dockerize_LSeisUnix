/*
 * Exercise emalloc, erealloc
 * From ANSI C -- A Lexical Guide, page 372
 *
 */

#include "par.h"
#define ERROR 	NULL

string combine(string *a, string *b)
{
	if (NULL == *a) {
		*a = *b;
		*b = NULL;
		return *a;
	} else if (ERROR == *b)
		return *a;
	
	if (ERROR == (*a = (string) erealloc(*a, strlen(*a) + strlen(*b))))
		return ERROR;
	
	return  strcat(*a, *b);
}

/* Copy a string into a malloc'd hole */
string copy(string s)
{
	size_t len;
	string ret;

	if (!(len = strlen(s)))  return ERROR;
	if (ERROR == (ret = (string) emalloc(len)))  return ERROR;

	return strcpy(ret, s);
}

main(void)
{
	string a, b;

	a = copy("A fine string. ");
	b = copy("Another fine string. ");

	puts(combine(&a, &b));

	return EXIT_SUCCESS;
}
