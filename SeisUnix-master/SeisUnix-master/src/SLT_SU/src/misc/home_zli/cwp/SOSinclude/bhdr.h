/*
 * bhdr.h - include file for bhed offset array
 */

#ifndef BHDR_H
#define BHDR_H

static struct {
	char *key;	char *type;	int offs;
} bhdr[] = {
	   "jobid",		"l",		0,
	    "lino",		"l",		4,
	    "reno",		"l",		8,
	   "ntrpr",		"h",		12,
	    "nart",		"h",		14,
	     "hdt",		"h",		16,
	     "dto",		"h",		18,
	     "hns",		"h",		20,
	     "nso",		"h",		22,
	  "format",		"h",		24,
	    "fold",		"h",		26,
	   "tsort",		"h",		28,
	  "vscode",		"h",		30,
	    "hsfs",		"h",		32,
	    "hsfe",		"h",		34,
	   "hslen",		"h",		36,
	   "hstyp",		"h",		38,
	    "schn",		"h",		40,
	   "hstas",		"h",		42,
	   "hstae",		"h",		44,
	  "htatyp",		"h",		46,
	   "hcorr",		"h",		48,
	   "bgrcv",		"h",		50,
	    "rcvm",		"h",		52,
	   "mfeet",		"h",		54,
	   "polyt",		"h",		56,
	    "vpol",		"h",		58,
};
#endif
