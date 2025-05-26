# include <math.h>
# include <stdlib.h>
# include <stdio.h>

# define MAX(a,b)  (((a) > (b)) ? (a) : (b) )
# define NLAYMAX 20

/* Header file for neural network problems.  This file
contains all the routines required to do back propagation.
A maximum of NLAYMAX layers are allowed, and all variables
are floating point.  For a version in which the variables
are all double see neural_double.h */

void check(int *ptrok, float *ptrout, float *ptrdesire, 
	   float *ptrerror, int *ptrnode, float *ptresum,
	   int nlay)
/*
	ptrout			pointer to output vector 
	ptrdesire		pointer to desired vector 
	ptrerror		pointer to error vector 
	ptrnode			pointer to node vector 
	ptrok			pointer to ok flag for 
				convergence check 
	ptresum			pointer to error 	
	nlay			number of layers in net 

   Routine to compute the error between the actual
   and desired solution.  Actual solution resides
   in out[nlay][i].  Desired solution is in desire[i].
   Error = (desired-actual)*derivative of sigmoid. 
*/

{
	double fabs(double);

	typedef int Loop;

	int nnode,ok,ibias2;
	static int ibias=0;
	Loop i;
	float *ptrtmp1, *ptrend;
	float eps;
	register float ee;


	eps = .1;					/* error tolerance */
	*ptresum = 0.0;					/* initialize the error */
	nnode = *(ptrnode + nlay-1);			/* number of nodes in last layer */
	
	if(ibias == 0)					/* For first time through */
	  {
	  for(i=0; i<=nlay-2; ++i)			/* find number of first node */
	  ibias += *(ptrnode+i);			/* in last layer */
	  }

	*ptresum = 0.0;					/* initialize the error. */
	ibias2 = ibias;
	ptrtmp1 = ptrout + ibias;
	ptrend  = ptrerror + nnode-1;
	
	for( ; ptrerror <= ptrend; ++ptrerror, 
	                          ++ptrdesire,		/* Compute the error between desired */
				  ++ptrtmp1)		/* and actual. */
	{						
	  ee = *ptrtmp1;
	  *ptrerror = (*ptrdesire-ee) 
			    *(1-ee)*ee;			/* error to back propagate from node i */
	  *ptresum = MAX(*ptresum,
		    (float) fabs((double)(ee-*ptrdesire)));
	}

	ok = 1;						/* set completion flag */
	if(*ptresum > eps)ok = 0;

	*ptrok = ok;
}

void delta(float *ptrwt, int nlay, float *ptrdelwt, 
           float *ptrerror, float *ptrout, int *ptrnode, 
	   float eta)
/*
	ptrwt					   pointer to wt vector 
	ptrdelwt   				   pointer to wt perturbations 
	ptrerror				   pointer to error vector. 
	ptrout					   pointer to output vector 
	ptrnode					   pointer to node vector 

	nlay					   number of layers in net 
	eta					   momentum term 

   Routine to update the weights using the delta rule.
   The network is assumed to be fully connected and
   feedforward. 
*/

{
	typedef int Loop;

	Loop i,ilay,inode,jnode;			   /* loop indices */
	int nnode;					   /* number of nodes in a layer */
	int ibias,ibiaswt;				   /* array offsets */
	int nodebelow,nodeabove;			   /* nodes above/below a layer */
	int iwt;					   /* temporary variables */
	static int firstnode[NLAYMAX];			   /* number of first node in each layer */
	static int firstwt[NLAYMAX];			   /* number of first wt in each layer. */
	static int ientry=0;				   /* entry flag */
	register float *ptrtmp, *ptrtmp2, *ptrtmp3; 	   /* temporary pointers */
	float *ptrend, *ptrend2;			   /* "                " */
	double ee;					   /* error */
	register float tmp;				   /* temporary variable */

	if(ientry==0)					   /* on first entry compute */
	{						   /* layer info arrays */
	  firstnode[0] = 0;  
	  firstwt[0] = 0; 
	  firstwt[1] = 0;

	  for(i=0; i<=nlay-1; ++i)			   /* compute first node in each layer */
	    firstnode[i+1] = firstnode[i] + *(ptrnode + i);

	  for(i=2; i<= nlay-1; ++i)			   /* compute first wt in each layer */
	    firstwt[i] = firstwt[i-1] + *(ptrnode+i-2)**(ptrnode+i-1);
	}
	ientry = 1;

	nnode = *(ptrnode + nlay-1);			   /* number of nodes in last layer */
	ptrtmp = ptrdelwt + firstnode[nlay-1];		   /* error from first node of last layer */
	ptrtmp2 = ptrerror;				   /* error vector */
	ptrend = ptrtmp + nnode-1;			

	for(; ptrtmp <= ptrend; ++ptrtmp, ++ptrtmp2)       /* set delwt = error vector for last layer */
	  *ptrtmp = *ptrtmp2;

 /* First find the error from each node to back propagate */
	
	if(nlay != 2)
	{
	for(ilay=nlay-2; ilay>=1; --ilay)		   /* loop over each layer */
	{
	  nnode = *(ptrnode + ilay);			   /* number of nodes in layer ilay */
	  ibias = firstnode[ilay];			   /* first node in layer ilay */
	  nodebelow = *(ptrnode + ilay + 1)-1;		   /* number of nodes -1 below layer ilay */
	  iwt   = firstwt[ilay+1];			   /* first wt in layer ilay */
	  ptrtmp = ptrdelwt + firstnode[ilay+1];	   /* error in from first node below */

	  for(inode=0; inode<= nnode-1; ++inode)  	   /* loop over all nodes in current layer */
	  {
	    ibiaswt   = iwt + inode;			   /* offset of first wt in layer below */
	    tmp       = 0.;				   /* initialize accumulator */

	    for(jnode=0; jnode<=nodebelow; ++jnode)	   /* loop over all nodes in layer below */
	    {
	      tmp += *(ptrwt+ibiaswt)**(ptrtmp+jnode);	  /* form the dot product  */
	      ibiaswt += nnode;
	    }
	    ee = *(ptrout + (inode+ibias));		   /* error */     
	    *(ptrdelwt + (inode+ibias)) = tmp*(1.-ee)*ee;  /* error output to back prop */
	  }
	}
	}

/* Now update the weights */

	for(ilay=1; ilay<=nlay-1; ++ilay)		   /* Loop over each layer */
	{
	  nnode = *(ptrnode + ilay);			   /* number of nodes in layer ilay */
	  ptrtmp3 = ptrwt + firstwt[ilay];    		   /* weight vector */
	  nodeabove = *(ptrnode + ilay-1);		   /* number of nodes in layer above */
	  ptrtmp = ptrdelwt + firstnode[ilay];		   /* back propagated error */
	  ptrend = ptrtmp + nnode-1;	

	  for(; ptrtmp<=ptrend; ++ptrtmp)		   /* loop over all nodes */
	  {
	    tmp = *ptrtmp*eta;				   /* wt perturbation * momentum */
	    ptrtmp2 = ptrout+firstnode[ilay-1];		   /* output vector */
	    ptrend2 = ptrtmp2 + nodeabove-1;		   /* error from node above */

	    for(; ptrtmp2<=ptrend2; ++ptrtmp2, ++ptrtmp3)  /* update each weight */
	      *ptrtmp3 += tmp**ptrtmp2;
	  }
	}
}

