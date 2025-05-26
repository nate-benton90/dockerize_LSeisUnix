/* Make some test traces */

#define	STDOUT	1
#define DECAY	1/8
main()
{
	int i, j, ns=64, ntr=32;
	float x;

	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			if (i == ns/4) x = 1.0;
			if (i == ns/1) x = 1.0*DECAY;
			if (i == 3*ns/4) x = 1.0*DECAY*DECAY;
			else x = 0.0;
			write(STDOUT, &x, sizeof(float));
		}
	}
}
