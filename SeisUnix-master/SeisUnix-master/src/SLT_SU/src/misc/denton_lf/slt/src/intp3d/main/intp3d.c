/* fast and accurate taup-p interpolation  */

#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"INTP3D - 3D inline or crossline tau-p interpolation 		\n" 
"\n"
"intp3d [parameters] <input-data >interpolated-data 		\n" 
"\n"
"Required parameters:							\n"
"None \n"
"Optional parameters:							\n"
"pkey=tracl             primary key word to identify gather type \n"
"skey=tracr             secondary key word to identify trace position \n"
"ofill=                 starting skey number to output \n"
"                       (default to the minimum skey of each input gather)\n"
"nfill=                 number of skey number to ouput \n"
"                       (default to the number of skey of each input gather)\n"
"dfill=1                skey number increment to output \n"
"nsmax=512              maximum number traces per input gather \n"
"extrap=1               extrapolate trace output (1=yes 0=fill in with zero)\n"
"datain=standard input  name of input data (default to standard input)	\n"
"dataout=standard output name of input data (default to standard output)\n"
"fmin=0.                lowest frequency to process (Hz)		\n"
"fmax=2./3*(0.5/dt)     highest frequency to process (Hz)		\n"
"                       (default to two thirds of Nyquist)		\n" 
"ntfft=nt*1.5           fft length of trace 				\n"
"niter=10               maximum number of iterations used to solve 	\n"
"                       for the inverse filter equation 	 	\n"
"tol=0.000001           tolerance value used to solve 			\n"
"                       for the inverse filter equation 	 	\n"
"Notes:									\n"
"1. See Tech Memo SRA GR 91-1M for technical discussions 		\n" 
"2. In the output, original traces will be marked with key word \n"
"   duse=1 (segy trace header bytes 35-36), while interpolated traces \n"
"   will be marked with key word duse=2 \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	11/12/92   \n"		    
;


void intout(float *xi, float *xo, float *yo, char *hdrs, int nxi, int nxo,
		int nt, FILE *outfp, int extrap, 
		String hdtp, int index);

void changeval(String type, Value *val, float f);

void intps(float *xi, float *yi, float *xo, float *yo, int nxi, int nxo,
		int nt, float dt, int np, float fmin, float fmax, int ntfft, 
		int niter, float tol, 
		float *xipre, int nxipre, complex *ccexp, int iccexp,
		complex *ccexpo, int iccexpo);  


