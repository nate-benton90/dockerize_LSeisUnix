#include "ghdr.h"
#include "gridhd.h"
#include "comva.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc = 
"SEGY2MOVIE - convert segy trace data into 3-D movie data formats 	\n"
"									\n"
"segy2movie <segy_file >movie_file [optional parameters] 		\n"
"									\n"
"Required parameters:							\n"
"none									\n"
"						        		\n"
"Optional parameter: 							\n"
"									\n"
"perc=100.0              percentile used to determine clip		\n"
"clip=(perc percentile)  clip used to determine bclip and wclip		\n"
"pperc=perc              percentile for determining positive clip value\n"
"nperc=100.0-perc        percentile for determining negative clip value\n"
"pclip=clip              data values outside of [nclip,pclip] are clipped\n"
"nclip=-clip             data values outside of [nclip,pclip] are clipped\n"
"clippanle=1             panel (cdp, common-offset section, etc.) number \n"
"                        at which clip(s) is to be determined. if =0, the \n"
"                        whole input dataset will be used to determined  \n"
"                        clip(s)					\n"
"ntrpp=0                 number of traces per panel			\n"
"ihead=0                 add grid header to output movie dataset (0=n;1=y)\n"
"o1=tr.delrt             starting coordinate of 1rd axis of 3D movie cube\n" 
"                        (default to the minimum time in 1st trace header)\n"
"d1=tr.dt                coordinate increment of 1rd axis of 3D movie cube\n" 
"                        (default to the sample rate in 1st trace header)\n"
"o2=0                    starting coordinate of 2nd axis of 3D movie cube\n" 
"d2=1                    coordinate increment of 2nd axis of 3D movie cube\n" 
"o3=0                    starting coordinate of 3rd axis of 3D movie cube\n" 
"d3=1                    coordinate increment of 3rd axis of 3D movie cube\n" 
"                        (o2,d2,o3,d3 used only when ihead=1)		\n"
"									\n"
"NOTE:									\n"
"	    1. 	It is recommended clip(s) be given to speed up computation. \n"
"	    2. 	When ntrpp is not given (=0), it will be determined	\n"
"		automatically from input data. 				\n"  
"	    3.  When input is a piped input, clip must be given.	\n"
"									\n"
"	Author:		Zhiming Li	      	9-30-91			\n"
"\n";
/**************** end self doc ***********************************/

