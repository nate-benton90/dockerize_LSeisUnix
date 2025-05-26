
#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 								",
" SUKILL_TRID - zero out traces	if the trid is set to 3		",
" 								",
" sukill_trid <stdin >stdout     					",
" 								",
" Required parameters	None					",
" 								",
NULL};

/* Credits:
 *	Balazs Nemeth
 *
 * Trace header fields accessed: ns, trid
 */
/**************** end self doc ***********************************/


segy tr;

int
main(int argc, char **argv)
{
	int nt = 0;		/* number of time samples	*/

	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Get parameters */

	gettr(&tr);
	nt = tr.ns;
	if (tr.trid==3) memset( (void *) tr.data, (int) '\0', nt * FSIZE);
	puttr(&tr);

	while (gettr(&tr)) {
		if (tr.trid==3) memset( (void *) tr.data, (int) '\0', nt * FSIZE);
		puttr(&tr);
	}

	return EXIT_SUCCESS;
}
