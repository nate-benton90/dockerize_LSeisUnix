#define N 16
main(ac,av)
char **av;
{
	int i,ii;
	float x[N+2];

	if(ac==1) ii = 1;
	else ii = atoi(av[ac-1]);

	for(i=0;i<N;i++)
		x[i] = 0.0;

	x[ii] = 1.0;

	printv("Input signal:",x,N);

	refft(x,N,1,2);

	printv("FFT:",x,N+2);
	printvc("FFT:",x,N/2);

	refft(x,N,-1,-2);

	printv("IFFT:",x,N);
}

printvc(s,x,n)
char *s;
float *x;
int n;
{
	int i;
	printf("%s\n",s);
	printf("Real, Imag)\n");
	for(i=0;i<=n;i++)
		printf("(%10f , %10f)\n",x[i*2],x[i*2+1]);
}

printv(s,x,n)
char *s;
float *x;
int n;
{
	int i;
	printf("%s\n",s);
	for(i=0;i<n;i++)
		printf("%10.6f%c",x[i],(i!=n-1)?',':'\n');
}
