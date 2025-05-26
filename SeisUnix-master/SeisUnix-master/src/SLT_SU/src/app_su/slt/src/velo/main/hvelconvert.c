/* velocity card format conversion */

#include "usu.h"
#include "velo.h"


char *sdoc = 
"HVELCONVERT - convert WGC x-y HANDVEL to DISCO cdplbl HANDVEL cards 	\n"
"\n"
"hvelconvert [parameters] <handvel-xy >handvel-cdplbl 		\n" 
"\n"
"Required parameters:						 	\n"
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
"nvfmax=4096   maximum number of velocity functions in input HANDVEL    \n"
"              dataset                                              \n"
"ntvmax=256    maximum number of t-v pairs per velocity functions   \n"
"              in input HANDVEL dataset                                 \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	8/28/99   		\n"    
;

main(int argc, char **argv)
{
   	char *cbuf; 
   	int n1, n2, i, nxin, jv, icdpnow, icdp;
   	int icmax,ic,nvt,velo,time;
   	FILE *infp=stdin,*outfp=stdout;
   	float time4[4], velo4[4];
  	float  *times, *vrms; 
   	int cdplbl, cdplbls, cdplblx;
	float x, y, line, trace;
	double ttrace, lline;
	float x1,x2,x3,y1,y2,y3,trace1,trace2,trace3,line1,line2,line3;
	double xx1,xx2,xx3,yy1,yy2,yy3,ttrace1,ttrace2,ttrace3,lline1,lline2,lline3;
	double xnow, ynow;

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
	if (!getparint("ntvmax",&n1)) n1=256;
	if (!getparint("nvfmax",&n2)) n2=4096;
   	icmax = n1 * n2 ;
   	icdpnow = 0;
   	nxin = 0;

   	cbuf = (char*)malloc(81*sizeof(char));
   	times = (float*)malloc(n1*sizeof(int));
	vrms = (float*)malloc(n1*sizeof(int));
   	jv = 0;
	xx1 = x1; xx2 = x2; xx3 = x3;
	yy1 = y1; yy2 = y2; yy3 = y3;
	ttrace1 = trace1; ttrace2 = trace2; ttrace3 = trace3;
	lline1 = line1; lline2 = line2; lline3 = line3;

/* read input reflectivity file */
    for (ic=0;ic<icmax;ic++) {
       	if (feof(infp) !=0 ) break;
       	for(i=0;i<81;i++) cbuf[i]=' ';
       	gets(cbuf);
       	if ( cbuf[0]=='H' && cbuf[1]=='A' && cbuf[2]=='N' && cbuf[3]=='D' 
		&& cbuf[4]=='V' && cbuf[5]=='E' && cbuf[6]=='L' )  {

			icdp = 0;
			sscanf(cbuf+8,"%d %g %g",&icdp,&x,&y);

			if(icdp==0) err("cdp number can not be zero");

	  		if (icdpnow == 0 ) { 
				icdpnow = icdp; 
				xnow = x;
				ynow = y;
			}

		} else if(icdpnow!=0 && cbuf[0]!='*' ) { 

	  		if (icdp != icdpnow) {
	      		nvt = jv;
	      		nxin = nxin + 1;
	      		jv = 0;
				xy2sldb(xx1,yy1,ttrace1,lline1,xx2,yy2,ttrace2,lline2,
					  xx3,yy3,ttrace3,lline3,xnow,ynow,&ttrace,&lline);

				trace = ttrace + 0.5;
				line = lline + 0.5;

				cdplbls = line;
				cdplblx = trace;
				cdplbl = cdplbls*10000 + cdplblx;

				fprintf(stderr,
"HANDVEL conversion at cdp=%d trace=%d line=%d x=%g y=%g for %d t-v pairs \n",
		icdp, cdplblx, cdplbls, x, y, nvt); 
				printhvel(cdplbl,nvt,times,vrms,outfp);

	      		icdpnow = icdp;
				xnow = x;
				ynow = y;
			}
			for(i=0;i<4;i++) {
				time4[i] = 0.;
				velo4[i] = 0.;
			}
			sscanf(cbuf,"%f %f %f %f %f %f %f %f",
				&time4[0],&velo4[0],&time4[1],&velo4[1],
				&time4[2],&velo4[2],&time4[3],&velo4[3]);

	  		for(i=0;i<4;i++) {
	     		time = time4[i];
	     		velo = velo4[i];
	     		if (velo == 0.) break; 
	     		times[jv] = time;
	     		vrms[jv] = velo;
	     		jv = jv + 1;
	     	}
     	}
	}
    if (jv>0) {
		nxin = nxin + 1;	
		nvt = jv;

		xy2sldb(xx1,yy1,ttrace1,lline1,xx2,yy2,ttrace2,lline2,
	  			xx3,yy3,ttrace3,lline3,xnow,ynow,&ttrace,&lline);

		trace = ttrace + 0.5;
		line = lline + 0.5;

		cdplbls = line;
		cdplblx = trace;
		cdplbl = cdplbls*10000 + cdplblx;

		printhvel(cdplbl,nvt,times,vrms,outfp);

		fprintf(stderr,
"HANDVEL conversion at cdp=%d trace=%d line=%d x=%g y=%g for %d t-v pairs \n",
		icdp, cdplblx, cdplbls, x, y, nvt); 
     }

     fprintf(stderr,"\n");
     fprintf(stderr,"HANDVEL conversion done for %d cdps\n",nxin);
     free(times);
     free(vrms);
     free(cbuf);

	 return 0;
}
