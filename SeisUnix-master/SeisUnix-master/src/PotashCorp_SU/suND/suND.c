/* Multi dimensional selection of traces */

#include "su.h"
#include "segy.h"          
#include "suhdr.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 	   							",
" suND   - Multi dimensional selection of traces based header on words ",
" 	   							",
"   suND df=datafile hf=headerfile cf=crossreference file	",
" 								",
"   nd=		Number of dimensions				",
" 								",
"   pr=par1,par2,par3,..parn Range of parameters for selection	",
"   key=key1,key2,key3,..keyn Header words corresponding to     ",
" 			      parameter array.			",
" 								",
" 								",
"   Header file format:						",
"	indx1 dim1 indx2 dim2 indx3 dim3 ... indxN dimN Seq	",
"								",
"   Cross reference file format					",
"	indx1 indx2 indx3 indx4 .... indxN Seq             	",
"								",
"  The outputs are gathers, headerword gaps marks the gathers	",
"								",
NULL};

/**************** end self doc ***********************************/

typedef struct par_rec {
	long i;
	long p;
} par_rec;

void fix_lower(int n, void *a,size_t size,unsigned int *indx);
void fix_upper(int n, void *a,size_t size,unsigned int *indx);
void usort_par_v(par_rec *a, unsigned int n, unsigned int *un);
void sort_par_v(par_rec *a, unsigned int n);
int cmp_par_p(const void *a, const void *b);
int cmp_par_i(const void *a, const void *b);
int cmp_par_ip(const void *a, const void *b);
int cmp_par_pi(const void *a, const void *b);
void scan_cf(par_rec *pa,unsigned int npa,par_rec *indx, unsigned int nix,char *IF);
long Get_tr_par(segy *tr,cwp_String *key,cwp_String *type,int *index,unsigned int ipar);

segy tr;
segy tr2;

