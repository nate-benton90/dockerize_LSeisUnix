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
