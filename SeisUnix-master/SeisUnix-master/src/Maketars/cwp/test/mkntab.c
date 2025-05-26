#include "cwp.h"
main()
{
	int nmin,n,no,nfft,nffto;
	int mininterval = 1;
	int maxinterval = 720719;  /* 720720 gives infinite loop */
	float cpu,total;
	float secinterval = 15.0;
	complex *c=alloc1complex(720720);
	int npfao2 (int nmin, int nmax);
		
	for (n=0; n<720720; ++n)
		c[n].r = c[n].i = 0.0;

	for (nmin=npfa(mininterval); nmin<=maxinterval; nmin=npfa(nmin+1)) {
		n = npfa(nmin);
		for (nfft=0,total=0.0; total<secinterval; ++nfft) {
			cpu = cpusec();
			pfacc(1,n,c);
			total += cpusec()-cpu+FLT_EPSILON;
		}
		printf("{ %7d, %6f },\n", n, secinterval/nfft);
	}
}