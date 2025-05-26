#include <stdio.h>
#include "suhdr.h"


char *ealloc1bit(unsigned int n)
{
	return((char *) ealloc1(((n-1)>>3)+1,sizeof(char)));
}

void free1bit(char *a)
{
	free1(a);
}

void zerobit(char *a,unsigned int n)
{
	memset( (void *) a, (int) '\0',(((n-1)>>3)+1)*sizeof(char));
}

void setbit(char *a,unsigned int n)
{
	char *bs;
		
	bs=a+(n>>3);
	*bs=(*bs | 1 << (n%8));
}

void clearbit(char *a,unsigned int n)
{
	char *bs;
		
	bs=a+(n>>3);
	*bs=(*bs & (255 ^ ( 1 << (n%8) )));
}


unsigned int readbit(char *a,unsigned int n)
{
	char *bs;
	char v,m;
		
	bs=a+(n>>3);
	m = 1&(*bs>> n%8);
	return((unsigned int)m);
}

void ORbit(char *a,char *b ,char *c,unsigned int n)
{
	unsigned int i;
	size_t s;
	char r;
	char *ap,*bp,*cp;
	unsigned int ni=(((n-1)>>3)+1);
		
	for(i=0;i<ni;i++) {
		s=sizeof(char)*i;
		ap=a+s;
		bp=b+s;
		cp=c+s;
		r=(*ap|*bp);
		*cp=r;
	}
}

void ANDbit(char *a,char *b ,char *c,unsigned int n)
{
	unsigned int i;
	size_t s;
	char r;
	char *ap,*bp,*cp;
	unsigned int ni=(((n-1)>>3)+1);
		
	for(i=0;i<ni;i++) {
		s=sizeof(char)*i;
		ap=a+s;
		bp=b+s;
		cp=c+s;
		r=(*ap&*bp);
		*cp=r;
	}
}
