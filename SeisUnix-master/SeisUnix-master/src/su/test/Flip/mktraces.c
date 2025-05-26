main()
{
	int i, j, ntr=9, ns=64;
	float x;

	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			x = 0.0;
			if (i == 1 || i == ns-16) x = 1.0;
			write(1, &x, sizeof(float));
		}
	}
}
