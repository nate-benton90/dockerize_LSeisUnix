/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.                       */

/* limits.h - environmental limits
 * ENVIRONMENT-DEPENDENT - ADJUST TO LOCAL SYSTEM
 *
 * $Author: jkc $
 * $Source: /src/general/include/RCS/limits.h,v $
 * $Revision: 1.8 $ ; $Date: 88/11/26 22:21:10 $
 */

#ifndef LIMITS_H
#define LIMITS_H

#define CHAR_BIT           8	/* number of bits in a char 	*/
#define CHAR_MAX         127	/* largest char value	 	*/
#define CHAR_MIN        -128	/* smallest char value	 	*/
#define INT_MAX   2147483647	/* largest int value	 	*/
#define INT_MIN  -2147483648	/* smallest int value	 	*/
#define LONG_MAX  2147483647	/* largest long value	 	*/
#define LONG_MIN -2147483648	/* smallest long value	 	*/
#define SCHAR_MAX        127	/* largest signed char value 	*/
#define SCHAR_MIN       -128	/* smallest signed char value 	*/
#define SHRT_MAX       32767	/* largest short value	 	*/
#define SHRT_MIN      -32768	/* smallest short value	 	*/
#define UCHAR_MAX        255	/* largest unsigned char value 	*/
#define UINT_MAX  4294967295	/* largest unsigned int value	*/
#define ULONG_MAX 4294967295	/* largest unsigned long value	*/
#define USHRT_MAX      65535	/* largest unsigned short value	*/

/* If HUGE is not defined as the largest double in math.h, define
   it appropriately here:
#define	HUGE	xxxxxxxxx
*/

#endif
