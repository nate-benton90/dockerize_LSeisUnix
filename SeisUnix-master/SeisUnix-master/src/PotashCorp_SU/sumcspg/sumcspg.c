/* sumcspg.c */
/* B.Nemeth */
#include "suhdr.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
 

/* BUFSIZ 8192 */
/* The size of the arrays is N*BUFSIZ */
/* Max Number of cdp gathers scatterpoint N*BUFSIZ */
#define N 1
#define DIAG 0
#define NPR 2

/*********************** self documentation *****************************/
char *sdoc[] = {"Make common scatterpoint gathers				",
" sumcspg [Required parameters] [Optional parameters] >			",
"									   ",
" Required Parameter:						 		",
"									   ",
"	dh	spatial sampling of scatter gather if sqr=0		 ",
"	hm	maximum offset of scatter gather				",
"	v	velocity used to extract scatter gather			",
"									   ",
"	igdir	directory of input cdp gathers				",
"	ogdir	directory of output cspg gathers				",
"	itbf	Ascii file with the names of input cspg description tables  ",
"		(outputs of cspg_prep)					",
"									   ",
" Optional parameter:							",
"									   ",
"	sqr=0   sqr=1 square root sampling of offset, this case dh is ommited",
"	nh	number of offsets in scatter gather;  required if sqr=1	",
"	hs=0	minimum offset of scatter gather				",
"	ext=.hsu   extension of cdpgather files				",
"									   ",
"	ssx=sx   header word of source x coordinate			  ",
"	ssy=sy   header word of source y coordinate			  ",
"	sgx=gx   header word of receiver x coordinate			",
"	sgy=gy   header word of receiver y coordinate			",
"		The values are multiplied by power of scalco		",
"									   ",
"	mts=0	Mute time shift in ms; negative is up			",
"	tmmax=FULL TRACE  maximum time to migrate				",
"									   ",
"	MAXIMUM OFFSET IN THE DATA THAT MAKE SENSE TO KEEP IS hm*2	   ",
"									   ",
NULL};

static void intdiff(int *a,int na,int *b,int nb,
		int *ints,unsigned int *ns,int *difa,unsigned int *nda,int *difb,unsigned int *ndb);
char *read_gthr(cwp_String dir,cwp_String fname,unsigned int *ntr,unsigned int *ns,float *dt,float tmmax,float mts);
static void mk_cspg (segy *tr,float **cspg, unsigned short int **nnz,int *ti,float *he2,
		float dt,float hs,int nh,int ns,int fls,float v,float x,float y,
		cwp_String ssx,cwp_String ssy,cwp_String sgx,cwp_String sgy);
void *p_cdpg(void *td);

segy trout,tr,str;  
char **pcdpn=NULL;	 /* pointer array to the new cdp gathers */
char **pcdpo=NULL;	 /* pointer array to the old cdp gathers */
char **swappcdp;
static int fpf=0;		/* first processing indicator */

/* typedef of data structure for the threads */
struct tdata {
	int		icdpg;
	char 		**pcdpn;
	unsigned int 	*ntrgn;
	float 		**cspg;
	unsigned short int **nnz;
	int 		*ti;
	float 		*he2;
	float 		dt;
	float 		hs;
	int 		nh;
	int		ns;
	float		v;
	float		x;
	float		y;
	cwp_String 	ssx;
	cwp_String	ssy;
	cwp_String	sgx;
	cwp_String	sgy;
	float		*tsize;
	unsigned int	*titr;
	int		skip;
};

/* allocation of data structure for threads */
struct tdata TD[NPR];
pthread_t tid[NPR];	/* array of thread id */
barrier_t bar;		  
int done;
		


