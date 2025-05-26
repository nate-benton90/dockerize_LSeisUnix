h25901
s 00000/00000/00015
d D 1.2 88/11/15 14:04:02 shuki 2 1
c 
e
s 00015/00000/00000
d D 1.1 88/04/14 13:49:52 shuki 1 0
c date and time created 88/04/14 13:49:52 by shuki
e
u
U
f e 0
t
T
I 1
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
E 1
