
#include "usgrid.h"
#include "par.h"

char *sdoc = 
"GRID3DTO2D - Extract a 2D grid line from 3D grid 			\n" 
"\n"
"grid3dto2d [parameters] <input-dat >output-data 		\n" 
"\n"
"Required parameters:							\n"
"stracr=                starting value of tracr (line number) of input 	\n"
"stracl=                starting value of tracl (trace number) of input	\n"
"ntracr=                number of tracr (line number) of input 		\n"
"ntracl=                number of tracl (trace number) of input		\n"
"dtracr=                increment of tracr (line number) of input 	\n"
"dtracl=                increment of tracl (trace number) of input	\n"
"otracr=                starting value of tracr (line number) of output	\n"
"otracl=                starting value of tracl (trace number) of output\n"
"etracr=                ending value of tracr (line number) of output	\n"
"etracl=                ending value of tracl (trace number) of output	\n"
"ntrace=                number of traces in the output 2d line		\n"
"cdpfst=                first cdp number of output grid			\n"
"cdpinc=                cdp number increment of output grid		\n"
"\n"
"Optional parameters:							\n"
"NONE 						\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	12/13/94		\n"
;


int main(int argc, char **argv)
{

	usghed usgh;


	FILE *infp=stdin, *outfp=stdout;

	int stracl, stracr, dtracl, dtracr, ntracl, ntracr;
	int otracl, otracr, etracl, etracr, ntrace;
	int cdpfst;
	float cdpinc; 
	float *d1, *d2, *d3, *d4, *data;

	int i, j, nt, it;
	float tmpl , tmpr;
	float wl1, wl2, wr1, wr2;
	int il1, il2, ir1, ir2; 
	int il, ir;
	int lpos;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

    	if(!getparint("stracl",&stracl)) err("stracl missing"); 
    	if(!getparint("stracr",&stracr)) err("stracr missing"); 
    	if(!getparint("ntracl",&ntracl)) err("ntracl missing"); 
    	if(!getparint("ntracr",&ntracr)) err("ntracr missing"); 
    	if(!getparint("dtracl",&dtracl)) err("dtracl missing"); 
    	if(!getparint("dtracr",&dtracr)) err("dtracr missing"); 


    	if(!getparint("otracl",&otracl)) err("otracl missing"); 
    	if(!getparint("otracr",&otracr)) err("otracr missing"); 
    	if(!getparint("etracl",&etracl)) err("etracl missing"); 
    	if(!getparint("etracr",&etracr)) err("etracr missing"); 
    	if(!getparint("ntrace",&ntrace)) err("ntrace missing"); 
    	if(!getparint("cdpfst",&cdpfst)) err("cdpfst missing"); 
    	if(!getparfloat("cdpinc",&cdpinc)) err("cdpinc missing"); 


	/* make file size to be able to exceed 2 G on convex */
	file2g(infp);
	file2g(outfp);

	if(fgetusghdr(infp,&usgh)!=0) err("error fgetusghdr");
	
	nt = usgh.n1;
	/* memory allocations */
	data = (float*) emalloc(nt*sizeof(float)); 
	d1 = (float*) emalloc(nt*sizeof(float)); 
	d2 = (float*) emalloc(nt*sizeof(float)); 
	d3 = (float*) emalloc(nt*sizeof(float)); 
	d4 = (float*) emalloc(nt*sizeof(float)); 

	/* loop over output traces */

	for(i=0;i<ntrace;i++) {

		if(ntrace>1) { 
			tmpl = etracl-otracl;
			tmpl = tmpl / (ntrace-1.) * i + otracl;
			tmpr = etracr-otracr;
			tmpr = tmpr / (ntrace-1.) * i + otracr;
		} else {
			tmpl = otracl;
			tmpr = otracr;
		}

		tmpl = (tmpl - stracl)/dtracl; 
		tmpr = (tmpr - stracr)/dtracr; 

		il = tmpl;
		ir = tmpr;
		
		if(il<0) {
			il1 = 0;
			il2 = 0;
			wl1 = 0.5;
			wl2 = 0.5;
		} else if(il>=ntracl-1) {
			il1 = ntracl - 1;
			il2 = ntracl - 1;
			wl1 = 0.5;
			wl2 = 0.5;
		} else {
			il1 = il;
			il2 = il + 1;
			wl2 = tmpl - il;
			wl1 = 1. - wl2;
		}

		if(ir<0) {
			ir1 = 0;
			ir2 = 0;
			wr1 = 0.5;
			wr2 = 0.5;
		} else if(ir>=ntracr-1) {
			ir1 = ntracr - 1;
			ir2 = ntracr - 1;
			wr1 = 0.5;
			wr2 = 0.5;
		} else {
			ir1 = ir;
			ir2 = ir + 1;
			wr2 = tmpr - ir;
			wr1 = 1. - wr2;
		}

		lpos = (il1+ir1*ntracl);
		lpos = lpos*nt*4;
		fseek2g(infp,lpos,0);
		efread(d1,sizeof(float),nt,infp);

		lpos = (il2+ir1*ntracl);
		lpos = lpos*nt*4;
		fseek2g(infp,lpos,0);
		efread(d2,sizeof(float),nt,infp);

		lpos = (il1+ir2*ntracl);
		lpos = lpos*nt*4;
		fseek2g(infp,lpos,0);
		efread(d3,sizeof(float),nt,infp);

		lpos = (il2+ir2*ntracl);
		lpos = lpos*nt*4;
		fseek2g(infp,lpos,0);
		efread(d4,sizeof(float),nt,infp);

		for(it=0;it<nt;it++) {
			data[it] = (d1[it]*wl1 + d2[it]*wl2)*wr1 +
			      	(d3[it]*wl1 + d4[it]*wl2)*wr2;
		}
		efwrite(data,sizeof(float),nt,outfp);
	}

	free(data);
	free(d1);
	free(d2);
	free(d3);
	free(d4);

	usgh.n2 = ntrace;
	usgh.n3 = 1;
	usgh.n4 = 1;
	usgh.n5 = 1;
	usgh.o2 = cdpfst;
	usgh.d2 = cdpinc;

	if(fputusghdr(outfp,&usgh)!=0) err("error fputusghdr");

	return 0;

}

