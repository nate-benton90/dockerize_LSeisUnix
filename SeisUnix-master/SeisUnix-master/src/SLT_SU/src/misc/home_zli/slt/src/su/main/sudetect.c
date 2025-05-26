
#include "su.h"
#include "segy.h"
#include "header.h"
#include "xplot.h"



/*********************** self documentation **********************/
string sdoc =
" 								 \n"
" SUDETECT - detect and zero bad amplitude traces 		 \n"
" 								 \n"
" sudetect <datain [parameters] >dataout			 \n"
" 							         \n"
" Required parameters:						 \n"
"  none								 \n"
" Optional parameters:						 \n"
"  min=0.0       minimum absolute value of good amplitudes 	 \n"	
"  max=1.0e+6    maximum absolute value of good amplitudes 	 \n"	
"  report=0      report mode; 					 \n"	
"                (0=only print out bad trace positions;		 \n"
"                 1=print both good and bad trace positions)     \n"
"  printout=     name of file to hold printout; if not given,    \n"
"                 it default to screen                           \n" 
"  action=0      action taken on bad amplitudes			 \n"
"		 (0=no action, report only, no dataout;		 \n"
"                 1=zero out bad amplitudes, output dataout;	 \n"
"                 2=zero out entire trace which has bad amplitudes, \n"
"                   output dataout;                             \n" 
"                 3=scale the bad trace with the desired rms given \n"
"                   by rmsout;  				\n"
"                   or scale the trace whose rms is greater than \n"
"                   the desired rmsout.)			\n"
"  rmsout=       Desired output rms value of trace.		\n"
"                If action=3, rmsout is required. 		\n" 
"  itmin=1       starting sample of amplitude analysis 		\n"
"  itmax=nt      ending sample of amplitude analysis (nt is 	\n"
"                the number of samples per trace)		\n"
" 							         \n"
" Notes:							 \n"
"   1. When action=0, user should not specify '>dataout'.        \n"
"   2. A histogram showing percentage of data points versus 	 \n"
"      amplitude range will be displayed on x-window screen. 	 \n"
"      Amplitudes corresponding to infinity and NaN (Not A Number)\n"
"      are ploted within range from 10**39 to 10**40. 		 \n" 
"   3. rms amplitude of trace will be printed also. 		\n"
"   4. mute time in trace header is used to start amplitude analysis if	\n"
"      itmin is not given. Otherwise mute is ignored.		\n"
"\n"
" Author:  Zhiming Li		4/23/1992                        \n"
" 							         \n"
;	
/**************** end self doc ***********************************/


#ifdef __convex__
        #define file2g(x)       fseek64(x,0,0);
#else
        #define file2g(x)       fseek(x,0,0);
#endif


segytrace trin; 		/* defind input and output trace buffers */


