/* SUOP2: $Revision: 1.4 $ ; $Date: 90/12/26 00:04:40 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUOP2 - do a binary operation on two data sets		\n"
" 								\n"
" suop2 data1 data2 op=diff >stdout				\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameter:						\n"
" 	op=diff		binary operation: sum, prod, quo,	\n"
" 	       		default is diff				\n"
" 								\n"
" Note1: Output = data1 op data2 with the header of data1	\n"
" 								\n"
" Note2: For convenience and backward compatibility, this	\n"
" 	program may be called without an op code as:		\n"
" 								\n"
" 	susum  file1 file2 == suop2 file1 file2 op=sum		\n"
" 	sudiff file1 file2 == suop2 file1 file2 op=diff		\n"
" 	suprod file1 file2 == suop2 file1 file2 op=prod		\n"
" 	suquo  file1 file2 == suop2 file1 file2 op=quo		\n"
" 								\n"
" Note3: If an explicit op code is used it must FOLLOW the	\n"
"	filenames.						\n"
" 								\n"
" Note4: With op=quo, divide by 0 is trapped and 0 is returned.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Shuki
 *	CWP: Jack
 *
 * Notes:
 *	If efficiency becomes important consider inverting main loop
 *      and repeating operation code within the branches of the switch.
 */

#define	ADD	1
#define	SUB	2
#define	MUL	3
#define	DIV	4

segy intrace1, intrace2;

main(int argc, char **argv)
{
	FILE *fp1;	/* file pointer for first file		*/
	FILE *fp2;	/* file pointer for second file		*/
	string op;	/* operation: add, sub, mul, div	*/
	int iop;	/* integer abbrev. for op in switch	*/
	int nt;		/* number of sample points on traces	*/
	int nbytes;	/* number of bytes on traces		*/
	int itr = 0;	/* number of trace being processed	*/
	FILE *outfp=stdout;
	char *dummy;


	/* Initialize */
	initargs(argc, argv);
	askdoc(2); /* two file args required */


	/* Open two files given as arguments for reading */
	fp1 = efopen(argv[1], "r");
	fp2 = efopen(argv[2], "r");


	file2g(fp1);
	file2g(fp2);
	file2g(outfp);

	/* Get operation */
	if (!getparstring("op", &op))	op = "diff";


	if      (STREQ(op, "diff"))	iop = SUB;
	else if (STREQ(op, "sum"))	iop = ADD;
	else if (STREQ(op, "prod"))	iop = MUL;
	else if (STREQ(op, "quo"))	iop = DIV;
	else     err("unknown operation=\"%s\", see self-doc", op);

	dummy = (char*) malloc(3600*sizeof(char));
	fread(dummy,sizeof(char),3600,fp2);
	free(dummy);

	/* Loop over the traces */
	while (fgettr(fp1, &intrace1) &&
				(nbytes = fgettr(fp2, &intrace2))) {

		if ((nt = intrace1.ns) != intrace2.ns) {
			warn("trace %d:", itr);
			err("%s and %s have different ns (%d vs %d)",
				argv[1], argv[2], intrace1.ns, intrace2.ns);
		}


		/* Do the desired binary operation */
		switch(iop) { register int i;
		case SUB:
			for (i = 0; i < nt; ++i)
				intrace1.data[i] -= intrace2.data[i];
		break;
		case ADD:
			for (i = 0; i < nt; ++i)
				intrace1.data[i] += intrace2.data[i];
		break;
		case MUL:
			for (i = 0; i < nt; ++i)
				intrace1.data[i] *= intrace2.data[i];
		break;
		case DIV:
			for (i = 0; i < nt; ++i) {
				float denom = intrace2.data[i];
				if (CLOSETO(denom, 0.0)) intrace1.data[i] = 0.0;
				else	intrace1.data[i] /= denom;
			}
					  
		break;
		default:  /* defensive programming */
			err("mysterious operation=\"%s\"", op);
		}

		fputtr(outfp,&intrace1);
		++itr;
	}

	/* See if both files exhausted; notice if fd1 exhausted, then
	   we don't do an fgettr on fd2 on the final pass above */
	if (!nbytes) {
		warn("%s still had traces when %s was exhausted",
						argv[1], argv[2]);
		warn("processed %d pairs of traces before EOF", itr);
	} else if (fgettr(fp2, &intrace2)) {
		warn("%s still had traces when %s was exhausted",
						argv[2], argv[1]);
		warn("processed %d pairs of traces before EOF", itr);
	}


	return EXIT_SUCCESS;
}
