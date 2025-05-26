bzero(s,n)
char *s;
int n;
{
	while(n--)
		*s++ = (char)0;
}
