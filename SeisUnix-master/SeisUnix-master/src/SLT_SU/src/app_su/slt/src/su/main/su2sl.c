#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SU2SL - Seismic Unix segy/grid to/from Seismic Linux segy/grid conversion \n" 
"\n"
"su2sl [parameters] <input-data >output-data 		\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
" skipbyte=3200    will copy skipbyte bytes to output before \n"
"                  conversion; for segy file use skipbyte=3200; \n"
"                  for grid or binary file, use skipbyte=0; \n"
"Notes: \n"
"  example 1. convert SU segy to SL segy: \n"
"         < su.segy su2sl > sl.segy \n"
"  example 2. convert SL segy to SU segy: \n"
"         < sl.segy su2sl > su.segy \n"
"  example 3. convert velocity grid from Unix to Linux : \n"
"         < unix.vgrid su2sl skipbyte=0 > linux.vgrid \n"
"  example 4. convert velocity grid from Linux to Unix : \n"
"         < linux.vgrid su2sl skipbyte=0 > unix.vgrid \n"
"\n"
"AUTHOR:	Zhiming Li,       ,	5/2/2000   \n"		    
;

main(int argc, char **argv)
{

	segytrace tr;

	FILE *infp=stdin, *outfp=stdout;
	int skipbyte;
	char *cbuf;
	char c4i[4], c4o[4];

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

   	if(!getparint("skipbyte",&skipbyte)) skipbyte=3200; 

	/* make file size to be able to exceed 2 G on convex */
	file2g(infp);
	file2g(outfp);

	if(skipbyte>0) {
		cbuf = (char*) malloc(skipbyte*sizeof(char));
		fread(cbuf,sizeof(char),skipbyte,infp);
		fwrite(cbuf,sizeof(char),skipbyte,outfp);
	}

	do {
		c4o[0] = c4i[2];	
		c4o[1] = c4i[3];	
		c4o[2] = c4i[0];	
		c4o[3] = c4i[1];	
		fwrite(c4o,sizeof(char),4,outfp);
	} while (fgets(c4i,4,infp));

	free(cbuf);
	return 0;

}

