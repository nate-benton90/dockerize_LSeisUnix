h38623
s 00000/00000/00018
d D 1.2 88/11/15 14:01:20 shuki 2 1
c 
e
s 00018/00000/00000
d D 1.1 88/04/14 14:05:44 shuki 1 0
c date and time created 88/04/14 14:05:44 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * local.h - include file for non-portable items
 */

#ifndef MTDEVICE

/* The following must define actual tape devices defined in the
 * system's /dev directory; often mt0 and rmt0 will do.
 */
/* TIMNA
#define		MTDEVICE	"/dev/1600mt0"
#define		RMTDEVICE	"/dev/r1600mt0"
*/

#define		MTDEVICE	"/dev/mt0"
#define		RMTDEVICE	"/dev/rmt0"

#endif	MTDEVICE
E 1
