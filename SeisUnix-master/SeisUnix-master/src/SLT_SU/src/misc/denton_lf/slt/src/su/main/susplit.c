
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUSPLIT - split a dataset into multiple datasets			\n\
									\n\
susplit <input dataout1= [options]					\n\
									\n\
Required Parameters:							\n\
	none								\n\
									\n\
Optional Parameters:							\n\
	nout	= 1	number of output split datasets 		\n\
	dataout1=	1st split output dataset name			\n\
	dataout2=	2nd split output dataset name			\n\
	...								\n\
	dataoutnout=	nout-th split output dataset name		\n\
	key = tracl	Key header word to split on (see segy.h)	\n\
	min = -1e+10    min value of key header word for dataout1	\n\
	dout= 1e+20	value increment of key header to split 		\n\
	abs = 0		=1 to take absolute value of 			\n\
			key header word					\n\
        datain=		alternative input data name (instead of <input)	\n\
									\n\
                                                                        \n\
Notes:                                                                  \n\
1. traces with            key < min+dout   will be output to dataout1   \n\
   traces with min+dout<= key < min+2*dout will be output to dataout2   \n\
    ......                                                              \n\
   traces with min+(nout-1)*dout<=  key    will be output to dataoutnout \n\
Author:	Zhiming Li	      	8/29/92					\n\
";
/**************** end self doc *******************************************/

segytrace tr;
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
	String key;	
	Value val;	
	int ival;	
	string type;	
	int index;	
	float min;	
	float maxin,minin;	
	short ab;	
	register int i;	
	int start;
	char *datain;
	string *dataouti;	
	char *dataout;
	FILE *infp, **outfpi;
	int nout, iout;
	float tmp, dout;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Default parameters;	User-defined overrides */
	if (!getparstring("key"     , &key))		key = "tracl";
	if (!getparfloat("min"     , &min))		min = -1.e+10;
	if (!getparfloat("dout"     , &dout))		dout = 1.e+20;
	if (!getparshort("abs"     , &ab))		ab = 0;
	if (!getparstring("datain" , &datain)) {
		infp = stdin;
	} else {
		infp = efopen(datain,"r");
	}
	file2g(infp);
	/* get id headers for possible updates */
	fgethdr(infp,&ch,&bh);

	if (!getparint("nout"     , &nout))		nout = 1;
	dataouti = (string *) malloc(nout*sizeof(string));
	outfpi = (FILE**) malloc(sizeof(FILE *)*nout);
	dataout = (char*) malloc(10*sizeof(char));
	for(i=0;i<nout;i++) {
		sprintf(dataout,"dataout%d\0",i+1);
		if(!getparstring(dataout,&dataouti[i]))
			err(" error getparstring %s", dataout);
		outfpi[i] = efopen(dataouti[i],"w");
		file2g(outfpi[i]);
		auxputhdr(outfpi[i],&ch,&bh);
	}

	type = hdtype(key);
	index = getindex(key);

	if (!fgettr(infp,&tr)) err("can't get first trace");
	
	maxin = min + nout * dout;
	minin = min ;
	fprintf(stderr," ==== susplit parameters ==== \n");
	for(i=0;i<nout;i++) {
		fprintf(stderr," dataout%d key=%s min=%d max=%d \n",
			i+1,key,(int)(minin+i*dout),(int)(minin+(i+1)*dout-1));
	}

	/* Main loop over traces */
	do {
		/* Trace select */
		gethval(&tr, index, &val);
		if (ab) val = valtoabs(type, val);
		ival = vtoi(type, val);
		/* decide which output dataset for this trace */
		/*
		if ((minin <= ival) && (ival <= maxin) ) {
		*/
			tmp = (float)(ival-min)/dout; 
			iout = (int) tmp;
			if(iout<0) iout=0;
			if(iout>=nout) iout = nout-1; 
			auxputtr(outfpi[iout],&tr);
		/*	
		}
		*/
	} while (fgettr(infp,&tr));


	return EXIT_SUCCESS;
}
