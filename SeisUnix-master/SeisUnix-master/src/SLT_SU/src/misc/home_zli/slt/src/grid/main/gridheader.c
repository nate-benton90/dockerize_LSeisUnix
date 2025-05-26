/* GRIDHEADER print/update/add header values of an evenly sampled grid file*/
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "par.h"

char *sdoc = 
"GRIDHEADER - print/update/add/remove header values of a grid file	\n"
"\n"
"gridheader [parameters] <grid.input >grid.output			\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"hdremove=0     remove header at output (0=no 1=yes)			\n"
"scale=         scale to be applied to header values			\n"
"dtype=         grid data type (2=short; 4=float; 8=complex)		\n"
"n1=            number of samples along 1st dimension 			\n"
"               (e.g., number of depths) in the grid.input file		\n"
"n2=            number of samples along 2nd dimension 			\n"
"               (e.g., number of cdps) in  the grid.input file		\n"
"n3=            number of samples along 3nd dimension 			\n"
"               (e.g., number of lines) in  the grid.input file		\n"
"n4=            number of samples along 4th dimension 			\n"
"n5=            number of samples along 5th dimension 			\n"
"d1=            sample interval along 1st dimension 			\n"
"               (e.g., depth interval) in the grid.input file		\n"
"d2=            sample interval along 2nd dimension 			\n"
"               (e.g., cdp increment) in the grid.input file		\n"
"d3=            sample interval along 3rd dimension 			\n"
"               (e.g., line increment) in the grid.input file		\n"
"d4=            sample interval along 4th dimension 			\n"
"d5=            sample interval along 5th dimension 			\n"
"o1=            minimum coordinate along 1st dimension 			\n"
"               (e.g., starting depth) in the grid.input file		\n"
"o2=            minimum coordinate along 2nd dimension 			\n"
"               (e.g., starting cdp) in the grid.input file		\n"
"o3=            minimum coordinate along 3rd dimension 			\n"
"               (e.g., starting line) in the grid.input file		\n"
"o4=            minimum coordinate along 4th dimension 			\n"
"o5=            minimum coordinate along 5th dimension 			\n"
"dcdp2=         cdp number increment along the 2nd dimension		\n"
"dline3=        line number increment along the 3rd dimension		\n"
"ocdp2=         starting cdp number along the 2nd dimension		\n"
"oline3=        starting line number along the 3rd dimension		\n"
"gmin=          minimum data value of the grid 				\n"
"gmax=          maximum data value of the grid 				\n"
"orient=        grid axes orientation  					\n"
"               0=not defined;						\n"
"               1=time(depth)-inline-crossline				\n" 
"               2=time(depth)-crossline-inline				\n" 
"               3=inline-time(depth)-crossline				\n" 
"               4=inline-crossline-time(depth)				\n" 
"               5=crossline-time(depth)-inline				\n" 
"               6=crossline-inline-time(depth)				\n" 
"gtype=         grid type   						\n"
"               0=not defined;						\n"
"               1=time-rms velocity					\n" 
"               2=time-average velocity					\n" 
"               3=time-interval velocity				\n" 
"               4=depth 		 				\n" 
"\n"
"NOTE: \n"
" 1. If any one of the above 24 grid parameters is given, the program \n"
"    updates (if the header exists in the grid.input file)  or adds (if \n"
"    the header does not exists in the grid.input file) header to the 	\n"
"    grid.output file.							\n"
" 2. If none of the above 24 grid parameters is given and hdremove=0, 	\n"
"    the program prints the header values of the grid.input file. 	\n"
"    No grid.output is needed. 						\n"
" 3. When hdremove=1, the input grid will be copied to output with 	\n"
"    the header removed if input contains a header.			\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	8/24/92   \n"		    
;

