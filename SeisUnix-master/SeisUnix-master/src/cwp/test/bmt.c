/* test big matrix routines */

#include "cwp.h"

#define vfunc(i1,i2) (((i1)+(i2)*10)%128)

void vfill(nbpe,dir,i1,i2,n,v)
int nbpe,dir,i1,i2,n;
char *v;
{
	int i,j;
	if (dir==1) {
		for (i=0; i<n; i++,i1++)
			for (j=0; j<nbpe; j++)
				*v++ = vfunc(i1,i2);
	} else {
		for (i=0; i<n; i++,i2++)
			for (j=0; j<nbpe; j++)
				*v++ = vfunc(i1,i2);
	}
}

void vcheck(nbpe,dir,i1,i2,n,v)
int nbpe,dir,i1,i2,n;
char *v;
{
	int i,j;
	if (dir==1) {
		for (i=0; i<n; i++,i1++)
			for (j=0; j<nbpe; j++)
				if (*v++ != vfunc(i1,i2)) {
					printf("Error!\n");
					exit(-1);
				}

	} else {
		for (i=0; i<n; i++,i2++)
			for (j=0; j<nbpe; j++)
				if (*v++ != vfunc(i1,i2)) {
					printf("Error!\n");
					exit(-1);
				}
	}
}

#define NBPEMAX 8
#define NMAX 100
#define NDIM NBPEMAX*NMAX
char v[NDIM];

main()
{
	int seed,itest,ntest,nbpe,n1,n2,dir,i1,i2,iacc,nacc,acc,n;
	bmstate *state;

	/* get seed and other parameters */
	printf("Enter seed\n");
	scanf("%d",&seed);
	printf("Enter number of tests\n");
	scanf("%d",&ntest);
	printf("Enter number of accesses per test\n");
	scanf("%d",&nacc);
	
	/* seed random number generator */
	sranuni(seed);

	/* for all tests */
	for (itest=0; itest<ntest; itest++) {

		/* init state */
		nbpe = 1+NBPEMAX*franuni();
		n1 = 1+NMAX*franuni();
		n2 = 1+NMAX*franuni();
		state = (bmstate *)bmalloc(nbpe,n1,n2);
		printf("nbpe = %d\n",nbpe);
		printf("n1 = %d\n",n1);
		printf("n2 = %d\n",n2);
		(void) bmdump(state);
		/*
		*/

		/* write complete matrix (one way or the other) */
		dir = 1+2*franuni();
		/*
		printf("initial write in direction %d\n",dir);
		*/
		if (dir==1) {
			for (i2=0; i2<n2; i2++) {
				vfill(nbpe,dir,0,i2,n1,v);
				bmwrite(state,dir,0,i2,n1,v);
			}
		} else {
			for (i1=0; i1<n1; i1++) {
				vfill(nbpe,dir,i1,0,n2,v);
				bmwrite(state,dir,i1,0,n2,v);
			}
		}

		/* for all accesses */
		for (iacc=0; iacc<nacc; iacc++) {
			dir = 1+2*franuni();
			i1 = n1*franuni();
			i2 = n2*franuni();
			if (dir==1)
				n = 1+(n1-i1)*franuni();
			else
				n = 1+(n2-i2)*franuni();
			acc = 2*franuni();
			if (acc==0) {
/*
				printf("read %d: dir=%d i1=%d i2=%d n=%d\n",
					iacc,dir,i1,i2,n);
*/
				bmread(state,dir,i1,i2,n,v);
				vcheck(nbpe,dir,i1,i2,n,v);
			} else {
/*
				printf("write %d: dir=%d i1=%d i2=%d n=%d\n",
					iacc,dir,i1,i2,n);
*/
				vfill(nbpe,dir,i1,i2,n,v);
				bmwrite(state,dir,i1,i2,n,v);
			}
		}

		/* free state */
		bmfree(state);
	}
}