main(int argc, char **argv)
{
    	int nt,nxi,nxo,ntfft,it,ix,nf,i,extrap;
	int nxipre;
    	float dt,fmin,fmax;
    	float *xi, *xo, *xipre;
    	float *yi, *yo;
    	char *hdrs;
    	float tol;
    	int niter, np;
    	string datain, dataout;
	complex *ccexp, *ccexpo;
	int iccexp=1, iccexpo=1;
	int nxip, ifmin, lftwk, npp, nph;
	float tmp, tmp2;
	float df; 

    	FILE *infp,*outfp;

	segytrace tr;

	String pkey="tracl", ptype, skey="tracr", stype;
     	Value pval, sval;
       	int indxp, indxs;

	int ofill=0, dfill=1, nfill=1;
	int iof=1, inf=1;
	int nsmax, ns; 
	int is, ip, ipre;


	float *sort;
	int *sortindex;
	char *hdrsort;



    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	getparstring("pkey",&pkey);
	getparstring("skey",&skey);
	if(!getparint("ofill",&ofill)) iof = 0 ;
        if(!getparint("dfill",&dfill)) dfill = 1;
       	if(!getparint("nfill",&nfill))inf = 0;
     	if (!getparint("nsmax",&nsmax)) nsmax = 2000;

	/* open input/output */
    	if (!getparstring("datain",&datain)) {
		infp = stdin;
	} else {
		infp = fopen(datain,"r");
	}
    	if (!getparstring("dataout",&dataout)) {
		outfp = stdout;
	} else {
		outfp = fopen(dataout,"w");
	}
	/* make file size to be able to exceed 2 G on convex */
	file2g(infp);
	file2g(outfp);

	/* read in first trace for nt and dt */
        if (!fgettr(infp,&tr))  err("can't get first trace");
	nt = tr.ns; 
	dt = (float)tr.dt/1000000.;

	/* optional parameters */
    	if (!getparint("ntfft",&ntfft)) ntfft=(nt*3/2)/2*2;
    	if (ntfft < nt) ntfft = nt;
	nf = ntfft;
	radix_(&nf,&ntfft);
    	if (!getparfloat("fmin",&fmin)) fmin = 0.;
    	if (!getparfloat("fmax",&fmax)) fmax = .5 / dt * 2. / 3.;
    	if (!getparint("niter",&niter)) niter = 10; 
    	if (!getparfloat("tol",&tol)) tol = 0.000001;
    	if (!getparint("extrap",&extrap)) extrap = 1; 


        ptype  = hdtype(pkey);
	indxp = getindex(pkey);
	stype  = hdtype(skey);
	indxs = getindex(skey);
	gethval(&tr, indxp, &pval);
	ipre = vtoi(ptype,pval);
	gethval(&tr, indxs, &sval);
	is = vtoi(stype,sval);

	nxo = nfill;
	if(inf==0) nxo = nsmax;

    	xi = (float*)malloc(nsmax*sizeof(float));
    	xipre = (float*)malloc(nsmax*sizeof(float));
    	yi = (float*)malloc(nt*nsmax*sizeof(float));
    	xo = (float*)malloc(nxo*sizeof(float));
    	yo = (float*)malloc(nt*nxo*sizeof(float));
	hdrs = (char*) malloc(nsmax*HDRBYTES*sizeof(char));

	sort = (float*)malloc(nsmax*nt*sizeof(float));
	hdrsort = (char*)malloc(nsmax*HDRBYTES);
	sortindex = (int*)malloc(nsmax*sizeof(int));

	iccexp = 1;
	nxip = nsmax + 10;
    	initpf_(&ntfft,&dt,&fmin,&fmax,&nxip,&ifmin,&df,
		&nf,&lftwk,&npp,&nph);
	tmp = (nph*2+1)*nf*nxip;
	tmp = tmp*sizeof(complex);

	if( tmp > 1600000000. ) {
		fprintf(stderr," --- forward t-p coefficients --- " );
		fprintf(stderr," need memory %f MB \n",tmp/1000000.);
		fprintf(stderr," memory limit exceeded \n");
		fprintf(stderr," no pre-computation of coefficients \n");
		iccexp = -1; 
		ccexp = (complex*) malloc(sizeof(complex));
		tmp = 0.;
	} else {
		ccexp = (complex*) malloc((nph*2+1)*nf*nxip*sizeof(complex));
	}

	iccexpo = 1;
	tmp2 = (nph*2+1)*nf*nxo;
	tmp2 = tmp2*sizeof(complex);
	tmp = tmp + tmp2;
	if( tmp > 1600000000. ) {
		fprintf(stderr," --- inverse t-p coefficients --- " );
		fprintf(stderr," need memory %f MB \n",tmp/1000000.);
		fprintf(stderr," memory limit exceeded \n");
		fprintf(stderr," no pre-computation of coefficients \n");
		iccexpo = -1; 
		ccexpo = (complex*) malloc(sizeof(complex));
	} else {
		ccexpo = (complex*) malloc((nph*2+1)*nf*nxo*sizeof(complex));
	}

   	for(ix=0;ix<nxo;ix++) xo[ix] = ofill + ix*dfill;

	/* loop over input traces */
	ns = 0;
	nxipre = 0;

	do {

	
                gethval(&tr, indxp, &pval);
		ip = vtoi(ptype,pval);
		gethval(&tr, indxs, &sval);
		is = vtoi(stype,sval);

		if(ns>nsmax) 
			err("maximum number traces %d exceed %d \n",ns,nsmax);

		if(ip==ipre) {

			for(it=0;it<nt;it++) yi[it+ns*nt] = tr.data[it];
			xi[ns] = is;
			bcopy((char*)&tr,hdrs+ns*HDRBYTES,HDRBYTES);
			ns = ns + 1;

		} else if(ip!=ipre && ns>0) {

			nxi = ns;
			/* sort xi into ascending order */
			for(i=0;i<nxi;i++) sortindex[i] = i;
			qkisort(nxi,xi,sortindex);
			bcopy(yi,sort,nxi*nt*sizeof(float));

			for(i=0;i<nxi;i++) {
               			bcopy(sort+sortindex[i]*nt,yi+i*nt,
					nt*sizeof(float));
			}

			bcopy(xi,sort,nxi*sizeof(float));
			for(i=0;i<nxi;i++) {
               			xi[i] = sort[sortindex[i]];
			}

			bcopy(hdrs,hdrsort,nxi*HDRBYTES);
			for(i=0;i<nxi;i++) {
			    bcopy(hdrsort+sortindex[i]*HDRBYTES,
				hdrs+i*HDRBYTES,HDRBYTES);
			}

			np = nxi;

			if(inf==0) nxo = nxi;
			if(iof==0) {
				for(ix=0;ix<nxo;ix++) xo[ix] = xi[ix];
			}

			/* tau-p interpolation */
			intps(xi,yi,xo,yo,nxi,nxo,
				nt,dt,np,fmin,fmax,ntfft, 
				niter,tol,xipre,nxipre,ccexp,iccexp,
				ccexpo,iccexpo);
			for(ix=0;ix<nxi;ix++) {
				xipre[ix] = xi[ix];
			}
			nxipre = nxi;


			/* output gather */
			intout(xi,xo,yo,hdrs,nxi,nxo,nt,
				outfp,extrap,stype,indxs);

			fprintf(stderr,
" interpolation done from input %d live traces to output %d traces at %s=%d \n",nxi, nxo, pkey, ipre);

			ns = 0;
			for(it=0;it<nt;it++) yi[it+ns*nt] = tr.data[it];
			xi[ns] = is;
			bcopy((char*)&tr,hdrs+ns*HDRBYTES,HDRBYTES);

			ipre = ip;
			ns = ns + 1;
		}
			
	} while(fgettr(infp,&tr)); 
			

	if (ns>0) {

		nxi = ns;

		for(i=0;i<nxi;i++) sortindex[i] = i;
		qkisort(nxi,xi,sortindex);
		bcopy(yi,sort,nxi*nt*sizeof(float));

		for(i=0;i<nxi;i++) {
       			bcopy(sort+sortindex[i]*nt,yi+i*nt,
			nt*sizeof(float));
		}
		bcopy(xi,sort,nxi*sizeof(float));
		for(i=0;i<nxi;i++) {
               		xi[i] = sort[sortindex[i]];
		}
		bcopy(hdrs,hdrsort,nxi*HDRBYTES);
		for(i=0;i<nxi;i++) {
		    bcopy(hdrsort+sortindex[i]*HDRBYTES,
			hdrs+i*HDRBYTES,HDRBYTES);
		}

		np = nxi;

		if(inf==0) nxo = nxi;
		if(iof==0) {
			for(ix=0;ix<nxo;ix++) xo[ix] = xi[ix];
		}

		/* interpolation */
                intps(xi,yi,xo,yo,nxi,nxo,nt,dt,np,fmin,fmax,ntfft,niter,tol,
			xipre,nxipre,ccexp,iccexp,
			ccexpo,iccexpo);
		/* output gather */
		intout(xi,xo,yo,hdrs,nxi,nxo,nt,outfp,extrap,stype,indxs);

		fprintf(stderr,
" interpolation done from input %d live traces to output %d traces at %s=%d \n",nxi, nxo, pkey, ip);

	}


    	free(xi);
    	free(yi);
    	free(xo);
    	free(yo);
	free(hdrs);
	free(sort);
	free(sortindex);
	free(hdrs);

	return 0;

}



