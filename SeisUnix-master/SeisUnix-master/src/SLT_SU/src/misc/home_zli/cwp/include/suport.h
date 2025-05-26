/* suport.h - include file for non-portable items
 * ENVIRONMENT-DEPENDENT - ADJUST TO LOCAL SYSTEM
 *
 * $Author: jkc $
 * $Source: /usr/local/cwp/src/su/include/RCS/suport.h,v $
 * $Revision: 1.7 $ ; $Date: 91/01/08 19:45:29 $
 */

#ifndef SUPORT_H
#define SUPORT_H

/* Note: CWPBIN defined in cwp.h; non-ansi stuff in Portability */

/* Pager of choice--usually more or pg */
#define		PAGE_PROGRAM	"pg"

/* The following must define actual tape devices defined in the
 * system's /dev directory; often mt0 and rmt0 will do.
 */
#define		MTDEVICE	"/dev/mt0"
#define		RMTDEVICE	"/dev/rmt0"

/* If unsigned abbreviations not available, remove comments */
/* typedef unsigned char	uchar;  */
/* typedef unsigned short	ushort; */
/* typedef unsigned long	ulong;  */
/* typedef unsigned int	uint */

/* define floating point values */

#define FLT_EPSILON	1.192092895507815e-7F
/* #define FLT_MAX	    3.4028234663852886e+38F */ 


#endif
