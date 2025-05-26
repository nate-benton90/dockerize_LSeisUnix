/* sugprscl.c */
/* B.Nemeth */



#include "suhdr.h"
/*********************** self documentation *****************************/
char *sdoc[] = {"sugprscl < radar traces >outfile                     	 ",
"SU program to scale gpr data                          			 ",
" 									 ",
" 									 ",
" 	type=0		A*x^b - power function scaling			 ",
" 	     1		A*exp(bx) - exponential function scaling	 ",
" 									 ",
" 	nx=51		trace window length				 ",
" 									 ",
NULL};
   
/* Segy data constans */
segy 	tr;				/* SEGY trace */

void scale_tr(float *data,float *a,int n,int type);

int main( int argc, char *argv[] )
{
 

	int nx;
	int nt;
	int type;
	
	float *stacked;
	int *nnz;
	int itr=0;
	
 
	initargs(argc, argv);
   	requestdoc(1);

	if (!getparint("type", &type)) type = 0;
	if (!getparint("nx", &nx)) nx = 51;
	if( !ISODD(nx) ) {
		nx++;
		warn(" nx has been changed to %d to be odd.\n",nx);
	}
	
	
	/* Get info from first trace */ 
	if (!gettr(&tr))  err("can't get first trace");
	nt = tr.ns;
	
	stacked = ealloc1float(nt);
	nnz = ealloc1int(nt);
	memset((void *) nnz, (int) '\0', nt*ISIZE);
	memset((void *) stacked, (int) '\0', nt*FSIZE);

	/* read nx traces and stack them */
	/* The first trace is already read */
	
	{ int i,it;
	  float **tr_b;
	  char  **hdr_b;
	  int NXP2=nx/2;
	  
		/* ramp on read the first nx traces and create stack */
		
	  	tr_b = ealloc2float(nt,nx);
		hdr_b = (char**)ealloc2(HDRBYTES,nx,sizeof(char));
		
		memcpy((void *) hdr_b[0], (const void *) &tr, HDRBYTES);
		memcpy((void *) tr_b[0], (const void *) &tr.data, nt*FSIZE);
		
		for(i=1;i<nx;i++) {
			gettr(&tr);
			memcpy((void *) hdr_b[i], (const void *) &tr, HDRBYTES);
			memcpy((void *) tr_b[i], (const void *) &tr.data, nt*FSIZE);
		}
		
		for(i=0;i<nx;i++) 
			for(it=0;it<nt;it++) 
				stacked[it] += tr_b[i][it];
		
		
		for(it=0;it<nt;it++)
			stacked[it] /=(float)nx;
		
			
		/* scale and write out the first nx/2 +1 traces */
		for(i=0;i<NXP2+1;i++) {
			memcpy((void *) &tr, (const void *) hdr_b[i], HDRBYTES);
			memcpy((void *) tr.data, (const void *) tr_b[i], nt*FSIZE);
			
			scale_tr(tr.data,stacked,nt,type);

			puttr(&tr);
			itr++;
		}
		
		/* do the rest of the traces */
		gettr(&tr);
		
		do {
			
			/* Update the stacked trace  - remove old */
			for(it=0;it<nt;it++) 
				stacked[it] -= tr_b[0][it]/(float)nx;
				
			/* Bump up the storage arrays */
			/* This is not very efficient , but good enough */
			{int ib;
				for(ib=1;ib<nx;ib++) {
					memcpy((void *) hdr_b[ib-1], (const void *) hdr_b[ib], HDRBYTES);
					memcpy((void *) tr_b[ib-1], (const void *) tr_b[ib], nt*FSIZE);
				}
			}
			
			/* Store the new trace */
			memcpy((void *) hdr_b[nx-1], (const void *) &tr, HDRBYTES);
			memcpy((void *) tr_b[nx-1], (const void *) &tr.data, nt*FSIZE);
			
			/* Update the stacked array  - add new */
			for(it=0;it<nt;it++) 
				stacked[it] += tr_b[nx-1][it]/(float)nx;
			
			/* Filter and write out the middle one NXP2+1 */
			memcpy((void *) &tr, (const void *) hdr_b[NXP2], HDRBYTES);
			memcpy((void *) tr.data, (const void *) tr_b[NXP2], nt*FSIZE);
			
			scale_tr(tr.data,stacked,nt,type);
			
			puttr(&tr);
			itr++;
			
			
		} while(gettr(&tr));

		/* Ramp out - write ot the rest of the traces */
		/* filter and write out the last nx/2 traces */
		for(i=NXP2+1;i<nx;i++) {
			memcpy((void *) &tr, (const void *) hdr_b[i], HDRBYTES);
			memcpy((void *) tr.data, (const void *) tr_b[i], nt*FSIZE);
			
			scale_tr(tr.data,stacked,nt,type);
			
			puttr(&tr);
			itr++;
		
		}
		
		
	}
		
  
	
	free1float(stacked);
	free1int(nnz);
   	return EXIT_SUCCESS;
}
void scale_tr(float *data,float *a,int n,int type)
/*

	type 0
	Find scalers for the power function y=a*x^b
	
	this can be linearized to ln(Y) = Ln(a) +b*ln(x) and solved
	to file a line to ln(x) and ln(y)

	type 1
	Find scalers for the exp function y=a*exp(x^b)
	
	this can be linearized to ln(Y) = Ln(a) +b and solved
	to file a line to x and ln(y)

*/



{
	float **X;
	float *y;
	float *res;
        
	float a_and_b[2];
        int jpvt[2];
        float qraux[2];
        float work[4];
        int k;
        
	int it;                                                                                      
	
	X = ealloc2float(n,2);
	y=ealloc1float(n);
	res=ealloc1float(n);	

	if(type==0) {
		for(it=0;it<n;it++) {
			y[it] = (float)log(fabs(a[it]));
			X[1][it] = (float)log((double)(it+1));
			X[0][it] = 1.0;
		}
	} else {
		for(it=0;it<n;it++) {
			y[it] = (float)log(fabs(a[it]));
			X[1][it] = (float)it;
			X[0][it] = 1.0;
		}
	} 
	
	
	sqrst(X, n, 2,y,0.0,&a_and_b[0],res,&k,&jpvt[0],&qraux[0],&work[0]);
	
	if(type==0) {
		for(it=0;it<n;it++) {
			data[it] /=a_and_b[0]*powf((float)it+1,a_and_b[1]);
		}
	} else {
		for(it=0;it<n;it++) {
			data[it] /=a_and_b[0]*exp((float)it*a_and_b[1]);
		}
	}
	
	
	free1float(y);
	free1float(res);
}
