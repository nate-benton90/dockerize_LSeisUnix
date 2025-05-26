char *sdoc =
"SUCOMPRESS_MERGE - merge of decompressed SU datasets \n"
"\n"
"sucompress_merge < file.comp > file.comp.merge [options]\n"
"\n"
"options:\n"
"head=1                assume SEGY file header is prepended to input files \n"
"maxbyte=32000000      maximum number of bytes per sucompressed gather \n"
"                      (should be less or equal to the maximum number \n"
"                      bytes per original gather before compression) \n"
"datain=               input compressed dataset name \n"
"                      specify datain=file1 datain=file2 ... \n"
"                      datain=filen for multiple input data sets \n"
" Examples: \n"
"    sucompress_merge datain=file1.su.comp datain=file2.su.com \n"
"      > files.su.merge.comp \n"
"\n"
" Author: Z. LI        1/2/2000 \n"
"\n";
#include <stdio.h>
#include <su.h>
#include <segy.h>
#include <cwp.h>
#include <hdr.h>
#include "sucomp.h"
#include "seilib.h"

comp_header ch;
segytrace tr;

main (int argc , char **argv) {
	int maxbyte=32000000;
	char *data;
	FILE *outfp=stdout;
	FILE *infp; 
	char *datain;
	int head=1, ndata;
	int i;

	int iigroup, iitrace, iimaxbyte, iimaxtrace;
	float iicomp, iimse;
	char *c80;

/* SU initializations */
	initargs (argc,argv);
	askdoc (1);

	file2g(outfp);

/* fetch parameters */
	getparint ("head",&head);
	getparint ("maxbyte",&maxbyte);

	ndata = 0;
	ndata = countparname("datain");
	if(ndata==0) err(" no input datain \n");

/* allocate storage */
	data = (char*) malloc (maxbyte>3600?maxbyte:3600);

	iimse = 0.;
	iicomp = 0.;
	iigroup = 0;
	iitrace = 0;
	iimaxbyte = 0;
	iimaxtrace = 0;
	c80 = (char*)malloc(80*sizeof(char));

/* merge compressed data */
	for(i=0;i<ndata;i++) {

	  	getnparstring(i+1,"datain",&datain);
		infp = fopen(datain, "r");
		file2g(infp);

		/* restore header */
		fseeko (infp,0,0);
		if (head) {
			fread (data,1,3600,infp);
			if(i==0) fwrite (data,1,3600,outfp);
		}

		fprintf(stderr," merge datain=%s ...\n",datain); 

 		fread (&ch,1,sizeof(ch),infp);

		/* read each chunk and copy to output */
		do {
			if(ch.nbyte>maxbyte) 
		err(" ch.nbyte=%d exceeds maxbyte:%d  \n",ch.nbyte,maxbyte);
			if (fread (data,1,ch.nbyte,infp) < ch.nbyte) break;
			fwrite (&ch,1,sizeof(ch),outfp);
			fwrite (data,1,ch.nbyte,outfp);

			iigroup += 1;
			iitrace += ch.ntrace;
			if(ch.nbyte>iimaxbyte) iimaxbyte = ch.nbyte;
			if(ch.ntrace>iimaxtrace) iimaxtrace = ch.ntrace;
			iimse += ch.mse;
			iicomp += ch.compress;

		} while(fread (&ch,1,sizeof(ch),infp));

		fclose(infp);

	}

	iicomp = iicomp/iigroup;
	iimse = iimse/iigroup;

	sprintf(c80,"C40 END EBCDIC      %s %d %d %d %d %8.3f %8.3f",
		ch.key,iigroup,iitrace,iimaxbyte,iimaxtrace,iicomp,iimse);

	if(head==1) {
		fseeko(outfp,80*39,0);
		fwrite(c80,80,sizeof(char),outfp);
	}


	free(c80);
	free(data);
	exit(0);

}
