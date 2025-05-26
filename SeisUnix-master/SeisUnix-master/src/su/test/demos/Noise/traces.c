#include "cwp.h"
main()
{
	float x;	/* trace value       */
	int ns = 512;	/* number of samples */
	int ntr = 32;	/* number of traces  */
	int	n1 = 96,  n2 = 160, n3 = 224,
		n4 = 288, n5 = 352, n6 = 416; /* depths in samples */
	float	r1 = 0.2, r2 = -.4, r3 = 0.8,
		r4 = -.8, r5 = 0.4, r6 = -.2; /* reflection coeffs */

	{ int j;
	for (j = 0; j < ntr; j++) {
		int i;
		for (i = 0; i < ns; i++) {
			x = 0.0;
			if (i == n1) x = r1;
			if (i == n2) x = r2;
			if (i == n3) x = r3;
			if (i == n4) x = r4;
			if (i == n5) x = r5;
			if (i == n6) x = r6;
			fwrite(&x, FSIZE, 1, stdout);
		}
	} }
}
