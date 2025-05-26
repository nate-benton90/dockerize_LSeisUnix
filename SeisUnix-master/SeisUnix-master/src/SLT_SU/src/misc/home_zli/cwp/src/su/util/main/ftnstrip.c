/* FTNSTRIP: $Revision: 1.7 $ ; $Date: 90/06/12 16:31:09 $		*/

#include "par.h"

/*********************** self documentation ******************************/
string sdoc =
" 									\n"
" FTNSTRIP - convert a file of floats plus record delimiters created 	\n"
"      via Fortran to a file containing only floats (as created via C)	\n"
" 									\n"
" ftnstrip <ftn_data >c_data 						\n"
" 									\n"
" Caveat: this code assumes the conventional Fortran format of header	\n"
"         and trailer integer containing the number of byte in the	\n"
"         record.  This is overwhelmingly common, but not universal.	\n"
" 									\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Jack
 */


main(int argc, char **argv)
{
	int n1bytes;
	char *buf;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	while (efread(&n1bytes, ISIZE, 1, stdin)) {
		buf = ealloc1(n1bytes, 1);
		efread(buf, n1bytes, 1, stdin);
		efwrite(buf, n1bytes, 1, stdout);
		free1(buf);
		efread(&n1bytes, ISIZE, 1, stdin);
	}

	return EXIT_SUCCESS;
}
