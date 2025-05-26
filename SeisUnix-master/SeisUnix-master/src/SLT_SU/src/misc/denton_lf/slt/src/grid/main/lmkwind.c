/* LMKWIND program */
#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc = 
"LMKWIND - Landmark horizon window output program 			\n"
"\n"
"lmkwind <landmark.in.file layertop= layerbot= op= >landmark.out.file	\n" 
"\n"
"Required parameters:							\n"
"landmark.in.file=  name of Landmark input file 	\n"
"landmark.out.file= name of Landmark output file 	\n"
"layertop=          name of layer top grid 				\n"
"layerbot=          name of layer bottom grid 				\n"
"Optional parameters:							\n"
"op=0               0=output picks outside of layertop and layerbot 	\n"
"                   otherwise=output picks within layertop and layerbot	\n"
"xlpos=2            column position of landmark xline number 		\n"
"slpos=1            column position of landmark line number 		\n"
"tzpos=5            column position of landmark time (in ms) or depth 	\n"
"maxp=5000000        maximum number of rows in the landmark pick file	\n"
"o1=                minimum crossline number of layertop/layerbot grid \n"
"d1=                crossline number increment of layertop/layerbot grid\n"
"o2=                minimum line number of layertop/layerbot grid	\n"
"d2=                line number increment of layertop/layerbot grid 	\n"
"\n"
"                   The above 4 parameters default to grid header	\n"
"                   of input layertop and layerbot files; use gridheader \n"
"                   to print or update the grid headers if needed; 	\n"
"		    when these parameters are supplied,	they will 	\n"
"                   overwrite the values in the velocity grid	\n"
"verbos=0           1= for print deleted picks in standard error output \n"
"NOTES:						 			\n"
" 1. This program will output picks within or outside a layer	\n"
"    defined by layertop and layerbot grids.	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	06/25/02   		\n"
;

main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout, *topfp, *botfp;
	string layertop, layerbot;

	int maxp=5000000,xlpos=2,slpos=1,tzpos=5;
	float o1, d1, o2, d2;
	float xl,sl;
	usghed usghtop, usghbot;
	int n1, n2, i1, i2, verbos, iw;
	int op;

	int ierr;
	float *fbuf;
	char *cbuf;
	float tmp;
	float zt, zb, z;
	float *top, *bot;
	int nc, nf;


   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	/* get input parameters */
	if( !getparstring("layertop",&layertop) ) err("layertop missing");
	if( !getparstring("layerbot",&layerbot) ) err("layerbot missing");

	topfp = efopen(layertop,"r");
	file2g(topfp);
	ierr = fgetusghdr(topfp, &usghtop);
	if(ierr!=0) err("error get grid header of layertop ");

	botfp = efopen(layerbot,"r");
	file2g(botfp);
	ierr = fgetusghdr(botfp, &usghbot);
	if(ierr!=0) err("error get grid header of layerbot ");

	if( !getparfloat("o1",&o1) ) o1 = usghtop.o1;
	if( !getparfloat("d1",&d1) ) d1 = usghtop.d1;
	if( !getparfloat("o2",&o2) ) o2 = usghtop.o2;
	if( !getparfloat("d2",&d2) ) d2 = usghtop.d2;
	n1 = usghtop.n1;
	n2 = usghtop.n2;

	if(d1==0) err("d1 equals 0");
	if(d2==0) err("d2 equals 0");

	if (usghbot.o1!=o1) err(" check o1 of layerbot or layertop");
	if (usghbot.d1!=d1) err(" check d1 of layerbot or layertop");
	if (usghbot.o2!=o2) err(" check o2 of layerbot or layertop");
	if (usghbot.d2!=d2) err(" check d2 of layerbot or layertop");


	if( !getparint("xlpos",&xlpos) ) xlpos=2; xlpos -= 1;
	if( !getparint("slpos",&slpos) ) slpos=1; slpos -= 1;
	if( !getparint("tzpos",&tzpos) ) tzpos=5; tzpos -= 1;
	if( !getparint("maxp",&maxp) ) maxp=5000000;
	if( !getparint("verbos",&verbos) ) verbos=0;
	if( !getparint("op",&op) ) op=0;


	nc = 200;
	nf = 10;

	/* memory allocations */
    	top = (float*)emalloc(n1*n2*sizeof(float));
    	bot = (float*)emalloc(n1*n2*sizeof(float));
    	fbuf = (float *) malloc(nf*sizeof(float));
    	cbuf = (char *) emalloc(nc*sizeof(char));

	fseek2g(topfp,0,0);
	efread(top,sizeof(float),n1*n2,topfp);
	fseek2g(botfp,0,0);
	efread(bot,sizeof(float),n1*n2,botfp);

    	fgets(cbuf,nc,infp);
    	do {
		sscanf(cbuf,"%f %f %f %f %f",
			&fbuf[0],&fbuf[1],&fbuf[2],&fbuf[3],&fbuf[4]);
		xl = fbuf[xlpos];
		sl = fbuf[slpos];
		z = fbuf[tzpos];
	
		tmp = (xl - o1)/d1 + 0.5;
		i1 = tmp;
		if(i1<0) i1=0;
		if(i1>n1-1) i1=n1-1;
		tmp = (sl - o2)/d2 + 0.5;
		i2 = tmp;
		if(i2<0) i2=0;
		if(i2>n2-1) i2=n2-1;

		zt = top[i1+i2*n1];
		zb = bot[i1+i2*n1];

		iw = 0;
		if( z>=zt && z<= zb && zb>zt ) iw=1; 
		
		if( (op==0 && iw==0) || (op!=0 && iw==1) ) { 
    			fputs(cbuf,outfp);
		} else {
			if(verbos) fputs(cbuf,stderr);
		}
		
                bzero(cbuf,nc);
	} while(fgets(cbuf,nc,infp));
		
	free(cbuf);
	free(fbuf);
	free(top);
	free(bot);

	exit(0);
}
