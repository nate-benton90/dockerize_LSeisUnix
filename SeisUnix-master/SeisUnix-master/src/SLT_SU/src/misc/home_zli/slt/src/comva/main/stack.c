
#include "su.h"
#include "segy.h"
#include "header.h"


/*********************** self documentation **********************/
string sdoc =
" 								\n"
" STACK - cdp stacking of traces 			        \n"
" 								\n"
" stack <datain [parameters] >dataout			\n"
" 							        \n"
" Required parameters:						\n"
"  none								\n"
" Optional parameters:						\n"
"  npow=1.0           each stacked sample is divided by the npow-th    \n"
"                     power of number of live samples (0.5<=npow<=1.0) \n"
" 							        \n"
" Author:  Zhiming Li		8/20/1991                        \n"
" 							        \n"
"NOTE:								\n"
"	both top mute and bottom mute will be applied		\n"
;
/**************** end self doc ***********************************/


segytrace trin, trout;

main(int argc, char **argv)
{
	int  n1,i1,cdp_now,ntrace;
	float d1, *stk, *fold, o1, npow;
	int tmute_top,imute_top,ifold;
	int tmute_bot,imute_bot;
	int mutmax, mutmin;
	FILE *infp=stdin, *outfp=stdout;
	segybhdr bh;
	segychdr ch;
	
	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

/* parameters fetching */

	/* read segy id headers */ 
        fgethdr(infp,&ch,&bh);
	/* update binary header */ 
	bh.tsort = 3;
        bh.fold = 1;
	/* output id headers */
        fputhdr(outfp,&ch,&bh);


	if (!fgettr(infp,&trin))  err("can't get first trace");
	if (!getparfloat("npow",&npow)) npow=1.0;

	cdp_now = trin.cdp;
        d1 = trin.dt;
	o1 = trin.delrt;
	if ( d1 >= 1000 ) d1 = d1/1000.;
/*	if ( o1 >= 1000 ) o1 = o1/1000.; */
	if ( trin.dz != 0. ) {
		d1 = trin.dz;
		o1 = trin.fz;
	}
	n1 = trin.ns;

	stk = (float*) malloc(n1*sizeof(float));
	fold = (float*) malloc(n1*sizeof(float));

	for(i1=0;i1<n1;i1++) {
		stk[i1] = 0.;
	   	fold[i1] = 0.;
        }

	ifold = 0;
	ntrace = 0;

	mutmin = trin.mute;
	mutmax = trin.mutb;

	/* Main loop over traces */
	do {
	   	if ( trin.cdp == cdp_now ) {
	      		ifold += 1;
	      		tmute_top = trin.mute;
	      		tmute_top = (tmute_top - o1)/d1; 
	      		imute_top = tmute_top;
	      		tmute_bot = trin.mutb;
	      		tmute_bot = (tmute_bot - o1)/d1; 
	      		imute_bot = tmute_bot;

			if(mutmin>trin.mute) mutmin=trin.mute;
			if(mutmax<trin.mutb) mutmax=trin.mutb;

			if(imute_top<0) imute_top=0;
			if(imute_top>n1) imute_top=n1;
			if(imute_bot<=1) imute_bot=n1;
			if(imute_bot>n1) imute_bot=n1;

	      		for(i1=imute_top;i1<imute_bot;i1++) {
	        		stk[i1] += trin.data[i1];
	         		fold[i1] += 1.;
              		}
	      		if(ifold==1) memcpy((char*)&trout,(char*)&trin,240);
	   	} else {
	      		ntrace += 1;
	      		trout.offset = 0;
	      		trout.nhs = ifold;
	      		trout.tracl = ntrace;
			trout.sx = (trout.gx + trout.sx)/2;
			trout.gx = trout.sx;
			trout.mute = mutmin;
			if(mutmax>0) trout.mutb = mutmax;

	      		for(i1=0;i1<n1;i1++) {
	         		if(fold[i1] > 1.) {
					if(npow == 1.) {
	            			   trout.data[i1] = stk[i1] / fold[i1]; 
					} else if(npow!=0.0) {
	            			   trout.data[i1] = stk[i1] /
						pow(fold[i1],npow); 
					}
	         		}
	         		else {
	            			trout.data[i1] = stk[i1]; 
	         		}
			}
			
	      		fputtr(outfp,&trout);
			/* store the current trace */
	      		tmute_top = trin.mute;
	      		tmute_top = (tmute_top - o1)/d1; 
	      		imute_top = tmute_top;
	      		tmute_bot = trin.mutb;
	      		tmute_bot = (tmute_bot - o1)/d1; 
	      		imute_bot = tmute_bot;

			mutmin=trin.mute;
			mutmax=trin.mutb;

			if(imute_top<0) imute_top=0;
			if(imute_top>n1) imute_top=n1;
			if(imute_bot<=1) imute_bot=n1;
			if(imute_bot>n1) imute_bot=n1;

			for(i1=0;i1<imute_top;i1++) {
	        		stk[i1] = 0.;
	         		fold[i1] = 0.;
			}
	      		for(i1=imute_top;i1<imute_bot;i1++) {
	        		stk[i1] = trin.data[i1];
	         		fold[i1] = 1.;
              		}
			for(i1=imute_bot;i1<n1;i1++) {
	        		stk[i1] = 0.;
	         		fold[i1] = 0.;
			}

	      		ifold = 1;
	      		cdp_now = trin.cdp;
	      		memcpy((char*)&trout,(char*)&trin,240);
	  	}
	} while (fgettr(infp,&trin));
/* last cdp */
	ntrace += 1;
	trout.offset = 0;
	trout.nhs = ifold;
	trout.tracl = ntrace;
	trout.sx = (trout.gx + trout.sx)/2;
	trout.gx = trout.sx;
	trout.mute = mutmin;
	if(mutmax>0) trout.mutb = mutmax;
	for(i1=0;i1<n1;i1++) {
       		if(fold[i1] > 1.) {
			if(npow == 1.) {
	            	   trout.data[i1] = stk[i1] / fold[i1]; 
			} else if(npow!=0.0) {
	            	   trout.data[i1] = stk[i1] /
				pow(fold[i1],npow); 
			}
       		}
       		else {
       			trout.data[i1] = stk[i1]; 
       		}
	}	
	fputtr(outfp,&trout);

	return EXIT_SUCCESS;
}
