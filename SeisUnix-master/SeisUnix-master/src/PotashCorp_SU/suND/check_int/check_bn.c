#include "su.h"
#include "suhdr.h"
#include <openssl/bn.h>




int
main()
{

	int in=3000,i;

        BIGNUM *bita;
	
	bita=BN_new();
	BN_zero(bita);
	
	BN_set_bit(bita,1576);
	
	for(i=0;i<in;i++) 
		if(BN_is_bit_set(bita,i)) 
			fprintf(stderr," %d\n",i);

	return(0);
}