/* main program */
main(int argc, char **argv)
{
	
	
	float min, max; 		/* scale */
	double dtmp, tmp, dmin, dmax;
	int nt;			/* number of samples per trace */
	int it;			/* time index */
	FILE *infp=stdin, *outfp=stdout;	
	int itrace;
	int nbad, ibad, report_good, action;
	char *printout;
	FILE *printfp;
	float fmax, fmin;
	int *count, itmp, i; 
	float *plots;
	double rms, rmssum;
	float rmsout, scale;
	int imute, ntrace;
	int itmin, itmax, its;
	
/* initialize */
	initargs(argc, argv);	/* initialize parameter reading */ 
	askdoc(1);		/* on-line self documentation */

/* parameters fetching */

	if (!getparfloat("min",&min)) min=0.0;
	if (!getparfloat("max",&max)) max=1.0e+6;
	if (!getparint("report",&report_good)) report_good=0;
	if (!getparint("action",&action)) action=0;
	if (!getparstring("printout",&printout)) {
		printfp = stderr;
	} else {
		printfp = fopen(printout,"w");
	}

	if (action==3) {
		if(!getparfloat("rmsout",&rmsout)) 
			err(" rmsout missing ");
	}

/* processing */ 

	file2g(infp);
	if (!fgettr(infp,&trin))  err("can't get first trace");
				/* read in the first trace, exit if fail */

	nt = trin.ns;		/* obtain number of samples per trace */

	if(action!=0) file2g(outfp);



	if (!getparint("itmin",&itmin)) itmin = 0;
	if (!getparint("itmax",&itmax)) itmax = nt;

	dmin = min;
	dmax = max;
	nbad = 0;
	itrace=1;
	fmax = 0.;
	fmin = 1.e+38;
	count = (int*) malloc(40*sizeof(int));
	plots = (float*) malloc(160*sizeof(float));
	bzero(count,40*sizeof(int));
	bzero(plots,160*sizeof(float));
	rmssum = 0.;
	ntrace = 0;

	/* Main loop over traces */
	do {

		ibad = 0;
		rms = 0.;
		if(itmin==0) {
			imute = (trin.mute - trin.delrt)*1000/(int)trin.dt;
			if(imute<0) imute=0;
			its = imute;
		} else {
			its = itmin;
		}
		for(it=its;it<nt;it++) {
		        if( !ieeefinite(&trin.data[it]) ) {
				ibad += 1;
				if(action==1) trin.data[it] = 0.;
				count[39] += 1;
			} else {
				rms = rms + trin.data[it]*trin.data[it]; 
				dtmp = trin.data[it];
				tmp = fabs(dtmp);
				if(tmp<dmin || tmp>dmax) {
					ibad += 1;
					if(action==1) trin.data[it] = 0.;
				}
				if(tmp>fmax) {
					 fmax = tmp;
				} else if (tmp<fmin) {
					 fmin = tmp;
				} 
				if(dtmp>0.) {
					dtmp = log10(tmp);
					itmp = dtmp;
				} else {
					itmp = 0;
				}
				if(itmp<0) {
					itmp = 0;
				} else if(itmp>38) {
					itmp = 39;
				} 
				count[itmp] += 1;
			}
		}
		if(nt>its) {
			rms = sqrt(rms/(nt-its));
		}
		
		if(ibad>0 && action==2) bzero((char*)trin.data,nt*4);

		if( (ibad>0 || rms>rmsout) && action==3) {
			if(rms==0) {
				bzero((char*)trin.data,nt*4);
			} else {
				scale = rmsout/rms;
				for(it=0;it<nt;it++) trin.data[it] *=scale;
			}
		}
		if(ibad>0) nbad += 1;

		/* report */
		if(ibad>0) {
		    fprintf(printfp, 
	    "BAD amplitude at trace %d with trace number=%d rms=%g \n",
			itrace,trin.tracl,rms);
		} else if(report_good == 1) {
		    fprintf(printfp, 
		    "GOOD amplitude at trace %d with trace number=%d rms=%g\n",
			itrace,trin.tracl,rms);
		} else if(action==3 && rms>rmsout) {
		    fprintf(printfp, 
	    "BAD rms amplitude at trace %d with trace number=%d rms=%g \n",
			itrace,trin.tracl,rms);
		}
		fflush(printfp);
		itrace +=1;
		if(action!=0) fputtr(outfp,&trin);
		rmssum += rms;
		ntrace += 1; 
 
	} while (fgettr(infp,&trin));		/* reading traces */

	if( ntrace>1) rmssum = rmssum / ntrace;

	fprintf(printfp, 
	"===> Total Number of Bad Traces in Input = %d \n",nbad);
	fprintf(printfp, 
	"===> Maximum Absolute Amplitude in Input = %g \n",fmax);
	fprintf(printfp, 
	"===> Minimum Absolute Amplitude in Input = %g \n",fmin);
	fprintf(printfp, 
	"===> average trace rms amplitude in Input = %g \n",rmssum);
	itmp = 0;
	for(i=0;i<40;i++) itmp += count[i]; 

	
	for(i=0;i<40;i++) {
		plots[i*4] = i;
		plots[i*4+1] = (float)count[i]/(float)itmp*100.;
		plots[i*4+2] = i+1;
		plots[i*4+3] = (float)count[i]/(float)itmp*100.;
	}
		
	for(i=0;i<39;i++) {
		if(count[i]>0) {
			if(i==0) {
				tmp = 0.;
			} else {
				tmp = pow(10.,(float)i);
			}
	fprintf(printfp,
	"===> Percentage of Amplitudes from %g to %g =%-5.2f \n",
	tmp, pow(10.,(float)(i+1)),
	(float)count[i]/(float)itmp*100.);
		}
	}
	if(count[39]>0) fprintf(printfp,
	"===> Percentage of Infinity/NaN Amplitudes =%-5.2f \n",
	(float)count[39]/(float)itmp*100.);
	
	itmp = 80;
	dump2xgraph(plots,&itmp,1,"Input Amplitude Distribution","power of 10",
			"percentage","normal");

	free(count);
	free(plots);

	return 0;
}
