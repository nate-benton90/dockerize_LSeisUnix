/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.                       */

#include "cwp.h"


/*********************** self documentation **********************/
/* compute solution for x in Ax=b using conjugate gradients */


/*********************** end self documentation **********************/
#include "cwp.h"

/* compute solution for x in Ax=b using conjugate gradients */
float testvector[5] = {3.00, 3.00, 5.00, 7.00, 9.00} ;
float testmatrix[5][4] = {	{1.00, 1.00, 1.00, 0.00},
				{1.00, 2.00, 0.00, 0.00},
				{1.00, 3.00, 1.00, 0.00},
				{1.00, 4.00, 0.00, 1.00},
				{1.00, 5.00, 1.00, 1.00} };
int
main()
{
	int i,j;		/* counters */
	int m=5;		/* size of b vector */
	int n=4;		/* size of x vector */
	int niter=10;		/* do 4 iterations */
	float *x=NULL;		/* unknown vector being solved for in Ax=b */
	float *b=NULL;		/* vector b in  Ax =b  */
	float **a=NULL;		/* matrix A in  Ax =b  */

	/* allocate space */
	x=alloc1float(n); 	
	b=alloc1float(m);
	a=alloc2float(n,m);

	/* read data into arrays */
	for (j=0; j<m; ++j) {
		b[j] = testvector[j];
		for (i=0; i<n; ++i) {
			a[j][i] = testmatrix[j][i];
		}
	}

	/* call conjugate gradient solver to find x in  Ax = b */
	simple_conj_gradient(n, x, m, b, a, niter);
	
	/* print out the results */
	printf("\n");
	printf("Solving Ax=b by the conjugate gradient method \n");
	printf("\n");
	printf("The 5x4  matrix A\n");
	for (j=0; j<m; ++j) {
		for (i=0; i<n; ++i){
			printf("%f ", a[j][i] );
		}
		printf("\n");
	}
	printf("\n");

	printf("The vector b\n");
	for (j=0; j<m; ++j) {
			printf("%f ", b[j] );
	}

	printf("\n");
	printf("\n");

	
	printf("The output vector x \n");
	for (i=0; i<n; ++i) {
			printf("%f ", x[i] );
	}
	printf("\n");

}

