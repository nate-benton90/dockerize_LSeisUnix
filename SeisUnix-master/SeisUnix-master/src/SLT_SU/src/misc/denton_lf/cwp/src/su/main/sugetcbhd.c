/* SUGETCBHD: */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUGETCBHD - sugetcbhd writes the SEGY ascii and binary headers \n"
" 								\n"
" sugetcbhd <infile [>outfile]					\n"
" 								\n"
" Required parameters:						\n"
" 	None							\n"
" 								\n"
" Optional parameters:						\n"
"								\n"
" outfile=stdout		output file (default: terminal)	\n"
" 								\n"
" author: 	Zhiming Li		         9/19/91	\n"
;
/**************** end self doc ***********************************/

segychdr ch;
segybhdr bh;

main(int argc, char **argv)
{

	int i;
	char *cbuf;

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	file2g(stdin);
	file2g(stdout);

	gethdr(&ch,&bh);
	printf("\n");
	printf(">>>>>> ASCII HEADER <<<<<< \n");
	printf("\n");
	cbuf = (char*) malloc(80*sizeof(char));
	for(i=0;i<40;i++) {
		strncpy(cbuf,(char*)ch.crd[i],80);
		cbuf[79]='\n';
		printf("%s",cbuf);
	}
	free(cbuf);

	printf("\n");
	printf(">>>>>> BINARY HEADER <<<<<< \n");
	printf("\n");
	printf("jobid=%d lino=%d reno=%d ntrpr=%hd \n",
					bh.jobid,bh.lino,bh.reno,bh.ntrpr);
	printf("nart=%hd hdt=%hd dto=%hd hns=%hd \n",
					bh.nart,bh.hdt,bh.dto,bh.hns);
	printf("nso=%hd format=%hd fold=%hd tsort=%hd \n",
					bh.nso,bh.format,bh.fold,bh.tsort);
	printf("vscode=%hd hsfs=%hd hsfe=%hd hslen=%hd \n",
					bh.vscode,bh.hsfs,bh.hsfe,bh.hslen);
	printf("hstyp=%hd schn=%hd hstas=%hd hstae=%hd \n",
					bh.hstyp,bh.schn,bh.hstas,bh.hstae);
	printf("htatyp=%hd hcorr=%hd bgrcv=%hd rcvm=%hd\n",
					bh.htatyp,bh.hcorr,bh.bgrcv,bh.rcvm);
	printf("mfeet=%hd polyt=%hd vpol=%hd \n",
					bh.mfeet,bh.polyt,bh.vpol);

	return EXIT_SUCCESS;
}
