
/* SURMO: $Revision: 1.3 $ ; $Date: 93/08/13 10:22:46 $		*/

/*----------------------------------------------------------------------
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SURMO - multiple suppression via residual-moveout stacking    \n"
" 								\n"
" surmo <infile [parameters] >outfile 				\n"
" 								\n"
" Required Parameters:\n"
"\n"
" cdp=cdp1,cdp2,...	specified CDP array at which window parameter 	\n"
"			arrays must be set 			\n"
" or (For common shot gather) \n"
" shot=shot1,shot2,...	specified shot array at which window parameter	\n"
"			arrays must be set 			\n"
"\n"
" For each of the above CDP (or shot) numbers, specify one parameter and \n"
" six parameter arrays in each of which number of elements is equal to \n"
" number of windows: \n" 
"\n"
"	amp=1			amplitude factor of multiples \n"
" 	tmin=tmin1,tmin2,...  	minimum time			\n" 
" 	tmax=tmax1,tmax2,...  	maximum time 			\n" 
" 	rmin=rmin1,rmin2,...  	minimum residual-moveout at far offset	\n" 
" 	rmax=rmax1,rmax2,...  	maximum residual-moveout at far offset	\n" 
" 	rbeg=rbeg1,rbeg2,...  	beginning residual-moveout of multiple 	\n" 
" 	rend=rend1,rend2,...  	ending residual-moveout of multiple 	\n" 
"\n"
"Optional Parameters:\n"
"\n"
"nwin=1		number of time windows				\n"
"fmin=0		minimum frequency in input data			\n"
"fmax=0.25/dt	maximum frequency in input data			\n"
"prw=0.01	prewhiting factor				\n"
"ttaper=0.2	time taper on windows of traces 		\n"
"maxnoff=100		maximum number of offset		\n"
"nsoff=maxnoff		number of offset to be processed	\n"
"mult=0		=1 for creating multiple model stored in file 'mfp'.  	\n"
"\n"
" Note: \n"
" 1. Input data are common shot or common CMP gathers with SEGY format.	\n"
" 2. amp=0 means no processing for multiple suppression. \n"
" 3. Linear interpolation and constant extrapolation are used to compute \n"
"\t the parameters at CDPs not specified. \n"
" 4. The offset may be positive or negative or both. \n"
" 5. The parameter nsoff is used only for data set whose offset array starts \n"
"\t from the near-zero offset.\n"
" 6. In each CDP, the multiple events should be within the time window \n"
"\t (tmin+ttaper, tmax-ttaper). \n"
" 7. rmin, rmax, rbeg and rend are measured at the far offset determined \n"
"\t by nsoff (if it is specified). 	\n"
"\n"
" Author: Zhenyue Liu,  Colorado School of Mines, 08/13/93	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 */

void rmo(float **trg, float **trgm, int nt, float dt, float *offset, int noff,
	float tmin, float tmax, float rmin, float rmax, float rbeg, float rend,
	float fmin, float fmax, float ttaper, float amp, int mult, float prw);
	
static void interpw (int nt, int ncdp, float *cdp, float **w,
        float cdpt, float *wt);
segy tr, tro;

