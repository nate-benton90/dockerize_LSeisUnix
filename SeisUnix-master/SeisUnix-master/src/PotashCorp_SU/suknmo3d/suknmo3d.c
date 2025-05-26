/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */

/* NMO: $Revision: 1.21 $ ; $Date: 1998/08/24 20:54:33 $		*/


#include "suhdr.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"									     ",
" KNMO - Kirchoff NMO for an arbitrary velocity function of time and CDP and stack",
"									     ",
" suknmo3d <stdin >stdout [optional parameters]				     ",
"									     ",
" Required parameters:							     ",
" f=			filename of the knmo file (This is a su segy file)   ",
"									     ",
"									     ",
" The seismic traces and velocity traces are matched to each other	     ",
" using the cdp header word in both					     ",
"									     ",
" Optional Parameters:							     ",
" fac=1.0		a factor to multiply velocities			     ",
" key=cdp		header keyword that marks the stackable gathers	",
"									     ",
"									     ",
" This routine applies all the corrections and filters required to do a	     ",
" Kirchoff NMO and stacking of common scatter point gathers.		     ",
"									     ",
" Needs some work!									     ",
"									     ",
"									     ",

NULL};

/* Credits:
 *	SEP: Shuki, Chuck Sword
 *	CWP: Shuki, Jack, Dave Hale, Bjoern Rommel
 *      Modified: 08/08/98 - Carlos E. Theodoro - option for lateral offset
 *
 * Technical Reference:
 *	The Common Depth Point Stack
 *	William A. Schneider
 *	Proc. IEEE, v. 72, n. 10, p. 1238-1254
 *	1984
 *
 * Trace header fields accessed: ns, dt, delrt, offset, cdp, sy
 */
/**************** end self doc *******************************************/

void getvel (int nt, int ncdp, float *cdp, 
	float **ovv,int cdpt, 
	float *ovvt);
void knmo(float *ind, float *outd,int nt, float *vrms, 
          float t0,float offset,float dt,int imt,int sph);
void PhA_filter(float *t,int nt,float dt,int flag,int *first);

segy tr,trv,outtr;

