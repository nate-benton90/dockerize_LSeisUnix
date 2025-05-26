/*
 * $Author: jkc $
 * $Source: /src/su/portability/RCS/tmpfile.c,v $
 * $Revision: 1.3 $ ; $Date: 88/12/03 09:47:49 $
 */

#include <stdio.h>

FILE *tmpfile()
{
	static char t[] = "/usr/tmp/XXXXXX";
	char *tempname, *mktemp();
	FILE *fp, *fopen();

	tempname = mktemp(t);
	if (NULL == (fp = fopen(tempname, "w+")))
		fprintf(stderr, "%s: %d: fopen failed", __FILE__, __LINE__);

	if (-1 == unlink(tempname))
		fprintf(stderr, "%s: %d: unlink failed", __FILE__, __LINE__);

	return fp;
}

#ifdef TEST
main()
{
	FILE *temp;
	char buf[100];
	char *msg = "Some data written to a temporary file.\n";

	/* Tell the user what's going on */
	printf("This program should repeat the message:\n\n");
	printf("%s", msg);
	printf("\nThe message is repeated using tmpfile().\n\n\n\n");



	/* Exercise tmpnam() */
	if (NULL == (temp = tmpfile()))
		fprintf(stderr, "%s: %d: tmpfile failed", __FILE__, __LINE__);

	fputs(msg, temp);
	rewind(temp);
	fgets(buf, 100, temp);
	fputs(buf, stdout);

	return 0;
}
#endif
