#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"GAMAN - compute gamma (Vtrue/Vmig) semblance for cdp gathers\n"
"\n"
"gaman <stdin >stdout [optional parameters]\n"
"\n"
"Optional Parameters:\n"
"ng=41                   number of gamma values to scan  \n"
"dg=0.01                 gamma sampling interval	 \n"
"fg=.80                  first gamma value 		\n"
"smute=1.5               samples with stretch exceeding smute are zeroed \n"
"dzratio=5               ratio of output to input depth sampling intervals \n"
"nsmooth=dzratio*2+1     length of semblance num and den smoothing window \n"
"norm=0                  normalization flag (1=yes; 0=no; 2=g-normalization) \n"
"verbose=1               =1 for diagnostic print on stderr; 0=no printout \n"
"ocdp=1st input cdp      first cdp location to output the gamma semblance \n" 
"ncdp=all                number of gamma semblance to output 		\n" 
"dcdp=1                  cdp increment to output the gamma semblance \n" 
"sumcdp=1                number of semblances summed per output cdp location \n"
"			  	when cdp increment is 1; otherwise, \n"
"				semblences at cdp numbers between \n"
"				(cdpout-sumcdp/2) and (cdpout+sumcdp/2) \n"
"				are summed together to be output at \n"
"				cdp position=cdpout \n"
"zpow=0.0                input trace will be gained by factor of depth**zpow\n"
"				before computing semblance	\n"
"\n"
"Notes:\n"
"Semblance is defined by the following quotient:\n"
"\n"
"                 n-1                 \n"
"               [ sum q_g(z,j) ]^2      \n"
"                 j=0                 \n"
"      s(z,g) = ------------------     \n"
"		  dsum		      \n"
"\n"
"where q_g(z,j) is input trace corrected with gamma value of g, and  \n"
"        n-1                 \n"
"dsum= n sum [q_g(z,j)]^2, if norm=1; or dsum=n, if norm=0; \n"
"        j=0                 \n"
"                 	    	    n-1                 \n"
"   		or dsum = n max { [ sum q_g(z,j) ]^2 }, if norm=2     \n"
"                 	     g	    j=0                 \n"
"\n"
"and n is the number of non-zero samples after muting.\n"
"Smoothing (nsmooth) is applied separately to the numerator and denominator\n"
"before computing this semblance quotient.\n"
"\n"
"gamma*10000 is output in offset filed in the trace header \n"
"\n"
"Input traces should be sorted by cdp - gaman outputs a group of \n"
"semblance traces every time cdp changes.  Therefore, the output will\n"
"be useful only if input are cdp gathers.\n"
"\n"
"Trace header fields accessed:  ns, dt, delrt, offset, cdp, fz, dz. \n"
"Trace header fields modified:  ns, dt, offset, dz. \n"
" \n"
" Author: Zhiming Li             10/9/91			\n"
"\n";
/**************** end self doc *******************************************/


segy tr;
SU_bhed bh;
SU_ched ch;

