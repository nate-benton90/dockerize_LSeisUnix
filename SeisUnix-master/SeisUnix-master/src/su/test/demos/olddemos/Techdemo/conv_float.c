/* conv_float - convert between 32 bit IBM and IEEE floating numbers
 *
 * Credits:
 *	CWP: Brian
 *
 * Parameters:
 *    from	- input vector
 *    to	- output vector, can be same as input vector
 *    len	- number of floats in vectors
 *    type	- conversion type
 *
 * Notes:
 *	Up to 3 bits lost on IEEE -> IBM
 *
 *	IBM -> IEEE may overflow or underflow, taken care of by 
 *	substituting large number or zero
 *
 *	Byte swapping branches have been inserted to ease rewriting
 *	this machine dependent subroutine for other machines (eg. VAX).
 *
 *----------------------------------------------------------------------
 * Permission to use the SU programs is granted.  Courteous users will
 * retain this acknowlegment of origins at the Stanford Exploration
 * Project, Stanford University and the Center for Wave Phenomena, 
 * Colorado School of Mines.   Inquiries/bug reports should be sent to:
 * Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 * Golden, CO 80014
 *----------------------------------------------------------------------
 *
 * $Author: jkc $
 * $Source: /src/su/lib/RCS/conv_float.c,v $
 * $Revision: 1.4 $ ; $Date: 88/05/17 21:57:15 $
*/


/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/lib/RCS/conv_float.c,v $";
static char revid[] =
	"   $Revision: 1.4 $ ; $Date: 88/05/17 21:57:15 $";



#define	IBMtoIEEE	1
#define IEEEtoIBM	2


void conv_float(from,to,len,type)
char *from, *to;
int len, type;
{

	unsigned fconv, fsign, fexpn;
	register unsigned fmant;
	register int t;
	register char *bp = (char *) &fconv;

	if (len <= 0) return;

	switch(type) {
	case IEEEtoIBM:
		while (len--) {

			/* Load. Swap bytes if necessary for Vax */
			bp[0] = from[0];
			bp[1] = from[1];
			bp[2] = from[2];
			bp[3] = from[3];
			from += 4;

			fsign = (unsigned) 0x80000000 & fconv;
			fexpn = (unsigned) 0x7f800000 & fconv;
			fmant = ((unsigned) 0x007fffff & fconv) |
			 	(unsigned) 0x00800000;

			if (!fexpn) fconv = 0;
			else {

				t = (int) (fexpn >> 23) - 128;
				while (t & 0x3) {
					++t;
					fmant >>= 1;
				}
			
				fexpn = (unsigned) ((t>>2) + 64) << 24;
				fconv = fsign | fexpn | fmant;
			}

			*to++ = bp[0];
			*to++ = bp[1];
			*to++ = bp[2];
			*to++ = bp[3];
		}
		break;
	
	case IBMtoIEEE:
		while (len--) {

			/* Load. Swap bytes if necessary */
			bp[0] = from[0];
			bp[1] = from[1];
			bp[2] = from[2];
			bp[3] = from[3];
			from += 4;

			fsign = (unsigned) 0x80000000 & fconv;
			fexpn = (unsigned) 0x7f000000 & fconv;
			fmant = (unsigned) 0x00ffffff & fconv;

			if (!fmant) fconv = 0;
			else {
				t = (int) (fexpn >> 22) - 256;

				while (!(fmant & 0x00800000)) {
					--t;
					fmant <<= 1;
				}

				/* Check for valid IEEE exponent */

				if (t > 127) fconv = fsign | 0x7ff80000;
				else if (t < -128) fconv = 0;
				else {
					fexpn = ((unsigned) (t+128)) << 23;
					fconv = fsign | fexpn |
						(0x007fffff & fmant);
				}
			}

			*to++ = bp[0];
			*to++ = bp[1];
			*to++ = bp[2];
			*to++ = bp[3];
		}
		break;
	
	default:
		break;
	}
}
