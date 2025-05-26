/* Make some test traces -- for Holverson Deno paper */
/* a.out | suaddhead ns=64 | sufft | suamp >amptraces */

#define	STDOUT	1
main()
{
	int i, j, ns=64, ntr=8;
	float x;

	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			if (i == 0 || i == 2) x = 1.0;
			else x = 0.0;
			write(STDOUT, &x, sizeof(float));
		}
	}
}
