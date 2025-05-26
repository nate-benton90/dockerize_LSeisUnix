/* suresstat_tr_f.c */
/* B.Nemeth */

#include "suhdr.h"
#define BLANK -99999
#define SHT 0
#define RCV 1
#define CDP 2
#define SEQ 3
#define ISGN(x) ( (x) < 0 ? -1 : 1)

/*********************** self documentation *****************************/
char *sdoc[] = {
" SURESSTAT_TR - compute residual static correction                       ",
"                                                                       ",
" suresstat_tr fn=                                                        ",
"                                                                       ",
" Required parameters:                                                  ",
"        fn=            Seismic file                                    ",
"                                                                       ",
" Optional parameters:                                                  ",
"	maxshft=0.008	Maximum allowable time shift/iteration in s     ",
"	it=5		Number of iteration                             ",
"	st=sh.st        Output shot static file                         ",
"	rt=rv.st	Output receiver static file                     ",
"	tx=4*maxshft*it Length of xcorrelation function                 ",
"                                                                       ",
"	maxeshft=maxshft*it	Maximum expected time shift,            ",
"                    		this value is used to padd the traces,  ",
"                      		therefor the larger it is the more      ",
"                     		computation is required to do the       ",
"                       	xcorrelation.                           ",
"       cns=1           0 do not consolidate corrections after each     ",  
"                       iteration                                       ",
"	ffa=1		0 do not do cross correlation with fft          ",
"                                                                       ",
"       sm=0		=1 Filter receiver statics                      ",
"       smw=5		Median filter length (odd number)               ",
"                                                                       ",
"       mcdp=100000     Maximum number of cdp's                         ",
"       msht=4000       Maximum number of shots id                      ",
"       mrcv=4000       Maximum number of receiver id                   ",
"                                                                       ",
"                                                                       ",
"                                                                       ",
"  Note:                                                                ",
"                                                                       ",
"                                                                       ",
"  To window out the most cohherent region of the section you want      ",
"  to use for residual static correction use suwind tmin= tmax= and     ",
"  save the result into a file.                                         ",
"  This will reduce the amount of time the code will spent on scaning   ",
"  the file, since the file is much smaller.                                 ",
"  The results of the static calculation have to be applied to the data ",
"  by suapstat.                                                         ",
"                                                                       ",
"  Header values required:                                              ",
"                         ep -      shot point                          ",
"                         sdepth -  receiver point                      ",
"                         cdp -     CDP location                        ",
NULL};

/* The algorithm used here is based on
	Ronen, Claerbout, 1985. Surface-consistent residual static
	estimation by stack-power maximization. 
	Geophysics, 50, 12: 2759-2767.
   The above is modified to shuffle the shotpoint and receiver point access
   sequence randomly for each iteration. This is believed to result in a more
   stable algorithm.
   
   Author: Balazs Nemeth; Saskatoon, Saskatchewan, CANADA
*/	

typedef struct GStruct{
	int id;		/* gather id */
	int ncdp;	/* number of cdps in the gather */
	int ntr;	/* number of traces in the gather */
	int *cdpn;	/* array(n) of cdp numbers */
	int *cdpsn;	/* position of cdp gathers in the memory */
	int *trn;	/* position of traces with gather id in the memory */
	int corr;	/* static correction value for this gather in samples */
	int **o_corr;	/* pointer arrays for the "other" correction values */
			/* Shot gather pointer to receiver corrections */
	float xcorv;	/* Correlation value; used for diagnostics */	
} Gst;

/* Global variables */
segy tr;		/* SEGY trace */

FILE *fp;		/* File pointer */
void *data;		/* pointer to the data matrix */ 
void *cdpg;		/* pointer to the cdp stacked gathers */
unsigned short **nnz;	/* pointer to stacked smaples for each stacked trace */
int disk;		/* use disk flag */ 
int trsize;		/* size of data trace in bytes */
int maxlag;		/* max shift in samples */
int iter;		/* number of iteration */
int iit;		/* number of iteration counter*/

/* Super traces */
float **sptr1;
float **sptr2;
unsigned short **snnz;  /* samples stacked */
float *sptr1f;		/* pointer to the full trace */
float *sptr2f;		/* pointer to the full trace */
float *nsptr2f;		/* notmalized supertrace of stacked starces of the full trace */
unsigned short *snnzf; /* pointer to the full snnz */
int sptrsl;		/* size of sptr*  sptrsl*nt */
int pnt;		/* size of the padded trace nt+4*(maxlag*it)) */
int zpt;		/* value of the zeroth element of the sptr array */

int **sp1;		/* Diagnostic arrays */
int **sp2;
int idiag;
segy dtr;

/* Internal Functions */
void u_getr(FILE *dp, float **dat,segy *tr, int a, int disk);
void cp2tr(float *tr, float *intr,int ns, unsigned short *nnz);
void m_sptr1(float **sptr,int nt,Gst G);
void m_sptr2(float **sptr,int nt,unsigned short **snnz,Gst G);
void w_sptr2(float **sptr,int nt,unsigned short **snnz,Gst G);
void sa_str(float **sptr1,float **sptr2,unsigned short **snnz,int ntr,int nt,int sign,int corr);
double e_pwr(void *cdpg,unsigned short **nnz,int ncdp,int nt);
void consolidate(Gst *R, int nr, Gst *S, int ns,float dt);

