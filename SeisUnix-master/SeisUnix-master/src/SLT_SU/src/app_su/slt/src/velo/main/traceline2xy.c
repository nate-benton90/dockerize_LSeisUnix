/* trace-line to x-y conversion */

#include "usu.h"
#include "velo.h"

char *sdoc = 
"traceline2xy - convert trace-line to x-y  	\n"
"\n"
"traceline2xy [parameters] <tl-cards >xy-cards 		\n" 
"\n"
"Required parameters:						 	\n"
"tl-cards      input multi-column ascii file (trace line ...) \n" 
"xy-cards      output multi-column ascii file (x y ...) \n" 
"x1=           x coordinate of 1st corner of the 3D master grid \n"
"y1=           y coordinate of 1st corner of the 3D master grid \n"
"trace1=       trace number of 1st corner of the 3D master grid \n"
"line1=        line number of 1st corner of the 3D master grid \n"
"x2=           x coordinate of 2nd corner of the 3D master grid \n"
"y2=           y coordinate of 2nd corner of the 3D master grid \n"
"trace2=       trace number of 2nd corner of the 3D master grid \n"
"line2=        line number of 2nd corner of the 3D master grid \n"
"x3=           x coordinate of 3rd corner of the 3D master grid \n"
"y3=           y coordinate of 3rd corner of the 3D master grid \n"
"trace3=       trace number of 3rd corner of the 3D master grid \n"
"line3=        line number of 3rd corner of the 3D master grid \n"
"numcol=2      number of columns per row in the input file \n"
"              (first colume is trace and second colume is line) \n"
"              maximum 10 \n"
"\n"
"Optional parameters:						 	\n"
"maxrow=4096   maximum number of rows in the input file    \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	8/28/99   		\n"    
;

main(int argc, char **argv)
{
   	char *cbuf; 
   	int n1, n2, i1, i2, i;
   	FILE *infp=stdin,*outfp=stdout;
   	float *cols;
	float x, y, line, trace;
	double xx, yy, ttrace, lline;
	float x1,x2,x3,y1,y2,y3,trace1,trace2,trace3,line1,line2,line3;
	double xx1,xx2,xx3,yy1,yy2,yy3,ttrace1,ttrace2,ttrace3,lline1,lline2,lline3;
	int iline, itrace;

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

	if (!getparfloat("x1",&x1)) err("must specify x1");
	if (!getparfloat("x2",&x2)) err("must specify x2");
	if (!getparfloat("x3",&x3)) err("must specify x3");
	if (!getparfloat("y1",&y1)) err("must specify y1");
	if (!getparfloat("y2",&y2)) err("must specify y2");
	if (!getparfloat("y3",&y3)) err("must specify y3");
	if (!getparfloat("line1",&line1)) err("must specify line1");
	if (!getparfloat("line2",&line2)) err("must specify line2");
	if (!getparfloat("line3",&line3)) err("must specify line3");
	if (!getparfloat("trace1",&trace1)) err("must specify trace1");
	if (!getparfloat("trace2",&trace2)) err("must specify trace2");
	if (!getparfloat("trace3",&trace3)) err("must specify trace3");

/* memory allocation */
	if (!getparint("numcol",&n1)) n1=10;
	if (!getparint("maxrow",&n2)) n2=4096;
	if(n1>10) err(" numcol %d is greater than 10 \n");

   	cbuf = (char*)malloc(81*sizeof(char));
   	cols = (float*)malloc(10*sizeof(float));
	xx1 = x1; xx2 = x2; xx3 = x3;
	yy1 = y1; yy2 = y2; yy3 = y3;
	ttrace1 = trace1; ttrace2 = trace2; ttrace3 = trace3;
	lline1 = line1; lline2 = line2; lline3 = line3;

/* read input file */
    for (i2=0;i2<n2;i2++) {
       	if (feof(infp) !=0 ) break;
       	for(i=0;i<81;i++) cbuf[i]=' ';
       	gets(cbuf);

		sscanf(cbuf,"%g %g %g %g %g %g %g %g %g %g ",
			&cols[0],&cols[1],&cols[2],&cols[3],&cols[4],
			&cols[5],&cols[6],&cols[7],&cols[8],&cols[9]);

		ttrace = cols[0];
		lline = cols[1];

		sl2xydb(ttrace1,lline1,xx1,yy1,ttrace2,lline2,xx2,yy2,
			ttrace3,lline3,xx3,yy3,ttrace,lline,&xx,&yy);

		printf(" %12.2f %12.2f",xx,yy);
		for(i1=2;i1<n1;i1++) printf(" %g",cols[i1]);
		printf("\n");

	}
	free(cols);
	return 0;
}