int main( int argc, char *argv[] )
{
	
	/* CSPG parameters */
	float hm;	/* max offset of cspg */
	int sqr;	/* flag of sqrt sampling of offset */
	float hs;	/* minimum offset of cspg gathers */
	int nh;	/* number of traces in cspg */
	float dh;	/* spacing of traces in cspg*/
	float v;	/* velocity used to compute cspg */
	unsigned int ns_r;	/* number of samples in input traces returned*/
	int ns=0;		/* number of samples in input traces */
	float dt=0;	/* sample interval */
	float dt_r;	/* sample interval returned */
	unsigned int trsiz=0;	/* Size of 1 trace ns*FSIZE + HDRBYTES */
	unsigned int ntr;	/* number of traces in gather */
	unsigned int tntr;	/* number of traces in gather */
	int cspgn;	/* number of the cspg */
	char cspgnf[BUFSIZ]; /* filename of the cspg */	
	
	/* processing parameters */
	float tmin;	/* trace min time */
	float tmax;	/* trace max time */
	float *offset=NULL;	/* pre-computed eq offset */
	float **cspg=NULL;	/* data matrix of the cspg gather */
	unsigned short int **nnz=NULL;  /* non zero stacked samples */
	float *he2=NULL;	/* squared eq offset */
	int **ti=NULL;	/* transition time indexes */
	float *t=NULL;	/* time of each sample */
	
	/* IO parameters */
	cwp_String igdir;  /* directory of input cdp gathers */
	char cdpn[BUFSIZ];   /* name of input cdp gathers */
	cwp_String ext;	/* extension of input cdp gathers */
	cwp_String ogdir;  /* directory of output cspg */
	cwp_String itbf;   /* name of the file that containes
				the names of cspg description tables */ 
	
	FILE *tnfp;	  /* file pointer of the table name file */
	FILE *tfp;	  /* file pointer of the table name file */
	FILE *dfp;	  /* output datafile pointer */
	
	char tblfn[BUFSIZ]; /* name of the table file */
	struct stat st;	/* descriptor of the file */
	
	float x;	  /* x coordinate of cspg */
	float y;	  /* y coordinate of cspg */
	unsigned int *cdpan;	/* new array of cdp values forming the cspg gather */
			  /* since it is not know how many of them it is 
				declared with a fix size */
	int nn;
	unsigned int *cdpao;	/* old array of cdp values forming the cspg gather */
			  /* since it is not know how many of them it is 
				declared with a fix size */
	int no;
	unsigned int *cdpand;	/* elements that are only in cdpan */
	unsigned int *cdpaod;	/* elements that are only in cdpao */
	unsigned int *cdpac;	/* elements that are common in cdpan and cdpao */
	unsigned int *cdpgindn;	/* index of new cdpgathers, tha values in it are the cdp
				number of the gather, so we know that which gather
				is the first second etc */
	unsigned int *cdpgindo;
	unsigned int *ntrgo;		 /* array with the values of number of traces*/
				 /* in cdpg */ 
	unsigned int *ntrgn;
	unsigned int ndn;			/* number of elements that are only in cdpan */ 
	unsigned int ndo;			/* number of elements that are only in cdpao */
	unsigned int ndc;			/* number of elements that are common */
	unsigned int *swap;
	static int ft=0;		/* first trace flag indicator */
	
	/* Header word for coordinates */
	cwp_String ssx;
	cwp_String ssy;
	cwp_String sgx;
	cwp_String sgy;
	
	/* Mute time */
	float mts=0.0;			/* Mute time shift */
	
	/* max time migrate */
	float tmmax;
	
	/* checks */
	int frp;
	int afp;
	float *tmp;		   /* temporary storage variable */
			
	initargs(argc, argv);
   	requestdoc(1);

	/* These are the parameters required to make the cspg gathers */
	MUSTGETPARFLOAT("hm",&hm);
	MUSTGETPARFLOAT("v",&v);
	if(!getparint ("sqr", &sqr)) sqr=0;
	if(!getparfloat ("hs", &hs)) hs=0.0;
	if(sqr) MUSTGETPARINT("nh",&nh);
	if(!sqr)MUSTGETPARFLOAT("dh",&dh);
	if(!sqr) nh=(hm-hs)/dh-1;
	
	/* IO parameters */
	MUSTGETPARSTRING("igdir", &igdir);
	MUSTGETPARSTRING("ogdir", &ogdir);
	MUSTGETPARSTRING("itbf",   &itbf);
	if(!getparstring ("ext", &ext)) ext=".hsu";
	
	/* coordinates in headers */
	if(!getparstring ("ssx", &ssx)) ssx="sx";
	if(!getparstring ("ssy", &ssy)) ssy="sy";
	if(!getparstring ("sgx", &sgx)) sgx="gx";
	if(!getparstring ("sgy", &sgy)) sgy="gy";
	
	/* Mute time shift */
	if(!getparfloat ("mts", &mts)) mts=0.0;

	if(!getparfloat ("tmmax", &tmmax)) tmmax=99999.9;
	
	/* open the table file */
	tnfp = efopen(itbf,"r");
	
	/* allocate arrays */
	cdpao = (unsigned int *)ealloc1int(N*BUFSIZ);
	cdpan = (unsigned int *)ealloc1int(N*BUFSIZ);
	cdpand = (unsigned int *)ealloc1int(N*BUFSIZ);
	cdpaod = (unsigned int *)ealloc1int(N*BUFSIZ);
	cdpac = (unsigned int *)ealloc1int(N*BUFSIZ);
	cdpgindn = (unsigned int *)ealloc1int(N*BUFSIZ);
	cdpgindo = (unsigned int *)ealloc1int(N*BUFSIZ);
	ntrgo = (unsigned int *)ealloc1int(N*BUFSIZ);
	ntrgn = (unsigned int *)ealloc1int(N*BUFSIZ);
	tmp = ealloc1float(N*BUFSIZ);
		
	pcdpn = ealloc1(N*BUFSIZ,sizeof(char*));
	pcdpo = ealloc1(N*BUFSIZ,sizeof(char*));
	
	/* zero out arrays */
	memset((void *) cdpao,(int) '\0',N*BUFSIZ);
	no=0;
	memset((void *) cdpan,(int) '\0',N*BUFSIZ);
	nn=0;
	
	/* set the index arrays */
/*	memset((void *) cdpgindo,(int) '\-1',N*BUFSIZ);	
	memset((void *) cdpgindn,(int) '\-1',N*BUFSIZ);
*/		
	do {
		/* get the one entry from the table file */
		fscanf(tnfp,"%s",tblfn);

		/* open the table file */
		tfp = efopen(tblfn,"r");
	
		/* get info from the file */
		stat(tblfn,&st);
		
		/* the number of entries in the file equal */
		/* size_of_file/4 -2 ( 2 is the x and y coordinates */
		nn = (int)(st.st_size)/4-2; 

		/* create a gather number from the name of the file */
		cspgn = atoi(strrchr(tblfn,(int)'/')+1);
		
		/* scan the file for data */
		fread(&x,sizeof(float),1,tfp);
		fread(&y,sizeof(float),1,tfp);
		fread(&tmp[0],sizeof(float),nn,tfp);
		{ register int icdp=0;
			for(icdp=0;icdp<nn;icdp++) {	
				cdpan[icdp] = (int)tmp[icdp];
			}
		}
		fclose(tfp);
		intdiff((int *)cdpan,nn,(int *)cdpao,no,(int *)cdpac,&ndc,(int *)cdpand,&ndn,(int *)cdpaod,&ndo);
					
		/* zero the total trace counter */
		tntr=0;
		
		/* keep the cdp gathers that are common to the new and old */
		{ register int ic,indx;
			for(ic=0;ic<ndc;ic++) {
				/* find common cdp numbers in the old index and
				   transfere to the new index */
				indx=0;
				while(cdpgindo[indx]!=cdpac[ic]){
					indx++;
				}
				cdpgindn[ic]=cdpac[ic];
				/* transefere the old pointers to the new ar. */
				pcdpn[ic] = pcdpo[indx];
				ntrgn[ic] = ntrgo[indx];
				ntrgo[indx] = 0;
				pcdpo[indx] = NULL;
				tntr+=ntrgn[ic];
			}
			if(DIAG>0) fprintf(stderr,"Old_transfered\n");
			
			/* free the space of the old cdpg's */
			frp=0;
			for(ic=0;ic<ndo;ic++) {
				indx=0;
				while(cdpgindo[indx]!=cdpaod[ic]){
					indx++;
				}
					if(pcdpo[indx]!=NULL) {
						/* bmfree(pcdpo[indx]); */
						free(pcdpo[indx]);
						pcdpo[indx]=NULL;
					}
					frp++;
			}
					
			if(DIAG>0) fprintf(stderr,"Old_freed\n");

			/* add the new entries after ndc common has added */
			afp=0;
			for(ic=0;ic<ndn;ic++)  {
				cdpgindn[ic+ndc]=cdpand[ic];
				
				/* filename of the new gathers */
				sprintf(cdpn,"%d%s",cdpand[ic],ext);
				if(DIAG>0) fprintf(stderr," %s %d %d\n",cdpn,ic+ndc,tntr);
				
				/* read the gather */ 
				pcdpn[ic+ndc] = read_gthr(igdir,cdpn,&ntr,&ns_r,&dt_r,tmmax,mts);
				ntrgn[ic+ndc]=ntr;
				if(DIAG>1) fprintf(stderr," ;");
				if(ntr && !ft) {
					ns=ns_r;
					dt=dt_r;
					trsiz = ns*FSIZE+HDRBYTES;
					ft=1;
				}
				if(DIAG >1) fprintf(stderr," :");
				if(ntr!=0) {
					tntr+=ntr;
				}
				afp++;
			}
			if(DIAG>0) fprintf(stderr,"New entires added\n");
			
			/* DIAGNOSTICS */
/*			for(ic=0;ic<no;ic++) if(pcdpo[ic]!=NULL) {
				fprintf(stderr,"****** %d\n",pcdpo[ic]);
				}
						
*/		}
		
		/* Processing */		
		/* For the first time arrays has to be allocated */
		/* We need meaningfull dt and ns to do it */
		/* If first time do setup , else skipp all this */
		if(!fpf) {
			if(dt==0) {
				dt=0.002;
				warn("tr.dt is not set dt=0.002 is assumed");
			}	
			tmax= (ns-1)*dt;
			tmin= 0.0;
			offset = ealloc1float(nh+1);	
			cspg = ealloc2float(ns,nh);
			nnz = (unsigned short int **)ealloc2(ns,nh,sizeof(unsigned short int));
			he2 = ealloc1float(nh+1);
			ti = ealloc2int(nh+1,NPR);
			t = ealloc1float(ns);
			/* Compute the offsets for the bins */
			
			{ register int ih;
	  			register float sdh=0;
				if(sqr) sdh = (sqrt(hm)-sqrt(hs))/(nh);
					for(ih=0;ih<=nh;ih++) {
						if(sqr) {
							offset[ih]= SQR(sqrt(hs)+(ih)*sdh);
							he2[ih] = SQR(SQR(sqrt(hs)+(ih)*sdh-sdh/2.0));				
						} else {
							offset[ih] = hs+(ih)*dh;
							he2[ih] = SQR(offset[ih]-dh/2.0);
						}
					}
			}
	
			/* pre compute times */
			{ register int i;
				for(i=0;i<ns;i++) t[i] = i*dt;
			}	

			fprintf(stderr," Sample interval: %f\n",dt);
			fprintf(stderr," Number of samples: %d\n",ns);
			/* set the first processing flag */
			fpf=1;
		}
		
		if(DIAG>1) fprintf(stderr," .\n");
		/* zero out nnz cspg and ti arrays */
		memset( (void *) nnz[0], (int) '\0',ns*nh*sizeof(unsigned short int));
		memset( (void *) cspg[0], (int) '\0',ns*nh*FSIZE);
		/* memset( (void *) ti, (int) -1,nh*ISIZE*NPR); */
			
		{ register int icdpg,ith,error;
			   unsigned int titr=0;
			   float tsize=0.0;	/* Trace pointer */
			   pthread_attr_t attr;

			/* Initialize and set thread detached attribute */
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

			/* Set thread scheduling */
			pthread_attr_setschedpolicy(&attr,SCHED_OTHER); 
			pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);

				
			/* Set the concurrency level for the threads */
/*			fprintf(stderr," %d\n",pthread_getconcurrency());
			pthread_setconcurrency(NPR);
*/				
			/* Create threads */
			bar = barrier_init(NPR+1);
			done= 0;
			for(ith = 0;ith < NPR;ith++) {
				error= pthread_create(&tid[ith],NULL,p_cdpg,&(TD[ith]));
				fprintf(stderr," %d %d\n",error,ith);
			}
			
			/* Loop through the cdp gathers */
			for(icdpg=0;icdpg<nn;icdpg+=NPR) {
				
				
				/* Load data into the data transfere structure */
				for(ith = 0;ith < NPR;ith++) {
					if(icdpg+ith < nn) {
						TD[ith].icdpg = icdpg+ith;
						TD[ith].pcdpn = pcdpn;
						TD[ith].ntrgn = ntrgn;
						TD[ith].cspg  = cspg;
						TD[ith].nnz   = nnz;
						TD[ith].ti	= ti[ith];
						TD[ith].he2   = he2;
						TD[ith].dt	= dt;
						TD[ith].hs	= hs;
						TD[ith].ns	= ns;
						TD[ith].nh	= nh;
						TD[ith].v	= v;
						TD[ith].x	= x;
						TD[ith].y	= y;
						TD[ith].ssx   = ssx;
						TD[ith].ssy   = ssy;
						TD[ith].sgy   = sgy;
						TD[ith].sgx   = sgx;
						TD[ith].tsize = &tsize;
						TD[ith].titr  = &titr;
						TD[ith].skip  = 0;
					} else {
						TD[ith].skip  = 1;
					}
						
				}
				
				/* start the slaves */
					 barrier_wait(bar); 
				 
				/* wait for finish */
					 barrier_wait(bar);

			}
			
			/* set the kill flag */
			done=1;
	
			/* kill the slaves */
			barrier_wait(bar);
			for(ith=0; ith < NPR; ith++) pthread_join(tid[ith],NULL);
	
			/* clean up the mess */
				 barrier_destroy(bar);
			
			fprintf(stderr,"Number of traces used to create m.gather= %d, Size= %6.2f Mb, ",titr,tsize);
		}
		if(DIAG>1) fprintf(stderr,"*\n");

		trout.ns= ns;
		/* write out the migration gather */
		/* create the name of the output gather */
		sprintf(cspgnf,"%s/%d.csg",ogdir,cspgn);
		dfp=efopen(cspgnf,"w");
		{ register int ih;
			for(ih=0;ih<nh;ih++) {
				memset( (void *) &trout, (int) '\0',ns*FSIZE+HDRBYTES);
			
				/* normalize the gather 
				by the number of samples added to */
				{ register int i;
					for(i=0;i<ns;i++)
						if(nnz[ih][i]>=1) trout.data[i]=cspg[ih][i]/nnz[ih][i];
				}
				trout.offset=(int)(offset[ih]);
				trout.ns=ns;
				trout.dt=dt*1000000.0;
				trout.cdpt=nh;
				trout.fldr=cspgn;
				trout.gx=x;
				trout.gy=y;
				trout.cdp=(int)(cspgn/1000);
				trout.ep=cspgn-trout.cdp*1000;
				fputtr(dfp,&trout);
			}
		}
		efclose(dfp);
		
		/* diagnostics */
		fprintf(stderr,"%s\n",tblfn);
		
		/* swap the files so that cdpao is going to be cdpan */		
		swap = cdpao;
		cdpao = cdpan;
		cdpan = swap;
		no = nn;
		
		/* swap the index files */
		swap = cdpgindo;
		cdpgindo = cdpgindn;
		cdpgindn = swap; 
		
		/* swap the number of trace indexes */
		swap = ntrgo;
		ntrgo = ntrgn;
		ntrgn = swap; 
		
		/* swap the cdp gather pointers */
		swappcdp = pcdpo;
		pcdpo = pcdpn;
		pcdpn = swappcdp;
		if(DIAG>0) fprintf(stderr,"Swapped\n");
		
	} while(!feof(tnfp));
	efclose(tnfp);
	
	/* get information from the first header */
