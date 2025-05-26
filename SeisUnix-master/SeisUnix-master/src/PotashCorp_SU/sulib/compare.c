#include "stdlib.h"

int LU_cmp(const void *a, const void *b)
{
	unsigned long i1 = *((unsigned long*) a) ;
	unsigned long i2 = *((unsigned long*) b) ;
	
	if(i1<i2) return(-1);
	if(i1>i2) return(1);
	return(0);
}

int U_cmp(const void *a, const void *b)
{
	unsigned int i1 = *((unsigned int*) a) ;
	unsigned int i2 = *((unsigned int*) b) ;
	
	if(i1<i2) return(-1);
	if(i1>i2) return(1);
	return(0);
}

int I_cmp(const void *a, const void *b)
{
	int i1 = *((int*) a) ;
	int i2 = *((int*) b) ;
	
	if(i1<i2) return(-1);
	if(i1>i2) return(1);
	return(0);
}

int F_cmp(const void *a, const void *b)
{
	float i1 = *((float*) a) ;
	float i2 = *((float*) b) ;
	
	if(i1<i2) return(-1);
	if(i1>i2) return(1);
	return(0);
}

int D_cmp(const void *a, const void *b)
{
	double i1 = *((double*) a) ;
	double i2 = *((double*) b) ;
	
	if(i1<i2) return(-1);
	if(i1>i2) return(1);
	return(0);
}
