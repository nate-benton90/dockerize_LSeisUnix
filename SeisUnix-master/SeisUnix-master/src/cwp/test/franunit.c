/* test franuni */

#include "cwp.h"

main()
{
	int i;
	float u;
	
	sranuni(305);
	for (i=0; i<1000; i++)
		u = franuni();
	printf("%0.6f should equal 0.157039\n",u);
	
	sranuni(305);
	for (i=0; i<1000; i++)
		u = franuni();
	printf("%0.6f should equal 0.157039\n",u);
}
