/* SUDIPFILT: $Revision: 1.5 $ ; $Date: 91/02/08 15:05:43 $		*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUDIPFILT - DIP--or better--SLOPE Filter in f-k domain	\n"
" 								\n"
" sudipfilt <infile >outfile [optional parameters]		\n"
"								\n"
" Required Parameters:						\n"
" dt=(from header)      if not set in header then mandatory	\n"
" Optional parameters:						\n"
" datatype=0            type of data (0=stack; 1=shot; 2=cdp; 3=offset) \n" 
" slopes=0.0            monotonically increasing slopes	(ms/trace)	\n"
" amps=1.0              amplitudes corresponding to slopes	\n"
" bias=0.0              slope made horizontal before filtering	(ms/trace) \n"
"\n"
" Examples:							\n"
"\n"
" <shot.gathers sudipfilt datatype=1 slopes=-10.,-5.,0.,5.,10. \\ \n"
" amps=0.,0.5,1.,0.5,0. >shot.gathers.dipfilt                    \n"
"\n"
" will apply dipfiltering to each shot gather of input dataset called \n"
" shot.gathers and output to dataset called shot.gathers.dipfilt. The \n"
" dipfilter is designed as:					\n"
"\n"
"       slopes (ms/trace):  <=-10   -5    0    5    >=10	\n"
"          amplitudes:         0.   0.5   1.  0.5    0.      	\n"
" Notes:							\n"
"1. Dipfilter is applied to each gather, depending on datatype.	\n" 
"   For stack, gather is the whole line. Gather changes when the\n"
"   following trace header value changes:			\n"
"   	    trace header value		datatype		\n" 	 
"               no change                  0                    \n"
"           fldr (field record #)          1                    \n"
"           cdp (cdp #)                    2                    \n"
"           offset (signed offset)         3                    \n"
"2. Linear interpolation and constant extrapolation are used to	\n"
" determine amplitudes for slopes that are not specified.	\n"
" Linear moveout is removed before slope filtering to make	\n"
" slopes equal to bias appear horizontal.  This linear moveout	\n"
" is restored after filtering.  The bias parameter may be	\n"
" useful for spatially aliased data.  The slopes parameter is	\n"
" compensated for bias, so you need not change slopes when you	\n"
" change bias.							\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Dave (algorithm--originally called slopef)
 *	     Jack (reformatting for SU)
 *	     Zhiming Li (redefine slope in ms/trace, add datatype to allow
 *                       no-stack data filtering, rewrite slopefilter 
 *		         subroutine so that it vectorizes)
 */


#define PFA_MAX	720720	/* Largest allowed nfft	          */

segy tr;

