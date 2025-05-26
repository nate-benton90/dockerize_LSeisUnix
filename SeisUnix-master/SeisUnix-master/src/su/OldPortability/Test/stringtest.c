#include "../../include/cwpdefs.h"

int mystrlen(s)
string s;
{
	string base = s;

	while ( *s++ ) ;

	return (int) (--s - base);
}

int mystrchr(s, c)
string s;
char c;
{
	string base;

	for ( base = s - 1; *s && *s != c; ++s ) ;

	return *s ? (int) (s - base) : -1;
}

int mystrrchr(s, c)
string s;
char c;
{
	string end;
	int count = mystrlen(s);

	for ( end = s + count; count && *end != c; --end, --count ) ;

	return *end == c ? (int) (end - s + 1) : -1;
}

#ifdef TEST
main()
{
	char c1 = 'f';
	char c2 = 's';
	char c3 = 'd';
	char c4 = 'x';
	string s0 = "";
	string s1 = "firstsecond";
	string s2 = "f";

	printf("length of \"%s\" = %d\n", s0, mystrlen(s0));
	printf("length of \"%s\" = %d\n", s1, mystrlen(s1));
	printf("length of \"%s\" = %d\n", s2, mystrlen(s2));

	printf("index of \"%c\" in \"%s\" = %d\n", c1, s1, mystrchr(s1, c1));
	printf("index of \"%c\" in \"%s\" = %d\n", c2, s1, mystrchr(s1, c2));
	printf("index of \"%c\" in \"%s\" = %d\n", c3, s1, mystrchr(s1, c3));
	printf("index of \"%c\" in \"%s\" = %d\n", c4, s1, mystrchr(s1, c4));

	printf("rindex of \"%c\" in \"%s\" = %d\n", c1, s1, mystrrchr(s1, c1));
	printf("rindex of \"%c\" in \"%s\" = %d\n", c2, s1, mystrrchr(s1, c2));
	printf("rindex of \"%c\" in \"%s\" = %d\n", c3, s1, mystrrchr(s1, c3));
	printf("rindex of \"%c\" in \"%s\" = %d\n", c4, s1, mystrrchr(s1, c4));
	
	return SUCCEED;
}
#endif
