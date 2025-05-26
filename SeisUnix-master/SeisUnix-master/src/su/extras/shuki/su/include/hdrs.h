/*
 * hdrs.h - include file for su header arrays
 */

#ifndef INCLUDE_HDR_H
#define INCLUDE_HDR_H

#define HDRBYTES	100	/* Bytes in the trace header */
#define TR_NK		25	/* Number of key trace header words */
static struct trhdr_tag {
	char *key;	char *type;	int offs;
} trhdr[] = {
	   "tracl",		"i",		0,
	   "tracr",		"i",		4,
	    "fldr",		"i",		8,
	     "cdp",		"i",		12,
	    "cdpt",		"i",		16,
	    "trid",		"i",		20,
	     "nvs",		"i",		24,
	     "nhs",		"i",		28,
	  "offset",		"i",		32,
	      "sx",		"i",		36,
	      "sy",		"i",		40,
	      "gx",		"i",		44,
	      "gy",		"i",		48,
	   "sstat",		"i",		52,
	   "gstat",		"i",		56,
	   "tstat",		"i",		60,
	    "muts",		"i",		64,
	    "mute",		"i",		68,
	     "_ns",		"i",		72,
	     "_dt",		"i",		76,
	    "gain",		"i",		80,
	  "ungpow",		"f",		84,
	 "unscale",		"f",		88,
	     "ntr",		"i",		92,
	    "mark",		"i",		96,
};
#define BH_NK		33	/* Number of key binary header words */
static struct bhdr_tag {
	char *key;	char *type;	int offs;
} bhdr[] = {
	   "jobid",		"i",		0,
	    "lino",		"i",		4,
	    "reno",		"i",		8,
	   "ntrpr",		"i",		12,
	    "nart",		"i",		16,
	      "dt",		"i",		20,
	     "dto",		"i",		24,
	      "ns",		"i",		28,
	     "nso",		"i",		32,
	  "format",		"i",		36,
	    "fold",		"i",		40,
	   "tsort",		"i",		44,
	  "vscode",		"i",		48,
	    "hsfs",		"i",		52,
	    "hsfe",		"i",		56,
	   "hslen",		"i",		60,
	   "hstyp",		"i",		64,
	    "schn",		"i",		68,
	   "hstas",		"i",		72,
	   "hstae",		"i",		76,
	  "htatyp",		"i",		80,
	   "hcorr",		"i",		84,
	   "bgrcv",		"i",		88,
	    "rcvm",		"i",		92,
	   "mfeet",		"i",		96,
	     "nvs",		"i",		100,
	     "ntr",		"i",		104,
	      "id",		"i",		108,
	   "esize",		"i",		112,
	 "version",		"i",		116,
	    "name",		"c",		120,
	    "area",		"c",		136,
	  "client",		"c",		152,
};
#endif INCLUDE_HDR_H
