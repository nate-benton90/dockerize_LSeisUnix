/*
 * ghdr.h - include file for grid offset array
 */

#ifndef GHDR_H
#define GHDR_H

static struct {
	char *key;	char *type;	int offs;
} ghdr[] = {
	   "scale",		"f",		0,
	   "dtype",		"f",		4,
	   "n1",		"f",		8,
	   "n2",		"f",		12,
	   "n3",		"f",		16,
	   "n4",		"f",		20,
	   "n5",		"f",		24,
	   "d1",		"f",		28,
	   "d2",		"f",		32,
	   "d3",		"f",		36,
	   "d4",		"f",		40,
	   "d5",		"f",		44,
	   "o1",		"f",		48,
	   "o2",		"f",		52,
	   "o3",		"f",		56,
	   "o4",		"f",		60,
	   "o5",		"f",		64,
	   "dcdp2",		"f",		68,
	   "dline3",		"f",		72,
	   "ocdp2",		"f",		76,
	   "oline3",		"f",		80,
	   "gmin",		"f",		84,
	   "gmax",		"f",		88,
	   "orient",		"f",		92,
	   "gtype",		"f",		96,
};
#endif