/*	if (!gettr(&tr)) err("can't get first trace");
	ns = nt = tr.ns;
	dt = (float)tr.dt/1000000.0;
	fprintf(stderr," %f\n", v);
	
	if(tr.dt==0) {
		dt=0.002;
		warn("tr.dt is not set dt=0.002 is assumed");
	}
*/

	free1float(offset);
	free2float(cspg);
	free2((void *) nnz);
	free1float(he2);
	free1float(tmp);
	free2int(ti);
	free1float(t);
	free1int((int*)cdpaod);
	free1int((int *)cdpand);
	free1int((int *)cdpac);
	free1int((int *)cdpao);
	free1int((int *)cdpan);
	free1int((int *)cdpgindn);
	free1int((int *)cdpgindo);
	free1((int *)pcdpo);
	free1((int *)pcdpn);
	
	
	return EXIT_SUCCESS;
}

void *p_cdpg(void *td)
{
	struct tdata *D;
	segy *strp;
	int trp;
	int itr;
		
		D = td;
		
	for(;;) {
		
		barrier_wait(bar);
		if(done) {
			pthread_exit(NULL);
		}
		
		if(!D->skip) {
			/* Loop through the traces within one gather */
			trp=0;
		
			for(itr=0;itr<D->ntrgn[D->icdpg];itr++) {
				
				/* increment the pointer by the trace size */
				strp=(segy *)(D->pcdpn[D->icdpg]+trp);

			
				/* add the trace to the cspg gather */
				/* the first real sample of the trace
			   	is the first after the mute zone,
			   	beacuse we do not store the muted zone.
			   	This value is stored in str.gain  */
				if(DIAG>2) fprintf(stderr,",");
				mk_cspg (strp,D->cspg,D->nnz,D->ti,D->he2,
					 	D->dt,D->hs,D->nh,D->ns,(*strp).gain,
					 D->v,D->x,D->y,D->ssx,D->ssy,D->sgx,D->sgy);
			
				/* increment the trace pointer to the start of the next trace */
				trp+=HDRBYTES+(*strp).igc*FSIZE;
				*D->titr+=1;
			}
			*D->tsize+=(float)trp/1048576.0;
		}
		
		barrier_wait(bar);
	}
}
	



