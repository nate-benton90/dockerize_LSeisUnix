
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "neural_float.h"

#define MAX(a,b)  (((a) > (b)) ? (a) : (b) )
#define NLAYMAX 20

float *floatmem(int);
void topology(int *, int *);
void netcon(float *, int *, int);
void response(float *, float *, float *, int *, int);
void check(int *, float *, float *, float *, int *, float *, int);
void delta(float *, int, float *, float *, float *, int *, float);

main()
/* Routine to solve the exclusive (xor) problem. */
{

	int numnode[NLAYMAX], nnode=0, nwt=0,i;
	int nlay,itrain,ok,numtrain=1,icnt=0;
	float eta=1., etareq=.9, esum, etamax;

	float *ptrerror;
	float *ptrout; 
	float *ptrdesire;
	float *ptrinput;
	float *ptrwt;
	float *ptrdelwt;

	int *ptrnode = &numnode[0];

	topology(ptrnode,&nlay);

	for(i=0; i<=nlay-1; ++i)
	  nnode += numnode[i];

	printf("\n Number of nodes in net = %d\n",nnode);

	for(i=1; i<=nlay-1; ++i)
	  nwt += numnode[i]*numnode[i-1];

	printf("\n Number of weights = %d\n\n",nwt);
	
	ptrerror  = floatmem(numnode[nlay-1]);
	ptrout    = floatmem(nnode);
	ptrdesire = floatmem(numnode[nlay-1]);
	ptrinput  = floatmem(numnode[0]);
	ptrwt     = floatmem(nwt);
	ptrdelwt  = floatmem(nnode);

	netcon(ptrwt, ptrnode, nlay);

	while(numtrain <= 4)
	{
	eta=.1;
	while(eta < etareq)
	{
	  etamax = 0.0;
	  for(itrain=1; itrain<=numtrain; ++itrain)
	  {
	    switch ( itrain )
	    {
	      case 1:
	        *(ptrinput)=0;
	        *(ptrinput+1)=1;
	        *ptrdesire=1;
	        break;
	      case 2:
	        *ptrinput=1;
	        *(ptrinput+1)=0;
	        *ptrdesire=1;
	        break;
	      case 3:
	        *ptrinput=1;
		*(ptrinput+1)=1;
		*ptrdesire=0;
		break;
	      case 4:
		*ptrinput=0;
		*(ptrinput+1)=0;
		*ptrdesire=0;
	        break;
	    }

	    response(ptrwt, ptrinput, ptrout, ptrnode, nlay);
	    check(&ok, ptrout, ptrdesire, ptrerror, ptrnode, &esum, nlay);

	    if(!ok)
	      delta(ptrwt, nlay, ptrdelwt, ptrerror, ptrout, ptrnode, eta);

	    etamax = MAX(etamax, esum);

	  }

	eta = MAX(1.-etamax, .1);  
	++icnt;

	printf("Presentations = %d Learning rate = %f Max error = %f\r",
	        icnt,eta,etamax); 
	}
	++numtrain;
	}

	printf("\n");
	*(ptrinput)=1;
	*(ptrinput+1)=0;
	response(ptrwt, ptrinput, ptrout, ptrnode, nlay);
	printf("response to 1,0 = %f\n",*(ptrout+nnode-1));		

	*(ptrinput)=0;
	*(ptrinput+1)=1;
	response(ptrwt, ptrinput, ptrout, ptrnode, nlay);
	printf("response to 0,1 = %f\n",*(ptrout+nnode-1));		

	*(ptrinput)=1;
	*(ptrinput+1)=1;
	response(ptrwt, ptrinput, ptrout, ptrnode, nlay);
	printf("response to 1,1 = %f\n",*(ptrout+nnode-1));		

	*(ptrinput)=0;
	*(ptrinput+1)=0;
	response(ptrwt, ptrinput, ptrout, ptrnode, nlay);
	printf("response to 0,0 = %f\n",*(ptrout+nnode-1));		
	
}
