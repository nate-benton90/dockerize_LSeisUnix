/* Make some test traces */

#include "par.h"

main()
{
	int fd, i, j, k=0, ns=64, ntr=16;
	float x;
	float buf[8192];

	fd = eopen("traces", O_CREAT|O_TRUNC|O_WRONLY, 666);
	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			if (i == ns/2) buf[k++] = 1.0;
			else buf[k++] = 0.0;
		}
	}
	ewrite(fd, buf, ns*ntr*sizeof(float));
}
