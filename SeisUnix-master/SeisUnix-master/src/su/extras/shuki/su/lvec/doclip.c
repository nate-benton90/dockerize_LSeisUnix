doclip(clip,n,p)
float clip,*p;
{
	register nn;
	register float *pp,c,mc;
	pp = p;
	c = clip;
	mc = -clip;
	nn = n;
	do {
		if( *pp > c ) *pp = c;
		else if( *pp < mc ) *pp = mc;
		pp++;
	} while(--nn);
}
