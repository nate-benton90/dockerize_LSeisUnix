/* Make some test traces */

#define	STDOUT	1
main()
{
	int i, j, npoints=6, ncurves=2;
	float x, y;

	for (j = 0; j < ncurves; ++j) {
		write(STDOUT, &npoints, sizeof(int));
		for (i = 0; i < npoints; ++i) {
			x = (float) 10*j + (float) i;
			y = (float) i;
			write(STDOUT, &x, sizeof(float));
			write(STDOUT, &y, sizeof(float));
		}
	}
}
