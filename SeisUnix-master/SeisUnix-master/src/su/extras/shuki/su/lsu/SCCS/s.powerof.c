h24746
s 00000/00000/00015
d D 1.2 88/11/15 14:01:55 shuki 2 1
c 
e
s 00015/00000/00000
d D 1.1 88/04/14 13:47:51 shuki 1 0
c date and time created 88/04/14 13:47:51 by shuki
e
u
U
f e 0
t
T
I 1
float powerof(p,n)
float *p;
{
	register nn;
	register float *pp,ppower;

	nn = n;
	pp = p;
	ppower = 0.0;
	while(nn--) {
		ppower += (*pp) * (*pp);
		pp++;
	}
	return(ppower);
}
E 1
