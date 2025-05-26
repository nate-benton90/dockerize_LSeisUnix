#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUXYFIX - Fix X and Y coordinates of source and receive  			\n"
"\n"
"suxyfix <stdin >stdout [optional parameters]\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp gathers 			\n"
"stdout                     Name of output cdp gathers 			\n"
"Optional Parameters:\n"
"qc=0    compute offset from sx,sy,gx,gy and compare with offset \n"
"        in the trace header. qc=1 print cdp and cdpt where the \n"
"        computed offset and the offset from trace header does not \n"
"        match. qc=0 do not print. \n"
" none	\n"
"\n"
"Author:	Zhiming Li		      		12-13-99		\n"
"Notes:									\n"
"  1. This program is used to correct the problem of incorrect x and y \n"
"     of source and receiver created by some partial moveout programs. \n"
"     These partial moveout programs sometimes modified the offset of \n"
"     of the trace after partial moveout, but did not update the x and y \n"
"     of the source and receiver. \n"
"  2. This program will use the midpoint x and y of the input trace \n"
"     (calculatred from sx,gx,sy,gy) and the offset to recompute \n"
"     the sx,sy,gx,gy alone the original azimuth of source-receiver \n"
"\n";
/**************** end self doc *******************************************/


segytrace tr;

main(int argc, char **argv)
{
	FILE *infp=stdin,*outfp=stdout;
	float gx,gy,sx,sy,mx,my,ofo,dd;
	int qc=0;

	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

	file2g(infp);
	file2g(outfp);

	getparint("qc",&qc);

	if(!fgettr(infp,&tr)) err(" error getting first trace");

	/* loop over traces */
	do {
		sx = tr.sx;
		sy = tr.sy;
		gx = tr.gx;
		gy = tr.gy;
		mx = (sx+gx)/2.;
		my = (sy+gy)/2.;
		dd = sqrt((sx-gx)*(sx-gx)+(sy-gy)*(sy-gy));
		if(tr.scalco>1) {
			dd = dd * tr.scalco;
		} else if(tr.scalco<0) {
			dd =  - dd / tr.scalco;
		}
		ofo = tr.offset;
		if(ofo<0) ofo = - ofo;
		if(qc==1) {
			if(fabs(dd-ofo)>1) {
	fprintf(stderr,"cdp= %d cdpt= %d offset_computed= %d offset_header= %d \n",
			tr.cdp,tr.cdpt,(int)dd,(int)ofo);
			}
		}
		if(dd>0.) {
			tr.sx = mx+ofo*(tr.sx-mx)/dd; 
			tr.gx = mx+ofo*(tr.gx-mx)/dd; 
			tr.sy = my+ofo*(tr.sy-my)/dd; 
			tr.gy = my+ofo*(tr.gy-my)/dd; 
		} else {
			tr.sx = mx-ofo/2; 
			tr.gx = mx-ofo/2; 
			tr.sy = my; 
			tr.gy = my; 
		}	
		fputtr(outfp,&tr);
	} while (fgettr(infp,&tr));

	return EXIT_SUCCESS;
}
