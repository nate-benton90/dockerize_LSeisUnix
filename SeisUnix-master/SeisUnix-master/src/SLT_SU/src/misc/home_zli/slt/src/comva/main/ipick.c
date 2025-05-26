#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
string sdoc =
" 									\n"
" IPICK - INTERFACE FILE EDIT of Segy-data on X-window 			\n"
" 									\n"
" ipick <stdin [optional parameters]					\n"
" 							        	\n"
"X Functionality:\n"
"Button 1       Zoom with rubberband box\n"
"Button 2       Show mouse (x1,x2) coordinates while pressed\n"
"Button 3       Same as key 'a'                        \n"
"Q key          Quit (can also use Motif Action button)\n"
"a key          Append current mouse (x1,x2) location to end of picks \n"
"i key          Insert current mouse (x1,x2) location into middle of picks \n"
"d key          Delete current mouse (x1,x2) location from picks \n"
"p key          Display picks \n"
"s key          Save IFILE \n"
"c key          Clear all saved mouse (x1,x2) picks of the current horizon\n"
"f key          go to next horizon      \n"
"b key          go to previous horizon  \n"
"1-9 key        go to horizon 1-9	\n"
"0 key          go to horizon 10        \n"
"F1 key         go to horizon 11        \n"
"F2 key         go to horizon 12        \n"
"Fi key         go to horizon i+10      \n"
"F12 key        go t2o horizon 22        \n"
"T key          edit top velocity of current horizon \n"
"               (Dashed line --- previous; Solid line --- edited; 	\n"
"                to save edited velocity, use S key; to exit, use Q key; \n"
"                to edit, use right button of mouse;			\n"
"                to show mouse location, use button 2 of mouse)		\n"
"B key          edit bottom velocity of current horizon \n"
"               (Dashed line --- previous; Solid line --- edited; 	\n"
"                to save edited velocity, use S key; to exit, use Q key; \n"
"                to edit, use right button of mouse;			\n"
"                to show mouse location, use button 2 of mouse)		\n"
"\n"
"Required parameters:							\n"
"NONE									\n"
"Optional parameters: 							\n"
"f1=tr.fz               depth of first sample 				\n"
"d1=tr.dz               depth sampling interval 			\n"
"f2=                    lateral position of first trace 		\n"
"d2=                    trace interval 					\n"
"xmin=                  minimum x position of the section               \n"
"xmax=                  maximum x position of the section		\n"
"zmin=                  minimum z position of the section               \n"
"zmax=                  maximum z position of the section		\n"
"  (All the above eight parameter are from trace headers, by default) 	\n" 
"VELO=NULL              average velocity VELO card and interval velocity \n"
"                        gradient DVDZ card file name              \n"
"                       (if not given, i.e., NULL, the picking will output \n"
"                        the positions of the horizons with velocity \n"
"                        fields being interpolated from those in input 	\n"
"                        ifile/hfile if present;			\n"
"                        if given, the velocity fields will be computed \n"
"                        from VELO cards once, before editing with T or B \n"
"                        keys, or before s key if T and B keys are not used)\n" 
"n1=from binary header   number of samples per trace \n"
"n2=from binary header   number of traces per section \n"
"panel=1                 panel to perform the pick \n"
"\n"
"See iipick for specifications of other parameters 			\n"
"NOTE: \n"
" 1. Interface and velocity picks output format will follow the ifile format \n"
" 2. This program is for picking IFILE on stack or constant offset sections. \n"
"    Number of traces per panel or record (ntrpr) in input binary header \n"
"    must be correctly specified. This number is used to determine number \n"
"    of traces to display on screen for picking.                         \n"
" 3. It is a good idea to save IFILE (using s key) once a while during 	\n"
"    picking, to avoid loss of picked results due to system crash.	\n"    
" 4. tr.fz and tr.dz are depth of firs sample and depth interval of input \n"
"    depth section.							\n"
" 5. If fz, dz, sx, gx, scalco in the trace headers, and hns and ntrpr	\n"
"    in the binary header are not defined correctly, f1, d1, f2, d2, n1	\n"
"    and n2 must be supplied as input parameters.			\n"   
"									\n"
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
	int ppos;		/* position of the panel		*/
	FILE *infp=stdin;
	int n3,n2,n1;
	float xmin,zmin,xmax,zmax;
	int cdpxmin,cdpxmax;
	char zattrib[80], dunits[80];
	


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);
	

	/* Get info from headers and first trace */
      	fgethdr(infp,&ch,&bh);
	if (!getparint("n1", &n1)) n1 = bh.hns;
	if (!getparint("n2", &n2)) n2 = bh.ntrpr;

	if(bh.mfeet==2) {
		sprintf(dunits,"%s\0","FEET");
	} else {
		sprintf(dunits,"%s\0","METERS");
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
	   n2 = (ftell(infp)-EBCBYTES-BNYBYTES)/(n1*sizeof(float)+HDRBYTES);
	   warn("less traces were found in input! \n");
	}
	fseek(infp,0L,0);
 
	seismic =  (tr.trid == 0 || tr.trid == TREAL);

	if (!getparint("panel", &panel)) panel=1;



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


	if (!getparfloat("f1", &f1)) {
		if (seismic) {
			f1 = (tr.delrt) ? (float) tr.delrt/1000.0 : 0.0;
		        if(tr.delrt<1000) f1=tr.delrt;
			if(tr.dz!=0.) f1=tr.fz;
		} else {
			f1 = (tr.f1) ? tr.f1 : 0.0;
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
		if(ntr==0) {
		   if (!getparfloat("xmin", &xmin)) {
			xmin = (tr.sx+tr.gx) * .5;
			if(tr.scalco>1) {
				xmin = xmin * tr.scalco; 
			} else if(tr.scalco<0) {
				xmin = xmin / (-tr.scalco); 
			}
		   }
	           if (!getparfloat("zmin", &zmin)) zmin = tr.fz;
	           if (!getparfloat("zmax", &zmax)) zmax = zmin+(n1-1)*d1;
		   if (!getparfloat("f2", &f2)) f2 = xmin; 
		   if (!getparfloat("d2", &d2)) d2 = xmin;
	           if (!getparint("cdpxmin", &cdpxmin)) cdpxmin = tr.cdp;
		}
	    	if(ntr==1) {
		   if (!getparfloat("d2",&d2)) {
			if(tr.scalco>1) {
				d2=fabs((tr.sx+tr.gx)*.5*tr.scalco-d2);
			} else if(tr.scalco<0) {
				d2=fabs((tr.sx+tr.gx)*.5/(-tr.scalco)-d2);
			} else {
				d2=fabs((tr.sx+tr.gx)*.5-d2);
			}
		   }	
		   if (!getparint("ppos", &ppos)) ppos = tr.offset;
	    	}
		if(ntr==n2-1) {
		   if (!getparfloat("xmax", &xmax)) {
			xmax = (tr.sx+tr.gx) * .5;
			if(tr.scalco>1) {
				xmax = xmax * tr.scalco; 
			} else if(tr.scalco<0) {
				xmax = xmax / (-tr.scalco); 
			}
		   }
		   if(xmax<=xmin) xmax = xmin + (n2-1)*d2;
	           if (!getparint("cdpxmax", &cdpxmax)) cdpxmax = tr.cdp;
		}
	}



	if(tr.dz==0) {
		sprintf(zattrib,"%s\0","TIME SECTION"); 
	} else {
		sprintf(zattrib,"%s\0","DEPTH SECTION"); 
	}

	/* Set up iipick command line */

	sprintf(plotcmd,
"iipick n1=%d n2=%d d1=%f d2=%f f1=%f f2=%f ppos=%d xmin=%f xmax=%f zmin=%f zmax=%f cdpxmin=%d cdpxmax=%d zattrib=%s dunits=%s ",
	    n1, n2, d1, d2, f1, f2, ppos,xmin,xmax,zmin,zmax,cdpxmin,cdpxmax,
	    zattrib,dunits);

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
