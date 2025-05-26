copy(p,q,n)
char *p,*q;
{
	register char *pp,*qq;
	pp = p; qq = q;
	while(n--) *(qq++) = *(pp++);
}