main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp;

    	int n1=0,n2=0,n3=1,n4=1,n5=1;
    	float o1=0,o2=0,o3=0,o4=0,o5=0,d1=0,d2=0,d3=0,d4=0,d5=0;
	float scale=0; 
	int dtype=0, dtypeout;
	float ocdp2=0,dcdp2=0,oline3=0,dline3=0,gmin=0,gmax=0;
	int orient=0, gtype=0;
	int hdremove;

	int iout=0, i, i1, ierr;
	char *trace, *traceout;

	ghed gh;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* get input grid parameters */
    	ierr = fgetghdr(infp,&gh);
	if(ierr==0) {

		fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
                	&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                	&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
			&orient,&gtype);

		fprintf(stderr," === From input grid header === \n");
		fprintf(stderr," scale=%g dtype=%d \n",scale,dtype);
		fprintf(stderr," n1=%d n2=%d n3=%d n4=%d n5=%d \n",
			n1,n2,n3,n4,n5);
		fprintf(stderr," d1=%g d2=%g d3=%g d4=%g d5=%g \n",
			d1,d2,d3,d4,d5);
		fprintf(stderr," o1=%g o2=%g o3=%g o4=%g o5=%g \n",
			o1,o2,o3,o4,o5);
		fprintf(stderr," dcdp2=%g dline3=%g ocdp2=%g oline3=%g \n",
			dcdp2,dline3,ocdp2,oline3);
		fprintf(stderr," gmin=%g gmax=%g orient=%d gtype=%d\n", 
			gmin,gmax,orient,gtype);
	} else {
		warn(" warning: fgetghdr error; input grid nonstandard \n");
		dtype = 4;
	}
	
	/* get update parameters */
	if (getparfloat("scale", &scale)) iout = 1;
	dtypeout=dtype; if (getparint("dtype", &dtypeout)) iout = 1;
	if (getparint("n1", &n1)) iout = 1;
	if (getparint("n2", &n2)) iout = 1;
	if (getparint("n3", &n3)) iout = 1;
	if (getparint("n4", &n4)) iout = 1;
	if (getparint("n5", &n5)) iout = 1;
	if (getparfloat("d1", &d1)) iout = 1;
	if (getparfloat("d2", &d2)) iout = 1;
	if (getparfloat("d3", &d3)) iout = 1;
	if (getparfloat("d4", &d4)) iout = 1;
	if (getparfloat("d5", &d5)) iout = 1;
	if (getparfloat("o1", &o1)) iout = 1;
	if (getparfloat("o2", &o2)) iout = 1;
	if (getparfloat("o3", &o3)) iout = 1;
	if (getparfloat("o4", &o4)) iout = 1;
	if (getparfloat("o5", &o5)) iout = 1;
	if (getparfloat("dcdp2", &dcdp2)) iout = 1;
	if (getparfloat("dline3", &dline3)) iout = 1;
	if (getparfloat("ocdp2", &ocdp2)) iout = 1;
	if (getparfloat("oline3", &oline3)) iout = 1;
	if (getparfloat("gmin", &gmin)) iout = 1;
	if (getparfloat("gmax", &gmax)) iout = 1;
	if (getparint("orient", &orient)) iout = 1;
	if (getparint("gtype", &gtype)) iout = 1;
	if (!getparint("hdremove", &hdremove)) {
		hdremove = 0;
	} else {
		iout = 1;
	}
	
	if(iout==1) {
		if(hdremove==0) {
		     	if(scale==0.) 
			      err(" scale must be given to update grid header");
			if(dtypeout==0) 
			      err(" dtype must be given to update grid header");
			if(n1==0) 
				err(" n1 must be given to update grid header ");
			if(n2==0) 
				err(" n2 must be given to update grid header ");
			if(n3==0) 
				err(" n3 must be given to update grid header ");
			if(n4==0) 
				err(" n4 must be given to update grid header ");
			if(n5==0) 
				err(" n5 must be given to update grid header ");
			if(ierr!=0) {
				if (!getparfloat("d1", &d1)) d1=0.;
				if (!getparfloat("d2", &d2)) d2=0.;
				if (!getparfloat("d3", &d3)) d3=0.;
				if (!getparfloat("d4", &d4)) d4=0.;
				if (!getparfloat("d5", &d5)) d5=0.;
				if (!getparfloat("o1", &o1)) o1=0.;
				if (!getparfloat("o2", &o2)) o2=0.;
				if (!getparfloat("o3", &o3)) o3=0.;
				if (!getparfloat("o4", &o4)) o4=0.;
				if (!getparfloat("o5", &o5)) o5=0.;
				if (!getparfloat("dcdp2", &dcdp2)) dcdp2=0;
				if (!getparfloat("dline3", &dline3)) dline3=0;
				if (!getparfloat("ocdp2", &ocdp2)) ocdp2 = 0;
				if (!getparfloat("oline3", &oline3)) oline3=0;
				if (!getparfloat("gmin", &gmin)) gmin=0;
				if (!getparfloat("gmax", &gmax)) gmax=0;
				if (!getparint("orient", &orient)) orient=0;
				if (!getparint("gtype", &gtype)) gtype=0;
			}
		}
		outfp = stdout;		
		fseek64(outfp,0,0);
		trace = (char*) malloc(n1*dtype);
		traceout = (char*) malloc(n1*dtypeout);
		fseek64(infp,0,0);
		for(i=0;i<n2*n3*n4*n5;i++) {
			efread(trace,dtype,n1,infp);
			if(dtype==dtypeout) {
				efwrite(trace,dtype,n1,outfp);
			} else {
				if(dtypeout==1) {
					for(i1=0;i1<n1;i1++)
						traceout[i1*dtypeout] = (unsigned char) trace[i1*dtype];
				} else if(dtypeout==2) {
					for(i1=0;i1<n1;i1++)
						traceout[i1*dtypeout] = (short) trace[i1*dtype];
				} else if(dtypeout==4) {
					for(i1=0;i1<n1;i1++)
						traceout[i1*dtypeout] = (float) trace[i1*dtype];
				} else if(dtypeout==8) {
					for(i1=0;i1<n1;i1++)
						traceout[i1*dtypeout] = (double) trace[i1*dtype];
				}
				efwrite(traceout,dtypeout,n1,outfp);
			}

		}
		fflush(outfp);
		free(trace);
		free(traceout);
		if(hdremove==0) {
			fprintf(stderr,
				" === Update/add output grid header === \n");
			fprintf(stderr," scale=%g dtype=%d \n",scale,dtypeout);
			fprintf(stderr,
				" n1=%d n2=%d n3=%d n4=%d n5=%d \n",
				n1,n2,n3,n4,n5);
			fprintf(stderr,
				" d1=%g d2=%g d3=%g d4=%g d5=%g \n",
				d1,d2,d3,d4,d5);
			fprintf(stderr,
				" o1=%g o2=%g o3=%g o4=%g o5=%g \n",
				o1,o2,o3,o4,o5);
			fprintf(stderr,
				" dcdp2=%g dline3=%g ocdp2=%g oline3=%g \n",
				dcdp2,dline3,ocdp2,oline3);
			fprintf(stderr,
				" gmin=%g gmax=%g orient=%d gtype=%d\n",
				gmin,gmax,orient,gtype);

			toghdr(&gh,&scale,&dtypeout,&n1,&n2,&n3,&n4,&n5,
                		&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                		&dcdp2,&dline3,&ocdp2,&oline3,
				&gmin,&gmax,&orient,&gtype);
			ierr = fputghdr(outfp,&gh);
			if(ierr!=0) warn(" fputghdr error \n");
		}

		efclose(outfp);
	}
	efclose(infp);

	exit(0);
}
