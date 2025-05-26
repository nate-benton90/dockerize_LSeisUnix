/* TAPEFWRITE: $Revision: 1.3 $ ; $Date: 89/11/10 10:48:32 $	*/


/* Exercise the tape drive	*/

#include "/usr/local/include/cwpdefs.h"

#define BSIZE	10240			/* Buffer size		*/
#define TSIZE	8240			/* Trace size		*/
#define NTIMES	5280			/* Number of writes	*/
#define TAPE	"/dev/r1600mt0"		/* Name of tape file	*/


main()
{
	FILE *t_fp;	/* file descriptor for tape	*/
	int itr;	/* current trace number		*/
	void perror();	/* system error routine		*/
	static char buf[BSIZE];	/* buffer: all zeroes	*/


	/* Open tape file */
	if (NULL == (t_fp = fopen(TAPE, "w"))) {
		perror("can't open tape write_only");
		exit(2);
	}


	/* Copy "trace" repeatedly from buffer to tape */
	for (itr = 0; itr < NTIMES; ++itr) {

		if (NULL == fwrite(buf, 1, TSIZE, t_fp)) {
			perror("fwrite failed");
			exit(5);
		}

		if ((itr + 1) % 20 == 0) {
			(void) fprintf(stderr,
				" %d traces written to tape\n", itr + 1);
		}
	}


	/* Clean up */
	if (EOF == fclose(t_fp)) {
		perror("close failed on tape");
		exit(7);
	}

	return 0;
}