void intout(float *xi, float *xo, float *yo, char *hdrs, int nxi, int nxo,
		int nt, FILE *outfp, int extrap,
		String hdtp, int index) {

	segytrace tro, tr1, tr2;
	float tmp, res, ftmp;
	int one=1, indx, itmp, ix, it;
	float sx, gx, sy, gy, mx, my, dd, ofo;
	Value val;

 
	/* output interpolated result */
	for(ix=0;ix<nxo;ix++) {
		for(it=0;it<nt;it++) tro.data[it]=yo[it+ix*nt];
		tmp = xo[ix];
		bisear_(&nxi,&one,xi,&tmp,&indx);
		if(tmp<=xi[0]) {
			itmp = 1;
			bcopy(hdrs,(char*)&tro,HDRBYTES);
		} else if(tmp>=xi[nxi-1]) {
			itmp = nxi-1;
			bcopy(hdrs+(nxi-1)*HDRBYTES,
				(char*)&tro,HDRBYTES);
		} else {
			if(indx==nxi) indx=indx-1;
			itmp = indx;
			if(abs(tmp-xi[itmp-1])<abs(tmp-xi[itmp])) {
				bcopy(hdrs+(itmp-1)*HDRBYTES,
					(char*)&tro,HDRBYTES);
			} else {
				bcopy(hdrs+itmp*HDRBYTES,
					(char*)&tro,HDRBYTES);
			}
		}

		bcopy(hdrs+(itmp-1)*HDRBYTES,(char*)&tr1,
			HDRBYTES);
		bcopy(hdrs+itmp*HDRBYTES,(char*)&tr2,
			HDRBYTES);
		res = (tmp-xi[itmp-1])/(xi[itmp]-xi[itmp-1]);

		ftmp  = tr1.cdp + res*(tr2.cdp-tr1.cdp) + .5;
		tro.cdp = ftmp;
		ftmp = tr1.mute + res*(tr2.mute-tr1.mute) + .5; 
		tro.mute = ftmp;

		ftmp = tr1.sx + res*(tr2.sx-tr1.sx) + .5; 
		tro.sx = ftmp;
		ftmp = tr1.sy + res*(tr2.sy-tr1.sy) + .5; 
		tro.sy = ftmp;
		ftmp = tr1.gx + res*(tr2.gx-tr1.gx) + .5; 
		tro.gx = ftmp;
		ftmp = tr1.gy + res*(tr2.gy-tr1.gy) + .5; 
		tro.gy = ftmp;

		/* if offsets are the same sign, linearly interpolate
		   to get the output offset; otherwise, use the
		   closest trace's offset */

		if(tr1.offset*tr2.offset >= 0.) {
			ftmp = tr1.offset + res*(tr2.offset-tr1.offset) + .5; 
			tro.offset = ftmp;
		} 

		if(abs(tmp-xi[itmp-1])<0.1 || abs(tmp-xi[itmp])<0.1) {
			tro.duse = 1;
		} else {
			tro.duse = 2;
		}

		changeval(hdtp,&val,xo[ix]);
		puthval(&tro, index, &val);

		/* need to adjust (x,y) of source and receiver, with
		the new offset for pre-stack migration */

		sx = tro.sx;
		sy = tro.sy;
		gx = tro.gx;
		gy = tro.gy;
		mx = (sx+gx)/2.;
		my = (sy+gy)/2.;
		dd = sqrt((sx-gx)*(sx-gx)+(sy-gy)*(sy-gy));
		if(tro.scalco>1) {
			dd = dd * tro.scalco;
		} else if(tro.scalco<0) {
			dd =  - dd / tro.scalco;
		}
		ofo = tro.offset;
		if(ofo<0) ofo = - ofo;
		if (dd>0.) {
			tro.sx = mx+(tro.sx-mx)*ofo/dd;
			tro.gx = mx+(tro.gx-mx)*ofo/dd;
			tro.sy = my+(tro.sy-my)*ofo/dd;
			tro.gy = my+(tro.gy-my)*ofo/dd;
		} else {
			tro.sx = mx-ofo/2;
			tro.gx = mx-ofo/2;
			tro.sy = my;
			tro.gy = my;
		}

		if(xi[0]-tmp>0.1 || tmp-xi[nxi-1]>0.1 ) {
			if(extrap==0) {
				tro.trid = 2;
				bzero(tro.data,nt*sizeof(float));
			}
		}

		fputtr(outfp,&tro); 
	}
}

