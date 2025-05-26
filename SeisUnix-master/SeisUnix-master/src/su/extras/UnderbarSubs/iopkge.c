/* IOPKGE: $Revision: 1.17 $ ; $Date: 89/11/07 22:22:55 $	*/

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

/* iopkge - routines for reads and writes with error checking
 *
 * eread   - read with error check
 * ewrite  - write with error check
 * efread  - fread with error check
 * efwrite - fwrite with error check
 * pread   - read on pipe with error check
 * pfread  - fread on pipe with error check
 *
 * Returns:
 *	eread returns number of bytes actually read
 *	ewrite returns number of bytes actually written
 *	efread returns number of items actually read
 *	efwrite returns number of items actually written
 *	pread returns number of bytes actually read from pipe
 *	pfread returns number of items actually read from pipe
 *
 * Notes:
 *	Up to main to check for zero bytes read if special action is
 *	required for this case.
 *
 * Synopsis:
 *	int eread(fd, buf, nbytes)
 *	int fd       - file descriptor of file to be read
 *	char *buf    - buffer to store characters read
 *	uint nbytes  - number of bytes to be read
 *
 *	int ewrite(fd, buf, nbytes)
 *	int fd       - file descriptor of file to be written
 *	char *buf    - buffer to store characters written
 *	uint nbytes  - number of bytes to be written
 *
 *	int efread(ptr, itemsize, nitems, stream)
 *	char *ptr    - pointer to buffer
 *	int itemsize - sizeof(*ptr)
 *	int nitems   - number of items to be read
 *	FILE *stream - input stream
 *
 *	int efwrite(ptr, itemsize, nitems, stream)
 *	char *ptr    - pointer to buffer
 *	int itemsize - sizeof(*ptr)
 *	int nitems   - number of items to be written
 *	FILE *stream - output stream
 *
 *	int pread(fd, buf, nbytes)
 *	int fd;      - file descriptor of file to be read
 *	char *buf;   - pointer to buffer
 *	uint nbytes; - number of bytes to be read
 *
 *	int pfread(ptr, itemsize, nitems, stream)
 *	char *ptr;    - pointer to buffer
 *	int itemsize; - sizeof(item pointed to)
 *	int nitems;   - number of items to be read
 *	FILE *stream; - input stream
 *
 * Credits: 
 *	Rochkind, "Advanced UNIX Programming"
 *	SEP: Rick, Ron, Jon, Stew
 *	CWP: Shuki, Jack
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/lib/RCS/iopkge.c,v $";
static string revid =
	"   $Revision: 1.17 $ ; $Date: 89/11/07 22:22:55 $";



static bool first = true;	/* to check if first entry	*/


int eread(fd, buf, nbytes)
int fd;
char *buf;
uint nbytes;
{
	int nread;	/* items actually read	*/

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	if (-1 == (nread = read(fd, buf, nbytes)))
		syserr("eread: read failed"); 

	return nread;
}


int ewrite(fd, buf, nbytes)
int fd;
char *buf;
uint nbytes;
{
	int nwrite;	/* bytes actually written	*/


	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	switch (nwrite = write(fd, buf, nbytes)) {
	case -1:
		syserr("ewrite: write failed");
	default:
		if (nwrite != nbytes) {
			errno = EFBIG; /* steal reasonable string */
			syserr("ewrite: write failed");
		}

		return nwrite;
	}
}


int efread(ptr, itemsize, nitems, stream)
char *ptr;
int itemsize;
int nitems;
FILE *stream;
{
	int nread;	/* items actually read	*/

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	nread = fread(ptr, itemsize, nitems, stream);

	if (ferror(stream)) syserr("efread: fread failed"); 

	return nread;
}


int efwrite(ptr, itemsize, nitems, stream)
char *ptr;
int itemsize;
int nitems;
FILE *stream;
{
	int nwrite;	/* items actually written	*/

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	nwrite = fwrite(ptr, itemsize, nitems, stream);
	if (ferror(stream)) {
		syserr("efwrite: fwrite failed"); 
	} else if (nwrite != nitems) {
		err("efwrite: only %d items from %d", nwrite, nitems);
	}

	return nwrite;
}


int pread(fd, buf, nbytes)
int fd;
char *buf;
uint nbytes;
{
	int nread, ntotal = 0;

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	while (nbytes) {
		switch (nread = read(fd, buf, nbytes)) {
		case -1:
			syserr("pread: read failed");
		case 0:	/* finished */
			return 0; /* must not default so return or break */
		default:
			ntotal += nread;
			nbytes -= nread;
			buf += nread;
		break;
		}
	}

	return ntotal;
}


