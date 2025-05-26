#include <stdio.h>
settp()
{
	extern ntpp1,ntp;
	extern float *tp;

	int i;
	for(i=0;i<ntp;i++)
		tp[i] = (1.+i)/ntpp1;
}

tap()
{
	extern ntpp1,ntpm1,ntp,nt,ntr;
	extern float *tp,*data;		/* <- datap ? */
	int i,j;
	float *p;

	fprintf(stderr,"tap: ");
fprintf(stderr,"nt=%d ntr=%d\n",nt,ntr);
	for(i=0;i<ntr;i++) {
		p = data + i*nt;
fprintf(stderr,"i=%d data=%x p=%x\t",i,data,p);
		for(j=0;j<ntp;j++)
			p[j] *= tp[j];
		p += nt-ntp-1;
fprintf(stderr,"p=%x\n",p);
		for(j=0;j<ntp;j++)
			p[j] *= tp[ntpm1-j];
	}
	fprintf(stderr,"done\n");
}
