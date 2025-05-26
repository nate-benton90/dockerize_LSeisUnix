flush(p,n,x)
float *p,x; int n;
{
	register float *pp,*pe;
	for(pp=p,pe=p+n;pp<pe;pp++)
		*pp = x;
}
