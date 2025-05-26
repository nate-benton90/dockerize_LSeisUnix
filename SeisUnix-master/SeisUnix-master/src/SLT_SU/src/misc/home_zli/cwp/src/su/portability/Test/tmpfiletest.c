#include "../../include/cwp.h"

FILE *mytmpfile()
{
	static char t[] = "/usr/tmp/XXXXXX";
	char *tempname, *mktemp();
	FILE *fp, *fopen();

	tempname = mktemp(t);
	if (NULL == (fp = fopen(tempname, "w+")))
		err("%s: fopen failed", __FILE__);

	if (-1 == unlink(tempname))
		err("%s: unlink failed", __FILE__);

	return fp;
}

#ifdef TEST
main()
{
	FILE *temp;
	char buf[100];

	if (NULL == (temp = mytmpfile()))
		err("%s: tmpfile failed", __FILE__);

	fputs("Some data written to a temporary file.\n", temp);
	rewind(temp);
	fgets(buf, 100, temp);
	fputs(buf, stdout);

	return 0;
}
#endif