int
main(int argc, char **argv)
{


	cwp_String dfn;		/* data file name */
	cwp_String hfn;		/* header dump file name */
	cwp_String cfn;		/* cross reference file  */
	
	FILE *dfp;		/* data file pointer */
	FILE *hfp;		/* header file pointer */
	FILE *cfp;		/* header file pointer */
	
	unsigned long ntr;		/* number of traces */
	unsigned int nd;		/* number of dimensions */
	unsigned int nc;		/* number of columns */
	
	par_rec **P;			/* parameter vectors */
        unsigned int *nP;		/* number of unique parameters */
	par_rec  **CF;			/* cross reference table */
	
	long *par_range=NULL;		/* range of parameters for selection */
	
	
	/* Serach index arrays */
	char **IFlag;
        
	/* key handling */
	cwp_String key[SU_NKEYS];  	/* array of keywords                 */
        cwp_String type[SU_NKEYS]; 	/* array of keywords                 */
        int index[SU_NKEYS];    	/* name of type of getparred key        */

	int verbose=1;
	
	
	/* Initialize */
        initargs(argc, argv);
        requestdoc(1);

		
	
	MUSTGETPARSTRING("df",  &dfn);	
	MUSTGETPARSTRING("hf",  &hfn);
	MUSTGETPARSTRING("cf",  &cfn);
	MUSTGETPARINT("nd",  &nd);
	nc=2*nd+1;
	if ((countparval("pr"))!=nd) {
		err("Number of parameter ranges must equal number of parameters!\n");
	} else {
		par_range = (long *) ealloc1(nd,sizeof(long));
		getparint("pr",(int *)par_range);
	}
        
	/* Get "key" values */
        if ((countparval("key"))==nd) {
     		getparstringarray("key",key);

        } else {
                err("Number of Dimenions not equalt number of keys!\n");
        }

        /* get types and indexes corresponding to the keys */
        { int ikey;
		for (ikey=0; ikey<nd; ++ikey) {
                	type[ikey]=hdtype(key[ikey]);
                	index[ikey]=getindex(key[ikey]);
        	}
	}


	/* open file for read */	
	hfp = efopen(hfn,"r");

	/* take a trace count */
	ntr=0;
	{ unsigned int i;
		do{
			ntr++;
			for(i=0;i<nc;i++)
				fscanf(hfp,"%*d");
		} while(!feof(hfp));
	}
	ntr--;
	fprintf(stderr,"%lu\n",ntr);
	
	rewind(hfp);
	
	/* Allocate parameter vectors */
	P = (par_rec**)ealloc2(ntr,nd,sizeof(par_rec));
	nP = (unsigned int*)ealloc1(nd,sizeof(unsigned int));
	CF = (par_rec **)ealloc2(ntr,nd+1,sizeof(par_rec));
	
	/* Allocate index flags */
	IFlag = (char **)ealloc1(nd+1,sizeof(char *));
	{ int id;
		for(id=0;id<nd+1;id++) { 
			IFlag[id] = ealloc1bit(ntr);
			zerobit(IFlag[id],ntr);
			}
	}
	
	/* Read the index and parameter the vectors */
	{ unsigned int i,ip;
		for(i=0;i<ntr;i++) {			
			for(ip=0;ip<nd;ip++)
				fscanf(hfp," %ld %ld",&P[ip][i].i,&P[ip][i].p);
				fscanf(hfp," %*d");
		}
	}
	efclose(hfp);
	fprintf(stderr,"Reading parameters done.\n");
	
	/* read cross reference table */
	cfp = efopen(cfn,"r");
	/* Read the indexes and seq number */
	{ unsigned int i,ip;
		for(i=0;i<ntr;i++) {			
			for(ip=0;ip<nd+1;ip++) {
				fscanf(cfp," %ld",&CF[ip][i].p);
				CF[ip][i].i=i;
			}
		}
	}

	efclose(cfp);
	fprintf(stderr,"Cross reference table reading done.\n");

	/* Sorting */
	/* Sort cross reference table entries */
	{ unsigned int ip;
		for(ip=0;ip<nd+1;ip++) {
			sort_par_v(CF[ip],ntr);	
		}
	}
	
	/* Sorting */
	/* Each parameter vector is sorted with unique value sort
	   tan the results sorted by their parameters */
	{ unsigned int ip;
		for(ip=0;ip<nd;ip++) {
			usort_par_v(P[ip],ntr,&nP[ip]);	
			sort_par_v(P[ip],nP[ip]);	
		}
	}
	fprintf(stderr,"Sorting done.\n");
	
	/* Select parameters accrording to par_range arrays*/
	
	{ unsigned int it,ip,indx_u=0,indx_l=0,itr;
	  long PRM;
	  par_rec upper,lower;
	
		
		/* Open trace data file */
		dfp = efopen(dfn,"r");
		itr=0;
		
		while(fgettra_l(dfp,&tr,(unsigned long)itr)) {
		
			/* First trace is the one that will be filtered */
			tr.mark=1;
			tr.gaps=itr;
			puttr(&tr);
		
			for(ip=0;ip<nd;ip++) {
				
				PRM=Get_tr_par(&tr,key,type,index,ip);
			
				zerobit(IFlag[ip],ntr);
			
				upper.p = PRM+par_range[ip];	
				lower.p = PRM-par_range[ip];
				indx_u=0,indx_l=0;
				/* Find the range of index that fall into
				   par+- par_range interval
			 	  Search for upper and lower values with binary search,
			 	  than extend the range to the first/last element
				*/ 
				
				xindexf(nP[ip],P[ip],&upper,sizeof(par_rec),cmp_par_p,&indx_u);
				fix_upper(ntr,P[ip],sizeof(par_rec),&indx_u);
				
				xindexf(nP[ip],P[ip],&lower,sizeof(par_rec),cmp_par_p,&indx_l);
				fix_lower(ntr,P[ip],sizeof(par_rec),&indx_l);
			
				/* Indexing */
				/* Scan cross reference file for parameter indexes 
			   	and put tags in the IFlag arrays */
			 
				scan_cf(&P[ip][indx_l],indx_u-indx_l,CF[ip],ntr,IFlag[ip]);
			
				
			}
		
			/* Select the common elements of index flags */
			ANDbit(IFlag[0],IFlag[1],IFlag[nd],ntr);
			{ unsigned int  cbit=1;
			  unsigned long iout;
				for(ip=2;ip<nd;ip++) 
					ANDbit(IFlag[nd],IFlag[ip],IFlag[nd],ntr);
					for(it=0;it<ntr;it++) 
						if(readbit(IFlag[nd],it)==1) {
							iout=(unsigned long)CF[nd][it].i;
							if((unsigned int)iout!=itr) {
								fgettra_l(dfp,&tr2,iout);
								tr2.mark=0;
								tr2.gaps=itr;
								puttr(&tr2);
								cbit++;
							}
						}
			}
			if(verbose) fprintf(stderr," %u\n",itr);
			itr++;
		}
	}
	
	fclose(dfp);
	   
	free2((void**)P);
	
	return EXIT_SUCCESS;
}

