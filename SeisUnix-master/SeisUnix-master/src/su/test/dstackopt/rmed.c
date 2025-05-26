#include <stdio.h>
rmed(f,nt,l,lh,w,buf)
float *f,*w,*buf; int nt,l,lh;
{
	int i,j;
	float median();

	fprintf(stderr,"rmed: ");
	copy(f,buf,1,nt);
	copy(buf,w,l);
	bubbles(w,l);			/* start from a sorted window */
	f[lh] = w[lh];		/* fist point to be covered */
	for(i=lh+1;i<nt-lh;i++) {
		for(j=0;j<l;j++)	/* the new term */
			if(w[j] == buf[i-lh-1]) {
				w[j] = buf[i+lh];
				break;
				}
		bubble(w,l);
		f[i] = w[lh];	/* overewrite the input */
		}
	fprintf(stderr,"done\n");
}

bubbles(f,nt)
float *f; int nt;
{
	int i,swaps;
	while(1) {
		for(i=1;i<nt;i++) {		/* the bubble floats */
			if(f[i-1]>f[i]) {
				swap(f+i,f+i-1);
				}
			}
		for(i=nt-2,swaps=0;i>0;i--) {	/* the weight drawns */
			if(f[i-1]>f[i]) {
				swap(f+i,f+i-1);
				swaps++;
				}
			}
		if(swaps == 0) return(0);
		}
}

bubble(f,nt)
float *f; int nt;
{
	int i,start=0;
	for(i=1;i<nt;i++) {		/* loop upwards */
		if(f[i-1]>f[i]) {
			swap(f+i,f+i-1);/* the bubble floats */
			start = 1;	/* I have done something */
			}
		else if(start) return(0);/* return if it is all over */
		}
	for(i=nt-2;i>0;i--) { 		/* do the same in the other way */
		if(f[i-1]>f[i]) {
			swap(f+i,f+i-1);
			start = 1;
			}
		else if(start) return(0);
		}
	return(1);
}

swap(p,q)
float *p,*q;
{
	register float temp;
	temp = *p;
	*p = *q;
	*q = temp;
}
