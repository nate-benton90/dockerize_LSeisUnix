#include "cwp.h"

main()
{
	int i1,i2,i3,n1=3,n2=4,n3=2;
	int ***a;

	a = alloc3int(n1,n2,n3);
	printf("a = %d\n",a);
	printf("a[0] = %d\n",a[0]);
	printf("a[0][0] = %d\n",a[0][0]);
	for (i3=0; i3<n3; i3++)
		for (i2=0; i2<n2; i2++)
			for (i1=0; i1<n1; i1++)
				a[i3][i2][i1] = i1+i2*n1+i3*n1*n2;
	printf("a filled\n");
	for (i3=0; i3<n3; i3++)
		for (i2=0; i2<n2; i2++)
			for (i1=0; i1<n1; i1++)
				printf("a[%d][%d][%d] = %d\n",
					i3,i2,i1,a[i3][i2][i1]);
	free3int(a);
}
