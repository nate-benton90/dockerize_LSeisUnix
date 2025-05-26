/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

main()
{
	int nmin,nmax;
	do {
		printf("Enter nmin nmax\n");
		scanf("%d %d",&nmin,&nmax);
		printf("npfa = %d  npfao = %d\n",npfa(nmin),npfao(nmin,nmax));
	} while (nmin>0);
}
