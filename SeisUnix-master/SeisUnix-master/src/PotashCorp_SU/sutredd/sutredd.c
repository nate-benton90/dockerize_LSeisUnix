/* suslice */
/* B.Nemeth */



#include "suhdr.h"

/*********************** self documentation *****************************/
char *sdoc[] = {" SUTREDD - zero out noisy traces based on cross corelation  ",
"                                                                           ",
" sutredd < infile > outfile [optinal parameters]                           ",
"                                                                           ",
" Opional Parameter:						 	    ",
"                                                                           ",
" ts=                   start of analysis window                            ",
"                       default  start of trace                             ",
" te=			maximum time of analysis window; 		    ",
" 			default full trace                     		    ",
" npw=5			width of spatial correlation window                 ",
" trsh=1.0		treshold                                            ",
" Noisy traces will be zerod and header trid set two                        ",
NULL};
   
segy **get_gather(cwp_String *key,cwp_String *type,Value *n_val, int *nt,int *ntr,
		      float *dt,int *first);
segy **put_gather(segy **rec,int *nt, int *ntr);
void do_facor(float *data1,float *data2, float *acor,int n,int f,int nc);

/* Segy data constans */
segy tr;				/* SEGY trace */
int verbose;

int main( int argc, char *argv[] )
{
	/* Segy data constans */
	cwp_String key;		/* header key word from segy.h		*/
	cwp_String type;	/* ... its type				*/
	Value val;
	segy **rec_o;		/* trace header+data matrix */
	float *data;				/* trace data */
	float *sdata;				/* scaled trace data */
	float *mdata;				/* model trace data */
	float *xtr;		/* correlation window */
	float *corr;		/* correlation values */
	int ntx;		/* correlation window size */
	int first=0;		/* true when we passed the first gather */
	int nt;                 /* number of time samples               */
        int ntr=0;              /* number of traces                     */
	float dt;		/* sample interval */
	float ts;		/* min time of window */
	float te;		/* max time of window */
	int its;		/* min time of window in samples*/
	int ite;		/* max time of window in samples*/
	int ntw;		/* length of window in samples*/
	int npw;		/* length of window in samples*/
	int itr,imtr,imts,imte;
	int it;
	int d;
	
	float cval;		/* correlation value */
	float lcval;		/* left form the actual */
	float rcval;		/* right from the actual */
	float trsh;		/* treshold */
	int z=0; 		/* number of zerod traces */
	 
	initargs(argc, argv);
   	requestdoc(1);
	
	if (!getparstring("key", &key))	key = "fldr";
	if (!getparint("npw", &npw))	npw = 5;
	if (!getparint("d", &d))	d = 0;
	if (!getparfloat("trsh", &trsh)) trsh = 1.0;
	
        /* get information from the first header */
	rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);

	if(dt==0.0) {
		if(!getparfloat("dt",&dt)) { dt=0.002;
			warn("tr.dt is not set dt=0.002 is assumed");
		}
	}

	if(!getparfloat("ts",&ts)) ts=((*rec_o[0]).delrt)/1000.0;
	if(!getparfloat("te",&te)) te=(nt-1)*dt+((*rec_o[0]).delrt)/1000.0;
	
	
	its = (int) ( (ts-(float)((*rec_o[0]).delrt)/1000.0)/( dt)+0.5);
	ite = (int) ( (te-(float)((*rec_o[0]).delrt)/1000.0)/( dt)+0.5);
	
	if( its<0 || ite>(nt-1)) {
		err(" Time of window is out of bound");
	}
	ntw=ite-its;
	
	/* check for odd npw */
	if(npw%2==0) npw++;
	
	ntx=(int)(ntw/3);
	if(ntx%2==0) ntx++;
	xtr = ealloc1float(ntx);
	data = ealloc1float(ntw);
	mdata = ealloc1float(ntw);
	sdata = ealloc1float(ntw);
	
	do {
		corr = ealloc1float(ntr);
		memset( (void *) mdata, (int) '\0', ntw*FSIZE);
		
		/* form a model trace from a window around the trace*/
		/* 1st trace -1 to set things up */
		itr=-1;
		imts=MAX((itr-(npw-1)/2-1),0);
		imte=MIN((itr+(npw-1)/2+1),ntr-1);
		for(imtr=imts;imtr<imte;imtr++) {
			for(it=its;it<ite;it++)
				mdata[it]+=(*rec_o[imtr]).data[it];
		}
		
		/* for each trace */
		for(itr=0;itr<ntr;itr++) {
			
			/* boundaries of the window */
			imts=MAX((itr-(npw-1)/2-1),0);
			imte=MIN((itr+(npw-1)/2+1),ntr-1);
			
			/* add one trace from the right */
			/* if not the last one */
			if(imte<ntr-1)
				for(it=its;it<ite;it++)
					mdata[it]+=(*rec_o[itr]).data[it];
			
			/* remove one  trace from the left */
			/* if not the first one */
			if(imts>0)
				for(it=its;it<ite;it++)
					mdata[it]-=(*rec_o[itr]).data[it];
			
			/*remove the trace itself*/
			for(it=its;it<ite;it++)
				mdata[it]-=(*rec_o[itr]).data[it];
			
			/* Xcorrelation */
			for(it=its;it<ite;it++)
				sdata[it]=(*rec_o[itr]).data[it]*(imte-imts);
				
			do_facor(sdata,mdata,xtr,ntw,-ntx/2,ntx);
			corr[itr]=fabs(xtr[isamaxs(ntx,xtr,1)]);
			
			/* add the trace back */
			for(it=its;it<ite;it++)
				mdata[it]+=(*rec_o[itr]).data[it];
		}
		
		/* once the correlation is done analyze the values */
		lcval=rcval=0.0;
		z=0;
		for(itr=0;itr<ntr;itr++) {
			cval=corr[itr];
			if(itr>0) lcval=corr[itr-1];
			if(itr<ntr-1) rcval=corr[itr+1];
			if(itr==0) lcval=rcval;
			if(itr==(ntr-1)) rcval=lcval;
			cval=cval/(lcval+rcval)*.5;
			if(d) fprintf(stderr," %d %f\n",itr+1,cval);
			if(cval>trsh) {		
				/* zero out the trace */
				memset( (void *) (*rec_o[itr]).data , (int) '\0', nt*FSIZE);
				(*rec_o[itr]).trid=2;
				z++;
			}
		}
		
		fprintf(stderr," Zerod %d traces out of %d for file %d\n",
		               z,ntr,(*rec_o[0]).fldr);	
		
		rec_o = put_gather(rec_o,&nt,&ntr);
		free1float(corr);
		
		rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);		
	} while(ntr);
	
	free1float(xtr);
	free1float(data);
	free1float(mdata);
	free1float(sdata);
   return EXIT_SUCCESS;
}
segy **get_gather(cwp_String *key,cwp_String *type,Value *n_val, int *nt,int *ntr,
		      float *dt,int *first)
{
	int nsegy;
	FILE *tracefp;
	FILE *headerfp;
	segy **rec=NULL;
	int ntrr=0;
	int indx=0;
	static Value val;
	
	
	*type = hdtype(*key);
	indx = getindex(*key);
        *ntr = 0;

	if(*first==0) {

		/* Take a trace count */
        	/* get info from first trace */
        	nsegy = gettr(&tr);
		if (nsegy==0)  err("can't get first trace");
        	ntrr++;
		*nt = tr.ns;
        	*dt = (float) tr.dt/1000000.0;
		gethval(&tr, indx, n_val);
		*first=1;
	}

        /* Store traces in tmpfile while getting a count */
	tracefp = etmpfile();
	headerfp = etmpfile();
	if (verbose) warn("using tmpfile() call");
        do {
                efwrite(&tr, 1, HDRBYTES, headerfp);
                efwrite(tr.data, FSIZE, *nt, tracefp);
		/* read the next trace */
		*ntr+=1;
		val=*n_val;
		nsegy = gettr(&tr);
		if(nsegy) ntrr++;
		gethval(&tr, indx, n_val);
	} while (nsegy && !valcmp(*type,val,*n_val));

	/* No more data */
	if(nsegy==0 && ntrr==0 ) {
		 *ntr=0;
		 return(rec=NULL);
	}
        
	if(rec!=NULL) free(rec);
	
	/* allocate memory for the record */
	{ register int i;
		rec = ealloc1(*ntr,sizeof(segy *));
		for(i=0;i<*ntr;i++)
			rec[i] = (segy *)ealloc1((*nt*FSIZE+HDRBYTES),sizeof(char));
	}
	
	
	/* load traces into an array and close temp file */
	erewind(headerfp);
        erewind(tracefp);
	{ register int ix;
        	for (ix=0; ix<*ntr; ix++)
                	fread (rec[ix],HDRBYTES, 1, headerfp);
        	efclose (headerfp);
		for(ix=0; ix<*ntr; ix++)
                	fread ((*rec[ix]).data,FSIZE, *nt, tracefp);
        	efclose (tracefp);
	}
	
	return(rec);	
}