static void intdiff(int *a,int na,int *b,int nb,
		int *ints,unsigned int *ns,int *difa,unsigned int *nda,int *difb,unsigned int *ndb)
/* find the intersection and diffrence of two integer arrays */
/* a array one, na its dimension */
/* b array two, nb its dimension */
/* ints array of intersection, ns number of intersecting values */
/* difa array of a difference, nda number of differing values */
/* difb array of b difference, ndb number of differing values */
{
	
	int noint=0,is=0;
	
	/* initialize */
	*ns=*nda=*ndb=0;
	
	{ register int ia,ib;
	
		/* find intersect and diff a */
		for(ia=0;ia<na;ia++) {
			ib=0; noint=0;
			while(a[ia]!=b[ib]) {
				ib++;
				if(ib>=nb) { /* no intersect */
					difa[*nda]=a[ia];
					*nda+=1;
					noint=1;
					break;
				}				
			}
			if(noint==0) { /* intersect */
				ints[*ns]=a[ia];
				*ns+=1;
			}
		}
		/* find diff b */
		for(ib=0;ib<nb;ib++) {
			is=0;
			while(ints[is]!=b[ib]) {
				is++;
				if(is>=*ns) { /* no intersect */
					difb[*ndb]=b[ib];
					*ndb+=1;
					break;
				}				
			}
		}				
	}		
}

