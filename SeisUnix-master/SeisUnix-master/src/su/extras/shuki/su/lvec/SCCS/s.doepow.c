h34227
s 00000/00000/00018
d D 1.2 88/11/15 14:04:03 shuki 2 1
c 
e
s 00018/00000/00000
d D 1.1 88/04/14 13:49:53 shuki 1 0
c date and time created 88/04/14 13:49:53 by shuki
e
u
U
f e 0
t
T
I 1
doepow(epow,n,p)
float epow,*p;
{
	int j;
	static int first=1;
	static float *e;
	register float *q1,*q2,*q3;

	if(epow==0.0) return;
	if(first) {
		e = (float*) malloc(n*sizeof(float));
		for(j=0;j<n;j++)
			e[j] = exp(epow*(float)j/n);
		first=0;
	}
	for(q1=p,q2=e,q3=e+n;q2<q3;q1++,q2++)
		*q1 *= *q2;
}
E 1
