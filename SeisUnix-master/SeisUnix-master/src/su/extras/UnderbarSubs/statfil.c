/* STATFIL: $Revision: 1.3 $ ; $Date: 90/03/15 09:04:24 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (isis!csm9a!jkcohen)
 *----------------------------------------------------------------------
 */

#include "cwp.h"
#include <sys/stat.h>

/* statfil - determine type of file from file descriptor
 * statprint - print the filetype as a string
 *
 * Returns:
 *	statfil:   filetype (an enum type defined in cwp.h)
 *	statprint: string
 *
 * Synopsis:
 *	filetype statfil(fd)
 *	int fd;	  file descriptor
 *
 *	string statprint(fd)
 *	int fd;	  file descriptor
 *
 * Credits:
 *	SEP: Einar, Stew
 *	CWP: Jack
 *
 * Example:
 *	filetype ftype;
 *	...
 *	ftype = statfil(STDOUT)
 *	if (ftype = TTY) {
 *		...
 *
 * Notes:
 *	/dev/null is handled as a special case and classified
 *	as a disk file.
 *
 *	The SEP isapipe and isatape subroutines have been junked and
 *	most of the checks have been TENTATIVELY simplified.
 *
 *	The check on tapes demands the RAW interface.  This is moot
 *	and easily changed.
 *
 *	A check for DIRECTORIES was added since it doesn't
 *	cost much, but the newer items such as FIFOS and SOCKETS
 *	have been ignored, though they could be treated as was the
 *	DIRECTORY type.  If other DEVICES become of interest,
 *	they can be treated as was /dev/null.  If such new types
 *	are added, the typedef "filetype" in cwp.h must be extended.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /usr/local/src/su/lib/RCS/statfil.c,v $";
static string revid =
	"   $Revision: 1.3 $ ; $Date: 90/03/15 09:04:24 $";



static bool first = true;	/* to check if first entry	*/

filetype statfil(fd)
int fd;

{
	struct stat sfd;	/* for passed fd	*/
	struct stat sdn;	/* for /dev/null	*/
	struct stat smt;	/* for tape devices	*/
	char tapestring[50];	/* mtdevice strings	*/


	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	if (-1 == fstat(fd, &sfd)) {
		(void) syswarn("%s: fstat failed", __FILE__);
		return BADFILETYPE;
	}

	/* UNIX subroutine */
	if (isatty(fd))  return TTY;

	/* Standard stat test for regular file */
	if ((sfd.st_mode & S_IFMT) == S_IFREG) return DISK;

	/* Standard stat test for directory */
	if ((sfd.st_mode & S_IFMT) == S_IFDIR) return DIRECTORY;

	/* Detect tape by comparing its major device number to	*/
	/* /dev/RMTDEVICE (as defined in portdefs.h).  Complain	*/
	/* and exit if raw interface not used.			*/
	(void) strcpy(tapestring, (char *) RMTDEVICE);
	if (0 == stat(tapestring, &smt) &&
	     major(sfd.st_rdev) == major(smt.st_rdev)) {
			return TAPE;
	}

	(void) strcpy(tapestring, (char *) MTDEVICE);
	if (0 == stat(tapestring, &smt) &&
	     major(sfd.st_rdev) == major(smt.st_rdev)) {
			err("%s: use rmt instead of mt", __FILE__);
	}

	/* Only pipes have 0 links */
	if (!sfd.st_nlink) return PIPE;

	/* Detect file as /dev/null by its device number and	*/
	/* classify it as a disk.				*/
	if (0 == stat("/dev/null", &sdn) &&
		sfd.st_rdev == sdn.st_rdev) return DISK;

	warn("%s: couldn't determine file type", __FILE__);
	return BADFILETYPE;
}


string statprint(fd)
int fd;
{
	switch (statfil(fd)) {
		case TTY:	return "TTY";
		case DISK:	return "DISK";
		case DIRECTORY:	return "DIRECTORY";
		case TAPE:	return "TAPE";
		case PIPE:	return "PIPE";
		default:	return "BADFILETYPE";
	}
}

