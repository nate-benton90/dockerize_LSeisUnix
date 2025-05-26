/* SUSPIKE: $Revision: 1.3 $ ; $Date: 91/02/14 14:02:44 $	*/

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
string sdoc = "\
								\n\
SUSPIKE - make a small spike data set 				\n\
								\n\
suspike [optional parameters] > out_data_file  			\n\
								\n\
Creates a common offset su data file with up to four spikes	\n\
for impulse response studies					\n\
								\n\
Optional parameters:						\n\
	nt=64 		number of time samples			\n\
	ntr=32		number of traces			\n\
 	dt=0.004 	time sample rate in seconds		\n\
 	offset=400 	offset					\n\
	nspk=4		number of spikes			\n\
	ix1= ntr/4	trace number (from left) for spike #1	\n\
	it1= nt/4 	time sample to spike #1			\n\
	ix2 = ntr/4	trace for spike #2			\n\
	it2 = 3*nt/4 	time for spike #2			\n\
	ix3 = 3*ntr/4;	trace for spike #3			\n\
	it3 = nt/4;	time for spike #3			\n\
	ix4 = 3*ntr/4;	trace for spike #4			\n\
	it4 = 3*nt/4;	time for spike #4			\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Shuki, Chris
 *
 */

segychdr ch;
segybhdr bh;
segy tr;

main(int argc, char **argv)
{
	int nt,ntr,itr;
	int nspk,ix1,it1,ix2,it2;
	int ix3,it3,ix4,it4;
	float dt,offset;


	/* Initialize */
	initargs(argc, argv);
	askdoc(0); /* stdin not used */


	nt = 64;	igetpar("nt", &nt);		tr.ns = nt;
	ntr = 32;	igetpar("ntr", &ntr);
	dt = 0.004;	fgetpar("dt", &dt);		tr.dt = dt*1000000;
	offset = 400;	fgetpar("offset", &offset);	tr.offset = offset;
	nspk = 4;	igetpar("nspk", &nspk);
	ix1 = ntr/4;	igetpar("ix1", &ix1); 
	it1 = nt/4;	igetpar("it1", &it1);
	ix2 = ntr/4;	igetpar("ix2", &ix2);
	it2 = 3*nt/4;	igetpar("it2", &it2);
	ix3 = 3*ntr/4;	igetpar("ix3", &ix3);
	it3 = nt/4;	igetpar("it3", &it3);
	ix4 = 3*ntr/4;	igetpar("ix4", &ix4);
	it4 = 3*nt/4;	igetpar("it4", &it4);


	/* create id headers and output */
        idhdrs(&ch,&bh,nt);
	puthdr(&ch,&bh);
	
	for (itr = 0; itr < ntr; itr++) {
		tr.trid = 2;
		bzero(tr.data, nt * FSIZE);
		if (itr == ix1-1) {
			tr.data[it1-1] = 1.0;  
			tr.trid = 1;
		}
		if (nspk > 1 && itr == ix2-1) {
			tr.data[it2-1] = 1.0;
			tr.trid = 1;
		}
		if (nspk > 2 && itr == ix3-1) {
			tr.data[it3-1] = 1.0;
			tr.trid = 1;
		}
		if (nspk > 3 && itr == ix4-1) {	
			tr.data[it4-1] = 1.0;
			tr.trid = 1;
		}
		tr.tracl = itr + 1;
		puttr(&tr);
	}


	return EXIT_SUCCESS;
}
