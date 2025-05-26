#include "cwp.h"

main()
{
	int nmin;
	do {
		printf("Enter nmin\n");
		scanf("%d",&nmin);
		printf("npfa = %d\n",npfa(nmin));
	} while (nmin>0);
}
