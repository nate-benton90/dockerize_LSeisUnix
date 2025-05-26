#include <sys/types.h>
#include "su.h"
#include "segy.h"
#include <signal.h>
#include "header.h"
#include "suhdr.h"
#define NPR 2
#define LOOKFAC 2       /* Look ahead factor for npfao    */
#define PFA_MAX 720720  /* Largest allowed nfft           */

/*********************** self documentation **********************/
char *sdoc[] = {
" 								",
" SUVWGSTAK - 3D volume stack with weights                       ",
" 								",
"     sufvwgstak <stdin >stdout nx= ny= [optional parameters]	",
" 							        ",
" Required parameters:						",
" 	nx=		number of bins in x direction		",
" 	ny=		number of bins in y direction		",
" 			these must be >0, for straight 2D data  ",
" 			ny=1                                    ",
" 							        ",
" Optional parameters: 						",
" 							        ",
" 							        ",
"       Binning parameters:					",
" 	cc=0		cmp coordinates are in trace header word",
" 			gx and gy, or			        ",
" 			cc=1 compute cmp coordinate from word   ",
" 			sx, sy and gx,gy		        ",
" 	xc=1		x coordinate of the corner bin centre   ",
" 	yc=1		y coordinate of the corner bin centre   ",
" 	dbx=20		bin size in x direction		        ",
" 	dby=20		bin size in y direction		        ",
" 	dirx=1		direction of bin numbering in x  dir.   ",
" 			-1=right to left; other=left to right   ",
" 	diry=1		direction of bin numbering in Y dir.    ",
" 			-1=up to down; other= down to up        ",
" 	deg=0		rotation of the bin-grid relative to    ",
" 			coordinate system in degrees	        ",
" 	nnz=0		1 write out the stacking fold volume    ",
" 			into file nnz.stk.		        ",
" 			Only works when sm=0		        ",
" 	w=1		Weight the traces by a weight		",
" 			factor specified in tr.corr	        ",
" 			After stacking the averaged weight      ",
" 			factor is stored in tr.corr	        ",
" 							        ",
" 							        ",
" 	bin_sm=2	Smear the input trace through bin_sm    ",
" 			bin boundaries. The default value       ",
" 			2 means that the trace is smeared       ",
" 			across 2 boundaries in all  directions  ",
" 			relative to the centre bin	        ",
" 							        ",
" 							        ",
NULL};


/* data structure for threads */
struct tdata {
	segy *str;
	int *ns;
	int *cc;
	float *xe;   /* binning */
	float *ye;
	float *degr;
	float *dbx;
	float *dby;
	int *dirx;
	int *diry;
	int *nx;
	int *ny;
	void *data;	/* data matrixes */
	void *nnzim;
	int *w;
	unsigned short int **dead;
	float **weight;
	float *bin_bndr;
};	

/* Internal functions */
void trace_smear(float x, float y, int **bin_sm, float * bin_sm_w, int *n,float br);

/* multipple trace gettr */
int getmtr(segy *tr,int mn,int *rn);
int getmftr(FILE *fp, segy *tr,int mn,int *rn);

/* threads */
void *pr_thr_stk(void *D);


/* data for the threads */
struct tdata TD[NPR];

pthread_t tid[NPR];	/* array of thread id */

barrier_t bar;			
int done;


segy intrace[NPR], outtrace;

