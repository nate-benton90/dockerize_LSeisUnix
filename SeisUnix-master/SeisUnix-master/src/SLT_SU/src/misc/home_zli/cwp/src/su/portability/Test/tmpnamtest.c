#include "../../include/cwp.h"

char *mytmpnam(s)
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

	mytmpnam(file1);

	if (NULL == (temp1 = fopen(file1, "w+")))
		err("couldn't open first temp file");

	fputs("Some data written to the first temporary file.\n", temp1);
	rewind(temp1);
	fgets(buf, 100, temp1);
	fputs(buf, stdout);

	if (-1 == unlink(file1))
		err("unlink of first temp file failed");



	file2 = mytmpnam(NULL);

	if (NULL == (temp2 = fopen(file2, "w+")))
		err("couldn't open second temp file");

	fputs("Some data written to the second temporary file.\n", temp2);
	rewind(temp2);
	fgets(buf, 100, temp2);
	fputs(buf, stdout);

	return 0;
}
#endif