int pfread(ptr, itemsize, nitems, stream)
char *ptr;
int itemsize;
int nitems;
FILE *stream;
{
	int nread, ntotal = 0;

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	while(nitems) {
		nread = fread(ptr, itemsize, nitems, stream);
		if (ferror(stream)) {
			syserr("pfread: fread failed"); 
		} else if (!nread || feof(stream)) {
			return 0;
		} else {
			ntotal += nread;
			nitems -= nread;
			ptr += nread*itemsize;
		}
	}

	return ntotal;
}


/* For FORTRAN */
int eread_(fd, buf, n)
int *fd;
char *buf;
uint *n;
{
	return eread(*fd, buf, *n);
}

int ewrite_(fd, buf, n)
int *fd;
char *buf;
uint *n;
{
	return ewrite(*fd, buf, *n);
}

int efread_(ptr, itemsize, nitems, stream)
char *ptr;
int *itemsize;
int *nitems;
FILE *stream;
{
	return efread(ptr, *itemsize, *nitems, stream);
}

int efwrite_(ptr, itemsize, nitems, stream)
char *ptr;
int *itemsize;
int *nitems;
FILE *stream;
{
	return efwrite(ptr, *itemsize, *nitems, stream);
}

int pread_(fd, buf, n)
int *fd;
char *buf;
uint *n;
{
	return pread(*fd, buf, *n);
}

int pfread_(ptr, itemsize, nitems, stream)
char *ptr;
int *itemsize;
int *nitems;
FILE *stream;
{
	return pfread(ptr, *itemsize, *nitems, stream);
}


