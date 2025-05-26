char *sdoc =
"SUCOMPRESS_WIND - window of decompressed SU datasets \n"
"\n"
"sucompress_wind < file.comp > file.comp.wind [options]\n"
"\n"
"options:\n"
"min=-2000000000       minimum gather number to output \n"
"max=2000000000        maximum gather number to output \n"
"j=1                   output compressed gathers with key word \n"
"                      increment of j \n"
"maxbyte=32000000      maximum number of bytes per sucompressed gather \n"
"                      it should be less or equal to product of \n"
"                         the max number of traces per gather \n"
"                         the number of samples per trace \n"
"                         and 4. \n"
"                      it is save to specify a bigger number \n"
"                      (for memory pre-allocation)   \n"
"head=1                SEGY header at begining of compressed file \n"
"                      1=yes 0=no \n"
"Examples: \n"
" 1. to select compressed gathers with key values of \n"
"       200,206,212,..,494,500, use: \n" 
"  sucompress_wind min=200 max=500 j=6 <input.su.comp >input.su.comp.wind \n"
"\n"
"Author: Z. LI,        10/22/2000 \n"
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
	FILE *infp=stdin; 
	int head=1;
	int i;
	int min=-2000000000, max=2000000000, j=1;

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
	getparint ("j",&j);
	getparint ("min",&min);
	getparint ("max",&max);

/* allocate storage */
	data = (char*) malloc (maxbyte>3600?maxbyte:3600);

/* window compressed data */

	file2g(infp);
	file2g(outfp);

	/* restore header */
	if (head) {
		fread (data,1,3600,infp);
		fwrite (data,1,3600,outfp);
	}

	iimse = 0.;
	iicomp = 0.;
	iigroup = 0;
	iitrace = 0;
	iimaxbyte = 0;
	iimaxtrace = 0;
	c80 = (char*)malloc(80*sizeof(char));

	fread (&ch,1,sizeof(ch),infp);
	/* read each chunk and copy to output */
	do {
		if(ch.nbyte>maxbyte) 
		err(" ch.nbyte=%d exceeds maxbyte:%d  \n",ch.nbyte,maxbyte);
		if (fread (data,1,ch.nbyte,infp) < ch.nbyte) break;
	 	i = vtoi(hdtype(ch.key),ch.value);
		if( (i-min)%j==0 && i>=min && i<=max) { 
			fwrite (&ch,1,sizeof(ch),outfp);
			fwrite (data,1,ch.nbyte,outfp);
			iigroup += 1;
			iitrace += ch.ntrace;
			if(ch.nbyte>iimaxbyte) iimaxbyte = ch.nbyte;
			if(ch.ntrace>iimaxtrace) iimaxtrace = ch.ntrace;
			iimse += ch.mse;
			iicomp += ch.compress;

		}
	} while(fread (&ch,1,sizeof(ch),infp));

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
