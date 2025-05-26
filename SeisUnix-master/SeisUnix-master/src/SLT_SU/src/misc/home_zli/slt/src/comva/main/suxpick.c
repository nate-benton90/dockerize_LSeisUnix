#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
string sdoc =
" 									\n"
" SUXPICK - Segy-data X-window PICK	 			\n"
" 									\n"
" suxpick <stdin [optional parameters]					\n"
" 							        	\n"
" Optional parameters: 							\n"
" d1=			sampling interval 				\n"
" f1=			first sampling time/depth			\n"
" d2=			trace interval 					\n"
" f2=			first trace position 				\n"
" All the above four parameter default from trace headers)	 	\n" 
" panel=1		panel to pick 					\n"
" dtype=0		display type (0=variable density; 1=variable area) \n"
" >>> TYPE xipick OR xwpick FOR MORE PARAMETER SPECIFICATIONS <<<	\n"
" 							        	\n"
" author: Zhiming Li		      		9/12/91			\n"
;
/**************** end self doc *******************************************/


segytrace tr;
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
	char plotcmd[BUFSIZ];	/* build ximage command for popen 	*/
	float *trbuf;		/* trace buffer			 	*/
	FILE *datafp;		/* fp for trace data file		*/
	FILE *plotfp;		/* fp for plot data			*/
	int nt;			/* number of samples on trace		*/
	int ntr;		/* number of traces			*/
	float d1;		/* time/depth sample rate 		*/
	float d2;		/* trace/dx sample rate 		*/
	float f1;		/* tmin/zmin				*/
	float f2;		/* tracemin/xmin	 		*/
	bool seismic;		/* is this seismic data?		*/
	int panel;		/* panel to pick			*/
	int dtype;		/* type of display 			*/
	int ppos;		/* position of the panel		*/
	FILE *infp=stdin;
	int n3,n2,n1,nod2;
	


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);
	

	/* Get info from headers and first trace */
      	fgethdr(infp,&ch,&bh);
	n1 = bh.hns;
	if (bh.tsort==2) {
	   n2 = bh.fold;
	} else {
	   n2 = bh.ntrpr;
	} 
	if (!fgettr(infp,&tr)) err("can't get first trace");
	nt = tr.ns;
	if ( n1!=nt ) 
	   warn("samples/trace in bhdr and trhdr different; trhdr used! \n");
	n1 = nt;
	fseek(infp,0L,2);
	n3=(ftell(infp)-EBCBYTES-BNYBYTES)/(n1*sizeof(float)+HDRBYTES)/n2;
        if(n3==0) {
	   n3=1;
	   n2=(ftell(infp)-EBCBYTES-BNYBYTES)/(n1*sizeof(float)+HDRBYTES);
	   warn("less traces were found in input! \n");
	}
	fseek(infp,0L,0);
 
	seismic =  (tr.trid == 0 || tr.trid == TREAL);

	if (!getparint("panel", &panel)) panel=1;
	if (!getparint("dtype", &dtype)) dtype=0;
	if (!getparfloat("d1", &d1)) {
		if (seismic) {
			if (tr.dz!=0.) {
				d1 = tr.dz;
			} else if (tr.dt) {
				d1 = (float) tr.dt / 1000000.0;
				if (tr.dt<1000) d1 = tr.dt;
			} else {
				d1 = 0.004;
				warn("tr.dt not set, assuming dt=0.004");
			}
		} else { /* non-seismic data */
			if (tr.d1) {
				d1 = tr.d1;
			} else {
				d1 = 1.0;
				warn("tr.d1 not set, assuming d1=1.0");
			}
		}
	}

	nod2 = 0;
	if (!getparfloat("d2", &d2)) {
	   nod2 = 1;
	   if(bh.tsort==2) {
	      d2 = tr.offset;
	   }
	   else {
	      d2 = tr.cdp;
	   }
	}

	if (!getparfloat("f1", &f1)) {
		if (seismic) {
			f1 = (tr.delrt) ? (float) tr.delrt/1000.0 : 0.0;
		        if(tr.delrt<1000) f1=tr.delrt;
			if(tr.dz!=0.) f1=tr.fz;
		} else {
			f1 = (tr.f1) ? tr.f1 : 0.0;
		}
	}

	if (!getparfloat("f2", &f2)) {
		if (bh.tsort==2) {
	           f2 = tr.offset;
		}
		else {
	           f2 = tr.cdp;
		}
	}


	/* Allocate trace buffer */
	trbuf = ealloc1float(nt);


	/* Create temporary "file" to hold data */
	datafp = etempfile(NULL);


	/* Loop over input traces & put them into the data file */
	ntr = 0;
     fseek(infp,EBCBYTES+BNYBYTES+(panel-1)*n2*(n1*sizeof(float)+HDRBYTES),0);


	for(ntr=0;ntr<n2;ntr++) {
	    	if(!fgettr(infp,&tr)) err("get trace error \n");
	    	efwrite(tr.data, FSIZE, nt, datafp);
	    	if(ntr==1 && nod2==1 ) {
			if (bh.tsort==2) {
                                d2 = tr.offset-d2;
                                if (!getparint("ppos", &ppos)) ppos = tr.cdp;
                        } else {
                                d2 = tr.cdp-d2;
                                if (!getparint("ppos", &ppos)) ppos = tr.offset;
                        }

	    	}
	}




	/* Set up xipick or xwpick command line */
	if ( dtype == 0 ) {
	   sprintf(plotcmd,
"xipick n1=%d n2=%d d1=%f d2=%f f1=%f f2=%f ppos=%d",
		    n1, n2, d1, d2, f1, f2, ppos);
	} else {
	   sprintf(plotcmd,
"xwpick n1=%d n2=%d d1=%f d2=%f f1=%f f2=%f ppos=%d",
		    n1, n2, d1, d2, f1, f2, ppos);
	}

	for (--argc, ++argv; argc; --argc, ++argv) {
		if (strncmp(*argv, "d1=", 3) && /* skip those already set */
		    strncmp(*argv, "d2=", 3) &&
		    strncmp(*argv, "f1=", 3) &&
		    strncmp(*argv, "f2=", 3)) {
		    
			strcat(plotcmd, " ");   /* put a space between args */
			strcat(plotcmd, "\"");  /* user quotes are stripped */
			strcat(plotcmd, *argv); /* add the arg */
			strcat(plotcmd, "\"");  /* user quotes are stripped */
		}
	}


	/* Open pipe; read data to buf; write buf to plot program */
	plotfp = epopen(plotcmd, "w");
	rewind(datafp);
	{ register int itr;
		for (itr = 0; itr < ntr; ++itr) {
			efread (trbuf, FSIZE, nt, datafp);
			efwrite(trbuf, FSIZE, nt, plotfp);
		}
	}


	/* Clean up */
	epclose(plotfp);
	efclose(datafp);


	return EXIT_SUCCESS;
}
