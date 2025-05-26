/*
 * $Author: jkc $
 * $Source: /src/su/portability/RCS/strchr.c,v $
 * $Revision: 1.3 $ ; $Date: 88/11/29 23:02:22 $
 */

#include <stdio.h>

char *strchr(s, c)
char *s;
char c;
{
	while ( *s && *s++ != c ) ;

	return (*--s == c) ? s : NULL;
}


#ifdef TEST
main()
{
	char c1 = 'f';
	char c2 = 's';
	char c3 = 'd';
	char c4 = 'x';
	char *s = "firstsecond";
	char *t = "";

	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s, c1, strchr(s, c1));
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s, c2, strchr(s, c2));
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s, c3, strchr(s, c3));
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s, c4, strchr(s, c4));
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						t, c4, strchr(t, c4));

	return 0;
}
#endif
