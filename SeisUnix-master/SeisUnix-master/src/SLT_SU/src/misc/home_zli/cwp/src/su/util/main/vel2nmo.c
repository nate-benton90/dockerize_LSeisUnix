/* VEL2NMO - convert mouse picks for sunmo */

#include "par.h"

/*********************** self documentation ******************************/
string sdoc =
" 									\n"
" VEL2NMO - convert suximage mouse picks to sunmo format 		\n"
" 									\n"
" vel2nmo <stdin >stdout 						\n"
" 									\n"
" This is a real special tool to convert the data pairs written out by	\n"
" suvelan | suximage using the \"s\" operation, to the format needed by	\n"
" sunmo.  In more generic terms: it assumes (x,y) data pairs are one to	\n"
" line with no delimiters in an ASCII file.  The output is the form:	\n"
"	tnmo=t0,t1,...							\n"
"	vnmo=v0,v1,...							\n"
" 									\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *
 *	CWP: Jack
 */


/* Caveat: A much more general tool could be developed.  In particular,
/* one could allow pipes by using a tmpfile to admit the needed "rewind"
/* operation.	*/

main(int argc, char **argv)
{
	float t, v;
	char buf[BUFSIZ];

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	/* Handle first time outside loop to set up output format */
	if (!gets(buf))  err("empty file");
	if (2 == sscanf(buf, "%f %f", &t, &v)) printf("tnmo=%g", t);
	else  err("bad first line from stdin:\n%s", buf);

	/* Write the remaining times t */
	while (gets(buf)) {
		if (2 == sscanf(buf, "%f %f", &t, &v)) printf(",%g", t);
		else  err("bad line from stdin:\n%s", buf);
	}
	putchar('\n');

	/* Rewind and get the v's */
	rewind(stdin);
	gets(buf);
	sscanf(buf, "%f %f", &t, &v);
	printf("vnmo=%g", v);
	while (gets(buf)) {
		sscanf(buf, "%f %f", &t, &v);
		printf(",%g", v);
	}
	putchar('\n');
}