void changeval(String type, Value *val, float f) {
	switch (*type) {
        case 's':
                err("can't change char header word");
        break;
        case 'h':
                val->h = f;
        break;
        case 'u':
                val->u = f;
        break;
        case 'l':
                val->l = f;
        break;
        case 'v':
                val->v = f;
        break;
        case 'i':
                val->i = f;
        break;
        case 'p':
                val->p = f;
        break;
        case 'f':
                val->f = f;
        break;
        case 'd':
                val->d = f;
        break;
        default:
                err("unknown type %s", type);
        break;
        }
}

void intps(float *xi, float *yi, float *xo, float *yo, int nxi, int nxo,
		int nt, float dt, int np, float fmin, float fmax, int ntfft, 
		int niter, float tol, 
		float *xipre, int nxipre, complex *ccexp, int iccexp,
		complex *ccexpo, int iccexpo) {

	int it,ix,lftwk;
        float df,dxmin;
        float *dxi, *dp, *wp;
        float *wk, *ftwk;
        complex *sig, *b, *d, *r, *wk1, *wk2, *wk3, *wk4, *wk5;
        complex *swk, *fyi, *si, *so;
        complex ctemp;
        int ifmin, nph, nf, inifft, iniwp;

	float *xip, *yip, dxp;
	int nxip, npp;
	int icexp, icexpo;

	/* check to see if the input xi is changed or not */
	if(iccexp == -1) {
		icexp = -1;
	} else {
		icexp = 0;
		icexpo = 0;
		if(nxi == nxipre) {
			icexp = 1;
			icexpo = 1;
			for(ix=0;ix<nxi;ix++) {
				if(xi[ix]!=xipre[ix]) {
					icexp = 0;
					icexpo = 0;
					break;
				}
			}
		}
	}
	if(iccexpo == -1) {
		icexpo = -1;
	}

	/* pad input data 5 traces on each side */
	nxip = nxi + 10;
	npp = np + 10;

    	dxi = (float*)malloc(nxip*sizeof(float));
    	xip = (float*)malloc(nxip*sizeof(float));
    	yip = (float*)malloc(nxip*nt*sizeof(float));

	if(nxi>1) {
		dxp = xi[1] - xi[0];
	} else {
		dxp = 1;
	}
	for(ix=0;ix<5;ix++) {
		xip[ix] = xi[0] + (ix-5)*dxp;
		/* taper first trace and put at padded trace location */
		for(it=0;it<nt;it++) {
			yip[it+ix*nt]=yi[it]*(ix+1.)/5.;
		}
	}
	for(ix=0;ix<nxi;ix++) {
		xip[ix+5] = xi[ix]; 
		for(it=0;it<nt;it++) yip[it+(ix+5)*nt] = yi[it+ix*nt];
	}
	if(nxi>1) {
		dxp = xi[nxi-1] - xi[nxi-2];
	} else {
		dxp = 1;
	}
	for(ix=0;ix<5;ix++) {
		xip[nxi+5+ix] = xi[nxi-1] + (ix+1)*dxp;
		/* taper last trace and put at padded trace location  */
		for(it=0;it<nt;it++) {
			yip[it+(ix+nxi+5)*nt]=yi[it+(nxi-1)*nt]*(5.-ix)/5.;
		}
	}

	/* initialize tau-p interpolation */
    	initpf_(&ntfft,&dt,&fmin,&fmax,&nxip,&ifmin,&df,
		&nf,&lftwk,&npp,&nph);

	/* design inverse filter */
    	d = (complex*)malloc(npp*sizeof(complex));
    	sig = (complex*)malloc(npp*sizeof(complex));
    	r = (complex*)malloc(npp*npp*sizeof(complex));
    	b = (complex*)malloc(npp*sizeof(complex));
    	wk1 = (complex*)malloc(npp*sizeof(complex));
    	wk2 = (complex*)malloc(npp*sizeof(complex));
    	wk3 = (complex*)malloc(npp*sizeof(complex));
    	wk4 = (complex*)malloc(npp*sizeof(complex));
    	wk5 = (complex*)malloc(npp*sizeof(complex));


    	filter_(xip,&nxip,&nph,dxi,&dxmin,
		d,sig,r,b,wk1,wk2,wk3,wk4,wk5,&tol,&niter);

    	free(sig);
    	free(r);
    	free(b);
    	free(wk1);
    	free(wk2);
   	free(wk3);
    	free(wk4);
    	free(wk5);

    	/* trace fft */
    	fyi = (complex*)malloc(nf*nxip*sizeof(complex));
    	ftwk = (float*)malloc(lftwk*sizeof(float));
    	wk = (float*)malloc(ntfft*sizeof(float));
    	inifft = 0;
    	trafft_(yip,fyi,wk,ftwk,&nt,&ntfft,&nxip,
		&ifmin,&nf,&lftwk,&inifft);

	/* forward tau-p in frequency domain */
    	iniwp = 0;
    	si = (complex*)malloc(nf*npp*sizeof(complex));
    	wp = (float*)malloc(nf*npp*sizeof(float));
    	dp = (float*)malloc(nf*sizeof(float));
    	fwdtp_(fyi,si,wp,dp,&nf,&ifmin,&df,
		&nph,xip,dxi,&nxip,&dxmin,&iniwp,ccexp,&icexp);
    	free(fyi);
    	free(dxi);

    	/* apply the filter  */
    	so = (complex*) malloc(nf*npp*sizeof(complex));
    	filtp_(si,so,d,&nf,&nph);
    	free(si);
    	free(d);

    	/* inverse tau-p transform */

    	swk = (complex*)malloc(nf*sizeof(complex));
    	invtp_(so,yo,dp,swk,wp,xo,wk,ftwk,
			&nf,&nt,&ntfft,&nxo,&nph,&ifmin,&lftwk,
			ccexpo,&icexpo); 

    	free(so);
    	free(swk);
    	free(wp);
    	free(wk);
    	free(ftwk);
    	free(dp);
	free(xip);
	free(yip);

	np = npp;
}