int
main(int argc, char **argv)
{
	int verbose;	/* verbose flag				*/
	int i,itr=0;
	int error;
	int nprc;
        char *tmpdir;            /* directory path for tmp files         */
	
	/* trace */
	int ns;
	float dt;
	int trcount=0;
	
	/* data */
	void *data_out;
	void *nnzim;
	unsigned short int *nnzi;
	unsigned short int **dead;
	float **weight;
	float *zero;
	int nr;
	int nnz;		/* flag to output the stacking fold volume */
	
	/* cdp */
	int ncdp;
	
	
	/* binning */
	float xc;
	float yc;
	float dbx;
	float dby;
	float deg;
	float degr;
	int dirx;
	int diry;
	int nx;
	int ny;
	float xe;
	float ye;
	double **cdpcx;
	double **cdpcy;
	int ix;
	int iy;
	int cc;
	
	int w;			/* trace weight factor */
	float bin_sm;

	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

	/* Set parameters */
	if (!getparint   ("verbose", &verbose))	 verbose = 0;

        /* Look for tmpdir */
        MUSTGETPARSTRING("tmpdir",&tmpdir);
        if (!STREQ(tmpdir, "") && access(tmpdir, WRITE_OK))
                err("you can't write in %s (or it doesn't exist)", tmpdir);

	/* binning stuff */
	if(!getparfloat("xc",&xc)) xc=1;
	if(!getparfloat("yc",&yc)) yc=1;
	if(!getparfloat("dbx",&dbx)) dbx=20;
	if(!getparfloat("dby",&dby)) dby=20;
	if(!getparfloat("deg",&deg)) deg=0;
	degr = 3.141592653/180 * deg;
	if(!getparint("nnz",&nnz)) nnz=0;
	if(!getparint("cc",&cc)) cc=0;
	if(!getparint("w",&w)) w=1;
	if(!getparint("dirx",&dirx)) dirx=1;
	if(!getparint("diry",&diry)) diry=1;
	MUSTGETPARINT("nx", &nx);
	MUSTGETPARINT("ny", &ny);
	/* centre of  bin# 1,1 */
	xe = xc;
	ye = yc;
	
	if(!getparfloat("bin_sm",&bin_sm)) bin_sm=2;

	
	/* number of cdps */
	ncdp=nx*ny;
			
	/* get the first trace */
	gettr(&intrace[0]);
	ns=intrace[0].ns;
	/* dt is used only to set output header value d1 */
	if (!getparfloat("dt", &dt))	dt = ((double) intrace[0].dt)/1000000.0;
	if (!dt) {
		dt = .002;
		warn("dt not set, assumed to be .002");
	}
	
	/* allocate some more arrays */
	zero = ealloc1float(ns);
	cdpcx = ealloc2double(ny,nx);
	cdpcy = ealloc2double(ny,nx);
	nnzi = (unsigned short int *)ealloc1(ns,sizeof(unsigned short int));
	dead = (unsigned short int **)ealloc2(ny,nx,sizeof(unsigned short int));
	weight = ealloc2float(ny,nx);
	data_out  = bmalloc(FSIZE,ns,ncdp);
	nnzim = bmalloc(sizeof(unsigned short int),ns,ncdp);
	for(i=0;i<ns;i++) {
		nnzi[i]=0;
		zero[i]=0.0;
	}

	/* compute bin centre coordinates */
	for(ix=1;ix<=nx; ix++) {
		for(iy=1;iy<=ny;iy++) { 
			cdpcx[ix-1][iy-1] = (double)xc + dirx*(ix-1)*dbx*cos(degr) +
                        	diry*(iy-1)*dby*sin(degr);
			cdpcy[ix-1][iy-1] = (double)yc + diry*(iy-1)*dby*cos(degr) -
                        	dirx*(ix-1)*dbx*sin(degr);
			if(verbose==1) fprintf(stderr," %d%d %f %f\n",
				ix,iy,cdpcx[ix-1][iy-1],cdpcy[ix-1][iy-1]);
			}
			/* set the dead flag */
			dead[ix-1][iy-1]=0;
			weight[ix-1][iy-1]=0.0;
	}
	
	
	/* set initial values in stacking arrays */
	for(i=0;i<ncdp;i++) { 
		bmwrite(data_out,1,0,i,ns,zero);
		bmwrite(nnzim,1,0,i,ns,nnzi);
	}
		
	nprc=NPR;
	/* Create threads */
	bar = barrier_init(nprc+1);
	done= 0;
	for(i = 0;i < nprc;i++) {
		error= pthread_create(&tid[i],NULL,pr_thr_stk,&(TD[i]));
		fprintf(stderr," Threads:\n");
		fprintf(stderr," %d %d\n",error,i);
	}
	/* load permanent data to threads */
	for(i = 0;i < NPR;i++) {
		TD[i].str = &intrace[i];
		TD[i].ns = &ns;
		TD[i].cc = &cc;
		TD[i].xe = &xe;
		TD[i].ye = &ye;
		TD[i].degr = &degr;
		TD[i].dbx = &dbx;
		TD[i].dby = &dby;
		TD[i].dirx = &dirx;
		TD[i].diry = &diry;
		TD[i].nx = &nx;
		TD[i].ny = &ny;
		TD[i].data = data_out;
		TD[i].nnzim = nnzim;
		TD[i].w = &w;
		TD[i].dead = dead;
		TD[i].weight = weight;
		TD[i].bin_bndr = &bin_sm;
	}
	
	/* Load the rest of the traces from 1 to NPR-1, so we have NPR traces */
	/* If the number of traces in stdin is smaller than NPR this might bomb out */
	for(i=1;i<NPR;i++) gettr(&intrace[i]);
	trcount=0;
	fprintf(stderr,"Number of traces stacked:");
	fprintf(stderr,"%10d",trcount);
	do{
		/* start the slaves */
		barrier_wait(bar); 
		
		/* wait for finish */
		barrier_wait(bar);
		fprintf(stderr,"\b\b\b\b\b\b\b\b\b\b");
		fprintf(stderr,"%10d",trcount);
		trcount+=NPR;
	} while(getmtr(intrace,NPR,&nr));
	
	/* set the kill flag */
	done=1;
	
	/* kill the slaves */
	barrier_wait(bar);
	for(i=0; i < NPR; i++) pthread_join(tid[i],NULL);
	
	/* clean up the mess */
	barrier_destroy(bar);
	
	/* process the last few nr traces (nr <NPR) */
	/* we only need nr+1 threads */
	bar = barrier_init(nr+1);
	done= 0;
	for(i = 0;i < nr;i++) {
		error= pthread_create(&tid[i],NULL,pr_thr_stk,&(TD[i]));
	}
		
	/* load permanent data to threads */
	for(i = 0;i < nr;i++) {
		TD[i].str = &intrace[i];
		TD[i].ns = &ns;
		TD[i].xe = &xe;
		TD[i].ye = &ye;
		TD[i].degr = &degr;
		TD[i].dbx = &dbx;
		TD[i].dby = &dby;
		TD[i].dirx = &dirx;
		TD[i].diry = &diry;
		TD[i].nx = &nx;
		TD[i].ny = &ny;
		TD[i].data = data_out;
		TD[i].nnzim = nnzim;
		TD[i].w = &w;
		TD[i].dead = dead;
		TD[i].weight = weight;
		TD[i].bin_bndr = &bin_sm;
	}

	/* start the slaves */
	barrier_wait(bar); 
		
	/* wait for finish */
	barrier_wait(bar);

	/* set the kill flag */
	done=1;
	
	/* kill the slaves */
	barrier_wait(bar);
	for(i=0; i < nr; i++) pthread_join(tid[i],NULL);
	
	/* clean up the mess */
	barrier_destroy(bar);
	
	trcount+=nr;
	fprintf(stderr,"\b\b\b\b\b\b\b\b\b\b");
	fprintf(stderr,"%10d\n",trcount);

	
	{ FILE *nfp=NULL;
	  int itnz;
	  unsigned short int nnzm;
		/* Do a strait stack and exit */
		/* write out the result*/
		/* If requested write out the stack fold */
		
		/* Open stack fold file */
		if(nnz) nfp=efopen("nnz.stk","w"); 
	
		for(ix=1;ix<=nx; ix++) {
			for(iy=1;iy<=ny;iy++) { 
				outtrace.nhs = (short)dead[ix-1][iy-1]; 
				itr= (iy-1)*nx+ix-1;
				if(outtrace.nhs>0) {
					bmread(data_out,1,0,itr,ns,outtrace.data);
				/*	bmread(nnzim,1,0,itr,ns,nnzi);
					
					nnzm=0;
					
					for(i=0;i<ns;i++) 
						if(nnzi[i]!=0) {
							outtrace.data[i] /= (float)nnzi[i];
							if(nnzm<nnzi[i]) nnzm=nnzi[i];
						} 
					
					outtrace.corr = NINT(weight[ix-1][iy-1]/(float)nnzm); */
					outtrace.corr = NINT(weight[ix-1][iy-1]);
					outtrace.trid=0;
				} else {
					memcpy((void *) &outtrace.data,(const void*) zero,
						ns*FSIZE);
					outtrace.trid=1;
				} 
				outtrace.ns = ns;  
				outtrace.dt = (int)(dt*1000000.0);  
				outtrace.tracl = i;  
				outtrace.cdp = ix;  
				outtrace.ep = iy; 
				outtrace.cdpt =(int)(ix*1000+iy);  
				outtrace.sx = (int)cdpcx[ix-1][iy-1]*10.0; 
				outtrace.sy = (int)cdpcy[ix-1][iy-1]*10.0; 
				outtrace.gx = (int)cdpcx[ix-1][iy-1]*10.0; 
				outtrace.gy = (int)cdpcy[ix-1][iy-1]*10.0; 
				outtrace.scalco = 1;  
				puttr(&outtrace);
				if(nnz) {
						for(itnz=0;itnz<ns;itnz++) 
							outtrace.data[itnz]=(float)nnzi[itnz];
						fputtr(nfp,&outtrace);
				}
			}
		}
		if(nnz) efclose(nfp);
		return EXIT_SUCCESS;
	}
		
		
}

