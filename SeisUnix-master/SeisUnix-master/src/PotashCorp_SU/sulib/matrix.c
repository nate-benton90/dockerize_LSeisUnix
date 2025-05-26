#include<stdio.h>
#include <stdlib.h>
#include<math.h>
#include "su.h"


/**  Allocation of vector storage  ***********************************/

float *vector(n)
int n;
/* Allocates a float vector with range [1..n]. */
{

    float *v;

    v = (float *) malloc ((unsigned) n*sizeof(float));
    if (!v) err("Allocation failure in vector().");
    return v-1;

}

/**  Allocation of float matrix storage  *****************************/

float **matrix(n,m)
int n, m;
/* Allocate a float matrix with range [1..n][1..m]. */
{
    int i;
    float **mat;

    /* Allocate pointers to rows. */
    mat = (float **) malloc((unsigned) (n)*sizeof(float*));
    if (!mat) err("Allocation failure 1 in matrix().");
    mat -= 1;

    /* Allocate rows and set pointers to them. */
    for (i = 1; i <= n; i++)
        {
        mat[i] = (float *) malloc((unsigned) (m)*sizeof(float));
        if (!mat[i]) err("Allocation failure 2 in matrix().");
        mat[i] -= 1;
        }

     /* Return pointer to array of pointers to rows. */
     return mat;

}

/**  Deallocate vector storage  *********************************/

void free_vector(v,n)
float *v;
int n;
/* Free a float vector allocated by vector(). */
{
   free((char*) (v+1));
}

/**  Deallocate float matrix storage  ***************************/

void free_matrix(mat,n,m)
float **mat;
int n, m;
/* Free a float matrix allocated by matrix(). */
{
   int i;

   for (i = n; i >= 1; i--)
       {
       free ((char*) (mat[i]+1));
       }
   free ((char*) (mat+1));
}


void transpose_2d(void **mat,int n2,int n1,void **matt,size_t nbpe)

/* mat aligned by mat[n2][n1]
   matt           mat[n1][n2]
*/

{
        int i1,i2;
        void *state;        /* allocate big matrix state */
        
	state = bmalloc(nbpe,n1,n2);
                                                                                                      
        /* put vectors along 1st dimension to big matrix */
        for (i2=0; i2<n2; i2++) {
                bmwrite(state,1,0,i2,n1,mat[i2]);
        }
                                                                                                      
        for (i1=0; i1<n1; i1++) {
                bmread(state,2,i1,0,n2,matt[i1]);
        }
                                                                                                      
        /* free big matrix state */
        bmfree(state);
}



