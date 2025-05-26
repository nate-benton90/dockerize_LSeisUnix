#include "usgrid.h"
#include "subc.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUAREAZERO - zeor traces within a defined area 		\n"
"\n"
"suareazero <stdin >stdout [parameters]\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp gathers 			\n"
"stdout                     Name of output cdp gathers 			\n"
"tracekey=                  segy trace header key word for trace number \n" 
"linekey=                   segy trace header key word for line number \n"
"mintrace=                  minimum trace number to zero traces \n"
"maxtrace=                  maximum trace number to zero traces \n"
"minline=                   minimum line number to zero traces \n"
"maxline=                   maximum line number to zero traces \n"
"Optional Parameters:\n"
"\n"
"Author:	Zhiming Li		      		5-12-2001	\n"
"Notes:		\n"
" 1. Output traces within the defined area (mintrace<=trace<=maxtrace, \n"
"    minlin<=line<=maxline) will have mute time set to the maximum time \n"
"    of the trace. \n"

"\n";
/**************** end self doc *******************************************/


segytrace tr;


main(int argc, char **argv)
{

	FILE *infp=stdin,*outfp=stdout;
	String tracekey="fldr", linekey="ep", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk;
	int mintrace,maxtrace,minline,maxline;
	int ntrace, nline;
	int it, nt, tmax;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if(!getparstring("tracekey",&tracekey)) err(" tracekey missing");
	if(!getparstring("linekey",&linekey)) err("linekey missing");
	if(!getparint("mintrace",&mintrace)) err(" mintrace missing");
	if(!getparint("maxtrace",&maxtrace)) err(" maxtrace missing");
	if(!getparint("minline",&minline)) err(" minline missing");
	if(!getparint("maxline",&maxline)) err(" maxline missing");

	trktype = hdtype(tracekey);
	lnktype = hdtype(linekey);
	indxtrk = getindex(tracekey);
	indxlnk = getindex(linekey);

	file2g(infp);
	file2g(outfp);

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	nt = tr.ns;
	tmax = tr.delrt + (tr.ns-1)*tr.dt/1000; 

	fprintf(stderr," suareazero parameters \n");
	fprintf(stderr," ================== \n");
	fprintf(stderr," tracekey=%s linekey=%s \n",tracekey,linekey);
	fprintf(stderr," mintrace=%d maxtrace=%d \n",mintrace,maxtrace);
	fprintf(stderr," minline=%d maxline=%d \n",minline,maxline);
	fprintf(stderr,"\n");
		
	do {

		gethval(&tr,indxtrk,&trkval);
		ntrace = vtoi(trktype,trkval);
		gethval(&tr,indxlnk,&lnkval);
		nline = vtoi(lnktype,lnkval);

		if(nline>=minline && nline<=maxline &&
		   ntrace>=mintrace && ntrace<=maxtrace) {
			for(it=0;it<nt;it++) tr.data[it] = 0.;
		}
		tr.mute = tmax;
		fputtr(outfp,&tr);

	} while (fgettr(infp,&tr));

	return EXIT_SUCCESS;
}

