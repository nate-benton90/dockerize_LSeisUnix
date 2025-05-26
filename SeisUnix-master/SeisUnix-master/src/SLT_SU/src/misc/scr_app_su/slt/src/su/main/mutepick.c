#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
string sdoc =
" 									\n"
" MUTEPICK - Segy-data Mute (Top/Bottom) PICK on X-Window		\n"
" 									\n"
" mutepick <stdin [optional parameters]					\n"
" 							        	\n"
"X Functionality:							\n"
"Button 1       Zoom with rubberband box				\n"
"Button 2       Show mouse (x1,x2) location while pressed		\n"
"Button 3       The same as a or A (depends on previous key) 		\n"
"Q key          Quit (can also use Motif Action button)			\n"
"a key          Append current mouse location to end of 		\n"
"			the top-zone mute picks 			\n"
"i key          Insert current mouse location in middle of 		\n"
"			the top-zone mute picks				\n"
"d key          Delete current mouse location from 		 	\n"
"			the top-zone mute picks				\n"
"A key          Append current mouse location to end of 		\n"
"			the bottom-zone mute picks 			\n"
"I key          Insert current mouse location in middle of 		\n"
"			the bottom-zone mute picks			\n"
"D key          Delete current mouse location from 		 	\n"
"			the bottom-zone mute picks			\n"
"p key          Display picks  						\n"
"s key          Append saved MUTE picks to pick file 			\n"
"\n"
"Optional parameters: 							\n"
"mpicks=stderr          file to save MUTE picks (picks will be appended to \n"
"                         end of the file. (default: screen output) \n"
"d1=                    sampling interval 				\n"
"f1=                    first sampling time/depth			\n"
"d2=                    trace interval 					\n"
"f2=                    first trace position 				\n"
"All the above four parameter default from trace headers)	 	\n" 
"panel=1                panel to pick 					\n"
"ntpp=fold or ntrpr     number of traces per panel (=fold for cdp input	\n"
"                       =ntrpr for other input --- number of traces per \n"
"                       record)						\n" 
"dtype=0                display type (0=variable density; 1=variable area) \n"
"itoff=0   output offset-time when itoff=0; output time-offset when \n"
"          itoff=1 \n"
"perc=100.0             percentile used to determine clip\n"
"clip=(perc percentile) clip used to determine bclip and wclip\n"
"bperc=perc             percentile for determining black clip value\n"
"wperc=100.0-perc       percentile for determining white clip value\n"
"bclip=clip             data values outside of [bclip,wclip] are clipped\n"
"wclip=-clip            data values outside of [bclip,wclip] are clipped\n"
"cmap=gray              gray, hue, or default colormaps may be specified\n"
"verbose=1              =1 for info printed on stderr (0 for no info)\n"
"xbox=50                x in pixels of upper left corner of window\n"
"ybox=50                y in pixels of upper left corner of window\n"
"wbox=550               width in pixels of window\n"
"hbox=700               height in pixels of window\n"
"x1beg=x1min            value at which axis 1 begins\n"
"x1end=x1max            value at which axis 1 ends\n"
"d1num=0.0              numbered tic interval on axis 1 (0.0 for automatic)\n"
"f1num=x1min            first numbered tic on axis 1 (used if d1num not 0.0)\n"
"n1tic=1                number of tics per numbered tic on axis 1\n"
"grid1=none             grid lines on axis 1 - none, dot, dash, or solid\n"
"label1=time (or depth) label on axis 1\n"
"x2beg=x2min            value at which axis 2 begins\n"
"x2end=x2max            value at which axis 2 ends\n"
"d2num=0.0              numbered tic interval on axis 2 (0.0 for automatic)\n"
"f2num=x2min            first numbered tic on axis 2 (used if d2num not 0.0)\n"
"n2tic=1                number of tics per numbered tic on axis 2\n"
"grid2=none             grid lines on axis 2 - none, dot, dash, or solid\n"
"label2=offset (or cdp) label on axis 2\n"
"labelfont=Erg14        font name for axes labels\n"
"title=Mute Picking     title of plot\n"
"titlefont=Rom22        font name for title\n"
"labelcolor=blue        color for axes labels\n"
"titlecolor=red         color for title\n"
"gridcolor=blue         color for grid lines\n"
"topmutecolor=blue      color for top-zone mute picks \n"
"bottommutecolor=red    color for bottom-zone mute picks \n"
"style=seismic          normal (axis 1 horizontal, axis 2 vertical) or\n"
"labelfont=Erg14        font name for axes labels\n"
"                       seismic (axis 1 vertical, axis 2 horizontal)\n"
"Following four parameters are for dtype=1	\n"
"xcur=1.0               wiggle excursion in traces corresponding to clip\n"
"wt=1                   =0 for no wiggle-trace; =1 for wiggle-trace\n"
"va=1                   =0 for no variable-area; =1 for variable-area fill\n"
"nbpi=72                number of bits per inch at which to rasterize\n"
"\n"
"NOTE: \n"
"Mute Picks Output Card Format: \n"
"1---5---10---15----21----27----33----39----45----51----57----63----69----75\n"
"MUTE       ppos   px1   tp1   bt1   px2   tp2   bp2   px3   tp3   bt3 \n"
"\n"
"where          ppos indicates mute pick panel position (cdp or offset) \n"
"               pxi, i=1,2,..., are mute time x position (offset or cdp) \n"
"               tpi, i=1,2,..., are mute time at top-mute zone \n"
"               bti, i=1,2,..., are mute time at bottom-mute zone \n"
"\n"
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
	int n3,n2,n1;
	


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);
	

	/* Get info from headers and first trace */
      	fgethdr(infp,&ch,&bh);
	n1 = bh.hns;
	if(!getparint("ntpp",&n2)) {
		if (bh.tsort==2) {
	  		 n2 = bh.fold;
		} else {
	   		n2 = bh.ntrpr;
		} 
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
			/* sampling interval in ms or in m (ft) */
			if ( tr.dz!=0. ) { 
				d1 = tr.dz;	
			} else if (tr.dt) {
				d1 = (float) tr.dt / 1000.0;
				if (tr.dt<1000) d1 = tr.dt;
			} else {
				d1 = 0.004 * 1000.;
				warn("tr.dt not set, assuming dt=4");
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

	if (!getparfloat("d2", &d2)) {
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
	    	if(ntr==1) {
			if (bh.tsort==2) {
	       			if(!getparfloat("d2",&d2)) d2 = tr.offset-d2;
		  		if (!getparint("ppos", &ppos)) ppos = tr.cdp;
			} else {
	       			if(!getparfloat("d2",&d2)) d2 = tr.cdp-d2;
		  		if (!getparint("ppos", &ppos)) ppos = tr.offset;
			}
	    	}
	}



	/* Set up xipick or xwpick command line */
	if ( dtype == 0 ) {
	   sprintf(plotcmd,
"mipick n1=%d n2=%d d1=%f d2=%f f1=%f f2=%f ppos=%d",
		    n1, n2, d1, d2, f1, f2, ppos);
	} else {
	   sprintf(plotcmd,
"mwpick n1=%d n2=%d d1=%f d2=%f f1=%f f2=%f ppos=%d",
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
