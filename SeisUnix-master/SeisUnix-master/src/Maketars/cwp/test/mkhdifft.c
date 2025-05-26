#include "cwp.h"

main()
{
	int l,nfft,i;
	float h,d[100],zamp[100];
	complex z[200];

	while(1) {
		printf("Enter h l:\n");
		scanf("%f %d",&h,&l);
		mkhdiff(h,l,d);
		pp1d(stdout,"half-derivative filter",2*l+1,-l,d);
		getchar();
		getchar();
		nfft = npfa(100);
		for (i=0; i<=2*l; i++)
			z[i] = cmplx(d[i],0.0);
		for (i=2*l+1; i<nfft; i++)
			z[i] = cmplx(0.0,0.0);
		pfacc(1,nfft,z);
		for (i=0; i<nfft/2+1; i++)
			zamp[i] = fcabs(z[i]);
		pp1d(stdout,"amplitude spectrum",nfft/2+1,0,zamp);
	}
}
