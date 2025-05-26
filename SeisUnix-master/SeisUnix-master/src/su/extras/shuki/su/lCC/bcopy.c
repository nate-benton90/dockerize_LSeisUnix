bcopy(s1,s2,n)
char *s1,*s2;
int n;
{
	int i;
	for(i=0;i<n;i++)
		s2[i] = s1[i];
}
