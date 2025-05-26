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
"  max=1.0e+38   maximum absolute value of good amplitudes 	 \n"	
"  num=40        number of amplitude intervals to compute histogram \n"
"  logscale=1    log(10) scale used in horizontal axis of histogram \n"
"                1=yes 0=linear-scale \n"
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
"      are ploted the same as amplitude from 10**39 to 10**40. 		 \n" 
"   3. rms amplitude of trace will be printed also. 		\n"
"   4. mute time in trace header is used to start amplitude analysis if	\n"
"      itmin is not given. Otherwise mute is ignored.		\n"
"\n"
" Author:  Zhiming Li		4/23/1992                        \n"
" 							         \n"
;	
/**************** end self doc ***********************************/


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
	int itmp, i, itrtmp; 
	float *count, ftmp, ttmp;
	float *trcount;
	float *plots;
	double rms, rmssum;
	float rmsout, scale;
	int imute, ntrace;
	int itmin, itmax, its;
	int num, logscale;
	float dscale;
	
/* initialize */
	initargs(argc, argv);	/* initialize parameter reading */ 
	askdoc(1);		/* on-line self documentation */

/* parameters fetching */

	if (!getparfloat("min",&min)) min=0.0;
	if (!getparfloat("max",&max)) max=1.0e+38;
	if (!getparint("num",&num)) num=40;
	if (!getparint("logscale",&logscale)) logscale=1;
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

	if(logscale==0) {
		dscale = (max-min)/(num-1.);
	}

/* processing */ 

	file2g(infp);
	/* read in the first trace, exit if fail */
	if (!fgettr(infp,&trin))  err("can't get first trace");

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
	count = (float*) malloc(num*sizeof(float));
	trcount = (float*) malloc(num*sizeof(float));
	plots = (float*) malloc(num*4*sizeof(float));
	bzero(count,num*sizeof(int));
	bzero(trcount,num*sizeof(int));
	bzero(plots,num*4*sizeof(float));
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
		itrtmp = 0;
		for(it=its;it<nt;it++) {
	        if( !ieeefinite(&trin.data[it]) ) {
				ibad += 1;
				if(action==1) trin.data[it] = 0.;
				count[num-1] += 1;
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
				if(logscale==1) {
					if(tmp>0.) {
						dtmp = log10(tmp);
						itmp = dtmp;
					} else {
						itmp = 0;
					}
				} else {
					dtmp = (tmp-min)/dscale;
					itmp = dtmp;
				}
				if(itmp<0) {
					itmp = 0;
				} else if(itmp>num-1) {
					itmp = num-1;
				} 
				count[itmp] += 1.;
			}
			if(itrtmp<itmp) itrtmp = itmp;
		}
		trcount[itrtmp] = itrtmp + 1;
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
	ftmp = 0.;
	for(i=0;i<num;i++) ftmp += count[i]; 
	ttmp = 0.;
	for(i=0;i<num;i++) ttmp += trcount[i]; 

	
		
	if(logscale==1) {
		for(i=0;i<num;i++) {
			plots[i*4] = i;
			plots[i*4+1] = (float)count[i]/ftmp*100.;
			plots[i*4+2] = i+1;
			plots[i*4+3] = (float)count[i]/ftmp*100.;
		}
		for(i=0;i<num;i++) {
			if(count[i]>0) {
				if(i==0) {
					tmp = 0.;
				} else {
					tmp = pow(10.,(float)i);
				}
			fprintf(printfp,
				"===> Percentage of Amplitudes from %g to %g =%-10.7f \n",
				tmp, pow(10.,(float)(i+1)), count[i]/ftmp*100.);
			fprintf(printfp,
			"===> Percentage of Traces with max_amp from %g to %g =%-10.7f \n",
				tmp, pow(10.,(float)(i+1)), trcount[i]/ttmp*100.);
			}
		}
		/*
		if(count[num-1]>0) fprintf(printfp,
		"===> Percentage of Infinity/NaN Amplitudes =%-10.7f \n",
		count[num-1]/ftmp*100.);
		*/
		itmp = num*2;
		dump2xgraph(plots,&itmp,1,"Input Amplitude Distribution","power of 10",
			"percentage","normal");

	} else {
		for(i=0;i<num;i++) {
			plots[i*4] = min+i*dscale;
			plots[i*4+1] = (float)count[i]/ftmp*100.;
			plots[i*4+2] = min+(i+1)*dscale;
			plots[i*4+3] = (float)count[i]/ftmp*100.;
		}
		for(i=0;i<num;i++) {
			if(i==0) {
				tmp = min;
			} else {
				tmp = min + i*dscale;
			}
			fprintf(printfp,
				"===> Percentage of Amplitudes from %g to %g =%-10.7f \n",
				tmp, tmp+dscale, count[i]/ftmp*100.);
			fprintf(printfp,
			"===> Percentage of Traces with max_amp from %g to %g =%-10.7f \n",
				tmp, tmp+dscale, trcount[i]/ttmp*100.);
		}
		itmp = num*2;
		dump2xgraph(plots,&itmp,1,"Input Amplitude Distribution","amplitude",
			"percentage","normal");

	}

	free(count);
	free(plots);

	return 0;
}