int getmtr(segy *tr,int mn,int *rn)
/* returns nonzero if it read mn traces, zero if less then mn */
/* the exact number of traces read is given in rn */
{
	int i=0;
	int read;
	
	do {
		read=gettr(&tr[i]);
	/*	{ int it;
			if(tr[i].cdp !=25025) {
				for(it=0;it<tr[i].ns;it++) tr[i].data[it]=0.0;
			}
		}
	*/	i++;
	}while(i<mn && read!=0);
	if(read==0) i--;
	*rn=i;
	return(read);
}

int getmftr(FILE *fp, segy *tr,int mn,int *rn)
/* returns nonzero if it read mn traces, zero if less then mn */
/* the exact number of traces read is given in rn */
{
	int i=0;
	int read;
	
	do {
		read=fgettr(fp,&tr[i]);
		i++;
	}while(i<mn && read!=0);
	if(read==0) i--;
	*rn=i;
	return(read);
}

void *pr_thr_stk(void *td) {

#define MAXNTR 256
	int cdp;
	double gx,gy;
	double sx,sy;
	int binx,biny;
	double binxc,binyc; 
	double x,y;
	int i;
	int itr;
	float smp;
	float smpn;
	float weight;
	unsigned short int nnz;
	struct tdata *D;
	float fbinx,fbiny;
	int nb;
	float *br;
	int **bin_sm;
	float *bin_sm_w;
	int ism;
	
	
	D = td;
	for(;;) {
		barrier_wait(bar);
		if(done==1) {
			pthread_exit(NULL);
		}
		cdp=D->str->cdp;
		if(*D->cc==1) { 
			gx = (double)D->str->gx*pow((double)10,(double)D->str->scalco);
			gy = (double)D->str->gy*pow((double)10,(double)D->str->scalco);
			sx = (double)D->str->sx*pow((double)10,(double)D->str->scalco);
			sy = (double)D->str->sy*pow((double)10,(double)D->str->scalco);
			x = sx+(gx-sx)/2.0;
			y = sy+(gy-sy)/2.0;
		} else {
			x = (double)D->str->gx*pow((double)10,(double)D->str->scalco);
			y = (double)D->str->gy*pow((double)10,(double)D->str->scalco);
		}
	
		br = D->bin_bndr;
	
       		 bin(*D->xe,*D->ye,(float)x,(float)y,*D->degr,*D->dbx,*D->dby,*D->dirx,*D->diry,
                &binx,&biny,&binxc,&binyc,&fbinx,&fbiny);
		
		/*
		fprintf(stderr," %f %f -> %d %d %f %f   %f %f\n",x,y,binx,biny,fbinx,fbiny,binxc,binyc);
		*/
		
		/* allocate array for bin numbers and weights */
		bin_sm = ealloc2int(2,MAXNTR);
		bin_sm_w = ealloc1float(MAXNTR);
		
		/* Compute bin numbers from fbinx and fbiny */
		trace_smear(fbinx,fbiny,bin_sm,bin_sm_w,&nb,*br);
		
		for(ism=0;ism<nb;ism++) {
			binx=bin_sm[ism][0];
			biny=bin_sm[ism][1];
			if((binx >= 1) && (binx <= *D->nx) && 
				(biny >= 1) && (biny <=*D->ny)) {
				D->dead[binx-1][biny-1] +=1;
		
				if(*D->w==1) {
					weight=(float)D->str->corr*bin_sm_w[ism];
				} else { 
					weight=bin_sm_w[ism];
				}
			
				D->weight[binx-1][biny-1]+=weight;
			
				/*fprintf(stderr," %f\n",weight); */
				itr= (biny-1)*(*D->nx)+binx-1;
				for(i=0;i<(*D->ns);i++) {
					smpn = weight*D->str->data[i]; 
					bmread(D->data,1,i,itr,1,&smp);
					smp+=smpn;
					bmwrite(D->data,1,i,itr,1,&smp);
				/*	if(smpn!=0.0) {
						bmread(D->nnzim,1,i,itr,1,&nnz);
						nnz++;
						bmwrite(D->nnzim,1,i,itr,1,&nnz);
					} */
				}
			}
		}
		
		free2int(bin_sm);
		free1float(bin_sm_w);
		
		barrier_wait(bar);
	}

}

