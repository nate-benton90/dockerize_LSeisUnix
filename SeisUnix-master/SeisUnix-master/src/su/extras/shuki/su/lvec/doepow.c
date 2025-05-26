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
