#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"COM2P5 - Apply 2.5-D compensation filter \n"
"\n"
"com2p5 <stdin >stdout [optional parameters]\n"
"\n"
"Required Parameters:\n"
"None \n"
"Optional Parameters:\n"
"\n"
"Notes:\n"
" The filter to be applied is, in frequency-domain,	\n"
"\n"
"	sqrt( - i * f )					\n"
" where i is sqrt(-1), and f is frequency.		\n"
" \n"
" Author: Zhiming Li             2/20/92			\n"
"\n";
/**************** end self doc *******************************************/


segy tr;

main(int argc, char **argv)
{
	int nt;		/* number of samples per trace */

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(1);

	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;

	/* loop over input traces */
	do {
		f2p5_(tr.data,&nt);
		puttr(&tr);
	} while (gettr(&tr));

	return EXIT_SUCCESS;
}
