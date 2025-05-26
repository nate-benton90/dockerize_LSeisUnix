/* Program to stack shots and autocorrelation to remove source delays */

#include "suhdr.h"
#define  MNG 100 

/*********************** self documentation **********************/
char *sdoc[] = {
"   SUSHSTK - Gather stacking with an optional static shift correction ",
"					                               ",
"   sushstk < stdin > stdout [optional parameters ]                    ",
"  					                               ",
"   Optional prameters			                               ",
"  					                               ",
"   st=1       correct for static shift between gathers; 0 no correction",
"   stk=1      0 Do not stack the records                              ",
"   shfm=.015  maximum allowed static shift                            ",
"   key=fldr     segy keyword that marks the records                     ",
"                                                                      ",
"   The correction is computed by xcorrelating the records.             ",
NULL};

segy **get_gather(cwp_String *key,cwp_String *type,Value *n_val, int *nt,int *ntr,
		      float *dt,int *first);
segy **put_gather(segy **rec,int *nt, int *ntr);
void do_facor(float *data1,float *data2, float *acor,int n,int f,int nc);

segy tr;
int verbose;
int main( int argc, char *argv[] )
{
	cwp_String key;		/* header key word from segy.h		*/
	cwp_String type;	/* ... its type				*/
	Value val;
	segy **rec_o[MNG];	/* trace header+data matrix */
	float *xtr;		/* correlation window */
	int ntx;		/* correlation window size */
	int first=0;		/* true when we passed the first gather */
	int lag;		/* time shift in smaples between traces */
	float dt;		/* sample interval  */
	int nt;			/* number of time samples */
	int ntr;		/* total number of traces in first record */
	int ntf;		/* number of time samples in first record */
	int ntrf;		/* total number of traces */
	int ing=0;		/* gather counter */
	float *tmp;		/* temporary storage for shifted trace */
	int *rlag;		/* realtive shift between records in samples */
	int avlag=0;
	int verbose;
	
	int st;			/* st. correction flag */
	int stk;		/* stacking  flag */
	unsigned short int **nnz=NULL; /* nonzero stacked values */
	float shfm;		/* maximum allowed shift */
	int ishfm;		/* maximum allowed shift in samples */
	
	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);
	
	if (!getparstring("key", &key))		key = "fldr";
	if (!getparint("st", &st))		st = 1;
	if (!getparint("stk", &stk))		stk = 1;
	if (!getparint("verbose", &verbose))	verbose = 0;
	if (!getparfloat("shfm", &shfm))	shfm = 0.015;
	

	/* get the first record */
	rec_o[ing] = get_gather(&key,&type,&val,&ntf,&ntrf,&dt,&first);
	if(verbose && ntrf) {
			fprintf(stderr," %d %d %d %d\n",
					 ntf,ntrf,ntrf,(*rec_o[ing][0]).ep);
	}
	if(ntrf==0) err("Can't get first record\n");
	nt=ntf;
	ishfm=(int)(shfm/dt);
	tmp = ealloc1float(ntf);
	
	do {
		ing++;
		rec_o[ing] = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
		if(verbose && ntr) {
			fprintf(stderr," %d %d %d %d\n",
					 ntf,ntrf,ntr,(*rec_o[ing][0]).ep);
		}	
		if(ntr>0 && ntr!=ntrf) err("records of different size\n");
	} while(ntr);
	
	rlag = ealloc1int(ing);
	
	{ register int ir,itr,it,lcnt,tlag;
		ntx=ntf-2;
		if(ntx%2==0) ntx++;
		xtr = ealloc1float(ntx);
		
		/* For each record.. */
		for(ir=1;ir<ing;ir++) {
			lag=0;
			lcnt=0;
			/* for each trace.. */
			for(itr=0;itr<ntrf;itr++) {
				
				/* do the corss correalation */ 		
				do_facor((*rec_o[ir][itr]).data,(*rec_o[0][itr]).data,
				          xtr,ntf,-ntx/2,ntx);
				
				/* pick the maximum */
				tlag = isamaxs(ntx,xtr,1)-ntx/2;
				if(abs(tlag) < ishfm) {
					lag += tlag;
					lcnt++;
				}
			}
			
			/* averaged lag of the traces for this record */
			lag = (int)(lag/lcnt);
			rlag[ir]=lag;
			
		}
		free1float(xtr);
		
		/* compute the average lag relative to the first */
		for(ir=1;ir<ing;ir++) avlag +=rlag[ir];
		avlag =(int)rint((double)(avlag/(ing-1)));
		
		/* create the shift values */
		rlag[0]=-avlag;
		for(ir=1;ir<ing;ir++) rlag[ir]-=avlag;
		
		/* apply the shifts */
		for(ir=0;ir<ing;ir++) {
			lag=rlag[ir];
			if (verbose) fprintf(stderr," %d",lag);
			for (itr=0;itr<ntrf;itr++) {
				memset( (void *) tmp, (int) '\0', ntf*FSIZE);
				if ( lag > 0) {   /* trace has to be shifted down */
					for(it=0;it<nt-lag;it++) tmp[it+lag] = (*rec_o[ir][itr]).data[it];
					/*memcpy( (void *) &tr.data[it+lag], (const void*)
					traces[ix],
					(nt-lag)*FSIZE); */
				} else {	   /* trace has to be shifted up */
					for(it=0;it<nt+lag;it++) tmp[it] = (*rec_o[ir][itr]).data[it-lag]; 
					/* memcpy( (void *) &tr.data, (const void *)&(traces[ix])[it-lag],
					(nt+lag)*FSIZE); */
				}
				memcpy((void *)(*rec_o[ir][itr]).data, (const void *) tmp,nt*FSIZE);
			}
		}
		
		if(stk) {
			nnz = (unsigned short int**) ealloc2(nt,ntrf,sizeof(unsigned short int));
			memset ((void *) nnz[0], (int) '\0', nt*ntrf*sizeof(unsigned short int)); 
			for(ir=1;ir<ing;ir++) {
				for (itr=0;itr<ntrf;itr++) {
					for(it=0;it<nt;it++){ 
						if((*rec_o[ir][itr]).data[it]!=0.0) {
							(*rec_o[0][itr]).data[it]+=(*rec_o[ir][itr]).data[it];
							nnz[itr][it]++;
						}
					}
				}
			}
			for (itr=0;itr<ntrf;itr++) {
				for(it=0;it<nt;it++){
					if(nnz[itr][it]>0) 
						(*rec_o[0][itr]).data[it]/=nnz[itr][it];
				}
			} 
		}
	}
	
	if(verbose) warn("\nNumber of gathers %10d\n",ing);
	{ register int ir,itr;
		/* if stacked only output the first record */
		if(stk) {
			for (itr=0;itr<ntrf;itr++) (*rec_o[0][itr]).nvs=ing;
			ing=1;
		}
		for(ir=0;ir<ing;ir++) 
		    rec_o[ir] = put_gather(rec_o[ir],&ntf,&ntrf);
	}
	if(nnz!=NULL) free2((void**)nnz);
	free1float(tmp);
	free1int(rlag); 
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
