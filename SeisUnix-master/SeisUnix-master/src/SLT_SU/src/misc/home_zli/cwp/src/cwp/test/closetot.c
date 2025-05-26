/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* Test CLOSETO macro in cwp.h */

#include "cwp.h"

main(void)
{
	float x = 1.0;
	float y = 1.0 + 2.0*FLT_EPSILON;
	float z = 1.0 + 0.5*FLT_EPSILON;

	if (CLOSETO(x,y)) printf("CLOSETO bombed\n");
	if (!CLOSETO(x,z)) printf("CLOSETO bombed\n");
	printf("Silence is Golden\n");
	return EXIT_SUCCESS;
}
