h45555
s 00000/00000/00047
d D 1.2 88/11/15 14:04:11 shuki 2 1
c 
e
s 00047/00000/00000
d D 1.1 88/04/14 13:50:01 shuki 1 0
c date and time created 88/04/14 13:50:01 by shuki
e
u
U
f e 0
t
T
I 1
/*	this is a translation of program from SEP 10 p100. */
/*	quant reorders a[] that 
 *		a[j]<a[k] if j<k
 *	i.e. a[k-1] is the k/n quntile.
 */
quant(k,a,n)
int k,n; float *a;
{
	int low,hi,i,j;
	double ak,aa;
	low = 0;
	hi = n-1;
	while(low < hi)
	{
		ak = a[k];
		i = low;
		j = hi;
		do
		{
			while(a[i] < ak)
			{
				i++;
			}
			while(a[j] > ak)
			{
				j--;
			}
			if(i <= j)
			{
				aa = a[i];
				a[i] = a[j];
				a[j] = aa;
				i++;
				j--;
			}
		}
		while(i <= j);
		if(j < k)
		{
			low = i;
		}
		if(k < i)
		{
			hi = j;
		}
	}
}
E 1