#ifdef TEST
main(argc, argv)
int argc; char **argv;
{
	int fdr, fdw;
	char msg[BUFSIZ];
	char erbuf[BUFSIZ], ewbuf[BUFSIZ], rbuf[BUFSIZ], wbuf[BUFSIZ];
	char pbuf[1], pfbuf[1];
	uint mbytes, rbytes, wbytes;
	FILE *fpr, *fpw;
	int ritems, witems;
	int (*readptr) ();	/* pointer to eread() or pread()	*/
	int eread();		/* must be declared to use ptr		*/
	int pread();		/* must be declared to use ptr		*/

	xargc = argc; xargv = argv;

	/* Exercise eread and ewrite */
  	if (-1 == (fdw = open("junk.wr", O_RDWR | O_CREAT | O_TRUNC, 0666)))
  		syserr("can't open write file");

 	strcpy(ewbuf, "   Writing with ewrite\n");
	wbytes = strlen(ewbuf);
	ewrite(fdw, ewbuf, wbytes);

	rew(fdw);
	read(fdw, rbuf, wbytes);
	strcpy(msg, "***ewrite from file to buffer ...");
	mbytes = strlen(msg);
	write(STDOUT, msg, mbytes);
	write(STDOUT, rbuf, wbytes);

  	if (-1 == (fdr = open("junk.rd", O_RDWR | O_CREAT | O_TRUNC, 0666)))
  		syserr("can't open read file");

 	strcpy(wbuf, "   Reading with eread\n");
	rbytes = strlen(wbuf);
	write(fdr, wbuf, rbytes);
	strcpy(wbuf, "eread saw zero bytes\n");
	wbytes = strlen(wbuf);

	strcpy(msg, "***eread from file to buffer ...");
	mbytes = strlen(msg);
	write(STDOUT, msg, mbytes);
	rew(fdr);
	if (!eread(fdr, erbuf, rbytes)) {
		write(STDOUT, wbuf, wbytes);
	} else {
		write(STDOUT, erbuf, rbytes);
	}

 	strcpy(wbuf, "");
	rbytes = strlen(wbuf);
	write(fdr, wbuf, rbytes);
	strcpy(wbuf, "   eread saw zero bytes\n");
	wbytes = strlen(wbuf);

	strcpy(msg, "***eread from EMPTY file to buffer ...");
	mbytes = strlen(msg);
	write(STDOUT, msg, mbytes);
	rew(fdr);
	if (!eread(fdr, erbuf, rbytes)) {
		write(STDOUT, wbuf, wbytes);
	} else {
		write(STDOUT, erbuf, rbytes);
	}

	if (-1 == close(fdw)) syserr("close failed on write file");
	if (-1 == close(fdr)) syserr("close failed on read file");


	/* Exercise efread and efwrite */
  	if (NULL == (fpw = fopen("junk.fwr", "w+")))
  		syserr("can't open fwrite file");

 	strcpy(ewbuf, "   Writing with efwrite\n");
	witems = strlen(ewbuf);
	efwrite(ewbuf, 1, witems, fpw);
	rewind(fpw);

	fread(rbuf, 1, witems, fpw);
	rewind(fpw);
	strcpy(msg, "***efwrite from file to buffer ...");
	mbytes = strlen(msg);
	fwrite(msg, 1, mbytes, stdout);
	fwrite(rbuf, 1, witems, stdout);

  	if (NULL == (fpr = fopen("junk.frd", "w+")))
  		syserr("can't open fread file");

 	strcpy(wbuf, "   Reading with efread\n");
	ritems = strlen(wbuf);
	fwrite(wbuf, 1, ritems, fpr);
	rewind(fpr);
	strcpy(wbuf, "   efread saw zero bytes\n");
	witems = strlen(wbuf);

	strcpy(msg, "***efread from file to buffer ...");
	mbytes = strlen(msg);
	fwrite(msg, 1, mbytes, stdout);
	if (!efread(erbuf, 1, ritems, fpr)) {
		fwrite(wbuf, 1, witems, stdout);
	} else {
		fwrite(erbuf, 1, ritems, stdout);
	}
	rewind(fpr);

 	strcpy(wbuf, "   Reading byte by byte with efread\n");
	ritems = strlen(wbuf);
	fwrite(wbuf, 1, ritems, fpr);
	rewind(fpr);
	strcpy(wbuf, "   exit loop: efread returned zero\n");
	witems = strlen(wbuf);

	strcpy(msg, "***efread file byte by byte to buffer ...");
	mbytes = strlen(msg);
	fwrite(msg, 1, mbytes, stdout);
	while (efread(erbuf, 1, 1, fpr)) {
		fwrite(erbuf, 1, 1, stdout);
	}
	rewind(fpr);
	fwrite(wbuf, 1, witems, stdout);

 	strcpy(wbuf, "");
	ritems = strlen(wbuf);
	fwrite(wbuf, 1, ritems, fpr);
	rewind(fpr);
	strcpy(wbuf, "   efread saw zero bytes\n");
	witems = strlen(wbuf);

	strcpy(msg, "***efread from EMPTY file to buffer ...");
	mbytes = strlen(msg);
	fwrite(msg, 1, mbytes, stdout);
	efread(erbuf, 1, ritems, fpr);
	rewind(fpr);
	fwrite(wbuf, 1, witems, stdout);

	if (EOF == fclose(fpw)) {
		syserr("fclose failed on fwrite file");
	}
	if (EOF == fclose(fpr)) {
		syserr("fclose failed on fread file");
	}

	/* Exercise pread and eread */
	/* Set appropriate read function for input filetype */
/* canNOT simultaneously test pread and pfread: stdin gets used up!
	switch(statfil(STDIN)) {
	case TTY:
		err("input can't be tty");
	break;
	case DISK:
	case TAPE:
		readptr = eread;
		strcpy(ewbuf, "***Disk stdin: use eread ...   ");
		wbytes = strlen(ewbuf);
		ewrite(STDOUT, ewbuf, wbytes);
	break;
	case PIPE:
		readptr = pread;
		strcpy(ewbuf, "***Pipe stdin: use pread ...   ");
		wbytes = strlen(ewbuf);
		ewrite(STDOUT, ewbuf, wbytes);
	break;
	default:
		err("undefined input filetype %s", statprint(STDIN));
	break;
	}

	while ((*readptr)(STDIN, pbuf, 1)) {
		ewrite(STDOUT, pbuf, 1);
	}
end canNOT*/

	/* Exercise pfread and efread */
	/* Set appropriate read function for input filetype */
	switch(statfil(STDIN)) {
	case TTY:
		err("input can't be tty");
	break;
	case DISK:
	case TAPE:
		readptr = efread;
		strcpy(ewbuf, "***Disk stdin: use efread ...   ");
		witems = strlen(ewbuf);
		efwrite(ewbuf, 1, witems, stdout);
	break;
	case PIPE:
		readptr = pfread;
		strcpy(ewbuf, "***Pipe stdin: use pfread ...   ");
		witems = strlen(ewbuf);
		efwrite(ewbuf, 1, witems, stdout);
	break;
	default:
		err("undefined input filetype %s", statprint(STDIN));
	break;
	}

	while ((*readptr)(pfbuf, 1, 1, stdin)) {
		efwrite(pfbuf, 1, 1, stdout);
	}

	return SUCCEED;
}
#endif