void trace_smear(float x, float y, int **bin_sm, float * bin_sm_w, int *n,float br)
/* INPUT
	x, y  floating point bin number such as 38.6
	number of bins to smear data into n
   OUT
        bin_sm array of bins to add the trace tp
	bin_sm_w array of bin weights
*/ 
{
/*#define BINW 2*0.693 */
#define BINW 2*1.2 
	int nx,ny;
	int ix,iy,indx;
	float xs,ys;
	
	float *da;
	int *ia;
	int *xi,*yi;
	int ns;
	int nb;
	double wsum;
	
	
	/* Bin centre in bin coordinate system is at (int)x+.5 (int)y+.5 */
	
	/* find a closest n bins to x and y */
	
	nx=ny=NINT(2*br+4);

	ns = (nx+1)*(ny+1);
	da = ealloc1float(ns);
	ia = ealloc1int(ns);
	xi = ealloc1int(ns);
	yi = ealloc1int(ns);
	
	indx=0;
	for(ix=-nx/2;ix<=nx/2;ix++) {
		for(iy=-ny/2;iy<=ny/2;iy++) {
			xs=(float)((int)x)+0.5+(float)ix;
			ys=(float)((int)y)+0.5+(float)iy;
			xi[indx] = (int)x - ix;
			yi[indx] = (int)y - iy;
			da[indx] = distance(x,y,xs,ys);
			ia[indx] = indx;
			indx++;
		}
		
	}
	
	qkisort(ns,da,ia);
		
	/* Find out the n closest bin numbers */
	nb=0;
	while (da[ia[nb]]<=br && nb<MAXNTR) {
		bin_sm[nb][0]=xi[ia[nb]];
		bin_sm[nb][1]=yi[ia[nb]];
		nb++;
	}
	
	/* compute weighting function */
	/* gaussian function intervall -0.693 - 0.693 corresponds to bin size */
	/* e-0.693 = 0.5 */
	wsum=0.0;
	for(ix=0;ix<nb;ix++) {
		bin_sm_w[ix]=exp(-da[ia[ix]]*BINW);
		wsum+=bin_sm_w[ix];
	}
	for(ix=0;ix<nb;ix++) 
		bin_sm_w[ix]/=(float)wsum;
		
	*n=nb;
	
	free1float(da);
	free1int(ia);
	free1int(xi);
	free1int(yi);
#undef BINW
#undef MAXNTR
}


