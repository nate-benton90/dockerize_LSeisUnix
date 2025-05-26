/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* Compute maximum and minimum sizes for integer types (quick and dirty)
 * These results will be in limits.h on most systems
 *
 * Credits:
 *	CWP: Jack
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
*/

main()
{
	short maxshort();
	long maxlong();
	int maxint();
	unsigned short maxushort();
	unsigned long maxulong();
	unsigned int maxuint();

	printf("max short =  %d\n", maxshort());
	printf("min short = %d\n", -(1 + maxshort()));

	printf("max long =  %ld\n", maxlong());
	printf("min long = %ld\n", -(1 + maxlong()));

	printf("max int =  %d\n", maxint());
	printf("min int = %d\n", -(1 + maxint()));

	printf("max unsigned short = %u\n", maxushort());

	printf("max unsigned long = %lu\n", maxulong());

	printf("max unsigned int = %u\n", maxuint());
}

short maxshort()
{
	short i = 1, j;
	while (i > 0) {
		j = i;
		i <<= 1;
	}
	return(j - 1 + j);
}

long maxlong()
{
	long i = 1, j;
	while (i > 0) {
		j = i;
		i <<= 1;
	}
	return(j - 1 + j);
}

int maxint()
{
	int i = 1, j;
	while (i > 0) {
		j = i;
		i <<= 1;
	}
	return(j - 1 + j);
}

unsigned short maxushort()
{
	unsigned short i = 1, j = 0;
	while (i > j) {
		j = i;
		i <<= 1;
	}
	return(j - 1 + j);
}

unsigned long maxulong()
{
	unsigned long i = 1, j = 0;
	while (i > j) {
		j = i;
		i <<= 1;
	}
	return(j - 1 + j);
}

unsigned int maxuint()
{
	unsigned int i = 1, j = 0;
	while (i > j) {
		j = i;
		i <<= 1;
	}
	return(j - 1 + j);
}