segytrace tr;	/* on  input: SEGY hdr & (float) trace data */
		/* on output: data as unsigned chars without segy headers */
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
	float *temp, nclip, pclip, nperc, pperc, perc, clip;
	int nt, needclip, is, ns, i1, i2, i3, n1, n2, n3, op, o2, o3;
	int clippanel, ntrpp, nsegy;
	FILE *infp=stdin, *outfp=stdout;
	filetype ftypein;
	bool ispipe;
	char *otr; 
	float si, soffset, sscale, oo2, oo3, d2, d3, d1, oo1;
	int ntrace;

	int ierr, ihead;
	ghed gh;

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	/* check to see if piped input */
	ftypein = filestat(STDIN);
	ispipe = (ftypein == PIPE || ftypein == FIFO ) ? true : false;

	/* get clip value */
	needclip = 0;
        if (getparfloat("clip",&clip)) {
                pclip = clip;
                nclip = -clip;
		
        } else {
        	if (!getparfloat("pclip",&pclip)) needclip = 1;
        	if (!getparfloat("nclip",&nclip)) needclip = 1;
	}
	if (!getparint("clippanel", &clippanel)) clippanel = 1;
	if (!getparint("ntrpp", &ntrpp)) ntrpp = 0;
	if (!getparint("ihead", &ihead)) ihead = 0;
       	if (!getparfloat("o2",&oo2)) oo2 = 0.;
       	if (!getparfloat("d2",&d2)) d2 = 1.;
       	if (!getparfloat("o3",&oo3)) oo3 = 0.;
       	if (!getparfloat("d3",&d3)) d3 = 1.;

	if (ispipe && needclip==1) 
		err("clip must be given if input from a pipe!\n");

	/* find out ntrpp */
	gethdr(&ch,&bh);
	if ( ntrpp==0 ) {
		if ( bh.tsort == 2 ) {
			ntrpp = bh.fold;
		} else if (bh.tsort == 3 || bh.tsort == 1) {
			ntrpp = bh.ntrpr;
		} else {
			ntrpp = 1;
		}
		fprintf(stderr,"Number of Traces per Panel = %d \n",ntrpp);
	}
	n2 = ntrpp;
	if(!(nsegy = gettr(&tr))) err("can't get first trace");
	n1=tr.ns; 

       	if (!getparfloat("o1",&oo1)) oo1 = 0.001 * (float)tr.delrt;
       	if (!getparfloat("d1",&d1)) d1 = 0.001 * (float)tr.dt;


	/* if clip computation is needed */
	if (needclip == 1 ) {
		/* determine n3 (number of panels to determin clip) */
		if( clippanel == 0) {
			efseek(infp,0L,2);
        		n3 = (eftell(infp)-3600)/nsegy/n2;
			if ( n3 <1 ) n3=1;
			op = 3600;
		} else {
			op = 3600 + (clippanel-1)*nsegy*n2;
			n3 = 1;
		}
                temp = (float *)malloc(n1*n2*n3*sizeof(float));
		/* read data into temp for clip determination */
		efseek(infp,op,0);
		for(i3=0;i3<n3;i3++) {
			o3 = i3 * n1 * n2;
			for(i2=0;i2<n2;i2++) {
				o2 = i2 * n1 + o3;
				gettr(&tr);
				for(i1=0;i1<n1;i1++) temp[i1+o2] = tr.data[i1];
			}
		}
		ns = n1 * n2 * n3;
		/* determine clips from percentiles */
                perc = 100.0;  getparfloat("perc",&perc);
                if (!getparfloat("pclip",&pclip)) {
                        pperc = perc; getparfloat("pperc",&pperc);
                        is = (ns*pperc/100.0);
                        if (is<0) is = 0;
                        if (is>ns-1) is = ns-1;
                        qkfind(is,ns,temp);
                        pclip = temp[is];
                }
		if (!getparfloat("nclip",&nclip)) {
                        nperc = 100.0-perc;  getparfloat("nperc",&nperc);
                        is = (ns*nperc/100.0);
                        if (is<0) is = 0;
                        if (is>ns-1) is = ns-1;
                        qkfind(is,ns,temp);
                        nclip = temp[is];
                }
                free(temp);
		fprintf(stderr,"pclip=%g nclip=%g \n",pclip,nclip);
	}

	otr = (char*) malloc(n1*sizeof(char)); 

	/* Main loop over segy traces */
	ntrace = 0;
	if (needclip == 1 ) {
		fseek(infp,3600,0);
		gettr(&tr);
	}
	sscale = (nclip!=pclip)?255.0/(pclip-nclip):1.0e10;
	soffset = - nclip*sscale;
	do {
		/* Apply the scale and load in char data */
		for (i1 = 0; i1 < n1; ++i1) { 
			si = soffset + tr.data[i1]*sscale;
			if ( si <0.0 ) si = 0.0;
			if ( si >255.0 ) si = 255.0; 
			otr[i1] = (unsigned char) si;
		}

		/* Output the "movie" data format */
		efwrite(otr,n1,sizeof(char),outfp);
		ntrace = ntrace + 1;
	} while (gettr(&tr));
	fprintf(stderr,"Total Number of Traces Converted = %d \n", ntrace);

	if(ihead==1) {
		n3 = ntrace / n2;
		fflush(outfp);
		bzero((char*)&gh,GHDRBYTES);
		gh.scale=1.;
		putgval(&gh,"dtype",1.);
		putgval(&gh,"n1",(float)n1);
		putgval(&gh,"n2",(float)n2);
		putgval(&gh,"n3",(float)n3);
		putgval(&gh,"gmin",nclip);
		putgval(&gh,"gmax",pclip);
		putgval(&gh,"o1",oo1);
		putgval(&gh,"d1",d1);
		putgval(&gh,"o2",oo2);
		putgval(&gh,"d2",d2);
		putgval(&gh,"o3",oo3);
		putgval(&gh,"d3",d3);
		ierr = fputghdr(outfp,&gh);
	}
		
	fclose(outfp);
	free(otr);

	

	return EXIT_SUCCESS;
}