main(int argc, char **argv)
{
	int nt;		/* number of time samples			*/
	float dt;	/* time sampling interval			*/
	int ntr;	/* number of traces				*/
	float dx;	/* trace spacing (spatial sampling interval)	*/
	int nslopes;	/* number of slopes specified			*/
	float *slopes;	/* slopes at which amplitudes are specified	*/
	int namps;	/* number of amplitudes specified		*/
	float *amps;	/* amplitudes corresponding to slopes		*/
	float bias;	/* slope bias					*/
	FILE *hdrfp;	/* fp for header storage file			*/
	FILE *datafp;   /* fp for trace storage file                    */
	int datatype;	/* type of data input */
	int ipre, inow;
	int i, itmp;
	float tmp;
	float *data, *pfft, *as, smin, ds, s, a;
	complex *cpfft;
	int ns, ntfft, nxfft;


	/* Hook up getpar to handle the parameters */
	initargs(argc,argv);
	askdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&tr))  err("can't get first trace");
	nt = tr.ns;


	/* Get parameters */
	if (!getparfloat("dt", &dt)) dt = (float)tr.dt/1000000.0;
	if (!dt) err("dt field is zero and not getparred");

	/* MUSTGETPARFLOAT("dx", &dx); */
	/* since slopes in ms/trace, we don't need dx */
	dx = 15.;

	nslopes = countparval("slopes");
	if (nslopes) {
		slopes = alloc1float(nslopes);
		getparfloat("slopes", slopes);
		for(i=0;i<nslopes;i++) {
			slopes[i] = slopes[i]*0.001/dx;
		}
	} else {
		nslopes = 1;
		slopes = alloc1float(nslopes);
		slopes[0] = 0.0;
	}

	namps = countparval("amps");
	if (namps) {
		amps = alloc1float(namps);
		getparfloat("amps", amps);
	} else {
		namps = 1;
		amps = alloc1float(namps);
		amps[0] = 1.0;
		warn("no amps given--doing no-op");
	}

	if (!getparfloat("bias", &bias)) bias = 0.0;
	bias = bias * 0.001/dx;
	if (!getparint("datatype", &datatype)) datatype = 0;

	/* Check parameters */
	if (nslopes != namps)
		err("number of slopes (%d) must equal number of amps(%d)",
			nslopes, namps);
	for (i=1; i<nslopes; ++i)
		if (slopes[i] <= slopes[i-1])
			err("slopes must be monotonically increasing");

	/* interpolate slopes and amps */
	smin = slopes[0];
	if(nslopes>1) {
		ns = 256;
		ds = (slopes[nslopes-1]-smin)/(ns-1); 
	} else {
		ns = 1;
		ds = 1.;
	}
	as = (float*) malloc(ns*sizeof(float));
	for(i=0;i<ns;i++) {
		s = smin + i *ds;
		/* linearly interpolate to find amplitude */
                intlin(nslopes,slopes,amps,amps[0],amps[nslopes-1],
                       1,&s,&a);
		as[i] = a;
	}


	/* Store traces and headers in tmpfile while getting a count */
	hdrfp  = etempfile(NULL);
	datafp  = etempfile(NULL);
	ntr = 0;
	if(datatype==0) {
		ipre = 0;
	} else if (datatype==1) {
		ipre = tr.fldr; 
	} else if (datatype==2) {
		ipre = tr.cdp;
	} else if (datatype==3) {
		ipre = tr.offset;
	}	

	do { 
		if(datatype==0) {
			inow = 0;
		} else if (datatype==1) {
			inow = tr.fldr; 
		} else if (datatype==2) {
			inow = tr.cdp;
		} else if (datatype==3) {
			inow = tr.offset;
		}	
		if(inow==ipre) {
			efwrite(&tr, 1, HDRBYTES, hdrfp);
			efwrite(tr.data, FSIZE, nt, datafp);
			++ntr;
		} else {
			/* Apply slope filter */
			ntfft = npfar(nt);
			nxfft = npfa(ntr);
			cpfft = (complex*)malloc((ntfft/2+1)*nxfft
						*sizeof(complex));
			pfft = (float*)malloc(ntfft*nxfft*sizeof(float));
			data = (float*)malloc(ntr*nt*sizeof(float));

			rewind(datafp);
			efread(data, FSIZE, nt*ntr, datafp);

			dpf_(&ns,&smin,&ds,as,&bias,&nt,&dt,&ntr,&dx,data,
		     		cpfft,pfft,&ntfft,&nxfft);

			free(cpfft);
			free(pfft);

			/* Output filtered traces */
			rewind(hdrfp);
			{ register int itr;
	  			for (itr = 0; itr < ntr; ++itr) {
					efread(&tr, 1, HDRBYTES, hdrfp);
					for(i=0;i<nt;i++) 
						tr.data[i]=data[i+itr*nt];
					/* zero data in mute zone */ 
					tmp = (float)tr.mute*1000./(float)tr.dt;
					itmp = tmp;
					if(itmp>nt) itmp=nt;
					for(i=0;i<itmp;i++)
						tr.data[i]=0.;
					puttr(&tr);
	  			}
			}

			free(data);

			rewind(hdrfp);
			rewind(datafp);

			efwrite(&tr, 1, HDRBYTES, hdrfp);
			efwrite(tr.data, FSIZE, nt, datafp);
			ntr = 1;
			ipre = inow;
		}
	} while (gettr(&tr));
	
	/* last gather */
	if(ntr>0) {
		ntfft = npfar(nt);
		nxfft = npfa(ntr);
		cpfft = (complex*)malloc((ntfft/2+1)*nxfft
					*sizeof(complex));
		pfft = (float*)malloc(ntfft*nxfft*sizeof(float));
		data = (float*)malloc(ntr*nt*sizeof(float));

		rewind(datafp);
                efread(data, FSIZE, nt*ntr, datafp);

                /* Apply slope filter */
		dpf_(&ns,&smin,&ds,as,&bias,&nt,&dt,&ntr,&dx,data,
	     		cpfft,pfft,&ntfft,&nxfft);

		free(cpfft);
		free(pfft);

                /* Output filtered traces */
                rewind(hdrfp);
                { register int itr;
                	for (itr = 0; itr < ntr; ++itr) {
                        	efread(&tr, 1, HDRBYTES, hdrfp);
				for(i=0;i<nt;i++) 
					tr.data[i]=data[i+itr*nt];
                                /* zero data in mute zone */
				tmp = (float)tr.mute*1000./(float)tr.dt;
				itmp = tmp;
				if(itmp>nt) itmp=nt;
                                for(i=0;i<itmp;i++)
                               		tr.data[i]=0.;
                                puttr(&tr);
                        }
		}
		free(data);
	}



	/* Clean up */
	efclose(hdrfp);
	efclose(datafp);
	free(data);
	free(as);
	free1float(slopes);
	free1float(amps);

}