int main( int argc, char *argv[] )
{
	
	char *fn="";		/* name of the file */
	int nt;                 /* number of time samples               */
	int ntx;                /* number of time samples xcorrelation */
        int ntr;                /* number of traces                     */
        float tx;               /* lenght of xcorrelation              */
	float dt=0.002;		/* sample interval */
	int spcount=0;		/* shot point counter */
	int rvcount=0;		/* receiver counter */
	int cdpcount=0;		/* cdp counter */
	
	int mcdp;		/* max number of cdp */
	int mrcv;		/* max number of rcv */
	int msht;		/* max number of shots */
	int mfold=-1;		/* maximum fold */
	int smfold=-1;		/* maximum fold of shot gathers*/
	int rmfold=-1;		/* maximum fold of receiver gathers*/
	
	int *ST;		/* temporary shot array */
	int *RT;		/* temporary receiver array */
	int *CT;		/* temporary cdp array */
	int **tbl;		/* table with cdp,rcv,shot,seqn info */
	
	Gst *Sg;		/* shot gather table structure */
	Gst *Rg;		/* receiver gather table structure */
	int **Cg;		/* CDP gather info */
	
	double power=0.0;	/* stack power */
	double xpower=0.0;	/* cross correlation power */
	float maxshft;		/* max shift in s in every iteration */
	float maxeshft;		/* expected total maximum shift in s */
	int imaxeshft;		/* expected total maximum shift in samples */
	int *ind;		/* array of random access sequence */	
	int ffa=0;		/* flag to do fft xcorrelation */	
	
	float *xtr; 	        /* cross correlation trace */
	int lag; 	        /* lag of max in xcor */
	
	int cns;		/* Consolidate flag */
	
	int sm;			/* smoothing flag */
	int smw;		/* width of gaussian smoothing window */
	int stinc;		/* station increment */
				/* required for smoothing segment selection */
	
	FILE *stf, *rtf;	/* shot end receiver static files */
	char *st="";		/* shot static file name */
	char *rt="";		/* receiver static file name */
	
	initargs(argc, argv);
   	requestdoc(1);
	
	
	
	getparstring("fn",&fn);
	if( !getparfloat("maxshft",&maxshft)) maxshft=0.016;
	if( !getparint("it",&iter)) iter=10;
	if( !getparint("ffa",&ffa)) ffa=1;
	if( !getparstring("st",&st)) st="sh.st";
	if( !getparstring("rt",&rt)) rt="rv.st"; 
	if( !getparint("sm",&sm)) sm=0;
	if( !getparint("smw",&smw)) smw=5;
	if( !getparint("stinc",&stinc)) stinc=1;

	if( !getparint("mcdp",&mcdp)) mcdp=100000;
	if( !getparint("msht",&msht)) msht=4000;
	if( !getparint("nrcv",&mrcv)) mrcv=4000;
	if( !getparint("cns",&cns)) cns=1;
	
	/* Open the file */
	fp = efopen(fn,"r");
	
	/* get the number of traces in data */
	ntr = fgettra(fp, &tr, 0);
	nt = tr.ns;
	dt = tr.dt / 1000000.0;
	maxlag = NINT(maxshft/dt);
	
	if( !getparfloat("tx",&tx)) {
		ntx= 4*iter*maxlag;
	} else {
		ntx=NINT(tx/dt);
	}
	/* make sure it is odd */
	if(ntx%2==0) ntx++;
	
	if( !getparfloat("maxeshft",&maxeshft)) {
		imaxeshft=maxlag*iter;
	} else {
		imaxeshft=NINT(maxeshft/dt);
	}
	
	/* padding = 2*2*maxlag*it since we have both shot end 
	   receiver correction for one trace and it can be >0 or <0*/
	/* padded array size 2*maxlag*it+nt+2*maxlag*it */
	/* This can be overidden by specifying maxeshft ; the maximum expected shift */
	/* this replaces maxlag*it */ 
	pnt = nt+4*imaxeshft;
	
	/* zero element of the array full array*/
	zpt=-2*imaxeshft;
	
	
	/* Allocate temporary arrays */
	ST = ealloc1int(msht);
	RT = ealloc1int(mrcv);
	CT = ealloc1int(mcdp);
	tbl = ealloc2int(4,ntr);
	
	
	/* try to allocate space for the data in memory */
	trsize = nt*FSIZE+HDRBYTES;
	data = bmalloc(sizeof(char),trsize,ntr);
	if (!data) {
		warn(" File is too big %10d storing it on disk\n",trsize*ntr);
	 	disk=1;
	} else {
		disk=0;
	}
	
	/* prepare shot and receiver temporary tabels */
 	{ register int i,flag,p;
		for(i=0;i<msht;i++) ST[i]=BLANK;
		for(i=0;i<mrcv;i++) RT[i]=BLANK;
		for(i=0;i<mcdp;i++) CT[i]=BLANK;
	 
		/* scan the datafile and load info into the table */
		for(i=0;i<ntr;i++) {
			fgettra(fp, &tr, i);
			if(disk==0) bmwrite(data,1,0,i,trsize,&tr);
		
			tbl[i][SHT] = tr.ep;
			p=0; flag=0;
			while( ST[p]!=BLANK) {
				if(ST[p]==tr.ep) flag=1;
				p++;
			}
			if (flag==0) {
				ST[p]=tr.ep;
				spcount++;
			}
		
			tbl[i][RCV] = tr.sdepth;
			p=0; flag=0;
			while( RT[p]!=BLANK) {
				if(RT[p]==tr.sdepth) flag=1;
				p++;
			}
			if (flag==0) {
				RT[p]=tr.sdepth;
				rvcount++;
			}
		
			tbl[i][CDP] = tr.cdp;
			p=0; flag=0;
			while( CT[p]!=BLANK) {
				if(CT[p]==tr.cdp) flag=1;
				p++;
			}
			if (flag==0) {
				CT[p]=tr.cdp;
				cdpcount++;
			}
			
			tbl[i][SEQ] = i;

			if(i%((int)ntr/10)==0) 
				fprintf(stderr,"Number of traces scaned = %10d\n",i);
		}
	}
	
	/* diagnostics */
/*	{ register int itrd;
		segy dtr;
		for(itrd=0;itrd<ntr;itrd++) {
			u_getr(fp,data,&dtr,itrd,disk);
			dtr.dt=4000;
			dtr.ns=nt;
			puttr(&dtr);
		}
	}			
*/	
	fprintf(stderr," Number of traces= %10d\n",ntr);
	fprintf(stderr," Number of shots= %10d\n",spcount);
	fprintf(stderr," Number of receivers= %10d\n",rvcount);
	fprintf(stderr," Number of cdps= %10d\n",cdpcount);

	/* Create CDP gathers */
	/* allocate space for gathers */
	cdpg = bmalloc(sizeof(float),pnt,cdpcount);
	nnz = (void *)ealloc2(pnt,cdpcount,sizeof(unsigned short));
	{ register int itr,it;
		for(itr=0;itr<cdpcount;itr++) 
			for(it=0;it<pnt;it++) nnz[itr][it]=0;
	}
	Cg = ealloc2int(2,cdpcount);
	
	/* create the stack */
	fprintf(stderr,"Creating stack\n");
	{ register int icdp,itr,cdp;
	        float *tmpa;
		tmpa=ealloc1float(pnt);
		 
		for(icdp=0;icdp<cdpcount;icdp++) {
			cdp=CT[icdp];
			Cg[icdp][0]=cdp;
			memset( (void *) tmpa, (int) '\0',pnt*FSIZE);
			for(itr=0;itr<ntr;itr++) {
				if( cdp == tbl[itr][CDP]) {
					u_getr(fp,data,&tr,tbl[itr][SEQ],disk);
					cp2tr(&tmpa[-zpt],tr.data,nt,&nnz[icdp][-zpt]);
					
					/* fold */
					Cg[icdp][1]++;
				}
			}
			bmwrite(cdpg,1,0,icdp,pnt,tmpa);
			
			/* diagnostics */
/*			segy dtr;
			bmread(cdpg,1,0,icdp,nt,tmpa);
			memcpy((void *) dtr.data, (const void *) tmpa, pnt*FSIZE);
			dtr.dt=4000;
			dtr.ns=pnt;
			dtr.cdp=cdp;
			puttr(&dtr);			
*/		}
		free1float(tmpa);
	}			
	
	/* Figure out maximum fold */
	{ register int icdp;
		for(icdp=0;icdp<cdpcount;icdp++) {
			if(Cg[icdp][1] > mfold) mfold=Cg[icdp][1];
		}
	}
	fprintf(stderr," Max. fold= %10d\n",mfold);
	fprintf(stderr,"Done\n");
	
	
	/* Build Shot and Reciver gather tabels */
	fprintf(stderr,"Building Shot and Reciver gather tables\n");
	
	/* allocate tables */
	Sg = ealloc1(spcount,sizeof(Gst));
	Rg = ealloc1(rvcount,sizeof(Gst));
	
	/* Fill the table for shot gathers */
	{ register int is,itr,ncdp,ngtr,i,p,flag;
		for(is=0;is<spcount;is++) {
			Sg[is].id=ST[is];
			Sg[is].corr=0;
			
			/* Count the number of traces belonging to this gather */			
			ngtr=0;
			for(itr=0;itr<ntr;itr++) 
				if(Sg[is].id==tbl[itr][SHT]) ngtr++;
			Sg[is].ntr=ngtr;
			
			/* Allocate pointer array to traces*/
			Sg[is].trn = ealloc1int(Sg[is].ntr);
			
			/* Assume that unique #cdps equal the #of traces */
			/* Later we will check this and reallocate the array */
			Sg[is].cdpn = ealloc1int(Sg[is].ntr);
			for(i=0;i<Sg[is].ntr;i++) Sg[is].cdpn[i]=BLANK; 
			
			/* fill the pointer array so we know where traces are*/
			/* Also count the unique number of CDPs */
			i=0;
			ncdp=0;
			for(itr=0;itr<ntr;itr++)
				if(Sg[is].id==tbl[itr][SHT]) {
					Sg[is].trn[i]=itr;
					i++;
					
					/* Count the cdps */
					p=0; flag=0;
					while( Sg[is].cdpn[p]!=BLANK) {
						if(Sg[is].cdpn[p]==tbl[itr][CDP]) {
							flag=1;
							break;
						}
						p++;
					}
					if (flag==0) {
						Sg[is].cdpn[p]=tbl[itr][CDP];
						ncdp++;
					}
					
				}
			Sg[is].ncdp=ncdp;
			
			/* If there are same cdps # within one gather reallocate 
			   the cdp array */
			Sg[is].cdpsn = ealloc1int(Sg[is].ncdp);
			if(Sg[is].ncdp!=Sg[is].ntr) { 
				free1int(Sg[is].cdpn);
				Sg[is].cdpn=ealloc1int(Sg[is].ncdp);
				for(i=0;i<Sg[is].ncdp;i++) Sg[is].cdpn[i]=BLANK; 
				
				/* Redo the cdpnumbers into the cdp array */
				for(itr=0;itr<ntr;itr++)
					if(Sg[is].id==tbl[itr][SHT]) {
					
						p=0; flag=0;
						while( Sg[is].cdpn[p]!=BLANK && p<Sg[is].ncdp) {
						
							/* If the this cdp is already there break */
							if(Sg[is].cdpn[p]==tbl[itr][CDP]) {
								flag=1;
								break;
							}
							p++;
						}
					
						/* If it is not there append to the end at */
						/* at position p */
						if (flag==0) {
							Sg[is].cdpn[p]=tbl[itr][CDP];
						}
					}
			}
			
			/* Put the seq position of cdp gathers into cdpsn */
			for(itr=0;itr<Sg[is].ncdp;itr++) {
				p=0;
				
				/* Look at Cg cdp table for the cdp number */
				while(Sg[is].cdpn[itr]!=Cg[p][0]) {
					p++;
					if(p==cdpcount) {
						err("Something went wrong!\n");
					}
				}
				
				/* Assign the seq. number to the itr trace position */
				Sg[is].cdpsn[itr]=p;
/*				fprintf(stderr," %d %d %d %d\n",is,itr,Sg[is].cdpsn[itr],Sg[is].cdpn[itr]);
*/			}

			/*generate the xreference table for the receiver 
			 correction values */
			/* One correction for each trace */
			Sg[is].o_corr = ealloc1(Sg[is].ntr,sizeof(int*));
		}
	}
	
	/* Fill the table for Reciver gathers */
	{ register int ir,itr,ncdp,ngtr,i,p,flag;
		for(ir=0;ir<rvcount;ir++) {
			Rg[ir].id=RT[ir];
			Rg[ir].corr=0;
			
			/* Count the number of traces belonging to this gather */			
			ngtr=0;
			for(itr=0;itr<ntr;itr++) 
				if(Rg[ir].id==tbl[itr][RCV]) ngtr++;
			Rg[ir].ntr=ngtr;
			
			/* Allocate pointer array to traces*/
			Rg[ir].trn = ealloc1int(Rg[ir].ntr);
			
			/* Assume that unique #cdps equal the #of traces */
			/* Later we will check this and reallocate the array */
			Rg[ir].cdpn = ealloc1int(Rg[ir].ntr);
			for(i=0;i<Rg[ir].ntr;i++) Rg[ir].cdpn[i]=BLANK; 
			
			/* fill the pointer array so we know where traces are*/
			/* Also count the unique number of CDPs */
			i=0;
			ncdp=0;
			for(itr=0;itr<ntr;itr++)
				if(Rg[ir].id==tbl[itr][RCV]) {
					Rg[ir].trn[i]=itr;
					i++;
					
					/* Count the cdps */
					p=0; flag=0;
					while( Rg[ir].cdpn[p]!=BLANK) {
						if(Rg[ir].cdpn[p]==tbl[itr][CDP]) {
							flag=1;
							break;
						}
						p++;
					}
					if (flag==0) {
						Rg[ir].cdpn[p]=tbl[itr][CDP];
						ncdp++;
					}
					
				}
			Rg[ir].ncdp=ncdp;
			
			/* If there are same cdps # within one gather reallocate 
			   the cdp array */
			Rg[ir].cdpsn = ealloc1int(Rg[ir].ncdp);
			if(Rg[ir].ncdp!=Rg[ir].ntr) { 
				free1int(Rg[ir].cdpn);
				Rg[ir].cdpn=ealloc1int(Rg[ir].ncdp);
				for(i=0;i<Rg[ir].ncdp;i++) Rg[ir].cdpn[i]=BLANK; 
				
				/* Redo the cdpnumbers into the cdp array */
				for(itr=0;itr<ntr;itr++)
					if(Rg[ir].id==tbl[itr][RCV]) {
					
						p=0; flag=0;
						while( Rg[ir].cdpn[p]!=BLANK && p<Rg[ir].ncdp) {
						
							/* If the this cdp is already there break */
							if(Rg[ir].cdpn[p]==tbl[itr][CDP]) {
								flag=1;
								break;
							}
							p++;
						}
					
						/* If it is not there append to the end at */
						/* at position p */
						if (flag==0) {
							Rg[ir].cdpn[p]=tbl[itr][CDP];
						}
					}
			}
			
			/* Put the seq position of cdp gathers into cdpsn */
			for(itr=0;itr<Rg[ir].ncdp;itr++) {
				p=0;
				
				/* Look at Cg cdp table for the cdp number */
				while(Rg[ir].cdpn[itr]!=Cg[p][0]) {
					p++;
					if(p==cdpcount) {
						err("Something went wrong!\n");
					}
				}
				
				/* Assign the seq. number to the itr trace position */
				Rg[ir].cdpsn[itr]=p;
/*				fprintf(stderr," %d %d %d %d\n",is,itr,Rg[ir].cdpsn[itr],Rg[ir].cdpn[itr]);
*/			}
			
			/*generate the xreference table for the shot 
			 correction values */
			/* One correction for each trace */
			Rg[ir].o_corr = ealloc1(Rg[ir].ntr,sizeof(int*));
		}
			
	}
	
	/* make the xreference link for the correction values for Shot*/
	{ register int is,ir,itr,rcv;
		for(is=0;is<spcount;is++) {
			for(itr=0;itr<Sg[is].ntr;itr++) {
				
				/* get the receiver number for this trace */
				rcv = tbl[Sg[is].trn[itr]][RCV];
				
				/* Find this number in the Rg table */
				ir=0;
				while(Rg[ir].id!=rcv) {
					ir++;
					if(ir>rvcount) err("Should never get here\n");
				}
				
				/* make the link */
				Sg[is].o_corr[itr]=&Rg[ir].corr;
			}
		}
	}

	/* make the xreference link for the correction values for Reciver*/
	{ register int is,ir,itr,sh;
		for(ir=0;ir<rvcount;ir++) {
			for(itr=0;itr<Rg[ir].ntr;itr++) {
				
				/* get the shot number for this trace */
				sh = tbl[Rg[ir].trn[itr]][SHT];
				
				/* Find this number in the Sg table */
				is=0;
				while(Sg[is].id!=sh) {
					is++;
					if(is>spcount) err("Should never get here\n");
				}
				
				/* make the link */
				Rg[ir].o_corr[itr]=&Sg[is].corr;
			}
		}
	}
			
	/* Figure out maximum fold for shot and receiver gathers*/
	{ register int is;
		for(is=0;is<spcount;is++) {
			if(Sg[is].ntr > smfold) smfold=Sg[is].ntr;
		}
	}
	{ register int ir;
		for(ir=0;ir<rvcount;ir++) {
			if(Rg[ir].ntr > rmfold) rmfold=Rg[ir].ntr;
		}
	}
	fprintf(stderr," Shot Max. fold= %10d\n",smfold);
	fprintf(stderr," Receiver Max. fold= %10d\n",rmfold);
	fprintf(stderr,"Done\n");

	/* Free tempory arrays */
	free1int(ST);
	free1int(RT);
	free1int(CT);
	free2int(tbl);
	
	/* Checking */
/*	fprintf(stderr,"Checking Shot table <-> data\n");
	{ register int is,itr;
		for(is=0;is<spcount;is++) {
			for(itr=0;itr<Sg[is].ntr;itr++) {
				u_getr(fp, data,&tr,Sg[is].trn[itr],disk);
				if(tr.ep!=Sg[is].id)
						err(" %d %d do not match\n",tr.ep,Sg[is].id);
			}
		}
	}
	
	fprintf(stderr,"Checking Receiver table <-> data\n");
	{ register int ir,itr;
		for(ir=0;ir<rvcount;ir++) {
			for(itr=0;itr<Rg[ir].ntr;itr++) {
				u_getr(fp, data,&tr,Rg[ir].trn[itr],disk);
				if(tr.sdepth!=Rg[ir].id)
						err(" %d %d do not match\n",tr.sdepth,Rg[ir].id);
			}
		}
	}
	
	fprintf(stderr,"Checking Shot table <-> cdpstack \n");
	{ register int is,itr;
		for(is=0;is<spcount;is++) {
			for(itr=0;itr<Sg[is].ncdp;itr++) {
				if(Cg[Sg[is].cdpsn[itr]][0]!=Sg[is].cdpn[itr])
						err(" %d %d do not match\n",Cg[Sg[is].cdpsn[itr]],Sg[is].cdpn[itr]);
			}
		}
	}

	fprintf(stderr,"Checking Receiver table <-> cdpstack \n");
	{ register int ir,itr;
		for(ir=0;ir<rvcount;ir++) {
			for(itr=0;itr<Rg[ir].ncdp;itr++) {
				if(Cg[Rg[ir].cdpsn[itr]][0]!=Rg[ir].cdpn[itr])
						err(" %d %d do not match\n",Rg[Rg[ir].cdpsn[itr]],Rg[ir].cdpn[itr]);
			}
		}
	}
*/
	
	/* Allocate memory */
	/* Supertrace 1 and Supertrace 2*/
	/* There is no point reallocating all the time */
	/* sptr1 and sptr2 depending on the current gather fold */
	/* maximum fold is used */ 

	sptrsl=MAX(smfold,rmfold);
	sptr1 = ealloc2float(pnt,sptrsl);
	sptr2 = ealloc2float(pnt,sptrsl);
	nsptr2f = ealloc1float(pnt*sptrsl);
	snnz = (unsigned short **) ealloc2(pnt,sptrsl,sizeof(unsigned short));
	
	/* reset the pointers to the first element of the unshifted trace */
	{ register int i;
		for(i=0;i<sptrsl;i++) {
			sptr1[i]+=-zpt; 
			sptr2[i]+=-zpt;
			snnz[i]+=-zpt;
		}
	} 
	sptr1f=&sptr1[0][0]+zpt;
	sptr2f=&sptr2[0][0]+zpt;
	snnzf=&snnz[0][0]+zpt;
	
	/* Xcorrelation array */
	xtr = ealloc1float(ntx);

	/* These are for diagnostics */
/*	sp1=ealloc2int(3,sptrsl);
	sp2=ealloc2int(3,sptrsl);
*/
	/* Compute initial stack power */
	power=e_pwr(cdpg,nnz,cdpcount,pnt);	
	fprintf(stderr,"Original power= %10.5e \n\n",power);
	
	/* Start iterations */
	for(iit=0;iit<iter;iit++) {

		
		xpower=0.0;
		
		/* SHOT STATICS */
		
		/* Generate random access sequence for shots */
		ind = ealloc1int(spcount);
		shuffle(ind,spcount);
		
		/* For each shot do */
		{ register int is;
		  float rms1,rms2;
		  
			for(is=0;is<spcount;is++) {
			
			idiag=Sg[ind[is]].id;
				
			/* zero xcorrelation array */
			memset ( (void *) xtr,(int) '\0', ntx*FSIZE); 
			
			/* Create supertrace 1; shot traces */
			m_sptr1(sptr1,nt,Sg[ind[is]]);
			
			/* Create supertrace 2; stacked traces */
			m_sptr2(sptr2,pnt,snnz,Sg[ind[is]]);

			/* diagnostics */
/*			{ register int dit;
				for(dit=0;dit<smfold*pnt;dit++) tr.data[dit]=sptr1f[dit];
			}
			tr.ns=smfold*pnt;
			tr.d1=1;
			tr.ep=Sg[ind[is]].id;
			puttr(&tr);
			{ register int dit;
				for(dit=0;dit<smfold*pnt;dit++) tr.data[dit]=sptr2f[dit];
			}
			tr.ns=smfold*pnt;
			tr.d1=1;
			tr.ep=Sg[ind[is]].id;
			puttr(&tr);

*/			
			/* remove sptr1 from sptr2 */
			sa_str(sptr1,sptr2,snnz,Sg[ind[is]].ncdp,nt,-1,0);
			
			/* Normalize stacked traces before xcorrelation */
			memset ( (void *) nsptr2f,(int) '\0', pnt*sptrsl*FSIZE); 
			
			{ register int ipnt;
			  
			        rms2=0.0;
				rms1=0.0;
				for(ipnt=0;ipnt<Sg[ind[is]].ncdp*pnt;ipnt++) {
					if(snnzf[ipnt]>0)
						nsptr2f[ipnt]=sptr2f[ipnt]/(float)snnzf[ipnt];
					rms2+=SQR(sptr2f[ipnt]);
					rms1+=SQR(sptr1f[ipnt]);
				}
			} 

			/* Cross correlate */
			if(ffa) {
				do_facor(sptr1f,nsptr2f,xtr,Sg[ind[is]].ncdp*pnt,-ntx/2,ntx);
			} else {
				xcor(Sg[ind[is]].ncdp*pnt,0,sptr1f,Sg[ind[is]].ncdp*pnt,0,nsptr2f,ntx,-ntx/2+1,xtr);
			}
				
			/* Xcorrelation power */
			{ int ix;
					for(ix=0;ix<ntx;ix++) xpower+=2.0*fabs((double)xtr[ix]);
			}
			
			/* diagnostics */
/*			{ register int dit;
				for(dit=0;dit<ntx;dit++) tr.data[dit]=xtr[dit];
			}
			tr.ns=ntx;
			tr.d1=1;
			tr.ep=Sg[ind[is]].id;
			puttr(&tr);
*/						
			/* Determine and store correction */
			lag = -isamaxs(ntx,xtr,1)+ntx/2-1;

			/* do not exceed maxshift */
			if(abs(lag) > maxlag) lag= ISGN(lag)*maxlag;

			Sg[ind[is]].corr+=-lag;
			Sg[ind[is]].xcorv = xtr[isamaxs(ntx,xtr,1)]/sqrt(rms1*rms2);
			
			/* If the correction is larger than the maximum expected shift */
			/* reset the correction to that value, so we won`t run out of space */
			/* update the lag accordingly */
			if(abs(Sg[ind[is]].corr) >imaxeshft ) {
				{ int o_corr,o_lag;
					o_lag=lag;
					fprintf(stderr," Total Correction  %d for shot %5d would be larger than abs. max. %d with update %d\n",
			                		Sg[ind[is]].corr+lag,Sg[ind[is]].id,imaxeshft,-o_lag);
					
					/* Remove the most recent correction */
			/*		Sg[ind[is]].corr+=+o_lag;
					o_corr=Sg[ind[is]].corr;
					
					lag=-ISGN(Sg[ind[is]].corr)*(imaxeshft-abs(Sg[ind[is]].corr));
			*/		Sg[ind[is]].corr+=+lag;
					lag=Sg[ind[is]].corr;
					Sg[ind[is]].corr=0;
					fprintf(stderr," Correction update is changed from %5d to %5d; Total correction is %5d\n",
							 -o_lag,-lag,Sg[ind[is]].corr);
				}
			}

			/* Apply correction */
			/* replace relevant part of the stack apply adjustment*/
			/* with the new stack */
			/* Add sptr1 to sptr2 */
			sa_str(sptr1,sptr2,snnz,Sg[ind[is]].ncdp,nt,1,-lag);
			
			/* diagnostics */
/*			{ register int dit;
				for(dit=0;dit<smfold*pnt;dit++) tr.data[dit]=sptr2f[dit];
				tr.ns=smfold*pnt;
				tr.d1=1;
				tr.ep=Sg[ind[is]].id;
				puttr(&tr);
			}
*/			w_sptr2(sptr2,pnt,snnz,Sg[ind[is]]);
 
			}
		}
				
		/* RECEIVER STATICS */
		
		/* Generate random access sequence for receivers */
		ind = ealloc1int(rvcount);
		shuffle(ind,rvcount);
		
		/* For each receiver do */
		{ register int ir;
		  float rms1,rms2;
			for(ir=0;ir<rvcount;ir++) {
			
				/* zero xcorrelation array */
				memset ( (void *) xtr,(int) '\0', ntx*FSIZE); 
			
				/* Create supertrace 1; receiver traces */
				m_sptr1(sptr1,nt,Rg[ind[ir]]);
			
				/* Create supertrace 2; stacked traces */
				m_sptr2(sptr2,pnt,snnz,Rg[ind[ir]]);
				/* diagnostics */
/*				{ register int dit;
				for(dit=0;dit<rmfold*pnt;dit++) tr.data[dit]=sptr1f[dit];
				}
				tr.ns=rmfold*pnt;
				tr.d1=1;
				tr.sdepth=Rg[ind[ir]].id;
				puttr(&tr);
				{ register int dit;
				for(dit=0;dit<rmfold*pnt;dit++) tr.data[dit]=sptr2f[dit]-sptr1f[dit];
				}
				tr.ns=rmfold*pnt;
				tr.d1=1;
				tr.sdepth=Rg[ind[ir]].id;
				puttr(&tr);
*/
				/* diagnostics */
	/*			{register int slt;
					for(slt=0;slt<Rg[ind[ir]].ncdp;slt++) 
						fprintf(stderr," %d %d\n",sp1[slt][2],sp2[slt][2]);
				}
	*/			
			
				/* remove sptr1 from sptr2 */
				if(Rg[ind[ir]].ncdp>1) sa_str(sptr1,sptr2,snnz,Rg[ind[ir]].ncdp,nt,-1,0);		
			
				/* Normalize stacked traces before xcorrelation */
				memset ( (void *) nsptr2f,(int) '\0', pnt*sptrsl*FSIZE); 
				{ register int ipnt;
			  
			        	rms2=0.0;
					rms1=0.0;
					for(ipnt=0;ipnt<Rg[ind[ir]].ncdp*pnt;ipnt++) {
						if(snnzf[ipnt]>0) 
							nsptr2f[ipnt]=sptr2f[ipnt]/(float)snnzf[ipnt];
						rms2+=SQR(sptr2f[ipnt]);
						rms1+=SQR(sptr1f[ipnt]);
					}
				} 
		
				/* Cross correlate */
				if(ffa) {
					do_facor(sptr1f,nsptr2f,xtr,Rg[ind[ir]].ncdp*pnt,-ntx/2,ntx);
				} else {
					xcor(Rg[ind[ir]].ncdp*pnt,0,sptr1f,Rg[ind[ir]].ncdp*pnt,0,nsptr2f,ntx,-ntx/2+1,xtr);
				}
				
				/* Xcorrelation power */
				{ int ix;
					for(ix=0;ix<ntx;ix++) xpower+=2.0*fabs((double)xtr[ix]);
				}
		
				/* diagnostics */
/*				{ register int dit;
					for(dit=0;dit<ntx;dit++) tr.data[dit]=xtr[dit];
					tr.ns=ntx;
					tr.d1=1;
					tr.cdpt=Rg[ind[ir]].ncdp;
					tr.sdepth=Rg[ind[ir]].id;
					puttr(&tr);
				}
			
*/			
				/* Determine and store correction */
				lag = -isamaxs(ntx,xtr,1)+ntx/2-1;

				/* do not exceed maxshift */
				if(abs(lag) > maxlag) lag= ISGN(lag)*maxlag;

				Rg[ind[ir]].corr+=-lag;
				Rg[ind[ir]].xcorv = xtr[isamaxs(ntx,xtr,1)]/sqrt(rms1*rms2);
			
				/* If the correction is larger than the maximum expected shift */
				/* reset the correction to that value, so we won`t run out of space */
				/* update lag */
				if(abs(Rg[ind[ir]].corr) >imaxeshft ) {
					{ int o_corr,o_lag;
						o_lag=lag;
						fprintf(stderr," Total Correction %d for receiver %5d would be larger than abs. max. %d with update %d\n",
			                			Rg[ind[ir]].corr+lag,Rg[ind[ir]].id,imaxeshft,-o_lag);
					
						/* Remove the most recent correction */
					/*	Rg[ind[ir]].corr+=+o_lag;
						o_corr=Rg[ind[ir]].corr;
					
						lag=-ISGN(Rg[ind[ir]].corr)*(imaxeshft-abs(Rg[ind[ir]].corr));
					*/	Rg[ind[ir]].corr+=+lag;
						
						lag=Rg[ind[ir]].corr;
						Rg[ind[ir]].corr=0;
						fprintf(stderr," Correction update is changed from %5d to %5d; Total correction is %5d\n",
							 -o_lag,-lag,Rg[ind[ir]].corr);
					}
				}

				/* Apply correction */
				/* replace relevant part of the stack apply adjustment*/
				/* with the new stack */
				/* Add sptr1 to sptr2 */
				if(Rg[ind[ir]].ncdp>1) sa_str(sptr1,sptr2,snnz,Rg[ind[ir]].ncdp,nt,1,-lag);
				w_sptr2(sptr2,pnt,snnz,Rg[ind[ir]]);
		
			}
		}
		/* Consolidate correction values */
		
		if(cns) consolidate(Rg,rvcount,Sg,spcount,dt);
		
		/* Estimate stack power */
		power=e_pwr(cdpg,nnz,cdpcount,pnt);	
		fprintf(stderr," it= %3d power= %10.5e xpower= %10.5e \n\n",iit,power,xpower);
	
		/* Smooth receiver statics if requested so */
		if (sm) {
			{ float *indx,*val;
			  int ir;
	 
	 			indx=ealloc1float(rvcount);
				val=ealloc1float(rvcount);
				for(ir=0;ir<rvcount;ir++) {
					indx[ir]=(float)Rg[ir].id;
					val[ir]=(float)Rg[ir].corr;
				}
				sm_st(indx,val,rvcount,smw,stinc,5);
				for(ir=0;ir<rvcount;ir++) Rg[ir].corr=NINT(val[ir]);
				free1float(indx);
				free1float(val);
			}
		}
		
		/* WRITE OUT STATICS */
		{ char *fn,*sit;
		
			fn = ealloc1(sizeof(char),MAX(strlen(rt),strlen(st))+10);
			sit = ealloc1(sizeof(char),10);
			sprintf(sit,"-%d",iit);
			
			strcpy(fn,st);
			strcat(fn,sit);
			
			stf = efopen(fn,"w");
			{ register int i=0;
				for(i=0;i<spcount;i++) {
					fprintf(stf," %10d %10.6f %10.6f\n",Sg[i].id,Sg[i].corr*dt,Sg[i].xcorv);
				}
			}
			efclose(stf);
			
			strcpy(fn,rt);
			strcat(fn,sit);
	
			/* Write out receiver static */
			rtf = efopen(fn,"w");
			{ register int i=0;
				for(i=0;i<rvcount;i++) {
					fprintf(rtf," %10d %10.6f %10.6f\n",Rg[i].id,Rg[i].corr*dt,Rg[i].xcorv);
				}
			}
			efclose(rtf);
			
			free1(fn);
			free1(sit);
		}
	} 
	
	return EXIT_SUCCESS;
}

