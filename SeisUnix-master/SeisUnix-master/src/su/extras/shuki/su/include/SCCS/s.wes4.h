h33628
s 00000/00000/00087
d D 1.3 88/11/15 14:01:21 shuki 3 2
c 
e
s 00011/00008/00076
d D 1.2 88/05/16 06:41:39 shuki 2 1
c 
e
s 00084/00000/00000
d D 1.1 88/05/03 06:49:14 shuki 1 0
c date and time created 88/05/03 06:49:14 by shuki
e
u
U
t
T
I 1
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

D 2
	short int tfirst   /* Time of first sample */
E 2
I 2
	short int tfirst;   /* Time of first sample */
E 2
	short int undefined1;

	short int sp;      /* Shot point */
D 2
	short int mstat;   /* two-way time correction from
				           cdf datum elevation to stacking datum. */
E 2
I 2
	short int mstat;   /* two-way time correction from cdf datum elevation to stacking datum. */
E 2

D 2
	short int sstat;   /* Shot station */
	short int gstat;   /* Geophone station */
E 2
I 2
	short int sstation; /* Shot station */
	short int gstation; /* Geophone station */
E 2

	short int me;      /* Midpoint elevation */
D 2
	short int mstat;   /* Midpoint station */
E 2
I 2
	short int mstation;/* Midpoint station */
E 2

	short int tnzfirst;/* Time of first non-zero sample (mute time) */
	short int undefined2;

	short int ivfc;    /* Input velocity function code */
	short int undefined3;

	short int undefined4;
	short int cdatum;  /* Cdf datum static correction after application */

	long  int fr;      /* Field reel number */

	short int ff;      /* Field file number */
	short int fc;      /* Field channel number */

D 2
	short int threed   /* 3-D line */
E 2
I 2
	short int threed;  /* 3-D line */
E 2
	short int undefined5;

D 2
	float     sthreed; /* 3-D shotpoint */
E 2
I 2
	float     shotp; /* 3-D shotpoint */
E 2

	short int poffset; /* Perpendicular offset from midpoint to profile */
	short int poaz;    /* Perpendicular offset azimuth */
I 2

	char unass[300];

	float data[W4_NDAT];
E 2

} Wes4_tr;

#endif INCLUDE_WES4_H
E 1
