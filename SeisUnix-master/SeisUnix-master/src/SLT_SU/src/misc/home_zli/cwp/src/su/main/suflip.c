/* SUFLIP: $Revision: 1.5 $ ; $Date: 90/12/23 16:32:55 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUFLIP - flip a data set in various ways			\n"
" 								\n"
" suflip <data1 >data2 flip=1 verbose=0				\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameters:						\n"
" 	flip=1 	rotational sense of flip			\n"
" 			+1  = flip 90 deg clockwise		\n"
" 			-1  = flip 90 deg counter-clockwise	\n"
" 			 0  = transpose data			\n"
" 			 2  = flip right-to-left		\n"
" 			 3  = flip top-to-bottom		\n"
" 	verbose=0	verbose = 1 echoes flip info		\n"
" 								\n"
" NOTE:  tr.dt header field is lost if flip=-1,+1.  It can be	\n"
"        reset using sushw.					\n"
"        [programs internally use tr.dt/1000000.0]		\n"
" 								\n"
" EXAMPLE PROCESSING SEQUENCES:					\n"
"   1.	suflip flip=-1 <data1 | sushw key=dt a=4000 >data2	\n"
" 								\n"
"   2.	suflip flip=2 <data1 | suflip flip=2 >data1_again	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Chris, Jack
 *
 * Caveat:
 *	right-left flip (flip = 2) and top-bottom flip (flip = 3)
 *	don't require the matrix approach.  We sacrificed efficiency
 *	for uniform coding.
 */

/* suroutine prototype */
void flipper(register float **indata, register float **flipdata,
		register int ncol, register int nrow, register int flip);

segy tr;
segybhdr bh;
segychdr ch;


main(int argc, char **argv)
{
	int flip;		/* flag dictating sense of flip		*/
	int verbose;		/* flag for echoing info		*/
	FILE *hdrfp;		/* fp for header storage file		*/
	float dt;		/* time sample rate			*/
	int nt;			/* samples per trace on input		*/
	int ntsize; 		/* ... measured in bytes		*/
	int ntr;		/* traces in input data			*/
	int ntrsize; 		/* ... measured in bytes		*/
	register int i;		/* counter			 	*/
	register float **data;	/* matrix for input data		*/
	register
	    float **flipdata;	/* matrix for flipped data		*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Set parameters */
	if (!getparint("verbose", &verbose))	verbose = 0;
	if (!getparint("flip",    &flip))    	flip = 1;
	if (flip != -1 && flip != 0 && flip != 1 && flip != 2 && flip != 3)
		err("flip = %d, flag must be -1, 0, 1, 2, or 3", flip);


	/* Prepare temporary file to hold headers */
	/*hdrfp = tmpfile();*/
	hdrfp = etempfile(NULL);

	/* Get info from first trace and store first header */
	if (!(ntr = gettra(&tr,0)))  err("can't get first trace");
	dt = (float)tr.dt/1000000.0;
	nt = tr.ns;
	ntsize = nt * FSIZE;
	ntrsize = ntr * FSIZE;

/* get id headers for possible updates */
	fseek(stdin,0,0);
        gethdr(&ch,&bh);
        puthdr(&ch,&bh);
	gettr(&tr);

	/* Allocate data matrices */
	data = ealloc2float(nt, ntr);
	if (flip == -1 || flip == 0 || flip == 1) {
		flipdata = ealloc2float(ntr, nt);
	} else {
		flipdata = ealloc2float(nt, ntr);
	}


	/* Loop over input traces & put them into data matrix */
	i = 0;
	do {
		memcpy((char*)data[i], (char*)tr.data, ntsize); 
		efwrite((char *)&tr, 1, HDRBYTES, hdrfp);
		++i;
	} while (gettr(&tr));
	erewind(hdrfp);


	/* Verbose print */
	if (verbose) {
		fprintf(stderr, "\nSUFLIP:          flip = %d\n", flip);
		fprintf(stderr, "input:   samples/trace = %d", nt);
		fprintf(stderr, "    traces = %d \n", ntr);
		if (flip == 1 || flip == 0 || flip == -1) {
			fprintf(stderr, "output:  samples/trace = %d", ntr);
			fprintf(stderr, "    traces = %d \n\n", nt);
		} else {
			fprintf(stderr, "output:  samples/trace = %d", nt);
			fprintf(stderr, "    traces = %d \n\n", ntr);
		}
	}


	/* Sub does the flipping */
	flipper(data, flipdata, nt, ntr, flip); 


	/* Output the result by pulling traces off flipdata matrix */
	if (flip == -1 || flip == 0 || flip == 1) {
		for (i = 0; i < nt; i++) {
			memcpy((char*)tr.data, (char*)flipdata[i], ntrsize); 
			efread(&tr, 1, HDRBYTES, hdrfp);
			tr.ns = ntr;
			tr.tracl = i + 1;
			puttr(&tr);
		}
	} else {
		for (i = 0; i < ntr; i++) {
			memcpy((char*)tr.data, (char*)flipdata[i], ntsize); 
			efread(&tr, 1, HDRBYTES, hdrfp);
			tr.ns = nt;
			tr.dt = dt*1000000.0;
			tr.tracl = i + 1;
			puttr(&tr);
		}
	}

	
	return EXIT_SUCCESS;
}


void flipper(register float **indata, register float **flipdata,
		register int ncol, register int nrow, int flip)
{
	register int icol, irow;

	switch (flip) {

	case -1: 	/*  flip 90 deg counter-clockwise  */

		for (irow = 0; irow < nrow; irow++) {
			for (icol = 0; icol < ncol; icol++) {
				flipdata[icol][nrow-1 - irow] =
							indata[irow][icol];
			}
		}
	break;
	case 0: 	/*  transpose  */

		for (irow = 0; irow < nrow; irow++) {
			for (icol = 0; icol < ncol; icol++) {
				flipdata[icol][irow] = indata[irow][icol];
			}
		}
	break;
	case 1: 	/*  flip 90 deg clockwise  */

		for (irow = 0; irow < nrow; irow++) {
			for (icol = 0; icol < ncol; icol++) {
				flipdata[ncol-1 - icol][irow] =
						indata[irow][icol];
			}
		}
	break;
	case 2:		/*  flip right-to-left  */

		for (irow = 0; irow < nrow; irow++) {
			for (icol = 0; icol < ncol; icol++) {
				flipdata[nrow-1 - irow][icol] =
						indata[irow][icol];
			}
		}
	break;
	case 3:		/*  flip top-to-bottom  */

		for (irow = 0; irow < nrow; irow++) {
			for (icol = 0; icol < ncol; icol++) {
				flipdata[irow][ncol-1 - icol] =
							indata[irow][icol];
			}
		}
	break;
	default:	/*  defensive programming  */

		err("%d: mysterious flip value: %d", __LINE__, flip);
	}
	return;
}