float *floatmem(int elements)

{
	float *ptr;

	ptr = (float *) calloc(sizeof(float),elements);
	if(ptr == (float *) 0)
	{
	  printf("Memory allocation failed \n");
	  exit(1);
	}
	return(ptr);
}

void netcon(float *ptrwt, int *ptrnode, int nlay)

/*
	float *ptrwt					pointer to wt vector
	int *ptrnode					pointer to node vector
	int nlay					number of layers in net
*/

{
	long random();

	typedef int LOOP;

	float *ptrtmp1=ptrwt;
	float normal, no2;	
	LOOP i,j,jmax,k,kmax;

	normal = pow(2,31)-1;				/* normalization factor */
	no2 = normal/2;
	normal = 1./(2.*normal);			/* for random # generator */

	for(i=1; i<=nlay-1; ++i)			/* loop over each layer */
	{
	  jmax = *(ptrnode+i);
	  kmax = *(ptrnode+i-1);
	  for(j=1; j<=jmax; ++j)			/* loop over each node */
	  {
	   for(k=1; k<=kmax; ++k,++ptrtmp1)		/* loop over all weights */
	   {
	     *ptrtmp1 = (float) (random()-no2)*normal;	/* initialize wt to small */
	    }						/* random # */
	  }
	}
}

void response(float *ptrwt, float *ptrinput, float *ptrout,
	      int *ptrnode, int nlay)      

/*
	ptrwt					pointer to wt vector. 
	ptrinput				pointer to input vector. 
	ptrout   				pointer to output vector.    
	ptrnode					pointer to node vector 
	
	nlay					number of layers in net 
				      
   Routine to compute the response of the net to an 
   input vector.  This routine assumes that the pointers
   are equal to a[0] of the array they point to.      
*/

{
	typedef int Loop;

	register float *ptrfinal;			
	register float *ptrtmp1, *ptrtmp2;
	int numnod,kfinal,ibias,ibias2;
	Loop i,j;
	register float temp;

	double exp(double);

	ptrtmp1 = ptrout;
	ptrfinal = ptrout + *ptrnode-1;
	ptrtmp2 = ptrinput;

	for(; ptrtmp1<=ptrfinal; ++ptrtmp1, ++ptrtmp2)	/* Store input vector as */
	  *ptrtmp1 = *ptrtmp2;				/* first row in the net */
		
	ptrtmp1 = ptrwt;				/* initialize tmp pointer */
	ibias = *ptrnode;
	ibias2 = 0;					/* first output node */

	for(i=1; i<= nlay-1; ++i)			/* Loop over each layer */
	{  						/* above the current */
	  numnod = *(ptrnode + i);			/* number of tlu's in layer i */
	  kfinal = *(ptrnode+i-1);			/* number of nodes in layer */

	  for(j=1; j<=numnod; ++j)			/* Loop over each tlu */
	  {
	    temp = 0.0;							
	    ptrtmp2 = ptrout + ibias2;			/* first output above current layer */
	    ptrfinal = ptrtmp2 + kfinal-1;

	    for( ; ptrtmp2 <= ptrfinal; ++ptrtmp1, ++ptrtmp2)	
	    {
	      temp += (*ptrtmp1)*(*ptrtmp2);		/* Form the dot product between the */
	     }						/* weight and output vectors */
	   *(ptrout+ibias) = 1./(1.+(float) exp((double) -temp));	/* Apply the sigmoid transform */
	   ++ibias;
	  }
	  ibias2 += kfinal;						
	}
	    
	
}

void topology(int *ptrnode, int *ptrnlay)

{
	int numlay,ilay,resp;

	printf("Number of layers ");
	scanf("%d",&numlay);
	if(numlay >= NLAYMAX)
	{
	  printf("Number of layers must be less than %d\n",NLAYMAX);
	  exit(1);
	}

	*ptrnlay = numlay;   

	for(ilay=0; ilay<=numlay-1; ++ilay)
	{
	  printf("Number of nodes in layer %d ",ilay+1);
	  scanf("%d",&resp);
	  *(ptrnode + ilay) = resp;  
	}
}
