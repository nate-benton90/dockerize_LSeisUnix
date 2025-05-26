h22022
s 00000/00000/00012
d D 1.2 88/11/15 14:04:06 shuki 2 1
c 
e
s 00012/00000/00000
d D 1.1 88/04/14 13:49:56 shuki 1 0
c date and time created 88/04/14 13:49:56 by shuki
e
u
U
f e 0
t
T
I 1
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
E 1
