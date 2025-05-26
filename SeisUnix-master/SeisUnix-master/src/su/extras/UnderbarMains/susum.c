/* SUSUM: $Revision: 2.6 $ ; $Date: 89/05/25 16:54:05 $		*/

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
#include "segy.h"
#include "fconst.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUSUM - get the sum of two data sets				\n\
								\n\
susum data1 data2 >stdout					\n\
								\n\
Note:	This command violates the usual SU syntax, since	\n\
	it accepts the two filenames as arguments instead of	\n\
	using getpar.  Thus, no input redirect is used.		\n\
								\n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Jack, Chris
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/susum.c,v $";
static string revid =
	"   $Revision: 2.6 $ ; $Date: 89/05/25 16:54:05 $";



segy intrace1, intrace2;

main(argc, argv)
int argc; char **argv;
{
	int fd1;	/* file descriptor of first input trace	*/
	int fd2;	/* file descriptor of second input trace*/
	int nt;		/* number of sample points on traces	*/
	int nbytes;	/* number of bytes on traces		*/
	int itr = 0;	/* number of trace being processed	*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(2); /* two file args required */


	/* Open two files given as arguments for reading */
	if (-1 == (fd1 = open(argv[1], O_RDONLY))) {
		syserr("can't open %s", argv[1]);
	}
	if (-1 == (fd2 = open(argv[2], O_RDONLY))) {
		syserr("can't open %s", argv[2]);
	}

	/* Loop over the traces */
	while (fgettr(fd1, &intrace1) &&
				(nbytes = fgettr(fd2, &intrace2))) {

		if (intrace1.ns != intrace2.ns) {
			warn("trace %d:", itr);
			err("%s and %s have different ns (%d vs %d)",
				argv[1], argv[2], intrace1.ns, intrace2.ns);
		}

		nt = (int) intrace1.ns;	/* disaster to pass &ushort */
		vadd_(intrace2.data, ONE, intrace1.data, ONE,
					  intrace1.data, ONE, &nt);

		puttr(&intrace1);
		itr++;
	}

	/* See if both files exhausted; notice if fd1 exhausted, then
	   we don't do an fgettr on fd2 on the final pass above */
	if (!nbytes) {
		warn("%s still had traces when %s was exhausted",
						argv[1], argv[2]);
		warn("processed %d pairs of traces before EOF", itr);
	} else if (fgettr(fd2, &intrace2)) {
		warn("%s still had traces when %s was exhausted",
						argv[2], argv[1]);
		warn("processed %d pairs of traces before EOF", itr);
	}


	return SUCCEED;
}