segy **put_gather(segy **rec,int *nt, int *ntr)
{


	segy tr;
	
	{ register int i;
		for(i=0;i<*ntr;i++) {
			memcpy( (void *) &tr, (const void *) rec[i],
					*nt*FSIZE+HDRBYTES);
			puttr(&tr);
			free1((void *)rec[i]);
		}
	}
	return(rec=NULL);
}
/* Fast crosscorelation with FFT */
void do_facor(float *data1,float *data2, float *acor,int n,int f,int nc)
{
#define LOOKFAC	2	/* Look ahead factor for npfaro	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */

	int nfft;
	int nf;
	int i,j;
	float snfft;
	complex *fft1;
	complex *fft2;
	float *pdata1;
	float *pdata2;


	/* Set up pfa fft */
	nfft = npfaro(n,LOOKFAC*n);
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
	snfft=1.0/nfft;
	nf=nfft/2+1;
	if(abs(f) > nc ) 
		err(" First element of correlation is out of bound\n");
	if(nc>n) 
		err(" Length of corr is larger than the length of input \n");
	

	fft1 = ealloc1complex(nfft);
	fft2 = ealloc1complex(nfft);
	pdata1 = ealloc1float(nfft);
	pdata2 = ealloc1float(nfft);
	
	memcpy( (void *) pdata1, (const void *) data1, n*FSIZE);
	memset( (void *) &pdata1[n],  (int) '\0', (nfft-n)*FSIZE);
	memcpy( (void *) pdata2, (const void *) data2, n*FSIZE);
	memset( (void *) &pdata2[n],  (int) '\0', (nfft-n)*FSIZE);
	
	pfarc(1,nfft,pdata1,fft1);
	pfarc(1,nfft,pdata2,fft2);
	for(i=0;i<nf;i++) fft1[i] = cmul(fft1[i],conjg(fft2[i]));
	pfacr(-1,nfft,fft1,pdata1);
	sscal(n,snfft,pdata1,1);
	
	if (f < 0) {
		for(i=abs(f),j=0;i>=MAX(0,-f-nc);i--,j++)
			acor[j] = pdata1[i];
		for(i=0;j<nc;i++,j++)
			acor[j] = pdata1[i];
	} else {
		for(i=f,j=0;i<MIN(nc,n-f);i++,j++)
			acor[j] = pdata1[i];
	}	
		
	free1complex(fft1);
	free1complex(fft2);
	free1float(pdata1);
	free1float(pdata2);
}
