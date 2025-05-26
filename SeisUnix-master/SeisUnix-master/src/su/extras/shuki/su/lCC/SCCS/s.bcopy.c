h17140
s 00000/00000/00008
d D 1.2 88/11/15 14:04:53 shuki 2 1
c 
e
s 00008/00000/00000
d D 1.1 88/04/14 14:00:58 shuki 1 0
c date and time created 88/04/14 14:00:58 by shuki
e
u
U
f e 0
t
T
I 1
bcopy(s1,s2,n)
char *s1,*s2;
int n;
{
	int i;
	for(i=0;i<n;i++)
		s2[i] = s1[i];
}
E 1
