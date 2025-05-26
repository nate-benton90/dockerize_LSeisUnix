#include <stdio.h>

main()
{
	FILE *temp1, *temp2, *fopen();
	char file1[L_tmpnam], *file2, buf[100], *tmpnam();
	void rewind();

	(void) tmpnam(file1);

	if (NULL == (temp1 = fopen(file1, "w+")))
		fprintf(stderr, "%d: fopen failed", __LINE__);

	fputs("Some data written to the first temporary file.\n", temp1);
	rewind(temp1);
	fgets(buf, 100, temp1);
	fputs(buf, stdout);

	if (-1 == unlink(file1))
		fprintf(stderr, "%d: unlink failed", __LINE__);



	file2 = tmpnam(NULL);

	if (NULL == (temp2 = fopen(file2, "w+")))
		fprintf(stderr, "%d: fopen failed", __LINE__);

	fputs("Some data written to the second temporary file.\n", temp2);
	rewind(temp2);
	fgets(buf, 100, temp2);
	fputs(buf, stdout);

	if (-1 == unlink(file2))
		fprintf(stderr, "%d: unlink failed", __LINE__);

	return 0;
}