long Get_tr_par(segy *tr,cwp_String *key,cwp_String *type,int *index,unsigned int ipar)
{

	Value val;
	
	gethval(tr, index[ipar],&val);
	return(vtol(type[ipar],val));
}


void scan_cf(par_rec *pa,unsigned int npa,par_rec *cfpar, unsigned int ncf,char *IF)
{
	unsigned int itr;
	unsigned int ipar;
	par_rec *found;
	unsigned int found_u;
	unsigned int found_l;
	
	for(ipar=0;ipar<npa;ipar++) {
		
		found=bsearch(&pa[ipar],cfpar,ncf,sizeof(par_rec),cmp_par_ip);
		if(found==NULL) err("Error\n");
		found_u = (unsigned int)(((unsigned int)found-(unsigned int)cfpar)/sizeof(par_rec));
		found_l=found_u;
		
		fix_upper(ncf,cfpar,sizeof(par_rec),&found_u);
		fix_lower(ncf,cfpar,sizeof(par_rec),&found_l);
		
		for(itr=found_l;itr<=found_u;itr++) {
				setbit(IF,cfpar[itr].i);
		}
	} 
	
}


void fix_lower(int n, void *a,size_t size,unsigned int *indx)
{
	
	char *b = a+(*indx)*size;
	char *ap=b;
	
	int i = *indx;
	
	while(i>-1 && cmp_par_p(b,ap)==0) {
		ap-=size;
		i--;
	}
	*indx=(unsigned int)i+1;
}

void fix_upper(int n, void *a,size_t size,unsigned int *indx)
{
	
	char *b = a+(*indx)*size;
	char *ap=b;
	int i = *indx;
	
	while(i<n && cmp_par_p(b,ap)==0) {
		ap+=size;
		i++;
	}
	*indx=(unsigned int)i-1;
}

void usort_par_v(par_rec *a, unsigned int n, unsigned int *un)
{

	
	unqsort(a,n,sizeof(par_rec),cmp_par_i,un);	

}

int cmp_par_i(const void *a, const void *b)
{
        par_rec i1 = *((par_rec*) a) ;
        par_rec i2 = *((par_rec*) b) ;
        
        if(i1.i<i2.i) return(-1);
        if(i1.i>i2.i) return(1);
        return(0);
}

void sort_par_v(par_rec *a, unsigned int n)
{

	
	qsort(a,n,sizeof(par_rec),cmp_par_p);	

}

int cmp_par_p(const void *a, const void *b)
{
        par_rec i1 = *((par_rec*) a) ;
        par_rec i2 = *((par_rec*) b) ;
        
        if(i1.p<i2.p) return(-1);
        if(i1.p>i2.p) return(1);
        return(0);
}

int cmp_par_ip(const void *a, const void *b)
{
        par_rec i1 = *((par_rec*) a) ;
        par_rec i2 = *((par_rec*) b) ;
        
        if(i1.i<i2.p) return(-1);
        if(i1.i>i2.p) return(1);
        return(0);
}

int cmp_par_pi(const void *a, const void *b)
{
        par_rec i1 = *((par_rec*) a) ;
        par_rec i2 = *((par_rec*) b) ;
        
        if(i1.p<i2.i) return(-1);
        if(i1.p>i2.i) return(1);
        return(0);
}

