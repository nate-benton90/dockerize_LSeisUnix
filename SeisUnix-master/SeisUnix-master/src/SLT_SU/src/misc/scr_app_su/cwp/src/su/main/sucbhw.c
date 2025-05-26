/* SUCBHW: */


#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUCBHW - change binary header word 				\n\
								\n\
sucbhw <stdin >stdout [optional parameters]			\n\
								\n\
Required parameters:						\n\
	key1=		1st parameter to change			\n\
	val1=		1st parameter value 			\n\
Optional parameters:						\n\
	key2=		2nd parameter to change			\n\
	val2=		2nd parameter value 			\n\
	key3=		3rd parameter to change			\n\
	val3=		3rd parameter value 			\n\
	datain=     name of the input data (instead of stdin) to \n\
				change the binary header in place (ignore stdout) \n\
				if not given, will output to stdout	\n\
				if given, will update input binary header in place \n\
								\n\
Examples:							\n\
								\n\
to set the cdp fold be 20:					\n\
	sucbhw <data >outdata key1=fold val1=20			\n\
to set the number of traces per record be 120:			\n\
	sucbhw <data >outdata key1=ntrpr val1=120		\n\
								\n\
Author:  	Zhiming Li,         				\n\
";
/**************** end self doc ***********************************/



SU_bhed bh;
SU_ched ch;
segy tr;

main(int argc, char **argv)
{
	String key1, key2, key3;
	String type1, type2, type3;
	int index1, index2, index3;
	int val1,val2,val3;
	Value vval1, vval2, vval3;
	void changeval(String type, Value *valp, int val);
	int inplace;
	char *datain;
	FILE *infp;



	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	file2g(stdin);
	file2g(stdout);

	/* Get parameters */
	if (!getparstring("key1", &key1))	err("key1 missing \n");
	if (!getparint("val1", &val1))	err("val1 missing \n");

	if (!getparstring("datain", &datain)) {
		inplace = 0;
	} else {
		inplace = 1;
		infp = fopen(datain,"r+");
	}

	/* open file with < 2G access */
	if(inplace==0) {
		file2g(stdin);
		file2g(stdout);
		gethdr(&ch,&bh);
	} else {
		file2g(infp);
		fgethdr(infp,&ch,&bh);
	}

	/* Get binary header */

	type1  = bhdtype(key1);
	index1 = getbindex(key1);
	changeval(type1, &vval1, val1);
	putbhval(&bh, index1, &vval1);

	if ( (getparstring("key2", &key2)) &&
	     (getparint("val2", &val2)) ) {
		type2  = bhdtype(key2);
		index2 = getbindex(key2);
		changeval(type2, &vval2, val2);
		putbhval(&bh, index2, &vval2);
	}

	if ( (getparstring("key3", &key3)) &&
	     (getparint("val3", &val3)) ) {
		type3  = bhdtype(key3);
		index3 = getbindex(key3);
		changeval(type3, &vval3, val3);
		putbhval(&bh, index3, &vval3);
	}

	/* Put binary header */
	if(inplace==0) {
		puthdr(&ch,&bh);
		while (gettr(&tr)) {
			puttr(&tr);
		}
	} else {
		file2g(infp);
		efwrite(&ch,sizeof(char),3200,infp);
		efwrite(&bh,sizeof(char),400,infp);
		efclose(infp);
	}


	return EXIT_SUCCESS;
}


void changeval(String type, Value *valp, int val)
{
	switch (*type) {
	case 's':
		err("can't change char header word");
	break;
	case 'h':
		valp->h = val;
	break;
	case 'u':
		valp->u = val;
	break;
	case 'l':
		valp->l = val;
	break;
	case 'v':
		valp->v = val;
	break;
	case 'i':
		valp->i = val;
	break;
	case 'p':
		valp->p = val;
	break;
	case 'f':
		valp->f = val;
	break;
	case 'd':
		valp->d = val;
	break;
	default:
		err("unknown type %s", type);
	break;
	}
}
