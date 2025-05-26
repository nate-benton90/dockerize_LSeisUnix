/* For suflip */
main()
{
	int i, j, ntr=9, ns=64;
	float x;

	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			x = i + j;
			write(1, &x, sizeof(float));
		}
	}
}
