#include "subc.h"
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "su.h"
#include "segy.h"
#include "header.h"

void migs(int nl, int ns, int nofo, float *mig, int ntau,
	FILE *imgfp, int ktrace, int ntl, float dldt, float *trace, 
	float *tras, float dtl, float tminl, int nt, 
	float *ss, float *sl, float *gs, float *gl, 
	int *imutel, float *fovt2, float *s, float *l,
	float *tm, int *iofs, float apers, float aperl, float *fold,
	float *w1, float *work1, float *work2, float *wsave, float *tracef,
	int *ifcut, int *ltaper, float ksmax, float klmax,
	float f0, float df, int nf, float ftaper, int nsave, int nfft,
	float dtau, float tau0, float angmax, 
	int *indxw,int nindxw, int incdxw,
	float *resamp, int ires, int ntres,
	float *s2, float *scs, float *scg, int ncpu, 
	float *vr2, float *vi2, float *vq4, float *tau,
	float *migp, float *foldp) {


	int il, itr, i, ir0, it, ii, ip;
	float tmp;
	int nsl;

	nsl = ns*nl;


/*
	#pragma parallel local(itr,ip,ir0,it,tmp,i,trace,ii,vi2,vq4,s2) 
	#pragma local(work1,work2,indxw,resamp,scs,scg)
	#pragma byvalue(ktrace,nt,ntl,dldt,iofs,ntau,tar,tminl,dtl)
	#pragma byvalue(ss,sl,gs,gl,imutel,fovt2,nsl,s,l,tm,apers,aperl)
	#pragma byvalue(f0,df,nf,ftaper,ifcut,ltaper,nsave,nfft)
	#pragma byvalue(ksmax,klmax,dtau,tau0,angmax,nindxw,incdxw,ncpu)
	#pragma byvalue(ires,ntres,s2)
	#pragma shared(tras,wsave,foldp,migp,fold,mig)
	{
	#pragma pfor iterate (itr=0; ktrace; 1)
	#pragma schedtype(dynamic) chunksize(1)
*/
	for(itr=0;itr<ktrace;itr++) {
		/* linearly interpolate input trace */
		ir0 = itr*nt;
		ip = itr%ncpu;
		bzero(migp+ip*ntau*nsl,ntau*nl*ns);
		bzero(foldp+ip*nsl,nl*ns);
		if(ntl!=nt) {
               		for(it=0;it<ntl;it++) {
                   		tmp = it*dldt;
                       		i = (int)tmp;
                       		tmp = tmp - i;
                   		if(i>=0 && i<nt-1) {
                       			trace[it] = 
						(1.-tmp)*tras[i+ir0]+
						tmp*tras[i+1+ir0];
                       		}
			}	
		} else {
               		for(it=0;it<ntl;it++)
                     		trace[it] = tras[it+ir0];
		}
	
		/* 3d prestack time migration */
		ii = iofs[itr]-1;
		vr22vi2_(vr2+itr*ntau,vi2,&ntau);
		vi22vq4_(vi2,vq4,&ntau);
		vt2s2_(vr2+itr*ntau,vq4,s2,tau,&ntau);
		pstm3d_(trace,&ntl,&tminl,&dtl,&ss[itr],&sl[itr],
			&gs[itr],&gl[itr],&imutel[itr],
			fovt2+itr*ntau,migp+ip*ntau*nsl,&nsl,&ntau,s,l,
			tm,&apers,&aperl,foldp+nsl*ip,w1,
			&f0,&df,&nf,&ftaper,
			ifcut,ltaper,tracef,
			wsave+ip*nsave,&nsave,work1,work2,&nfft,
			&ksmax,&klmax,&dtau,&tau0,&angmax,
			indxw,&nindxw,&incdxw,&ncpu,
			resamp,&ires,&ntres,s2,scs,scg);

/*
		#pragma critical
		{
*/
			for(it=0;it<ntau*nsl;it++) 
				mig[it+ntau*nsl*ii] += migp[it+ip*ntau*nsl];
			for(it=0;it<nsl;it++) 
				fold[it+nsl*ii] += foldp[it+ip*nsl];
/*
		}
*/
	}
/*
	}
*/

}
