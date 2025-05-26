/* add x and y to the trace header from trace-line numbers */

#include "usu.h"
#include "velo.h"

char *sdoc = 
"suaddxy - calculate x and y from trace and line number 	\n"
"\n"
"suaddxy [parameters] <input >output 		\n" 
"\n"
"Required parameters:						 	\n"
"input         input data set name \n" 
"output        output data set name \n" 
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
"\n"
"Optional parameters:						 	\n"
"tracekey=ep   segy key word for trace number \n"
"linekey=fldr  segy key word for line number \n"
"xkey=sx       segy key word for x \n"
"ykey=sy       segy key word for y \n"
"\n"
"Notes: \n"
" 1. Output x and y are truncated to the nearest integer and stored at \n"
"    xkey and ykey locations. \n"
" 2. When sx,sy,gx,gy are used for xkey or ykey, scalco is not modified.\n" 
"    Reset scalco=0 is recommeded either before or after suaddxy. \n"
" 3. When gelev,selev,sdepth,gdel,sdel,swdep,gwdep are used for xkey \n"
"    or ykey, scalel is not modified. Reset scalel=0 is recommended \n"
"    either before or after suaddxy. \n"
"AUTHOR:		Zhiming Li,       ,	4/22/02   		\n"    
;

void changeval(String type, Value *val, int f);

main(int argc, char **argv)
{
   	FILE *infp=stdin,*outfp=stdout;
	float x, y, line, trace;
	double xx, yy, ttrace, lline;
	float x1,x2,x3,y1,y2,y3,trace1,trace2,trace3,line1,line2,line3;
	double xx1,xx2,xx3,yy1,yy2,yy3,ttrace1,ttrace2,ttrace3,lline1,lline2,lline3;
	int iline, itrace;
	int itmp;

	String tracekey="ep", linekey="fldr", trktype, lnktype;
	String xkey="sx", ykey="sy", xktype, yktype;
	Value trkval, lnkval, xkval, ykval;
	int indxtrk, indxlnk;
	int indxxk, indxyk;


	segytrace tr;


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

	/* optional parameters */
	getparstring("tracekey",&tracekey);
	getparstring("linekey",&linekey);
	getparstring("xkey",&xkey);
	getparstring("ykey",&ykey);
	trktype = hdtype(tracekey);
	lnktype = hdtype(linekey);
	xktype = hdtype(xkey);
	yktype = hdtype(ykey);
	indxtrk = getindex(tracekey);
	indxlnk = getindex(linekey);
	indxxk = getindex(xkey);
	indxyk = getindex(ykey);

	xx1 = x1; xx2 = x2; xx3 = x3;
	yy1 = y1; yy2 = y2; yy3 = y3;
	ttrace1 = trace1; ttrace2 = trace2; ttrace3 = trace3;
	lline1 = line1; lline2 = line2; lline3 = line3;

	file2g(infp);
	file2g(outfp);
	if (!fgettr(infp,&tr))  err("can't get first trace");

/*  loop over input traces */
	do {
		gethval(&tr,indxtrk,&trkval);
		itmp = vtoi(trktype,trkval);
		ttrace = itmp;
		gethval(&tr,indxlnk,&lnkval);
		itmp = vtoi(lnktype,lnkval);
		lline = itmp;

		sl2xydb(ttrace1,lline1,xx1,yy1,ttrace2,lline2,xx2,yy2,
			ttrace3,lline3,xx3,yy3,ttrace,lline,&xx,&yy);

		itmp = xx;
		changeval(xktype,&xkval,itmp);
		puthval(&tr, indxxk, &xkval);
		itmp = yy;
		changeval(yktype,&ykval,itmp);
		puthval(&tr, indxyk, &ykval);

		fputtr(outfp,&tr);
	} while(fgettr(infp,&tr));

	return 0;
}

void changeval(String type, Value *val, int f) {

	switch (*type) {
	case 's':
		err("can't change char header word");
	break;
	case 'h':
		val->h = f;
	break;
	case 'u':
		val->u = f;
	break;
	case 'l':
		val->l = f;
	break;
	case 'v':
		val->v = f;
        break;
	case 'i':
		val->i = f;
	break;
        case 'p':
		val->p = f;
	break;
	case 'f':
		val->f = f;
	break;
	case 'd':
		val->d = f;
	break;
	default:
		err("unknown type %s", type);
	break;

	}
}
