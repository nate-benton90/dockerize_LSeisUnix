/* Make some spike traces for sudemo */

#define	STDOUT	1
#define	UNIT	1.0
#define	DECAY	64.0
main()
{
	int i, j, ns=64, ntr=32;
	float x;

	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			if (i == ns/4) x = UNIT;
			else if (i == ns/2) x = UNIT/DECAY;
			else if (i == 3*ns/4) x = UNIT/(DECAY*DECAY);
			else x = 0.0;
			write(STDOUT, &x, sizeof(float));
		}
	}
}