void consolidate(Gst *R, int nr, Gst *S, int ns,float dt)
/* Consolidate the corrections:
   remove the constant shift from the shot and receiver correction.
   This forces the corrections to have near zero mean */
   
{
	float sm=0.0,rm=0.0;
	int i,ism,irm,im;
	
	for(i=0;i<nr;i++) rm+=(float)(R[i].corr);
	for(i=0;i<ns;i++) sm+=(float)(S[i].corr);
	
	fprintf(stderr," Shot c. mean= %f  Rec.c. mean= %f \n",(float)(rm/nr*dt),(float)(sm/ns*dt));
	ism=NINT((float)(sm/ns));
	irm=NINT((float)(rm/nr));
	if(ism*irm <0) {
		im=MIN(abs(ism),abs(irm));
		fprintf(stderr," Removed mean =%f\n",im*dt);
		if(im == abs(ism)) {
			im = ISGN(ism)*-1;
			for(i=0;i<nr;i++) R[i].corr-=im;
			for(i=0;i<ns;i++) S[i].corr+=im;
		} else {
			im = ISGN(irm)*-1;
			for(i=0;i<nr;i++) R[i].corr+=im;
			for(i=0;i<ns;i++) S[i].corr-=im;
		}
	} 
}	

double e_pwr(void *cdpg,unsigned short **nnz,int ncdp,int nt)
{
	float *tmpa;
	double pwr=0.0;
	int tnt;
	
	tmpa=ealloc1float(nt);
	/* zpt is < 0 */
	tnt=nt+2*zpt;
	{ register int icdp,it;
		for(icdp=0;icdp<ncdp;icdp++) {
			bmread(cdpg,1,0,icdp,nt,tmpa);
			for(it=-zpt;it<nt;it++)
				if(nnz[icdp][it]>1) tmpa[it]/=(float)nnz[icdp][it];
			pwr+=(double)(sdot(tnt,&tmpa[-zpt],1,&tmpa[-zpt],1)/tnt);
		}
	}
	free1float(tmpa);
	return(pwr);
}
	
