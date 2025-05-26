/* test frannor */
#include "cwp.h"

#define NBINS 32
#define XMIN -3.0
#define XMAX  3.0

int inbin (float xmin, float xmax, int nbins, float data)
{
	int bin;
	
	if (data<xmin)
		bin = 0;
	else if (data>=xmax)
		bin = nbins-1;
	else
		bin = 1+(data-xmin)/(xmax-xmin)*(nbins-2);
	return bin;
}

main()
{
	int i,j,h[NBINS],nr;
	float f[NBINS];
	
	srannor(305);
	for (i=0; i<3; i++)
		printf("frannor() = %f\n",frannor());
	
	printf("Enter number of normals:  ");
	scanf("%d",&nr);
	for (i=0; i<NBINS; i++)
		h[i] = 0.0;

	srannor(305);
	for (i=0; i<nr; i++) {
		j = inbin(XMIN,XMAX,NBINS,frannor());
		h[j] += 1;
	}
	
	for (i=0; i<NBINS; i++)
		f[i] = h[i]/(float)nr;
	pplot1(stdout,"histogram",NBINS,f);
}
