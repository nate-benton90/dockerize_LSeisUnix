#include "cwp.h"

main()
{
	int i, n = 1000000;
	float cpu_used, wall_used, a, b = 1.0;

	cpu_used = cputime();
	for (i = 0, a = 0.0; i < n; ++i)  a = a + b;
	cpu_used = cputime() - cpu_used;
	printf("a = %f  cpu time = %f\n", a, cpu_used);

	wall_used = walltime();
	for (i = 0, a = 0.0; i < n; ++i)  a = a + b;
	wall_used = walltime() - wall_used;
	printf("a = %f  wall time = %f\n", a, wall_used);
}