int
main(int argc, char **argv)
{
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	int ncdp;	/* number of cdps specified */
	float *cdp;	/* array[ncdp] of cdps */
	int icdp;	/* index into cdp array */
	int nvnmo;	/* number of vnmos specified */
	float *vnmo;	/* array[nvnmo] of vnmos */
	float *tnmo;	/* array[ntnmo] of tnmos */
	float **ovv;	/* array[ncdp][nt] of sloth (1/velocity^2) functions */
	int *nnz;	/* non zero sample counter */
	float *ovvt;	/* array[nt] of sloth for a particular trace */
 	float v;
	float w;
	
        cwp_String key;         /* header key word from segy.h          */
        cwp_String type;        /* ... its type                         */
        Value val;
        segy **rec;           /* trace header+data matrix */
        int first=0;            /* true when we passed the first gather */
        float dt;
        int nt;
        int ntr;
        int ng=0;
	int firstf=0;
	
	
	
	
	cwp_String f="";	/* su velocity file  */
	FILE *fp;		/* file pointer */
	float fac=1.0;
	
	int verbose;
	
	
	
	/* hook up getpar */
	initargs(argc, argv);
	requestdoc(1);

	
	MUSTGETPARSTRING("f",&f);
	if (!getparfloat("fac",&fac)) fac = 1.0;
        if (!getparstring("key", &key)) key = "cdp";
	if (!getparint("verbose",&verbose)) verbose = 1;
 	
	/* get information from the first header */
        rec = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	ft = (*rec[0]).delrt/1000.0;
	
	
	/* Read cdps */
	if((fp=fopen(f,"r"))==NULL)
                        err("cannot open file=%s\n",f);
	
	ncdp = fgettra(fp,&trv,0);
	nvnmo=trv.ns;

	ovv = ealloc2float(nt,ncdp);
	cdp = ealloc1float(ncdp);
	vnmo = ealloc1float(nvnmo);
	tnmo = ealloc1float(nvnmo);
	nnz = ealloc1int(nt);
	icdp=0;
	
	for(it=0;it<nvnmo;it++)
		tnmo[it]=ft+it*(double)trv.dt/1000000.0;
	
        do {
		{float tn;
			cdp[icdp]=trv.cdp;
			for(it=0; it<nvnmo;it++) 
				vnmo[it] =fac*trv.data[it];
			for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
				intlin(nvnmo,tnmo,vnmo,vnmo[0],vnmo[nvnmo-1],1,&tn,&v);
				ovv[icdp][it] = (double)v;
			}
			icdp++;
		}
	} while(fgettr(fp,&trv));

	fclose(fp);
        free1float(vnmo);
        free1float(tnmo);

	/* allocate workspace */
	ovvt = ealloc1float(nt);

	/* loop over gathers */
	do {
		if(verbose) fprintf(stderr," %d\n",vtoi(type,val));
		
		getvel(nt,ncdp,cdp,ovv,vtoi(type,val),ovvt);
		memset( (void *) nnz, (int) '\0',nt*ISIZE);
		
		{ int itr,it;
		  float *tmp;
		  	
			tmp = ealloc1float(nt);
			
			knmo((*rec[0]).data,tmp,nt,ovvt, 
          			(*rec[0]).delrt/1000.0,(*rec[0]).offset,dt,1,0);
			memcpy((void *) (*rec[0]).data, (const void *) tmp, nt*FSIZE);
			
			for(itr=1;itr<ntr;itr++) {
				knmo((*rec[itr]).data,tmp,nt,ovvt, 
          				(*rec[itr]).delrt/1000.0,(*rec[itr]).offset,dt,1,0);
				
				for(it=0;it<nt;it++) {
					if(tmp[it]!=0.0) {
						nnz[it]+=1;
						(*rec[0]).data[it]+=tmp[it];
					}
				}
			}
			free1float(tmp);
		}
		
		
		PhA_filter((*rec[0]).data,nt,dt,2,&firstf);
		
		for(it=0;it<nt;it++) {
			if(nnz[it]>1) {
				(*rec[0]).data[it]/=nnz[it];
			}
		}
		
		
		/* write output trace */
		puttr(rec[0]);
		
		{ int i;
			for(i=0;i<ntr;i++)
				free1((void *)rec[i]);
		}
		rec = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	} while (rec);

	return EXIT_SUCCESS;
}


/* match cdp in trace to cdp in velocity file */
void getvel (int nt, int ncdp, float *cdp, float **ovv, 
                      int cdpt, float *ovvt)
{
	int indx=0;

	/* if before first cdp, err */
	/*fprintf(stderr," %d\n", cdpt); */
	if (cdpt<(int)cdp[0]) err(" trace CDP # %d is smaller than one in velocity file %d \n",cdpt,(int)cdp[0]);
	
	/* else if beyond last cdp, error*/
	if (cdpt>(int)cdp[ncdp-1]) err(" trace CDP # %d is larger than largets in velocity file %d ",cdpt,(int)cdp[ncdp-1]);
	
	/* else, copy */
	else {
		xindex(ncdp,cdp,cdpt,&indx);
		if((int)(cdp[indx])!=cdpt) err(" CDP # %d not in velocity file exiting \n",cdpt);
		memcpy( (void *) ovvt, (const void *) ovv[indx], nt*FSIZE);
	}
}


