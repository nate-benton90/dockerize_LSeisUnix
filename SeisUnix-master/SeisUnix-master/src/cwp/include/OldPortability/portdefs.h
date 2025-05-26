/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.                       */

/* portdefs.h - include file for non-portable items
 * ENVIRONMENT-DEPENDENT - ADJUST TO LOCAL SYSTEM
 *
 * $Author: jkc $
 * $Source: /usr/local/src/general/include/RCS/portdefs.h,v $
 * $Revision: 1.3 $ ; $Date: 90/03/27 13:28:15 $
 */

#ifndef PORTDEFS_H
#define PORTDEFS_H

/* Note: If the target machine does not support the enum type, then
 *       the enums in cwp.h must be rewritten as defines, etc.
 */

/* Use if offsetof not in stddef.h */
/* STRICT_ALIGN is the data type with the most restrictive alignment */
/* #define		STRICT_ALIGN	long	*/
/* st is the structure tag or typedef, m is the field
 * To use, define the variable _aligned (cf. above) as:
 * static STRICT_ALIGN _aligned = 0; */
/* Formerly called OFFSET in SU */
/* #define	offsetof(st,m) ((char*) &((st*)&_aligned)->m - (char*)(st*)&_aligned)	*/

/* Remove comments if not declared in math.h or errno.h */
/* extern int errno; */

/* Pager of choice--usually more or pg */
#define		PAGE_PROGRAM	"pg"

/* The following must define actual tape devices defined in the
 * system's /dev directory; often mt0 and rmt0 will do.
 */
#define		MTDEVICE	"/dev/mt0"
#define		RMTDEVICE	"/dev/rmt0"


/* If void not available, remove comments from next line */
/* typedef int void; */


/* If uchar is available, remove next line*/
typedef	unsigned char	uchar;


/* If ushort, ulong, uint not available, remove comments from next lines */
/* typedef unsigned short	ushort;	*/
/* typedef unsigned long	ulong;	*/
/* typedef unsigned int		uint;	*/

/*	READ_OK  - read  permission for access(2)
 *	WRITE_OK - write permission for access(2)
 *	EXEC_OK  - exec  permission for access(2)
 *	FILE_OK  - file  existence  for access(2)
 *	Note: these are changed from the usual defines in file.h
 *	      because this include exists on some machines and
 *	      not others, often overlaps fcntl.h, etc.  Lint is
 *            happier with a fresh start.
 */
#define		READ_OK		4
#define		WRITE_OK	2
#define		EXEC_OK		1
#define		FILE_OK		0

/* Remove comments if L_tmpnam is not defined in stdio.h	*/
/* #define L_tmpnam	19	*/

/* Remove comments if you don't have these */
/*
#define		TRUE		(1)
#define		FALSE		(0)
#define		SEEK_SET	(0)
#define		SEEK_CUR	(1)
#define		SEEK_END	(2)
*/

/* These declarations make lint happy on our system.  You should
 * probably comment them out for openers since they may have just
 * the opposite effect on your system.
 */
off_t lseek();
int fseek();
/* void rewind(); */

void exit();

/* All but calloc are declared in our malloc.h.  As a close decision, we
 * didn't include malloc.h in cwpdefs.h and instead type them here.
 */
char *malloc();
void free();
char *realloc();
char *calloc();

/* This is a quick fix for systems that don't have vfprintf for use
 * with varargs.h.  Note that ANSI C uses stdargs.h instead of varargs.h.
 * In SU this is used only in errpkge.c.
 * See good discussion in Koenig - C Traps and Pitfalls.
 */
/*
#define vfprintf(stderr, format, args)	_doprnt((format), (args), (stderr))
*/

#endif