char *read_gthr(cwp_String dir,cwp_String fname,unsigned int *ntr,unsigned int *ns,float *dt,float tmax,float mts)
/* read a gather and allocate a chunk of memory for the gather */
/* returned is a pointer to the memory */
/* the memory has to be freed from somwhere outside */
{
	char name[BUFSIZ];
	FILE *fp;
	char *data;
	int fls;	/* first live smaple that survived the mute and the mute time shift */
	int nsm;	/* number if samples without the mute zone */
	int tsm;	/* total size of the gather without the mute zone */
	size_t psize;	/* page size */
	
	
	psize = sysconf(_SC_PAGESIZE);
	
	if(DIAG>0) fprintf(stderr,"s_r");
	
	sprintf(name,"%s/%s",dir,fname);
	
	if ((fp = fopen(name,"r")) == NULL) {
		*ntr=0;
		data = NULL;
		if(DIAG>0) fprintf(stderr,"e_r");
		warn("Non existing cdp gather %s\n",name); 
		return(data);
	}
	
	*ntr=0;
	tsm=0;
	fgettr(fp,&tr);	
	if(!fpf) {
		*dt = ((double)tr.dt)/1000000.0;	
		*ns = MIN(NINT(tmax/(*dt)),tr.ns);
	}
	
	do{
		*ntr+=1;
		/* Compute the first live sample for this trace */
		/* muts and mts are in ms */
		fls=NINT(((float)tr.muts+mts)/(*dt*1000.0));
		fls=MIN(MAX(fls,0),*ns-1);
		
		/* Compute the number of samples without the mute zone */
		nsm=*ns-fls;
		
		/* Total size of the gather without the mute zone in samples*/
		tsm+=nsm;
		
	} while(fgettr(fp,&tr));
	rewind(fp);
	
	/* Size of the gather without nute zone , tsm*FSIZE + ntr*HDRBYTES */
	/*data = bmalloc(sizeof(char),tsm*FSIZE+*ntr*HDRBYTES,1); */
	/* allign allocated memory by the page */
	{ unsigned int msize;
		msize=((int)((tsm*FSIZE+*ntr*HDRBYTES)/psize)+1)*psize;
		data = malloc(msize);
		if(data==NULL) 
			err("Error in allocating memory in read module size= %dxFSIZE+%d*HDRBYTES; MSIZE: %d\n",tsm,*ntr,msize);
	}
	if(DIAG>0) fprintf(stderr,"m_r");
	if(DIAG>2) fprintf(stderr," %d %d ",*ntr,tsm);
	{ register int itr,trp;
		/* itr - trace counter; trp-pointer to the start of the trace in memory */
		trp=0;
		for(itr=0;itr<*ntr;itr++) { 
			fgettr(fp,&tr);
			
			/* Compute the first live sample for this trace */
			/* store it in tr.gain */
			fls=NINT(((float)tr.muts+mts)/(*dt*1000.0));
			tr.gain=MIN(MAX(fls,0),*ns-1);
			
			/*Compute the number of samples without the mute zone*/
			/* store it in tr.igc */
			tr.igc=*ns-tr.gain;
			
			/* Diag */
			if(DIAG>1) fprintf(stderr," %d %d %d %d %x\n",*ntr,tr.gain,tr.igc,trp,(int)data);
			
			/* Copy the header first */
			/* bmwrite(data,1,trp,0,HDRBYTES,&tr); */
			memcpy((void *) &data[trp], (const void *) &tr,HDRBYTES);
			if(DIAG>2) fprintf(stderr,"wh_r");
			
			
			/* Copy the trace without the mute zone */
			/* bmwrite(data,1,trp+HDRBYTES,0,tr.igc*FSIZE,&tr.data[tr.gain]); */
			memcpy((void *) &data[trp+HDRBYTES], (const void *) &tr.data[tr.gain],tr.igc*FSIZE);
			
			if(DIAG>2) fprintf(stderr,"wd_r");
				
			/* Update the pointer trp, number of bytes of the first trace */
			trp+=HDRBYTES+tr.igc*FSIZE;
			
		}

	}
	
	fclose(fp);
	if(DIAG>1) fprintf(stderr,"e_r");
	return(data);
	
}