main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	float dt;	/* time sampling interval */
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	int ncdp;	/* number of cdps specified */
	float *cdp;	/* array[ncdp] of cdps */
	int icdp;	/* index into cdp array */
	int nwin;	/* number of vnmos specified */
	int iwin;
	int gottrace, done;
	long oldcdp, newcdp;
	float **tmin, **tmax, **rmax, **rmin, **rbeg, **rend;
	float *tmint, *tmaxt, *rmaxt, *rmint, *rbegt, *rendt;
	float tminw, tmaxw, rminw, rmaxw, rbegw, rendw;
	float fmin, fmax, prw, **amp, *ampt, ttaper;
	int maxnoff, nsoff, nsoff_old, noff, ioff, index, mult, shot=0;
	float *offset, offmax;
	float **trg, **trgm;
	FILE *mfp, *hfp;

	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

	/* get information from the first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000000.0;
	ft = tr.delrt/1000.0;


	/* get CDP or shot array */
	ncdp = countparval("cdp");
	if(!ncdp) {
		shot = 1;
		ncdp = countparval("shot");
	}
	if(!ncdp) err("a CDP or SHOT array must be specified");
	cdp = ealloc1float(ncdp);
	if(!shot) getparfloat("cdp",cdp);
		else getparfloat("shot",cdp);
	for(icdp=1; icdp<ncdp; ++icdp)
		if(cdp[icdp]<=cdp[icdp-1])
			err("CDP or SHOT must be monotonically increased");

	if (countparname("tmin")!=ncdp)
		err("a tmin array must be specified for each cdp");
	if (countparname("tmax")!=ncdp)
		err("a tmax array must be specified for each cdp");
	if (countparname("rmin")!=ncdp)
		err("a rmin array must be specified for each cdp");
	if (countparname("rmax")!=ncdp)
		err("a rmax array must be specified for each cdp");
	if (countparname("rbeg")!=ncdp)
		err("a rbeg array must be specified for each cdp");
	if (countparname("rend")!=ncdp)
		err("a rend array must be specified for each cdp");

	if(!getparint("nwin",&nwin)) nwin = 1;
	tmin = ealloc2float(nwin,ncdp);
	tmax = ealloc2float(nwin,ncdp);
	rmin = ealloc2float(nwin,ncdp);
	rmax = ealloc2float(nwin,ncdp);
	rbeg = ealloc2float(nwin,ncdp);
	rend = ealloc2float(nwin,ncdp);
	tmint = ealloc1float(nwin);
	tmaxt = ealloc1float(nwin);
	rmint = ealloc1float(nwin);
	rmaxt = ealloc1float(nwin);
	rbegt = ealloc1float(nwin);
	rendt = ealloc1float(nwin);
	amp = ealloc2float(1,ncdp);
	ampt = ealloc1float(1);

	for (icdp=0; icdp<ncdp; ++icdp) {
		if(countnparval(icdp+1,"tmin")!=nwin ||
		   countnparval(icdp+1,"tmax")!=nwin ||
		   countnparval(icdp+1,"rmin")!=nwin ||
		   countnparval(icdp+1,"rmax")!=nwin ||
		   countnparval(icdp+1,"rbeg")!=nwin ||
		   countnparval(icdp+1,"rend")!=nwin)
	err("number of tmin, tmax, rmin, rmax, rbeg and rend must equal nwin");

		if(!getnparfloat(icdp+1,"amp",amp[icdp]))
			amp[icdp][0] = 1.0;
		getnparfloat(icdp+1,"tmin",tmin[icdp]);
		getnparfloat(icdp+1,"tmax",tmax[icdp]);
		getnparfloat(icdp+1,"rmin",rmin[icdp]);
		getnparfloat(icdp+1,"rmax",rmax[icdp]);
		getnparfloat(icdp+1,"rbeg",rbeg[icdp]);
		getnparfloat(icdp+1,"rend",rend[icdp]);
	}


	/* get  optional parameters */
	if (!getparfloat("fmin",&fmin)) fmin = 0;
	if (!getparfloat("fmax",&fmax)) fmax = 0.25/dt;
	if (!getparfloat("ttaper",&ttaper)) ttaper = 0.2;
	if (!getparint("mult",&mult)) mult = 0;
	if (!getparint("maxnoff",&maxnoff)) maxnoff = 100;
	if (!getparint("nsoff",&nsoff)) nsoff = maxnoff;
	if (!getparfloat("prw",&prw)) prw = 0.01;


	offset = ealloc1float(maxnoff);
	trg = ealloc2float(nt,maxnoff);
	if(mult) {
		mfp = fopen("mfp","w");
		trgm = ealloc2float(nt,maxnoff);
	}

        /* open temporary file for headers */
        hfp = tmpfile();

	/* set old cdp or old shot for the first trace */
	oldcdp = (!shot)?tr.cdp:tr.sx;
	newcdp = oldcdp;
	nsoff_old = nsoff;
	gottrace = 1;
	done = 0;
	noff = 0;

	/* loop over traces */
	do {

                /* if cdp has changed or no more input traces */
                if (newcdp!=oldcdp || !gottrace) {

                        /* rewind trace header file */
                        fseek(hfp,0L,SEEK_SET);

                        /* cut overlarge offset and normalize offset array */
			nsoff = MIN(noff,nsoff_old);
			offmax = MAX(offset[0],offset[nsoff-1]);
                        for(ioff=0; ioff<nsoff; ++ioff)
                                offset[ioff] /= offmax;

			/* compute window parameters for old cdp */
			interpw(nwin,ncdp,cdp,tmin,oldcdp,tmint);
			interpw(nwin,ncdp,cdp,tmax,oldcdp,tmaxt);
			interpw(nwin,ncdp,cdp,rmin,oldcdp,rmint);
			interpw(nwin,ncdp,cdp,rmax,oldcdp,rmaxt);
			interpw(nwin,ncdp,cdp,rbeg,oldcdp,rbegt);
			interpw(nwin,ncdp,cdp,rend,oldcdp,rendt);
			interpw(1,ncdp,cdp,amp,oldcdp,ampt);

			/* Build multiple model */
		        for(iwin=0; iwin<nwin; ++iwin){
				tminw = tmint[iwin]-ft;
				tmaxw = tmaxt[iwin]-ft;
				rminw = rmint[iwin];
				rmaxw = rmaxt[iwin];
				rbegw = rbegt[iwin];
				rendw = rendt[iwin];

				if(tminw>=tmaxw || ampt[0]<0.01) break;

				rmo(trg,trgm,nt,dt,offset,nsoff,tminw,tmaxw,
					rminw,rmaxw,rbegw,rendw,fmin,fmax,
					ttaper,ampt[0],mult,prw);
		        }
			for(ioff=0; ioff<noff; ++ioff){
                                /* read trace header and determine cdp index */
                                efread(&tro,HDRBYTES,1,hfp);

				/* get multiple-removed trace */ 
				for (it=0; it<nt; ++it)
					tro.data[it] = trg[ioff][it];
				/* write output trace */
				puttr(&tro);
			}
			if(mult){
				fwrite(trgm[0],sizeof(float),nt*noff,mfp);
			}

                        /* if no more traces, break */
                        if (!gottrace) break;

                        /* rewind trace header file */
                        fseek(hfp,0L,SEEK_SET);

			/* reset offset number and old cdp*/
			noff = 0;
			oldcdp = (!shot)?tr.cdp:tr.sx;
		}

		if(!gottrace) break;

                efwrite(&tr,HDRBYTES,1,hfp);

		for(it=0; it<nt; ++it)
			trg[noff][it] = tr.data[it];

		offset[noff] = tr.offset*tr.offset;
		if(mult) 
			for(it=0; it<nt; ++it)
				trgm[noff][it] = 0;

		noff += 1;
		if(noff>maxnoff) err("number of offset exceeds  maxnoff");
		if(!gettr(&tr)) gottrace = 0;
			else newcdp = (!shot)?tr.cdp:tr.sx;
	}while(!done);
	
	free1float(tmint);
	free1float(tmaxt);
	free1float(rmint);
	free1float(rmaxt);
	free1float(rbegt);
	free1float(rendt);
	free2float(tmin);
	free2float(tmax);
	free2float(rmin);
	free2float(rmax);
	free2float(rbeg);
	free2float(rend);
	free2float(trg);
	if(mult) free2float(trgm);

	return EXIT_SUCCESS;
}

/* linearly interpolate/extrapolate window parameters between cdps */
static void interpw (int nt, int ncdp, float *cdp, float **w,
        float cdpt, float *wt)
{
        static int index=0;
        int it;
        float a1,a2;

        /* if before first cdp, constant extrapolate */
        if (cdpt<=cdp[0]) {
                for (it=0; it<nt; ++it)
                        wt[it] = w[0][it];

        /* else if beyond last cdp, constant extrapolate */
        } else if (cdpt>=cdp[ncdp-1]) {
                for (it=0; it<nt; ++it)
                        wt[it] = w[ncdp-1][it];

        /* else, linearly interpolate */
        } else {
                xindex(ncdp,cdp,cdpt,&index);
                a1 = (cdp[index+1]-cdpt)/(cdp[index+1]-cdp[index]);
                a2 = (cdpt-cdp[index])/(cdp[index+1]-cdp[index]);
                for (it=0; it<nt; ++it)
                        wt[it] = a1*w[index][it]+a2*w[index+1][it];
        }
}

