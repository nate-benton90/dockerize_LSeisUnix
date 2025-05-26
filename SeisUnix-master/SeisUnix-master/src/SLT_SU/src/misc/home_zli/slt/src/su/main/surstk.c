#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SURSTK - Reciprocity Stack within cdp Gathers 		\n"
" 								\n"
" surstk <infile >outfile [optional parameters]		\n"
"								\n"
" Required Parameters:						\n"
" None    							\n"
" Optional parameters:						\n"
" flip=1               Change negative offset sign at output 	\n" 
"                      (1=yes; 0=no)				\n"
" tol=0.0              Tolerance of offset in absolute value	\n"
"                      comparison of positive offset and negative	\n"
"                      offsets.				 	\n"
"                      all traces with 				\n"
"			  abs[abs(offsetJ)-abs(offsetI)]<=tol   \n"
"                      will be stacked together to output one   \n"
"                      trace with positive offset		\n" 
"\n"
" Author:	Zhiming Li		      		6/18	\n"
"\n"
" Notes:							\n"
"  1. input data must be cdp gathers with tsort=2 and 		\n"
"     fold=Maximum Number of Traces per CDP defined in		\n"
"     the binary header.					\n"
;
/**************** end self doc ***********************************/

segytrace tr;
segybhdr bh;
segychdr ch;

void rcpstk(FILE *outfp, char *buf, float *offsets, int ntr, 
	float tol, int flip, int nt);  

main(int argc, char **argv)
{

	int flip, nt, nfoldmax, nsegy, ntr, ipre, inow;
	float tol, *offsets;
	char *buf;
	char *hdrs;
	FILE *outfp=stdout;

	/* Hook up getpar to handle the parameters */
	initargs(argc,argv);
	askdoc(1);

	/* Get parameters */
	if (!getparint("flip", &flip)) flip = 1;
	if (!getparfloat("tol", &tol)) tol = 0.;

	/* get data type */
	gethdr(&ch,&bh);
	if(bh.tsort!=2) 
		err(" Header Value tsort In Input Binary Header Is Not 2 \n");
	if(bh.fold==0) 
		err(" Header Value fold In Input Binary Header Is Zero \n");
	puthdr(&ch,&bh);

	fprintf(stderr," Maximum Number Of Folds In Input Is %d \n",bh.fold);

	nfoldmax = 2 * bh.fold;
 
	/* Get info from first trace */ 
	if (!gettr(&tr))  err("can't get first trace");
	nt = tr.ns;
	nsegy = nt * sizeof(float) + HDRBYTES;

	ntr = 0;
	ipre = tr.cdp;

	offsets = (float*) malloc(nfoldmax*sizeof(float)); 
	buf = (char*) malloc(nfoldmax*nsegy*sizeof(char)); 

	do { 
		inow = tr.cdp;

		if(ntr>=nfoldmax) 
			err(" %d traces at cdp=%d exceeds maximum number %d \n",
				ntr+1,tr.cdp,nfoldmax); 

		if(inow==ipre) {
			offsets[ntr] = tr.offset;
			bcopy((char*)&tr, buf+ntr*nsegy, nsegy);
			++ntr;
		} else {
			/* reciprocity stack and output */

			rcpstk(outfp,buf,offsets,ntr,tol,flip,nt);

			ntr = 0;
			offsets[ntr] = tr.offset;
			bcopy((char*)&tr, buf+ntr*nsegy, nsegy);
			++ntr;
			ipre = inow;
		}
	} while (gettr(&tr));
	
	/* last gather */
	if(ntr>0) {
		rcpstk(outfp,buf,offsets,ntr,tol,flip,nt);
	}
}

void rcpstk(FILE *outfp, char *buf, float *offsets, int ntr, 
	float tol, int flip, int nt) { 

	segytrace tr1, tr2;
	int *indx, i, j;
	int nsegy, fold, ihdr, it ;
	float ofi, ofj, dif, ofsum, ofmax, scale;

	if (ntr==0) return;

	nsegy = nt*sizeof(float) + HDRBYTES;

	indx = (int*) malloc(ntr*sizeof(int));
	for(i=0;i<ntr;i++) indx[i] = i;

	for(i=0;i<ntr;i++) {
		/* see if the trace has been reciprocity-stacked */
		if(indx[i]==-1) continue;
		ofi = fabs(offsets[i]);
		bcopy(buf+i*nsegy,(char*)&tr1,nsegy);
		fold = 1;
		ofsum = ofi;
		ofmax = offsets[i];
		ihdr = i; 
		for(j=i+1;j<ntr;j++) {
			if(indx[j]==-1) continue;
			ofj = fabs(offsets[j]);
			if(fabs(ofi - ofj)<=tol) {
				bcopy(buf+j*nsegy,(char*)&tr2,nsegy);
				for(it=0;it<nt;it++)
					tr1.data[it] += tr2.data[it];
				ofsum += ofj;
				fold ++ ;
				if(ofmax<offsets[j]) {
					ofmax= offsets[j]; 
					ihdr = j;
				}
				indx[j] = -1;
			}
		}
		if(fold>1.) {
			scale = 1./fold;
			for(it=0;it<nt;it++) 
				tr1.data[it] = tr1.data[it]*scale;
			ofsum = ofsum * scale;
		}
		bcopy(buf+ihdr*nsegy,(char*)&tr1,HDRBYTES);
		if(flip==1 || fold>1) {
			ofsum = ofsum + 0.5;
			tr1.offset = ofsum;
		}
		fputtr(outfp,&tr1);
	}
	free(indx);	
}
