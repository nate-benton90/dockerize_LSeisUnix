doscale(scale,n,p)
float scale,*p;
{
	register nn;
	register float *pp,t;
	pp = p;
	t = scale;
	nn = n;
	do {
		*(pp++) *= t;
	} while(--nn);
}
