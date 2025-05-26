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