static void mk_cspg (segy *tr,float **cspg, unsigned short int **nnz,int *ti,float *he2,
		float dt,float hs,int nh,int ns,int fls,float v,float x,float y,
		cwp_String ssx,cwp_String ssy,cwp_String sgx,cwp_String sgy)
{
	float gx;	/* group x coordinate */
	float gy;	/* group y coordinate */
	float sx;	/* source x coordinate */
	float sy;	/* source y coordinate */
	float cmpx;	/* cmp x coordinate */
	float cmpy;	/* cmp y coordinate */
	float xa;	/* scp-cmp distance */
	float xa2;	/* .. squared */
	float ha;	/* sour. recev. offs */
	float ha2;	/* .. squared */
	int ni;		/* number of valid eq. off bins */
	int nim;	/* number of valid eq. off bins */
	float t1;	/* constans for eq. off calculations */
	float t2; 
	
	int indx_sx;   /* Coordinate Header word indexes and values */
	Value val_sx;
	cwp_String type_sx;
	int indx_sy;
	Value val_sy;
	cwp_String type_sy;
	int indx_gx;
	Value val_gx;
	cwp_String type_gx;
	int indx_gy;
	Value val_gy;
	cwp_String type_gy;
	
		type_sx = hdtype(ssx);
		indx_sx = getindex(ssx);
		type_sy = hdtype(ssy);
		indx_sy = getindex(ssy);
		type_gx = hdtype(sgx);
		indx_gx = getindex(sgx);
		type_gy = hdtype(sgy);
		indx_gy = getindex(sgy);
 		
		gethval(tr, indx_sx, &val_sx);
		gethval(tr, indx_sy, &val_sy);
		gethval(tr, indx_gx, &val_gx);
		gethval(tr, indx_gy, &val_gy);
		
		gx = vtof(type_gx,val_gx)*pow(10,(*tr).scalco); /* receiver x */
			gy = vtof(type_gy,val_gy)*pow(10,(*tr).scalco); /* receiver y */
			sx = vtof(type_sx,val_sx)*pow(10,(*tr).scalco); /* shot x */
			sy = vtof(type_sy,val_sy)*pow(10,(*tr).scalco); /* shot y */
			cmpx = gx+(sx-gx)/2.0;	/* cmp x computed */
			cmpy = gy+(sy-gy)/2.0;		/* cmp y computed */

			xa = sqrt(SQR(cmpx-x)+SQR(cmpy-y)); /* scp-cmp distance */ 
			ha = sqrt(SQR(gx-cmpx)+SQR(gy-cmpy)); /* sour. recev. offs half dist */
		xa2 = xa*xa;
		ha2 = ha*ha;
		
		ni=nh;				/* number of valid eq. off bins */
		
		/* constans for eq. off calculations */
		/* 2.0*x*h*/
		t1=2.0*xa*ha;		
		/* x^2+h^2 */
		t2=xa2+ha2;
		
		/* if the first eqo is larger than t2 than bail out */
		if(he2[0] >= t2) return;
		
		/* eq. offset bin transition time indexes ti */
		{ register int i;
		 	/* index of first usefull time sample (minimum eq. off.) */
			/* or 2*x/v */
			ti[0]=NINT(MAX((2*xa/v),(t1/(v*sqrt(t2-he2[0]))))/dt);
			i=1;
			
			/* try to compute for ni offsets */
			/* if he2[i] is eq. or larger than t2 */
			/* bail out and set t[i] to n-1 */
			while( (i<=ni) && (he2[i]<t2)) {
				if (xa==0) {
					if(ha==0) ti[i]=0;
					else  ti[i] = NINT((2.0*ha/v)/dt);
				} else if(ha==0) {
					ti[i] = NINT((2.0*xa/v)/dt);
				} else {
					ti[i]= MIN(NINT((t1/(v*sqrt(t2-he2[i])))/dt)
							   ,ns-1);
				}
				i++;
			}
			nim=i-1;
			
			/* If nim < ni we stopped because he2[nim] => t2 */
			/* so set ti[nim] to ns-1. */
			/* The other option is that i=ni but he2[i]<t2 */
			/* then ti[nim=ni] should have the right time. */
			if(nim < ni ) ti[nim]=ns-1;
			
			
			/* put the samples from the trace windows of
			   ti[i+1]-ti[i] into the migration gather
			   at the proper offset */
			{ register int i,it,tit,ip=-1,ic=0,itl=0,flag=0;
			  register float smpl=0.0,smpll=0.0;
				for(i=0;i<nim;i++) { 
					for(it=ti[i];it<ti[i+1];it++) {
						/* We have to honour the mute zone.
						   The first sample of the data is
						   the first live sample after the mute zone.
						   Therefore if it is smaller than fls
						   we do not copy the sample */ 
						   if(it>=fls) {
							tit=it-fls;
							smpl=(*tr).data[tit];
							if(!CLOSETO(smpl,0.0)) {
								flag=1;
								cspg[i][it]+=smpl;
						 		nnz[i][it]++;
								
								/* Interpolation option */
								/* If we jumped offsets because the
								   trace sample intervall is coarser
								   than what we would need to get a sample
								   at every eq offset, go back and interpolate
								   those offset from the last sample (smpll) 
								   of the preavious offset (ip)
								   and the first sample of the current offset */
								
								/*ic==-1 if we are at the first sample of the
								  current offset, but passed the first live offset
								  as ic is declared to be zero */
								if(ic==-1) {
									ic=i;
									
									/* check for jumped offset */
									if(ic!=ip+1) {
									
										/* we want to interpolate */
										/* first live smaple of this offset 
										   is smpl, time of it is it;
										   last sample of the previous
										   offset is smpll, time of it is itl */
										   { int ii,di,iit;
											float dsmpl,dit;
												
											di=ic-ip;
											dit=(it-itl)/di;
											dsmpl=(smpl-smpll)/di;
										   	for(ii=ip+1;ii<ic;ii++) {
												iit=NINT(itl+dit);
												cspg[ii][iit]+=(smpll+ii*dsmpl);
						 						nnz[ii][iit]++;
											}
										   }
									}
								}
						   }
						}
					}
					if(flag) {
						ip=i;
						ic=-1;
						smpll=smpl;
						itl=it;
						flag=0;
					}
				}
			}
		}
} 
