#include <stdio.h>
main()
{
	unsigned char i;
	FILE *p;

	p = popen("od -c | more","w");

	for(i=0;i<128;i++)
		fwrite(&i,1,1,p);
	pclose(p);
	exit(0);
}