void w_sptr2(float **sptr,int nt,unsigned short **snnz,Gst G)
/* Write the corrected part of the stack stack back to the stacked section 
   Update snnz, showing the fold of the traces */
{
	float *tmp;
	
	tmp=ealloc1float(pnt);
	{ register int icdp;
		for(icdp=0;icdp<G.ncdp;icdp++) {
			memcpy((void *) tmp,(const void *) &sptr[icdp][zpt], pnt*FSIZE);
			bmwrite(cdpg,1,0,G.cdpsn[icdp],pnt,tmp);
			
			memcpy((void *) nnz[G.cdpsn[icdp]], 
			        (const void *) &snnz[icdp][zpt],pnt*sizeof(unsigned short));
			
			/* diagnostics */
	/*		{ 
				memcpy((void *) dtr.data, (const void *) tmp,pnt*FSIZE);
				dtr.ns=pnt;
				dtr.dt=4000;
				dtr.tracf=iit;
				dtr.cdp=G.cdpn[icdp];
				dtr.ep=idiag;
				puttr(&dtr);
			}	
	*/
		}
	}
	free1float(tmp);
}

void sa_str(float **sptr1,float **sptr2,unsigned short **snnz,int ntr,int nt,int sign,int corr)
/* Substract or add two supertraces , and also update the snnz array
   showing the fold of sptr2 */
{
	int itr,it;
	float tmp1,tmp2;
	int tmpi;

	for(itr=0;itr<ntr;itr++) {
		for(it=0;it<nt;it++) {
			tmp2=sptr2[itr][it+corr];
			tmp1=((float)sign)*sptr1[itr][it];
			/* Substarct */
			/* Do not allow to substract when the fold is 1 */
			if(sign==-1) {
				if(!CLOSETO(tmp1,0.0) && snnz[itr][it+corr]>1 ) {
					tmpi=snnz[itr][it+corr]+sign;
					if(tmpi<0) fprintf(stdout," %d %d %d\n",snnz[itr][it+corr],sign,corr);
					snnz[itr][it+corr]+=sign;
					sptr2[itr][it+corr]=(tmp1+tmp2);
				}
			} else {
			/* Add the two traces */
				if(!CLOSETO(tmp1,0.0) ) {
					tmpi=snnz[itr][it+corr]+sign;
					if(tmpi<0) fprintf(stdout," %d %d %d\n",snnz[itr][it+corr],sign,corr);
					snnz[itr][it+corr]+=sign;
					sptr2[itr][it+corr]=(tmp1+tmp2);
			
				}
			} 
		}
	}
}


