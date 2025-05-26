#include "comva.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
string sdoc =
" 									\n"
" VPICK - Segy-data Velocity Semblance PICK on X-Window		\n"
" 									\n"
" vpick <semblance [optional parameters]				\n"
" 							        	\n"
"X Functionality:							\n"
"Button 1 (L)   Zoom with rubberband box				\n"
"Button 2 (M)   Show mouse (x1,x2) location while pressed		\n"
"Button 3 (R)   Append current mouse location to end of 		\n"
"			the velocity picks 			\n"
"Q key          Quit (can also use Motif Action button)			\n"
"i key          Insert current mouse location in middle of 		\n"
"			the velocity picks			\n"
"d key          Delete the pick closest to the current mouse location 	\n"
"                       from the velocity picks				\n"
"p key          Display velocity picks (in solid lines)			\n"
"r key          Display interval velocity curves (in dashed lines)	\n"
"s key          Append saved velocity picks to pick file 		\n"
"A key          Analysis of interval velocity gradient (move mouse above\n"
"                       the top of analyzed zone before typing A key; type \n"
"                       A key; then move the mouse to below the bottom of \n"
"                       analyzed zone; then press left button of the mouse) \n"
"D key          Delete the gradient analysis at the zone indicated by  	\n"
"                       the mouse location 				\n"
"\n"
"Optional parameters: 							\n"
"mpicks=stderr          file to save VELO and DVDZ picks (picks will be \n"
"                       appended at end of the file. (default: to screen) \n"
"d1=                    sampling interval (depth or time) 		\n"
"f1=                    first sampling depth/time			\n"
"d2=                    trace interval (velocity scan interval)		\n"
"f2=                    first trace position (first scan velocity)	\n"
"\n"
"(All the above four parameter default from trace headers, if not given)\n" 
"\n"
"vmind=f2               minimum velocity to display 	           \n"
"vmaxd=f2+(nv-1)*d2     maximum velocity to display 	           \n"
"cdp=1                  cdp number to pick 				\n"
"fcdpvelan=1            first cdp number of input velan (semblance) data set \n"
"dcdpvelan=1            cdp increment of input velan (semblance) data set \n"
"cdppre=0               cdp position of picked velan from mpicks \n"
"                       to be used as the previous pick \n"
"                       (0=closest one on mpicks)		\n"
"perc=100.0             percentile used to determine clip\n"
"clip=(perc percentile) clip used to determine bclip and wclip\n"
"bperc=perc             percentile for determining black clip value\n"
"wperc=100.0-perc       percentile for determining white clip value\n"
"bclip=clip             data values outside of [bclip,wclip] are clipped\n"
"wclip=-clip            data values outside of [bclip,wclip] are clipped\n"
"cmap=rgb               rgb, gray, or hue colormaps may be specified\n"
"verbose=1              =1 for info printed on stderr (0 for no info)\n"
"xbox=50                x in pixels of upper left corner of window\n"
"ybox=50                y in pixels of upper left corner of window\n"
"wbox=700               width in pixels of window\n"
"hbox=800               height in pixels of window\n"
"x1beg=x1min            value at which axis 1 begins\n"
"x1end=x1max            value at which axis 1 ends\n"
"d1num=0.0              numbered tic interval on axis 1 (0.0 for automatic)\n"
"f1num=x1min            first numbered tic on axis 1 (used if d1num not 0.0)\n"
"n1tic=1                number of tics per numbered tic on axis 1\n"
"grid1=none             grid lines on axis 1 - none, dot, dash, or solid\n"
"label1=depth (or time) label on axis 1\n"
"x2beg=x2min            value at which axis 2 begins\n"
"x2end=x2max            value at which axis 2 ends\n"
"d2num=0.0              numbered tic interval on axis 2 (0.0 for automatic)\n"
"f2num=x2min            first numbered tic on axis 2 (used if d2num not 0.0)\n"
"n2tic=1                number of tics per numbered tic on axis 2\n"
"grid2=none             grid lines on axis 2 - none, dot, dash, or solid\n"
"label2=velocity        label on axis 2\n"
"labelfont=Erg14        font name for axes labels\n"
"title=Velocity Picking title of plot\n"
"titlefont=Rom22        font name for title\n"
"labelcolor=blue        color for axes labels\n"
"titlecolor=red         color for title\n"
"gridcolor=blue         color for grid lines\n"
"style=seismic          normal (axis 1 horizontal, axis 2 vertical) or\n"
"labelfont=Erg14        font name for axes labels\n"
"                       seismic (axis 1 vertical, axis 2 horizontal)\n"
"vpickcolor=red         color for velocity picks \n"
"vrefcolor=green        color for reference velocity curve \n"
"                       (interval velocities are in dashed lines)	\n"
"vpcrdcolor=purple      color for previous velocity picks read from mpicks \n"
"ifile=NONE             file name of IFILE or HFILE			\n"
"vgfile=NONE            file name of reference velocity grid file	\n"
"ivgtype=0              type of reference velocity (0=interval 1=average) \n"
"fcdpvg=from-header     first cdp number of the velocity grid file	\n"
"dcdpvg=from-header     cdp number increment of the velocity grid file	\n"
"fzvg=from-header       first depth  of the velocity grid file	\n"
"dzvg=from-header       depth interval of the velocity grid file	\n"
"nzvg=from-header       number of depths in the velocity grid file	\n"
"ivmode=0               mode of converting average velocity picks to 	\n"
"                       interval velocity curve  			\n"
"                       (0=Constant interval velocity between two picks \n"
"                        1=Linear interpolation of average velocity picks \n"
"                          to depth interval of dzvg before converting to \n"
"                          interval velocity, when gradient analysis is not\n"
"                          performed; 					\n"
"                          Use two adjacent average velocity picks and 	\n"
"			   interval velocity gradient information, to  	\n"
"                          determine linearly-varying interval velocity \n"
"                          function, when gradient analysis is performed) \n" 
"avplo=80               automatic-velocity-pick-search low-end percentage \n" 
"avphi=120              automatic-velocity-pick-search high-end percentage \n" 
"ivof=0                 input velan type and output velocity card type;	\n"
"                       (0=depth migration velan and output VELO card	\n"
"                       (1=time stacking velan and output VELF card	\n"
"\n"
"NOTE: \n"
" 1. Velocity Picks Output Card Format: (right-adjust) \n"
"1---5----11--15----21----27----33----39----45----51----57----63----69----75 \n"
"VELO    cdp        z1    v1    z2    v2    z3    v3    z4    v4    z5    v5 \n"
"\n"
"where          cdp indicates cdp number of current panel \n"
"               zi, i=1,2,..., are depth (or time)  \n"
"               vi, i=1,2,..., are velocity at zi \n"
" 2. Interval Velocity Gradient Output Card Format: (right-adjust) \n"
"1---5----11--15----21----27----33----39----45----51----57----63----69----75 \n"
"DVDZ    cdp       zt1   zb1 dvdz1   zt2   zb2 dvdz2   zt3   zb3 dvdz3 	 \n"
"\n"
"where          cdp indicates cdp number of current panel \n"
"               zti, i=1,2,..., are depth of top of gradient analysis zone \n"
"               zbi, i=1,2,..., are depth of bottom of gradient analysis zone\n"
"               dvdzi, i=1,2,..., are interval velocity gradient at [zti,zbi]\n"
" 3. if vgfile (reference velocity grid file) is given, and fcdpvg, dcdpvg, \n"
" fzvg, dzvg, and nzvg are specified properly, the reference velocity (e.g., \n"
" previous migration velocity) curve will be displayed on top of the velan. \n"
" By default, picked average velocity displayed in blue, reference average \n"
" velocity in green, picked interval velocity in dashed blue, reference \n"
" interval velocity in dashed green  		\n" 
" 4. number of velocities per semblance is obtained from parameter--fold in \n"
" the binary header. velocities are obtained from parameter--offset in \n"
" the trace header \n" 
" 5. Automatic velocity pick will be performed, only when ifile is input. \n"
" When vgfile is present, avplo and avphi will be used to limit the search \n"
" range with central velocity being the reference velocity (vgfile input). \n" 
" When vgfile is not given, automatic pick will search through the whole \n"
" velocity range of VELAN input.					\n" 
" 6. When vgfile is given and it includes header, fzvg, dzvg, fzvg, fcdpvg \n"
" and dcdpvg will default to the values in the grid header. When vgfile is \n"
" nonstandard (no header file), these parameters will default to 	\n"
" 	fzvg = 0.							\n"
" 	dzvg = must be specified 					\n"
" 	nzvg = must be specified 					\n"
" 	fcdpvg = 1							\n"
" 	dcdpvg = 1							\n"
"\n"
" author: Zhiming Li		      		10/3/91			\n"
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
	int ppos;		/* position of the panel		*/
	int cdp;		/* cdp position to pick			*/
	int dcdpvelan;		/* cdp increment on velan data input    */ 
	int fcdpvelan;		/* 1st cdp number on velan data input    */ 
	FILE *infp=stdin;
	int n3,n2,n1;
	int n2n, n20, i1, itr;
	float vmind,vmaxd, temp;
	


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);
	

	/* Get info from headers and first trace */
      	fgethdr(infp,&ch,&bh);
	n1 = bh.hns;
	if (bh.tsort==2) {
	   n2 = bh.fold;
	} else {
	   err("input must be cdp ordered");
	} 
	getparint("n1", &n1);
	getparint("n2", &n2);
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

	if (!getparint("cdp", &cdp)) cdp=1;
	if (!getparint("dcdpvelan", &dcdpvelan)) dcdpvelan=1;
	if (!getparint("fcdpvelan", &fcdpvelan)) fcdpvelan=1;

	if (!getparint("panel", &panel)) panel=(cdp-fcdpvelan)/dcdpvelan + 1;

	if (!getparfloat("d1", &d1)) {
		if (seismic) {
			/* sampling interval in ms or in m (ft) */
			if (tr.dz!=0.) {
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

	if (!getparfloat("d2", &d2)) d2 = tr.offset;

	if (!getparfloat("f1", &f1)) {
		if (seismic) {
			f1 = (tr.delrt) ? (float) tr.delrt/1000.0 : 0.0;
		        if(tr.delrt<1000) f1=tr.delrt;
			if(tr.dz!=0.) f1=tr.fz;
		} else {
			f1 = (tr.f1) ? tr.f1 : 0.0;
		}
	}

	if (!getparfloat("f2", &f2)) f2 = tr.offset;


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
	       		d2 = tr.offset-d2;
		  	if (!getparint("ppos", &ppos)) ppos = tr.cdp;
	    	}
	}


	if (!getparfloat("vmind", &vmind)) vmind = f2;
	if (!getparfloat("vmaxd", &vmaxd)) vmaxd = f2+(n2-1)*d2; 
	
	temp = (vmind-f2)/d2-.5;
	n20 = temp;
	vmind = f2 + n20 * d2;
	temp = (vmaxd-f2)/d2+1.;
	n2n = temp;
	vmaxd = f2 + (n2n-1) * d2;

	n2 = n2n - n20;
	f2 = vmind;


	/* Set up xipick or xwpick command line */
	sprintf(plotcmd,
		"vipick n1=%d n2=%d d1=%f d2=%f f1=%f f2=%f ppos=%d",
		 n1, n2, d1, d2, f1, f2, ppos);

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
		
	
	for (itr = n20; itr < n2n; ++itr) {
		if(itr<0 || itr>ntr) {
			for (i1=0; i1<n1; i1++) trbuf[i1] = 0.;
		} else {
			efseek(datafp,itr*nt*FSIZE, 0);
			efread (trbuf, FSIZE, nt, datafp);
		}
		efwrite(trbuf, FSIZE, nt, plotfp);
	}


	/* Clean up */
	epclose(plotfp);
	efclose(datafp);


	return EXIT_SUCCESS;
}
