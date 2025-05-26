#include "cwp.h"

main()
{
	float x,y;

	while(1) {
		printf("Enter x y:\n");
		scanf("%f %f",&x,&y);
		printf("NINT(x) = %d\n",NINT(x));
		printf("NINT(y) = %d\n",NINT(y));
		printf("NINT(x+y) = %d\n",NINT(x+y));
		printf("MIN(x,y) = %f\n",MIN(x,y));
		printf("MAX(x,y) = %f\n",MAX(x,y));
	}
}
