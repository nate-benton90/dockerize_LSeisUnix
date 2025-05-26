/* segc.h - include file for SEGC traces
 *
 * declarations for:
 *	typedef struct {} segc - binary identification header.
 *	                         
 *
 * Note:
 *	Bit Fields in the binary header assume bits are assigned from left
 *      to right within the data word.
 *
 * Reference:
 *	E. P. Meiners, L. L. Lenz, A. E. dalby, J. M. Hornsby,
 *      "Special Report: Recommended Standards for Digital Tape Formats",
 *	Geophysics, vol. 37, no. 1 (February 1972), P. 36-44.
 *	
 * $Author: kbierbau $
 * $Source: /NeXTMount/usr/local/src/su/include/RCS/segc.h,v $
 * $Revision: $
 */ 

#ifndef SEGC_H
#define SEGC_H

#define	BINARYC		24	/* Bytes in the binary ident. header	*/

typedef struct segc		/* segc - identification header and data 
				block */
{				
	unsigned int filen:16;	/* 4 digit file number */
	
	unsigned int form:16;	/* 4 digit format code */
	
	unsigned int datum:48;	/* 12 digit data identification constants
				-date, line no., reel no., etc. */
	
	unsigned int numb:12;	/* number of bytes per data scan */
	
	unsigned int inter:4;	/* sample interval in integral number of
				miliseconds */
	
	unsigned int manu:8;	/* 2 digit maufacturer's code */
	
	unsigned int serid:24;	/* 6 digit equipment serial number */
	
	unsigned int lenth:8;	/* length of record in seconds - code 00
				indicates continuous recording */
	
	unsigned int gainm:4;	/* amplifier gain control mode:
				8 = binary gain
				4 = programmed gain
				2 = ganged AGC
				1 = individual AGC
				9 = floating point gain control
				*/
				
	unsigned int type:4;	/* type of record:
				8 = shot
				4 = shot bridle
				2 = test
				1 = other
				*/
				
	unsigned int lowst:8;	/* 2 digit low-cut filter setting */
	
	unsigned int lowsp:4;	/* low-cut filter slope setting in db/oct */
	
	unsigned int zero:4;	/* all zeros */
	
	unsigned int hihst:12;	/* 3 digit high-cut filter setting */
	
	unsigned int hihsp:4;	/* high-cut filter slope setting in db/oct */	
	
	unsigned int specl:8;	/* 2 digit special filter setting - rejection
	    			or other */
				
	unsigned int alias:4;	/* alias filter setting - 1, 2, 3, or 4 
				indicates sample rate of alias filter */
				
	unsigned int gainc:4;	/* common gain constant */
														 
};

#endif
