/* SUOP: $Revision: 1.3 $ ; $Date: 90/12/25 23:36:13 $		*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines,.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUOP - do unary arithmetic operation on segys 		\n"
" 								\n"
" suop <stdin >stdout op=abs					\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameter:						\n"
" 	op=abs		operation flag		 		\n"
" 	       		abs   : absolute value			\n"
" 	       		ssqrt : signed square root 		\n"
" 	       		sqr   : square	 			\n"
" 	       		ssqr  : signed square 			\n"
" 	       		exp   : exponentiate			\n"
" 	       		slog  : signed natural log 		\n"
" 	       		slog10: signed common log 		\n"
" 	       		cos   : cosine 				\n"
" 	       		sin   : sine 				\n"
" 	       		tan   : tangent		 		\n"
" 	       		cosh  : hyperbolic cosine 		\n"
" 	       		sinh  : hyperbolic sine 		\n"
" 	       		tanh  : hyperbolic tangent 		\n"
" 								\n"
" Note:	Binary ops: diff, sum, prod and quo afforded by suop2	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Shuki, Jack
 *
 * Notes:
 *	If efficiency becomes important consider inverting main loop
 *      and repeating operation code within the branches of the switch.
 */


#define	FABS	1
#define	SSQRT	2
#define	SQR	3
#define	SSQR	4
#define EXP	5
#define SLOG	6
#define SLOG10	7
#define COS	8
#define SIN	9
#define TAN	10
#define COSH	11
#define SINH	12
#define TANH	13


segy tr;

main(int argc, char **argv)
{
	string op;	/* operation: abs, exp, ..., 		*/
	int iop;	/* integer abbrev. for op in switch	*/
	int nt;		/* number of samples on input trace	*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get information from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;


	/* Get operation */
	if (!getparstring("op", &op))	op = "abs";

	if      (STREQ(op, "abs"))	iop = FABS;
	else if (STREQ(op, "ssqrt"))	iop = SSQRT;
	else if (STREQ(op, "sqr"))	iop = SQR;
	else if (STREQ(op, "ssqr"))	iop = SSQR;
	else if (STREQ(op, "exp"))	iop = EXP;
	else if (STREQ(op, "slog"))	iop = SLOG;
	else if (STREQ(op, "slog10"))	iop = SLOG10;
	else if (STREQ(op, "cos"))	iop = COS;
	else if (STREQ(op, "sin"))	iop = SIN;
	else if (STREQ(op, "tan"))	iop = TAN;
	else if (STREQ(op, "cosh"))	iop = COSH;
	else if (STREQ(op, "sinh"))	iop = SINH;
	else if (STREQ(op, "tanh"))	iop = TANH;
	else     err("unknown operation=\"%s\", see self-doc", op);


	/* Main loop over traces */
	do {

		switch(iop) { register int i;
		case FABS:
			for (i = 0; i < nt; ++i)
				tr.data[i] = ABS(tr.data[i]);
		break;
		case SSQRT:
			for (i = 0; i < nt; ++i) {
				float x = tr.data[i];
				tr.data[i] = SGN(x) * sqrt(ABS(x));
			}
		break;
		case SQR:
			for (i = 0; i < nt; ++i) {
				float x = tr.data[i];
				tr.data[i] = x * x;
			}
		break;
		case SSQR:
			for (i = 0; i < nt; ++i) {
				float x = tr.data[i];
				tr.data[i] = SGN(x) * x * x;
			}
		break;
		case EXP:
			for (i = 0; i < nt; ++i)
				tr.data[i] = exp(tr.data[i]);
		break;
		case SLOG:
			for (i = 0; i < nt; ++i) {
				float x = tr.data[i];
				tr.data[i] = SGN(x) * log(ABS(x));
			}
		break;
		case SLOG10:
			for (i = 0; i < nt; ++i) {
				float x = tr.data[i];
				tr.data[i] = SGN(x) * log10(ABS(x));
			}
		break;
		case COS:
			for (i = 0; i < nt; ++i)
				tr.data[i] = cos(tr.data[i]);
		break;
		case SIN:
			for (i = 0; i < nt; ++i)
				tr.data[i] = sin(tr.data[i]);
		break;
		case TAN:
			for (i = 0; i < nt; ++i)
				tr.data[i] = tan(tr.data[i]);
		break;
		case COSH:
			for (i = 0; i < nt; ++i)
				tr.data[i] = cosh(tr.data[i]);
		break;
		case SINH:
			for (i = 0; i < nt; ++i)
				tr.data[i] = sinh(tr.data[i]);
		break;
		case TANH:
			for (i = 0; i < nt; ++i)
				tr.data[i] = tanh(tr.data[i]);
		break;
		default:  /* defensive programming */
			err("mysterious operation=\"%s\"", op);
		}

		puttr(&tr);

	} while (gettr(&tr));


	return EXIT_SUCCESS;
}