void knmo(float *ind, float *outd,int nt, float *vrms, 
          float t0,float offset,float dt,int imt,int sph)
{
/* This routine applies the moveout corection, obliquity factor
	The phase shift filter, spherical divergence factor
	have to be applied to the stacked trace */
/* imt index of first live sample in input trace */
#define TPR 10
	register int i;
	float *txa;	/* amplitude of interpolated tx */
	float of2;
	float t;
	float tm;
	float *ttn;
	float *atn;
	int ssc=1;
		
	
	tm=imt*dt;
	if (offset==0.0) {
		for(i=0;i<nt;i++) {
			t=t0+i*dt;
			outd[i] = ind[i];
		}
		return;
	} else {	
		memset( (void *) outd, (int) '\0',nt*FSIZE);
		ttn=ealloc1float(nt);
		atn=ealloc1float(nt);
		memset( (void *) ttn, (int) '\0',nt*FSIZE);
		
		/* Diffraction shape times */
		of2=4.0*SQR(offset)/(dt*dt);
		for(i=0;i<nt;i++) {
			ttn[i]=sqrt((float)i*(float)i+of2/SQR(vrms[i]));
		}
		
		/* Do not let diffraction xross each other */
		{
			for(i=nt-1;i>1;i--) 
				if(ttn[i-1] > ttn[i]) imt=MAX(i,imt);
		}
		
		/* inverse stretch factor */
                atn[0] = ttn[1]-ttn[0];
		for(i=1;i<nt;i++) 
			atn[i]=ttn[i]-ttn[i-1];
			
		ints8r(nt,1.0,0,ind,0.0,0.0,nt,&ttn[0],&outd[0]);
                
		/* if specified, scale by the stretch factor */
               if (ssc)
                	for (i=imt; i<nt; ++i)
                        	outd[i] *= atn[i];
				
		free1float(atn);
			
	}
		/* Scale by the inverse obliquity factor To/T */ 
		
		for(i=imt;i<nt;i++) {
			txa = &outd[i];
			*txa /=SQR((float)i/ttn[i]);			
		}
		
	/* tapering and zeroing above and delow the data zone */
	{ int si,ei,it,itpr=TPR,itpra;
	
		si=MAX(imt,0);
		ei=MIN(imt+itpr,nt);
		itpra=ei-si+1;
		for(it=0;it<si;it++) outd[it]=0.0;
		for(it=si;it<ei;it++) outd[it]
			*=(float)sin((double)((it-si)*1.57/itpra));
	} 
		free1float(ttn);
	return;
}
		
void PhA_filter(float *t,int nt,float dt,int flag,int *first)
{	
#define TWOPI           2.0 * PI
#define LOOKFAC         2       /* Look ahead factor for npfao    */
#define PFA_MAX         720720  /* Largest allowed nfft           */

	float *rt=NULL;
	complex *ct=NULL,*ctf=NULL;
	int i,nfft=0,nf=0;
	float domega,onfft=1.0;
	int nt_padd=nt+nt/2;
			

		/* Set up for fft */
        	nfft = npfaro(nt_padd,LOOKFAC*nt_padd);
        	if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                	err("Padded nt=%d -- too big", nfft);

        	nf = nfft/2 + 1;
        	onfft = 1.0 / nfft;
        	domega = TWOPI * onfft / dt;
		
		/* Allocate fft arrays */
        	rt   = ealloc1float(nfft);
        	ct   = ealloc1complex(nf);
        	ctf   = ealloc1complex(nf);
		
		memcpy((void *) rt, (const void *) t, nt*FSIZE);
		memset( (void *) &rt[nt], (int) '\0',(nfft-nt)*FSIZE);
		
		if(flag==2) {
			for(i=0;i<nf;i++)
				ctf[i] = csqrt(cmplx(0.0,-i*domega)); 	
		} else {
			for(i=0;i<nf;i++)
				ctf[i] = cmplx(0.0,-i*domega); 	
		}



		pfarc(1,nfft,rt,ct);
	
			for(i=0;i<nf;i++)
				ct[i]=cmul(ct[i],ctf[i]);
	
		pfacr(-1,nfft,ct,rt);
		
		for(i=0;i<nt;i++) {
			t[i]=rt[i]*onfft;
		}
		
		free1float(rt);
		free1complex(ct);
		free1complex(ctf);
		

}

	
		
