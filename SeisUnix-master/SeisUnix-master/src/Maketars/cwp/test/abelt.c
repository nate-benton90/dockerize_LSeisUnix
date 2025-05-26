/* compute Abel transform of a cone function (Bracewell, p. 264) */
#include "cwp.h"
#define N 100
main()
{
	int i,n=N;
	float f[N],g[N],e[N],a,r,k,dr,dk;
	void *at;
		
	a = 1.0;
	dr = dk = 1.0/n;
	
	for (i=0,r=0.0; i<n; ++i,r+=dr) {
		f[i] = a-r;
	}		
	at = abelalloc(n);
	abel(at,f,g);
	for (i=0,k=0.0; i<n; ++i,k+=dk) {
		g[i] *= dr;
		e[i] = g[i]-(k!=0.0 ? a*sqrt(a*a-k*k)-k*k*acosh(a/k) : a*a);
	}
	abelfree(at);
	pplot1(stdout,"Abel transform error",n,e);
}

