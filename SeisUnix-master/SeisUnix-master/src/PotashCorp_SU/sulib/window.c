#include <stdio.h>
#include "su.h"

void c_window(int ntr,int nw,int no,float w[],float ww[],int *rnp,int ws[],int we[])
/* Given ntr numbers, nw window length, no overlap
  the routine optimally select the nw window traces out of
  ntr number with no overlap.
  The routine returns the number of windows in rnp
  rnp window start and end values in ws and we, respectively.
  The ntr trace weights are returned in w.
  w has to be declared outside to the size of ntr.
  ws and we have to be at least npr in size 
  The window weights are returned in ww. ww has to be declared
  outside to nw size.  
*/
{
static int iminarg1,iminarg2;
#define IWMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))

	int np;		/* number of passes */
	int sw,ew;      /* start and end of spatial window */
	float dw;
	
	memset( (void *) w, (int) '\0', ntr*sizeof(float));
	memset( (void *) ww, (int) '\0', nw*sizeof(float));
		
	{ register int i;		
		ew=nw-1;
		sw=0;
		np=1;
		for(i=sw;i<=ew;i++) ww[i]=(float)nw;
		ww[0]=1.0; ww[nw-1]=1.0;
		dw=(float)(nw-1)/(float)(no-1);
		for(i=1;i<no;i++) ww[i]=1.0+dw*i;
		for(i=1;i<no;i++) ww[nw-1-i]=ww[i];
		for(i=sw;i<=ew;i++) w[i]+=ww[i-sw];
				
		ws[np-1]=sw; we[np-1]=ew;
		while(ew<ntr-1) {
			ew+=nw-no;
			ew=IWMIN(ew,ntr-1);
			sw=ew-nw+1;
			np++;
			for(i=sw;i<=ew;i++) w[i]+=ww[i-sw];
			ws[np-1]=sw; we[np-1]=ew;
		}
	}
	*rnp=np;
}

void hanning_w(int n,float *w)
/*
	returns an n element long hanning window 
	
	w[k] = 0.5(1-cos(2PI * K/n+1)) k=1,....n
	
*/
{
	int i;
	float PI2=2.0*PI;
	
	for(i=0;i<n;i++) 
		w[i] = 0.5*(1-cos(PI2*(i+1)/(n+1)));		
}
