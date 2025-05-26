/* Make some non-zero phase traces to test suenv */

#include <math.h>

main()
{
	int i, j, ntr=64, ns=64;
	float x, dt = 0.004;
	float w0 = 180.0, t0 = 0.0, alpha = 40.0;

	for (j = 0; j < ntr; j++) {
		for (i = 0; i < ns; i++) {
			x = exp(-alpha*fabs((i*dt - t0))) *
						sin(w0*(i*dt - t0));
			write(1, &x, sizeof(float));
		}
	}
}
