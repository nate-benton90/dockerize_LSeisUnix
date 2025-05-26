#include <stdio.h>

char *astrchr(s, c)
char *s;
char c;
{
	char t;

	while ( (t = *s) && *s++ != c ) ;

	return t ? --s : NULL;
}


#ifdef TEST
main()
{
	char c1 = 'f';
	char c2 = 's';
	char c3 = 'd';
	char c4 = 'x';
	char *s1 = "firstsecond";
	char *s2 = "";
	int i, n = 1000000;
	long t, time();

	t = time( (long *) 0);
	for (i = 1; i < n; ++i)
		astrchr(s1, c3);
	printf("time is: %d\n", time((long *) 0) - t);

/*
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s1, c1, astrchr(s1, c1));
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s1, c2, astrchr(s1, c2));
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s1, c3, astrchr(s1, c3));
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s1, c4, astrchr(s1, c4));
	printf("portion of \"%s\" starting with \"%c\" is %s\n",
						s2, c4, astrchr(s2, c4));

*/
	return 0;
}
#endif
