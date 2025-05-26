/* These traces can be used to verify a bug somewhere in vertwig/suwig
   by:
   maketraces | suaddhead ns=64 | wig | tube
*/

#define	STDOUT	1
main()
{
	int i, j, ns=64, ntr=32;
	float x;

	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			x = 0.0;
			if (i == 0) x = 3.0;
			if (i == 1) x = 1.0;
			if (i == ns/2-1) x = 15.0;
			if (i == ns/2) x = 20.0;
			if (i == ns/2+1) x = 15.0;
			write(STDOUT, &x, sizeof(float));
		}
	}
}