/* For FORTRAN */
filetype statfil_(fd)
int *fd;
{
	return statfil(*fd);
}


#ifdef TEST

main(argc, argv)
int argc; char **argv;
{
	static filetype ftype;
	int fd;

	initargs(argc, argv);


	fprintf(stderr, "Checking statfil ...\n");
	/* Get filetype of stdin	*/
	switch (ftype = statfil(STDIN)) {
	case TTY:
		fprintf(stderr, "filetype(STDIN) = TTY\n");
	break;
	case DISK:
		fprintf(stderr, "filetype(STDIN) = DISK\n");
	break;
	case DIRECTORY:
		fprintf(stderr, "filetype(STDIN) = DIRECTORY\n");
	break;
	case TAPE:
		fprintf(stderr, "filetype(STDIN) = TAPE\n");
	break;
	case PIPE:
		fprintf(stderr, "filetype(STDIN) = PIPE\n");
	break;
	case BADFILETYPE:
		fprintf(stderr, "filetype(STDIN) = BADFILETYPE\n");
	break;
	default:
	    fprintf(stderr, "statfil(stdin) failed: ftype = %d\n", ftype);
	}


	/* Get filetype of stdout	*/
	switch (ftype = statfil(STDOUT)) {
	case TTY:
		fprintf(stderr, "filetype(STDOUT) = TTY\n");
	break;
	case DISK:
		fprintf(stderr, "filetype(STDOUT) = DISK\n");
	break;
	case DIRECTORY:
		fprintf(stderr, "filetype(STDOUT) = DIRECTORY\n");
	break;
	case TAPE:
		fprintf(stderr, "filetype(STDOUT) = TAPE\n");
	break;
	case PIPE:
		fprintf(stderr, "filetype(STDOUT) = PIPE\n");
	break;
	case BADFILETYPE:
		fprintf(stderr, "filetype(STDOUT) = BADFILETYPE\n");
	break;
	default:
	    fprintf(stderr, "statfil(stdout) failed: ftype = %d\n", ftype);
	}

	/* Get filetype of argv[1]	*/
	if (argc == 1) {
		fprintf(stderr, "no filename given\n");
		exit(1);
	}
	if (-1 == (fd = open(argv[1], O_RDONLY))) {
		fprintf(stderr, "can't open %s", argv[1]);
		exit(2);
	}

	switch (ftype = statfil(fd)) {
	case TTY:
		fprintf(stderr, "filetype(fd) = TTY\n");
	break;
	case DISK:
		fprintf(stderr, "filetype(fd) = DISK\n");
	break;
	case DIRECTORY:
		fprintf(stderr, "filetype(fd) = DIRECTORY\n");
	break;
	case TAPE:
		fprintf(stderr, "filetype(fd) = TAPE\n");
	break;
	case PIPE:
		fprintf(stderr, "filetype(fd) = PIPE\n");
	break;
	case BADFILETYPE:
		fprintf(stderr, "filetype(argv[1]) = BADFILETYPE\n");
	break;
	default:
	    fprintf(stderr, "statfil(argv[1]) failed: ftype = %d\n", ftype);
	}


	fprintf(stderr, "Checking statprint ...\n");
	/* Print filetype of stdin	*/
	fprintf(stderr, "filetype(STDIN) = %s\n", statprint(STDIN));


	/* Print filetype of stdout	*/
	fprintf(stderr, "filetype(STDOUT) = %s\n", statprint(STDOUT));


	/* Print filetype of argv[1]	*/
	if (argc == 1) {
		fprintf(stderr, "no filename given\n");
		exit(1);
	}
	if (-1 == (fd = open(argv[1], O_RDONLY))) {
		fprintf(stderr, "can't open %s", argv[1]);
		exit(2);
	}

	fprintf(stderr, "filetype(fd) = %s\n", statprint(fd));

	exit(0);
}
#endif
