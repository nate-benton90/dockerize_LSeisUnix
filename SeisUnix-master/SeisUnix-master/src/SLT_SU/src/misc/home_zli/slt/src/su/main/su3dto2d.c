
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SU3DTO2D - Extract a 2D line from 3D dataset 				\n" 
"\n"
"su3dto2d [parameters] <input-dat >output-data 		\n" 
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
"cdpfst=                first cdp number of the output 2d line		\n"
"cdpinc=                cdp number increment of the output 2d line	\n"
"\n"
"Optional parameters:							\n"
"nfold=                 number of cdp fold (default to fold in the id	\n"
"                       header 						\n"
"\n"
"Note:									\n"
"    1. the fold of the input 3D data must be constant and specified at \n"
"       the binary header (the number of traces per cdp must be the same) \n"
"       if fold=0 in the binary header, 1 is assumed (stack data)	\n"
"    2. tracl in the header represents trace number within the line 	\n"
"    3. tracr in the header represents line number 			\n"
"    4. input data set must be in disk					\n"
"    5. bilinear interpolation is used to extra the 2d output line	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	3/28/94			\n"
;

	#define file2g(x)	fseek(x,0,1);
	#define fseek2g(x,y,z)	fseek(x,y,z);

main(int argc, char **argv)
{

	segytrace tr;
	segychdr ch;
	segybhdr bh;


	FILE *infp=stdin, *outfp=stdout;

	int stracl, stracr, dtracl, dtracr, ntracl, ntracr;
	int otracl, otracr, etracl, etracr, ntrace, cdpfst;
	float cdpinc;
	char *data1, *data2, *data3, *data4;
	float *d1, *d2, *d3, *d4;

	int i, j, nt, nfold, nsegy, it;
	float tmpl , tmpr, tmp;
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
    	if(!getparfloat("cdpinc",&cdpinc)) err("cdpinc missing"); 
    	if(!getparint("cdpfst",&cdpfst)) err("cdpfst missing"); 


	/* make file size to be able to exceed 2 G on convex */
	file2g(infp);
	file2g(outfp);

	fgethdr(infp,&ch,&bh);
	
	nfold = bh.fold;
    	if(!getparint("nfold",&nfold)) nfold = bh.fold;

	if(nfold==0) warn("fold in binary is 0; reset to 1");
	if(nfold==0) nfold = 1;
	bh.fold = nfold;
        fputhdr(outfp,&ch,&bh);

	/* read in first trace for nt and dt */
        if (!fgettr(infp,&tr))  err("can't get first trace");

	nt = tr.ns; 

	nsegy = 240 + nt*sizeof(float);

	/* memory allocations */
	data1 = (char*) malloc(nfold*nsegy*sizeof(char)); 
	data2 = (char*) malloc(nfold*nsegy*sizeof(char)); 
	data3 = (char*) malloc(nfold*nsegy*sizeof(char)); 
	data4 = (char*) malloc(nfold*nsegy*sizeof(char)); 

	d1 = (float*) malloc(nt*sizeof(float)); 
	d2 = (float*) malloc(nt*sizeof(float)); 
	d3 = (float*) malloc(nt*sizeof(float)); 
	d4 = (float*) malloc(nt*sizeof(float)); 

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
		lpos = lpos*nfold*nsegy + 3600;
		fseek2g(infp,lpos,0);
		efread(data1,sizeof(char),nfold*nsegy,infp);

		lpos = (il2+ir1*ntracl);
		lpos = lpos*nfold*nsegy + 3600;
		fseek2g(infp,lpos,0);
		efread(data2,sizeof(char),nfold*nsegy,infp);

		lpos = (il1+ir2*ntracl);
		lpos = lpos*nfold*nsegy + 3600;
		fseek2g(infp,lpos,0);
		efread(data3,sizeof(char),nfold*nsegy,infp);

		lpos = (il2+ir2*ntracl);
		lpos = lpos*nfold*nsegy + 3600;
		fseek2g(infp,lpos,0);
		efread(data4,sizeof(char),nfold*nsegy,infp);

		for(j=0;j<nfold;j++) {
			bcopy(data1+j*nsegy,&tr,nsegy);

			bcopy(data1+j*nsegy+240,d1,nt*sizeof(float));
			bcopy(data2+j*nsegy+240,d2,nt*sizeof(float));
			bcopy(data3+j*nsegy+240,d3,nt*sizeof(float));
			bcopy(data4+j*nsegy+240,d4,nt*sizeof(float));

			for(it=0;it<nt;it++) {
				tr.data[it] = (d1[it]*wl1 + d2[it]*wl2)*wr1 +
					      (d3[it]*wl1 + d4[it]*wl2)*wr2;
			}

			tmp = cdpfst + i*cdpinc + 0.5;
			tr.cdp = tmp;
			tr.cdpt = j + 1;
			
			fputtr(outfp,&tr);
		}

	}

	return 0;

}

