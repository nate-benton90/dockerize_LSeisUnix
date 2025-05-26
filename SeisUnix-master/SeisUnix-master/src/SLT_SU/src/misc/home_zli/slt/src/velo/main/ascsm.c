/* resample of an ascii file */

#include "velo.h"


char *sdoc = 
"ASCSM - smoothing of an ascii multi-column file 	\n"
"\n"
"ascsm [parameters] <input >output			\n" 
"\n"
"Required parameters:						 	\n"
"NONE \n"
"\n"
"Optional parameters:						 	\n"
"nicols=2       number of columns in the input ascii file \n"
"scol=2         starting column to apply smoothing \n"
"ncol=1         number of columns to apply smoothing \n"
"null=-99999    value to represent no-data points \n"
"ns=11          number of rows to used in smoothing \n"
"stype=0        smoothing type (0=box-car) \n"
"nrmax=100000   maximum number of rows in the input file \n"
"NOTE \n"
"     maximum of 10 data columns per row allowed \n" 
"AUTHOR:		Zhiming Li,       ,	5/18/98   		\n"    
;

void smth(float *datai, float *datao, int nr, int stype, int ns, float null);

main(int argc, char **argv)
{
    	char *cbuf; 
    	FILE *infp=stdin,*outfp=stdout;
    	float  *cc, *datai, *datao;
		int nicols, scol, ncol, ns, stype, nrmax;
		float null;
		int nr, ic, ir, nc, i;


    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

		if (!getparint("nicols",&nicols)) nicols = 2;
		if(nicols>10) err(" number of data columns must be less than 10\n");
		if (!getparint("scol",&scol)) scol = 2;
		if (!getparint("ncol",&ncol)) ncol = 1;
		if (!getparfloat("null",&null)) null = -99999;
		if (!getparint("ns",&ns)) ns = 11;
		if (!getparint("stype",&stype)) stype = 0;
		if (!getparint("nrmax",&nrmax)) nrmax = 100000;

/* memory allocation */
    	cbuf = (char*)malloc(134*sizeof(char));
    	cc = (float*)malloc(10*sizeof(float));
		datai = (float*) malloc(10*nrmax*sizeof(float));
		datao = (float*) malloc(10*nrmax*sizeof(float));

    	for (ir=0;ir<nrmax;ir++) {
       		if (feof(infp) !=0 ) break;
       		for(i=0;i<134;i++) cbuf[i]=' ';
       		gets(cbuf);
			sscanf(cbuf,"%f %f %f %f %f %f %f %f %f %f\n",
					&cc[0],&cc[1],&cc[2],&cc[3],&cc[4],
					&cc[5],&cc[6],&cc[7],&cc[8],&cc[9]);
			for(ic=0;ic<10;ic++) {
				datai[ir+ic*nrmax] = cc[ic];
				datao[ir+ic*nrmax] = cc[ic];
			}
			/*
			fprintf(stderr,"%g %g \n",xin[ir],yin[ir]);
			*/
		}

		nr = ir-1;
		nc = nicols;

		fprintf(stderr," input ascii file  %d data columns and %d rows\n",
			nc,nr);

		for(ic=scol-1;ic<scol-1+ncol;ic++) {
			fprintf(stderr," smoothing on column %d \n",ic+1);
			smth(datai+ic*nrmax,datao+ic*nrmax,nr,stype,ns,null);
		}

		for(ir=0;ir<nr;ir++) {
			for(ic=0;ic<nc;ic++) fprintf(outfp," %g",datao[ic*nrmax+ir]);
			fprintf(outfp,"\n");
		}

    free(cbuf);
    free(cc);
	free(datai);
	free(datao);

    return (0);
}


void smth(float *datai, float *datao, int nr, int stype, int ns, float null)
{
	float *yi, *yo, tmp;
	int i, n, ii, j;


	yi = (float *) malloc(nr*sizeof(float));
	yo = (float *) malloc(nr*sizeof(float));

	n = 0;
	for(i=0;i<nr;i++) {
		if(datai[i]!=null) {
			yi[n] = datai[i];
			n = n + 1;
		}
	}

	fprintf(stderr," input %d data rows\n",n);

	for(i=0;i<n;i++) {
		tmp = 0.;
		for(ii=i-ns/2;ii<=i+ns/2;ii++) {
			j = ii;
			if(ii<0) j = 0;
			if(ii>n-1) j = n-1;
			tmp += yi[j];
		}
		yo[i] = tmp / ns;
	}

	n = 0;
	for(i=0;i<nr;i++) {
		if(datai[i]!=null) {
			datao[i] = yo[n];
			n = n + 1;
		} else {
			datao[i] = null;
		}
	}


	free(yi);
	free(yo);
}
