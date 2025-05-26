
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SU3DTO2D - Extract a 2D line from 3D dataset 				\n" 
"\n"
"su3dto2d [parameters] <input-dat >output-data 		\n" 
"\n"
"Required parameters:							\n"
"fline=                 first line number of input 	\n"
"ftrace=                first trace number of input	\n"
"nline=                 number of line number of input 		\n"
"ntrace=                number of trace number of input		\n"
"dline=                 increment of line number of input 	\n"
"dtrace=                increment of trace number of input	\n"
"sline=                 starting value of line number of output	\n"
"strace=                starting value of trace number of output\n"
"eline=                 ending value of line number of output	\n"
"etrace=                ending value of trace number of output	\n"
"ntrout=                number of traces in the output 2d diagonal line		\n"
"cdpfst=                first cdp number of the output 2d diagonal line		\n"
"cdpinc=                cdp number increment of the output 2d diagonal line	\n"
"\n"
"Optional parameters:							\n"
"nfold=                 number of cdp fold (default to fold in the id	\n"
"                       header 						\n"
"\n"
"Note:									\n"
"    1. the fold of the input 3D data must be constant and specified at \n"
"       the binary header (the number of traces per cdp must be the same) \n"
"       if fold=0 in the binary header, 1 is assumed (stack data)	\n"
"    2. input data set must be in disk and have ntracl x ntrcr traces \n"
"    3. bilinear interpolation is used to extra the 2d output line	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	3/28/94			\n"
;

main(int argc, char **argv)
{

	segytrace tr;
	segychdr ch;
	segybhdr bh;


	FILE *infp=stdin, *outfp=stdout;

	int ftrace, fline, dtrace, dline, ntrace, nline;
	int strace, sline, etrace, eline, ntrout, cdpfst;
	float cdpinc;
	char *data1, *data2, *data3, *data4;
	float *d1, *d2, *d3, *d4;

	int i, j, nt, nfold, nsegy, it;
	float tmpl , tmpr, tmp;
	float wl1, wl2, wr1, wr2;
	int il1, il2, ir1, ir2; 
	int il, ir;
	long long lpos;

  	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

   	if(!getparint("fline",&fline)) err("fline missing"); 
   	if(!getparint("ftrace",&ftrace)) err("ftrace missing"); 
   	if(!getparint("nline",&nline)) err("nline missing"); 
   	if(!getparint("ntrace",&ntrace)) err("ntrace missing"); 
   	if(!getparint("dline",&dline)) err("dline missing"); 
   	if(!getparint("dtrace",&dtrace)) err("dtrace missing"); 


   	if(!getparint("sline",&sline)) err("sline missing"); 
   	if(!getparint("strace",&strace)) err("strace missing"); 
   	if(!getparint("eline",&eline)) err("eline missing"); 
   	if(!getparint("etrace",&etrace)) err("etrace missing"); 
   	if(!getparint("ntrout",&ntrout)) err("ntrout missing"); 
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

	for(i=0;i<ntrout;i++) {

		if(ntrout>1) { 
			tmpl = etrace-strace;
			tmpl = tmpl / (ntrout-1.) * i + strace;
			tmpr = eline-sline;
			tmpr = tmpr / (ntrout-1.) * i + sline;
		} else {
			tmpl = strace;
			tmpr = sline;
		}

		fprintf(stderr,"itrace_out=%d tmpl=%g tmpr=%g \n",i,tmpl,tmpr);

		tmpl = (tmpl - ftrace)/dtrace; 
		tmpr = (tmpr - fline)/dline; 

		il = tmpl;
		ir = tmpr;

		fprintf(stderr,"itrace_out=%d il=%d ir=%d \n",i,il,ir);
		
		if(il<0) {
			il1 = 0;
			il2 = 0;
			wl1 = 0.5;
			wl2 = 0.5;
		} else if(il>=ntrace-1) {
			il1 = ntrace - 1;
			il2 = ntrace - 1;
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
		} else if(ir>=nline-1) {
			ir1 = nline - 1;
			ir2 = nline - 1;
			wr1 = 0.5;
			wr2 = 0.5;
		} else {
			ir1 = ir;
			ir2 = ir + 1;
			wr2 = tmpr - ir;
			wr1 = 1. - wr2;
		}

		lpos = (il1+ir1*ntrace);
		lpos = lpos*nfold*nsegy + 3600;
		fseek2g(infp,lpos,0);
		fread(data1,sizeof(char),nfold*nsegy,infp);

		lpos = (il2+ir1*ntrace);
		lpos = lpos*nfold*nsegy + 3600;
		fseek2g(infp,lpos,0);
		fread(data2,sizeof(char),nfold*nsegy,infp);

		lpos = (il1+ir2*ntrace);
		lpos = lpos*nfold*nsegy + 3600;
		fseek2g(infp,lpos,0);
		fread(data3,sizeof(char),nfold*nsegy,infp);

		lpos = (il2+ir2*ntrace);
		lpos = lpos*nfold*nsegy + 3600;
		fseek2g(infp,lpos,0);
		fread(data4,sizeof(char),nfold*nsegy,infp);

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