void m_sptr1(float **sptr,int nt,Gst G)
/* Make the data traces to be correlated with the stacked traces */
{
	static segy a;	
	
	/* zero out supertrace */
	memset( (void *) &sptr[0][zpt], (int) '\0',sptrsl*pnt*FSIZE);
	{ register int itr,it,sl;
		for(itr=0;itr<G.ntr;itr++) {
			/* get the trace from the data */
			u_getr(fp, data,&a,G.trn[itr],disk);
		
			/*find the slot(vertical position) for this trace */
			/* slots are in the same order as cdps in G.cdpn */
			sl=0;
			while(a.cdp!=G.cdpn[sl]) {
				sl++;
			}
			if(!(sl<G.ncdp)) {
				fprintf(stderr," *\n");
				return;
			}

			/* For checking */
		/*	sp1[sl][0]=a.ep;
			sp1[sl][1]=a.sdepth;
			sp1[sl][2]=a.cdp;
		*/	
			/* add trace data to slot */
			/* also apply correction */
			/* We can apply positve and negative shift 
			as the array is padded */
			{ int shift;
				shift=G.corr+*G.o_corr[itr];
				for(it=0;it<nt;it++) sptr[sl][it+shift]=a.data[it];		
			}
			
		}
	}
}

void m_sptr2(float **sptr,int nt,unsigned short **snnz,Gst G)
/* Make the stacked traces to be correlated with the data traces 
   also make the fold super array */
{
	float *tmp;
	
	tmp=ealloc1float(pnt);
	memset( (void *) &sptr[0][zpt], (int) '\0',sptrsl*pnt*FSIZE);
	{ register int icdp;
		for(icdp=0;icdp<G.ncdp;icdp++) {
			/* Diagnostics */
		/*	sp2[icdp][2]=G.cdpn[icdp];
		*/	
			bmread(cdpg,1,0,G.cdpsn[icdp],pnt,tmp);
			memcpy((void *) &sptr[icdp][zpt],
				 (const void *) tmp, pnt*FSIZE);
			memcpy((void *) &snnz[icdp][zpt], 
			        (const void *) nnz[G.cdpsn[icdp]],pnt*sizeof(unsigned short));
		}
	}
	free1float(tmp);
}

void cp2tr(float *tr, float *intr, int ns, unsigned short *annz)
{
	int i;
	
	{ register float tmpitr;
		for(i=0;i<ns;i++){
			tmpitr=intr[i];
			if(!CLOSETO(tmpitr,0.0)) {
				tr[i] += tmpitr;
				annz[i]+=1;
			}
		} 
	}
}

void u_getr(FILE *dp, float **dat,segy *tr, int a, int disk)
/* get  a trace from a disk file or from memory by seq number */
{

	if(disk==1) {
		fgettra(dp,tr,a);
	} else {
		bmread(data,1,0,a,trsize,tr);
	}
} 