main(int argc, char **argv)
{
	int ng;		/* number of gamma values */
	float dg;	/* gamma sampling interval */
	float fg;	/* first gamma */
	int ig;		/* gamma index */
	int dzratio;	/* ratio of output to input sampling intervals */
	int nsmooth;	/* length in samples of num and den smoothing window */
	int nz;		/* number of depth samples per input trace */
	float dz;	/* depth sampling interval for input traces */
	float fz;	/* depth of first sample input and output */
	int nzout;	/* number of output samples */
	float dzout;	/* depth sampling interval for output traces */
	int iz;		/* input depth sample index */
	int izout;	/* output depth sample index */
	int is;		/* depth sample index for smoothing window */
	int ismin;	/* lower limit on is */
	int ismax;	/* upper limit on is */
	int izmute;	/* depth sample index of first sample not muted */
	int izi;	/* depth sample index used in linear interpolation */
	float zi;	/* normalized depth for linear interpolation */
	float frac;	/* fractional distance from sample in interpolation */
	int gottrace;	/* =1 if an input trace was read */
	int done;	/* =1 if done with everything */
	int verbose;	/* =1 for diagnostic print */
	long cdp;	/* cdp from current input trace header */
	long cdpprev;	/* cdp from previous input trace header */
	float smute;	/* GMO stretch mute factor */
	float rsmute;	/* 1/smute for negative GMO stretch mute factor */
	float offset;	/* offset from input trace header */
	float offovs;	/* (offset/2)^2*(gamma^2-1) */
	float zn;	/* depth after residual gamma moveout (GMO) */
	float znmute;	/* mute depth after GMO */
	float nsum;	/* semblance numerator sum */
	float dsum;	/* semblance denominator sum */
	float g;	/* gamma */
	float temp;	/* temporary scalar */
	float *data;	/* array[nt] of input trace */
	float **sem;	/* array[ntout] of semblance */
	float **num;	/* array[nv][nz] of semblance numerators */
	float **den;	/* array[nv][nz] of semblance denominators */
	float **nnz;	/* array[nv][nz] for counting non-zero samples */
	int tracl;	/* trace number of the line */
	int norm;	/* semblance type (1=normalized; 0=non normalized) */
	int ocdp, dcdp, sumcdp, ncdp, icdp, isumcdp, cdpout;
	int mode;
	float *gain, zpow;	/* depth gain */
	float g2;
	int imute, mute;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(1);

	/* read id headers */
	gethdr(&ch,&bh);
	/* get parameters from the first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nz = tr.ns;

	if ( tr.dt < 1000 ) {
		dz = tr.dt;
		fz = tr.delrt;
	}
	else {
		dz = (float)tr.dt/1000;
		fz = tr.delrt/1000;
	}

	if ( tr.dz != 0. ) {
		dz = tr.dz;
		fz = tr.fz;
	} 

	cdp = tr.cdp;
	offset = tr.offset;
	mute = tr.mute;

	/* get optional parameters */
	if (!getparint("ng",&ng)) ng = 41;
	if (!getparfloat("dg",&dg)) dg = 0.01;
	if (!getparfloat("fg",&fg)) fg = .80;
	if (!getparfloat("smute",&smute)) smute = 1.5;
	if (smute<=1.0) err("smute must be greater than 1.0");
	if (!getparint("dzratio",&dzratio)) dzratio = 5;
	if (!getparint("nsmooth",&nsmooth)) nsmooth = dzratio*2+1;
	if (!getparint("verbose",&verbose)) verbose = 1;
	if (!getparint("norm",&norm)) norm = 0;

	if (!getparint("ocdp",&ocdp)) ocdp = cdp;
	if (!getparint("dcdp",&dcdp)) dcdp = 1;
	if (!getparint("ncdp",&ncdp)) ncdp = 999999;
	if (!getparint("sumcdp",&sumcdp)) sumcdp = 1;
	if (!getparfloat("zpow",&zpow)) zpow = 0.;


	/* determine output sampling */
	nzout = 1+(nz-1)/dzratio;
	dzout = dz*dzratio;
	/* update id headers and output them */
	bh.fold = ng;
	bh.tsort = 2;
	bh.hdt = dzout; 
	bh.hns = nzout; 
	puthdr(&ch,&bh);

	if (verbose) {
		fprintf(stderr,
			"\t === GAMAN PROGRAM PRINTOUT === \n");
		fprintf(stderr,
			"\t number of output depth samples is %d \n",nzout);
		fprintf(stderr,
			"\t output depth sampling interval is %f \n",dzout);
		fprintf(stderr,
			"\t output depth of first sample is %f \n",fz);
		fprintf(stderr,
			"\n");
	}

	/* allocate memory */
	data = ealloc1float(nz);
	num = ealloc2float(nz,ng);
	den = ealloc2float(nz,ng);
	nnz = ealloc2float(nz,ng);
	sem = ealloc2float(nzout,ng);

	/* compute zpow factors */
	if(zpow!=0.0) {
		gain = ealloc1float(nz);
		for(iz=0;iz<nz;iz++) {
			 temp = (fz + iz * dz)/(fz+nz*.5*dz);
			 if (temp > 0. ) {  
			 	gain[iz]=pow(temp,zpow);
			 } else {
			 	gain[iz]=0.;
				
			 }
		}
	}


	/* zero accumulators */
	for (ig=0; ig<ng; ++ig) {
		for (iz=0; iz<nz; ++iz) {
			num[ig][iz] = 0.0;
			den[ig][iz] = 0.0;
			nnz[ig][iz] = 0.0;
		}
	}

	/* initialize flags */
	gottrace = 1;
	done = 0;
	rsmute = 1./smute;
	icdp = 0;

	/* remember previous cdp */
	cdpprev = cdp - 1 ;
	cdpout = ocdp;
	tracl = 1;
	isumcdp = 0;

	/* loop over input traces */
	do {

		/* if got a trace */
		if (gottrace) {
			/* determine offset and cdp */
			offset = tr.offset;
			cdp = tr.cdp;
			mute = tr.mute;
		}
		

		if ( cdp >= cdpout-sumcdp/2 && cdp <= cdpout+sumcdp/2 ) {
			mode = 0;	/*within summed cdp range */
		} else if ( cdp < cdpout-sumcdp/2 ) {
			mode = 1;	/*before the summed cdp range */
		} else if ( cdp > cdpout+sumcdp/2 ) {
			mode = 2; 	/*after the summed cdp range */
		}

		if (mode==2 && isumcdp==0) break;

		if (gottrace && mode != 1) {
			/* get trace samples */
			bcopy(tr.data,data,nz*sizeof(float));
			if(zpow!=0.0) {
				for(iz=0;iz<nz;iz++) data[iz] *= gain[iz];
			} 
			
		}

		/* if cdp has changed or no more input traces */
		if ( (mode==2 || !gottrace) && isumcdp > 0 )  {

			/* set output trace header fields */
			tr.offset = 0;
			tr.cdp = cdpout;
			tr.ns = nzout;
			tr.dt = dzout;
			tr.dz = dzout;
			tr.fz = fz;
			tr.n2 = ng;

			/* loop over gamma values */
			for (ig=0; ig<ng; ++ig) {

				/* compute semblance quotients */
				for (izout=0; izout<nzout; ++izout) {
					iz = izout*dzratio;
					ismin = iz-nsmooth/2;
					ismax = iz+nsmooth/2;
					if (ismin<0) ismin = 0;
					if (ismax>nz-1) ismax = nz-1;
					nsum = dsum = 0.0;
					if ( norm == 1 ) {
					  for (is=ismin; is<=ismax; ++is) {
						nsum += num[ig][is]*
							num[ig][is];
						dsum += nnz[ig][is]*
							den[ig][is];
					  }
					} else {
					  for (is=ismin; is<=ismax; ++is) {
						nsum += num[ig][is]*
							num[ig][is];
						dsum += nnz[ig][is];
					  }
					}
					if(dsum!=0.) {
						sem[ig][izout] = nsum/dsum;
					}else {
						sem[ig][izout] = 0.;
					}
					sem[ig][izout]=sem[ig][izout]/isumcdp;
				}
			}

			if(norm==2) {
				for(izout=0;izout<nzout;izout++) {
					temp = sem[0][izout];
					for(ig=1;ig<ng;ig++) {
						if(temp<sem[ig][izout])
							temp = sem[ig][izout];
					}
					if ( temp > 0. ) {
						temp = 1./temp;
						for(ig=0;ig<ng;ig++) {
							sem[ig][izout] *= temp;
						}
					}	
				}
			}

			for (ig=0; ig<ng; ++ig) {
				/* output semblances */
				bcopy(sem[ig],tr.data,nzout*sizeof(float));
				/* update offset */
				g = fg + ig * dg;
				g = g * 10000.;
				tr.offset = (long) g;  
				tr.tracl = tracl;  
				tr.cdpt = ig + 1;  
				puttr(&tr);
				tracl = tracl + 1;

				/* zero accumulators */
				for (iz=0; iz<nz; ++iz) {
					num[ig][iz] = 0.0;
					den[ig][iz] = 0.0;
					nnz[ig][iz] = 0.0;
				}
			}

			/* diagnostic print */
			if (verbose) 
				fprintf(stderr,
"\t %d semblance(s) computed and summed at output cdp = %d\n",isumcdp,cdpout);


			/* next cdp output position */
			cdpout = cdpout + dcdp;
			isumcdp = 0;
			icdp = icdp + 1;
			
		}

		if ( cdp >= cdpout-sumcdp/2 && cdp <= cdpout+sumcdp/2 ) {
			/* loop over gamma values */
			for (ig=0,g=fg; ig<ng; ++ig,g+=dg) {
			
				g2 = 1./(g*g); 
				/* compute (offset/2)^2 * (gamma^2-1) */
				offovs = (offset*offset*0.25)*(g2-1.);

				/* determine mute depth after gmo */
				if ( g2 >= 1.0 ) {
					znmute=sqrt(offovs/(smute*smute-1.0));
				}	
				else {
					znmute=sqrt(offovs/(rsmute*rsmute-1.0));
				}
				izmute = (znmute-fz)/dz;
			
			/* do gmo via quick and dirty linear interpolation
			/* (accurate enough for gamma analysis) and
			/* accumulate semblance numerator and denominator */
				if(izmute<0) izmute = 0;
				temp = (mute-fz)/dz;
        			imute = temp;
				for (iz=izmute,zn=fz+izmute*dz; iz<nz; 
				     ++iz,zn+=dz) {
					temp = zn*zn + offovs;
			        	if(temp >=0.) {
						zi = (sqrt(temp)-fz)/dz;
					}
					else {
						zi = -100000;
					}
					izi = zi;
					if (izi<nz-1 && izi >= imute ) {
						frac = zi-izi;
						temp = (1.0-frac)*data[izi]+
							frac*data[izi+1];
						if (temp!=0.0) {
							num[ig][iz]+=temp;
							den[ig][iz]+=temp*temp;
							nnz[ig][iz] += 1.0;
						}
					}
				}
			}
			if ( cdp != cdpprev || !gottrace) {
				cdpprev = cdp;
				isumcdp += 1;
			}
		}

		/* if no more input traces, break input trace loop */
		if (!gottrace) break;
		if ( icdp >= ncdp ) break;

		/* get next trace (if there is one) */
		if (!gettr(&tr)) gottrace = 0;

	} while (!done);

	return EXIT_SUCCESS;
}
