/* TAPEWRITE: $Revision: 1.3 $ ; $Date: 89/11/10 10:48:32 $	*/


/* Exercise the tape drive	*/

#include "cwp.h"

#define BSIZE	10240			/* Buffer size		*/
#define TSIZE	1024			/* Trace size		*/
#define NTIMES	50			/* Number of writes	*/
#define TAPE	"/dev/rmt1"		/* Name of tape file	*/
#define SLEEPTIME	0		/* Sleep time		*/


main()
{
	int t_fd;	/* file descriptor for tape	*/
	int itr;	/* current trace number		*/
	static char buf[BSIZE];	/* buffer: all zeroes	*/


	/* Open tape file */
	if (-1 == (t_fd = open(TAPE, O_WRONLY | O_CREAT | O_TRUNC, 0666))) {
		perror("can't open tape write_only");
		exit(2);
	}


	/* Copy "trace" repeatedly from buffer to tape */
	for (itr = 0; itr < NTIMES; ++itr) {

		if (-1 == write(t_fd, buf, TSIZE)) {
			perror("write failed");
			exit(5);
		}

/*
		(void) sleep((uint) SLEEPTIME);
*/

		if ((itr + 1) % 20 == 0) {
			(void) fprintf(stderr,
				" %d traces written to tape\n", itr + 1);
		}
	}


	/* Clean up */
	if (-1 == close(t_fd)) {
		perror("close failed on tape");
		exit(7);
	}

	return 0;
}
