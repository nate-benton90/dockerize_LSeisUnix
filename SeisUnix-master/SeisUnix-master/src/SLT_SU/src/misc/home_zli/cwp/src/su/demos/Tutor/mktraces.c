/* Make some test traces */

#define	STDOUT	1
#define DECAY	1/16
main()
{
	int i, j, ns=64, ntr=32;
	float x;

	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			x = 0.0;
			if (i == ns/4) x = 1.0;
			if (i == ns/2) x = 1.0*DECAY;
			if (i == 3*ns/4) x = 1.0*DECAY*DECAY;
			write(STDOUT, &x, sizeof(float));
		}
	}
}
