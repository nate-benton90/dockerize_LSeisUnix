#include <stdlib.h>
#include "suhdr.h"


void qsort_lu(unsigned long *a, unsigned int n)
/* Sort an unsigned long array using the OSes qsort routine */
{

	qsort(&a[0],n,sizeof(unsigned long),LU_cmp);
}


void qsort_u(unsigned int *a, unsigned int n)
/* Sort an unsigned int array using the OSes qsort routine */
{

	qsort(&a[0],n,sizeof(unsigned long),U_cmp);
}

void qsort_i(int *a, unsigned int n)
/* Sort an int array using the OSes qsort routine */
{

	qsort(&a[0],n,sizeof(int),I_cmp);
}

void qsort_f(float *a, unsigned int n)
/* Sort a float array using the OSes qsort routine */
{

	qsort(&a[0],n,sizeof(float),F_cmp);
}

void qsort_d(int *a, unsigned int n)
/* Sort a double array using the OSes qsort routine */
{

	qsort(&a[0],n,sizeof(double),D_cmp);
}

void unqsort(void *a, size_t n,size_t size,int(*compar)(const void *, const void *),size_t *un)
/* Unique sort of an array
   only outputs the first element of an equal sub-sequence
   the unique elements are returned as the first un elmenst of array a
*/
{ 
       unsigned long ip;
        
        if(n==1) {
                *un=1;
                return;
        }
        /* do a quick sort */
        qsort(a,n,size,compar);

        
        /* check for equal sub-sequence */
        ip=1;
	*un=(size_t)n;
        {	char *ap=a+size;
        	char *bp=a;
        	while(ip<n) {   
			if(!compar(bp,ap)) {
 				
				while(compar(bp,ap)==0 && ip!=n) {
					ap+=size;
					ip++;
					*un-=1;
              			}
				
				memcpy((void *) bp+size,(const void*)ap,(n-ip+1)*size);
			}
			bp+=size;
			ap=bp+size;
                	ip++;
        	}
	}
}

void uqsort_ul(unsigned long *a, unsigned int n,unsigned int *un)
/* Unique sort of an array
   only outputs the first element of an equal sub-sequence
   the unique elements are returned as the first un elmenst of array a
*/
{
        unsigned int i,in;
        
        if(n==1) {
                *un=1;
                return;
        }
        /* do a quick sort */
        qsort(&a[0],n,sizeof(unsigned long),LU_cmp);

        
        /* check for equal sub-sequence */
        i=1;
        in=1;
        while(in<n) {   
                while(a[in]==a[i-1] && in!=n) {
                        in++;
                }
                a[i]=a[in];
                i++;
        }
        *un=i-1;
}

void uqsort_f(float *a, unsigned int n,unsigned int *un)
/* Unique sort of an array
   only outputs the first element of an equal sub-sequence
   the unique elements are returned as the first un elmenst of array a
*/
{
        unsigned int i,in;
        
        if(n==1) {
                *un=1;
                return;
        }
        /* do a quick sort */
        qsort(&a[0],n,sizeof(float),F_cmp);

        
        /* check for equal sub-sequence */
        i=1;
        in=1;
        while(in<n) {   
                while(a[in]==a[i-1] && in!=n) {
                        in++;
                }
                a[i]=a[in];
                i++;
        }
        *un=i-1;
}
