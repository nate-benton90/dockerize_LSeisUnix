/* Make spike traces to implement 2.2.26 (use MKS units) */
/* Note: nice to have (h2-h1)/c1 be an integer */
/* Better to make beta an array and directly fill in the non-zero entries */

#include <stdio.h>
#include <math.h>

#define NINT(x) ((int)((x)>0.0?(x)+0.5:(x)-0.5))
#define FSIZE sizeof(float)

main()
{
        int ns=512;	/* number of samples		*/
        int ntr=8;	/* number of traces		*/
	int n=5;	/* number of multiples		*/
	float c0=1.5;	/* surface speed 		*/
	float c1=2.0;	/* speed in layer 		*/
	float c2=2.2;	/* basement speed		*/
	int h1=100;	/* depth of first interface in samples	*/
	int h2=200;	/* depth of second interface in samples	*/
	float R1=(c1-c0)/(c1+c0); /* first reflection coefficient	*/
	float R2=(c2-c1)/(c2+c1); /* second reflection coefficient	*/
	
        { int k;
          for (k = 0; k < ntr; ++k) {
                int j;
                for (j = 0; j < ns; ++j) {
			int i;
			float beta = 0.0;
			if (j == h1)
				beta = R1;
			for (i = 1; i <= n+1; ++i) {
				if (j == NINT((h1 + i*(h2-h1)*c0/c1)))
					beta = R2*(1-R1*R1) *
						pow(-R1*R2,i-1);
			}
                        fwrite(&beta, FSIZE, 1, stdout);
                }
          }
	}
}
