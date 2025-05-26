#include "cwp.h"

#define NSINC 11
#define LSINC 8

main()
{
	int isinc;
	float frac,sinc[LSINC];

	for (isinc=0,frac=0.0; isinc<NSINC; isinc++,frac+=1.0/(NSINC-1)) {
		mksinc(frac,LSINC,sinc);
		pp1d(stdout,"",LSINC,-3,sinc);
	}
}
