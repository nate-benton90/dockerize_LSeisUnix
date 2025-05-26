/*
 * $Author: jkc $
 * $Source: /src/su/portability/RCS/tmpnam.c,v $
 * $Revision: 1.4 $ ; $Date: 88/12/03 09:47:41 $
 */

#include <stdio.h>

char *tmpnam(s)
char *s;
{
	static char t[] = "/usr/tmp/XXXXXX";
	static char name[L_tmpnam];
	char *mktemp();

	strcpy(name, mktemp(t));
	if (s) {
		strcpy(s, name);
		return s;
	} else {
		return name;
	}
	
}

#ifdef TEST
main()
{
	FILE *temp1, *temp2, *fopen();
	char file1[L_tmpnam], *file2, buf[100];
	char *msg1 = "Some data written to the first temporary file.\n";
	char *msg2 = "Some data written to the second temporary file.\n";
	void rewind();


	/* Tell the user what's going on */
	printf("This program should repeat the messages:\n\n");
	printf("%s", msg1);
	printf("%s", msg2);
	printf("\nThe first message is printed with the tmpnam(file) usage\n");
	printf("and the second with the file = tmpnam(NULL) usage.\n\n\n\n");



	/* First way of using tmpnam() */
	(void) tmpnam(file1);

	if (NULL == (temp1 = fopen(file1, "w+")))
		fprintf(stderr, "%d: fopen failed", __LINE__);

	fputs(msg1, temp1);
	rewind(temp1);
	fgets(buf, 100, temp1);
	fputs(buf, stdout);

	if (-1 == unlink(file1))
		fprintf(stderr, "%d: unlink failed", __LINE__);



	/* Second way of using tmpnam() */
	file2 = tmpnam(NULL);

	if (NULL == (temp2 = fopen(file2, "w+")))
		fprintf(stderr, "%d: fopen failed", __LINE__);

	fputs(msg2, temp2);
	rewind(temp2);
	fgets(buf, 100, temp2);
	fputs(buf, stdout);

	if (-1 == unlink(file2))
		fprintf(stderr, "%d: unlink failed", __LINE__);

	return 0;
}
#endif
