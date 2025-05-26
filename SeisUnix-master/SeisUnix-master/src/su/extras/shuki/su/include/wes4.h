/*
 * WGC Code 4 structures
 */

#ifndef INCLUDE_WES4_H
#define INCLUDE_WES4_H

#define	W4_EBCBYTES	3200  /* Bytes in the card image EBCDIC block */
#define	W4_BNYBYTES	400   /* Bytes in the binary coded block	*/
#define W4_NDAT	    16384 /* Arbitrary limit on data array size	*/

typedef struct wes4_tr {

	short int code;	   /* Format code always 4 to indicate WGC code 4 format */
	short int ssi;     /* Sample start index */

	long  int id;

	short int trace;
	short int cdf;

	short int sw;      /* Stack word */
	short int se;      /* Source elevation */

	long  int sx;      /* Source x coordinate */

	long  int sy;      /* Source y coordinate */

	short int sstat;   /* Source static correction */
	short int ge;      /* Geophone elevation */

	long  int gx;      /* Geophone x coordinate */

	long  int gy;      /* Geophone y coordinate */

	short int gstat;   /* Geophone static correction */
	short int offset;  /* Source to Geophone offset */

	float 	  tbf;     /* Trace balance factor */

	float 	  swf;     /* Stack weight factor */

	short int fgv;     /* Fixed gain value */
	short int polyt;   /* polyt = number of times polarity has been
                          reversed in all processing to date */

	short int tfirst;   /* Time of first sample */
	short int undefined1;

	short int sp;      /* Shot point */
	short int mstat;   /* two-way time correction from cdf datum elevation to stacking datum. */

	short int sstation; /* Shot station */
	short int gstation; /* Geophone station */

	short int me;      /* Midpoint elevation */
	short int mstation;/* Midpoint station */

	short int tnzfirst;/* Time of first non-zero sample (mute time) */
	short int undefined2;

	short int ivfc;    /* Input velocity function code */
	short int undefined3;

	short int undefined4;
	short int cdatum;  /* Cdf datum static correction after application */

	long  int fr;      /* Field reel number */

	short int ff;      /* Field file number */
	short int fc;      /* Field channel number */

	short int threed;  /* 3-D line */
	short int undefined5;

	float     shotp; /* 3-D shotpoint */

	short int poffset; /* Perpendicular offset from midpoint to profile */
	short int poaz;    /* Perpendicular offset azimuth */

	char unass[300];

	float data[W4_NDAT];

} Wes4_tr;

#endif INCLUDE_WES4_H
